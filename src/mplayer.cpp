/*
 * File: mplayer.cpp
 * Audio Driver for alsa sound
 * Author: Coolbrother
 * Date: Thu, 04/03/2021
 * 
 * */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <alsa/asoundlib.h>
#include <sndfile.h>
#include "mplayer.hpp"

#define APPNAME "AlsaDriver"
#define printErr(msg) (fprintf(stderr, "%s\n", msg), exit(1))

MPlayer::MPlayer() {
    this->m_handle = NULL;
    this->m_format = SND_PCM_FORMAT_FLOAT_LE;
    this->m_nbChannels =2; 
    this->m_sampleRate =44100;
    this->m_periodSize =0; 
    this->m_bufferSize =0;

}
//-----------------------------------------

MPlayer::~MPlayer() {
    this->closeDevice();
}
//-----------------------------------------

int MPlayer::openDevice() {
    // open sound device and set params
 	  const static char *device = "default";
	  // snd_output_t *output = NULL;
   int err;

    if ((err = snd_pcm_open(&m_handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
      fprintf(stderr, "%s: Playback open error: %s\n", APPNAME, snd_strerror(err));
      return EXIT_FAILURE;
    }

    if ((err = snd_pcm_set_params(m_handle,
        m_format,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        m_nbChannels,
        m_sampleRate,
        1, /* period */
        m_sampleRate / 4)) < 0) { // 50000000)) < 0) {	 /* latency: 0.5sec */ 
      fprintf(stderr, "%s: Playback open error: %s\n", APPNAME, snd_strerror(err));
      return EXIT_FAILURE;
    }

    return err;
}
//-----------------------------------------

void MPlayer::closeDevice() {
    // closing sound device
    // necessary to flush and send short sample
    if (m_handle) {
        std::cout << "Closing the device.." << std::endl;
        snd_pcm_drain(m_handle);
	      snd_pcm_close(m_handle);
    }

}
//-----------------------------------------

void MPlayer::writeBuf(float* buf, int nbFrames, int nbTimes) {
  for (int i=0; i < nbTimes; i++) {
      // Sending the sound
      m_nbFrames += snd_pcm_writei(m_handle, buf, nbFrames);
  }

}
//-----------------------------------------

void MPlayer::readFromInput() {
    if (snd_pcm_get_params(m_handle, &m_bufferSize, &m_periodSize) < 0) {
        printErr("Error: snd_pcm_get_params");
    }
    printf("period_size = %ld\n", (long)m_periodSize);
    printf("buffer_size = %ld\n", (long)m_bufferSize);
    const int bufSz = m_periodSize * m_nbChannels * sizeof(float);
    void* buf = malloc(bufSz);
    for (;;) {
        memset(buf, 0, bufSz);
        const ssize_t rd_sz = read(STDIN_FILENO, buf, bufSz);
        if (rd_sz < 0) {
            printErr("Error: read(stdin)");
        }
        if (rd_sz == 0) {
            break;
        }
        
        // write audio
        int nbFrames = snd_pcm_writei(m_handle, buf, m_periodSize);
        if (nbFrames < 0) {
            if ((nbFrames = snd_pcm_recover(m_handle, nbFrames, 1)) == 0) {
                printf("recovered after xrun (overrun/underrun)\n");
            }
            printErr("Error: snd_pcm_writei");
        }
    
    } // end FOR

    free(buf);

}
//-----------------------------------------

bool MPlayer::readSoundFile(const char* filename) {
    m_filename = filename;
    m_soundFile = sf_open(m_filename, SFM_READ, &m_soundInfo);
    if (m_soundFile) {
        m_nbChannels = m_soundInfo.channels;
        m_sampleRate = m_soundInfo.samplerate;
        m_nbFrames = m_soundInfo.frames;
        // Read the entire sound file to buffer on the heap
        m_bufSound.reset(new float[m_nbFrames * m_nbChannels]);
        // m_bufSound = new float[m_nbFrames * m_nbChannels];
        sf_count_t ret = sf_readf_float(m_soundFile, m_bufSound.get(), m_nbFrames);
        // sf_count_t ret = sf_readf_float(m_soundFile, m_bufSound, m_nbFrames);
        std::cout << "nbFrames read: " << ret << std::endl;
        sf_close(m_soundFile);
        if (ret == 0) {
            return false;
        }

        return true;
    } else {
        sf_close(m_soundFile);
        // throw "Unable to read the input file!";
        std::cerr << "Unable to read the input file " << filename << "!" << std::endl;
        return false;
    }

}
//-----------------------------------------

void MPlayer::loadFile(const char* filename) {
    
    this->readSoundFile(filename);
    // this->closeDevice();
}
//-----------------------------------------

void MPlayer::printSoundInfo() const {
    std::cout << "Filename: " << m_filename << std::endl;
    std::cout << "Number of Channels: " << m_nbChannels << std::endl;
    std::cout << "Number of Frames: " << m_nbFrames << std::endl;
    std::cout << "Sample Rate: " << m_sampleRate << std::endl;

}
//-----------------------------------------

void MPlayer::play() {
    int ret;
    if (snd_pcm_get_params(m_handle, &m_bufferSize, &m_periodSize) < 0) {
        printErr("Error: snd_pcm_get_params");
    }
    m_periodSize = 256;
    // m_bufferSize = m_periodSize * 3;
    printf("period_size = %ld\n", (long)m_periodSize);
    printf("buffer_size = %ld\n", (long)m_bufferSize);
    std::cout << std::endl;
    int maxFrames = m_nbFrames * m_nbChannels;
    int maxSize = m_periodSize * m_nbChannels;
    std::cout << "maxFrames: " << maxFrames << ", maxSize: " << maxSize << std::endl;
    // for (int i = 0; i < maxFrames; i += m_bufferSize) {
        
        // ret = snd_pcm_writei(m_handle, m_bufSound.get() + i, m_bufferSize);
        ret = snd_pcm_writei(m_handle, m_bufSound.get(), m_nbFrames);
        std::cout << "ret: " << ret << std::endl;
        if (ret < 0) {
          std::cerr << "Error: XRun is ocurred\n";
          // break;
        }
    // }

}
//-----------------------------------------

