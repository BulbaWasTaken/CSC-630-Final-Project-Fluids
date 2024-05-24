#version 330 core

out vec4 FragColor;
out float FragDepth;

in vec3 texCoords;
in vec3 sphere_center;

uniform vec3 viewPos;
uniform float point_radius;
uniform mat4 proj;
uniform mat4 view;
uniform vec3 aColor;
uniform sampler2D depthSampler;
uniform int width;
uniform int height;

vec3 getEyePos(vec2 coord)
{

    float viewportDepth = texture(depthSampler, coord).x;
    // Bilateral filter parameters
    float sigmaS = 5.0; // Spatial Gaussian standard deviation
    float sigmaR = 0.5; // Range Gaussian standard deviation
    int kernelSize = 40; // Size of the kernel

    vec2 texelSize = 1 / vec2(width, height);
    float sumWeights = 0.0;
    float sumDepth = 0.0;
    bool useAlternative = false;
    for (int i = -kernelSize; i <= kernelSize; ++i)
    {
        for (int j = -kernelSize; j <= kernelSize; ++j)
        {
            vec2 offset = vec2(float(i), float(j)) * texelSize;
            float neighborDepth = texture(depthSampler, coord + offset).x;

            // Spatial weight
            float spatialWeight = exp(-(i * i + j * j) / (2.0 * sigmaS * sigmaS));

            // Range weight
            float rangeWeight = exp(-pow(viewportDepth - neighborDepth, 2.0) / (2.0 * sigmaR * sigmaR));

            // Combined weight
            float weight = spatialWeight * rangeWeight;

            if (spatialWeight < 0.1 && rangeWeight < 0.1) {
                useAlternative = true;
            }

            sumWeights += weight;
            sumDepth += neighborDepth * weight;
        }
    }
    /*
    if (useAlternative) {
        vec3 N;
        N.xy = texCoords.xy * 2.0 - 1.0;
        float r2 = dot(N.xy, N.xy);

        if (r2 > 1.0) discard;
        N.z = -sqrt(1.0 - r2);
        vec3 normal2 = normalize(N);
        vec3 fragmentPos = sphere_center + normal2 * point_radius;
        coord = gl_FragCoord.xy * texelSize;
        vec4 clip_spc_pos = vec4(fragmentPos, 1.0) * proj;
        FragDepth = clip_spc_pos.z / clip_spc_pos.w;
        viewportDepth = FragDepth;
    }*/
    //else {
        viewportDepth = sumDepth / sumWeights;
  //  }

   // viewportDepth = sumDepth / sumWeights;


     float ndcDepth = viewportDepth * 2.0 - 1.0;
     vec4 clipSpacePos = vec4(coord * 2.0 - vec2(1.0), ndcDepth, 1.0);
     vec4 eyeSpacePos = inverse(proj) * clipSpacePos;

    return eyeSpacePos.xyz / eyeSpacePos.w;
}

void main()
{

    vec3 N;
    N.xy = texCoords.xy * 2.0 - 1.0;
    float r2 = dot(N.xy, N.xy);

    if (r2 > 1.0) discard;
    N.z = -sqrt(1.0 - r2);


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
    vec3 ambient = vec3(0.0, 0.0, 1.0) * ambient_strength;

    // diffuse
    vec3 light_pos = (view * vec4(vec3(0.0, 3.0, 2.0), 1.0)).xyz;
    vec3 light_dir = normalize(light_pos - fragmentPos);
    float diff = max(0.0, dot(normal, light_dir)) * 0.5 + 0.5;
    vec3 diffuse = diff * vec3(1.0);

    float diff2 = max(0.0, dot(normal2, light_dir));
    vec3 diffuse2 = diff2 * vec3(1.0);

    float SHININESS = 22.0;

    vec3 viewDir = normalize(vec3(0.0) - fragmentPos);
    vec3 halfDir = normalize(light_dir + viewDir);

    // Schlick's approximation for the Fresnel effect
    float F0 = 0.04; // Reflectance at normal incidence for non-metals
    vec3 F0_vec = vec3(F0);
    float cosTheta = max(dot(halfDir, viewDir), 0.0);
    vec3 fresnel = F0_vec + (vec3(1.0) - F0_vec) * pow(1.0 - cosTheta, 5.0);

    // Specular component using Blinn-Phong model
    float cosAngle = max(dot(halfDir, normal), 0.0);
    float scoeff = pow(cosAngle, SHININESS);
    vec3 specColor = fresnel * scoeff;

    // Volumetric absorption using Beer's Law
    float absorptionCoefficient = 0.02; // Adjust this value as needed
    float distanceThroughWater = 2.0 * point_radius; // Approximation of distance through the sphere
    float absorption = exp(-absorptionCoefficient * distanceThroughWater);

    // Combine all components
    vec4 clip_spc_pos = vec4(fragmentPos, 1.0) * proj;
    FragDepth = clip_spc_pos.z / clip_spc_pos.w;

    vec3 baseColor = ambient + diffuse * aColor + specColor;
    vec3 whiteBackground = vec3(1.0, 1.0, 1.0);
    vec3 finalColor = mix(whiteBackground, baseColor, absorption);

    FragColor = vec4(finalColor, absorption);
}