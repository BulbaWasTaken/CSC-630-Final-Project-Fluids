#include "particle_system.h"

ParticleSystem::ParticleSystem()
{   
    num_of_points = x_length * y_length * z_length;
    positions = new float[3 * num_of_points];
    velocities = new float[3 * num_of_points];
    densities = new float[num_of_points];

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
    delete[] velocities;
    velocities = nullptr;
    delete[] densities;
    densities = nullptr;
}

void ParticleSystem::setPos(int index, float x, float y, float z)
{
    positions[index * 3 + 0] = x;
    positions[index * 3 + 1] = y;
    positions[index * 3 + 2] = z;

}

void ParticleSystem::updateBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_of_points * sizeof(float), positions, GL_STATIC_DRAW);
}

void ParticleSystem::update(float delta_time)
{   
    for (int i = 0; i < num_of_points; i++)
    {
        velocities[i * 3] += 0;
        velocities[i * 3 + 1] += -10.0f * delta_time;
        velocities[i * 3 + 2] += 0;

        positions[i * 3] += velocities[i * 3] * delta_time;
        positions[i * 3 + 1] += velocities[i * 3 + 1] * delta_time;
        positions[i * 3 + 2] += velocities[i * 3 + 2] * delta_time;

        if (positions[i * 3] <= x_bounds)
        {
            positions[i * 3] = x_bounds;
            velocities[i * 3] *= -1.0f;
        }
        if (positions[i * 3 + 1] < y_bounds)
        {
            positions[i * 3 + 1] = y_bounds;
            velocities[i * 3 + 1] *= -1.0f * DAMPING;
        }
        if (positions[i * 3 + 2] <= z_bounds)
        {
            positions[i * 3 + 2] = z_bounds;
            velocities[i * 3 + 2] *= -1.0f;
        }
    }
}

void ParticleSystem::printPositions() const
{
    for (int i = 0; i < num_of_points; i++) {
        std::cout << "Position " << i << ": (" 
                  << positions[i * 3] << ", " 
                  << positions[i * 3 + 1] << ", " 
                  << positions[i * 3 + 2] << ")" << std::endl;
    }
}

void ParticleSystem::fillCube()
{
    int point_id = 0;

    for (int y = 0; y < y_length; y++) {
        for (int x = 0; x < x_length; x++) {
            for (int z = 0; z < z_length; z++) {

                positions[point_id * 3]     = x * spacing;
                positions[point_id * 3 + 1] = y * spacing;
                positions[point_id * 3 + 2] = z * spacing;

                velocities[point_id] = 0;
                velocities[point_id + 1] = 0;
                velocities[point_id + 2] = 0;

                densities[point_id] = 0;
                densities[point_id + 1] = 0;
                densities[point_id + 2] = 0;

                point_id++;
            }
        }
    }
}

void ParticleSystem::render(std::shared_ptr<Shader> shader, float delta_time)
{
    shader->use();

    update(delta_time);
    updateBuffer();
    // printPositions();

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num_of_points);   
}