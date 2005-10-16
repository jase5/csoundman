/*
    csmodule.c:

    Copyright (C) 2005 Istvan Varga
    based on dl_opcodes.c, Copyright (C) 2002 John ffitch

    This file is part of Csound.

    The Csound Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Csound is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Csound; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

/******************************************************************************
 * NEW PLUGIN INTERFACE                                                       *
 * ====================                                                       *
 *                                                                            *
 * Plugin libraries are loaded from the directory defined by the environment  *
 * variable OPCODEDIR (or the current directory if OPCODEDIR is unset) by     *
 * csoundPreCompile() while initialising a Csound instance, and are unloaded  *
 * at the end of performance by csoundReset().                                *
 * A library may export any of the following five interface functions,        *
 * however, the presence of csoundModuleCreate() is required for identifying  *
 * the file as a Csound plugin module.                                        *
 *                                                                            *
 * int csoundModuleCreate(CSOUND *csound)       (required)                    *
 * --------------------------------------                                     *
 *                                                                            *
 * Pre-initialisation function, called by csoundPreCompile().                 *
 *                                                                            *
 * int csoundModuleInit(CSOUND *csound)         (optional)                    *
 * ------------------------------------                                       *
 *                                                                            *
 * Called by Csound instances before orchestra translation. One possible use  *
 * of csoundModuleInit() is adding new opcodes with csoundAppendOpcode().     *
 *                                                                            *
 * int csoundModuleDestroy(CSOUND *csound)      (optional)                    *
 * ---------------------------------------                                    *
 *                                                                            *
 * Destructor function for Csound instance 'csound', called at the end of     *
 * performance, after closing audio output.                                   *
 *                                                                            *
 * const char *csoundModuleErrorCodeToString(int errcode)   (optional)        *
 * ------------------------------------------------------                     *
 *                                                                            *
 * Converts error codes returned by any of the initialisation or destructor   *
 * functions to a string message.                                             *
 *                                                                            *
 * int csoundModuleInfo(void)                   (optional)                    *
 * --------------------------                                                 *
 *                                                                            *
 * Returns information that can be used to determine if the plugin was built  *
 * for a compatible version of libcsound. The return value may be the sum of  *
 * any of the following two values:                                           *
 *                                                                            *
 *   ((CS_APIVERSION << 16) + (CS_APISUBVER << 8))      API version           *
 *   (int) sizeof(MYFLT)                                MYFLT type            *
 *                                                                            *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>

#include "csoundCore.h"
#include "csmodule.h"

#if defined(HAVE_LIBDL) || defined(LINUX) || defined(__CYGWIN__)
#include <dlfcn.h>
#elif defined(WIN32)
#include <windows.h>
#endif

#if defined(HAVE_DIRENT_H)
#  include <dirent.h>
#  ifdef __MACH__
#    ifdef DIRENT_FIX
typedef void*   DIR;
DIR             opendir(const char *);
struct dirent   *readdir(DIR*);
int             closedir(DIR*);
#    endif
#  endif
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
#  include <io.h>
#  include <direct.h>
#elif defined(__MACH__)
#  define ERR_STR_LEN 255
#  include <mach-o/dyld.h>
#endif

extern  int     allocgen(CSOUND *, char *, int (*)(FGDATA *, FUNC *));

/* module interface function names */

static  const   char    *opcode_init_Name =   "csound_opcode_init";
static  const   char    *fgen_init_Name =     "csound_fgen_init";

static  const   char    *PreInitFunc_Name =   "csoundModuleCreate";
static  const   char    *InitFunc_Name =      "csoundModuleInit";
static  const   char    *DestFunc_Name =      "csoundModuleDestroy";
static  const   char    *ErrCodeToStr_Name =  "csoundModuleErrorCodeToString";

static  const   char    *InfoFunc_Name =      "csoundModuleInfo";

/* environment variable storing path to plugin libraries */
static  const   char    *plugindir_envvar =   "OPCODEDIR";
static  const   char    *plugindir64_envvar = "OPCODEDIR64";

/* default directory to load plugins from if environment variable is not set */
static  const   char    *default_plugin_dir = ".";

typedef struct opcodeLibFunc_s {
    long    (*opcode_init)(CSOUND *, OENTRY **);  /* list of opcode entries  */
    NGFENS  *(*fgen_init)(CSOUND *);        /* list of named GEN routines    */
    void    (*dummy)(void);                 /* unused                        */
} opcodeLibFunc_t;

