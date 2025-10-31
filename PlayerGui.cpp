#include "PlayerGui.h"
#include <JuceHeader.h>

/*
 * PlayerGUI.cpp
 * -----------------------------
 * Implements the visual part of the player (UI behavior).
*/


PlayerGui::PlayerGui()
{
    formatManager.registerBasicFormats();




    // Buttons
    // NOTE: If you add new buttons to this list, make sure to update the array size
    // (currently 12). The size must match the number of elements inside the initializer
    // list to avoid runtime or compilation issues.
    for (auto* btn : std::array<juce::Button*, 12>{
        &loadButton, &playButton, &stopButton,
        &goStartButton, &goEndButton, &muteButton,
        &loopButton, &setAButton, &setBButton,
        &loopABButton, &skip5sec, &dec5sec
        })
    {
        addAndMakeVisible(btn);
        btn->addListener(this);
    }




    // Volume
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    addAndMakeVisible(volumeSlider);

    // Metadata label (Made by Adham Tamer)
    addAndMakeVisible(metadataLabel);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(playlistBox);
    playlistBox.setModel(this);

    // position slider (by omar)
    positionSlider.setRange(0, 1.0);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.addListener(this);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    addAndMakeVisible(positionSlider);

    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    addAndMakeVisible(timeLabel);

    // Set initial volume and start the timer for the GUI updates
    playerAudio.setGain((float)volumeSlider.getValue());
    startTimerHz(20); // Update GUI 20 times per second
}

PlayerGui::~PlayerGui()
{
    playerAudio.releaseResources();
}

void PlayerGui::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Simple Player", getLocalBounds(), juce::Justification::centredTop);


}

void PlayerGui::resized()
{
    int margin = 20;
    int y = 40;
    int gap = 10;
    int buttonWidth = 80;
    int buttonHeight = 35;

    int x = margin;

    loadButton.setBounds(x, y, buttonWidth, buttonHeight);  x += buttonWidth + gap;
    playButton.setBounds(x, y, buttonWidth, buttonHeight);  x += buttonWidth + gap;
    stopButton.setBounds(x, y, buttonWidth, buttonHeight);  x += buttonWidth + gap;

    // ===== Skip Buttons =====
    skip5sec.setBounds(x, y, buttonWidth, buttonHeight);  x += buttonWidth + gap;
    dec5sec.setBounds(x, y, buttonWidth, buttonHeight);   x += buttonWidth + gap;

    // ===== Existing Buttons after =====
    goStartButton.setBounds(x, y, buttonWidth, buttonHeight);  x += buttonWidth + gap;
    goEndButton.setBounds(x, y, buttonWidth, buttonHeight);    x += buttonWidth + gap;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight);     x += buttonWidth + gap;
    loopButton.setBounds(x, y, buttonWidth, buttonHeight);     x += buttonWidth + gap;
    setAButton.setBounds(x, y, buttonWidth, buttonHeight);     x += buttonWidth + gap;
    setBButton.setBounds(x, y, buttonWidth, buttonHeight);     x += buttonWidth + gap;
    loopABButton.setBounds(x, y, buttonWidth, buttonHeight);

    // ===== Sliders & Labels =====
    volumeSlider.setBounds(margin, y + 60, 250, 25);
    positionSlider.setBounds(margin, y + 100, getWidth() - 2 * margin, 25);
    timeLabel.setBounds(margin, y + 125, getWidth() - 2 * margin, 25);
    metadataLabel.setBounds(margin, y + 160, getWidth() - 2 * margin, 60);
    playlistBox.setBounds(margin, y + 230, getWidth() - 2 * margin, getHeight() - (y + 250));
}


void PlayerGui::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGui::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() != nullptr)
        playerAudio.getNextAudioBlock(bufferToFill);
    else
        bufferToFill.clearActiveBufferRegion();
}

void PlayerGui::releaseResources()
{

    playerAudio.releaseResources();
}

