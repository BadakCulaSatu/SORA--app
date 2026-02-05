#include <JuceHeader.h>

// --- ENGINE SORA CONTROL ---
class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)) {}

    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {}

    juce::AudioProcessorEditor* createEditor() override { return new juce::GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SORA Control"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}
};

// --- APP CONTROLLER ---
class SoraApp : public juce::JUCEApplication {
public:
    SoraApp() {}
    const juce::String getApplicationName() override { return "SORA Control"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }

    void initialise (const juce::String&) override {
        engine = std::make_unique<SoraEngine>();
        deviceManager.initialiseWithDefaultDevices (2, 2);
        player.setProcessor (engine.get());
        deviceManager.addAudioCallback (&player);
        
        window = std::make_unique<juce::DocumentWindow> ("SORA Control", 
            juce::Colours::black, juce::DocumentWindow::allButtons);
        window->setContentOwned (engine->createEditor(), true);
        window->setResizable (true, true);
        window->setVisible (true);
    }

    void shutdown() override {
        deviceManager.removeAudioCallback (&player);
        player.setProcessor (nullptr);
        engine.reset();
        window.reset();
    }

    void systemRequestedQuit() override { quit(); }

private:
    std::unique_ptr<SoraEngine> engine;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer player;
    std::unique_ptr<juce::DocumentWindow> window;
};

START_JUCE_APPLICATION (SoraApp)
