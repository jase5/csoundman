<!---

To maintain this document use the following markdown:

# First level heading
## Second level heading
### Third level heading

- First level bullet point
 - Second level bullet point
  - Third level bullet point

`inline code`

``` pre-formatted text etc.  ```

[hyperlink](url for the hyperlink)

Any valid HTML can also be used.

 --->

# CSOUND VERSION 6.15 RELEASE NOTES

-- The Developers

## USER-LEVEL CHANGES

### New opcodes

- ftset sets multiple elements of a table to a given value

- lufs opcode calculates a momentary, integrated and short-term loudness meter

- bob filter is a numerical simultion of the Moog analog resonant filter

- sterrain is an enhanced verion of wterrain with more possible orbits

- count, cntCreate, cntReas, cntReset, cntCycle and cntState togeher imprement a new conter object that cycles trough a constant range, similar to in PD

- new alias for sc_ opcodes: sc_lag -> lag, sc_lagud -> lagud, sc_trig -> trigholf, sc_phasor -> phasortrigo

- println is similar to printf butwithout the trigger

### New Gen and Macros

### Orchestra

- #include of a url now works again

- the end of file case is better handled in the pre-lexer

- corrections to reported line number in a few error cases

- conditional expressions yielding strings fixed, and other cases

- the sequence //* no lomnger is misinterpreted as starting acomment block

### Score

- New score opcode B is like b but is accumulative

- the end of file case is better handled in the pre-lexer

### Options

- keep-sorted-score and simple-sorted-score both can take a filename in which to write the score after a =

- print_version option prints the verion of csoud used at the end of a rendering

- syntax-check-only return an error if syntax failed

### Modified Opcodes and Gens

- cent, semitone, dB accuracy improved

- taninv2 now has an array version

- ftsice has more variations

- ptable ocodes are now deprecated as they are identical to table opcodes

- GEN20 case 9 (sinc fuction) now has an optional parameter tao the x range

- fprint(k)s now has a %s format specifier

- lastcycle corrected and clarified

### Utilities


### Frontends

- Belacsound:

- CsoundQt:


### General Usage


## Bugs Fixed

- setcols way very broken; fixed

- cps2pch and cpsxpc fixed in the case f a table of frequencies

- the 31 bit pseiudo random number generator was seeded with zero then itstayed on zero.  Than it now fixed.

- gen 20 was wrong in thecase of 8

- turning off an instrumemt from inside a UDO nowworks

# SYSTEM LEVEL CHANGES

-

### System Changes

- Many fixes to memory problems, mainlt invalid reads/writes

### Translations

### API


### Platform Specific

- WebAudio: 

- iOS

- Android

- Windows

- MacOS

- GNU/Linux

- Haiku port

- Bela
 - updated digiBelaOut and digiIOBela


==END==
commit 1e64bf60c65cd0bf63e2b6e05f019cfeacaca9ee
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue Jul 14 16:10:50 2020 +0100

    issue #1300

commit a2806d6af1b7acff488979c94b4d452349bf16e4
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue Jul 14 15:45:48 2020 +0100

    issue #1349

commit 55f588a9e6f1c766e63ada1e31b5066b8284a2c9
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue Jul 14 13:09:42 2020 +0100

    issue #1232

commit a0ad9bccb06712c45edd9a7bcc0d5b939d9d158e
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue Jul 14 12:56:09 2020 +0100

    fixed #1348


    issue #1358

commit 8d1898a8d7bb0cdfe0317691fe16f73382795a51
Author: Steven Yi <stevenyi@gmail.com>
Date:   Sat Jul 11 09:50:39 2020 -0400

    attempting fix for default opcode dir after LIBRARY_INSTALL_DIR change

commit 02294d46467022817da3e9c080a584dcaf9aa8c5
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun Jul 5 21:47:08 2020 +0100

    updated instructions to include setting release config

commit 7a7170fe7b2f636189b093dfa8b467b5ce4d844d
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun Jul 5 21:43:26 2020 +0100

    not hardcoding release

commit 1fd81c2297ac4d5846dfa0c5310e8e9df39cd5ca
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun Jul 5 21:31:57 2020 +0100

    improved status messages

commit e99e0002fe258b5ddd687710c2b7949b9fbc08d9
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun Jul 5 20:51:26 2020 +0100

    added Release config to crosscompile

