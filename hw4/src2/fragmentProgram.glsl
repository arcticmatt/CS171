//fragment shader
varying vec3 worldPos, camPos;
varying vec3 lightVec;

uniform sampler2D colorTex, normalTex;
uniform vec3 tangent, binormal;

void main()
{
    vec4 texcoord0 = gl_TexCoord[0];
    vec4 texColor = texture2D(colorTex, texcoord0.st);
    vec3 normal = texture2D(normalTex, texcoord0.st).rgb;

    vec4 diffuse_sum = vec4(0.0);
    vec4 specular_sum = vec4(0.0);
    vec3 posOfCam = -camPos;
    vec3 camDir = normalize(posOfCam);

    float lightDist = length(lightVec);
    lightDist *= gl_LightSource[0].quadraticAttenuation;
    float atten_frac = 1.0 / (1.0 + lightDist);

    vec3 lightDir = normalize(lightVec);

    vec4 l_diff = vec4(0.0);
    float NdotL = max(dot(normalize(normal), normalize(lightDir)), 0.0);
    NdotL *= atten_frac;
    if (NdotL > 0.0)
        l_diff += (gl_LightSource[0].diffuse * texColor * NdotL);
    diffuse_sum += l_diff;

    vec4 l_spec = vec4(0.0);
    float NdotS = pow(max(dot(normalize(normal), normalize(camDir + lightDir)),
                0.0), gl_FrontMaterial.shininess);
    NdotS *= atten_frac;
    if (NdotS > 0.0)
        l_spec += (gl_FrontMaterial.specular * gl_LightSource[0].specular * NdotS);
    specular_sum += l_spec;

    vec4 color = gl_LightModel.ambient * gl_FrontMaterial.ambient
        + diffuse_sum + specular_sum;

    gl_FragColor = color;
}
