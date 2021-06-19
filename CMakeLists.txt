cmake_minimum_required(VERSION 3.15)

project(shuriken VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

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

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

# target_compile_definitions(shuriken
#     PRIVATE
#         # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
#         JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_console_app` call
#         JUCE_USE_CURL=0)    # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_console_app` call


add_subdirectory(src)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

pkg_check_modules(AUBIO REQUIRED IMPORTED_TARGET aubio)
pkg_check_modules(FFTW3F REQUIRED IMPORTED_TARGET fftw3F)
pkg_check_modules(JACK REQUIRED IMPORTED_TARGET jack)
pkg_check_modules(LIBLO REQUIRED IMPORTED_TARGET liblo)
pkg_check_modules(RUBBERBAND REQUIRED IMPORTED_TARGET rubberband)
pkg_check_modules(SAMPLERATE REQUIRED IMPORTED_TARGET samplerate)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

target_link_libraries(shuriken
    PRIVATE
        # ConsoleAppData            # If you'd created a binary data target, you'd link to it here
        JUCECustom
        Qt5::Core
        Qt5::Widgets
        Qt5::OpenGL
        OpenGL::GL
        SndLib
        PkgConfig::AUBIO
        PkgConfig::FFTW3F
        PkgConfig::JACK
        PkgConfig::LIBLO
        PkgConfig::RUBBERBAND
        PkgConfig::SAMPLERATE
        PkgConfig::SNDFILE

    PUBLIC
        juce::juce_recommended_config_flags
        )

target_sources(shuriken
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

target_include_directories(shuriken
        PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ASIOSDK_INCLUDE_DIR})            # Needed for ASIO in JUCE

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
