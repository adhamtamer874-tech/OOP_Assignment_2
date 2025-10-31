#pragma once

#include "PlayerGui.h"
#include <JuceHeader.h>
#include <vector>


/*
 * MainComponent.h
 * -----------------------------
 * This is the main container of the application.
 *
 * - It holds the main layout of the app.
 * - It creates and displays PlayerGUI (the visual part of the player).
 * - It can contain multiple PlayerGUI components (for Task 4 bonus).
*/

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;

private:
    PlayerGui player1;
    PlayerGui player2;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent);
};
