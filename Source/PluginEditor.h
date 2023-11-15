/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

struct Placeholder : juce::Component {

    Placeholder();
    void paint(juce::Graphics& g) override;
    juce::Colour customColor;


};

class HorizontalMeter : public juce::Component {
public:
    void paint(Graphics& g)override {

        auto bounds = getLocalBounds().toFloat();

        g.setColour(juce::Colour(24, 25, 26));
        g.fillRoundedRectangle(bounds, 5.f);

        g.setColour(juce::Colour(0, 132, 67));


       const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));

        g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 5.f);


    };

    void setLevel(const float value) {
        level = value;
    }
private:
    float level = -60.f;
};
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider & ) override;
    //void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
       // bool, bool isButtonDown) override;


};

template < typename APVTS, typename Param, typename Name>

juce::RangedAudioParameter& getParam(APVTS& apvts, const Param& params, const Name& name) {
    auto param = apvts.getParameter(params.at(name));

    jassert(param != nullptr);

    return *param;
}
template< typename Attachment, typename APVTS, typename Params, typename ParamName, typename SliderType>

void makeAttachment(std::unique_ptr<Attachment>& attachment, APVTS& apvts,
    const Params& params, const ParamName& name, SliderType& slider) {
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}


struct LowBandControls : juce::Component {
    LowBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~LowBandControls();

    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::Slider attackSlider, releaseSlider, thresholdSlider, ratioSlider;

private:

    CustomLookAndFeel clnf;
    juce::Label testLabel;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ComboBox ratiobox1, ratiobox2, ratiobox3;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
        releaseSliderAttachment,
        thresholdSliderAttachment,
        ratioSliderAttachment;


       
};
struct MidBandControls : juce::Component {
    MidBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~MidBandControls();

    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::Slider attackSlider, releaseSlider, thresholdSlider, ratioSlider;

private:

    CustomLookAndFeel clnf;
    juce::AudioProcessorValueTreeState& apvts;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
        releaseSliderAttachment,
        thresholdSliderAttachment,
        ratioSliderAttachment;

};
struct HighBandControls : juce::Component {
    HighBandControls(juce::AudioProcessorValueTreeState& apvts);
    ~HighBandControls();

    void paint(juce::Graphics& g) override;
    void resized() override;

    juce::Slider attackSlider, releaseSlider, thresholdSlider, ratioSlider;

private:

    CustomLookAndFeel clnf;
    juce::AudioProcessorValueTreeState& apvts;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackSliderAttachment,
        releaseSliderAttachment,
        thresholdSliderAttachment,
        ratioSliderAttachment;

};

struct ReverbControls : juce::Component {
    ReverbControls(juce::AudioProcessorValueTreeState& apvts);
    ~ReverbControls();

    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::Slider sizeSlider, widthSlider, dampSlider, drySlider, wetSlider;

private:

    CustomLookAndFeel clnf;
    juce::AudioProcessorValueTreeState& apvts;
    juce::TextButton offBtn;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> sizeSliderAttachment,
        widthSliderAttachment,
        dampSliderAttachment,
        drySliderAttachment,
        wetSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> offBtnAttachment;
};

struct ThreeButtonsLow : juce::Component, juce::Button::Listener {
    ThreeButtonsLow(juce::AudioProcessorValueTreeState& apvts);
    ~ThreeButtonsLow();
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    void buttonClicked(juce::Button* button);
    juce::TextButton offBtn, solobtn, mutebtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> offBtnAtt, solobtnAtt, mutebtnAtt;


};

struct ThreeButtonsMid : juce::Component, juce::Button::Listener {
    ThreeButtonsMid(juce::AudioProcessorValueTreeState& apvts);
    ~ThreeButtonsMid();
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    void buttonClicked(juce::Button* button);
    juce::TextButton offBtn, solobtn, mutebtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> offBtnAtt, solobtnAtt, mutebtnAtt;


};

struct ThreeButtonsHigh : juce::Component, juce::Button::Listener {
    ThreeButtonsHigh(juce::AudioProcessorValueTreeState& apvts);
    ~ThreeButtonsHigh();
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    void buttonClicked(juce::Button* button);
    juce::TextButton offBtn, solobtn, mutebtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> offBtnAtt, solobtnAtt, mutebtnAtt;


};

struct DistortionComponent : juce::Component {
    DistortionComponent(juce::AudioProcessorValueTreeState& apvts);
    ~DistortionComponent();
    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::Slider distortionSlider, rangeSlider, mixSlider, gainSlider;

private:

    CustomLookAndFeel clnf;
    juce::AudioProcessorValueTreeState& apvts;
    juce::TextButton offBtn;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> distortionSliderAttachment,
        rangeSliderAttachment,
        mixSliderAttachment,
        gainSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> offBtnAttachment;

};

class TheEqualizerAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    TheEqualizerAudioProcessorEditor(TheEqualizerAudioProcessor&);
    ~TheEqualizerAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void drawNextFrameOfSpectrum();
    void drawFrame(juce::Graphics& g);
    void InitializeLabelsForCompressor();
    void InitializeGainSliderComponents();

    static constexpr auto scopeSize = 512;

    juce::Label LowAttackLabel, LowReleaseLabel,LowThresholdLabel, LowRatioLabel;
    juce::Label MidAttackLabel, MidReleaseLabel, MidThresholdLabel, MidRatioLabel;
    juce::Label HighAttackLabel, HighReleaseLabel, HighThresholdLabel, HighRatioLabel;

        
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    float scopeData[scopeSize];

    TheEqualizerAudioProcessor& audioProcessor;
    HorizontalMeter leftM, rightM;

    ReverbControls reverbControl{ audioProcessor.apvts };
    LowBandControls lowBandControls{ audioProcessor.apvts };
    ThreeButtonsLow threebuttonslow{ audioProcessor.apvts };

    MidBandControls midBandControls{ audioProcessor.apvts };
    ThreeButtonsMid threebuttonsmid{ audioProcessor.apvts };

    HighBandControls highBandControls{ audioProcessor.apvts };
    ThreeButtonsHigh threebuttonshigh{ audioProcessor.apvts };

    DistortionComponent distortionComponent{ audioProcessor.apvts };

    CustomLookAndFeel clnf;
    juce::Slider InputGainSlider, OutputGainSlider;
        
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ingainSliderAttachment,outgainSliderAttachment;
    
    juce::Label inputGainTitle, outputGainTitle, inputValueRange, outputValueRange;
    juce::Label lowBandTitles;
    juce::Label lowBandRanges;

    juce::Label midBandTitles;
    juce::Label midBandRanges;

    juce::Label highBandTitles;
    juce::Label highBandRanges;

    juce::Label reverbTitle;
    juce::Label reverbTitles;
    juce::Label reverbRange;

    juce::Label distortionTitles;
    juce::Label distortionRanges;


    Placeholder controller, analyzer, band; //bandControls;
    //myLookAndFeelV2 myLookAndFeelV2;
    juce::Slider inputGainSlider, outputGainSlider, lowToMidSlider,highToMidSlider;
    juce::Image back;
    juce::ImageComponent title;

   

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheEqualizerAudioProcessorEditor)
};