commit d79e32e6a11a0f8b90448f6f617f6c9c466d9fc7
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Fri Jul 3 14:46:46 2020 +0100

    fixed ticket 1341


commit d9ae5fd77efeaec6c66e6bf9dddcf161c930b70e
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat Jun 27 18:17:08 2020 +0100

    option

commit 77dbd4d6ddae244347c992a551ff1b7487b3c2d0
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat Jun 20 22:28:54 2020 +0100

    cmdline option to load all plugins from a given dir

commit d7560d1ec8899ee77e2769a81ec20a670885e467
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat Jun 20 22:19:48 2020 +0100

    added LoadPlugins to c++ interface

commit 1a9b7091ffafa1b8eff271a5e22ce5bafff72457
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat Jun 20 22:06:40 2020 +0100

    new API function to load plugins

commit b6667a35346710cbf798910c024affd5f1405870
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Fri Jun 19 23:43:42 2020 +0100

    set opcodedir

commit fa6a68f8b8a7158f623e849b66818def8e8aef06
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Fri Jun 19 22:58:16 2020 +0100

    opcodedir experimental feature

commit b50ad48221841f5abc9540b2f9c5dddbde1a67ff
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Fri Jun 19 10:07:11 2020 +0200

    better error for dispfft

commit d7350600ad4e76bdb6c3c4a8056119ef28284898
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Wed Jun 17 18:01:05 2020 +0100

    allowing some opcodes to use functional syntax if mono output

commit 5da38e1e9db74e4d9fa0a686dda3dcc4b19b572e
Author: joachimheintz <2196394+joachimheintz@users.noreply.github.com>
Date:   Sun Jun 14 20:29:31 2020 +0200

    increase MAXLINE to 8192
    
    i ran into issues in reading long lines from het.  and as far as i see, this would coincide with the limit in fprints as set in fout.c, line 1284.

commit 0c506fcef4cb23cbdfff4d8c916e46ec2fe91a13
Author: John ffitch <jpff@codemist.co.uk>
Date:   Fri Jun 12 15:46:48 2020 +0100

    Revised/extended sale opcode+ tweaks

commit 164abacfd1b4561848cb15ce2c10472ea5e94a39
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Mon Jun 1 13:18:20 2020 +0100

    resonbnk and apoleparams


commit 4987a50e3f9a05d1fb966cc054075187e70a441e
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun May 31 10:44:32 2020 +0100

    resonator bank

commit 69b2e4728765751d74278d125836b214d40f20a8
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat May 30 18:47:16 2020 +0100

    allpoleb opcode

Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat May 30 18:13:52 2020 +0100

    coeffs to params fix

commit ab8c930d00c16a7e63b93c23240688df9624dd35
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat May 30 17:43:31 2020 +0100

    coeffs to params

commit 3df3dfe17b8b9525706f81ed301d28b88c824764
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat May 30 15:58:41 2020 +0100

    pvscfs opcode complete

commit ad226acb57c682636e7de9b8ae805349bc4b6b40
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat May 30 10:43:09 2020 +0100

    pole to coeff and stabilisation code

commit a7bdd937cd2bde1fdbdaf6a9babe75177f5cdbb1
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Fri May 29 20:28:21 2020 +0100

    csound lock functions correctly from struct now

commit 0e036d7c3f70ff110516b193894c4360547bdff6
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Fri May 29 13:34:53 2020 +0100

    added the Durbin method to lpanal

commit 7a20da5cc99dd73354347d5ece45c473e31f6838
Author: John ffitch <jpff@codemist.co.uk>
Date:   Fri May 22 19:48:58 2020 +0100

    table opcodes were wrong wrt non powerof2 cases

commit c587c9cc29377ae5fb2164e7a25c066008116b61
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Wed May 20 20:29:28 2020 +0100

    lpcanal to pvs

commit 5577f451992f1e28a66b43031d3076546b812ed9
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 20:59:46 2020 +0100

    removing kprd restriction

commit dbe54203a066c943117d592a472446939419e70d
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 19:20:31 2020 +0100

    changed opcode name

commit 502990fab5c8b9a09f79bd11b7523ee71e54311f
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 17:54:13 2020 +0100

    added lpc signal analysis

