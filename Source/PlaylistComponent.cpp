/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 15 Feb 2023 12:42:38am
    Author:  Eleceb

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(
    DJAudioPlayer* _player,
    DeckGUI* _deckGUI1,
    DeckGUI* _deckGUI2
) :
    player(_player),
    deckGUI1(_deckGUI1),
    deckGUI2(_deckGUI2),
    songPlaying(),
    songPlayingId(NULL),
    startSearchSongIndex(0)
{
    addAndMakeVisible(addButton);
    addButton.addListener(this);

    addAndMakeVisible(searchTextField);
    searchTextField.addListener(this);
    searchTextField.setFont(20.0f);
    searchTextField.setTextToShowWhenEmpty("Search...", Colours::grey);
    searchTextField.setJustification(Justification::centredLeft);
    searchTextField.setColour(Label::backgroundColourId, buttonColour);
    searchTextField.onTextChange = [this] {startSearchSongIndex = 0;};

    addAndMakeVisible(searchButton);
    searchButton.addListener(this);

    tableComponent.getHeader().addColumn("Track title", 1, 250);
    tableComponent.getHeader().addColumn("Ext", 2, 60);
    tableComponent.getHeader().addColumn("Length", 3, 90);
    tableComponent.getHeader().addColumn("", 4, 100);
    tableComponent.getHeader().addColumn("", 5, 100);
    tableComponent.getHeader().addColumn("", 6, 100);
    tableComponent.getHeader().addColumn("", 7, 100);
    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);
    
    startTimer(500);
}

PlaylistComponent::~PlaylistComponent()
{
    stopTimer();
    tableComponent.setModel(nullptr);
}

void PlaylistComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
}

void PlaylistComponent::resized()
{
    double rowH = getHeight() / 6;
    addButton.setBounds(0, 0, getWidth(), rowH);
    searchTextField.setBounds(0, rowH, getWidth() / 2, rowH);
    searchButton.setBounds(getWidth() / 2, rowH, getWidth() / 2, rowH);
    tableComponent.setBounds(0, rowH * 2, getWidth(), rowH * 4);
}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

void PlaylistComponent::paintRowBackground(
    Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected
)
{
    // just highlight selected rows
    if (rowIsSelected)
    {
        g.fillAll(Colour::fromRGB(99, 148, 179));
    }
    else
    {
        g.fillAll(Colours::darkgrey);
    }
}

void PlaylistComponent::paintCell(
    Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected
)
{
    g.setColour(textColour);
    if (columnId == 1)
    {
        g.drawText(
            trackTitles[rowNumber],
            2,
            0,
            width - 4,
            height,
            Justification::centredLeft,
            true
        );
    }
    if (columnId == 2)
    {
        g.drawText(
            trackExtensions[rowNumber],
            2,
            0,
            width - 4,
            height,
            Justification::centredLeft,
            true
        );
    }
    if (columnId == 3)
    {
        g.drawText(
            trackLengths[rowNumber],
            2,
            0,
            width - 4,
            height,
            Justification::centredLeft,
            true
        );
    }
}

