/*
 ==============================================================================
 
 OpenGLComponent.cpp
 Created: 7 Sep 2022 1:54:48pm
 Author:  Bar moshe
 
 ==============================================================================
 */

#include <JuceHeader.h>
#include "OpenGLComponent.h"
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
    
    circle.radius=0.2;
    circle.lineWidth=0.012;
    
    
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
            { -1.0f, 1.0f }        // (-0.5, 0.5)
        },
        // Vertex 1
        {
            { 1.0f, 1.0f }        // (0.5, 0.5)
        },
        // Vertex 2
        {
            { 1.0f, -1.0f }        // (0.5, -0.5)
        },
        // Vertex 3
        {
            { -1.0f, -1.0f }      // (-0.5, -0.5)
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
                                          GL_DYNAMIC_DRAW                          // How we want the buffer to be drawn.
                                          );
    
    // Bind the IBO.
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObj);
    
    // Send the indices data.
    openGLContext.extensions.glBufferData(
                                          GL_ELEMENT_ARRAY_BUFFER,
                                          sizeof(unsigned int) * indexBuffer.size(),
                                          indexBuffer.data(),
                                          GL_DYNAMIC_DRAW
                                          );
    
    
    vertexShader =
                R"(
           attribute vec4 a_Position;
            void main(){
               gl_Position = a_Position;
            }
        )";
    
    fragmentShader =
            R"(
              struct Slice {
                  float start;
                  float lenght;
                  vec4 colour;
              };
              uniform vec2 u_resolution;
              uniform float innerRadius;
              uniform float lineWidth;
              void main() {
                  vec4 backgroundColour = vec4(20.0 / 255.0, 20.0 / 255.0, 47.0 / 255.0, 1.0);//background
                  vec4 kickColour = vec4(165.0 / 255.0, 43.0 / 255.0, 90.0 / 255.0, 1.0);//kick color
                  vec4 clapColour = vec4(255.0 / 255.0, 20.0 / 255.0, 48.0 / 255.0, 1.0);//clap color
                  vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
                  vec2 currentP = (gl_FragCoord.xy / u_resolution) - 1.0;//currnt point in x,y
                  float yOffset = u_resolution.x / u_resolution.y;
                  float distance1 = distance(vec2(currentP.x, currentP.y / yOffset), vec2(0.0, 0.0));
                  float outerRadius = innerRadius + lineWidth;
                  float outerRadiusBorder = outerRadius + lineWidth;
                  if(distance1 < outerRadius) {//inside the circle border
                      gl_FragColor = white;
                      if(distance1 < innerRadius)//inside circle it self
                                                     //**(here will be slices logic)**
                          if(currentP.x > 0.0)
                              if(currentP.x < lineWidth / 2.0)//if in the middle line from the positive side
                                  gl_FragColor = white;
                              else
                                  gl_FragColor = kickColour;
                          else if(currentP.x > -lineWidth / 2.0)//if in the middle line from the negative side
                              gl_FragColor = white;
                          else
                              gl_FragColor = clapColour;
                      else//anti alias with border
                      if(currentP.x > 0.0)
                          if(currentP.x < lineWidth / 2.0)
                              gl_FragColor = white;
                          else
                              gl_FragColor = mix(kickColour, white, (distance1 - innerRadius) / (outerRadius - innerRadius));
                      else if(currentP.x > -lineWidth / 2.0)
                          gl_FragColor = white;
                      else
                          gl_FragColor = mix(clapColour, white, (distance1 - innerRadius) / (outerRadius - innerRadius));
            
                  } else if(distance1 > outerRadiusBorder)//background color
                      gl_FragColor = backgroundColour;
                  else//anti aliasd border with background
                      gl_FragColor = mix(white, backgroundColour, (distance1 - outerRadius) / (outerRadiusBorder - outerRadius));
            
              }
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
        shaderProgram->setUniform("innerRadius", circle.radius);
        shaderProgram->setUniform("lineWidth", circle.lineWidth);
        shaderProgram->setUniform("activeSlices", circle.activeSlices);
        
        
        
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
    OpenGLHelpers::clear(Colours::cyan);
    
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
    
    //    // Enable to colour attribute.
    //    openGLContext.extensions.glVertexAttribPointer(
    //                                                   1,                              // This attribute has an index of 1
    //                                                   4,                              // This time we have four values for the
    //                                                   // attribute (r, g, b, a)
    //                                                   GL_FLOAT,
    //                                                   GL_FALSE,
    //                                                   sizeof(Vertex),
    //                                                   (GLvoid*)(sizeof(float) * 2)    // This attribute comes after the
    //                                                   // position attribute in the Vertex
    //                                                   // struct, so we need to skip over the
    //                                                   // size of the position array to find
    //                                                   // the start of this attribute.
    //                                                   );
    //    openGLContext.extensions.glEnableVertexAttribArray(1);
    glEnable(GL_MULTISAMPLE);
    glDrawElements(
                   GL_TRIANGLE_FAN,       // Tell OpenGL to render triangles.
                   indexBuffer.size(), // How many indices we have.
                   GL_UNSIGNED_INT,    // What type our indices are.
                   nullptr             // We already gave OpenGL our indices so we don't
                   // need to pass that again here, so pass nullptr.
                   );
    
    openGLContext.extensions.glDisableVertexAttribArray(0);
}

void OpenGLComponent::openGLContextClosing()
{
}
void OpenGLComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
}
