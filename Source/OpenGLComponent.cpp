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
    //openGLContext.setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2)
    
    circle.radius=0.5;
    circle.lineWidth=0.044*circle.radius;
    circle.activeSlices=5;
    circle.sliceStart[0]= 0.75;
    circle.sliceLen[0]=1.0/float(circle.activeSlices);
    for(int i =1;i< circle.activeSlices;i++)
    {
        circle.sliceRed[i]=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        circle.sliceBlue[i]=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        circle.sliceGreen[i]=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        circle.sliceAlpha[i]=1.0;
        circle.sliceStart[i]=addAngle(circle.sliceStart[i-1], -circle.sliceLen[i-1]);
        circle.sliceLen[i]=1.0/float(circle.activeSlices);
        
        
        
    }
    //    circle.sliceRed[1]=1.0;
    //    circle.sliceBlue[1]=0.0;
    //    circle.sliceGreen[1]=0.0;
       circle.sliceRed[0]=1.0;
        circle.sliceBlue[0]=0.0;
        circle.sliceGreen[0]=0.0;
      //  circle.sliceStart[0]= 0.75;
    //    circle.sliceLen[0]=(2.0/4.0);
    //    circle.sliceStart[1]=0.25;
    //    circle.sliceLen[1]=(1.0/2.0);
    
    
    for(int i =0;i< circle.activeSlices;i++)
    {
        std::cout<<"\n["<<i<<"]\nred - "<< circle.sliceRed[i]<<"\nGreen - "<< circle.sliceGreen[i]<<"\nblue -"<< circle.sliceBlue[i]<<"\nstart - "<<circle.sliceStart[i]<<"\nlen - "<<circle.sliceLen[i]<<"\n";
    }
}

OpenGLComponent::~OpenGLComponent()
{
    // Tell the context to stop using this Component.
    openGLContext.detach();
}
float OpenGLComponent::addAngle(float x, float y) {
    x = x + y;
    if(x<0.0)
        return x+1.0;
    else
        if(x>1.0)
            return x-1.0;
    return x;
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
            void main(){
               gl_Position = a_Position;
            }
        )";
    
    fragmentShader =
            R"(
            
            #define PI 3.1415926538
            #define ZERO 1e-6
            vec2 middle = vec2(0.0, 0.0);
            uniform vec2 u_resolution;
            uniform float radius;
            uniform float lineWidth;
            uniform int activeSlices;
            uniform float sliceStart[128];
            uniform float sliceLen[128];
            uniform float sliceRed[128];
            uniform float sliceGreen[128];
            uniform float sliceBlue[128];
            uniform float sliceAlpha[128];
            uniform bool changed;
            vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
            float when_gt(float x, float y) {
            return max(sign(x - y), 0.0);
            }
            float when_le(float x, float y) {
            return 1.0 - when_gt(x, y);
            }
            float when_lt(float x, float y) {
            return max(sign(y - x), 0.0);
            }
            float when_ge(float x, float y) {
            return 1.0 - when_lt(x, y);
            }
            float addAngle(float x, float y) {
            x = x + y;
            x = x - 1.0 * when_gt(x, 1.0);
            x = x + 1.0 * when_lt(x, 0.0);
            return x;
            }
            
            float distanceToLine(vec2 currentP, float fraq) {
            if(fraq == 0.25 || fraq == 0.75)
            return abs(currentP.x);
            float slope = tan(fraq * 2.0 * PI);
            return (abs((slope * currentP.x) - currentP.y) / sqrt((slope * slope) + 1.0));
            }
            bool pointInSlice(int i, float angleOfP) {
            return ((when_le(angleOfP, sliceStart[i]) * when_ge(angleOfP, addAngle(sliceStart[i] ,- sliceLen[i]))) == 1.0);
            }
            float calcAngleOfPoint(vec2 currentP) {
            float angleOfP = atan(currentP.y, currentP.x);
            angleOfP += when_lt(currentP.y, 0.0) * 2.0 * PI;
            angleOfP /= (2.0 * PI);
            return angleOfP;
            
            }
            void paintPoint(int i, float distToPoint, float distanceToStart, float distanceToEnd) {
            
            vec4 backgroundColour = vec4(20.0 / 255.0, 20.0 / 255.0, 47.0 / 255.0, 1.0);//background
            
            float outerRadius = radius + lineWidth;
            float outerRadiusBorder = outerRadius + lineWidth;
            if(when_gt(distToPoint, outerRadiusBorder) == 1.0)//backGround
            gl_FragColor = backgroundColour;
            else if(when_lt(distToPoint, outerRadiusBorder) * when_lt(outerRadius, distToPoint) == 1.0)//anti aliasd border with background
            gl_FragColor = mix(white, backgroundColour, (distToPoint - outerRadius) / (outerRadiusBorder - outerRadius));
            else if(when_lt(distanceToStart, lineWidth / 2.0) + when_lt(distanceToEnd, lineWidth / 2.0) != 0.0)
            gl_FragColor = white;
            else if(when_lt(distToPoint, radius) == 1.0)
            gl_FragColor = vec4(sliceRed[i], sliceGreen[i], sliceBlue[i], 1.0);
            else
            gl_FragColor = mix(vec4(sliceRed[i], sliceGreen[i], sliceBlue[i], 1.0), white, (distToPoint - radius) / (outerRadius - radius));
            
            }
            void CalcNewCircle() {
            
            vec2 currentP = (gl_FragCoord.xy / u_resolution) - 1.0;//currnt point in x,y the middle of the axis is center of shader
            float yOffset = u_resolution.x / u_resolution.y;
            float distToPoint = distance(vec2(currentP.x, currentP.y / yOffset), vec2(0.0, 0.0));
            for(int i = 0; i < activeSlices; i++) {
            float distanceToStart = distanceToLine(currentP, sliceStart[i]);
            float distanceToEnd = distanceToLine(currentP, addAngle(sliceStart[i] ,- sliceLen[i]));
            float angleOfP = calcAngleOfPoint(currentP);
            if(pointInSlice(i, angleOfP)) {
            paintPoint(i, distToPoint, distanceToStart, distanceToEnd);
            break;
            } else
            gl_FragColor = gl_FragColor;
            }
            
            }
            void displayTexture() {
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            }
            void main() {
            if(changed)
            CalcNewCircle();
            else
            displayTexture();
            
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
        
    }
    else
    {
        // Oops - something went wrong with our shaders!
        // Check the output window of your IDE to see what the error might be.
        jassertfalse;
    }
    
}

