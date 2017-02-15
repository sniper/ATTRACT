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
    void playSound(std::string name);
    void setPaused(bool state);
    bool isPlaying();
    std::string getCurSound();
    void stopSound();
private:
    std::string RESOURCE_DIR;
    std::string curSound;
    FMOD::System *fmodSystem;
    std::map<std::string, FMOD::Sound*> sounds;
    FMOD::Channel *channel;
    FMOD_RESULT result;
    void ERRCHECK(FMOD_RESULT res);
};

#endif /* FMODMANAGER_HPP */