typedef struct pluginLibFunc_s {
    int         (*InitFunc)(CSOUND *);      /* initialisation routine        */
    int         (*DestFunc)(CSOUND *);      /* destructor routine            */
    const char  *(*ErrCodeToStr)(int);      /* convert error code to string  */
} pluginLibFunc_t;

typedef struct csoundModule_s {
    struct csoundModule_s *nxt;             /* pointer to next link in chain */
    void        *h;                         /* library handle                */
    int         (*PreInitFunc)(CSOUND *);   /* pre-initialisation routine    */
                                            /*   (always NULL if opcode lib) */
    union {
      pluginLibFunc_t   p;                  /* generic plugin interface      */
      opcodeLibFunc_t   o;                  /* opcode library interface      */
    } fn;
    char        name[1];                    /* name of the module            */
} csoundModule_t;

static CS_NOINLINE void print_module_error(CSOUND *csound,
                                           const char *fmt, const char *fname,
                                           const csoundModule_t *m, int err)
{
    csound->MessageS(csound, CSOUNDMSG_ERROR, Str(fmt), fname);
    if (m != NULL && m->fn.p.ErrCodeToStr != NULL)
      csound->MessageS(csound, CSOUNDMSG_ERROR,
                       ": %s\n", Str(m->fn.p.ErrCodeToStr(err)));
    else
      csound->MessageS(csound, CSOUNDMSG_ERROR, "\n");
}

static int check_plugin_compatibility(CSOUND *csound, const char *fname, int n)
{
    int     myfltSize, minorVersion, majorVersion;

    myfltSize = n & 0xFF;
    if (myfltSize != 0 && myfltSize != (int) sizeof(MYFLT)) {
      csound->Warning(csound, Str("not loading '%s' (uses incompatible "
                                  "floating point type)"), fname);
      return -1;
    }
    if (n & (~0xFF)) {
      minorVersion = (n & 0xFF00) >> 8;
      majorVersion = (n & (~0xFFFF)) >> 16;
      if (majorVersion != (int) CS_APIVERSION ||
          minorVersion > (int) CS_APISUBVER) {
        csound->Warning(csound, Str("not loading '%s' (incompatible "
                                    "with this version of Csound)"), fname);
        return -1;
      }
    }
    return 0;
}

/* load a single plugin library, and run csoundModuleCreate() if present */
/* returns zero on success */

