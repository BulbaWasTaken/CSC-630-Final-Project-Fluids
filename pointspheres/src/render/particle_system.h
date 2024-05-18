
#ifndef PARTICLE_SYS_H
#define PARTICLE_SYS_H

#include <vector>
#include <memory>
#include <thread>

#include "shader.h"

class ParticleSystem
{
public:

    struct Point {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 force;
        glm::vec3 acceleration;
        float density;
        float pressure;
        float hash;
    };

    ParticleSystem();

    ~ParticleSystem();
    
    float poly6(float radius, float distance);

    float spikyGrad(float radius, float distance); // get lagrange gradient

    void calculateDensityPressure(int start, int end);

    void calculateForce(int start, int end);

    void updateParticles(float delta_time, int start, int end);

    void sort(); // sorts list
    
    int getHash3D(glm::vec3 cell);

    void getHashes(int start, int end);

    glm::vec3 getCell(int index, float radius);

    void initNeighborTable();
    
    void updateBuffer();
    
    void fillCube();
    
    void update(float delta_time);

    void render(std::shared_ptr<Shader> shader, float delta_time);

private:
    unsigned int VAO = 0,
                 VBO = 0,
        instance_VBO = 0;

    std::vector<Point> particles;

    // deprecated
    // 

    float POINT_RADIUS = 0.25f;
    unsigned int num_of_points = 0;

    const int x_length = 10;
    const int y_length = 15;
    const int z_length = 10;

    float spacing = 0.1f;

    const float x_bounds = 1.0f;
    const float y_bounds = -1.0f;
    const float z_bounds = 1.0f;

    float DAMPING = 0.3f;

    // Math domain
    const float PI = 3.14159265f;
    const float E_GRAVITY = 9.8f; // gravity force factor
    const float MASS = 0.8f; // mass of a particle
    const float KERNEL_RADIUS = 0.2f; // radius of influence
    const float KERNEL_RADIUS_2 = KERNEL_RADIUS * KERNEL_RADIUS;
    const float P_PRESSURE = 0.2f; // pressure force factor
    const float REST_DENSITY = 1000.f; // density particles go towards
    const float VISCOSITY = 2.0f; // particles resistance

    // values for kernel sets
    float poly6_k = 0;
    float spiky_k = 0;
    float spiky_k_n = 0;
    float set_density = 0;

    // hashing functions for neighbor search
    int hash_prime1 = 92837111;
    int hash_prime2 = 689287499;
    int hash_prime3 = 283923481;
    int hash_size = 10000;
    std::vector<u_int32_t>neighbor_table;
};

#endif
