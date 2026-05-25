#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "DivisionTableComponent.h"

class MuCalcEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit MuCalcEditor (MuCalcProcessor&);
    ~MuCalcEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateBpmDisplay();

    MuCalcProcessor& processor;

    juce::Label   bpmLabel;
    juce::Slider  bpmSlider;
    juce::Label   bpmReadout;
    juce::ToggleButton syncButton { "Sync to host tempo" };
    juce::Label   hostBpmLabel;
    juce::Label   displayLabel;
    juce::ComboBox displayCombo;
    juce::Label   hintLabel;
    DivisionTableComponent table;

    using APVTS = juce::AudioProcessorValueTreeState;
    std::unique_ptr<APVTS::SliderAttachment>   bpmAttachment;
    std::unique_ptr<APVTS::ButtonAttachment>   syncAttachment;
    std::unique_ptr<APVTS::ComboBoxAttachment> displayAttachment;

    double lastEffectiveBpm { -1.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuCalcEditor)
};