static CS_NOINLINE int csoundLoadExternal(CSOUND *csound,
                                          const char *libraryPath)
{
    csoundModule_t  m;
    volatile jmp_buf tmpExitJmp;
    csoundModule_t  *mp;
    char            *fname;
    void            *h, *p;
    int             (*infoFunc)(void);
    int             err;

    /* check for a valid name */
    if (libraryPath == NULL || libraryPath[0] == '\0')
      return CSOUND_ERROR;
    /* remove leading directory components from name */
    fname = (char*) libraryPath + (int) strlen(libraryPath);
    for ( ; fname[0] != DIRSEP && fname != (char*) libraryPath; fname--)
      ;
    if (fname[0] == DIRSEP)
      fname++;
    if (fname[0] == '\0')
      return CSOUND_ERROR;
    /* load library */
    err = csound->OpenLibrary(&h, libraryPath);
    if (err) {
#if defined(LINUX) || defined(__CYGWIN__)
#  if defined(_DEBUG) || defined(DEBUG)
      csound->Warning(csound, "%s", dlerror());
#  endif
#endif
      csound->Warning(csound, Str("could not open library '%s' (%d)"),
                              libraryPath, err);
      return CSOUND_ERROR;
    }
    /* check if the library is compatible with this version of Csound */
    infoFunc = (int (*)(void)) csoundGetLibrarySymbol(h, InfoFunc_Name);
    if (infoFunc != NULL) {
      if (check_plugin_compatibility(csound, fname, infoFunc()) != 0) {
        csoundCloseLibrary(h);
        return CSOUND_ERROR;
      }
    }
    /* was this plugin already loaded ? */
    for (mp = (csoundModule_t*) csound->csmodule_db; mp != NULL; mp = mp->nxt) {
      if (mp->h == h) {
        csoundCloseLibrary(h);
        return CSOUND_SUCCESS;
      }
    }
    /* find out if it is a Csound plugin */
    memset(&m, 0, sizeof(csoundModule_t));
    m.h = h;
    m.PreInitFunc =
        (int (*)(CSOUND *)) csoundGetLibrarySymbol(h, PreInitFunc_Name);
    if (m.PreInitFunc != NULL) {
      /* generic plugin library */
      m.fn.p.InitFunc =
          (int (*)(CSOUND *)) csoundGetLibrarySymbol(h, InitFunc_Name);
      m.fn.p.DestFunc =
          (int (*)(CSOUND *)) csoundGetLibrarySymbol(h, DestFunc_Name);
      m.fn.p.ErrCodeToStr =
          (const char *(*)(int)) csoundGetLibrarySymbol(h, ErrCodeToStr_Name);
    }
    else {
      /* opcode library */
      m.fn.o.opcode_init =
          (long (*)(CSOUND *, OENTRY **))
              csoundGetLibrarySymbol(h, opcode_init_Name);
      m.fn.o.fgen_init =
          (NGFENS *(*)(CSOUND *)) csoundGetLibrarySymbol(h, fgen_init_Name);
      if (m.fn.o.opcode_init == NULL && m.fn.o.fgen_init == NULL) {
        /* must have csound_opcode_init() or csound_fgen_init() */
        csoundCloseLibrary(h);
        csound->Warning(csound, Str("'%s' is not a Csound plugin library"),
                                libraryPath);
        return CSOUND_ERROR;
      }
    }
    /* set up module info structure */
    /* (note: space for NUL character is already included in size of struct) */
    p = (void*) malloc(sizeof(csoundModule_t) + (size_t) strlen(fname));
    if (p == NULL) {
      csoundCloseLibrary(h);
      csound->ErrorMsg(csound,
                       Str("csoundLoadExternal(): memory allocation failure"));
      return CSOUND_MEMORY;
    }
    mp = (csoundModule_t*) p;
    memcpy(mp, &m, sizeof(csoundModule_t));
    strcpy(&(mp->name[0]), fname);
    /* link into database */
    mp->nxt = (csoundModule_t*) csound->csmodule_db;
    csound->csmodule_db = (void*) mp;
    /* call csoundModuleCreate() if available */
    if (m.PreInitFunc != NULL) {
      memcpy((void*) &tmpExitJmp, (void*) &csound->exitjmp, sizeof(jmp_buf));
      if ((err = setjmp(csound->exitjmp)) != 0) {
        memcpy((void*) &csound->exitjmp, (void*) &tmpExitJmp, sizeof(jmp_buf));
        print_module_error(csound, "Error in pre-initialisation function "
                                   "of module '%s'", fname, NULL, 0);
        return (err == (CSOUND_EXITJMP_SUCCESS + CSOUND_MEMORY) ?
                CSOUND_MEMORY : CSOUND_INITIALIZATION);
      }
      err = m.PreInitFunc(csound);
      memcpy((void*) &csound->exitjmp, (void*) &tmpExitJmp, sizeof(jmp_buf));
      if (err != 0) {
        print_module_error(csound, "Error in pre-initialisation function "
                                   "of module '%s'", fname, &m, err);
        return CSOUND_INITIALIZATION;
      }
    }
    /* plugin was loaded successfully */
    return CSOUND_SUCCESS;
}

/**
 * Load plugin libraries for Csound instance 'csound', and call
 * pre-initialisation functions.
 * Return value is CSOUND_SUCCESS if there was no error, CSOUND_ERROR if
 * some modules could not be loaded or initialised, and CSOUND_MEMORY
 * if a memory allocation failure has occured.
 */
