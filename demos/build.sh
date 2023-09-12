rm -rf build
mkdir build
cd build
cmake ..
make
./main >> image.ppm
open image.ppm
cd ..