commit 8f53cef799b274ff0a138611ac9b6eedbc969d2c
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 17:20:23 2020 +0100

    initialisation

commit 98707d01694d3bf766525f6699eade22c556fc42
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 14:03:26 2020 +0100

    cps corrected

commit c03f27c5dc624cd2bbe8d1a61673940db51fd95f
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 13:27:54 2020 +0100

    allpole filter

commit 4961d7e524bc37b72e8256fc80c3c48bfb2cbe6b
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 11:01:07 2020 +0100

    fixed filter scaling

commit 7946ccda73cc3ba7051c7361213aeb63d5eebf1e
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue May 19 09:19:23 2020 +0100

    rms computation


commit ebf454156404148362e793f2be9805ec81a1baca
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Mon May 18 22:23:55 2020 +0100

    added flag argument, fixed scaling

commit e8d7e8314943b59f366167e3b0e68e8b2ffde7ba
Author: John ffitch <jpff@codemist.co.uk>
Date:   Mon May 18 14:30:08 2020 +0100

    new files

commit f1696497f552fefb0b85bc26d4d5917e841f9a05
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun May 17 21:39:04 2020 +0100

    update lpcfilter

commit 72db7e6fa55d2a8b9ecfb065dc72ceeb337754b7
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun May 17 21:01:48 2020 +0100

    fixed lpcfilter v2

commit 2422adae5833de84a0143b1d293122f1ce423a8b
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun May 17 18:19:14 2020 +0100

    added new lpc opcode

commit eb78069fc2b48e9d7d1714a7c498f721aefab4af
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sun May 17 16:42:01 2020 +0100

    fixed lpread code and test opcode

commit 0c33b36279b263d947a590f2026d6877b01816a7
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat May 16 11:01:34 2020 +0100

    new linear prediction functions

commit 411addb2e47205b533b12287c061ad2a07808e9a
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Sat May 16 10:35:43 2020 +0100

    linear prediction functions

commit 12e5676912b05de886118aaeb8ab6ac6e7eca225
Author: John ffitch <jpff@codemist.co.uk>
Date:   Wed May 13 20:59:39 2020 +0100

    Adjust for rounding error in GEN11

commit aa51ad9656395fbb158dfed7860ff35715665718
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Fri May 8 21:07:16 2020 +0100

    improved phase estimation in partial tracking

commit 582c740394c775c72a9e3ac22cf7a1abeaa3650d
Author: John ffitch <jpff@codemist.co.uk>
Date:   Fri May 8 20:52:16 2020 +0100

    Permit zero input channels

    
commit fb2e61cf2bea6caf5117c30c120430b0fa4ee56c
Author: John ffitch <jpff@codemist.co.uk>
Date:   Sat Apr 25 16:22:55 2020 +0100

    Suggeted fix to  gen16

commit cef10017a0f3bceb2591d61882d9e7fc7e4b5093
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Mon Apr 20 20:49:37 2020 +0100

    new gauss opcode

commit 6e5569aa2a614ef7a3994c522f84b6a9c626b9d7
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Sun Apr 19 20:04:21 2020 +0200

    fix ftprint not following the manual regarding trig == -1; fix ftprint showing wrong index;

commit ba2965e533f9e658ff00956642bbe7540e190827
Author: John ffitch <jpff@codemist.co.uk>
Date:   Sat Apr 18 21:34:50 2020 +0100

     add rndseed opcode

commit 2428e9c51595d8348068a67cfd35a2715cbf2c95
Author: John ffitch <jpff@codemist.co.uk>
Date:   Wed Apr 15 21:12:52 2020 +0100

    issue #1317 fixed

commit 6550362a23a006a28a441d288f637baca845fd49
Author: vlazzarini <victor.lazzarini@mu.ie>
Date:   Tue Apr 14 11:13:03 2020 +0100

    added pvsbandwidth

commit de4a066566430e8742989dc41638242f5d07fa04
Author: David Runge <dave@sleepmap.de>
Date:   Sat Apr 11 22:28:05 2020 +0200

    Fix for HDF5 1.12.0
    
    Opcodes/hdf5/HDF5IO.c:
    The HDF5 update to 1.12.0 introduced breaking API changes [1]
    Particularly H5Oget_info() got removed.
    Instead H5Oget_info1() can be called, which provides the same
    signature.
    For HDF5 prior to 1.12.0 the call to H5Oget_info() remains.
    
    Closes #1313
    
