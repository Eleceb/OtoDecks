/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 14 Feb 2023 6:03:41pm
    Author:  Eleceb

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse
) :
    audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0)
{
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{

}

void WaveformDisplay::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colour::fromRGB(99, 148, 179));
    if (fileLoaded)
    {
        audioThumb.drawChannel(g,
            getLocalBounds(),
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f
        );
        g.setColour(Colours::red);
        g.drawRect(position * getWidth(), 0, 1, getHeight());
    }
    else
    {
        g.setFont(20.0f);
        g.drawText("File not loaded...", getLocalBounds(),
            Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{

}

void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    if (fileLoaded)
    {
        DBG("wfd: loaded!");
        repaint();
    }
    else
    {
        DBG("wfd: not loaded!");
    }
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    DBG("wfd: change received!");

    repaint();
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position && !isnan(pos))
    {
        position = pos;
        repaint();
    }
}