int csoundLoadModules(CSOUND *csound)
{
#ifdef HAVE_DIRENT_H
    DIR             *dir;
    struct dirent   *f;
    const char      *dname, *fname;
    char            buf[1024];
    int             i, n, len, err = CSOUND_SUCCESS;

    if (csound->csmodule_db != NULL)
      return CSOUND_ERROR;

    /* open plugin directory */
    dname = csoundGetEnv(csound, (sizeof(MYFLT) == sizeof(float) ?
                                  plugindir_envvar : plugindir64_envvar));
    if (dname == NULL) {
      csound->opcodedirWasOK = 0;
#ifdef USE_DOUBLE
      dname = csoundGetEnv(csound, plugindir_envvar);
      if (dname == NULL)
#endif
        dname = default_plugin_dir;
    }
    dir = opendir(dname);
    if (dir == (DIR*) NULL) {
      csound->ErrorMsg(csound, Str("Error opening plugin directory '%s': %s"),
                               dname, strerror(errno));
      return CSOUND_ERROR;
    }
    /* scan all files in directory */
    while ((f = readdir(dir)) != NULL) {
      fname = &(f->d_name[0]);
      n = len = (int) strlen(fname);
#if defined(WIN32)
      strcpy(buf, "dll");
      n -= 4;
#elif defined(__MACH__)
      strcpy(buf, "dylib");
      n -= 6;
#else
      strcpy(buf, "so");
      n -= 3;
#endif
      if (n <= 0 || fname[n] != '.')
        continue;
      i = 0;
      do {
        if ((fname[++n] | (char) 0x20) != buf[i])
          break;
      } while (buf[++i] != '\0');
      if (buf[i] != '\0')
        continue;
      /* found a dynamic library, attempt to open it */
      if (((int) strlen(dname) + len + 2) > 1024) {
        csound->Warning(csound, Str("path name too long, skipping '%s'"),
                                fname);
        continue;
      }
      sprintf(buf, "%s%c%s", dname, DIRSEP, fname);
      n = csoundLoadExternal(csound, buf);
      if (n == CSOUND_ERROR)
        continue;               /* ignore non-plugin files */
      if (n < err)
        err = n;                /* record serious errors */
    }
    closedir(dir);
    return (err == CSOUND_INITIALIZATION ? CSOUND_ERROR : err);
#else
    return CSOUND_SUCCESS;
#endif  /* HAVE_DIRENT_H */
}

static int cmp_func(const void *p1, const void *p2)
{
    return (strcmp(*((const char**) p1), *((const char**) p2)));
}

int csoundLoadExternals(CSOUND *csound)
{
    char    *s, **lst;
    int     i, cnt, err;

    s = csound->dl_opcodes_oplibs;
    if (s == NULL || s[0] == '\0')
      return 0;
    /* IV - Feb 19 2005 */
    csound->dl_opcodes_oplibs = NULL;
    csound->Message(csound, Str("Loading command-line libraries:\n"));
    cnt = 1;
    i = 0;
    do {
      if (s[i] == ',')
        cnt++;
    } while (s[++i] != '\0');
    lst = (char**) mmalloc(csound, sizeof(char*) * cnt);
    i = cnt = 0;
    lst[cnt++] = s;
    do {
      if (s[i] == ',') {
        lst[cnt++] = &(s[i + 1]);
        s[i] = '\0';
      }
    } while (s[++i] != '\0');
    qsort((void*) lst, (size_t) cnt, sizeof(char*), cmp_func);
    i = 0;
    do {
      char  *fname = lst[i];
      if (fname[0] != '\0' && !(i && strcmp(fname, lst[i - 1]) == 0)) {
        err = csoundLoadExternal(csound, fname);
        if (err == CSOUND_INITIALIZATION || err == CSOUND_MEMORY)
          csound->Die(csound, Str(" *** error loading '%s'"), fname);
        else if (!err)
          csound->Message(csound, "  %s\n", fname);
      }
    } while (++i < cnt);
    /* file list is no longer needed */
    mfree(csound, lst);
    mfree(csound, s);
    return 0;
}

/**
 * Call initialisation functions of all loaded modules that have a
 * csoundModuleInit symbol, for Csound instance 'csound'.
 * Return value is CSOUND_SUCCESS if there was no error, and CSOUND_ERROR if
 * some modules could not be initialised.
 */
