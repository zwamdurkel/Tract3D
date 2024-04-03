#version 460 core
out vec4 FragColor;
uniform sampler2D ourTexture;

uniform int windowW;
uniform int windowH;
uniform bool blurEnabled;
//from https://www.shadertoy.com/view/sl33W4
const vec2 BLUR_DIRECTION_H = vec2(1.0, 0.0);// horizontal pass
const vec2 BLUR_DIRECTION_V = vec2(0.0, 1.0);// vertical pass
const float BLUR_OFFSETS[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
const float BLUR_WEIGHTS[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

vec4 applyBlur()
{
    vec2 resolution = vec2(windowW, windowH);
    float Pi = 6.28318530718;// Pi*2

    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0;// BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 3.0;// BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 0.5;// BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}

    vec2 Radius = Size/resolution;

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy/resolution;
    // Pixel colour
    vec4 Color = texture(ourTexture, uv);

    // Blur calculations
    for (float d=0.0; d<Pi; d+=Pi/Directions)
    {
        for (float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
        {
            Color += texture(ourTexture, uv+vec2(cos(d), sin(d))*Radius*i);
        }
    }

    // Output to screen
    Color /= Quality * Directions - 15.0;
    return Color;
}

void main() {
    if (blurEnabled){
        FragColor = applyBlur();
    } else {
        FragColor = texture(ourTexture, vec2(gl_FragCoord.x / windowW, gl_FragCoord.y / windowH));
    }

    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
