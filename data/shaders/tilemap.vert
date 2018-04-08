#version 330
in vec3 i_pos;
in vec4 i_uv;

uniform mat4 u_transform;
uniform mat4 u_view;
uniform vec2 u_dims;

out vec4 v_uv;

void main() {
    gl_Position = u_view * u_transform * vec4(i_pos.x * u_dims.x, i_pos.y * u_dims.y, 0, 1);
    v_uv = i_uv;
}
