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
    
    
private:
    // Create an OpenGLContext for this Component.
    OpenGLContext openGLContext;
    struct Vertex
    {
        float position[2];
        float colour[4];
    };
    struct Slice
    {
        float start;
        float lenght;
        float colour[4];
    };
    struct Circle
    {
        float radius;
        float lineWidth;
        int activeSlices = 14;
        float slices[128];
    };
    
    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;
    GLuint  vertexBufferObj; // Vertex buffer object.
    GLuint indexBufferObj; // Index buffer object.
    std::string vertexShader;
    std::string fragmentShader;
    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    Circle circle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent)
};
