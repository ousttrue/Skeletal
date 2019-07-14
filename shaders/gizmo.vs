R""(
#version 300 es
in vec3 POSITION;   
in vec3 COLOR_0;
out vec3 f_Color;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVPMatrix;

void main()                 
{                           
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(POSITION, 1);  
    // gl_Position = MVPMatrix * vec4(vPosition, 1);

    f_Color = COLOR_0;
}                           
)""