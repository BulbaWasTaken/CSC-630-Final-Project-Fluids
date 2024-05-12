
#ifndef PARTICLE_SYS_H
#define PARTICLE_SYS_H

#include <vector>
#include <memory>

#include "shader.h"

class ParticleSystem
{
public:
    ParticleSystem();

    ~ParticleSystem();
    
    float poly6(float radius, float distance);

    float spikyGrad(float radius, float distance); // get lagrange gradient

    float calculateDensity(); // calculates density

    float calculatePressure(); // calculates pressure

    void sort(float* positions); // sorts list
    
    int getHash3D(glm::vec3 cell);

    glm::vec3 getCell(const float* particles, int index, float radius);

    void initNeighborTable();
    
    void printPositions() const;
    
    void setPos(int index, float x , float y, float z);
    
    void updateBuffer();
    
    void fillCube();
    
    void setupGrid();
    
    void update(float delta_time);

    void render(std::shared_ptr<Shader> shader, float delta_time);

private:
    unsigned int VAO = 0,
                 VBO = 0,
        instance_VBO = 0;

    float* positions = nullptr;
    float* velocities = nullptr;
    float* densities = nullptr;
    float* hashes = nullptr;

    float POINT_RADIUS = 0.25f;
    unsigned int num_of_points = 0;

    const int x_length = 30;
    const int y_length = 75;
    const int z_length = 30;

    float spacing = 0.1f;

    const float x_bounds = 1.0f;
    const float y_bounds = -1.0f;
    const float z_bounds = 1.0f;

    float DAMPING = 0.7f;

    // Math domain
    const float PI = 3.14159265f;
    const float E_GRAVITY = 9.8f; // gravity force factor
    const float MASS = 1.0f; // mass of a particle
    const float KERNEL_RADIUS = 0.3f; // radius of influence
    const float P_PRESSURE = 0.2f; // pressure force factor
    const float REST_DENSITY = 200.f; // density particles go towards
    const float VISCOSITY = 40.0f; // particles resistance

    // hashing functions for neighbor search
    int hash_prime1 = 92837111;
    int hash_prime2 = 689287499;
    int hash_prime3 = 283923481;
    int hash_size = 10000;
    uint32_t* neighbor_table = nullptr;
};

#endif
