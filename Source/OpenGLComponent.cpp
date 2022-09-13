/*
 ==============================================================================
 
 OpenGLComponent.cpp
 Created: 7 Sep 2022 1:54:48pm
 Author:  Bar moshe
 
 ==============================================================================
 */

#include <JuceHeader.h>
#include "OpenGLComponent.h"
#define NUMBER_OF_VERTICES 1000
using namespace ::juce::gl;

//==============================================================================
OpenGLComponent::OpenGLComponent()
{
    // Indicates that no part of this Component is transparent.
    setOpaque(true);
    
    // Set this instance as the renderer for the context.
    openGLContext.setRenderer(this);
    
    // Tell the context to repaint on a loop.
    openGLContext.setContinuousRepainting(true);
    
    openGLContext.setMultisamplingEnabled(true);
    // Setup a pixel format object to tell the context what level of
    // multisampling to use.
    OpenGLPixelFormat pixelFormat;
    pixelFormat.multisamplingLevel = 4; // Change this value to your needs.
    openGLContext.setPixelFormat(pixelFormat);
    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);
    //openGLContext.setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
}

OpenGLComponent::~OpenGLComponent()
{
    // Tell the context to stop using this Component.
    openGLContext.detach();
}

void OpenGLComponent::paint (juce::Graphics& g)
{
}

void OpenGLComponent::newOpenGLContextCreated()
{
    
    // Generate 1 buffer, using our vbo variable to store its ID.
    openGLContext.extensions.glGenBuffers(1, &vertexBufferObj);
    
    // Generate 1 more buffer, this time using our IBO variable.
    openGLContext.extensions.glGenBuffers(1, &indexBufferObj);
    
    //    float radius1 = 0.5;
    //
    //    for(double i = 0; i < 2 * M_PI; i += 2 * M_PI / NUMBER_OF_VERTICES){
    //        Vertex v={{ float(cos(i) * radius1), float(sin(i) * radius1 )},{ 0.f, 0.f, 0.f, 1.f }};
    //        vertexBuffer.push_back(v);
    //    }
    
   
    //
    // Create four vertices each with a different colour.
    vertexBuffer = {
        // Vertex 0
        {
            { -1.0f, 1.0f },        // (-0.5, 0.5)
            { 1.f, 0.f, 0.f, 1.f }  // Red
        },
        // Vertex 1
        {
            { 1.0f, 1.0f },         // (0.5, 0.5)
            { 1.f, 0.5f, 0.f, 1.f } // Orange
        },
        // Vertex 2
        {
            { 1.0f, -1.0f },        // (0.5, -0.5)
            { 1.f, 1.f, 0.f, 1.f }  // Yellow
        },
        // Vertex 3
        {
            { -1.0f, -1.0f },       // (-0.5, -0.5)
            { 1.f, 0.f, 1.f, 1.f }  // Purple
        }
    };
    
    // We need 6 indices, 1 for each corner of the two triangles.
    indexBuffer = {
        0, 1, 2,
        0, 2, 3
    };
    
    // Bind the VBO.
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
    
    // Send the vertices data.
    openGLContext.extensions.glBufferData(
                                          GL_ARRAY_BUFFER,                        // The type of data we're sending.
                                          sizeof(Vertex) * vertexBuffer.size(),   // The size (in bytes) of the data.
                                          vertexBuffer.data(),                    // A pointer to the actual data.
                                          GL_STATIC_DRAW                          // How we want the buffer to be drawn.
                                          );
    
    // Bind the IBO.
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObj);
    
    // Send the indices data.
    openGLContext.extensions.glBufferData(
                                          GL_ELEMENT_ARRAY_BUFFER,
                                          sizeof(unsigned int) * indexBuffer.size(),
                                          indexBuffer.data(),
                                          GL_STATIC_DRAW
                                          );
    
    
    vertexShader =
                R"(
           attribute vec4 a_Position;
            attribute vec4 a_Color;
           varying vec4 frag_Color;
        
            void main(){
               frag_Color = a_Color;
               gl_Position = a_Position;
            }
        )";
    
    fragmentShader =
            R"(
            varying vec4 frag_Color;
              uniform vec2  u_resolution;
            float aspect = u_resolution.x/ u_resolution.y;
            vec2 px = vec2(1.0/u_resolution.x, 1.0/u_resolution.y);

            float getDistance(vec2 pixelCoord, vec2 playerCoord) {
                vec2 p = playerCoord * px * vec2(1.0, 1.0/aspect);
                // pixelCoord is normalized, but playerCoord is passed in as-is
                return distance(pixelCoord, p);
            }
            void main()
            {
                float x1=20.0/255.0;
                float x2=47.0/255.0;
                 vec4 colour2 = vec4(x1,x1,x2,1.0);
                 vec4 colour1 = vec4 (165.0/255.0, 43.0/255.0,90.0/255.0, 1.0);
                vec4 colour3 = vec4 (255.0/255.0, 20.0/255.0,48.0/255.0, 1.0);

                vec2 currentP=(gl_FragCoord.xy/u_resolution)-1.0;
                float yOffset=u_resolution.x/u_resolution.y;
                 float distance1 = distance (vec2(currentP.x,currentP.y/yOffset), vec2(0.0,0.0));
                 float innerRadius =0.2;
                 float outerRadius = 0.22;
                if(currentP.x<0.0)
                    if(distance1 < innerRadius)
                        gl_FragColor = colour1;
                    else
                        
                    if (distance1 > outerRadius)
                        gl_FragColor = colour2;
                    else
                        gl_FragColor = mix (colour1, colour2, (distance1 - innerRadius) / (outerRadius - innerRadius));
                else
                    
                    if(distance1 < innerRadius)
                        gl_FragColor = colour3;
                    else
                        
                    if (distance1 > outerRadius)
                        gl_FragColor = colour2;
                    else
                        gl_FragColor = mix (colour3, colour2, (distance1 - innerRadius) / (outerRadius - innerRadius));            }
            )";
    
    
    
    // Create an instance of OpenGLShaderProgram
    shaderProgram.reset(new OpenGLShaderProgram(openGLContext));
    
    // Compile and link the shader.
    // Each of these methods will return false if something goes wrong so we'll
    // wrap them in an if statement
    if (shaderProgram->addShader(vertexShader, GL_VERTEX_SHADER)
        && shaderProgram->addFragmentShader(fragmentShader)
        && shaderProgram->link())
    {
        // No compilation errors - set the shader program to be active
        shaderProgram->use();
        shaderProgram->setUniform("u_resolution", 400.0, 200.0);

    }
    else
    {
        // Oops - something went wrong with our shaders!
        // Check the output window of your IDE to see what the error might be.
        jassertfalse;
    }
    
}

