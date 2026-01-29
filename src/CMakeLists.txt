cmake_minimum_required(VERSION 3.16)

# Register all source and header files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)


FetchContent_GetProperties(fftw3_double SOURCE_DIR FFTW_SOURCE_DIR)
set(FFTW_INCLUDE_DIR ${FFTW_SOURCE_DIR}/api)

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
        ${CMAKE_CURRENT_SOURCE_DIR}
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
