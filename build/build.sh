echo "There is build script"
rm CMakeCache.txt
echo "Removed CMakeCache.txt file"
cmake -G "MinGW Makefiles" ..
make
