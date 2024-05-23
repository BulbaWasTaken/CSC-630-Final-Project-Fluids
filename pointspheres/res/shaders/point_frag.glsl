#version 330 core

out vec4 FragColor;
out float FragDepth;

in vec3 texCoords;
in vec3 sphere_center;

uniform vec3 viewPos;
uniform float point_radius;
uniform mat4 proj;
uniform vec3 aColor;
uniform int width;
uniform int height;

vec3 getEyePos(vec2 coord)
{
     float viewportDepth = texture(depthSampler, coord).x;


     float ndcDepth = viewportDepth * 2.0 - 1.0;
     vec4 clipSpacePos = vec4(coord * 2.0 - vec2(1.0), ndcDepth, 1.0);
     vec4 eyeSpacePos = proj * clipSpacePos;

    return eyeSpacePos.xyz / eyeSpacePos.w;
}

void main()
{
    vec3 N;
    N.xy = texCoords.xy*2.0-1.0;
    float r2 = dot(N.xy, N.xy);
    
    if(r2 > 1.0) discard;
    N.z = -sqrt(1.0-r2);

    vec2 texelSize = 1.0 / vec2(width, height);
     vec2 coord = gl_FragCoord.xy * texelSize;

     float viewportDepth = FragDepth;



    if (viewportDepth == 1.0)
    {
        discard;
    }

    // Reconstruct position from depth
     vec3 eyeSpacePos = getEyePos(coord);

    // Reconstruct normal from depth
    vec3 ddx = getEyePos(coord + vec2(texelSize.x, 0.0)) - eyeSpacePos;
    vec3 ddx2 = eyeSpacePos - getEyePos(coord + vec2(-texelSize.x, 0.0));

    if (abs(ddx.z) > abs(ddx2.z))
    {
        ddx = ddx2;
    }

    vec3 ddy = getEyePos(coord + vec2(0.0, texelSize.y)) - eyeSpacePos;
    vec3 ddy2 = eyeSpacePos - getEyePos(coord + vec2(0.0, -texelSize.y));

    if (abs(ddy.z) > abs(ddy2.z))
    {
        ddy = ddy2;
    }

    vec3 normal = normalize(cross(ddx, ddy));

    vec3 normal2 = normalize(N);
    vec3 fragmentPos = sphere_center + normal2 * point_radius;

    // ambient
    float ambient_strength = 0.3;
    vec3 ambient = vec3(0.0,0.0,1.0) * ambient_strength;

    // diffuse
    vec3 light_pos = (vec4(vec3(0.0, 4.0, 4.0),1.0)).xyz;
    vec3 light_dir = normalize(light_pos - fragmentPos);
    float diff = max(0.0, dot(normal, light_dir));
    vec3 diffuse = diff * vec3(1.0);

    float diff2 = max(0.0, dot(normal2, light_dir));
    vec3 diffuse2 = diff2 * vec3(1.0);

    float SHININESS = 222.0;

    vec3 viewDir = normalize(vec3(0.0) - fragmentPos);
    vec3 halfDir= normalize(light_dir + viewDir);
    float cosAngle = max(dot(halfDir, normal), 0.0);
    float scoeff = pow(cosAngle, SHININESS);
    vec3 specColor = vec3(1.0) * scoeff;

    // ambient and diffuse
    vec3 result = ambient + diffuse * aColor+specColor;
    FragColor = vec4(result, 1.0);
}