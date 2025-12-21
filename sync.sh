#!/usr/bin/env bash
set -e

VAULTS=~/Documents/vaults
DEST=./content

rsync -av --delete \
  --exclude '.git' \
  --exclude '.obsidian' \
  --exclude 'StableSR_doc' \
  --exclude '.gitignore' \
  --exclude '.vscode' \
  --exclude '.html' \
  --exclude 'README.md' \
  $VAULTS/* $DEST/
