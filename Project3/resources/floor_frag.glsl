#version 330 core

out vec4 FragColor;

in vec3 TexCoords;

uniform sampler2D floorText;

void main() {
    FragColor = texture(floorText, TexCoords.xy);
}