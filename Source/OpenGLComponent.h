/*
 ==============================================================================
 
 OpenGLComponent.h
 Created: 7 Sep 2022 1:54:48pm
 Author:  Bar moshe
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
using namespace juce;
//==============================================================================
/*
 */
class OpenGLComponent    : public Component,
public OpenGLRenderer
{
public:
    OpenGLComponent();
    ~OpenGLComponent() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;
    struct Vertex
    {
        float position[2];
        float colour[4];
    };
    
private:
    // Create an OpenGLContext for this Component.
    OpenGLContext openGLContext;
    
    struct Circle
    {
        float radius=0.0;
        float lineWidth=0.0;
        int activeSlices = 14;
        float sliceStart[128];
        float sliceLen[128];
        float sliceRed[128];
        float sliceGreen[128];
        float sliceBlue[128];
        float sliceAlpha[128];


    };
    
    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;
    GLuint  vertexBufferObj; // Vertex buffer object.
    GLuint indexBufferObj; // Index buffer object.
    std::string vertexShader;
    std::string fragmentShader;
    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    Circle circle;
    bool firstTime=true;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent)
};
