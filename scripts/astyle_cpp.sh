#!/bin/sh

# Applies our project-wide indentation rules to C++ source code.

astyle --style=gnu -r -n -c "*.cpp" "*.h"
exit $?