void PlayerGui::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file != juce::File{})
                {
                    playerAudio.loadFile(file);
                    fileLoaded = true;
                    auto* reader = formatManager.createReaderFor(file);
                    if (reader != nullptr)
                    {
                        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                        readerSource.reset(newSource.release());
                        // Reset GUI state for new file
                        loopABButton.setToggleState(false, juce::dontSendNotification);
                    }
                    playlistFiles.add(file.getFullPathName());
                    playlistBox.updateContent();
                }
            });
    }

    if (button == &playButton)
    {

        playerAudio.togglePlayPause();


        if (playerAudio.isPlaying()) {
            playButton.setButtonText("Pause");
        }
        else {
            playButton.setButtonText("Play");
        }
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
        playButton.setButtonText("Play");
    }


    else if (button == &goStartButton)
        playerAudio.setPosition(0.0);
    else if (button == &goEndButton)
        playerAudio.setPosition(transportSource.getLengthInSeconds());


    else if (button == &muteButton)
    {
        playerAudio.muteornot();
        muteButton.setButtonText(playerAudio.readifmute() ? "Unmute" : "Mute");
    }
    else if (button == &loopButton) {
        loopClicked();
    }
    // --- A-B Loop Button Logic ---
    else if (button == &setAButton) { playerAudio.setLoopStartPoint(); }
    else if (button == &setBButton) { playerAudio.setLoopEndPoint(); }
    else if (button == &loopABButton)
    {
        playerAudio.setABLooping(loopABButton.getToggleState());
    }
    else if (button == &skip5sec)
    {
        double currentPos = playerAudio.getPosition();
        playerAudio.setPosition(currentPos + 5.0); // skip +5s
    }

    else if (button == &dec5sec)
    {
        double currentPos = playerAudio.getPosition();
        double newPos = currentPos - 5.0;
        if (newPos < 0) newPos = 0; // prevent negative values

        playerAudio.setPosition(newPos); // skip -5s
    }


}

//=============== OMAR =================
void PlayerGui::loopClicked()
{
    bool newState = !playerAudio.getLooping(); // toggle properly
    playerAudio.setLooping(newState);
    loopButton.setButtonText(newState ? "Loop: ON" : "Loop: OFF");
}


void PlayerGui::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)slider->getValue());
    }
}
void PlayerGui::sliderDragStarted(juce::Slider* slider) {
    if (slider == &positionSlider)
        isDraggingPosition = true;
}

void PlayerGui::sliderDragEnded(juce::Slider* slider) {
    if (slider == &positionSlider) {
        if (playerAudio.getLength() > 0.0) {
            playerAudio.setPosition(slider->getValue() * playerAudio.getLength());
        }
        isDraggingPosition = false;
    }
}

void PlayerGui::timerCallback()
{
    double pos = playerAudio.getPosition();
    double len = playerAudio.getLength();

    if (len > 0.0 && !isDraggingPosition)
    {
        positionSlider.setValue(pos / len, juce::dontSendNotification);
    }

    auto formatTime = [](double seconds) {
        int total = static_cast<int>(std::round(seconds));
        return juce::String::formatted("%02d:%02d", total / 60, total % 60);
        };

    timeLabel.setText(formatTime(pos) + " / " + formatTime(len), juce::dontSendNotification);
}



//============Adham================
int PlayerGui::getNumRows()
{
    return playlistFiles.size();
}

void PlayerGui::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::darkblue);
    else
        g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.drawText(playlistFiles[rowNumber], 5, 0, width, height, juce::Justification::centredLeft);
}
void PlayerGui::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    auto file = juce::File(playlistFiles[row]);


    if (playerAudio.loadFile(file))
    {
        playerAudio.play();
    }


    if (auto* reader = formatManager.createReaderFor(file))
    {
        juce::String title = reader->metadataValues.getValue("title", file.getFileNameWithoutExtension());
        juce::String artist = reader->metadataValues.getValue("artist", "Unknown Artist");
        double duration = reader->lengthInSamples / reader->sampleRate;

        metadataLabel.setText(
            "Title: " + title +
            "\nArtist: " + artist +
            "\nDuration: " + juce::String(duration, 2) + " sec",
            juce::dontSendNotification
        );

        delete reader;
    }
}
