#!/bin/bash

set -euo pipefail

# Override these with environment variables when deploying to a different chain/account.
RPC_URL="${RPC_URL:-https://testnet-api-use1.dev.wire-dev.com}"
CONTRACT_ACCOUNT="sysio.nft"
CONTRACT_DIR="${CONTRACT_DIR:-./atomicassets}"
CONTRACT_WASM="${CONTRACT_WASM:-atomicassets.wasm}"
CONTRACT_ABI="${CONTRACT_ABI:-atomicassets.abi}"
ACTIVE_PERMISSION="${CONTRACT_ACCOUNT}@active"
INITIALIZE=false

usage() {
    echo "Usage: $0 [-i]"
    echo "  -i  Set sysio.code on ${CONTRACT_ACCOUNT}@active and call init after deploy."
    echo
    echo "Environment overrides:"
    echo "  RPC_URL=<url> CONTRACT_ACCOUNT=<account> CONTRACT_DIR=<dir> $0 [-i]"
    echo "  CONTRACT_WASM=<wasm> CONTRACT_ABI=<abi> $0 [-i]"
}

initialize() {
    clio -u "$RPC_URL" set account permission "$CONTRACT_ACCOUNT" active --add-code -p "$ACTIVE_PERMISSION"
    clio -u "$RPC_URL" push action "$CONTRACT_ACCOUNT" init '[]' -p "$ACTIVE_PERMISSION"
}

while getopts ":ih" opt; do
    case "$opt" in
        i)
            INITIALIZE=true
            ;;
        h)
            usage
            exit 0
            ;;
        \?)
            usage >&2
            exit 1
            ;;
    esac
done

shift $((OPTIND - 1))

if [ "$#" -ne 0 ]; then
    usage >&2
    exit 1
fi

if [ ! -f "$CONTRACT_DIR/$CONTRACT_WASM" ] || [ ! -f "$CONTRACT_DIR/$CONTRACT_ABI" ]; then
    echo "Missing contract artifacts in $CONTRACT_DIR. Run ./build.sh first." >&2
    exit 1
fi

clio -u "$RPC_URL" set contract "$CONTRACT_ACCOUNT" "$CONTRACT_DIR" "$CONTRACT_WASM" "$CONTRACT_ABI" -p "$ACTIVE_PERMISSION"

if [ "$INITIALIZE" = true ]; then
    initialize
fi
