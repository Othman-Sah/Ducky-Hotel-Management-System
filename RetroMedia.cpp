#include "RetroMedia.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace RetroUI {
    std::vector<std::string> playlist;
    int currentSongIndex = 0;
    bool isMusicPlaying = false;
    bool isMusicPaused = false;
    int mediaButtonFocus = -1;

    void loadMusic() {
        if (!playlist.empty()) return;
        WIN32_FIND_DATAA findFileData;
        std::string folder = "music/";
        HANDLE hFind = FindFirstFileA((folder + "*.mp3").c_str(), &findFileData);
        
        if (hFind == INVALID_HANDLE_VALUE) {
            folder = "musics/";
            hFind = FindFirstFileA((folder + "*.mp3").c_str(), &findFileData);
        }

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                playlist.push_back(folder + findFileData.cFileName);
            } while (FindNextFileA(hFind, &findFileData) != 0);
            FindClose(hFind);
        }
    }

    void playMusic() {
        if (playlist.empty()) return;
        mciSendStringA("close bgm", NULL, 0, NULL);
        std::string cmd = "open \"" + playlist[currentSongIndex] + "\" type mpegvideo alias bgm";
        mciSendStringA(cmd.c_str(), NULL, 0, NULL);
        mciSendStringA("play bgm", NULL, 0, NULL);
        isMusicPlaying = true;
        isMusicPaused = false;
    }

    void stopMusic() {
        mciSendStringA("close bgm", NULL, 0, NULL);
        isMusicPlaying = false;
        isMusicPaused = false;
    }

    void togglePauseMusic() {
        if (isMusicPlaying) {
            mciSendStringA(isMusicPaused ? "resume bgm" : "pause bgm", NULL, 0, NULL);
            isMusicPaused = !isMusicPaused;
        } else playMusic();
    }

    void nextMusic() { stopMusic(); if(!playlist.empty()) { currentSongIndex = (currentSongIndex + 1) % playlist.size(); playMusic(); } }
}