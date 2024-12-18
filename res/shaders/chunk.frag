#version 430
out vec4 v_frag;

in vec2 v_texcoord;

void main() {
    v_frag = vec4(v_texcoord, 0.0, 1.0);
}
