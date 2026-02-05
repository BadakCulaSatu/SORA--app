#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#define JUCE_SILENCE_XCODE_15_DEPRECATION_WARNINGS 1

#include <JuceHeader.h>

class SoraEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener {
public:
    SoraEditor (juce::AudioProcessor& p, juce::AudioDeviceManager& dm)
        : AudioProcessorEditor (&p), processor(static_cast<class SoraEngine&>(p)), deviceManager (dm)
    {
        setSize (400, 600);

        // --- SECTION 1: DEVICE ---
        addAndMakeVisible (inputLabel);
        inputLabel.setText ("Input Device:", juce::dontSendNotification);
        addAndMakeVisible (inputSelector);
        
        addAndMakeVisible (outputLabel);
        outputLabel.setText ("Output Device:", juce::dontSendNotification);
        addAndMakeVisible (outputSelector);
        updateDeviceLists();

        // --- SECTION 2: ROUTING & PLUGINS ---
        addAndMakeVisible (routingLabel);
        routingLabel.setText ("Routing & Processing", juce::dontSendNotification);

        addAndMakeVisible (chanLabel);
        chanLabel.setText ("Active Channels:", juce::dontSendNotification);
        addAndMakeVisible (chanSelector);
        chanSelector.addItem ("Stereo Output (Default)", 1);
        chanSelector.setSelectedId (1);

        addAndMakeVisible (pluginBtn);
        pluginBtn.setButtonText ("Scan & Insert Plugin");
        pluginBtn.onClick = [this] { scanPlugins(); };

        // Mute Button
        addAndMakeVisible (muteBtn);
        muteBtn.setButtonText ("Mute");
        muteBtn.setClickingTogglesState (true);
        muteBtn.onClick = [this] {
            processor.isMuted = muteBtn.getToggleState();
            muteBtn.setButtonText (processor.isMuted ? "Unmute" : "Mute");
        };

        // Volume Slider (Functional)
        addAndMakeVisible (volSlider);
        volSlider.setRange (0.0, 1.0);
        volSlider.setValue (processor.currentLevel);
        volSlider.setSliderStyle (juce::Slider::LinearHorizontal);
        volSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        volSlider.addListener (this);

        addAndMakeVisible (addBtn);
        addBtn.setButtonText ("[ + ] Add Output (A-Z)");
    }

    void sliderValueChanged (juce::Slider* slider) override {
        if (slider == &volSlider) processor.currentLevel = (float)volSlider.getValue();
    }

    void scanPlugins() {
        juce::AudioPluginFormatManager formatManager;
        formatManager.addDefaultFormats();
        juce::KnownPluginList pluginList;
        juce::AudioPluginScanner scanner (pluginList, formatManager, {}, true, true);
        
        juce::NativeMessageBox::showMessageBoxAsync (juce::AlertWindow::InfoIcon, "SORA Control", 
            "Scanning VST3/AU di folder sistem Mac lo...");
    }

    void updateDeviceLists() {
        if (auto* type = deviceManager.getCurrentAudioDeviceType()) {
            inputSelector.addItemList (type->getDeviceNames (true), 1);
            outputSelector.addItemList (type->getDeviceNames (false), 1);
            inputSelector.setSelectedId(1);
            outputSelector.setSelectedId(1);
        }
    }

    void paint (juce::Graphics& g) override {
        g.fillAll (juce::Colour (0xff2b302b));
        g.setColour (juce::Colours::white.withAlpha(0.3f));
        g.drawRoundedRectangle (15, 60, 370, 120, 5.0f, 1.0f);   // Device Box
        g.drawRoundedRectangle (15, 200, 370, 260, 5.0f, 1.0f);  // Routing Box
    }

    void resized() override {
        inputLabel.setBounds (30, 75, 100, 25);
        inputSelector.setBounds (140, 75, 230, 25);
        outputLabel.setBounds (30, 115, 100, 25);
        outputSelector.setBounds (140, 115, 230, 25);

        routingLabel.setBounds (30, 215, 300, 25);
        chanLabel.setBounds (30, 250, 120, 25);
        chanSelector.setBounds (160, 250, 200, 25);
        
        pluginBtn.setBounds (30, 300, 340, 35);
        muteBtn.setBounds (30, 360, 70, 40);
        volSlider.setBounds (110, 360, 260, 40);
        addBtn.setBounds (15, 500, 370, 50);
    }

private:
    class SoraEngine& processor;
    juce::AudioDeviceManager& deviceManager;
    juce::Label inputLabel, outputLabel, routingLabel, chanLabel;
    juce::ComboBox inputSelector, outputSelector, chanSelector;
    juce::TextButton pluginBtn, muteBtn, addBtn;
    juce::Slider volSlider;
};

class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        if (isMuted) {
            buffer.clear();
        } else {
            buffer.applyGain (currentLevel); // Volume berfungsi di sini
        }
    }

    juce::AudioProcessorEditor* createEditor() override { return new SoraEditor (*this, deviceManager); }
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SORA Control"; }
    juce::AudioDeviceManager deviceManager;
    float currentLevel = 0.8f;
    bool isMuted = false;

    // Boilerplate lainnya
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
};

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
