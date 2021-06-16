# Example Console App CMakeLists.txt

# To get started on a new console app, copy this entire folder (containing this file and C++
# sources) to a convenient location, and then start making modifications. For other examples of
# CMakeLists for console apps, check `extras/BinaryBuilder` and `extras/UnitTestRunner` in the JUCE
# repo.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.15)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(shuriken VERSION 0.0.1)
find_package(PkgConfig REQUIRED)
add_subdirectory(JUCE)
add_subdirectory(SndLibShuriken)

# `juce_add_console_app` adds an executable target with the name passed as the first argument
# (ConsoleAppExample here). This target is a normal CMake target, but has a lot of extra properties
# set up by default. This function accepts many optional arguments. Check the readme at
# `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_console_app(shuriken
    PRODUCT_NAME "Shuriken")     # The name of the final executable, which can differ from the target name
	
# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into the build tree. This header should be included with `#include <JuceHeader.h>`. The include
# path for this header will be automatically added to the target. The main function of the
# JuceHeader is to include all the JUCE module headers for a particular target; if you're happy to
# include module headers directly, you probably don't need to call this.

juce_generate_juce_header(shuriken)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

set_property(TARGET shuriken PROPERTY AUTOMOC ON)
set_property(TARGET shuriken PROPERTY AUTORCC ON)
set_property(TARGET shuriken PROPERTY AUTOUIC ON)

target_sources(shuriken
    PRIVATE
        consoleapp-demo.cpp)
		
# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(shuriken
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_console_app` call
        JUCE_USE_CURL=0)    # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_console_app` call
		
# If the target needs extra binary assets, they can be added here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when the target is built.

# juce_add_binary_data(ConsoleAppData SOURCES ...)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_core` module. Inter-module dependencies are
# resolved automatically. If you'd generated a binary data target above, you would need to link to
# it here too. This is a standard CMake command.

pkg_check_modules(AUBIO REQUIRED IMPORTED_TARGET aubio)
pkg_check_modules(LIBLO REQUIRED IMPORTED_TARGET liblo)
pkg_check_modules(RUBBERBAND REQUIRED IMPORTED_TARGET rubberband)
pkg_check_modules(SAMPLERATE REQUIRED IMPORTED_TARGET samplerate)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

target_link_libraries(shuriken
    PRIVATE
        # ConsoleAppData            # If you'd created a binary data target, you'd link to it here
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        SndLib
        PkgConfig::AUBIO
        PkgConfig::LIBLO
        PkgConfig::RUBBERBAND
        PkgConfig::SAMPLERATE
        PkgConfig::SNDFILE

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

set_property(TARGET shuriken PROPERTY POSITION_INDEPENDENT_CODE FALSE)

