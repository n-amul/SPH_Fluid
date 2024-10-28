#include "sphSystem.h"
#include "sphCalculation.h"

SPHSettings::SPHSettings(
    float mass, float restDensity, float gasConstant, float viscosity, float h,
    float g, float tension):
    mass(mass),
    restDensity(restDensity),
    gasConstant(gasConstant),
    viscosity(viscosity),
    h(h),
    g(g),
    tension(tension)
{
    //pre-compute coeffs
    poly6 = 315.0f / (64.0f * PI * pow(h, 9));//kernel funct
    spikyGrad = -45.0f / (PI * pow(h, 6));//pressure forces
    spikyLap = 45.0f / (PI * pow(h, 6));//viscosity forces
    h2 = h * h;
    selfDens = mass * poly6 * pow(h, 6); //The density contribution of a particle to itself
    massPoly6Product = mass * poly6; // Used in density calculations involving neighboring particles.
    sphereScale = glm::scale(glm::mat4(1.0),glm::vec3(h/2.f)); //scale matrix for rendering
}

SphSystem::~SphSystem()
{
    delete[] sphereModelMtxs;
    delete[] particles;
}

SphSystem::SphSystem(size_t particleCubeWidth, const SPHSettings &settings, const bool &runOnGPU): 
    particleCubeWidth(particleCubeWidth),
    settings(settings),
    runOnGPU(runOnGPU)
{
    particleCount = particleCubeWidth * particleCubeWidth * particleCubeWidth;
    particles = new Particle[particleCount];

    // Load sphere and allocate matrice space
    sphere = Model::Load("../../model/lowsphere.obj");
    sphereModelMtxs = new glm::mat4[particleCount];

    initParticles();

	// Generate VBO for sphere model matrices
    m_vbo=Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, &sphereModelMtxs[0], sizeof(glm::mat4), particleCount);

	// Setup instance VAO
    m_vao=VertexLayout::Create();
    m_vao->Bind();
    m_vao->SetAttrib(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), 0);
    m_vao->SetAttrib(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), sizeof(glm::vec4));
    m_vao->SetAttrib(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), sizeof(glm::vec4) * 2);
    m_vao->SetAttrib(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), sizeof(glm::vec4) * 3);
	glVertexAttribDivisor(2,1);
	glVertexAttribDivisor(3,1);
	glVertexAttribDivisor(4,1);
	glVertexAttribDivisor(5,1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//start init
	started = false;
}


void SphSystem::initParticles()
{
    //initializes a 3D grid of particles
    /*
        Places particles in a cubic arrangement.
        Introduces slight random offsets to particle positions to prevent numerical artifacts from perfect grid alignment.
        Initializes particle velocities to zero.
        Calculates model matrices for rendering each particle as a sphere.
    */
	std::srand(1024);
	float particleSeperation = settings.h + 0.01f;
	for (int i = 0; i < particleCubeWidth; i++) {
		for (int j = 0; j < particleCubeWidth; j++) {
			for (int k = 0; k < particleCubeWidth; k++) {
				float ranX = (float(rand()) / float((RAND_MAX)) * 0.5f - 1) * settings.h / 10;
				float ranY = (float(rand()) / float((RAND_MAX)) * 0.5f - 1) * settings.h / 10;
				float ranZ = (float(rand()) / float((RAND_MAX)) * 0.5f - 1)* settings.h / 10;
				glm::vec3 nParticlePos = glm::vec3(
                    i * particleSeperation + ranX - 1.5f,
                    j * particleSeperation + ranY + settings.h + 0.1f,
                    k * particleSeperation + ranZ - 1.5f);

                size_t particleIndex = i + (j + particleCubeWidth * k) * particleCubeWidth;
                Particle* particle = &particles[particleIndex];
                particle->position = nParticlePos;
                particle->velocity = glm::vec3(0);

                sphereModelMtxs[particleIndex] = glm::translate(glm::mat4(1.0),particle->position) * settings.sphereScale;
			}
		}
	}
}

void SphSystem::update(float deltaTime) {
	if (!started) return;
	// To increase system stability, a fixed deltaTime is set
	deltaTime = 0.003f;
    updateParticles(particles, sphereModelMtxs, particleCount, settings, deltaTime, runOnGPU);
}

void SphSystem::draw(const glm::mat4& viewProjMtx, Program* program) {
	// update the matrices that is in the GPU
	m_vbo->Bind();
    void* data=glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
    memcpy(data, sphereModelMtxs, sizeof(glm::mat4) * particleCount);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //draw particles
    program->Use();
    program->SetUniform("viewProjMtx", viewProjMtx);
    sphere->GetMesh(0)->GetVertexLayout()->Bind();
    glDrawElementsInstanced(GL_TRIANGLES, sphere->GetMesh(0)->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0, particleCount);
}

void SphSystem::reset() {
	initParticles();
	started = false;
}

void SphSystem::startSimulation() {
	started = true;
}