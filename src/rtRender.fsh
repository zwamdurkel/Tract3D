#version 460 core
out vec4 FragColor;
uniform sampler2D ourTexture;

uniform int windowW;
uniform int windowH;

void main() {
    FragColor = texture(ourTexture, vec2(gl_FragCoord.x / windowW, gl_FragCoord.y / windowH));
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
