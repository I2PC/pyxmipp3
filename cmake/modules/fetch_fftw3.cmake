cmake_minimum_required(VERSION 3.16)

include(FetchContent)

function(fetch_fftw3)
	set(FFTW3_URL https://fftw.org/fftw-3.3.10.tar.gz)

	cmake_policy(SET CMP0135 NEW) # To avoid warnings
	FetchContent_Declare(
        fftw3_double
		URL ${FFTW3_URL}
	)

	set(POSITION_INDEPENDENT_CODE ON)
	set(BUILD_SHARED_LIBS OFF)
	set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(ENABLE_THREADS ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(fftw3_double)

	FetchContent_Declare(
        fftw3_float
		URL ${FFTW3_URL}
	)

	set(ENABLE_FLOAT ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(fftw3_float)
endfunction()
