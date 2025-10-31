#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGui
    :public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ListBoxModel,
    public juce::Timer
{
public:
    PlayerGui();
    ~PlayerGui() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    // Audio setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    // Player activation
    void setPlayerID(int id) { playerID = id; }
    int getPlayerID() const { return playerID; }

    bool isPlaying() const
    {
        return transportSource.isPlaying();
    }

private:
    // GUI controls

    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop: OFF" };
    juce::TextButton skip5sec{ "+5>>" };
    juce::TextButton dec5sec{ "-5<<" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::ToggleButton loopABButton{ "A-B Loop" };
    juce::TextButton goStartButton{ "Start<<" };
    juce::TextButton goEndButton{ ">>End" };

    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Label timeLabel;
    juce::Label metadataLabel;

    PlayerAudio playerAudio;
    bool fileLoaded = false;
    bool isDraggingPosition = false;
    int playerID = 0;

    // Audio engine
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::ListBox playlistBox;
    juce::StringArray playlistFiles;

    // Activation callback


    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderDragStarted(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void loopClicked();

    // ListBox methods
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)
};