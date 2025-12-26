/*
  ==============================================================================

    DeckGUI.h
    Created: 14 Feb 2023 12:10:39am
    Author:  Eleceb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include <BinaryData.h>

//==============================================================================
/*
*/
class DeckGUI  : public Component,
                 public Button::Listener,
                 public Slider::Listener,
                 public FileDragAndDropTarget
{
public:
    DeckGUI(
        DJAudioPlayer* player,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse
    );
    ~DeckGUI() override;

    void paint (Graphics&) override;
    void resized() override;

    /** implement Button::Listener */
    void buttonClicked (Button* button) override;

    /** implement Slider::Listener */
    void sliderValueChanged (Slider* slider) override;

    void sliderDragEnded(Slider* slider) override; //Functions added

    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    //Functions added
    void trackStatusPaint();

    void movePlayheadTenSeconds(bool isForward);

    void sentIsRepeatOnToDeckGUI();

    void fileChooser();

    void libraryChooser(URL file);

    std::string secondsToTrackLength(int totalTimeInSeconds);

private:
    ImageButton playButton, stopButton, backwardButton, forwardButton, repeatButton;

    TextButton loadButton{"LOAD"};

    Slider volSlider, speedSlider, posSlider;

    Label trackLabel, volLabel, volNameLabel, speedLabel, speedNameLabel, posLabel, posLengthLabel;

    Image playButtonImage = ImageFileFormat::loadFrom(
        BinaryData::play_png,
        BinaryData::play_pngSize
    );
    Image pauseButtonImage = ImageFileFormat::loadFrom(
        BinaryData::pause_png,
        BinaryData::pause_pngSize
    );
    Image restartButtonImage = ImageFileFormat::loadFrom(
        BinaryData::restart_png,
        BinaryData::restart_pngSize
    );
    Image stopButtonImage = ImageFileFormat::loadFrom(
        BinaryData::stop_png,
        BinaryData::stop_pngSize
    );
    Image repeatButtonImage = ImageFileFormat::loadFrom(
        BinaryData::repeat_png,
        BinaryData::repeat_pngSize
    );
    Image backwardButtonImage = ImageFileFormat::loadFrom(
        BinaryData::backward_png,
        BinaryData::backward_pngSize
    );
    Image forwardButtonImage = ImageFileFormat::loadFrom(
        BinaryData::forward_png,
        BinaryData::forward_pngSize
    );

    FileChooser fChooser{"Select a file..."};

    DJAudioPlayer* player;

    WaveformDisplay waveformDisplay;

    bool isPlayPressed, isRepeatOn, isTrackEnded;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
