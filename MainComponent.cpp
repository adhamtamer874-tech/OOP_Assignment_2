#include "MainComponent.h"
#include "PlayerGui.h"

/*
 * MainComponent.cpp
 * -----------------------------
 * This file implements MainComponent.
 *
 * - Initialize and display PlayerGUI.
 * - Define how components are drawn (background, layout).
 * - Handle resizing (place the player correctly in the window).
 */
MainComponent::MainComponent()
{
    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    player1.setPlayerID(1);
    player2.setPlayerID(2);


    setSize(1000, 700);
    setAudioChannels(0, 2);
}


MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();


    if (player1.isPlaying())
    {
        player1.getNextAudioBlock(bufferToFill);
    }
    else if (player2.isPlaying())
    {
        player2.getNextAudioBlock(bufferToFill);
    }

}


void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);
    auto halfHeight = area.getHeight() / 2 - 10;

    player1.setBounds(area.removeFromTop(halfHeight));
    area.removeFromTop(20);
    player2.setBounds(area);
}
