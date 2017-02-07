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

/*error checking not working, fix later*/


using namespace std;
using namespace FMOD;



class FmodManager {
public:
    FmodManager();
    FmodManager(const FmodManager& orig);
    virtual ~FmodManager();
    void createStream(string path);
    void playSound();
    void setPaused(bool state);
private:
    System *fmodSystem;
    Sound *sound;
    Channel *channel;
    FMOD_RESULT result;
    void ERRCHECK(FMOD_RESULT res);
};

#endif /* FMODMANAGER_HPP */

