# Plot-OpenCL
Plot-OpenCL is a MassDB plotting tool accelerated with opencl.

# Build & Test (only tested for Ubuntu 20.04 with g++-9)

## Dependency
```
$ sudo apt install ocl-icd-opencl-dev cmake build-essential
```

## Build
```
$ mv CMakeLists_Linux.txt CMakeLists.txt
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cp ../sha256_mass.cl ./
```

## Test
```
$ ./plot-opencl <output_dir> <ordinal> <public_key>
```
Please note that Plot-OpenCL only supports bit_length of 32.

For more information about arguments, please refer to https://github.com/massnetorg/Plot-CPU/blob/main/README.md
