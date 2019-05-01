cmake_minimum_required(VERSION ${CMAKE_VERSION}})
project(Tane)
set(VERSION 2.0)

set(PROJECT_VERSION_MAJOR 19)
set(PROJECT_VERSION_MINOR 01)

set(CMAKE_CXX_STANDARD          11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE      false)
set(CMAKE_MODULE_PATH           ${CMAKE_MODULE_PATH} ${CMAKE_HOME_DIRECTORY}/cmake)

#-------------------------------------------------------------------- configure external library
set(MAYA_VER     "2018"                                                                                                     CACHE STRING "Maya Version")
set(RMAN_VER     "22.3"                                                                                                     CACHE STRING "Rman Version")
set(USD_VER      "19.01"                                                                                                    CACHE STRING "Usd Version")
set(REDSHIFT_VER "2.5.13"                                                                                                   CACHE STRING "RedShift Version")
set(PUBLISH_ROOT "/dexter/Cache_DATA/RND/dohyeon/build/${PROJECT_NAME}/${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"   CACHE STRING "PUBLISH ROOT")

include(GNUInstallDirs)
include(cotire)

include(echoTarget)
include(buildHelper)

include(local)
include(backstage)
3rdparty(include)

#-------------------------------------------------------------------- add definition
add_definitions(-DTANE_VER="${VERSION}")
add_definitions(-DMAYA_VER="${MAYA_VER}")
add_definitions(-DRMAN_VER="${RMAN_VER}")
add_definitions(-DUSD_VER="${USD_VER}")
add_definitions(-DREDSHIFT_VER="${REDSHIFT_VER}")
add_definitions(-DPUBLISH_ROOT="${PUBLISH_ROOT}")
#string(TIMESTAMP BUILD_TIME "%Y-%m-%d, %H:%M:%S")
#add_definitions(-DBUILD_TIME="${BUILD_TIME}")
#------------------------------------------------------------------- set gcc options
set(CMAKE_CXX_FLAGS             "-w -O3 -m64 -fPIC -fopenmp -std=c++1y -fext-numeric-literals")
set(CMAKE_CXX_FLAGS             "${CMAKE_CXX_FLAGS} -D_BOOL -DLINUX -DREQUIRE_IOSTREAM")
set(CMAKE_CXX_FLAGS_DEBUG       "-DDEBUG   -DENABLE_LOGGER=1")
set(CMAKE_CXX_FLAGS_RELEASE     "-DRELEASE -DENABLE_LOGGER=0")

#------------------------------------------------------------------- set nvcc options
set(CUDA_NVCC_FLAGS             "-w -O3 -m64 -arch=compute_61 -Xcompiler -fPIC -std=c++11 --expt-extended-lambda")
set(CUDA_NVCC_FLAGS             "${CUDA_NVCC_FLAGS} -D_BOOL -DLINUX -DREQUIRE_IOSTREAM")
set(CUDA_NVCC_FLAGS_DEBUG       "-DDEBUG   -DENABLE_LOGGER=1")
set(CUDA_NVCC_FLAGS_RELEASE     "-DRELEASE -DENABLE_LOGGER=0")

#------------------------------------------------------------------- set linking options
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#-------------------------------------------------------------------- add subdirectory(src)
add_subdirectory(src)

#--------------------------------------------------------------------- add publish command
add_custom_target(pub)
add_custom_command(TARGET pub
        COMMAND ${CMAKE_COMMAND} ../
                -DCMAKE_BUILD_TYPE=RELEASE
                -DMAYA_VER=2018
                -DRMAN_VER=22.3
                -DUSD_VER=19.01
                -DPUBLISH_ROOT=/netapp/backstage/pub/lib/${PROJECT_NAME}/${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        COMMAND make clean
        COMMAND make -j32
        )
