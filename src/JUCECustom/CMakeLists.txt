# Super stripped down CMakeLists from JUCE's original, just to support the functionality needed.

cmake_minimum_required(VERSION 3.12)

project(JUCE VERSION 6.0.8 LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

set(REAL_JUCE_DIR "${JUCE_SOURCE_DIR}/../JUCE")
set(JUCE_MODULES_DIR "${REAL_JUCE_DIR}/modules")
set(JUCE_CUSTOM_MODULES_DIR "${JUCE_SOURCE_DIR}/modules")

include(JUCEUtils.cmake)

juce_disable_default_flags()

set(CMAKE_CXX_EXTENSIONS FALSE)

find_package(ASIOSDK REQUIRED)
juce_add_modules(
	INSTALL_PATH "include/JUCE-${JUCE_VERSION}/modules"
	ALIAS_NAMESPACE juce
	"${JUCE_MODULES_DIR}/juce_audio_basics"
	# "${JUCE_MODULES_DIR}/juce_audio_devices"
	"${JUCE_CUSTOM_MODULES_DIR}/juce_audio_devices_jackmidi"
	"${JUCE_MODULES_DIR}/juce_core"
	"${JUCE_MODULES_DIR}/juce_data_structures"
	"${JUCE_MODULES_DIR}/juce_events")
	
add_library(JUCECustom STATIC)
target_include_directories(JUCECustom
	INTERFACE
		${JUCE_MODULES_DIR}
		${JUCE_CUSTOM_MODULES_DIR})
target_link_libraries(JUCECustom
	PRIVATE
		juce::juce_audio_basics
		juce::juce_audio_devices_jackmidi
		juce::juce_core
		juce::juce_data_structures
		juce::juce_events
		juce::juce_recommended_config_flags
		juce::juce_recommended_warning_flags)

# Set up DEBUG, NDEBUG, JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED macros on things that link to this.
# Don't plan to do much interference otherwise.
_juce_add_standard_defs(JUCECustom)
