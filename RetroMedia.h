#ifndef RETRO_MEDIA_H
#define RETRO_MEDIA_H

#include <vector>
#include <string>

namespace RetroUI {
    extern std::vector<std::string> playlist;
    extern int currentSongIndex;
    extern bool isMusicPlaying;
    extern bool isMusicPaused;
    extern int mediaButtonFocus;

    void loadMusic();
    void playMusic();
    void stopMusic();
    void togglePauseMusic();
    void nextMusic();
}
#endif