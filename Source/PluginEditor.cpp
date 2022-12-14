/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestShadersAudioProcessorEditor::TestShadersAudioProcessorEditor (TestShadersAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 1000);
    startTimer(500.0);
    for (auto i = 0; i < numberOfPizzas; ++i)
    {
        openGLComponents.push_back (std::make_unique<OpenGLComponent>());
        addAndMakeVisible (*openGLComponents.back());
    }
    // Add the child Component and make it visible.
        
}

TestShadersAudioProcessorEditor::~TestShadersAudioProcessorEditor()
{
}

//==============================================================================
void TestShadersAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    if(first)
        if(openGLComponents.size()>0){
            int index=0;
            for(int row=0;row<5;row++)
            {
                for(int col=0;col<5;col++)
                {
                    openGLComponents.at(index)->setBounds(col*200,row*200, 200, 200);
                    index+=1;
                }
            }
            //     This is generally where you'll want to lay out the positions of any
            //     subcomponents in your editor..
            first=false;
        }
    
}
void TestShadersAudioProcessorEditor::trigerRandom(){
    if(index>=numberOfPizzas)
        index=0;
    openGLComponents.at(index)->setRandom(true);
    openGLComponents.at(lastTriggerd)->setRandom(false);
    lastTriggerd=index;
    index++;


}
void TestShadersAudioProcessorEditor::resized()
{
    if(openGLComponents.size()>0){
        int index=0;
        for(int row=0;row<5;row++)
        {
            for(int col=0;col<10;col++)
            {
                openGLComponents.at(0)->setBounds(col*100,row*100, 100, 100);
                index+=1;
            }
        }
        //     This is generally where you'll want to lay out the positions of any
        //     subcomponents in your editor..
    }
    
}
