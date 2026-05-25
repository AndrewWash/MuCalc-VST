#include "DivisionTableComponent.h"

DivisionTableComponent::DivisionTableComponent()
{
    setInterceptsMouseClicks (true, false);
}

void DivisionTableComponent::setBpm (double newBpm)
{
    if (! juce::approximatelyEqual (newBpm, bpm))
    {
        bpm = newBpm;
        repaint();
    }
}

void DivisionTableComponent::setSampleRate (double sr)
{
    if (! juce::approximatelyEqual (sr, sampleRate))
    {
        sampleRate = sr;
        repaint();
    }
}

void DivisionTableComponent::setDisplayMode (MuCalc::DisplayMode m)
{
    if (m != mode)
    {
        mode = m;
        repaint();
    }
}

double DivisionTableComponent::mulFor (int row, int col) const
{
    const double base = MuCalc::noteRows[(size_t) row].quarterNoteMultiplier;
    switch (col)
    {
        case 0: return MuCalc::straightMultiplier (base);
        case 1: return MuCalc::dottedMultiplier   (base);
        case 2: return MuCalc::tripletMultiplier  (base);
    }
    return base;
}

juce::Rectangle<float> DivisionTableComponent::cellRect (int row, int col) const
{
    const auto bounds = getLocalBounds().toFloat();
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();
    const float headerH = (float) headerHeight;
    const float labelW = (float) labelColumnWidth;

    const int numRows = (int) MuCalc::noteRows.size();
    const float rowH = (h - headerH) / (float) numRows;
    const float colW = (w - labelW) / (float) MuCalc::columnLabels.size();

    return { labelW + (float) col * colW,
             headerH + (float) row * rowH,
             colW,
             rowH };
}

int DivisionTableComponent::cellRowAt (juce::Point<int> p) const
{
    if (p.y < headerHeight) return -1;
    const int numRows = (int) MuCalc::noteRows.size();
    const float rowH = (float) (getHeight() - headerHeight) / (float) numRows;
    const int row = (int) ((float) (p.y - headerHeight) / rowH);
    return juce::isPositiveAndBelow (row, numRows) ? row : -1;
}

int DivisionTableComponent::cellColAt (juce::Point<int> p) const
{
    if (p.x < labelColumnWidth) return -1;
    const float colW = (float) (getWidth() - labelColumnWidth) / (float) MuCalc::columnLabels.size();
    const int col = (int) ((float) (p.x - labelColumnWidth) / colW);
    return juce::isPositiveAndBelow (col, (int) MuCalc::columnLabels.size()) ? col : -1;
}

void DivisionTableComponent::mouseUp (const juce::MouseEvent& e)
{
    const int row = cellRowAt (e.getPosition());
    const int col = cellColAt (e.getPosition());
    if (row < 0 || col < 0) return;

    const double mul = mulFor (row, col);
    const double ms  = MuCalc::msFromQuarterMul (bpm, mul);

    juce::SystemClipboard::copyTextToClipboard (
        MuCalc::rawNumeric (ms, mode, sampleRate));

    flashRow = row;
    flashCol = col;
    flashTicks = 6;
    startTimerHz (30);
    repaint();
}

void DivisionTableComponent::timerCallback()
{
    if (--flashTicks <= 0)
    {
        stopTimer();
        flashRow = -1;
        flashCol = -1;
    }
    repaint();
}

void DivisionTableComponent::resized() {}

void DivisionTableComponent::paint (juce::Graphics& g)
{
    auto& lf = juce::LookAndFeel::getDefaultLookAndFeel();
    const auto bg     = lf.findColour (juce::ResizableWindow::backgroundColourId);
    const auto fg     = lf.findColour (juce::Label::textColourId);
    const auto faint  = fg.withAlpha (0.45f);
    const auto accent = juce::Colour (0xffadd8e6);

    g.fillAll (bg);

    const auto bounds = getLocalBounds().toFloat();
    const float w = bounds.getWidth();
    const float headerH = (float) headerHeight;
    const float labelW = (float) labelColumnWidth;
    const float colW = (w - labelW) / (float) MuCalc::columnLabels.size();

    g.setColour (fg);
    g.setFont (juce::Font (juce::FontOptions (14.0f)).boldened());
    for (int c = 0; c < (int) MuCalc::columnLabels.size(); ++c)
    {
        juce::Rectangle<float> headerRect (labelW + (float) c * colW, 0.0f, colW, headerH);
        g.drawText (MuCalc::columnLabels[(size_t) c], headerRect, juce::Justification::centred);
    }

    g.setColour (faint);
    g.drawHorizontalLine ((int) headerH - 1, 0.0f, w);

    const int numRows = (int) MuCalc::noteRows.size();
    for (int r = 0; r < numRows; ++r)
    {
        auto labelRect = juce::Rectangle<float> (0.0f,
                                                 headerH + (float) r * (bounds.getHeight() - headerH) / (float) numRows,
                                                 labelW,
                                                 (bounds.getHeight() - headerH) / (float) numRows);
        g.setColour (fg);
        g.setFont (juce::Font (juce::FontOptions (14.0f)).boldened());
        g.drawText (MuCalc::noteRows[(size_t) r].label, labelRect, juce::Justification::centred);

        for (int c = 0; c < (int) MuCalc::columnLabels.size(); ++c)
        {
            auto cell = cellRect (r, c);

            const bool isFlashing = (r == flashRow && c == flashCol);
            if (isFlashing)
            {
                g.setColour (accent.withAlpha (0.35f));
                g.fillRect (cell.reduced (1.0f));
            }

            g.setColour (faint);
            g.drawRect (cell, 0.5f);

            const double mul = mulFor (r, c);
            const double ms  = MuCalc::msFromQuarterMul (bpm, mul);

            g.setColour (fg);
            g.setFont (juce::Font (juce::FontOptions (13.0f)));
            g.drawText (MuCalc::formatValue (ms, mode, sampleRate),
                        cell.reduced (4.0f, 0.0f),
                        juce::Justification::centred);
        }
    }
}
