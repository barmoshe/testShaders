#define PI 3.1415926538
#define ZERO 1e-6
struct Slice {
    float start;
    float lenght;
    vec4 colour;
};
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
vec2 findXY(float angle) {
    angle = ((360.0 * angle)) * PI / 180.0;
    return vec2(radius * sin(angle), radius * cos(angle));
}

float when_le(float x, float y) {
    return 1.0 - when_gt(x, y);
}

float when_lt(float x, float y) {
    return max(sign(y - x), 0.0);
}
float when_eq(float x, float y) {
    return 1.0 - abs(sign(x - y));
}

float when_neq(float x, float y) {
    return abs(sign(x - y));
}

float when_gt(float x, float y) {
    return max(sign(x - y), 0.0);
}

float when_ge(float x, float y) {
    return 1.0 - when_lt(x, y);
}

float circleAdd(float x, float y) {
    x = x + y;
    x = x * (1.0 - (2.0 * when_lt(x, 0.0)));
    return x - floor(x);
}

float is_ge(float x, float y) {
    return max(when_gt(x, y), when_lt(abs(x - y), ZERO));
}

void main() {

    vec4 backgroundColour = vec4(20.0 / 255.0, 20.0 / 255.0, 47.0 / 255.0, 1.0);//background
    vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
    vec2 currentP = (gl_FragCoord.xy / u_resolution) - 1.0;//currnt point in x,y the middle of the axis is center of shader
    float yOffset = u_resolution.x / u_resolution.y;
    float distance1 = distance(vec2(currentP.x, currentP.y / yOffset), vec2(0.0, 0.0));
    float outerRadius = radius + lineWidth;
    float outerRadiusBorder = outerRadius + lineWidth;
    if(distance1 < outerRadius) { //inside the circle border
        float y = 0.0;
        for(int i = 0; i < activeSlices; i++) {
            //vec2 x1y1=findXY(sliceStart[i]);//slice start
            //vec2 x2y2=findXY(sliceStart[i]+sliceLen[i]);//slice ends
            //if((0.0-x1y1.y/0.0-x1y1.x)==(0.0-currentP.y/0.0-currentP.x))
            float angleOfP = atan(currentP.y, currentP.x);
            angleOfP += when_lt(currentP.y, 0.0) * 2.0 * PI;
            angleOfP /= 2.0;
            angleOfP /= PI;
            if(i == 0 || i == activeSlices - 1) {

                if(circleAdd(angleOfP, -sliceStart[i]) < sliceLen[i]) {
                    gl_FragColor = vec4(sliceRed[i], 0.0, 0.0, 1.0);
                    break;
                } else
                    gl_FragColor = white;
            } else {
                if(is_ge(angleOfP, sliceStart[i]) && !is_ge(angleOfP, sliceStart[i + 1]) {

                    gl_FragColor = vec4(sliceRed[i], 0.0, 0.0, 1.0);
                    break;
                } else
                    gl_FragColor = white;

            }
        }
    } else if(distance1 < outerRadiusBorder)//anti aliasd border with background
        gl_FragColor = mix(white, backgroundColour, (distance1 - outerRadius) / (outerRadiusBorder - outerRadius));
    else
        gl_FragColor = backgroundColour;

                   //     vec4 kickColour = vec4(165.0 / 255.0, 43.0 / 255.0, 90.0 / 255.0, 1.0);//kick color
                   //       vec4 clapColour = vec4(255.0 / 255.0, 20.0 / 255.0, 48.0 / 255.0, 1.0);//clap color
                   //    if(distance1 < outerRadius) {//inside the circle border
                   //          gl_FragColor = white;
                   //          if(distance1 < innerRadius)//inside circle it self
                   //                                         //**(here will be slices logic)**
                   //              if(currentP.x > 0.0)
                   //                  if(currentP.x < lineWidth / 2.0)//if in the middle line from the positive side
                   //                      gl_FragColor = white;
                   //                  else
                   //                      gl_FragColor = kickColour;
                   //              else if(currentP.x > -lineWidth / 2.0)//if in the middle line from the negative side
                   //                  gl_FragColor = white;
                   //              else
                   //                  gl_FragColor = clapColour;
                   //          else//anti alias with border
                   //          if(currentP.x > 0.0)
                   //              if(currentP.x < lineWidth / 2.0)
                   //                  gl_FragColor = white;
                   //              else
                   //                  gl_FragColor = mix(kickColour, white, (distance1 - innerRadius) / (outerRadius - innerRadius));
                   //          else if(currentP.x > -lineWidth / 2.0)
                   //              gl_FragColor = white;
                   //          else
                   //              gl_FragColor = mix(clapColour, white, (distance1 - innerRadius) / (outerRadius - innerRadius));
                   //
                   //      } else if(distance1 > outerRadiusBorder)//background color
                   //          gl_FragColor = backgroundColour;
                   //      else//anti aliasd border with background
                   //          gl_FragColor = mix(white, backgroundColour, (distance1 - outerRadius) / (outerRadiusBorder - outerRadius));

}
