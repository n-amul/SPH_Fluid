#ifndef SPH_SPH_H
#define SPH_SPH_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "SphSystem.h"

//-----------------------adjTable--------------------------------//
const uint32_t TABLE_SIZE = 262144;
const uint32_t NO_PARTICLE = 0xFFFFFFFF;

/// Returns a hash of the cell position
uint32_t getHash(const glm::ivec3 &cell);

/// Get the cell that the particle is in.
glm::ivec3 getCell(Particle* p, float h);

/// Creates the particle neighbor hash table.
/// It is the caller's responsibility to free the table.
uint32_t* createNeighborTable(Particle *sortedParticles, const size_t &particleCount);


//---------------------------------------------------------------//


//----------------------calculation------------------------------//
/// Calculates and stores particle hashes.
void parallelCalculateHashes(Particle *particles, size_t start, size_t end, const SPHSettings &settings);

/// Sort particles in place by hash.
void sortParticles(Particle *particles, const size_t &particleCount);

/// Update attrs of particles in place.
void updateParticles(
    Particle *particles, glm::mat4 *particleTransforms,
    const size_t particleCount, const SPHSettings &settings,
    float deltaTime, const bool onGPU);

#endif //SPH_SPH_H