/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <chrono>    
using namespace std::chrono;

//==============================================================================
TheEqualizerAudioProcessor::TheEqualizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), wave(1)
#endif
{

    wave.setRepaintRate(30);
    wave.setBufferSize(256);


    //compressor.release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"));
    //jassert(compressor.release != nullptr);
    //compressor.threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"));
    //jassert(compressor.threshold != nullptr);
    //compressor.ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"));
    //jassert(compressor.ratio != nullptr);
    //compressor.bypassed = dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("Bypassed"));
    //jassert(compressor.bypassed != nullptr);


    drive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("drive"));
    range = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("range"));
    blend = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("blend"));
    volume = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("volume"));
    distortionoff = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("distortionoff"));


    reverboff = dynamic_cast <juce::AudioParameterBool*>(apvts.getParameter("reverboff"));
    jassert(reverboff != nullptr);
  

    gainInParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Mix Gain"));
    jassert(gainInParam != nullptr);
    gainOutParam = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Makeup Gain"));
    jassert(gainInParam != nullptr);

    lowBandComp.attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack Low"));
    jassert(lowBandComp.attack != nullptr);
    lowBandComp.release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release Low"));
    jassert(lowBandComp.release != nullptr);
    lowBandComp.threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold Low"));
    jassert(lowBandComp.threshold != nullptr);
 

    midBandComp.attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack Mid"));
    jassert(midBandComp.attack != nullptr);
    midBandComp.release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release Mid"));
    jassert(midBandComp.release != nullptr);
    midBandComp.threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold Mid"));
    jassert(midBandComp.threshold != nullptr);

    highBandComp.attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack High"));
    jassert(highBandComp.attack != nullptr);
    highBandComp.release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release High"));
    jassert(highBandComp.release != nullptr);
    highBandComp.threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold High"));
    jassert(highBandComp.threshold != nullptr);

    lowBandComp.ratio = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Ratio Low"));
    jassert(lowBandComp.ratio != nullptr);
    midBandComp.ratio = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Ratio Mid"));
    jassert(midBandComp.ratio != nullptr);
    highBandComp.ratio = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Ratio High"));
    jassert(highBandComp.ratio != nullptr);

    lowBandComp.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Mute Low"));
    jassert(lowBandComp.mute != nullptr);
    midBandComp.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Mute Mid"));
    jassert(midBandComp.mute != nullptr);
    highBandComp.mute = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Mute High"));
    jassert(highBandComp.mute != nullptr);

    lowBandComp.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Comp Off Low"));
    jassert(lowBandComp.bypassed != nullptr);
    midBandComp.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Comp Off Mid"));
    jassert(midBandComp.bypassed != nullptr);
    highBandComp.bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Comp Off High"));
    jassert(highBandComp.bypassed != nullptr);


    lowBandComp.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Solo Low"));
    jassert(lowBandComp.solo != nullptr);
    midBandComp.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Solo Mid"));
    jassert(midBandComp.solo != nullptr);
    highBandComp.solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Solo High"));
    jassert(highBandComp.solo != nullptr);


    LowMidBorder = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Low Mid Cutoff"));
    jassert(LowMidBorder != nullptr);

    MidHighBorder = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Mid High Cutoff"));
    jassert(MidHighBorder != nullptr);

    LowPassFirst.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HighPassFirst.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    AllPassSecond.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    LowPassSecond.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HighPassSecond.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

  /*  invAP1.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    invAP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);*/

}

TheEqualizerAudioProcessor::~TheEqualizerAudioProcessor()
{
}

