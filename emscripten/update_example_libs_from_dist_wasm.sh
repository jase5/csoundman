#!/bin/sh
if [ ! -d examples-wasm ]
 then
   mkdir examples-wasm
fi
cp -r examples-src/* examples-wasm/
cp dist-wasm/* examples-wasm/javascripts
mv examples-wasm/javascripts/libcsound.wasm examples-wasm

