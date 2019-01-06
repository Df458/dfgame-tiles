#version 330
layout(points) in;
layout(triangle_strip, max_vertices=4) out;
in vec4 v_uv[];

uniform mat4 u_view;
uniform mat4 u_transform;
uniform vec2 u_dims;

out vec2 o_uv;

void main() {
    mat4 transform = u_transform * u_view;
    gl_Position = gl_in[0].gl_Position;
    o_uv = v_uv[0].xy;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + transform * vec4(u_dims.x, 0, 0, 0);
    o_uv = v_uv[0].xy + vec2(v_uv[0].z, 0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + transform * vec4(0, u_dims.y, 0, 0);
    o_uv = v_uv[0].xy + vec2(0, v_uv[0].w);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + transform * vec4(u_dims.x, u_dims.y, 0, 0);
    o_uv = v_uv[0].xy + v_uv[0].zw;
    EmitVertex();
    EndPrimitive();
}
