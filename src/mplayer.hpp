#ifndef MPLAYER_HPP
#define MPLAYER_HPP

#include <alsa/asoundlib.h>

class MPlayer {
public:
    MPlayer();
    int openDevice();
    void closeDevice();
    void writeBuf(float* buf, int nbFrames, int nbTimes);
    void readFromInput();
private:
    snd_pcm_t* m_handle;
    snd_pcm_uframes_t m_frames;
    snd_pcm_format_t m_format;
    int m_nbChannels; 
    int m_sampleRate;
    snd_pcm_uframes_t m_periodSize; 
    snd_pcm_uframes_t m_bufferSize;

};


#endif // MPLAYER_HPP
