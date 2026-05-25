#include "PluginProcessor.h"
#include "PluginEditor.h"

MuCalcProcessor::MuCalcProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, juce::Identifier ("MuCalcState"), makeLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MuCalcProcessor::makeLayout()
{
    using namespace juce;

    AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<AudioParameterFloat> (
        ParameterID { "bpm", 1 },
        "BPM",
        NormalisableRange<float> (20.0f, 999.0f, 0.01f),
        120.0f,
        AudioParameterFloatAttributes()
            .withLabel ("BPM")
            .withStringFromValueFunction ([] (float v, int) { return juce::String (v, 2); })));

    layout.add (std::make_unique<AudioParameterBool> (
        ParameterID { "syncHost", 1 },
        "Sync to Host",
        true));

    layout.add (std::make_unique<AudioParameterChoice> (
        ParameterID { "displayMode", 1 },
        "Display",
        StringArray { "ms", "Hz", "samples" },
        0));

    return layout;
}

void MuCalcProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate.store (sampleRate, std::memory_order_relaxed);
}

bool MuCalcProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainIn  = layouts.getMainInputChannelSet();
    const auto& mainOut = layouts.getMainOutputChannelSet();
    if (mainIn != mainOut) return false;
    return mainOut == juce::AudioChannelSet::mono()
        || mainOut == juce::AudioChannelSet::stereo();
}

void MuCalcProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    if (auto* ph = getPlayHead())
    {
        if (auto pos = ph->getPosition())
        {
            if (auto bpmOpt = pos->getBpm())
            {
                hostBpm.store (*bpmOpt, std::memory_order_relaxed);
                hostBpmValid.store (true, std::memory_order_relaxed);
            }
        }
    }

    juce::ignoreUnused (buffer);
}

double MuCalcProcessor::getEffectiveBpm() const noexcept
{
    if (isUsingHostBpm())
        return hostBpm.load (std::memory_order_relaxed);

    return static_cast<double> (apvts.getRawParameterValue ("bpm")->load());
}

bool MuCalcProcessor::isUsingHostBpm() const noexcept
{
    const bool sync = apvts.getRawParameterValue ("syncHost")->load() > 0.5f;
    return sync && hostBpmValid.load (std::memory_order_relaxed);
}

juce::AudioProcessorEditor* MuCalcProcessor::createEditor()
{
    return new MuCalcEditor (*this);
}

void MuCalcProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        if (auto xml = state.createXml())
            copyXmlToBinary (*xml, destData);
    }
}

void MuCalcProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MuCalcProcessor();
}
