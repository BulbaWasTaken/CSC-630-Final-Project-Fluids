
#include "particle_system.h"

ParticleSystem::ParticleSystem()
{
    num_of_points = x_length * y_length * z_length;

    fillCube();

    float vertices[] = {
       -POINT_RADIUS / 2.0f, -POINT_RADIUS / 2.0f, 0,
        POINT_RADIUS / 2.0f, -POINT_RADIUS / 2.0f, 0,
        POINT_RADIUS / 2.0f,  POINT_RADIUS / 2.0f, 0,

       -POINT_RADIUS / 2.0f, -POINT_RADIUS / 2.0f, 0,
        POINT_RADIUS / 2.0f,  POINT_RADIUS / 2.0f, 0,
       -POINT_RADIUS / 2.0f,  POINT_RADIUS / 2.0f, 0
    };

    poly6_k = 315.0f / (64.0f * PI * std::pow(abs(KERNEL_RADIUS), 9));
    spiky_k = -45.0f / (PI * pow(KERNEL_RADIUS, 6));
    spiky_k_n = 45.0f / (PI * pow(KERNEL_RADIUS, 6));
    set_density = poly6_k * MASS * std::pow(KERNEL_RADIUS, 6);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &instance_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Point), particles.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void *)offsetof(Point, position));

    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem()
{
    
}

float ParticleSystem::poly6(float radius, float distance)
{
    if (distance < radius)
    {
        float smoothing = radius * radius - distance * distance;
        float scale = 315.0f / (64.0f * PI * std::pow(abs(radius), 9));
        return scale * smoothing * smoothing * smoothing;
    }
    return 0.0f;
}

float ParticleSystem::spikyGrad(float radius, float distance)
{
    if (distance < 0.0001f)
        return 0.0f;
    float spiky = (radius - distance) * (radius - distance);
    return -45.0f / (PI * std::pow(radius, 6)) * spiky;
}

void ParticleSystem::calculateDensityPressure(int index)
{
    float new_density = 0;
    glm::vec3 cell = getCell(index, KERNEL_RADIUS);

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int z = -1; z <= 1; z++)
            {
                int hash = getHash3D(cell + glm::vec3(x, y, z));
                int index_n = neighbor_table[hash];
                if (index_n == INT_MAX)
                {
                    continue;
                }
                while (index_n < num_of_points)
                {
                    if (index_n == index)
                    {
                        index_n++;
                        continue;
                    }
                    Point *m = &particles[index];
                    Point *n = &particles[index_n];
                    if (n->hash != hash)
                    {
                        break;
                    }
                    float distance = glm::length(n->position - m->position);
                    if (distance < (KERENL_RADIUS_2))
                    {
                        float influence = poly6(KERNEL_RADIUS, distance);
                        new_density += MASS * influence;
                    }
                    index_n++;
                }
            }
        }
    }

    particles[index].density = new_density;

    // get pressure
    float new_pressure = (particles[index].density - REST_DENSITY);
    particles[index].pressure = new_pressure;
}

void ParticleSystem::calculateForce(int index)
{
    particles[index].force = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cell = getCell(index, KERNEL_RADIUS);

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int z = -1; z <= 1; z++)
            {
                int hash = getHash3D(cell + glm::vec3(x, y, z));
                int index_n = neighbor_table[hash];
                if (index_n == INT_MAX)
                {
                    continue;
                }
                while (index_n < num_of_points)
                {
                    if (index_n == index)
                    {
                        index_n++;
                        continue;
                    }
                    Point *m = &particles[index];
                    Point *n = &particles[index_n];
                    if (n->hash != hash)
                    {
                        break;
                    }
                    float distance = glm::length(n->position - m->position);
                    if (distance < (KERENL_RADIUS_2))
                    {
                        float distance_sqrt = std::sqrt(distance);
                        float influence = spikyGrad(KERNEL_RADIUS, distance);
                        glm::vec3 direction = glm::normalize(n->position - m->position);

                        // Pressure force
                        float factor = (2 * n->density) * influence;
                        if (factor == 0)
                            factor = 1;

                        glm::vec3 pressure_force = -direction * (m->pressure + n->pressure) / factor;
                        m->force += pressure_force;
                        
                        influence = poly6(KERNEL_RADIUS, distance);

                        // Viscosity force
                        factor = n->density * influence;
                        if (factor == 0)
                            factor = 1;

                        glm::vec3 velocity_diff = n->velocity - m->velocity;
                        glm::vec3 viscosity_force = VISCOSITY * MASS * velocity_diff / factor;
                        n->force += viscosity_force;
                    }
                    index_n++;
                }
            }
        }
    }
}