void OpenGLComponent::renderOpenGL()
{
    
    // Clear the screen by filling it with black.
    OpenGLHelpers::clear(Colours::whitesmoke);
 
    // Tell the renderer to use this shader program
    shaderProgram->use();
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObj);
    // Enable the position attribute.
    openGLContext.extensions.glVertexAttribPointer(
                                                   0,              // The attribute's index (AKA location).
                                                   2,              // How many values this attribute contains.
                                                   GL_FLOAT,       // The attribute's type (float).
                                                   GL_FALSE,       // Tells OpenGL NOT to normalise the values.
                                                   sizeof(Vertex), // How many bytes to move to find the attribute with
                                                   // the same index in the next vertex.
                                                   nullptr         // How many bytes to move from the start of this vertex
                                                   // to find this attribute (the default is 0 so we just
                                                   // pass nullptr here).
                                                   );
    openGLContext.extensions.glEnableVertexAttribArray(0);
    
    // Enable to colour attribute.
    openGLContext.extensions.glVertexAttribPointer(
                                                   1,                              // This attribute has an index of 1
                                                   4,                              // This time we have four values for the
                                                   // attribute (r, g, b, a)
                                                   GL_FLOAT,
                                                   GL_FALSE,
                                                   sizeof(Vertex),
                                                   (GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
                                                   // position attribute in the Vertex
                                                   // struct, so we need to skip over the
                                                   // size of the position array to find
                                                   // the start of this attribute.
                                                   );
    openGLContext.extensions.glEnableVertexAttribArray(1);
    glEnable(GL_MULTISAMPLE);
    glDrawElements(
                   GL_TRIANGLE_FAN,       // Tell OpenGL to render triangles.
                   indexBuffer.size(), // How many indices we have.
                   GL_UNSIGNED_INT,    // What type our indices are.
                   nullptr             // We already gave OpenGL our indices so we don't
                   // need to pass that again here, so pass nullptr.
                   );
   
    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);
}

void OpenGLComponent::openGLContextClosing()
{
}
void OpenGLComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
}
