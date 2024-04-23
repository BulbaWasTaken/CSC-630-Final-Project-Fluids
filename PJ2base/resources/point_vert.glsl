#version 330 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 instance_position;

out vec3 texCoords;
out vec3 sphere_center;

uniform mat4 proj;
uniform mat4 view;

void main()
{   

    mat4 model = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        instance_position.x, instance_position.y, instance_position.z, 1.0
    );

    mat4 view_model_mat = view * model;

    view_model_mat[0][0] = 1;   view_model_mat[1][0] = 0;   view_model_mat[2][0] = 0;
    view_model_mat[0][1] = 0;   view_model_mat[1][1] = 1;   view_model_mat[2][1] = 0;
    view_model_mat[0][2] = 0;   view_model_mat[1][2] = 0;   view_model_mat[2][2] = 1;

    gl_Position = proj * view_model_mat * vec4(vert_pos, 1.0);

    texCoords = normalize(vert_pos) * sqrt(2);
    sphere_center = (view * vec4(instance_position, 1.0)).xyz;
}