#!/bin/bash
find ./source/dagger/core -regex '.*\.\(cpp\|h\)' -exec clang-format -style=file -i {} \;
find ./source/dagger/gameplay -regex '.*\.\(cpp\|h\)' -exec clang-format -style=file -i {} \;
find ./source/dagger/tools -regex '.*\.\(cpp\|h\)' -not -wholename "*/imgui/*" -exec clang-format -style=file -i {} \;
clang-format -style=file -i source/dagger/main.cpp