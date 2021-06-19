include_guard(GLOBAL)
cmake_minimum_required(VERSION 3.12)

# ==================================================================================================

set(JUCE_CMAKE_UTILS_DIR ${CMAKE_CURRENT_LIST_DIR}
    CACHE INTERNAL "The path to the folder holding this file and other resources")

include("${JUCE_CMAKE_UTILS_DIR}/JUCEHelperTargets.cmake")

# ==================================================================================================

macro(_juce_make_absolute path)
    if(NOT IS_ABSOLUTE "${${path}}")
        get_filename_component("${path}" "${${path}}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
    endif()
endmacro()

macro(_juce_make_absolute_and_check path)
    _juce_make_absolute("${path}")

    if(NOT EXISTS "${${path}}")
        message(FATAL_ERROR "No file at path ${${path}}")
    endif()
endmacro()

# ==================================================================================================

function(_juce_module_sources module_path output_path built_sources other_sources)
    get_filename_component(module_parent_path ${module_path} DIRECTORY)
    get_filename_component(module_glob ${module_path} NAME)

    file(GLOB_RECURSE all_module_files
        CONFIGURE_DEPENDS LIST_DIRECTORIES FALSE
        RELATIVE "${module_parent_path}"
        "${module_path}/*")

    set(base_path "${module_glob}/${module_glob}")

    set(module_cpp ${all_module_files})
    list(FILTER module_cpp INCLUDE REGEX "^${base_path}[^/]*\\.cpp$")

    if(APPLE)
        set(module_mm ${all_module_files})
        list(FILTER module_mm INCLUDE REGEX "^${base_path}[^/]*\\.(mm|r)$")

        if(module_mm)
            set(module_mm_replaced ${module_mm})
            list(TRANSFORM module_mm_replaced REPLACE "\\.mm$" ".cpp")
            list(REMOVE_ITEM module_cpp ${module_mm_replaced})
            list(APPEND module_cpp ${module_mm})
        endif()
    endif()

    set(headers ${all_module_files})

    if(NOT module_cpp STREQUAL "")
        list(REMOVE_ITEM headers ${module_cpp})
    endif()

    foreach(source_list IN ITEMS module_cpp headers)
        list(TRANSFORM ${source_list} PREPEND "${output_path}/")
    endforeach()

    set(${built_sources} ${module_cpp} PARENT_SCOPE)
    set(${other_sources} ${headers} PARENT_SCOPE)
endfunction()

# ==================================================================================================

function(_juce_add_interface_library target)
    add_library(${target} INTERFACE)
    target_sources(${target} INTERFACE ${ARGN})
endfunction()

# ==================================================================================================

function(_juce_add_standard_defs juce_target)
    target_compile_definitions(${juce_target} INTERFACE
        JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
        $<IF:$<CONFIG:DEBUG>,DEBUG=1 _DEBUG=1,NDEBUG=1 _NDEBUG=1>)
endfunction()

# ==================================================================================================

# This creates an imported interface library with a random name, and then adds
# the fields from a JUCE module header to the target as INTERFACE_ properties.
# We can extract properties later using `_juce_get_metadata`.
# This way, the interface library ends up behaving a bit like a dictionary,
# and we don't have to parse the module header from scratch every time we
# want to find a specific key.
function(_juce_extract_metadata_block delim_str file_with_block out_dict)
    string(RANDOM LENGTH 16 random_string)
    set(target_name "${random_string}_dict")
    set(${out_dict} "${target_name}" PARENT_SCOPE)
    add_library(${target_name} INTERFACE IMPORTED)

    if(NOT EXISTS ${file_with_block})
        message(FATAL_ERROR "Unable to find file ${file_with_block}")
    endif()

    file(STRINGS ${file_with_block} module_header_contents)

    set(last_written_key)
    set(append NO)

    foreach(line IN LISTS module_header_contents)
        if(NOT append)
            if(line MATCHES " *BEGIN_${delim_str} *")
                set(append YES)
            endif()

            continue()
        endif()

        if(append AND (line MATCHES " *END_${delim_str} *"))
            break()
        endif()

        if(line MATCHES "^ *([a-zA-Z]+):")
            set(last_written_key "${CMAKE_MATCH_1}")
        endif()

        string(REGEX REPLACE "^ *${last_written_key}: *" "" line "${line}")
        string(REGEX REPLACE "[ ,]+" ";" line "${line}")

        set_property(TARGET ${target_name} APPEND PROPERTY
            "INTERFACE_JUCE_${last_written_key}" "${line}")
    endforeach()
endfunction()

# Fetches properties attached to a metadata target.
function(_juce_get_metadata target key out_var)
    get_target_property(content "${target}" "INTERFACE_JUCE_${key}")

    if(NOT "${content}" STREQUAL "content-NOTFOUND")
        set(${out_var} "${content}" PARENT_SCOPE)
    endif()
endfunction()

# Takes a target, a link visibility, and a variable-length list of framework
# names. On macOS, finds the requested frameworks using `find_library` and
# links them.
function(_juce_link_frameworks target visibility)
    foreach(framework IN LISTS ARGN)
        if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
            find_library("juce_found_${framework}" "${framework}" REQUIRED)
            target_link_libraries("${target}" "${visibility}" "${juce_found_${framework}}")
        endif()
    endforeach()
endfunction()

# ==================================================================================================

function(_juce_link_libs_from_metadata module_name dict key)
    _juce_get_metadata("${dict}" "${key}" libs)

    if(libs)
        target_link_libraries(${module_name} INTERFACE ${libs})
    endif()
endfunction()

# ==================================================================================================

function(juce_add_module module_path)
    set(one_value_args INSTALL_PATH ALIAS_NAMESPACE)
    cmake_parse_arguments(JUCE_ARG "" "${one_value_args}" "" ${ARGN})

    _juce_make_absolute(module_path)

    get_filename_component(module_name ${module_path} NAME)
    get_filename_component(module_parent_path ${module_path} DIRECTORY)

    set(module_header_name "${module_name}.h")

    if(NOT EXISTS "${module_path}/${module_header_name}")
        set(module_header_name "${module_header_name}pp")
    endif()

    if(NOT EXISTS "${module_path}/${module_header_name}")
        message(FATAL_ERROR "Could not locate module header for module '${module_path}'")
    endif()

    set(base_path "${module_parent_path}")

    _juce_module_sources("${module_path}" "${base_path}" globbed_sources headers)
	
	list(APPEND all_module_sources ${globbed_sources})

    _juce_add_interface_library(${module_name} ${all_module_sources})

    set_property(GLOBAL APPEND PROPERTY _juce_module_names ${module_name})

    set_target_properties(${module_name} PROPERTIES
        INTERFACE_JUCE_MODULE_SOURCES   "${globbed_sources}"
        INTERFACE_JUCE_MODULE_HEADERS   "${headers}"
        INTERFACE_JUCE_MODULE_PATH      "${base_path}")

    if(JUCE_ENABLE_MODULE_SOURCE_GROUPS)
        target_sources(${module_name} INTERFACE ${headers})
    endif()

    if(${module_name} STREQUAL "juce_core")
        _juce_add_standard_defs(${module_name})
    endif()

    target_include_directories(${module_name} INTERFACE ${base_path})

    target_compile_definitions(${module_name} INTERFACE JUCE_MODULE_AVAILABLE_${module_name}=1)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_definitions(${module_name} INTERFACE LINUX=1)
    endif()

    _juce_extract_metadata_block(JUCE_MODULE_DECLARATION "${module_path}/${module_header_name}" metadata_dict)

    _juce_get_metadata("${metadata_dict}" minimumCppStandard module_cpp_standard)

    if(module_cpp_standard)
        target_compile_features(${module_name} INTERFACE cxx_std_${module_cpp_standard})
    else()
        target_compile_features(${module_name} INTERFACE cxx_std_11)
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        _juce_get_metadata("${metadata_dict}" OSXFrameworks module_osxframeworks)

        foreach(module_framework IN LISTS module_osxframeworks)
            if(module_framework STREQUAL "")
                continue()
            endif()

            _juce_link_frameworks("${module_name}" INTERFACE "${module_framework}")
        endforeach()

        _juce_link_libs_from_metadata("${module_name}" "${metadata_dict}" OSXLibs)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        _juce_get_metadata("${metadata_dict}" linuxPackages module_linuxpackages)

        if(module_linuxpackages)
            _juce_create_pkgconfig_target(${module_name}_LINUX_DEPS ${module_linuxpackages})
            target_link_libraries(${module_name} INTERFACE juce::pkgconfig_${module_name}_LINUX_DEPS)
        endif()

        _juce_link_libs_from_metadata("${module_name}" "${metadata_dict}" linuxLibs)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
            if(module_name STREQUAL "juce_gui_basics")
                target_compile_options(${module_name} INTERFACE /bigobj)
            endif()

            _juce_link_libs_from_metadata("${module_name}" "${metadata_dict}" windowsLibs)
        elseif(MSYS OR MINGW)
            if(module_name STREQUAL "juce_gui_basics")
                target_compile_options(${module_name} INTERFACE "-Wa,-mbig-obj")
            endif()

            _juce_link_libs_from_metadata("${module_name}" "${metadata_dict}" mingwLibs)
        endif()
    endif()

    _juce_get_metadata("${metadata_dict}" dependencies module_dependencies)
    target_link_libraries(${module_name} INTERFACE ${module_dependencies})

    _juce_get_metadata("${metadata_dict}" searchpaths module_searchpaths)
    if(NOT module_searchpaths STREQUAL "")
        foreach(module_searchpath IN LISTS module_searchpaths)
            target_include_directories(${module_name}
                INTERFACE "${module_path}/${module_searchpath}")
        endforeach()
    endif()

    if(JUCE_ARG_INSTALL_PATH)
        install(DIRECTORY "${module_path}" DESTINATION "${JUCE_ARG_INSTALL_PATH}")
    endif()

    if(JUCE_ARG_ALIAS_NAMESPACE)
        add_library(${JUCE_ARG_ALIAS_NAMESPACE}::${module_name} ALIAS ${module_name})
    endif()
endfunction()

function(juce_add_modules)
    set(one_value_args INSTALL_PATH ALIAS_NAMESPACE)
    cmake_parse_arguments(JUCE_ARG "" "${one_value_args}" "" ${ARGN})

    foreach(path IN LISTS JUCE_ARG_UNPARSED_ARGUMENTS)
        juce_add_module(${path}
            INSTALL_PATH "${JUCE_ARG_INSTALL_PATH}"
            ALIAS_NAMESPACE "${JUCE_ARG_ALIAS_NAMESPACE}")
    endforeach()
endfunction()

# ==================================================================================================

function(juce_disable_default_flags)
    set(langs C CXX)
    set(modes DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)

    foreach(lang IN LISTS langs)
        foreach(mode IN LISTS modes)
            list(FILTER CMAKE_${lang}_FLAGS_${mode} INCLUDE REGEX "[/-]M[TD]d?")
            set(CMAKE_${lang}_FLAGS_${mode} "${CMAKE_${lang}_FLAGS_${mode}}" PARENT_SCOPE)
        endforeach()
    endforeach()
endfunction()

# ==================================================================================================
function(juce_configure_target target)

endfunction()

