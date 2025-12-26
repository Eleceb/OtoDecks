/*
  ==============================================================================

    PlaylistComponent.h
    Created: 15 Feb 2023 12:42:38am
    Author:  Eleceb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include <iostream>
#include <fstream>

//==============================================================================
/*
*/
class PlaylistComponent  : public Component,
                           public TableListBoxModel,
                           public Button::Listener,
                           //Listener for TextEditor
                           public TextEditor::Listener,
                           //For file drag and drop
                           public FileDragAndDropTarget,
                           //For Timer
                           public Timer
    
{
public:
    PlaylistComponent(
        DJAudioPlayer* player,
        DeckGUI* deckGUI1,
        DeckGUI* deckGUI2
    );
    ~PlaylistComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    int getNumRows() override;

    void paintRowBackground (
        Graphics&, 
        int rowNumber, 
        int width, 
        int height, 
        bool rowIsSelected
    ) override;

    void paintCell (
        Graphics&, 
        int rowNumber, 
        int columnId, 
        int width, 
        int height, 
        bool rowIsSelected
    ) override;

    Component* refreshComponentForCell(
        int rowNumber, 
        int columnId, 
        bool isRowSelected, 
        Component* existingComponentToUpdate
    ) override;

    void buttonClicked(Button* button) override;

    //Functions added
    void filesChooser();

    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    void removeTrack(int id);

    void searchTracks();

    void sortOrderChanged(
        int newSortColumnId,
        bool isForwards
    ) override;

    std::vector<std::string> alphabaticallySort(std::vector<std::string> trackDetails);

    template <typename A, typename B>
    void zip(
        const std::vector<A>& a,
        const std::vector<B>& b,
        std::vector<std::pair<A, B>>& zipped);

    template <typename A, typename B>
    void unzip(
        const std::vector<std::pair<A, B>>& zipped,
        std::vector<A>& a,
        std::vector<B>& b
    );

    void timerCallback() override;

    void savePlaylist();
    void loadPlaylist(Colour buttonColours, Colour textColours);

    std::string secondsToTrackLength(int totalTimeInSeconds);

private:
    TextButton addButton{"ADD TO MUSIC LIBRARY"}, searchButton{"SEARCH"};

    TextEditor searchTextField;

    FileChooser fChooser{"Select a file..."};

    DJAudioPlayer* player;

    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;

    TableListBox tableComponent;
    std::vector<std::string> trackTitles, trackExtensions, trackLengths, trackPaths;

    std::string songPlaying;
    int songPlayingId;

    int startSearchSongIndex;

    Colour buttonColour, textColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};