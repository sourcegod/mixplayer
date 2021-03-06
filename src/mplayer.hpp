#ifndef MPLAYER_HPP
#define MPLAYER_HPP

#include <memory> // smart pointers
#include <alsa/asoundlib.h>
#include <sndfile.h>

class MPlayer {
public:
    MPlayer();
    ~MPlayer();
    
    int openDevice();
    void closeDevice();
    void writeBuf(float* buf, int nbFrames, int nbTimes);
    void readFromInput();
    
    void loadFile(const char* filename);
    bool readSoundFile(const char* filename);
    void printSoundInfo() const;

private:
    const char* m_filename;
    SF_INFO m_soundInfo;
    SNDFILE* m_soundFile;
    std::unique_ptr<float> m_bufSound;
    
    snd_pcm_t* m_handle;
    snd_pcm_format_t m_format;
    int m_nbChannels; 
    int m_sampleRate;
    int m_nbFrames;
    snd_pcm_uframes_t m_periodSize; 
    snd_pcm_uframes_t m_bufferSize;

};


#endif // MPLAYER_HPP
