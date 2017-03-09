#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <memory>
#include <algorithm>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "ParticleManager.hpp"
#include "Program.h"
#include "Texture.h"
#include "MatrixStack.h"

#include "GLSL.h"

using namespace glm;
using namespace std;

float randomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}



// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);

int ParticleManager::FindUnusedParticle() {

    for (int i = LastUsedParticle; i < MAXPARTICLES; i++) {
        if (ParticlesContainer[i].life < 0) {
            LastUsedParticle = i;
            return i;
        }
    }

    for (int i = 0; i < LastUsedParticle; i++) {
        if (ParticlesContainer[i].life < 0) {
            LastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void ParticleManager::SortParticles() {
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MAXPARTICLES]);
}

ParticleManager::ParticleManager(std::string resource) :
RESOURCE_DIR(resource),
color(BLUE){



    particleShader = make_shared<Program>();
    particleShader->setShaderNames(RESOURCE_DIR + "particleVert.glsl", RESOURCE_DIR + "particleFrag.glsl");
    particleShader->init();

    particleShader->addUniform("P");
    particleShader->addUniform("V");
    particleShader->addUniform("Color");
    particleShader->addUniform("myTextureSampler");

    particleShader->addAttribute("vertPos");
    particleShader->addAttribute("Pcolor");


    particleTex = make_shared<Texture>();
    particleTex->setUnit(0);
    particleTex->setFilename(RESOURCE_DIR + "alpha.bmp");
    particleTex->initBMP();




    for (int i = 0; i < MAXPARTICLES; i++) {
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }


    g_particule_position_size_data = new GLfloat[MAXPARTICLES * 4];
    g_particule_color_data = new GLfloat[MAXPARTICLES * 4];

    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };



    // The VBO containing the positions and sizes of the particles

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof (GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles

    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof (GLfloat), NULL, GL_STREAM_DRAW);
    glEnable(GL_PROGRAM_POINT_SIZE);




}

void ParticleManager::update(double delta, vec3 cameraPosition) {
    // Generate 10 new particule each millisecond,
    // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
    // newparticles will be huge and the next frame even longer.
    int newparticles = (int) (delta * 10000.0);
    if (newparticles > (int) (0.016f * 10000.0))
        newparticles = (int) (0.016f * 10000.0);

    static bool flag = false;


    if (flag == false) {
        for (int i = 0; i < MAXPARTICLES; i++) {
            int particleIndex = i;
            ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
            float randz = randomFloat(-2, -4);
            ParticlesContainer[particleIndex].pos = vec3(0.4, -0.4, randz);
            ParticlesContainer[particleIndex].rot = randomFloat(0,360);
            
            //cout << ParticlesContainer[particleIndex].rot << endl;

            float spread = 1.5f;
            glm::vec3 maindir = glm::vec3(1.0f, 0.0f, 0.0f);
            // Very bad way to generate a random direction; 
            // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
            // combined with some user-controlled parameters (main direction, spread, etc)
            glm::vec3 randomdir = glm::vec3(
                    (rand() % 2000 - 1000.0f) / 1000.0f,
                    (rand() % 2000 - 1000.0f) / 1000.0f,
                    (rand() % 2000 - 1000.0f) / 1000.0f
                    );

            ParticlesContainer[particleIndex].speed = maindir;


            // Very bad way to generate a random color
            ParticlesContainer[particleIndex].r = 0.4;
            ParticlesContainer[particleIndex].g = 0.7;
            ParticlesContainer[particleIndex].b = 0.1;
            ParticlesContainer[particleIndex].a = 0.3;

            ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;

        }
        flag =true;
        
    }
    


    // Simulate all particles
    ParticlesCount = 0;
    for (int i = 0; i < MAXPARTICLES; i++) {

        Particle& p = ParticlesContainer[i]; // shortcut

        if (p.life > 0.0f) {

            // Decrease life
            //p.life -= delta;
            if (p.life > 0.0f) {

                // Simulate simple physics : gravity only, no collisions
                p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float) delta * 0.5f;
                //p.pos += p.speed * (float) delta;

                p.pos.x = p.pos.x + 0.025f * sin(p.rot * 3.14 / 180);
                p.pos.y = p.pos.y + 0.025f * cos(p.rot * 3.14 / 180);
                p.rot +=8.0f;
                if(p.rot >= 360)
                    p.rot = 0;
                //cout << rot << endl;
                //p.pos.y = sin(rot*3.14/180);


                p.cameradistance = glm::length(p.pos - cameraPosition);
                //ParticlesContainer[i].pos += glm::    vec3(0.0f,10.0f, 0.0f) * (float)delta;


                // Fill the GPU buffer
                g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
                g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
                g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;
                g_particule_position_size_data[4 * ParticlesCount + 3] = -0.3f;


                g_particule_color_data[4 * ParticlesCount + 0] = p.r;
                g_particule_color_data[4 * ParticlesCount + 1] = p.g;
                g_particule_color_data[4 * ParticlesCount + 2] = p.b;
                g_particule_color_data[4 * ParticlesCount + 3] = p.a;

            } else {
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
            }

            ParticlesCount++;

        }
    }

    SortParticles();
}

void ParticleManager::draw(mat4 VP, mat4 P, float camRot) {


    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPointSize(14.0f);

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof (GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof (GLfloat) * 4, g_particule_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof (GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof (GLfloat) * 4, g_particule_color_data);



    //VP = glm::translate(VP,  vec3(0.01, 0, 0));
    /*
    VP = glm::translate(VP,   vec3(0.1, -0.35, -3));
    static float rads = -1.0f;
    VP = glm::rotate(VP,rads, vec3(1,0,1) );
    rads= rads+0.05f;
    

    VP = glm::translate(VP,   vec3(0.1, 0, 0.5));
     */






    particleShader->bind();
    particleTex->bind(particleShader->getUniform("myTextureSampler"));

    //glBindVertexArray(vao);

    if(color == BLUE)
        glUniform3fv(particleShader->getUniform("Color"), 1,  value_ptr(vec3(0.5f, 1.0f, 1.0f )));
    else
        glUniform3fv(particleShader->getUniform("Color"), 1,  value_ptr(vec3(1.0f, 0.65f, 0.0f)));
    
    glUniformMatrix4fv(particleShader->getUniform("V"), 1, GL_FALSE, value_ptr(VP));

    glUniformMatrix4fv(particleShader->getUniform("P"), 1, GL_FALSE, value_ptr(P));


    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
            0, // attribute. No particular reason for 1, but must match the layout in the shader.
            4, // size : x + y + z + size => 4
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*) 0 // array buffer offset
            );

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
            1, // attribute. No particular reason for 1, but must match the layout in the shader.
            4, // size : r + g + b + a => 4
            GL_FLOAT, // type
            GL_FALSE, // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
            0, // stride
            (void*) 0 // array buffer offset
            );

    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml

    glVertexAttribDivisor(0, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(1, 1); // color : one per quad                                  -> 1

    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
    // but faster.
    glDrawArraysInstanced(GL_POINTS, 0, 1, ParticlesCount);


    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);


    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);



    particleTex->unbind();
    particleShader->unbind();

    GLSL::checkError(GET_FILE_LINE);

    glDisable(GL_BLEND);

}