int csoundInitModules(CSOUND *csound)
{
    csoundModule_t  *m;
    int             i, retval = CSOUND_SUCCESS;

    /* call init functions */
    for (m = (csoundModule_t*) csound->csmodule_db; m != NULL; m = m->nxt) {
      if (m->PreInitFunc != NULL) {
        if (m->fn.p.InitFunc != NULL) {
          i = m->fn.p.InitFunc(csound);
          if (i != 0) {
            print_module_error(csound, "Error starting module '%s'",
                                       &(m->name[0]), m, i);
            retval = CSOUND_ERROR;
          }
        }
      }
      else {
        /* deal with fgens if there are any */
        if (m->fn.o.fgen_init != NULL) {
          NGFENS  *names = m->fn.o.fgen_init(csound);
          for (i = 0; names[i].name != NULL; i++)
            allocgen(csound, names[i].name, names[i].fn);
        }
        if (m->fn.o.opcode_init != NULL) {
          OENTRY  *opcodlst_n;
          long    length;
          /* load opcodes */
          if ((length = m->fn.o.opcode_init(csound, &opcodlst_n)) < 0L)
            retval = CSOUND_ERROR;
          else {
            length /= (long) sizeof(OENTRY);
            if (length) {
              if (csound->AppendOpcodes(csound, opcodlst_n, (int) length) != 0)
                retval = CSOUND_ERROR;
            }
          }
        }
      }
    }
    /* return with error code */
    return retval;
}

/**
 * Call destructor functions of all loaded modules that have a
 * csoundModuleDestroy symbol, for Csound instance 'csound'.
 * Return value is CSOUND_SUCCESS if there was no error, and
 * CSOUND_ERROR if some modules could not be de-initialised.
 */
int csoundDestroyModules(CSOUND *csound)
{
    csoundModule_t  *m;
    int             i, retval;

    retval = CSOUND_SUCCESS;
    while (csound->csmodule_db != NULL) {
      m = (csoundModule_t*) csound->csmodule_db;
      /* call destructor functions */
      if (m->PreInitFunc != NULL && m->fn.p.DestFunc != NULL) {
        i = m->fn.p.DestFunc(csound);
        if (i != 0) {
          print_module_error(csound, "Error de-initialising module '%s'",
                                     &(m->name[0]), m, i);
          retval = CSOUND_ERROR;
        }
      }
      /* unload library */
      csoundCloseLibrary(m->h);
      csound->csmodule_db = (void*) m->nxt;
      /* free memory used by database */
      free((void*) m);
    }
    /* return with error code */
    return retval;
}

 /* ------------------------------------------------------------------------ */

#if defined(WIN32) && !defined(__CYGWIN__)

PUBLIC int csoundOpenLibrary(void **library, const char *libraryPath)
{
    *library = (void*) LoadLibrary(libraryPath);
    return (*library != NULL ? 0 : -1);
}

PUBLIC int csoundCloseLibrary(void *library)
{
    return (int) (FreeLibrary((HMODULE) library) == FALSE ? -1 : 0);
}

PUBLIC void *csoundGetLibrarySymbol(void *library, const char *procedureName)
{
    return (void*) GetProcAddress((HMODULE) library, procedureName);
}

#elif defined(LINUX) || defined(__CYGWIN__)

PUBLIC int csoundOpenLibrary(void **library, const char *libraryPath)
{
    *library = (void*) dlopen(libraryPath, RTLD_NOW | RTLD_LOCAL);
    return (*library != NULL ? 0 : -1);
}

PUBLIC int csoundCloseLibrary(void *library)
{
    return (int) dlclose(library);
}

PUBLIC void *csoundGetLibrarySymbol(void *library, const char *procedureName)
{
    return (void*) dlsym(library, procedureName);
}

#elif defined(__MACH__)

/* Set and get the error string for use by dlerror */
static const char *error(int setget, const char *str, ...)
{
    static char errstr[ERR_STR_LEN];
    static int err_filled = 0;
    const char *retval;
    NSLinkEditErrors ler;
    int lerno;
    const char *dylderrstr;
    const char *file;
    va_list arg;
    if (setget <= 0) {
      va_start(arg, str);
      strncpy(errstr, "dlsimple: ", ERR_STR_LEN);
      vsnprintf(errstr + 10, ERR_STR_LEN - 10, str, arg);
      va_end(arg);
      /* We prefer to use the dyld error string if getset is 1*/
      if (setget == 0) {
        NSLinkEditError(&ler, &lerno, &file, &dylderrstr);
#if 0
        fprintf(stderr, "dyld: %s\n", dylderrstr);
#endif
        if (dylderrstr && strlen(dylderrstr))
          strncpy(errstr, dylderrstr, ERR_STR_LEN);
      }
      err_filled = 1;
      retval = NULL;
    }
    else  {
      if (!err_filled)
        retval = NULL;
      else
        retval = errstr;
      err_filled = 0;
    }
    return retval;
}

