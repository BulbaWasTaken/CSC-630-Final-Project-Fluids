#version 330 core

out vec4 FragColor;
out float FragDepth;

in vec3 texCoords;
in vec3 sphere_center;

uniform vec3 viewPos;
uniform float point_radius;
uniform mat4 proj;
uniform vec3 aColor;

void main()
{
    vec3 N;
    N.xy = texCoords.xy*2.0-1.0;
    float r2 = dot(N.xy, N.xy);
    
    if(r2 > 1.0) discard;
    N.z = -sqrt(1.0-r2);

    vec3 normal = normalize(N);
    vec3 fragmentPos = sphere_center + normal * point_radius;

    // ambient
    float ambient_strength = 0.3;
    vec3 ambient = vec3(1.0) * ambient_strength;

    // diffuse
    vec3 light_pos = vec3(0.0, 2.0, -4.0);
    vec3 light_dir = normalize(light_pos - fragmentPos);
    float diff = max(0.0, dot(normal, light_dir));
    vec3 diffuse = diff * vec3(1.0);
    
    // depth test
    vec4 clip_spc_pos = vec4(fragmentPos, 1.0) * proj;
    FragDepth = clip_spc_pos.z / clip_spc_pos.w;

    // ambient and diffuse
    vec3 result = (ambient + diffuse) * aColor;
    FragColor = vec4(result, 1.0);
}