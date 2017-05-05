#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;
uniform sampler2D shadowDepth0;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec2 vTex;
in vec3 fragPos;
in vec3 fragNor;
in mat3 TBN;

out vec4 FragColor;

void main()
{
    vec3 color = texture(diffuseTex, vTex).rgb;
    //    vec3 ks = texture(specularTex, vTex).rgb;

    vec3 normal = texture(normalTex, vTex).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);

    vec3 lightColor = vec3(1.0);
    vec3 ambient = 0.3 * color;
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    
    vec3 lighting = ambient + (diffuse + specular) * color;

    FragColor = vec4(lighting, 1.0f);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
