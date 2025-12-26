#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include <BinaryData.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    Colour theme = Colour::fromRGB(55, 55, 55);
    Colour buttonColour = Colour::fromRGB(62, 61, 65);
    Colour textColour = Colours::white;
    
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbCache{100};

    DJAudioPlayer player1{formatManager}, player2{formatManager}, player3{formatManager};

    DeckGUI deckGUI1{&player1, formatManager, thumbCache}, deckGUI2{&player2, formatManager, thumbCache};

    PlaylistComponent playlistComponent{&player3, &deckGUI1, &deckGUI2};

    MixerAudioSource mixerSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
