#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// Mantra pencegah error versi macOS terbaru vs lama
#define JUCE_SILENCE_XCODE_15_DEPRECATION_WARNINGS 1
#define JUCE_COREGRAPHICS_RENDER_WITH_DIRECT2D 0

#include <JuceHeader.h>

// --- Editor dengan UI sesuai Gambar lo ---
class SoraEditor : public juce::AudioProcessorEditor {
public:
    SoraEditor (juce::AudioProcessor& p, juce::AudioDeviceManager& dm)
        : AudioProcessorEditor (&p), deviceSelector (dm, 0, 2, 0, 2, false, false, true, false)
    {
        setSize (400, 600);

        addAndMakeVisible (titleLabel);
        titleLabel.setText ("SORA Control", juce::dontSendNotification);
        titleLabel.setFont (juce::Font (24.0f, juce::Font::bold));

        // Section 1: Audio Device (Dropdown & List muncul di sini)
        addAndMakeVisible (deviceSelector);

        // Section 2: Routing
        addAndMakeVisible (routingLabel);
        routingLabel.setText ("Routing--", juce::dontSendNotification);

        addAndMakeVisible (pluginBtn);
        pluginBtn.setButtonText ("Insert Plugin (Klik)");
        
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
        g.fillAll (juce::Colour (0xff2b302b)); // Warna sesuai gambar lo
        g.setColour (juce::Colours::white);
        g.drawRect (15, 305, 370, 140, 1); // Kotak Routing
    }

    void resized() override {
        titleLabel.setBounds (20, 20, 300, 40);
        deviceSelector.setBounds (15, 70, 370, 230);
        
        routingLabel.setBounds (20, 315, 200, 20);
        pluginBtn.setBounds (30, 345, 340, 30);
        muteBtn.setBounds (30, 390, 45, 30);
        volSlider.setBounds (85, 390, 285, 30);
        
        addBtn.setBounds (15, 470, 370, 50);
    }

private:
    juce::Label titleLabel, routingLabel;
    juce::AudioDeviceSelectorComponent deviceSelector;
    juce::TextButton pluginBtn, muteBtn, addBtn;
    juce::Slider volSlider;
};

// --- Engine ---
class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>& b, juce::MidiBuffer&) override { b.clear(); }
    juce::AudioProcessorEditor* createEditor() override { return new SoraEditor (*this, deviceManager); }
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

    juce::AudioDeviceManager deviceManager;
};

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
