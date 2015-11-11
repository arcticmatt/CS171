//vertex shader
varying vec3 worldPos, camPos;
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;

    worldPos = vec3(gl_Position);
    camPos = vec3(gl_ModelViewMatrix * gl_Vertex);
    gl_Position = ftransform();
}
