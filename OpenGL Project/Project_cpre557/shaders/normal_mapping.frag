
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;  

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool normalMapping;

void main()
{           
    vec3 normal = normalize(fs_in.Normal);
    if(normalMapping)
    {
        // Obtain normal from normal map in range [0,1]
        normal = texture(normalMap, fs_in.TexCoords).rgb;
        // Transform normal vector to range [-1,1]
        normal = normalize(normal * 2.0 - 1.0);   
    }
     // Get diffuse color
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
    // Ambient
    vec3 ambient = 0.1 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}

