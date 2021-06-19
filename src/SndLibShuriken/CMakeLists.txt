cmake_minimum_required(VERSION 3.15)
project(SndLib)

add_library(SndLib STATIC)

target_sources(SndLib
	PRIVATE
		headers.c
		io.c
		sound.c)
target_include_directories(SndLib PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

find_package(GSL REQUIRED)
target_link_libraries(SndLib PRIVATE GSL::gsl GSL::gslcblas)
