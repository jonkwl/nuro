#version 330 core 

out vec4 FragColor;

in vec2 v_uv;

uniform sampler2D icon;
uniform vec3 tint;
uniform float alpha;

void main() {
    vec4 color = texture(icon, v_uv).rgba * vec4(tint, alpha);
    FragColor = color;
}