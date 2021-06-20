# $Id: $
#
# - Try to find the ASIO SDK
# Once done this will define
#
#  ASIOSDK_FOUND - system has ASIO SDK
#  ASIOSDK_ROOT_DIR - path to the ASIO SDK base directory
#  ASIOSDK_INCLUDE_DIR - the ASIO SDK include directory

if(WIN32)
else(WIN32)
  message(FATAL_ERROR "FindASIOSDK.cmake: Unsupported platform ${CMAKE_SYSTEM_NAME}" )
endif(WIN32)

find_path(ASIOSDK_ROOT_DIR
  common/asio.h
  HINTS
  ${SHURIKEN_ROOT_DIR}/asiosdk
)

find_path(ASIOSDK_INCLUDE_DIR
  asio.h
  PATHS
    ${ASIOSDK_ROOT_DIR}/common 
)  

# handle the QUIETLY and REQUIRED arguments and set ASIOSDK_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ASIOSDK DEFAULT_MSG ASIOSDK_ROOT_DIR ASIOSDK_INCLUDE_DIR)

mark_as_advanced(
    ASIOSDK_ROOT_DIR ASIOSDK_INCLUDE_DIR
)