cmake_minimum_required(VERSION 3.16)

include(FetchContent)

function(fetch_libtiff)
	cmake_policy(SET CMP0135 NEW) # To avoid warnings
	FetchContent_Declare(
        libtiff
		URL https://download.osgeo.org/libtiff/tiff-4.7.1.tar.gz
	)

	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(BUILD_SHARED_LIBS OFF)
	set(tiff-install OFF)
	set(tiff-docs OFF)
	FetchContent_MakeAvailable(libtiff)
endfunction()
