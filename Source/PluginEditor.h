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
class TestShadersAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TestShadersAudioProcessorEditor (TestShadersAudioProcessor&);
    ~TestShadersAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
 
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TestShadersAudioProcessor& audioProcessor;
    // Create an instance of the OpenGLComponent.
       OpenGLComponent openGLComponent1;
    OpenGLComponent openGLComponent2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestShadersAudioProcessorEditor)
};