//==============================================================================
const juce::String TheEqualizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TheEqualizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TheEqualizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TheEqualizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TheEqualizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TheEqualizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TheEqualizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TheEqualizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TheEqualizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void TheEqualizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TheEqualizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //We need to prepare the processor. That is achieved by passing the process spec object to the compressor.

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    juce::Logger::outputDebugString(juce::String(spec.maximumBlockSize));


    leftReverb.prepare(spec);
    rightReverb.prepare(spec);

    { for (auto& comp : compressors) {
        comp.prepare(spec);

    }
    LowPassFirst.prepare(spec);
    HighPassFirst.prepare(spec);
    AllPassSecond.prepare(spec);

    LowPassSecond.prepare(spec);
    HighPassSecond.prepare(spec);

    inputGain.prepare(spec);
    outputGain.prepare(spec);

    inputGain.setRampDurationSeconds(0.05);
    outputGain.setRampDurationSeconds(0.05);
    }


    for (auto& buffer : filterBuffers) 
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }



}

void TheEqualizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TheEqualizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

float TheEqualizerAudioProcessor::hardClipping(float* channelData) {

   
    if (*channelData > 0.7)
    {

        *channelData = 1;

    }
    // if the sample is less than -0.7 set it to -1.0
    if (*channelData < -0.7)

    {

        *channelData = -1;
    }

    return *channelData;
}
float TheEqualizerAudioProcessor::getRMS(const int channel) const {
    jassert(channel == 0 || channel == 1);
    if (channel == 0) {
        return rmsLeft;
    }
    if (channel == 1) {
        return rmsRight;
    }
}
void TheEqualizerAudioProcessor::pushNextSampleIntoFifo(float sample) noexcept
{


}

float TheEqualizerAudioProcessor::softClip(float sample, float drive) {
    const auto softClip = (2 / juce::MathConstants<float>::pi) * std::atanf(sample * juce::Decibels::decibelsToGain(drive));

    return softClip;

}
//2.poglavje, enacbe, opis algoritmov, teoreticno ozadje
//implementacija, eksperimenti, statistika signala pred in po filtriranju
// definirat kompresijo
void TheEqualizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto midHighParamValue = MidHighBorder->get();
    auto lowMidParamValue = LowMidBorder->get();
    milliseconds mil(1000);
    std::chrono::time_point<std::chrono::system_clock> start, end;




    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

  
    wave.pushBuffer(buffer);


    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();


    float input;
    float output;
   
    juce::dsp::AudioBlock<float> audioBlock{ buffer };
    start = std::chrono::system_clock::now();

    if (!distortionoff->get()) {
        for (int sample = 0; sample < audioBlock.getNumSamples(); sample++) {
            //float* channelData = buffer.getWritePointer(channel);
            for (int channel = 0; channel < audioBlock.getNumChannels(); channel++) {

                //float drySignal = *channelData;
                //*channelData *= drive->get() * range->get(); //drive je gain, range je drive


                ////(distorted * blend) + (clean * (1-blend))  ((2.0f / juce::MathConstants<float>::pi) * atan(*channelData))
                ////((distorted * blend) + (clean / blend)) /2
                //*channelData = (((softClip(*channelData) * blend->get()) + (drySignal * (1.0f - blend->get())))) * volume->get();

                //channelData++;

                auto* sampleData = audioBlock.getChannelPointer(channel);
                auto output = (softClip(sampleData[sample] * juce::Decibels::decibelsToGain(drive->get()), range->get()) 
                    * blend->get() 
                    + (sampleData[sample] * juce::Decibels::decibelsToGain(drive->get()))* (1.0 - blend->get()))
                    * juce::Decibels::decibelsToGain(volume->get());
                sampleData[sample] = output;

            }
        }
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    

    LowPassFirst.setCutoffFrequency(lowMidParamValue);
    HighPassFirst.setCutoffFrequency(lowMidParamValue);

    AllPassSecond.setCutoffFrequency(midHighParamValue);
    LowPassSecond.setCutoffFrequency(midHighParamValue);
    HighPassSecond.setCutoffFrequency(midHighParamValue);

    leftReverb.setParameters(ReverbSettings);
    rightReverb.setParameters(ReverbSettings);
    inputGain.setGainDecibels(gainInParam->get());
    outputGain.setGainDecibels(gainOutParam->get());
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftChannelBlock = block.getSingleChannelBlock(0);
    auto rightChannelBlock = block.getSingleChannelBlock(1);
    ReverbSettings.roomSize = *apvts.getRawParameterValue("Size");
    ReverbSettings.damping = *apvts.getRawParameterValue("Damp");
    ReverbSettings.width = *apvts.getRawParameterValue("Width");
    ReverbSettings.wetLevel = *apvts.getRawParameterValue("Wet");
    ReverbSettings.dryLevel = *apvts.getRawParameterValue("Dry");

    juce::dsp::ProcessContextReplacing<float> leftContext(leftChannelBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightChannelBlock);


    if (!reverboff->get()) {
        leftReverb.process(leftContext);
        rightReverb.process(rightContext);
    }

    //applyGain(buffer, inputGain);

    auto blockInputGain = juce::dsp::AudioBlock<float>(buffer);
    auto contextInputGain = juce::dsp::ProcessContextReplacing<float>(blockInputGain);

    inputGain.process(contextInputGain);

    for (auto& comp : compressors) {
        comp.setCompressorParameterValues();

    }

    for (auto& filterBufferCopy : filterBuffers) {
        filterBufferCopy = buffer;
    }

    LowPassFirst.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(filterBuffers[0])));
    AllPassSecond.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(filterBuffers[0])));

    HighPassFirst.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(filterBuffers[1])));

    filterBuffers[2] = filterBuffers[1];
    LowPassSecond.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(filterBuffers[1])));
    HighPassSecond.process(juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(filterBuffers[2])));


    for (size_t i = 0; i < filterBuffers.size(); i++) {

        compressors[i].process(filterBuffers[i]);

    }


 
    buffer.clear();


    auto createBand = [channelSize = numChannels, numOfSamples = numSamples](auto& inputBuffer, const auto& source) {
        for (auto i = 0; i < channelSize; i++) {
            inputBuffer.addFrom(i, 0, source, i, 0, numOfSamples);
        }
    };

    

    auto checkSolo = false;
    for (auto& comp : compressors) {
        if (comp.solo->get()) {
            checkSolo = true;
            break;
        }
    }
  
    if (checkSolo) {
        for (size_t i = 0; i < compressors.size(); ++i) {
            auto& comp = compressors[i];
            if (comp.solo->get()) {
                createBand(buffer, filterBuffers[i]);
            }
        }
    }
    else {
        for (size_t i = 0; i < compressors.size(); ++i) {
            auto& comp = compressors[i];

            if (!comp.mute->get()) {

                createBand(buffer, filterBuffers[i]);
            }
        }
    }
    
    juce::Logger::outputDebugString(juce::String(elapsed_seconds.count()));

    auto blockMakeUpGain = juce::dsp::AudioBlock<float>(buffer);
    auto contextMakeUpGain = juce::dsp::ProcessContextReplacing<float>(blockMakeUpGain);

    outputGain.process(contextMakeUpGain);

    rmsLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    rmsRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

 
}

