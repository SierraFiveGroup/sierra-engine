#include "al_buffer.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

namespace Sierra {
    ALBuffer::ALBuffer() {
        alGenBuffers(1, &mBuffer);
    }
    
    ALBuffer::~ALBuffer() {
        alDeleteBuffers(1, &mBuffer);
    }
    
    bool ALBuffer::loadFromFile(const std::string &filename) {
        std::string ext = filename.substr(filename.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
        if (ext == "wav") return loadWAV(filename);
        if (ext == "mp3") return loadMP3(filename);
    
        ERROR("Unsupported file extension: " << ext);
        return false;
    }
    
    bool ALBuffer::loadWAV(const std::string &filename) {
        drwav wav;
        if (!drwav_init_file(&wav, filename.c_str(), nullptr)) {
            ERROR("Failed to load WAV: " << filename);
            return false;
        }
    
        std::vector<int16_t> data(wav.totalPCMFrameCount * wav.channels);
        drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, data.data());
        drwav_uninit(&wav);
    
        ALenum format = (wav.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        alBufferData(mBuffer, format, data.data(), data.size() * sizeof(int16_t), wav.sampleRate);
        return true;
    }
    
    bool ALBuffer::loadMP3(const std::string &filename) {
        drmp3 mp3;
        if (!drmp3_init_file(&mp3, filename.c_str(), nullptr)) {
            ERROR("Failed to load MP3: " << filename);
            return false;
        }
    
        uint64_t frameCount = drmp3_get_pcm_frame_count(&mp3);
        std::vector<int16_t> data(frameCount * mp3.channels);
        drmp3_read_pcm_frames_s16(&mp3, frameCount, data.data());
        drmp3_uninit(&mp3);
    
        ALenum format = (mp3.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        alBufferData(mBuffer, format, data.data(), data.size() * sizeof(int16_t), mp3.sampleRate);
        return true;
    }
    
}

