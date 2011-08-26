#!/bin/bash

# The directory 'b' should contain your latest changes to keep the diffs minimal.
# only run this from the ./aire/ directory. As you can tell I can't be bothered
# making this script generic, neither can I be bothered to remember the patch
# creation process.

mkdir a
../navvie/navvie aire.uml ./a > /dev/null
diff -uN a b > aire_cpp.patch
rm -rf a
