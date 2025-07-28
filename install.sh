#!/usr/bin/env bash

set -e
echo "installing trashbhuwan..."
RELEASES=$(curl -s https://api.github.com/repos/tribhuwan-kumar/trashbhuwan/releases)
LATEST_RELEASE=$(echo "$RELEASES" | grep -oP '"tag_name":\s*"\K[^"]+' | head -n 1)

if [ -z "$LATEST_RELEASE" ]; then
    echo "Error: No releases found."
    exit 1
fi

echo "Latest version: $LATEST_RELEASE"
ASSET_URL=$(echo "$RELEASES" | grep -oP '"browser_download_url":\s*"\K[^"]+' | grep "trashbhuwan-linux.tar.gz" | head -n 1)

if [ -z "$ASSET_URL" ]; then
    echo "Error: No Linux release found for version $LATEST_RELEASE."
    exit 1
fi

echo "downloading asset from: $ASSET_URL"

# set up directories
BIN_DIR="$HOME/.local/bin"
TMP_DIR=$(mktemp -d)
TAR_FILE="$TMP_DIR/trashbhuwan-linux.tar.gz"

mkdir -p "$BIN_DIR"

# download and extract the asset
curl -sL "$ASSET_URL" -o "$TAR_FILE"
tar -xzf "$TAR_FILE" -C "$BIN_DIR"

# verify installation
if [ ! -x "$BIN_DIR/trashbhuwan" ]; then
    echo "Error: trashbhuwan binary is not executable."
    exit 1
fi

# clean up
rm -rf "$TMP_DIR"

# add ~/.local/bin to PATH if not already present
if ! echo "$PATH" | grep -q "$BIN_DIR"; then
    SHELL_CONFIG=""
    case "$SHELL" in
        */zsh)
            SHELL_CONFIG="$HOME/.zshrc"
            ;;
        */bash)
            SHELL_CONFIG="$HOME/.bashrc"
            ;;
    esac

    if [ -n "$SHELL_CONFIG" ]; then
        echo 'export PATH="$HOME/.local/bin:$PATH"' >>"$SHELL_CONFIG"
        echo "Added ~/.local/bin to PATH in $SHELL_CONFIG. Please restart your terminal or run: source $SHELL_CONFIG"
    else
        echo "Please add ~/.local/bin to your PATH manually."
    fi
fi

echo "trashbhuwan installed successfully!!"
echo "to get started, run: trashbhuwan"
