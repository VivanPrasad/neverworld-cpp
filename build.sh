#!/bin/bash

echo "Building Neverworld..."
includes="-Ithird_party -Ithird_party/Include"
libraries="-lgdi32 -lopengl32"
warnings="-Wno-write-strings -Wno-format-security"
g++ $includes -g src/main.cpp -o neverworld.exe $libraries $warnings