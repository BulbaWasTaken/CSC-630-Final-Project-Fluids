#include "particle_system.h"

ParticleSystem::ParticleSystem()
{   
    this->num_of_points = x_length * y_length * z_length;
    positions = new float[num_of_points];

    fillCube();

    float vertices[] = {
           -POINT_RADIUS / 2.0, -POINT_RADIUS / 2.0, 0,
            POINT_RADIUS / 2.0, -POINT_RADIUS / 2.0, 0,
            POINT_RADIUS / 2.0,  POINT_RADIUS / 2.0, 0,
            
           -POINT_RADIUS / 2.0, -POINT_RADIUS / 2.0, 0,
            POINT_RADIUS / 2.0,  POINT_RADIUS / 2.0, 0,
           -POINT_RADIUS / 2.0,  POINT_RADIUS / 2.0, 0
        };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &instance_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_of_points * sizeof(float), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem()
{
    delete[] positions;
    positions = nullptr;
}

void ParticleSystem::setPos(int index, float x, float y, float z)
{
    positions[index * 3 + 0] = x;
    positions[index * 3 + 1] = y;
    positions[index * 3 + 2] = z;
}

void ParticleSystem::update()
{
    
}

void ParticleSystem::fillCube()
{
    int point_id = 0;

    for (int x = 0; x < x_length; x++) {
        for (int y = 0; y < y_length; y++) {
            for (int z = 0; z < z_length; z++) {

                positions[point_id * 3] = x * spacing_x;
                positions[point_id * 3 + 1] = y * spacing_x;
                positions[point_id * 3 + 2] = z * spacing_x;

                point_id++;
            }
        }
    }
}

void ParticleSystem::render(std::shared_ptr<Shader> shader)
{
    shader->use();

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num_of_points);   
}