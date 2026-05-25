cmake_minimum_required(VERSION 3.22)

project(MuCalc VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "" FORCE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Configure into a separate build directory, e.g. cmake -B build")
endif()

add_subdirectory(JUCE)
add_subdirectory(clap-juce-extensions EXCLUDE_FROM_ALL)

juce_add_plugin(MuCalc
    COMPANY_NAME               "LearningLab"
    PLUGIN_MANUFACTURER_CODE   Llab
    PLUGIN_CODE                MCal
    BUNDLE_ID                  "com.learninglab.mucalc"
    FORMATS                    VST3 AU
    PRODUCT_NAME               "MuCalc"
    DESCRIPTION                "BPM / note-time calculator"
    IS_SYNTH                   FALSE
    NEEDS_MIDI_INPUT           FALSE
    NEEDS_MIDI_OUTPUT          FALSE
    IS_MIDI_EFFECT             FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD    TRUE
    VST3_CATEGORIES            "Fx" "Tools"
    AU_MAIN_TYPE               "kAudioUnitType_Effect"
)

clap_juce_extensions_plugin(TARGET MuCalc
    CLAP_ID         "com.learninglab.mucalc"
    CLAP_FEATURES   audio-effect utility
)

target_sources(MuCalc PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DivisionTableComponent.cpp
)

target_include_directories(MuCalc PRIVATE Source)

target_compile_definitions(MuCalc PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=1
)

target_link_libraries(MuCalc PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
