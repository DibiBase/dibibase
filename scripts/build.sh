#!/bin/sh

if [ -e  build/ ]; then
  ninja -C build/
else
  ./scripts/rebuild.sh
fi
