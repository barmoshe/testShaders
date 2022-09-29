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

float distanceToLine(vec2 currentP, float fraq) {
    float slope = tan(fraq * 2.0 * PI);
    return (abs((slope * currentP.x) - currentP.y) / sqrt((slope * slope) + 1.0));
}

bool pointInSlice(int i, float angleOfP) {
    return ((when_ge(angleOfP, sliceStart[i]) * when_le(angleOfP, sliceStart[i] + sliceLen[i])) == 1.0);
}
float calcAngleOfPoint(vec2 currentP) {
    float angleOfP=atan(currentP.y, currentP.x);
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
        float distanceToEnd = distanceToLine(currentP, sliceStart[i] + sliceLen[i]);
        float angleOfP = calcAngleOfPoint(currentP);
        if(pointInSlice(i, angleOfP)) {
            paintPoint(i, distToPoint,distanceToStart,distanceToEnd);
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
