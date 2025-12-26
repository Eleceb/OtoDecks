/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 13 Feb 2023 10:28:37pm
    Author:  Eleceb

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager)
: formatManager(_formatManager), ratioNow(1)
{
    
}

DJAudioPlayer::~DJAudioPlayer()
{

}

//==============================================================================
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);
}

void DJAudioPlayer::releaseResources() 
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 1.0)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0% and 100%");
    }
    else
    {
        transportSource.setGain(gain);
    }
}

void DJAudioPlayer::setSpeed(double ratio, bool isPlayPressed, bool isTrackEnded)
{
    if (ratio < 0.0000001 || ratio > 5.0)
    {
        DBG("DJAudioPlayer::setSpeed ratio should be between 1% and 500%");
        if (ratio == 0)
        {
            ratioNow = ratio;
            transportSource.stop();
        }
    }
    else
    {
        resampleSource.setResamplingRatio(ratio);
        if (!transportSource.isPlaying())
        {
            ratioNow = ratio;
            if (isPlayPressed && !isTrackEnded)
            {
                transportSource.start();
            }
        }
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (pos < 0 || pos > 1.0)
    {
        DBG("DJAudioPlayer::setPositionRelative pos should be between 00:00 and song length");
    }
    else
    {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}


void DJAudioPlayer::start()
{
    transportSource.start();
}

void DJAudioPlayer::stop()
{
    transportSource.stop();
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

double DJAudioPlayer::getCurrentPosition()
{
    return transportSource.getCurrentPosition();
}

double DJAudioPlayer::getLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}

bool DJAudioPlayer::isPlaying()
{
    return transportSource.isPlaying();
}

bool DJAudioPlayer::hasStreamFinished()
{
    return transportSource.hasStreamFinished();
}