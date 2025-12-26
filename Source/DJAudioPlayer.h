/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 13 Feb 2023 10:28:37pm
    Author:  Eleceb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DJAudioPlayer : public AudioSource
{
public:
    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio, bool isPlayPressed, bool isTrackEnded);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    void start();
    void stop();

    /** get the relative position of the playhead */
    double getPositionRelative();

    //Functions added
    double getCurrentPosition();

    //Get length in seconds for forward and backward buttons in DeckGUI to call
    double getLengthInSeconds();

    //See if the track is playing for forward, backward, and repeat buttons in DeckGUI to call
    bool isPlaying();

    bool hasStreamFinished();

    
private:
    double ratioNow;
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{&transportSource, false, 2};
};