/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Distortion.h"


struct ThreeBandCompressor {

    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{ nullptr };
    juce::AudioParameterFloat* threshold{ nullptr };
    juce::AudioParameterFloat* ratio{ nullptr };
    juce::AudioParameterBool* mute{ nullptr };
    juce::AudioParameterBool* solo{ nullptr };
    juce::AudioParameterBool* bypassed{ nullptr };

    void prepare(const juce::dsp::ProcessSpec& spec) {
        compressor.prepare(spec);
    }
    void setCompressorParameterValues() {

        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(ratio->get());

    }

    void process(juce::AudioBuffer<float>& buffer) {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto context = juce::dsp::ProcessContextReplacing<float>(block);

        context.isBypassed = bypassed->get();

        compressor.process(context);
    }

private:
    juce::dsp::Compressor<float> compressor;

};

//==============================================================================
/**
*/
class TheEqualizerAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    TheEqualizerAudioProcessor();
    ~TheEqualizerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    float softClip(float sample, float range);


   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();
    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout()
    };

    juce::AudioVisualiserComponent  wave;
    float hardClipping(float* channelData);

    float getRMS(const int channel) const;
    void pushNextSampleIntoFifo(float sample) noexcept;

    static constexpr auto fftOrder = 11;
    static constexpr auto fftSize = 1 << fftOrder;

    bool getFFTReady() {
        return nextFFTBlockReady;
    }

    void setFFTReady(bool b) {
        nextFFTBlockReady = b;
    }

    float* getFFTDataPtr() {
        return fftData.data();
    }

    float getFFTDataSample(int sample) {
        return fftData[sample];
    }
private:

    //==============================================================================


    LinearSmoothedValue<float> driveVolume, dryVolume, wetVolume;

    LinearSmoothedValue<float> tanhAmplitude, tanhSlope, sineAmplitude, sineFrequency;

    AudioBuffer<float> mixBuffer;
    /// <summary>
    /// //////////////////////////////////////////////////////////////////////////////////
    /// </summary>
    std::array<float, fftSize> fifo;
    std::array<float, fftSize * 2> fftData;
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;



    std::array<ThreeBandCompressor,3> compressors;
    ThreeBandCompressor& lowBandComp = compressors[0];
    ThreeBandCompressor& midBandComp = compressors[1];
    ThreeBandCompressor& highBandComp = compressors[2];
    juce::AudioParameterBool* reverboff{ nullptr };
    juce::AudioParameterFloat* drive{ nullptr };
    juce::AudioParameterFloat* range{ nullptr };
    juce::AudioParameterFloat* blend{ nullptr };
    juce::AudioParameterFloat* volume{ nullptr };
    juce::AudioParameterFloat* gain{ nullptr };
    juce::AudioParameterBool* distortionoff{ nullptr };

    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    Filter LowPassFirst, AllPassSecond,
        HighPassFirst, LowPassSecond,
        HighPassSecond;

    juce::AudioParameterFloat* LowMidBorder{ nullptr };
    juce::AudioParameterFloat* MidHighBorder{ nullptr };


    std::array<juce::AudioBuffer<float>, 3> filterBuffers;

    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* gainInParam{ nullptr };
    juce::AudioParameterFloat* gainOutParam{ nullptr };

    float rmsLeft, rmsRight;

   
    juce::dsp::Reverb::Parameters ReverbSettings;
    juce::dsp::Reverb leftReverb, rightReverb;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheEqualizerAudioProcessor)
};
