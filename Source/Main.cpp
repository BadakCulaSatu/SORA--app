#ifdef __APPLE__
#include <TargetConditionals.h>
#endif
#define JUCE_SILENCE_XCODE_15_DEPRECATION_WARNINGS 1

#include <JuceHeader.h>

class SoraEditor : public juce::AudioProcessorEditor {
public:
    SoraEditor (juce::AudioProcessor& p, juce::AudioDeviceManager& dm)
        : AudioProcessorEditor (&p), deviceManager (dm)
    {
        setSize (400, 600);

        // --- SECTION 1: DEVICE SETTINGS ---
        addAndMakeVisible (inputLabel);
        inputLabel.setText ("Input Device:", juce::dontSendNotification);
        addAndMakeVisible (inputSelector);
        
        addAndMakeVisible (outputLabel);
        outputLabel.setText ("Output Device:", juce::dontSendNotification);
        addAndMakeVisible (outputSelector);

        // Update list device ke dropdown
        updateDeviceLists();

        // --- SECTION 2: ROUTING BOX ---
        addAndMakeVisible (routingLabel);
        routingLabel.setText ("Routing & Processing", juce::dontSendNotification);
        routingLabel.setFont (juce::Font (16.0f, juce::Font::bold));

        // Dropdown untuk Slot Output (pengganti checklist)
        addAndMakeVisible (outSlotLabel);
        outSlotLabel.setText ("Active Out Channel:", juce::dontSendNotification);
        addAndMakeVisible (outSlotSelector);
        outSlotSelector.addItem ("All Channels Active", 1);
        outSlotSelector.setSelectedId (1);

        addAndMakeVisible (pluginBtn);
        pluginBtn.setButtonText ("Insert Plugin");
        pluginBtn.onClick = [this] { 
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon, "Plugin", "Plugin Manager akan terbuka...");
        };

        // Mute pakai simbol speaker 🔊 / 🔇
        addAndMakeVisible (muteBtn);
        muteBtn.setButtonText (juce::String::fromUTF8("\xF0\x9F\x94\x8A")); 
        muteBtn.setClickingTogglesState (true);
        muteBtn.onClick = [this] {
            muteBtn.setButtonText (muteBtn.getToggleState() ? 
                juce::String::fromUTF8("\xF0\x9F\x94\x87") : juce::String::fromUTF8("\xF0\x9F\x94\x8A"));
        };

        addAndMakeVisible (volSlider);
        volSlider.setRange (0.0, 1.0);
        volSlider.setSliderStyle (juce::Slider::LinearHorizontal);
        volSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

        // --- SECTION 3: FOOTER ---
        addAndMakeVisible (addBtn);
        addBtn.setButtonText ("[ + ] Add Output (A-Z)");
        addBtn.onClick = [this] {
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon, "Add Output", "Slot Output Baru Ditambahkan.");
        };
    }

    void updateDeviceLists() {
        auto* type = deviceManager.getCurrentAudioDeviceType();
        if (type != nullptr) {
            inputSelector.addItemList (type->getDeviceNames (true), 1);
            outputSelector.addItemList (type->getDeviceNames (false), 1);
        }
    }

    void paint (juce::Graphics& g) override {
        g.fillAll (juce::Colour (0xff2b302b));
        g.setColour (juce::Colours::white.withAlpha(0.2f));
        
        // Garis pemisah box
        g.drawRoundedRectangle (15, 60, 370, 140, 5.0f, 1.0f);   // Box Device
        g.drawRoundedRectangle (15, 220, 370, 240, 5.0f, 1.0f);  // Box Routing
    }

    void resized() override {
        auto area = getLocalBounds().reduced (20);
        
        // Layout Device
        inputLabel.setBounds (30, 75, 100, 25);
        inputSelector.setBounds (140, 75, 230, 25);
        
        outputLabel.setBounds (30, 115, 100, 25);
        outputSelector.setBounds (140, 115, 230, 25);

        // Layout Routing
        routingLabel.setBounds (30, 235, 300, 25);
        
        outSlotLabel.setBounds (30, 275, 120, 25);
        outSlotSelector.setBounds (160, 275, 200, 25);
        
        pluginBtn.setBounds (30, 320, 340, 35);
        
        muteBtn.setBounds (30, 375, 50, 40);
        volSlider.setBounds (90, 375, 280, 40);

        addBtn.setBounds (20, 500, 360, 50);
    }

private:
    juce::AudioDeviceManager& deviceManager;
    juce::Label inputLabel, outputLabel, routingLabel, outSlotLabel;
    juce::ComboBox inputSelector, outputSelector, outSlotSelector;
    juce::TextButton pluginBtn, muteBtn, addBtn;
    juce::Slider volSlider;
};

// --- ENGINE & APP LOADER (Tetap) ---
class SoraEngine : public juce::AudioProcessor {
public:
    SoraEngine() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>& b, juce::MidiBuffer&) override { b.clear(); }
    juce::AudioProcessorEditor* createEditor() override { return new SoraEditor (*this, deviceManager); }
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "SORA Control"; }
    juce::AudioDeviceManager deviceManager;
    // Boilerplate standard lainnya...
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
