#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec2 a_TilingFactor;
layout(location = 4) in int a_EntityID;
layout(location = 5) in int a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

struct VertexOutput
{
    vec4 Color;
    vec2 TexCoord;
    vec2 TilingFactor;
};

layout(location = 0) out VertexOutput Output;
layout(location = 3) out flat int v_EntityID;
layout(location = 4) out flat int v_TexIndex;

void main()
{
    Output.Color = a_Color;
    Output.TexCoord = a_TexCoord;
    Output.TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;
    v_TexIndex = a_TexIndex;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
    vec4 Color;
    vec2 TexCoord;
    vec2 TilingFactor;
};

layout(location = 0) in VertexOutput Input;
layout(location = 3) in flat int v_EntityID;
layout(location = 4) in flat int v_TexIndex;

uniform sampler2D u_Textures[32]; // Support up to 32 textures

void main()
{
    vec4 texColor = texture(u_Textures[int(v_TexIndex)], Input.TexCoord);
    vec4 finalColor = texColor * Input.Color; // Blend the texture color with the vertex color

    o_Color = finalColor;
    o_EntityID = v_EntityID;
}
