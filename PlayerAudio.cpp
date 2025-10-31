
#include "PlayerAudio.h"

/*
 * PlayerAudio.cpp
 * -----------------------------
 * Implements all functions declared in PlayerAudio.h
 */

PlayerAudio::PlayerAudio() {
    formatManager.registerBasicFormats();

}


PlayerAudio::~PlayerAudio()
{
    releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{

    if (isABLoopingActive && transportSource.isPlaying())
    {

        if (transportSource.getCurrentPosition() >= loopEndSeconds)
        {

            transportSource.setPosition(loopStartSeconds);
        }
    }


    transportSource.getNextAudioBlock(bufferToFill);
}
void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    transportSource.removeChangeListener(this);
    readerSource.reset();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            transportSource.removeChangeListener(this);
            readerSource.reset();

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
            transportSource.addChangeListener(this);
            readerSource->setLooping(shouldLoop);

            loopStartSeconds = 0.0;
            loopEndSeconds = transportSource.getLengthInSeconds();
            isABLoopingActive = false;
            return true;
        }
    }
    return false;
}
PlayerAudio* PlayerAudio::currentlyPlaying = nullptr;

void PlayerAudio::togglePlayPause()
{
    // أوقف أي لاعب تاني شغال
    if (currentlyPlaying != nullptr && currentlyPlaying != this) {
        currentlyPlaying->transportSource.stop();
    }

    // لو مش شغال - شغل
    if (!transportSource.isPlaying()) {
        transportSource.start();
        currentlyPlaying = this;
    }
    // لو شغال - أوقف (Pause)
    else {
        transportSource.stop();
        if (currentlyPlaying == this) {
            currentlyPlaying = nullptr;
        }
    }
}

// دالة Stop عادية
void PlayerAudio::stop()
{
    transportSource.stop();
    if (currentlyPlaying == this) {
        currentlyPlaying = nullptr;
    }
}
// ======= Asmaa ========
void PlayerAudio::setGain(float gain)
{
    currentGain = gain;


    if (!isMuted)

        transportSource.setGain(currentGain);
}
void PlayerAudio::muteornot()
{
    if (!isMuted)
    {
        savedGain = currentGain;
        transportSource.setGain(0.0f);
        isMuted = true;

    }
    else {
        transportSource.setGain(savedGain);

        isMuted = false;

    }
}
bool PlayerAudio::readifmute() const
{
    return isMuted;
}


void PlayerAudio::setPosition(double pos) {
    transportSource.setPosition(pos);
}


double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}




double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}


void PlayerAudio::setLooping(bool loop)
{
    shouldLoop = loop;
    if (readerSource)
        readerSource->setLooping(loop);
}

// ========== Omar =============
void PlayerAudio::changeListenerCallback(juce::ChangeBroadcaster* source)
{

    if (source == &transportSource)
    {
        if (shouldLoop && transportSource.hasStreamFinished())
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}


void PlayerAudio::setLoopStartPoint()
{
    loopStartSeconds = getPosition();

    if (loopStartSeconds >= loopEndSeconds) {
        loopEndSeconds = getLength();
    }
}

void PlayerAudio::setLoopEndPoint()
{
    loopEndSeconds = getPosition();

    if (loopEndSeconds <= loopStartSeconds) {
        loopStartSeconds = 0.0;
    }
}

void PlayerAudio::setABLooping(bool shouldABLoop)
{
    isABLoopingActive = shouldABLoop;
}



