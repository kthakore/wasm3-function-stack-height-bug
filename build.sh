#!/bin/sh
echo "Compiling wasm"
cargo build --target wasm32-unknown-unknown --release 

echo "Optimizing WASM"

wasm-opt -O3 target/wasm32-unknown-unknown/release/wasm3_function_stack_height_bug.wasm -o app.wasm
wasm-strip  app.wasm
xxd -iC  app.wasm  > ./wasm3_arduino/app_wasm.h

echo "Deploying to target arduino:mbed:nano33ble:/dev/cu.usbmodem14201"

arduino-cli compile --fqbn arduino:mbed:nano33ble wasm3_arduino

arduino-cli upload -p /dev/cu.usbmodem14201 --fqbn arduino:mbed:nano33ble wasm3_arduino

sleep 5
screen /dev/cu.usbmodem14201

echo "\r\n"