/* dlsymIntern is used by dlsym to find the symbol */
static void *dlsymIntern(void *handle, const char *symbol)
{
    NSSymbol *nssym = NULL;
    /* If the handle is -1, if is the app global context */
    if (handle == (void*) -1L) {
      /* Global context, use NSLookupAndBindSymbol */
      if (NSIsSymbolNameDefined(symbol)) {
        nssym = NSLookupAndBindSymbol(symbol);
      }
    }
    /* Now see if the handle is a struct mach_header* or not,
       use NSLookupSymbol in image for libraries, and
       NSLookupSymbolInModule for bundles */
    else {
      /* Check for both possible magic numbers depending
         on x86/ppc byte order */
      if ((((struct mach_header *)handle)->magic == MH_MAGIC) ||
          (((struct mach_header *)handle)->magic == MH_CIGAM)) {
        if (NSIsSymbolNameDefinedInImage((struct mach_header *)handle,
                                         symbol)) {
          nssym = NSLookupSymbolInImage(
                            (struct mach_header *)handle, symbol,
                            NSLOOKUPSYMBOLINIMAGE_OPTION_BIND |
                            NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR);
        }
      }
      else {
        nssym = NSLookupSymbolInModule(handle, symbol);
      }
    }
    if (!nssym) {
      error(0, "Symbol \"%s\" Not found", symbol);
      return NULL;
    }
    return NSAddressOfSymbol(nssym);
}

int csoundOpenLibrary(void **library, const char *libraryPath)
{
    NSObjectFileImage ofi = 0;
    NSObjectFileImageReturnCode ofirc;
    static int (*make_private_module_public) (NSModule module) = NULL;
    unsigned int flags =  NSLINKMODULE_OPTION_RETURN_ON_ERROR |
                          NSLINKMODULE_OPTION_PRIVATE;

    /* If we got no path, the app wants the global namespace,
       use -1 as the marker in this case */
    if (!libraryPath) {
      *library = (void*) -1L;
      return 0;
    }
    *library = NULL;
    /* Create the object file image, works for things linked
       with the -bundle arg to ld */
    ofirc = NSCreateObjectFileImageFromFile(libraryPath, &ofi);
    switch (ofirc) {
    case NSObjectFileImageSuccess:
      /* It was okay, so use NSLinkModule to link in the image */
      *library = NSLinkModule(ofi, libraryPath, flags);
      /* Don't forget to destroy the object file
         image, unless you like leaks */
      NSDestroyObjectFileImage(ofi);
      /* If the mode was global, then change the module, this avoids
         multiply defined symbol errors to first load private then
         make global. Silly, isn't it. */
      if (!make_private_module_public) {
        _dyld_func_lookup("__dyld_NSMakePrivateModulePublic",
                          (unsigned long*) &make_private_module_public);
      }
      make_private_module_public(*library);
      break;
    case NSObjectFileImageInappropriateFile:
      /* It may have been a dynamic library rather
         than a bundle, try to load it */
      *library = (void*) NSAddImage(libraryPath,
                                    NSADDIMAGE_OPTION_RETURN_ON_ERROR);
      break;
    case NSObjectFileImageFailure:
      error(0, "Object file setup failure :  \"%s\"", libraryPath);
      return -1;
    case NSObjectFileImageArch:
      error(0, "No object for this architecture :  \"%s\"", libraryPath);
      return -1;
    case NSObjectFileImageFormat:
      error(0, "Bad object file format :  \"%s\"", libraryPath);
      return -1;
    case NSObjectFileImageAccess:
      error(0, "Can't read object file :  \"%s\"", libraryPath);
      return -1;
    }
    if (*library == NULL) {
      error(0, "Can not open \"%s\"", libraryPath);
      return -1;
    }
    return 0;
}

int csoundCloseLibrary(void *library)
{
    if ((((struct mach_header *)library)->magic == MH_MAGIC) ||
        (((struct mach_header *)library)->magic == MH_CIGAM)) {
      error(-1, "Can't remove dynamic libraries on darwin");
      return -1;
    }
    if (!NSUnLinkModule(library, 0)) {
      error(0, "unable to unlink module %s", NSNameOfModule(library));
      return -1;
    }
    return 0;
}

