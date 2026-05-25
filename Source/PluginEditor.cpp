#include "PluginEditor.h"
#include "NoteDivisions.h"

MuCalcEditor::MuCalcEditor (MuCalcProcessor& p)
    : juce::AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible (bpmLabel);
    bpmLabel.setText ("BPM", juce::dontSendNotification);
    bpmLabel.setJustificationType (juce::Justification::centredLeft);
    bpmLabel.setFont (juce::Font (juce::FontOptions (16.0f)).boldened());

    addAndMakeVisible (bpmSlider);
    bpmSlider.setSliderStyle (juce::Slider::IncDecButtons);
    bpmSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 90, 28);
    bpmSlider.setRange (20.0, 999.0, 0.01);
    bpmSlider.setNumDecimalPlacesToDisplay (2);
    bpmSlider.setTextValueSuffix (" BPM");
    bpmSlider.setIncDecButtonsMode (juce::Slider::incDecButtonsDraggable_Vertical);

    addAndMakeVisible (bpmReadout);
    bpmReadout.setJustificationType (juce::Justification::centredRight);
    bpmReadout.setFont (juce::Font (juce::FontOptions (12.0f)));
    bpmReadout.setColour (juce::Label::textColourId, juce::Colours::lightgrey);

    addAndMakeVisible (syncButton);

    addAndMakeVisible (displayLabel);
    displayLabel.setText ("Show as", juce::dontSendNotification);
    displayLabel.setJustificationType (juce::Justification::centredLeft);

    addAndMakeVisible (displayCombo);
    displayCombo.addItem ("Milliseconds (ms)", 1);
    displayCombo.addItem ("Frequency (Hz)",    2);
    displayCombo.addItem ("Samples",           3);

    addAndMakeVisible (hintLabel);
    hintLabel.setText ("Click any cell to copy its value.", juce::dontSendNotification);
    hintLabel.setJustificationType (juce::Justification::centred);
    hintLabel.setFont (juce::Font (juce::FontOptions (11.0f)).italicised());
    hintLabel.setColour (juce::Label::textColourId, juce::Colours::lightgrey);

    addAndMakeVisible (table);

    bpmAttachment     = std::make_unique<APVTS::SliderAttachment>   (processor.apvts, "bpm", bpmSlider);
    syncAttachment    = std::make_unique<APVTS::ButtonAttachment>   (processor.apvts, "syncHost", syncButton);
    displayAttachment = std::make_unique<APVTS::ComboBoxAttachment> (processor.apvts, "displayMode", displayCombo);

    setResizable (true, true);
    setResizeLimits (320, 420, 720, 900);
    setSize (380, 540);

    updateBpmDisplay();
    startTimerHz (20);
}

void MuCalcEditor::updateBpmDisplay()
{
    const double effective = processor.getEffectiveBpm();
    const bool   usingHost = processor.isUsingHostBpm();

    if (! juce::approximatelyEqual (effective, lastEffectiveBpm))
    {
        lastEffectiveBpm = effective;
        table.setBpm (effective);
        table.setSampleRate (processor.getCurrentSampleRate());
    }

    bpmReadout.setText (usingHost ? "Following host" : "Manual",
                        juce::dontSendNotification);

    const int modeIdx = (int) processor.apvts.getRawParameterValue ("displayMode")->load();
    table.setDisplayMode (static_cast<MuCalc::DisplayMode> (modeIdx));

    bpmSlider.setEnabled (! usingHost);
}

void MuCalcEditor::timerCallback()
{
    updateBpmDisplay();
}

void MuCalcEditor::paint (juce::Graphics& g)
{
    auto& lf = juce::LookAndFeel::getDefaultLookAndFeel();
    g.fillAll (lf.findColour (juce::ResizableWindow::backgroundColourId));
}

void MuCalcEditor::resized()
{
    auto area = getLocalBounds().reduced (12);

    auto topRow = area.removeFromTop (34);
    bpmLabel.setBounds   (topRow.removeFromLeft (40));
    bpmReadout.setBounds (topRow.removeFromRight (110));
    bpmSlider.setBounds  (topRow);

    area.removeFromTop (6);
    syncButton.setBounds (area.removeFromTop (24));

    area.removeFromTop (6);
    auto displayRow = area.removeFromTop (26);
    displayLabel.setBounds (displayRow.removeFromLeft (70));
    displayCombo.setBounds (displayRow);

    area.removeFromTop (8);
    hintLabel.setBounds (area.removeFromBottom (18));

    area.removeFromTop (4);
    table.setBounds (area);
}
