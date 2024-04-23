#ifndef PARTICLE_SYS_H
#define PARTICLE_SYS_H

#include <vector>
#include <memory>

#include "shader.h"

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();
    
    void update();
    void setPos(int index, float x , float y, float z);
    void fillCube();
    void render(std::shared_ptr<Shader> shader);

private:
    unsigned int VAO = 0,
                 VBO = 0,
        instance_VBO = 0;

    float* positions = nullptr;

    float POINT_RADIUS = 0.2f;
    unsigned int num_of_points = 0;

    int x_length = 200;
    int y_length = 200;
    int z_length = 20;

    float spacing_x = 0.1f;
    float spacing_y = 0.1f;
    float spacing_z = 0.1f;
};

#endif