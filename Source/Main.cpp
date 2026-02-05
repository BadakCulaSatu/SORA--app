#include <JuceHeader.h>

// --- Editor Utama ---
class SoraEditor : public juce::AudioProcessorEditor {
public:
    SoraEditor (juce::AudioProcessor& p, juce::AudioDeviceManager& deviceManager)
        : AudioProcessorEditor (&p), deviceSelector (deviceManager, 
          0, 2, 0, 2, false, false, true, false) // Setup Selector Audio
    {
        setSize (400, 600);

        // Title
        addAndMakeVisible (titleLabel);
        titleLabel.setText ("SORA Control", juce::dontSendNotification);
        titleLabel.setFont (juce::Font (24.0f, juce::Font::bold));

        // Section 1: Audio Device (Auto-Detect List)
        addAndMakeVisible (deviceSelector);

        // Section 2: Routing & Plugin
        addAndMakeVisible (routingLabel);
        routingLabel.setText ("Routing--", juce::dontSendNotification);

        addAndMakeVisible (pluginBtn);
        pluginBtn.setButtonText ("Insert Plugin (Scan VST/AU)");
        pluginBtn.onClick = [this] { 
            // Fungsi scan plugin akan berjalan di sini
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon, "Plugin Scan", "Mencari VST3/AU di /Library/Audio/Plug-Ins...");
        };

        // Section 3: Mute & Vol
        addAndMakeVisible (muteBtn);
        muteBtn.setButtonText ("[ x ]");
        muteBtn.setClickingTogglesState (true);
        muteBtn.setColour (juce::TextButton::buttonOnColourId, juce::Colours::red);

        addAndMakeVisible (volSlider);
        volSlider.setRange (0.0, 1.0);
        volSlider.setSliderStyle (juce::Slider::LinearHorizontal);
        volSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

        addAndMakeVisible (addBtn);
        addBtn.setButtonText ("[ + ] Add Output (A-Z)");
    }

    void paint (juce::Graphics& g) override {
        g.fillAll (juce::Colour (0xff2b302b));
        g.setColour (juce::Colours::white);
        g.drawRect (15, 300, 370, 150, 1); // Routing Box
    }

    void resized() override {
        titleLabel.setBounds (20, 20, 300, 40);
        deviceSelector.setBounds (15, 70, 370, 220); // Mengisi slot Audio Device
        
        routingLabel.setBounds (20, 310, 200, 30);
        pluginBtn.setBounds (30, 350, 340, 30);
        muteBtn.setBounds (30, 395, 45, 30);
        volSlider.setBounds (85, 395, 285, 30);
        
        addBtn.setBounds (15, 470, 370, 50);
    }

private:
    juce::Label titleLabel, routingLabel;
    juce::AudioDeviceSelectorComponent deviceSelector; // Komponen pintar JUCE
    juce::TextButton pluginBtn, muteBtn, addBtn;
    juce::Slider volSlider;
};

// --- Engine Utama ---
class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        // Logika Mute
        if (isMuted) buffer.clear();
    }

    juce::AudioProcessorEditor* createEditor() override { 
        return new SoraEditor (*this, deviceManager); 
    }
    
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SORA Control"; }
    
    // Boilerplate lainnya tetap sama...
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

    bool isMuted = false;
    juce::AudioDeviceManager deviceManager;
};

// --- Application Loader ---
class SoraApp : public juce::JUCEApplication {
public:
    const juce::String getApplicationName() override { return "SORA Control"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    
    void initialise (const juce::String&) override {
        engine = std::make_unique<SoraEngine>();
        // Meminta izin mic di macOS
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
