#ifndef PARTICLE_SYS_H
#define PARTICLE_SYS_H

#include <vector>
#include <memory>

#include "shader.h"
#include "../utils/point_math.h"

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();
    
    void update(float delta_time);
    void printPositions() const;
    void setPos(int index, float x , float y, float z);
    void updateBuffer();
    void fillCube();
    void setupGrid();
    void render(std::shared_ptr<Shader> shader, float delta_time);

private:
    unsigned int VAO = 0,
                 VBO = 0,
        instance_VBO = 0;

    float* positions = nullptr;
    float* velocities = nullptr;
    float* densities = nullptr;

    float POINT_RADIUS = 0.25f;
    unsigned int num_of_points = 0;

    int x_length = 100;
    int y_length = 100;
    int z_length = 100;

    float spacing = 0.1f;

    float x_bounds = 0.0f;
    float y_bounds = -1.0f;
    float z_bounds = 0.0f;

    float DAMPING = 0.7f;
};

#endif