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

# Find system dependencies
find_package(Threads REQUIRED)
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module NumPy)

# Fetch dependencies
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

# Fetch main Xmipp repository
cmake_policy(SET CMP0135 NEW) # To avoid warnings
FetchContent_Declare(
    xmipp_core_sources
    URL https://github.com/I2PC/xmippCore/archive/refs/tags/v4.0.0.tar.gz
)	
FetchContent_Declare(
    xmipp3_sources
	URL https://github.com/I2PC/xmipp3/archive/refs/tags/v5.0.0-Beta-gal.tar.gz
)
FetchContent_Populate(xmipp_core_sources)
FetchContent_Populate(xmipp3_sources)

# Set project-wide compilation options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get FFTW include dir
FetchContent_GetProperties(fftw3_double SOURCE_DIR FFTW_SOURCE_DIR)
set(FFTW_INCLUDE_DIR ${FFTW_SOURCE_DIR}/api)

# Register all source and header files
set(XMIPP_CORE_DIR ${xmipp_core_sources_SOURCE_DIR})
set(XMIPP3_LIBRARY_DIR ${xmipp3_sources_SOURCE_DIR}/src/xmipp/libraries)
file(
	GLOB_RECURSE 
	SOURCES 
        ${XMIPP_CORE_DIR}/core/*.cpp
        ${XMIPP_CORE_DIR}/core/*.cc
        ${XMIPP_CORE_DIR}/core/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
)
list(
    APPEND
    SOURCES
        ${XMIPP3_LIBRARY_DIR}/reconstruction/ctf_enhance_psd.cpp
        ${XMIPP3_LIBRARY_DIR}/reconstruction/ctf_estimate_from_micrograph.cpp
        ${XMIPP3_LIBRARY_DIR}/reconstruction/ctf_estimate_from_psd_base.cpp
        ${XMIPP3_LIBRARY_DIR}/reconstruction/ctf_estimate_from_psd.cpp
        ${XMIPP3_LIBRARY_DIR}/reconstruction/ctf_estimate_from_psd_fast.cpp
        ${XMIPP3_LIBRARY_DIR}/reconstruction/ctf_estimate_psd_with_arma.cpp
        ${XMIPP3_LIBRARY_DIR}/reconstruction/fringe_processing.cpp
        ${XMIPP3_LIBRARY_DIR}/reconstruction/psd_estimator.cpp
        ${XMIPP3_LIBRARY_DIR}/data/basic_pca.cpp
        ${XMIPP3_LIBRARY_DIR}/data/basis.cpp
        ${XMIPP3_LIBRARY_DIR}/data/blobs.cpp
        ${XMIPP3_LIBRARY_DIR}/data/cpu.cpp
        ${XMIPP3_LIBRARY_DIR}/data/ctf.cpp
        ${XMIPP3_LIBRARY_DIR}/data/fft_settings.cpp
        ${XMIPP3_LIBRARY_DIR}/data/fftwT.cpp
        ${XMIPP3_LIBRARY_DIR}/data/filters.cpp
        ${XMIPP3_LIBRARY_DIR}/data/fourier_filter.cpp
        ${XMIPP3_LIBRARY_DIR}/data/fourier_projection.cpp
        ${XMIPP3_LIBRARY_DIR}/data/grids.cpp
        ${XMIPP3_LIBRARY_DIR}/data/integration.cpp
        ${XMIPP3_LIBRARY_DIR}/data/mask.cpp
        ${XMIPP3_LIBRARY_DIR}/data/morphology.cpp
        ${XMIPP3_LIBRARY_DIR}/data/normalize.cpp
        ${XMIPP3_LIBRARY_DIR}/data/numerical_tools.cpp
        ${XMIPP3_LIBRARY_DIR}/data/polar.cpp
        ${XMIPP3_LIBRARY_DIR}/data/projection.cpp
        ${XMIPP3_LIBRARY_DIR}/data/splines.cpp
        ${XMIPP3_LIBRARY_DIR}/data/wavelet.cpp
        ${XMIPP3_LIBRARY_DIR}/data/xmipp_image_over.cpp
        ${XMIPP3_LIBRARY_DIR}/data/xmipp_polynomials.cpp
)

# Create the shared library
add_library(xmippLib MODULE ${SOURCES})
target_link_libraries(
    xmippLib 
    PRIVATE 
        Threads::Threads
        Python::Module
        Python::NumPy
        sqlite3
        HDF5::CXX
        HDF5::HDF5
        FFTW3::Double
		FFTW3::Float
		FFTW3::DoubleThreads
		FFTW3::FloatThreads
        JPEG::JPEG
        TIFF::tiff
)
target_include_directories(
    xmippLib 
    PRIVATE 
        ${XMIPP_CORE_DIR}
        ${XMIPP3_LIBRARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${FFTW_INCLUDE_DIR}
)
set_target_properties(xmippLib 
    PROPERTIES
        PREFIX ""
        OUTPUT_NAME "xmippLib"
)

# Install library's binary files and headers
install(
	TARGETS xmippLib
	LIBRARY 
	    DESTINATION "${PYTHON_INSTALL_DIR}"
)

# Install Python scripts
install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/python/
	DESTINATION "${PYTHON_INSTALL_DIR}/"
)