commit 6ce22a01dc9c2d5573ceb969819bc622dab50902
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Mon Mar 16 14:26:25 2020 +0100

    replace simple char comparison by strcmp in chn_k string init

commit 308b4371408030c614132f57f15bdff7b02b4a90
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Thu Mar 12 23:47:05 2020 -0700

    Coreaudio: check if input/output devices have enough channels.
    Fail if the number of channels is less than requested via nchnls
    or nchnls_i. Without this, coreaudio fails silently and hangs.
    With this fix coreaudio has the same behaviour as portaudio on
    macOS

commit 53cbb19fea67e7a96d9d61190ded6531fa473c78
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Fri Mar 13 08:32:33 2020 +0100

    add warning for channel mismatch in coreaudio


commit c270902d78db558debb8d84a8828344cb9cb942d
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Thu Mar 12 23:47:05 2020 -0700

    Coreaudio: check if input/output devices have enough channels.
    Fail if the number of channels is less than requested via nchnls
    or nchnls_i. Without this, coreaudio fails silently and hangs.
    With this fix coreaudio has the same behaviour as portaudio on
    macOS

commit 430a99cdfbb97d4d8302d0f369aaedc136e1027d
Merge: ef906df32 39576bb32
Author: Steven Yi <stevenyi@gmail.com>
Date:   Fri Mar 20 14:37:03 2020 -0400

    chn_k: add possibility to set the input/output mode as string


commit 93be52a8dc769fa3ec452d388ae3432c207a0b7c
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Thu Mar 19 01:26:18 2020 +0100

    fixed reshapearray

commit 83e5137e3adcbe0d7f912fa381c8fa16b52d52b2
Author: John ffitch <jpff@codemist.co.uk>
Date:   Wed Mar 18 19:26:38 2020 +0000

    Various impromements in trim

commit e5d58878aa3ecbf38b57ed269153fba1ce625276
Merge: 456b10b92 24bbcc00a
Author: John ffitch <jpff@codemist.co.uk>
Date:   Fri Mar 13 17:02:58 2020 +0000

    Ensure that widget_reset() is called on reset

commit b6593a6d9d243a65ee8f4179d29e08d45af68104
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Thu Mar 12 23:47:05 2020 -0700

    Coreaudio: check if input/output devices have enough channels.
    Fail if the number of channels is less than requested via nchnls
    or nchnls_i. Without this, coreaudio fails silently and hangs.
    With this fix coreaudio has the same behaviour as portaudio on
    macOS

commit fb6627f63dd7995ecb8578af3ef210d697389149
Author: John ffitch <jpff@codemist.co.uk>
Date:   Tue Mar 10 22:03:25 2020 +0000

    Fix to setksmp problems

commit 03d5e69598a6679eb66ef94e77c808fcf40c6576
Author: Eduardo Moguillansky <eduardo.moguillansky@gmail.com>
Date:   Fri Mar 6 00:25:27 2020 +0100

    added version of chn_k accepting the mode as string. r=1 (input), w=2 (output), rw=3 (input+output)

commit 19aac3f59b37e0ca6560475f408e03cfaca6da44
Merge: 42f7af7c1 c444e8f75
Author: John ffitch <jpff@codemist.co.uk>
Date:   Thu Feb 27 16:48:22 2020 +0000

    Merge pull request #1283 from laserbat/yyin
    
    Fixed issue when linking csbeats

commit 093d3fdd17ed6fc789d316e30627b371b987c404
Author: John ffitch <jpff@codemist.co.uk>
Date:   Tue Feb 18 21:26:33 2020 +0000

    A¬pply fix for uninitialied data; PR 1257 and 1258

commit ae6dc653f5014bd424603b029a6a95692aee5f39
Author: John ffitch <jpff@codemist.co.uk>
Date:   Tue Feb 18 20:25:15 2020 +0000

    A¬pply fix to macro expamsion wthat had an uninitialised value; PR 1256

commit ec5d156401fbd4c65ce7c91a6c1375f9760be002
Author: Giulio Moro <giuliomoro@yahoo.it>
Date:   Sun Feb 9 23:55:27 2020 +0000

    Bela: updated include folders
