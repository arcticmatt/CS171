//fragment shader
varying vec3 normal, worldPos, camPos;
uniform int numLights, is_phong;
void main()
{
    if (is_phong == 1) {
        vec4 diffuse_sum = vec4(0.0);
        vec4 specular_sum = vec4(0.0);
        vec3 posOfCam = -camPos;
        vec3 camDir = normalize(posOfCam);

        for (int i = 0; i < numLights; i++) {
            float lightDist = distance(vec3(gl_LightSource[i].position), camPos);
            lightDist *= gl_LightSource[i].quadraticAttenuation;
            float atten_frac = 1.0 / (1.0 + lightDist);

            vec3 lightDir = normalize(vec3(gl_LightSource[i].position) - camPos);

            vec4 l_diff = vec4(0.0);
            float NdotL = max(dot(normalize(normal), normalize(lightDir)), 0.0);
            NdotL *= atten_frac;
            if (NdotL > 0.0)
                l_diff += (gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL);
            diffuse_sum += l_diff;

            vec4 l_spec = vec4(0.0);
            float NdotS = pow(max(dot(normalize(normal), normalize(camDir + lightDir)),
                        0.0), gl_FrontMaterial.shininess);
            NdotS *= atten_frac;
            if (NdotS > 0.0)
                l_spec += (gl_FrontMaterial.specular * gl_LightSource[i].specular * NdotS);
            specular_sum += l_spec;
        }

        vec4 color = gl_LightModel.ambient * gl_FrontMaterial.ambient
            + diffuse_sum + specular_sum;
        gl_FragColor = color;
    } else {
        gl_FragColor = gl_Color;
    }
}
