#pragma once

#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>

class MuCalcProcessor : public juce::AudioProcessor
{
public:
    MuCalcProcessor();
    ~MuCalcProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MuCalc"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    double getEffectiveBpm() const noexcept;
    double getCurrentSampleRate() const noexcept { return currentSampleRate.load(); }
    bool   isUsingHostBpm() const noexcept;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();

    std::atomic<double> hostBpm       { 120.0 };
    std::atomic<bool>   hostBpmValid  { false };
    std::atomic<double> currentSampleRate { 44100.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuCalcProcessor)
};
