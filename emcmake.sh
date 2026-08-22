#!/usr/bin/env bash
set -e

# 1. Load Emscripten environment
source /c/code/c++/emsdk/emsdk_env.sh

# 2. Clean previous web build
rm -rf build_web

# 3. Configure using emcmake (using absolute path to cmake)
emcmake /ucrt64/bin/cmake -B build_web -G Ninja \
  -DCMAKE_MAKE_PROGRAM=/ucrt64/bin/ninja.exe \
  -DCMAKE_BUILD_TYPE=MinSizeRel

# 4. Build the project (using absolute path to cmake)
/ucrt64/bin/cmake --build build_web
