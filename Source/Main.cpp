#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#define JUCE_SILENCE_XCODE_15_DEPRECATION_WARNINGS 1

#include <JuceHeader.h>

// --- Forward Declaration agar Editor kenal Engine ---
class SoraEngine;

class SoraEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener {
public:
    SoraEditor (SoraEngine& p, juce::AudioDeviceManager& dm);
    ~SoraEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    SoraEngine& processor;
    juce::AudioDeviceManager& deviceManager;

    juce::Label inputLabel, outputLabel, routingLabel, chanLabel;
    juce::ComboBox inputSelector, outputSelector, chanSelector;
    juce::TextButton pluginBtn, muteBtn, addBtn;
    juce::Slider volSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoraEditor)
};

class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        if (isMuted) {
            buffer.clear();
        } else {
            buffer.applyGain (currentLevel);
        }
    }

    juce::AudioProcessorEditor* createEditor() override { return new SoraEditor (*this, deviceManager); }
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SORA Control"; }
    
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
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

    juce::AudioDeviceManager deviceManager;
    float currentLevel = 0.7f;
    bool isMuted = false;
};

// --- Implementasi Editor ---
SoraEditor::SoraEditor (SoraEngine& p, juce::AudioDeviceManager& dm)
    : AudioProcessorEditor (&p), processor (p), deviceManager (dm)
{
    setSize (400, 600);

    addAndMakeVisible (inputLabel);
    inputLabel.setText ("Input Device:", juce::dontSendNotification);
    addAndMakeVisible (inputSelector);
    
    addAndMakeVisible (outputLabel);
    outputLabel.setText ("Output Device:", juce::dontSendNotification);
    addAndMakeVisible (outputSelector);

    // Fill ComboBox
    if (auto* type = deviceManager.getCurrentAudioDeviceType()) {
        inputSelector.addItemList (type->getDeviceNames (true), 1);
        outputSelector.addItemList (type->getDeviceNames (false), 1);
        inputSelector.setSelectedId(1);
        outputSelector.setSelectedId(1);
    }

    addAndMakeVisible (routingLabel);
    routingLabel.setText ("Routing & Processing", juce::dontSendNotification);

    addAndMakeVisible (chanSelector);
    chanSelector.addItem ("Stereo Master", 1);
    chanSelector.setSelectedId (1);

    addAndMakeVisible (pluginBtn);
    pluginBtn.setButtonText ("Scan Plugins");
    pluginBtn.onClick = [this] {
        juce::NativeMessageBox::showMessageBoxAsync (juce::AlertWindow::InfoIcon, "SORA", "Scanning system for VST3/AU...");
    };

    addAndMakeVisible (muteBtn);
    muteBtn.setButtonText ("Mute");
    muteBtn.setClickingTogglesState (true);
    muteBtn.onClick = [this] {
        processor.isMuted = muteBtn.getToggleState();
        muteBtn.setButtonText (processor.isMuted ? "Unmute" : "Mute");
    };

    addAndMakeVisible (volSlider);
    volSlider.setRange (0.0, 1.0);
    volSlider.setValue (processor.currentLevel);
    volSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    volSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    volSlider.addListener (this);

    addAndMakeVisible (addBtn);
    addBtn.setButtonText ("[ + ] Add Output");
}

SoraEditor::~SoraEditor() {}

void SoraEditor::sliderValueChanged (juce::Slider* slider) {
    if (slider == &volSlider) processor.currentLevel = (float)volSlider.getValue();
}

void SoraEditor::paint (juce::Graphics& g) {
    g.fillAll (juce::Colour (0xff2b302b));
    g.setColour (juce::Colours::white.withAlpha(0.2f));
    g.drawRoundedRectangle (15, 65, 370, 110, 5.0f, 1.0f);   // Device Box
    g.drawRoundedRectangle (15, 200, 370, 260, 5.0f, 1.0f);  // Routing Box
}

void SoraEditor::resized() {
    inputLabel.setBounds (30, 80, 100, 25);
    inputSelector.setBounds (140, 80, 230, 25);
    outputLabel.setBounds (30, 120, 100, 25);
    outputSelector.setBounds (140, 120, 230, 25);

    routingLabel.setBounds (30, 215, 300, 25);
    chanSelector.setBounds (30, 250, 340, 25);
    pluginBtn.setBounds (30, 300, 340, 35);
    muteBtn.setBounds (30, 360, 80, 40);
    volSlider.setBounds (120, 360, 250, 40);
    addBtn.setBounds (15, 500, 370, 50);
}

// --- App Loader ---
class SoraApp : public juce::JUCEApplication {
public:
    const juce::String getApplicationName() override { return "SORA Control"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    void initialise (const juce::String&) override {
        engine = std::make_unique<SoraEngine>();
        engine->deviceManager.initialiseWithDefaultDevices (2, 2);
        window = std::make_unique<juce::DocumentWindow> ("SORA Control", juce::Colours::black, 7);
        window->setUsingNativeTitleBar (true);
        window->setContentOwned (engine->createEditor(), true);
        window->centreWithSize (400, 600);
        window->setVisible (true);
    }
    void shutdown() override { engine.reset(); window.reset(); }
    void systemRequestedQuit() override { quit(); }
private:
    std::unique_ptr<SoraEngine> engine;
    std::unique_ptr<juce::DocumentWindow> window;
};
START_JUCE_APPLICATION (SoraApp)
