cmake_minimum_required(VERSION 3.15)

project(shuriken)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)

# macro(print_all_variables)
    # message(STATUS "print_all_variables------------------------------------------{")
    # get_cmake_property(_variableNames VARIABLES)
    # foreach (_variableName ${_variableNames})
        # message(STATUS "${_variableName}=${${_variableName}}")
    # endforeach()
    # message(STATUS "print_all_variables------------------------------------------}")
# endmacro()

# print_all_variables()
