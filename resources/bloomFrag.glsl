#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
//uniform float exposure;

void main()
{
//    const float gamma = 2.2;
    vec3 sceneColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    sceneColor += bloomColor; // additive blending
//    // tone mapping
//    vec3 result = vec3(1.0) - exp(-sceneColor * 1.3);
//    // also gamma correct while we're at it
//    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(sceneColor, 1.0f);
}
