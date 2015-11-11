//vertex shader
varying vec3 worldPos, camPos;
varying vec3 lightVec;
uniform vec3 tangent, binormal;
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;

    mat3 m;
    // Column major
    m[0][0] = tangent[0];
    m[1][0] = tangent[1];
    m[2][0] = tangent[2];
    m[0][1] = binormal[0];
    m[1][1] = binormal[1];
    m[2][1] = binormal[2];
    m[0][2] = gl_Normal[0];
    m[1][2] = gl_Normal[1];
    m[2][2] = gl_Normal[2];

    worldPos = vec3(gl_Position);

    camPos = vec3(gl_ModelViewMatrix * gl_Vertex);
    lightVec = vec3(gl_LightSource[0].position) - camPos;

    // Transform camPos and lightVec
    camPos = m * camPos;
    lightVec = m * lightVec;

    gl_Position = ftransform();
}
