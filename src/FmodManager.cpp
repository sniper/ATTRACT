/* 
 * File:   FmodManager.cpp
 * Author: Darryl
 * 
 * Created on February 6, 2017, 3:39 PM
 */

#include "FmodManager.hpp"

using namespace std;
using namespace FMOD;

FmodManager::FmodManager(string resource) :
channel(0),
RESOURCE_DIR(resource +"sound/"),
curSound(""){
    /* Create a System object and initialize */
    result = System_Create(&fmodSystem);
    ERRCHECK(result);

    result = fmodSystem->init(32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);

    /*create sounds */
    createStream("game", "game.mp3",true);
    createStream("menu", "menu.mp3",true);
    createStream("jump", "jump.mp3",false);
    createStream("jump_land", "jump_land.mp3",false);
    createStream("magnet", "magnet.mp3",false);
    createStream("walking", "walking.mp3",false);
}

FmodManager::FmodManager(const FmodManager& orig) {
}

FmodManager::~FmodManager() {
    result = fmodSystem->close();
    ERRCHECK(result);
    result = fmodSystem->release();
    ERRCHECK(result);

}

/*note: using a stream uses less ram, but uses more cpu.
 if framerate becomes an issue change this!
 bool is for whether you want the sound to loop or not*/
void FmodManager::createStream(string name, string path, bool loop) {
    Sound *sound;
    if(loop) 
        result = fmodSystem->createStream((RESOURCE_DIR + path).c_str(), (FMOD_MODE) FMOD_LOOP_OFF, NULL, &sound);
    else
        result = fmodSystem->createStream((RESOURCE_DIR + path).c_str(), (FMOD_MODE) FMOD_LOOP_NORMAL, NULL, &sound);
    ERRCHECK(result);
    sounds.insert(make_pair(name, sound));
}


void FmodManager::playSound(string name) {
    result = fmodSystem->playSound(sounds[name], 0, false, &channel);
    ERRCHECK(result);
    curSound = name;
}

void FmodManager::stopSound() {
    result = channel->stop();
    ERRCHECK(result);
}

string FmodManager::getCurSound() {
    return curSound;
}

bool FmodManager::isPlaying() {
    bool ret;
    if(channel == 0)
        return false;
    result = channel->isPlaying(&ret);
    ERRCHECK(result);
    return ret;
}

void FmodManager::setPaused(bool state) {
    result = channel->setPaused(state);
    ERRCHECK(result);
    
}

void FmodManager::ERRCHECK(FMOD_RESULT res) {
    if(res != FMOD_OK)
        cout << "FMOD ERROR: " << FMOD_ErrorString(res) << endl;
}
