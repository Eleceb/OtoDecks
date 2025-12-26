/*
  ==============================================================================

    DeckGUI.cpp
    Created: 14 Feb 2023 12:10:39am
    Author:  Eleceb

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(
    DJAudioPlayer* _player,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse
) :
    player(_player),
    waveformDisplay(formatManagerToUse, cacheToUse),
    isPlayPressed(false),
    isRepeatOn(false),
    isTrackEnded(false)
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(backwardButton);
    addAndMakeVisible(forwardButton);
    addAndMakeVisible(repeatButton);
    addAndMakeVisible(loadButton);

    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    addAndMakeVisible(trackLabel);
    addAndMakeVisible(volNameLabel);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedNameLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posLengthLabel);

    trackLabel.setText("File not loaded...", dontSendNotification);
    trackLabel.setJustificationType(Justification::centred);
    volNameLabel.setText("Volume:", dontSendNotification);
    speedNameLabel.setText("Speed:", dontSendNotification);
    posLabel.setText("00:00", dontSendNotification);
    posLabel.attachToComponent(&posSlider, true);
    posLengthLabel.setText("00:00", dontSendNotification);

    addAndMakeVisible(waveformDisplay);

    playButton.addListener(this);
    stopButton.addListener(this);
    backwardButton.addListener(this);
    forwardButton.addListener(this);
    repeatButton.addListener(this);
    loadButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.1);
    volSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    speedSlider.setRange(0.0, 5.0);
    speedSlider.setValue(1.0);
    speedSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    posSlider.setRange(0.0, 1.0);
    posSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
}

DeckGUI::~DeckGUI()
{

}

void DeckGUI::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    float buttonOpacity = 0.65f;

    stopButton.setImages(
        false, true, false, 
        stopButtonImage, buttonOpacity, {},
        stopButtonImage, 1.0f, {}, 
        stopButtonImage, buttonOpacity, {}
    );
    backwardButton.setImages(
        false, true, false, 
        backwardButtonImage, buttonOpacity, {},
        backwardButtonImage, 1.0f, {}, 
        backwardButtonImage, buttonOpacity, {}
    );
    forwardButton.setImages(
        false, true, false, 
        forwardButtonImage, buttonOpacity, {},
        forwardButtonImage, 1.0f, {}, 
        forwardButtonImage, buttonOpacity, {}
    );

    if (isTrackEnded && !isRepeatOn)
    {
        playButton.setImages(
            false, true, false,
            restartButtonImage, buttonOpacity, {},
            restartButtonImage, 1.0f, {},
            restartButtonImage, buttonOpacity, {}
        );
    }
    else if (!isPlayPressed)
    {
        playButton.setImages(
            false, true, false, 
            playButtonImage, buttonOpacity, {},
            playButtonImage, 1.0f, {}, 
            playButtonImage, buttonOpacity, {}
        );
    }
    else
    {
        playButton.setImages(
            false, true, false,
            pauseButtonImage, buttonOpacity, {},
            pauseButtonImage, 1.0f, {},
            pauseButtonImage, buttonOpacity, {}
        );
    }

    if (isRepeatOn) 
    {
        repeatButton.setImages(
            false, true, false,
            repeatButtonImage, 1.0f, Colour::fromRGB(58, 154, 95),
            repeatButtonImage, 1.0f, Colour::fromRGB(58, 184, 95),
            repeatButtonImage, 1.0f, Colour::fromRGB(58, 154, 95)
        );
    }
    else 
    {
        repeatButton.setImages(
            false, true, false,
            repeatButtonImage, buttonOpacity, {},
            repeatButtonImage, 1.0f, {},
            repeatButtonImage, buttonOpacity, {}
        );
    }

    if (player->getLengthInSeconds() != NULL)
    {
        trackStatusPaint();
    }

    int roundedVol = int(volSlider.getValue() * 100);

    volLabel.setText(
        std::to_string(roundedVol) + "%",
        dontSendNotification
    );

    int roundedSpeed = int(speedSlider.getValue() * 100);

    speedLabel.setText(
        std::to_string(roundedSpeed) + "%",
        dontSendNotification
    );
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 8;
    double sliderP = getWidth() / 6;
    trackLabel.setBounds(0, 0, getWidth(), rowH);
    volNameLabel.setBounds(0, rowH, sliderP, rowH);
    volLabel.setBounds(sliderP, rowH, sliderP, rowH);
    volSlider.setBounds(sliderP * 1.7, rowH, getWidth() - sliderP * 1.7, rowH);
    speedNameLabel.setBounds(0, rowH * 2, sliderP, rowH);
    speedLabel.setBounds(sliderP, rowH * 2, sliderP, rowH);
    speedSlider.setBounds(sliderP * 1.7, rowH * 2, getWidth() - sliderP * 1.7, rowH);
    waveformDisplay.setBounds(0, rowH * 3, getWidth(), rowH * 2);
    posSlider.setBounds(sliderP, rowH * 5, getWidth() - sliderP * 2, rowH);
    posLengthLabel.setBounds(posSlider.getRight(), rowH * 5, getWidth() - posSlider.getRight(), rowH);
    repeatButton.setBounds(getWidth() * 0.15 - rowH / 2, rowH * 6, rowH, rowH);
    backwardButton.setBounds(getWidth() * 0.35 - rowH / 2, rowH * 6, rowH, rowH);
    playButton.setBounds(getWidth() / 2 - rowH / 2, rowH * 6, rowH, rowH);
    forwardButton.setBounds(getWidth() * 0.65 - rowH / 2, rowH * 6, rowH, rowH);
    stopButton.setBounds(getWidth() * 0.85 - rowH / 2, rowH * 6, rowH, rowH);
    loadButton.setBounds(0, rowH * 7.2, getWidth(), rowH * 0.925);
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        if (!isPlayPressed && player->getLengthInSeconds() != NULL || isTrackEnded)
        {
            isPlayPressed = true;
            if (isTrackEnded)
            {
                player->setPositionRelative(0);
                isTrackEnded = false;
            }
            if (speedSlider.getValue() != 0)
            {
                player->start();
            }
        }
        else
        {
            isPlayPressed = false;
            player->stop();
        }
    }
    else if (button == &stopButton && player->getLengthInSeconds() != NULL)
    {
        player->setPositionRelative(0);
        isPlayPressed = false;
        isTrackEnded = false;
        player->stop();
    }
    //Backward button
    else if (button == &backwardButton)
    {
        movePlayheadTenSeconds(false);
    }
    //Forward button
    else if (button == &forwardButton)
    {
        movePlayheadTenSeconds(true);
    }
    //Repeat button
    else if (button == &repeatButton)
    {
        isRepeatOn = !isRepeatOn;
    }
    else if (button == &loadButton)
    {
        fileChooser();
    }
}

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue(), isPlayPressed, isTrackEnded);
    }
}

void DeckGUI::sliderDragEnded(Slider* slider)
{
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
        if (isPlayPressed && isTrackEnded)
        {
            player->start();
        }
        isTrackEnded = false;
    }
}

bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    if (files.size() == 1)
    {
        File chosenFile = files[0];
        player->loadURL(URL{chosenFile});
        waveformDisplay.loadURL(URL{chosenFile});
        posLengthLabel.setText(
            secondsToTrackLength(player->getLengthInSeconds()),
            dontSendNotification
        );
        trackLabel.setText(chosenFile.getFileName(), dontSendNotification);
        posSlider.setValue(0);
        isPlayPressed = false;
        isTrackEnded = false;
    }
}

void DeckGUI::trackStatusPaint()
{
    if (!posSlider.isMouseButtonDown())
    {
        posSlider.setValue(player->getPositionRelative());
    }

    posLabel.setText(
        secondsToTrackLength(std::floor(player->getLengthInSeconds() * posSlider.getValue())),
        dontSendNotification
    );

    if (player->getPositionRelative() < 1)
    {
        waveformDisplay.setPositionRelative(
            player->getPositionRelative()
        );
    }
    else
    {
        waveformDisplay.setPositionRelative(0.9999999);
    }

    if (isRepeatOn)
    {
        sentIsRepeatOnToDeckGUI();
    }
    else if (player->getPositionRelative() > 0.999995)
    {
        isTrackEnded = true;
    }
}

//Forward and backward buttons
void DeckGUI::movePlayheadTenSeconds(bool isForward)
{
    if (isForward)
    {
        //+10s
        if (player->getCurrentPosition() <= player->getLengthInSeconds() - 10)
        {
            player->setPositionRelative(
                (player->getCurrentPosition() + 10) /
                player->getLengthInSeconds()
            );
        }
        else
        {
            player->setPositionRelative(1);
        }
    }
    //-10s
    else
    {
        if (player->getCurrentPosition() >= 10)
        {
            if (!isTrackEnded)
            {
                player->setPositionRelative(
                    (player->getCurrentPosition() - 10) /
                    player->getLengthInSeconds()
                );
            }
        }
        else
        {
            player->setPositionRelative(0);
        }
    }
}

void DeckGUI::sentIsRepeatOnToDeckGUI()
{
    if (player->hasStreamFinished() && isPlayPressed)
    {
        player->setPositionRelative(0);
        player->start();
    }
}

void DeckGUI::fileChooser()
{
    auto fileChooserFlags =
    FileBrowserComponent::canSelectFiles;
    fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
    {
        File chosenFile = chooser.getResult();
        if (!chosenFile.getFileName().isEmpty())
        {
            player->loadURL(URL{chosenFile});
            waveformDisplay.loadURL(URL{chosenFile});
            posLengthLabel.setText(
                secondsToTrackLength(player->getLengthInSeconds()),
                dontSendNotification
            );
            trackLabel.setText(chosenFile.getFileName(), dontSendNotification);
            posSlider.setValue(0);
            isPlayPressed = false;
            isTrackEnded = false;
        }
    });
}

void DeckGUI::libraryChooser(URL file)
{
    player->loadURL(URL{file});
    waveformDisplay.loadURL(URL{file});
    posLengthLabel.setText(
        secondsToTrackLength(player->getLengthInSeconds()),
        dontSendNotification
    );
    trackLabel.setText(URL{file}.getFileName(), dontSendNotification);
    posSlider.setValue(0);
    isPlayPressed = false;
    isTrackEnded = false;
}

std::string DeckGUI::secondsToTrackLength(int totalTimeInSeconds)
{
    int second, hour, minute;
    std::string seconds, hours, minutes;

    minute = totalTimeInSeconds / 60;
    second = totalTimeInSeconds % 60;
    hour = minute / 60;
    minute = minute % 60;

    hours = std::to_string(hour);
    minutes = std::to_string(minute);
    seconds = std::to_string(second);

    if (hour != 0 && hour < 10) {
        hours = "0" + hours;
    }
    if (minute < 10) {
        minutes = "0" + minutes;
    }
    if (second < 10)
    {
        seconds = "0" + seconds;
    }

    if (hour == 0)
    {
        return minutes + ":" + seconds;
    }
    else
    {
        return hours + ":" + minutes + ":" + seconds;
    }
}