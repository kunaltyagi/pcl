#! /usr/bin/env bash

#SANITIZERS="-fsanitize=address,memory,undefined,implicit-conversion,integer,nullability"
#CXX_FLAGS_EXTRA="-Wconversion -Wcast-align -Wunused -Wshadow -Wold-style-cast -Wpointer-arith -Wcast-qual -Wno-missing-braces"
#-Wthread-safety
cmake \
  -DCMAKE_OSX_SYSROOT="/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk" \
  -DCMAKE_C_COMPILER="/usr/local/bin/gcc-9" \
  -DCMAKE_CXX_COMPILER="/usr/local/bin/g++-9" \
  -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wno-cpp ${CXX_FLAGS_EXTRA} ${SANITIZERS}" \
  -DPCL_ENABLE_CCACHE=ON \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
  -G Ninja \
  -DBUILD_global_tests=ON -DGTEST_SRC_DIR=$HOME/workspace/googletest/googletest -DGTEST_INCLUDE_DIR=$HOME/workspace/googletest/googletest/include/ \
  -DWITH_VTK:BOOL=OFF \
  $@ \
  ..
#  -DDOXYGEN_USE_SHORT_NAMES=OFF -DSPHINX_HTML_FILE_SUFFIX=php -DWITH_DOCS=ON \
#  -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/store/workspace/include-what-you-use/build/bin/include-what-you-use \
#  -DBUILD_examples=ON \
#  -DBUILD_CUDA=ON -DBUILD_GPU=ON -DCUDA_HOST_COMPILER=/opt/cuda/bin/c++ \
#  -DBUILD_cuda_io=ON -DBUILD_cuda_apps=ON \
#  -DBUILD_gpu_people=ON -DBUILD_gpu_surface=ON -DBUILD_gpu_tracking=ON \
#  -DBUILD_simulation=ON \
#  -DBUILD_surface_on_nurbs=ON \
#  -DWITH_TUTORIALS=ON \
#  -DBUILD_apps=ON -DBUILD_apps_point_cloud_editor=ON -DBUILD_apps_cloud_composer=ON -DBUILD_apps_modeler=ON \
