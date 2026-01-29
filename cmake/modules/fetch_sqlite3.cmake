cmake_minimum_required(VERSION 3.16)

include(FetchContent)

function(fetch_sqlite3)
	cmake_policy(SET CMP0135 NEW) # To avoid warnings
	FetchContent_Declare(
  		sqlite3
  		URL https://www.sqlite.org/2024/sqlite-amalgamation-3450100.zip 
	)
	FetchContent_MakeAvailable(sqlite3)

	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
	target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
endfunction()