Component* PlaylistComponent::refreshComponentForCell(
    int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate
)
{
    if (existingComponentToUpdate == nullptr)
    {
        if (columnId == 4)
        {
            TextButton* btn = new TextButton("preview");
            String id{std::to_string(rowNumber)};
            btn->setComponentID(id);
            btn->setName("preview");
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
        if (columnId == 5)
        {
            TextButton* btn = new TextButton("player1");
            String id{std::to_string(rowNumber)};
            btn->setComponentID(id);
            btn->setName("player1");
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
        if (columnId == 6)
        {
            TextButton* btn = new TextButton("player2");
            String id{std::to_string(rowNumber)};
            btn->setComponentID(id);
            btn->setName("player2");
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
        if (columnId == 7)
        {
            TextButton* btn = new TextButton("remove");
            String id{std::to_string(rowNumber)};
            btn->setComponentID(id);
            btn->setName("remove");
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    //Add button
    if (button == &addButton)
    {
        filesChooser();
    }
    //Search button
    else if (button == &searchButton)
    {
        searchTracks();
    }
    //Preview button
    else if (button->getName() == "preview")
    {
        int id = std::stoi(button->getComponentID().toStdString());
        
        if (songPlaying != trackTitles[id] && songPlayingId != id || !player->isPlaying())
        {
            if (player->isPlaying())
            {
                tableComponent.getCellComponent(4, songPlayingId)->setColour(TextButton::buttonColourId, buttonColour);
            }
            button->setColour(TextButton::buttonColourId, Colours::blue);
            player->loadURL(URL{File{trackPaths[id]}});
            player->start();
        }
        else
        {
            button->setColour(TextButton::buttonColourId, buttonColour);
            player->stop();
        }

        songPlaying = trackTitles[id];
        songPlayingId = id;
    }
    //Player1 button
    else if (button->getName() == "player1")
    {
        int id = std::stoi(button->getComponentID().toStdString());
        deckGUI1->libraryChooser(URL{File{trackPaths[id]}});
    }
    //Player2 button
    else if (button->getName() == "player2")
    {
        int id = std::stoi(button->getComponentID().toStdString());
        deckGUI2->libraryChooser(URL{File{trackPaths[id]}});
    }
    //Remove button
    else if (button->getName() == "remove")
    {
        int id = std::stoi(button->getComponentID().toStdString());
        removeTrack(id);
    }
}

void PlaylistComponent::filesChooser()
{
    auto fileChooserFlags =
        FileBrowserComponent::canSelectFiles;
    auto fileChooserFlags2 =
        FileBrowserComponent::canSelectMultipleItems;
    fChooser.launchAsync(fileChooserFlags2 | fileChooserFlags, [this](const FileChooser& chooser)
    {
        for (int i = 0; i < chooser.getResults().size(); ++i)
        {
            File chosenFile = chooser.getResults()[i];
            if (!chosenFile.getFileNameWithoutExtension().isEmpty())
            {
                trackTitles.push_back(chosenFile.getFileNameWithoutExtension().toStdString());
                trackExtensions.push_back(chosenFile.getFileExtension().toStdString());
                trackPaths.push_back(chosenFile.getFullPathName().toStdString());
                player->loadURL(URL{chosenFile});
                trackLengths.push_back(secondsToTrackLength(player->getLengthInSeconds()));
            }
        }
        tableComponent.updateContent();
        savePlaylist();
    });
}

bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    for (int i = 0; i < files.size(); ++i)
    {
        File chosenFile = files[i];
        trackTitles.push_back(chosenFile.getFileNameWithoutExtension().toStdString());
        trackExtensions.push_back(chosenFile.getFileExtension().toStdString());
        trackPaths.push_back(chosenFile.getFullPathName().toStdString());
        player->loadURL(URL{chosenFile});
        trackLengths.push_back(secondsToTrackLength(player->getLengthInSeconds()));
    }
    tableComponent.updateContent();
    savePlaylist();
}

void PlaylistComponent::removeTrack(int id)
{
    trackTitles.erase(trackTitles.begin() + id);
    trackExtensions.erase(trackExtensions.begin() + id);
    trackPaths.erase(trackPaths.begin() + id);
    trackLengths.erase(trackLengths.begin() + id);
    if (player->isPlaying())
    {
        if (id == songPlayingId)
        {
            player->stop();
            tableComponent.getCellComponent(4, songPlayingId)->setColour(TextButton::buttonColourId, buttonColour);
        }
        else if (songPlayingId > id && songPlayingId < trackTitles.size())
        {
            tableComponent.getCellComponent(4, songPlayingId)->setColour(TextButton::buttonColourId, buttonColour);
            tableComponent.getCellComponent(4, songPlayingId - 1)->setColour(TextButton::buttonColourId, Colours::blue);
            --songPlayingId;
        }
        else if (songPlayingId >= trackTitles.size())
        {
            tableComponent.getCellComponent(4, songPlayingId - 1)->setColour(TextButton::buttonColourId, Colours::blue);
            --songPlayingId;
        }
    }
    tableComponent.deselectAllRows();
    tableComponent.updateContent();
    startSearchSongIndex = 0;
    savePlaylist();
}

void PlaylistComponent::searchTracks()
{
    if (searchTextField.getText().isNotEmpty())
    {
        int i = startSearchSongIndex;

        int endSearchSongIndex = startSearchSongIndex - 1;

        if (endSearchSongIndex < 0)
        {
            endSearchSongIndex = trackTitles.size() - 1;
        }

        while (startSearchSongIndex != endSearchSongIndex && !trackTitles.empty() || trackTitles.size() == 1)
        {
            if (i >= trackTitles.size())
            {
                i = 0;
            }

            if (trackTitles[i].find(searchTextField.getText().toStdString()) != std::string::npos)
            {
                tableComponent.selectRow(i, false, true);
                startSearchSongIndex = i + 1;
                break;
            }
            else if (i == endSearchSongIndex)
            {
                tableComponent.deselectAllRows();
                break;
            }

            ++i;
        }
    }
    else
    {
        tableComponent.deselectAllRows();
    }
}

void PlaylistComponent::sortOrderChanged(
    int newSortColumnId,
    bool isForwards
)
{
    if (trackTitles.size() != 0)
    {
        if (newSortColumnId == 1)
        {
            trackTitles = alphabaticallySort(trackTitles);
        }
        else if (newSortColumnId == 3)
        {
            trackLengths = alphabaticallySort(trackLengths);
        }

        player->stop();
        if (songPlayingId != NULL || songPlayingId == 0)
        {
            tableComponent.getCellComponent(4, songPlayingId)->setColour(TextButton::buttonColourId, buttonColour);
        }
        tableComponent.deselectAllRows();
        tableComponent.updateContent();
        savePlaylist();
    }
}

std::vector<std::string> PlaylistComponent::alphabaticallySort(std::vector<std::string> trackDetails)
{
    // Zip the vectors together
    std::vector<std::pair<std::string, std::string>> zipped;
    zip(trackDetails, trackPaths, zipped);

    // Sort the vector of pairs
    if (tableComponent.getHeader().isSortedForwards())
    {
        std::sort(std::begin(zipped), std::end(zipped),
            [&](const auto& a, const auto& b)
            {
                return a.first < b.first;
            });
    }
    else
    {
        std::sort(std::begin(zipped), std::end(zipped),
            [&](const auto& a, const auto& b)
            {
                return a.first > b.first;
            });
    }
    
    // Write the sorted pairs back to the original vectors
    unzip(zipped, trackDetails, trackPaths);
    
    trackTitles.clear();
    trackExtensions.clear();
    trackLengths.clear();

    if (tableComponent.getHeader().getSortColumnId() == 1)
    {
        for (int i = 0; i < trackPaths.size(); ++i)
        {
            File chosenFile = trackPaths[i];
            trackExtensions.push_back(chosenFile.getFileExtension().toStdString());
            player->loadURL(URL{chosenFile});
            trackLengths.push_back(secondsToTrackLength(player->getLengthInSeconds()));
        }
    }
    else if (tableComponent.getHeader().getSortColumnId() == 3)
    {
        for (int i = 0; i < trackPaths.size(); ++i)
        {
            File chosenFile = trackPaths[i];
            trackTitles.push_back(chosenFile.getFileNameWithoutExtension().toStdString());
            trackExtensions.push_back(chosenFile.getFileExtension().toStdString());
        }
    }

    return trackDetails;
}

// Fill the zipped vector with pairs consisting of the
// corresponding elements of a and b. (This assumes 
// that the vectors have equal length)
template <typename A, typename B>
void PlaylistComponent::zip(
    const std::vector<A>& a,
    const std::vector<B>& b,
    std::vector<std::pair<A, B>>& zipped
)
{
    for (size_t i = 0; i < a.size(); ++i)
    {
        zipped.push_back(std::make_pair(a[i], b[i]));
    }
}

// Write the first and second element of the pairs in 
// the given zipped vector into a and b. (This assumes 
// that the vectors have equal length)
template <typename A, typename B>
void PlaylistComponent::unzip(
    const std::vector<std::pair<A, B>>& zipped,
    std::vector<A>& a,
    std::vector<B>& b
)
{
    for (size_t i = 0; i < a.size(); i++)
    {
        a[i] = zipped[i].first;
        b[i] = zipped[i].second;
    }
}

void PlaylistComponent::timerCallback()
{
    if (player->isPlaying())
    {
        if (player->hasStreamFinished())
        {
            tableComponent.getCellComponent(4, songPlayingId)->setColour(TextButton::buttonColourId, buttonColour);
        }
    }
}

void PlaylistComponent::savePlaylist()
{
    String playlist = File::getCurrentWorkingDirectory().getFullPathName() + "/TrackURLs.txt";
    std::ofstream trackURLs;
    trackURLs.open(playlist.toStdString());
    for (int i = 0; i < trackPaths.size(); ++i)
    {
        trackURLs << trackPaths[i] + "\n";
    }
    trackURLs.close();
}

void PlaylistComponent::loadPlaylist(Colour buttonColours, Colour textColours)
{
    buttonColour = buttonColours;
    textColour = textColours;
    String playlist = File::getCurrentWorkingDirectory().getFullPathName() + "/TrackURLs.txt";
    std::string line;
    std::ifstream trackURLs;
    trackURLs.open(playlist.toStdString());
    while (std::getline(trackURLs, line))
    {
        File chosenFile = line;
        trackTitles.push_back(chosenFile.getFileNameWithoutExtension().toStdString());
        trackExtensions.push_back(chosenFile.getFileExtension().toStdString());
        trackPaths.push_back(chosenFile.getFullPathName().toStdString());
        player->loadURL(URL{ chosenFile });
        trackLengths.push_back(secondsToTrackLength(player->getLengthInSeconds()));
    }
    trackURLs.close();

    tableComponent.updateContent();
}

std::string PlaylistComponent::secondsToTrackLength(int totalTimeInSeconds)
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