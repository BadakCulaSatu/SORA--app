#include <JuceHeader.h>

// --- Jendela Simulasi Plugin ---
class PluginWindow : public juce::DocumentWindow {
public:
    PluginWindow (juce::String name) : DocumentWindow (name, juce::Colours::grey, allButtons) {
        setSize (300, 200);
        setUsingNativeTitleBar (true);
    }
    void closeButtonPressed() override { setVisible (false); }
};

class SoraEditor : public juce::AudioProcessorEditor {
public:
    SoraEditor (juce::AudioProcessor& p) : AudioProcessorEditor (&p) {
        setSize (400, 500);

        // Title
        addAndMakeVisible (titleLabel);
        titleLabel.setText ("SORA Control", juce::dontSendNotification);
        titleLabel.setFont (juce::Font (24.0f, juce::Font::bold));

        // Audio Device Section
        addAndMakeVisible (deviceLabel);
        deviceLabel.setText ("Audio Device--", juce::dontSendNotification);
        addAndMakeVisible (inputSelector);
        inputSelector.setText ("Input-- (Dropdown Menu)");
        addAndMakeVisible (outputSelector);
        outputSelector.setText ("Output-- (Dropdown Menu)");

        // Routing Section
        addAndMakeVisible (routingLabel);
        routingLabel.setText ("Routing--", juce::dontSendNotification);
        addAndMakeVisible (outALabel);
        outALabel.setText ("Out A-- (Dropdown Menu)", juce::dontSendNotification);

        // Plugin Button
        addAndMakeVisible (pluginBtn);
        pluginBtn.setButtonText ("Insert Plugin (klik!)");
        pluginBtn.onClick = [this] { 
            if (plugWin == nullptr) plugWin = std::make_unique<PluginWindow>("SORA Plugin View");
            plugWin->setVisible (true);
            plugWin->toFront (true);
        };

        // Mute Button [ x ]
        addAndMakeVisible (muteBtn);
        muteBtn.setButtonText ("[ x ]");
        muteBtn.setClickingTogglesState(true);
        muteBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);

        // Volume Slider
        addAndMakeVisible (volSlider);
        volSlider.setRange (0, 100, 1);
        volSlider.setSliderStyle (juce::Slider::LinearHorizontal);
        volSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

        // Add Output Button
        addAndMakeVisible (addBtn);
        addBtn.setButtonText ("[ + ] Add Output (lanjutan dr abjad A-Z)");
    }

    void paint (juce::Graphics& g) override {
        g.fillAll (juce::Colour (0xff2b302b));
        g.setColour (juce::Colours::white);
        
        // Borders sesuai sketsa
        g.drawRect (15, 105, 370, 100, 1); // Device box
        g.drawRect (15, 255, 370, 125, 1); // Routing box
        g.drawRect (15, 395, 370, 50, 1);  // Add box
    }

    void resized() override {
        titleLabel.setBounds (20, 20, 300, 40);
        deviceLabel.setBounds (20, 75, 200, 30);
        inputSelector.setBounds (30, 115, 340, 30);
        outputSelector.setBounds (30, 155, 340, 30);
        
        routingLabel.setBounds (20, 225, 200, 30);
        outALabel.setBounds (30, 265, 340, 25);
        pluginBtn.setBounds (30, 295, 340, 30);
        
        // Baris Slider: Mute dulu baru Slider
        muteBtn.setBounds (30, 335, 45, 30);
        volSlider.setBounds (85, 335, 285, 30);
        
        addBtn.setBounds (30, 405, 340, 30);
    }

private:
    juce::Label titleLabel, deviceLabel, routingLabel, outALabel;
    juce::ComboBox inputSelector, outputSelector;
    juce::TextButton pluginBtn, muteBtn, addBtn;
    juce::Slider volSlider;
    std::unique_ptr<PluginWindow> plugWin;
};

// --- Boilerplate Engine & App (Tetap Sama) ---
class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>& b, juce::MidiBuffer&) override { b.clear(); }
    juce::AudioProcessorEditor* createEditor() override { return new SoraEditor (*this); }
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
    const juce::String getApplicationName() override { return "SORA Control"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    void initialise (const juce::String&) override {
        engine = std::make_unique<SoraEngine>();
        window = std::make_unique<juce::DocumentWindow> ("SORA Control", juce::Colours::black, 7);
        window->setUsingNativeTitleBar (true);
        window->setContentOwned (engine->createEditor(), true);
        window->centreWithSize (400, 550);
        window->setVisible (true);
    }
    void shutdown() override { engine.reset(); window.reset(); }
    void systemRequestedQuit() override { quit(); }
private:
    std::unique_ptr<SoraEngine> engine;
    std::unique_ptr<juce::DocumentWindow> window;
};
START_JUCE_APPLICATION (SoraApp)
