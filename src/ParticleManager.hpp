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

#include <iostream>

class Program;
class Texture;
class MatrixStack;


// CPU representation of a particle

struct Particle {
    glm::vec3 pos, speed;
    unsigned char r, g, b, a; // Color
    float size, angle, weight;
    float life; // Remaining life of the particle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

class ParticleManager {
public:
    ParticleManager(std::string resource);
    void update(double delta, glm::vec3 camPos);
    void draw(glm::mat4 VP);


private:
    std::string RESOURCE_DIR;
    GLuint vao;
    std::shared_ptr<Program> particleShader;
    std::shared_ptr<Texture> particleTex;

    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;


    const int MaxParticles = 100000;
    Particle ParticlesContainer[MaxParticles];
    int LastUsedParticle = 0;

    static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
    static GLubyte* g_particule_color_data = new GLubyte[MaxParticles * 4];

    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    int FindUnusedParticle();

    void SortParticles();


};



#endif /* PARTICLEMANAGER_HPP */