//==============================================================================
bool TheEqualizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TheEqualizerAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor(*this);
    return new TheEqualizerAudioProcessorEditor (*this);
}

//==============================================================================
void TheEqualizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream send(destData, true);
    apvts.state.writeToStream(send);
}

void TheEqualizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout TheEqualizerAudioProcessor::createParameterLayout() {
    // First parameter will be threshold that will range from -60 and +12, with size of step 1 decibel
    // Threshold will be adjustable by 1 db, skew parameter will affect how the values of threshold are distributed on slider

    APVTS::ParameterLayout layout;
    


    using namespace juce;


    layout.add(std::make_unique<AudioParameterFloat>("drive","Drive", juce::NormalisableRange<float>(-20.0f, 40.0f, 1.0f),1.0f)); // drive bo gain
    layout.add(std::make_unique<AudioParameterFloat>("range", "Range", juce::NormalisableRange<float>(0.0f, 20.0f, 1.0f), 1.0f)); // range bo drive
    layout.add(std::make_unique<AudioParameterFloat>("blend", "Blend", juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f), 1.0f));// blend = mix
    layout.add(std::make_unique<AudioParameterFloat>("volume", "Volume", juce::NormalisableRange<float>(-40.0f, 40.0f, 1.0f), 1.0f)); //    
    layout.add(std::make_unique<AudioParameterBool>("distortionoff", "Distortion Off",false));


    auto gainRange = juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f);
    layout.add(std::make_unique<AudioParameterFloat>(
        "Mix Gain",
        "Mix Gain",
        gainRange,
        0));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Makeup Gain",
        "Makeup Gain",
        gainRange,
        0));

    // THRESHOLD
    layout.add(std::make_unique<AudioParameterFloat>(
        "Threshold Low",
        "Threshold Low",
        // Range start, Range end, interval value, Skew Factor
        NormalisableRange<float>(-60,12,1,1) ,0));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Threshold Mid",
        "Threshold Mid",
        // Range start, Range end, interval value, Skew Factor
        NormalisableRange<float>(-60, 12, 1, 1), 0));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Threshold High",
        "Threshold High",
        // Range start, Range end, interval value, Skew Factor
        NormalisableRange<float>(-60, 12, 1, 1), 0));

    // ATTACK
    auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);
    layout.add(std::make_unique<AudioParameterFloat>(
        "Attack Low",
        "Attack Low",
        attackReleaseRange,
        300));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Attack Mid",
        "Attack Mid",
        attackReleaseRange,
        5));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Attack High",
        "Attack High",
        attackReleaseRange,
        5));

    //RELEASE
    layout.add(std::make_unique<AudioParameterFloat>(
        "Release Low",
        "Release Low",
        attackReleaseRange,
        5));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Release Mid",
        "Release Mid",
        attackReleaseRange,
        5));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Release High",
        "Release High",
        attackReleaseRange,
        5));


    // RATIO
  
    layout.add(std::make_unique<AudioParameterFloat>(
        "Ratio Low",
        "Ratio Low",
        NormalisableRange<float>(1, 100, 1, 1), 1));

    layout.add(std::make_unique<AudioParameterFloat>(
        "Ratio Mid",
        "Ratio Mid",
        NormalisableRange<float>(1, 100, 1, 1), 1));
    layout.add(std::make_unique<AudioParameterFloat>(
        "Ratio High",
        "Ratio High",
        NormalisableRange<float>(1, 100, 1, 1), 1));

    //BYPASS
    layout.add(std::make_unique <AudioParameterBool>(
        "Comp Off Low",
        "Comp Off Low",
        false));
    layout.add(std::make_unique <AudioParameterBool>(
        "Comp Off Mid",
        "Comp Off Mid",
        false));
    layout.add(std::make_unique <AudioParameterBool>(
        "Comp Off High",
        "Comp Off High",
        false));


    layout.add(std::make_unique <AudioParameterBool>(
        "Mute Low",
        "Mute Low",
        false));
    layout.add(std::make_unique <AudioParameterBool>(
        "Mute Mid",
        "Mute Mid",
        false));
    layout.add(std::make_unique <AudioParameterBool>(
        "Mute High",
        "Mute High",
        false));

    layout.add(std::make_unique <AudioParameterBool>(
        "Solo Low",
        "Solo Low",
        false));
    layout.add(std::make_unique <AudioParameterBool>(
        "Solo Mid",
        "Solo Mid",
        false));
    layout.add(std::make_unique <AudioParameterBool>(
        "Solo High",
        "Solo High",
        false));

    layout.add(std::make_unique <AudioParameterFloat>(
        "Low Mid Cutoff",
        "Low Mid Cutoff",
        NormalisableRange<float>(20, 999, 1, 1),
        400));
    layout.add(std::make_unique <AudioParameterFloat>(
        "Mid High Cutoff",
        "Mid High Cutoff",
        NormalisableRange<float>(1000, 20000, 1, 1),
        2000));


    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Size",
        "Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
        0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Damp",
        "Damp",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
        0.5f));


    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Width",
        "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
        0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Dry",
        "Dry",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
        0.5f
        ));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Wet",
        "Wet",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),
        0.5f
        ));
    layout.add(std::make_unique<juce::AudioParameterBool>("reverboff", "reverboff", false));




  

    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheEqualizerAudioProcessor();
}
