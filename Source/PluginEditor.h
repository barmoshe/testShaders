/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Include the OpenGLComponent.h file.
#include "OpenGLComponent.h"
//==============================================================================
/**
 */
class TestShadersAudioProcessorEditor  : public juce::AudioProcessorEditor,
private juce::Timer
{
public:
    TestShadersAudioProcessorEditor (TestShadersAudioProcessor&);
    ~TestShadersAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void trigerRandom();
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TestShadersAudioProcessor& audioProcessor;
    // Create an instance of the OpenGLComponent.
    //       OpenGLComponent openGLComponent;
    //    OpenGLComponent openGLComponent1;
    bool first=true;
    
    std::vector<std::unique_ptr<OpenGLComponent>> openGLComponents;
    void timerCallback() final
    {
        trigerRandom();
    }
    int numberOfPizzas=25
    ;
    int lastTriggerd=0;
    int index;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestShadersAudioProcessorEditor)
};
