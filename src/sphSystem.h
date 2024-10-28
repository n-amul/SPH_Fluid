#ifndef __SPHSYS_H__
#define __SPHSYS_H__

#include "model.h"
#include "Timer.h"
#include <thread>

struct Particle
{
	glm::vec3 position, velocity, acceleration, force;
    float density;
    float pressure;
    uint16_t hash;
};

struct SPHSettings
{
    SPHSettings(
        float mass, float restDensity, float gasConstant, float viscosity,
        float h, float g, float tension);

    glm::mat4 sphereScale;
    float poly6, spikyGrad, spikyLap, gasConstant, mass, h2, selfDens,
        restDensity, viscosity, h, g, tension, massPoly6Product;
};

class SphSystem {
private:
    SPHSettings settings;
    size_t particleCubeWidth;

	bool started;
    bool runOnGPU;
    BufferPtr m_vbo;
	//initializes the particles that will be used
	void initParticles();

	// Sphere geometry for rendering
    glm::mat4* sphereModelMtxs;
    ModelUPtr sphere;

public:
    SphSystem(size_t numParticles, const SPHSettings &settings, const bool &runOnGPU);
	~SphSystem();

	Particle* particles;
    size_t particleCount;

	//updates the SPH system
	void update(float deltaTime);
    //draws the SPH system and particles
	void draw(const glm::mat4& viewProjMtx, Program* shader);

	void reset();
	void startSimulation();
};
#endif