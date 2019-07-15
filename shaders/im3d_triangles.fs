R""(
#version 300 es
precision mediump float;
#define kAntialiasing 2.0

/*noperspective*/ in float vSize;
smooth in vec4 vColor;

out vec4 fResult;

void main()
{
    fResult = vColor;
}
)""