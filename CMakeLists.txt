cmake_minimum_required(VERSION 3.18)

# Define the project
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION PROJECT_VERSION)
string(STRIP "${PROJECT_VERSION}" PROJECT_VERSION)
project(
    pyxmipp3
	VERSION ${PROJECT_VERSION}
	LANGUAGES C CXX
)

# Include CTest to provide BUILD_TESTING option
include(FetchContent)
include(GNUInstallDirs)

# Set the module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(Threads REQUIRED)
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module NumPy)

include(fetch_hdf5)
include(fetch_sqlite3)
include(fetch_fftw3)
include(fetch_libtiff)
include(fetch_libjpeg)

fetch_hdf5()
fetch_sqlite3()
fetch_fftw3()
fetch_libtiff()
fetch_libjpeg()

if(SKBUILD_PLATLIB_DIR)
	set(PYTHON_INSTALL_DIR "${SKBUILD_PLATLIB_DIR}")
else()
	set(PYTHON_INSTALL_DIR "${Python_SITEARCH}")
endif()

# Set project-wide compilation options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add sources
add_subdirectory(src)
