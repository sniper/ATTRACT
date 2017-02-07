/* 
 * File:   FmodManager.cpp
 * Author: Darryl
 * 
 * Created on February 6, 2017, 3:39 PM
 */

#include "FmodManager.hpp"

FmodManager::FmodManager() :
channel(0),
sound(NULL) {
    /* Create a System object and initialize */
    result = System_Create(&fmodSystem);
    ERRCHECK(result);

    result = fmodSystem->init(32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);

}

FmodManager::FmodManager(const FmodManager& orig) {
}

FmodManager::~FmodManager() {
    result = sound->release(); /* Release the parent, not the sound that was retrieved with getSubSound. */
    ERRCHECK(result);
    result = fmodSystem->close();
    ERRCHECK(result);
    result = fmodSystem->release();
    ERRCHECK(result);

}

/*note: using a stream uses less ram, but uses more cpu.
 if framerate becomes an issue change this!*/
void FmodManager::createStream(string path) {
    result = fmodSystem->createStream(path.c_str(), (FMOD_MODE) FMOD_LOOP_NORMAL, NULL, &sound);
    ERRCHECK(result);
}

void FmodManager::playSound() {
    result = fmodSystem->playSound(sound, 0, false, &channel);
    ERRCHECK(result);
}

void FmodManager::setPaused(bool state) {
    result = channel->setPaused(state);
    ERRCHECK(result);
    
}

void FmodManager::ERRCHECK(FMOD_RESULT res) {
    if(res != FMOD_OK)
        cout << "FMOD ERROR: " << FMOD_ErrorString(res) << endl;
}
