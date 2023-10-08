#!/bin/sh

clang-format -i $(find . -name '**.h') $(find . -name '**.cc') $(find . -name '**.c')

