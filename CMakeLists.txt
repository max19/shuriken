cmake_minimum_required(VERSION 3.15)

project(shuriken VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(SHURIKEN_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "Shuriken project directory.")

# add_subdirectory(src/JUCE)
add_subdirectory(src/JUCECustom)

add_executable(shuriken)

set_property(TARGET shuriken PROPERTY AUTOMOC ON)
set_property(TARGET shuriken PROPERTY AUTORCC ON)
set_property(TARGET shuriken PROPERTY AUTOUIC ON)

target_compile_options(shuriken
        PRIVATE
    # juce::juce_recommended_warning_flags with some of the most annoying taken out.
    -Wall -Wextra -Wstrict-aliasing -Wuninitialized -Wunused-parameter
    -Wsign-compare -Woverloaded-virtual -Wreorder -Wsign-conversion
    -Wunreachable-code -Wcast-align -Wredundant-decls -Wno-implicit-fallthrough
    -Wno-maybe-uninitialized -Wno-missing-field-initializers -Wno-ignored-qualifiers 
    # -Wpedantic -Wswitch-enum -Wzero-as-null-pointer-constant

    # More annoying stuff.
    -Wno-parentheses
    )
    
add_subdirectory(src)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

pkg_check_modules(AUBIO REQUIRED IMPORTED_TARGET aubio)
pkg_check_modules(FFTW3F REQUIRED IMPORTED_TARGET fftw3F)
pkg_check_modules(RUBBERBAND REQUIRED IMPORTED_TARGET rubberband)
pkg_check_modules(SAMPLERATE REQUIRED IMPORTED_TARGET samplerate)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

target_link_libraries(shuriken
    PRIVATE
        JUCECustom
        Qt5::Core
        Qt5::Widgets
        Qt5::OpenGL
        OpenGL::GL
        SndLib
        PkgConfig::AUBIO
        PkgConfig::FFTW3F
        PkgConfig::RUBBERBAND
        PkgConfig::SAMPLERATE
        PkgConfig::SNDFILE

    PUBLIC
        juce::juce_recommended_config_flags
        )

if (UNIX)
    pkg_check_modules(JACK REQUIRED IMPORTED_TARGET jack)
    pkg_check_modules(LIBLO REQUIRED IMPORTED_TARGET liblo)
    target_link_libraries(shuriken PRIVATE PkgConfig::JACK PkgConfig::LIBLO)
endif()

target_sources(shuriken
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

target_include_directories(shuriken
        PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET shuriken PROPERTY POSITION_INDEPENDENT_CODE FALSE)
set_property(TARGET shuriken PROPERTY WIN32_EXECUTABLE TRUE)

# macro(print_all_variables)
    # message(STATUS "print_all_variables------------------------------------------{")
    # get_cmake_property(_variableNames VARIABLES)
    # foreach (_variableName ${_variableNames})
        # message(STATUS "${_variableName}=${${_variableName}}")
    # endforeach()
    # message(STATUS "print_all_variables------------------------------------------}")
# endmacro()

# print_all_variables()
