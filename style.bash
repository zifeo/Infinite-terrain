#!/usr/bin/env bash

clang-format -i $(find src -name '*.h') $(find src -name '*cpp') $(find src -name '*.glsl')
