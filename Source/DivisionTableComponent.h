#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "NoteDivisions.h"

class DivisionTableComponent : public juce::Component, private juce::Timer
{
public:
    DivisionTableComponent();
    ~DivisionTableComponent() override = default;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseUp (const juce::MouseEvent& e) override;

    void setBpm (double newBpm);
    void setSampleRate (double sr);
    void setDisplayMode (MuCalc::DisplayMode mode);

private:
    void timerCallback() override;

    double bpm        { 120.0 };
    double sampleRate { 44100.0 };
    MuCalc::DisplayMode mode { MuCalc::DisplayMode::Milliseconds };

    int flashRow { -1 };
    int flashCol { -1 };
    int flashTicks { 0 };

    juce::Rectangle<float> cellRect (int row, int col) const;
    int    cellRowAt (juce::Point<int> p) const;
    int    cellColAt (juce::Point<int> p) const;
    double mulFor    (int row, int col) const;

    static constexpr int headerHeight = 26;
    static constexpr int labelColumnWidth = 60;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DivisionTableComponent)
};
