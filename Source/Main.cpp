#include <JuceHeader.h>

class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    ~SoraEngine() override {}

    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override { buffer.clear(); }

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

class SoraApp : public juce::JUCEApplication {
public:
    SoraApp() {}
    const juce::String getApplicationName() override { return "SORA Control"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }

    void initialise (const juce::String&) override {
        engine = std::make_unique<SoraEngine>();
        window = std::make_unique<juce::DocumentWindow> ("SORA Control", 
            juce::Colours::black, juce::DocumentWindow::allButtons);
        window->setUsingNativeTitleBar (true);
        window->setContentOwned (engine->createEditor(), true);
        window->setResizable (true, true);
        window->centreWithSize (400, 300);
        window->setVisible (true);
    }

    void shutdown() override { engine.reset(); window.reset(); }
    void systemRequestedQuit() override { quit(); }

private:
    std::unique_ptr<SoraEngine> engine;
    std::unique_ptr<juce::DocumentWindow> window;
};

START_JUCE_APPLICATION (SoraApp)