void ParticleSystem::updateParticles()
{

}

void ParticleSystem::sort()
{
    std::sort(particles.begin(), particles.end(), [](const Point &a, const Point &b)
              { return a.hash < b.hash; });
}

int ParticleSystem::getHash3D(glm::vec3 cell)
{
    int hash_x = (int)cell.x * hash_prime1;
    int hash_y = (int)cell.y * hash_prime2;
    int hash_z = (int)cell.z * hash_prime3;
    int hash = hash_x ^ hash_y ^ hash_z;
    return hash % hash_size;
}

glm::vec3 ParticleSystem::getCell(int index, float radius)
{
    int x = particles[index].position.x / radius;
    int y = particles[index].position.y / radius;
    int z = particles[index].position.z / radius;
    return glm::vec3(x, y, z);
}

void ParticleSystem::initNeighborTable()
{
    neighbor_table.resize(hash_size, INT_MAX);

    uint32_t temp_hash = INT_MAX;
    int size = x_length * y_length * z_length;
    for (int i = 0; i < size; i++)
    {
        uint32_t point_hash = particles[i].hash;
        if (point_hash != temp_hash)
        {
            if (point_hash < hash_size)
            {
                neighbor_table[point_hash] = i;
                temp_hash = point_hash;
            }
        }
    }
}

void ParticleSystem::updateBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Point), particles.data(), GL_STATIC_DRAW);
}

void ParticleSystem::fillCube()
{
    for (int x = 0; x < x_length; x++)
    {
        for (int y = 0; y < y_length; y++)
        {
            for (int z = 0; z < z_length; z++)
            {

                Point p;

                float position_x = x * spacing;
                float position_y = y * spacing;
                float position_z = z * spacing;

                p.position = glm::vec3(position_x, position_y, position_z);
                p.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
                p.force = glm::vec3(1.0f, 1.0f, 1.0f);
                p.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
                p.density = 1;
                p.pressure = 0;
                p.hash = 0;

                particles.push_back(p);
            }
        }
    }
}

void ParticleSystem::update(float delta_time)
{



    for (int i = 0; i < num_of_points; i++)
    {
        if (particles[i].density == 0)
            particles[i].density = 1;
        glm::vec3 acceleration = (particles[i].force / particles[i].density) + glm::vec3(0.0f, -E_GRAVITY, 0.0f);

        particles[i].velocity += acceleration * delta_time;

        particles[i].position += particles[i].velocity * delta_time;

        // std::cout << particles[i].position.x << " " << particles[i].position.y << " " << particles[i].position.y << std::endl;

        // Boundary conditions
        if (particles[i].position.x > x_bounds)
        {   
            particles[i].position.x = x_bounds;
            particles[i].velocity.x = -particles[i].velocity.x * DAMPING;
        }
        if (particles[i].position.x < -x_bounds)
        {
            particles[i].position.x = -x_bounds;
            particles[i].velocity.x = -particles[i].velocity.x * DAMPING;
        }
        if (particles[i].position.y < y_bounds)
        {
            particles[i].position.y = y_bounds;
            particles[i].velocity.y = -particles[i].velocity.y * DAMPING;
        }
        if (particles[i].position.z > z_bounds)
        {   
            particles[i].position.z = z_bounds;
            particles[i].velocity.z = -particles[i].velocity.z * DAMPING;
        }
        if (particles[i].position.z < -z_bounds)
        {   
            particles[i].position.z = -z_bounds;
            particles[i].velocity.z = -particles[i].velocity.z * DAMPING;
        }

        particles[i].hash = getHash3D(getCell(i, KERNEL_RADIUS));

        initNeighborTable();

        sort();

        calculateDensityPressure(i);

        calculateForce(i);
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
