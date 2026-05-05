#!/bin/bash

PROJECT_DIR="atomicassets"
SOURCE_FILE="src/atomicassets.cpp"
INCLUDE_DIR="include"
# RICARDIAN_DIR="ricardian"
OUTPUT_FILE="${PROJECT_DIR}/atomicassets.wasm"

mkdir -p "$PROJECT_DIR"

rm -rf "${PROJECT_DIR:?}"/*

if [ -f "$SOURCE_FILE" ]; then
    # Compile the contract 
    cdt-cpp -abigen -contract "atomicassets" -o "$OUTPUT_FILE" "$SOURCE_FILE" -I "$INCLUDE_DIR"
    
    if [ -f "$OUTPUT_FILE" ]; then
        echo "Compilation successful, WASM file created at $OUTPUT_FILE"
    else
        echo "Compilation failed, no WASM file produced."
        exit 1
    fi
else
    echo "Error: Source file $SOURCE_FILE does not exist."
    exit 1
fi