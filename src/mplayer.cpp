/*
 * Audio Driver for alsa sound
 * Author: Coolbrother
 * Date: Thu, 04/03/2021
 * 
 * */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <alsa/asoundlib.h>
#define APPNAME "AlsaDriver"
#define printErr(msg) (fprintf(stderr, "%s\n", msg), exit(1))

snd_pcm_t *g_handle;
snd_pcm_sframes_t g_frames;
// snd_pcm_format_t format = SND_PCM_FORMAT_FLOAT;
snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
int nbChannels =2, sampleRate =44100;
snd_pcm_uframes_t periodSize =0, bufferSize =0;

int openDevice() {
    // open sound device and set params
 	  const static char *device = "default";
	  // snd_output_t *output = NULL;
   int err;

    if ((err = snd_pcm_open(&g_handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
      fprintf(stderr, "%s: Playback open error: %s\n", APPNAME, snd_strerror(err));
      return EXIT_FAILURE;
    }

    if ((err = snd_pcm_set_params(g_handle,
        format,
        // SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        nbChannels,
        sampleRate,
        1, /* period */
        sampleRate / 4)) < 0) { // 50000000)) < 0) {	 /* latency: 0.5sec */ 
      fprintf(stderr, "%s: Playback open error: %s\n", APPNAME, snd_strerror(err));
      return EXIT_FAILURE;
    }

    return err;
}
//-----------------------------------------

void closeDevice() {
    // closing sound device
    // necessary to flush and send short sample
    snd_pcm_drain(g_handle);
	  snd_pcm_close(g_handle);

}
//-----------------------------------------

void writeBuf(float* buf, int nbFrames, int nbTimes) {
  for (int i=0; i < nbTimes; i++) {
      // Sending the sound
      g_frames += snd_pcm_writei(g_handle, buf, nbFrames);
  }
  // printf("WriteBuf nbFrames: %d\n", g_frames);

}
//-----------------------------------------


void writeFile() {
    if (snd_pcm_get_params(g_handle, &bufferSize, &periodSize) < 0) {
        printErr("Error: snd_pcm_get_params");
    }
    printf("period_size = %ld\n", (long)periodSize);
    printf("buffer_size = %ld\n", (long)bufferSize);
    const int bufSz = periodSize * nbChannels * sizeof(float);
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
        int nbFrames = snd_pcm_writei(g_handle, buf, periodSize);
        if (nbFrames < 0) {
            if ((nbFrames = snd_pcm_recover(g_handle, nbFrames, 1)) == 0) {
                printf("recovered after xrun (overrun/underrun)\n");
            }
            printErr("Error: snd_pcm_writei");
        }
    
    } // end FOR

    free(buf);

}
//-----------------------------------------

