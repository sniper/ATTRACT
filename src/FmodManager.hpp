/* 
 * File:   FmodManager.hpp
 * Author: Darryl
 *
 * Created on February 6, 2017, 3:39 PM
 */

#ifndef FMODMANAGER_HPP
#define FMODMANAGER_HPP
#include "fmod/fmod.hpp"
#include "fmod/fmod_common.h"
#include "fmod/fmod_errors.h"
#include <iostream>
#include <map>

/*error checking not working, fix later*/






class FmodManager {
public:
    FmodManager(std::string resource);
    FmodManager(const FmodManager& orig);
    virtual ~FmodManager();
    void createStream(std::string name, std::string path, bool loop);
    void playSound(std::string name, bool loop);
    void setPaused(std::string name, bool state);
    bool isPlaying(std::string name);
    std::string getCurSound();
    void stopSound(std::string name);
private:
    std::string RESOURCE_DIR;
    std::string curSound;
    FMOD::System *fmodSystem;
    std::map<std::string, FMOD::Sound*> sounds;
    std::map<std::string, FMOD::Channel*> channels;

    FMOD_RESULT result;
    void ERRCHECK(FMOD_RESULT res);
};

#endif /* FMODMANAGER_HPP */

