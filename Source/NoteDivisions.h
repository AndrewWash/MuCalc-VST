#pragma once
#include <array>
#include <juce_core/juce_core.h>

namespace MuCalc
{
    struct NoteRow
    {
        juce::String label;
        double quarterNoteMultiplier;
    };

    inline constexpr std::array<const char*, 3> columnLabels { "Straight", "Dotted", "Triplet" };

    inline const std::array<NoteRow, 7> noteRows {{
        { "1/1",  4.0     },
        { "1/2",  2.0     },
        { "1/4",  1.0     },
        { "1/8",  0.5     },
        { "1/16", 0.25    },
        { "1/32", 0.125   },
        { "1/64", 0.0625  }
    }};

    inline double straightMultiplier (double base) { return base; }
    inline double dottedMultiplier   (double base) { return base * 1.5; }
    inline double tripletMultiplier  (double base) { return base * (2.0 / 3.0); }

    inline double msFromQuarterMul (double bpm, double quarterMul)
    {
        if (bpm <= 0.0) return 0.0;
        return (60000.0 / bpm) * quarterMul;
    }

    inline double hzFromMs       (double ms) { return ms > 0.0 ? 1000.0 / ms : 0.0; }
    inline double samplesFromMs  (double ms, double sampleRate) { return ms * sampleRate / 1000.0; }

    enum class DisplayMode { Milliseconds = 0, Hertz = 1, Samples = 2 };

    inline juce::String formatValue (double ms, DisplayMode mode, double sampleRate)
    {
        switch (mode)
        {
            case DisplayMode::Milliseconds:
                return juce::String (ms, 2) + " ms";
            case DisplayMode::Hertz:
            {
                const double hz = hzFromMs (ms);
                return hz >= 100.0 ? juce::String (hz, 1) + " Hz"
                                   : juce::String (hz, 2) + " Hz";
            }
            case DisplayMode::Samples:
                return juce::String (juce::roundToInt (samplesFromMs (ms, sampleRate))) + " smp";
        }
        return {};
    }

    inline juce::String rawNumeric (double ms, DisplayMode mode, double sampleRate)
    {
        switch (mode)
        {
            case DisplayMode::Milliseconds: return juce::String (ms, 4);
            case DisplayMode::Hertz:        return juce::String (hzFromMs (ms), 4);
            case DisplayMode::Samples:      return juce::String (juce::roundToInt (samplesFromMs (ms, sampleRate)));
        }
        return {};
    }
}
