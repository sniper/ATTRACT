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
RESOURCE_DIR(resource +"sound/"),
curSound(""){
    /* Create a System object and initialize */
    result = System_Create(&fmodSystem);
    ERRCHECK(result);

    result = fmodSystem->init(32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);

    /*create sounds */
    createStream("game", "game.mp3",true);
    createStream("menu", "menu_old.mp3",true);
    createStream("jump", "jump.mp3",false);
    createStream("jump_land", "jump_land.mp3",false);
    createStream("magnet", "magnet.mp3",false);
    createStream("walking", "walking.mp3",false);
    createStream("click", "click.mp3",false);
    createStream("select", "select.mp3",false);
    createStream("choose", "choose.mp3",false);
    createStream("death", "death.mp3",false);
    createStream("win", "win.mp3",false);
    createStream("flying", "flying.mp3", true);
    createStream("gps","gps.mp3", false);
    createStream("error", "error.mp3", false);
    createStream("crash", "crash.mp3", false);
    createStream("collecting", "collecting.mp3", true);
    createStream("start","start.mp3",false);
    createStream("gunget","gunget.mp3",false);
    createStream("boom","boom.mp3",false);

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
    Channel *channel = 0;
    
    if(loop) 
        result = fmodSystem->createSound((RESOURCE_DIR + path).c_str(), (FMOD_MODE) FMOD_LOOP_NORMAL, NULL, &sound);
    else
        result = fmodSystem->createSound((RESOURCE_DIR + path).c_str(), (FMOD_MODE) FMOD_LOOP_OFF, NULL, &sound);
    ERRCHECK(result);
    sounds.insert(make_pair(name, sound));
    
    channels.insert(make_pair(name,channel));
    
}

/*bool checks whether its a looping sound*/
void FmodManager::playSound(string name, bool loop) {
    result = fmodSystem->playSound(sounds[name], 0, false, &channels[name]);
    //ERRCHECK(result);
    if(loop)
        curSound = name;
}

void FmodManager::playSound(string name, bool loop, float volume) {
    
    
    result = fmodSystem->playSound(sounds[name], 0, false, &channels[name]);
    //ERRCHECK(result);
    result = channels[name]->setVolume(volume);
    //ERRCHECK(result);
    if(loop)
        curSound = name;
}

void FmodManager::stopSound(string name) {
    result = channels[name]->stop();
    ERRCHECK(result);
}

string FmodManager::getCurSound() {
    return curSound;
}

bool FmodManager::isPlaying(string name) {

    bool ret;
    if(channels[name] == 0)
        return false;
    result = channels[name]->isPlaying(&ret);
    ERRCHECK(result);
    return ret;
    
}

void FmodManager::setPaused(string name, bool state) {
    result = channels[name]->setPaused(state);
    ERRCHECK(result);
    
}

void FmodManager::ERRCHECK(FMOD_RESULT res) {
    if(res != FMOD_OK)
        cout << "FMOD ERROR: " << FMOD_ErrorString(res) << endl;
}