void *csoundGetLibrarySymbol(void *library, const char *procedureName)
{
    char  undersym[257];

    if ((int) strlen(procedureName) > 255) {
      error(-1, "Symbol name is too long");
      return NULL;
    }
    sprintf(undersym, "_%s", procedureName);
    return (void*) dlsymIntern(library, undersym);
}

#elif defined(mac_classic)

PUBLIC int csoundOpenLibrary(void **library, const char *libraryName)
{
    CFragConnectionID connID;
    Ptr         mainAddr;
    OSErr       err;
    Str63       macLibName;
    Str255      errName;

    *library = NULL;
    if (strlen(libraryName) < 64) {
      strcpy((char*) macLibName, libraryName);
      c2pstr((char*) macLibName);
    }
    else {
 /*   csoundMessage("%s is not a valid library name because it is too long.\n",
                    libraryName); */
      return -1;
    }
    /* first, test to see if the library is already loaded */
    err = GetSharedLibrary(macLibName, kPowerPCCFragArch, kFindCFrag,
                           &connID, &mainAddr, errName);
    if (err == noErr)
      return -1;        /* already loaded */
    else if (err != cfragLibConnErr) {  /* some other error occurred */
 /*   csoundMessage("Failed to load plugin library %s with Mac error %d.\n",
                    libraryName, err); */
      return -1;
    }
    else {  /* attempt to load the library */
      err = GetSharedLibrary(macLibName, kPowerPCCFragArch, kLoadCFrag,
                             &connID, &mainAddr, errName);
      if (err != noErr) {
 /*     csoundMessage("Failed to load plugin library %s with Mac error %d.\n",
                      libraryName, err); */
        return -1;
      }
    }
    *library = (void*) connID;
    return 0;
}

PUBLIC int csoundCloseLibrary(void *library)
{
    CFragConnectionID connID;
    OSErr       err;

    connID = (CFragConnectionID) library;
    err = CloseConnection(&connID);
    return 0 /* (err != noErr) */;  /* ignore errors */
}

PUBLIC void *csoundGetLibrarySymbol(void *library, const char *procedureName)
{
    OSErr       err;
    Ptr         symAddr;
    CFragSymbolClass  symClass;
    CFragConnectionID connID;
    Str255      macProcName;

    connID = (CFragConnectionID) library;
    if (strlen(procedureName) < 256) {
      strcpy((char*) macProcName, procedureName);
      c2pstr((char*) macProcName);
    }
    else {
 /*   csoundMessage("%s is not a valid library procedure name "
                    "because it is too long.\n", procedureName); */
      return NULL;
    }
    err = FindSymbol(connID, macProcName, &symAddr, &symClass);
    if (err != noErr) {
 /*   csoundMessage("Failed to find library procedure %s with Mac error %d.\n",
                    procedureName, err); */
      return NULL;
    }
    else if (symClass == kDataCFragSymbol) {
 /*   csoundMessage("Failed to load procedure %s "
                    "because it is not a code symbol.\n", procedureName); */
      return NULL;
    }

    return (void*) symAddr;
}

#else /* case for platforms without shared libraries -- added 062404, akozar */

int csoundOpenLibrary(void **library, const char *libraryPath)
{
    *library = NULL;
    return -1;
}

int csoundCloseLibrary(void *library)
{
    return 0;
}

void *csoundGetLibrarySymbol(void *library, const char *procedureName)
{
    return NULL;
}

#endif

static const char *opcodedirWarnMsg[] = {
    "################################################################",
#ifndef USE_DOUBLE
    "#               WARNING: OPCODEDIR IS NOT SET !                #",
#else
    "#              WARNING: OPCODEDIR64 IS NOT SET !               #",
#endif
    "# Csound requires this environment variable to be set to find  #",
    "# its plugin libraries. If it is not set, you may experience   #",
    "# missing opcodes, audio/MIDI drivers, or utilities.           #",
    "################################################################",
    NULL
};

void print_opcodedir_warning(CSOUND *p)
{
    if (!p->opcodedirWasOK) {
      const char  **sp;
      for (sp = &(opcodedirWarnMsg[0]); *sp != NULL; sp++)
        p->MessageS(p, CSOUNDMSG_WARNING, "        %s\n", Str(*sp));
    }
}

