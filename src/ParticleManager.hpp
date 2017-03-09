/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ParticleManager.hpp
 * Author: root
 *
 * Created on March 1, 2017, 7:05 PM
 */

#ifndef PARTICLEMANAGER_HPP
#define PARTICLEMANAGER_HPP

#define MAXPARTICLES 10

#include <iostream>

#define BLUE 1
#define ORANGE 0

class Program;
class Texture;
class MatrixStack;



// CPU representation of a particle

typedef struct Particle {
    glm::vec3 pos, speed, center;
    float r, g, b, a; // Color
    float size, angle, weight,rot;
    float life; // Remaining life of the particle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f
    
    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
} Particle;

class ParticleManager {
public:
    ParticleManager(std::string resource);
    void update(double delta, glm::vec3 camPos);
    void draw(glm::mat4 VP, glm::mat4 P, float camRot);
    void setColor(int in){color = in;}
    
    
private:
    std::string RESOURCE_DIR;
    GLuint vao;
    std::shared_ptr<Program> particleShader;
    std::shared_ptr<Texture> particleTex;
    
    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
    
    int color;
    
    
    Particle ParticlesContainer[MAXPARTICLES];
    int LastUsedParticle = 0;
    int ParticlesCount = 0;
    
    GLfloat* g_particule_position_size_data;
    GLfloat* g_particule_color_data;
    
    
    
    int FindUnusedParticle();
    
    void SortParticles();
    
    
};



#endif /* PARTICLEMANAGER_HPP */
