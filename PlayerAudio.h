#pragma once
#include <JuceHeader.h>


/*
 * PlayerAudio.h
 * -----------------------------
 * Handles the AUDIO logic (backend) of the player.
 */

class PlayerAudio : juce::ChangeListener
{
private:
    static PlayerAudio* currentlyPlaying;
    bool isMuted = false;
    float savedGain = 1.0f;
    float currentGain = 1.0f;
    bool shouldLoop = false;//o
    double loopStartSeconds = 0.0;//o
    double loopEndSeconds = 0.0;//o
    bool isABLoopingActive = false;//o
    // Audio
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)

public:
    PlayerAudio();
    ~PlayerAudio();
    //Audio

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const juce::File& file);
    void stop();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void muteornot();
    bool readifmute() const;
    void setLooping(bool loop);//o
    bool getLooping() const { return shouldLoop; }//o
    void togglePlayPause();
    bool isPlaying() const { return transportSource.isPlaying(); }
    bool isPaused() const { return !transportSource.isPlaying() && transportSource.getCurrentPosition() > 0; }

    void play() {
        
        if (currentlyPlaying != nullptr && currentlyPlaying != this) {
            currentlyPlaying->transportSource.stop();
        }

        
        transportSource.start();
        currentlyPlaying = this;
    }

    void setLoopStartPoint();//o
    void setLoopEndPoint();//o
    void setABLooping(bool shouldABLoop);//o

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;



};

