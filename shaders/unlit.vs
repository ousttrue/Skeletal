#version 300 es
precision mediump float;                   
in vec3 POSITION;
in vec3 NORMAL;
in vec2 TEXCOORD_0;

out vec3 f_Normal;
out vec2 f_TexCoord;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(POSITION, 1);  
    // gl_Position = MVPMatrix * vec4(POSITION, 1);
    f_Normal = (ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(NORMAL, 0)).xyz;
    f_TexCoord = TEXCOORD_0;
}