static void enableThePositionAttribute(const juce::OpenGLContext &openGLContext) {
    openGLContext.extensions.glVertexAttribPointer(
                                                   0,              // The attribute's index (AKA location).
                                                   2,              // How many values this attribute contains.
                                                   GL_FLOAT,       // The attribute's type (float).
                                                   GL_FALSE,       // Tells OpenGL NOT to normalise the values.
                                                   sizeof(OpenGLComponent::Vertex), // How many bytes to move to find the attribute with
                                                   // the same index in the next vertex.
                                                   nullptr         // How many bytes to move from the start of this vertex
                                                   // to find this attribute (the default is 0 so we just
                                                   // pass nullptr here).
                                                   );
    openGLContext.extensions.glEnableVertexAttribArray(0);
}

void OpenGLComponent::renderOpenGL()
{
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    srand(time(0));
    OpenGLHelpers::clear(Colour(20, 20, 47));
    shaderProgram->use();
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObj);
    enableThePositionAttribute(openGLContext);
    
    
    glDrawElements(
                   GL_TRIANGLE_FAN,       // Tell OpenGL to render triangles.
                   indexBuffer.size(), // How many indices we have.
                   GL_UNSIGNED_INT,    // What type our indices are.
                   nullptr             // We already gave OpenGL our indices so we don't
                   // need to pass that again here, so pass nullptr.
                   );
    
    openGLContext.extensions.glDisableVertexAttribArray(0);
    shaderProgram->setUniform("u_resolution",   1000.0, 1000.0);
    shaderProgram->setUniform("radius", circle.radius);
    shaderProgram->setUniform("lineWidth", circle.lineWidth);
    shaderProgram->setUniform("activeSlices", circle.activeSlices);
    shaderProgram->setUniform("sliceStart", circle.sliceStart,128);
    shaderProgram->setUniform("sliceRed", circle.sliceRed,128);
    shaderProgram->setUniform("sliceBlue", circle.sliceBlue,128);
    shaderProgram->setUniform("sliceGreen", circle.sliceGreen,128);
    shaderProgram->setUniform("sliceLen", circle.sliceLen,128);
    shaderProgram->setUniform("changed", firstTime);
    
    
    //        circle.activeSlices=((rand() %1) + 2) ;
    //    //circle.activeSlices=3;
    //        for(int i =0;i< circle.activeSlices;i++)
    //        {
    //            circle.sliceRed[i]=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    //            circle.sliceBlue[i]=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    //            circle.sliceAlpha[i]=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    //            circle.sliceStart[i]= i/circle.activeSlices;
    //            circle.sliceLen[i]=1.0/circle.activeSlices;
    //            if(i>0)
    //                circle.sliceStart[i]=circle.sliceStart[i-1]+circle.sliceLen[i];
    //            circle.sliceGreen[i]=static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    //
    //            //rstd::cout<<"\n["<<i<<"]\nred - "<< circle.sliceRed[i]<<"\nGreen - "<< circle.sliceGreen[i]<<"\nblue -"<< circle.sliceBlue[i];
    //        }
    
    //
    //        firstTime=false;
    
}

void OpenGLComponent::openGLContextClosing()
{
}
void OpenGLComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
}
