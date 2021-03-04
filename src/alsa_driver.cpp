/*
 * Audio Driver for alsa sound
 * Author: Coolbrother
 * Date: Thu, 04/03/2021
 * 
 * */

#include <stdlib.h>
#include <stdio.h>
#include <alsa/asoundlib.h>
#define APPNAME "AlsaDriver"
snd_pcm_t *g_handle;
snd_pcm_sframes_t g_frames;
int channels =2;
snd_pcm_format_t format = SND_PCM_FORMAT_FLOAT;
int rate = 44100;

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
        channels,
        rate,
        1, /* period */
        500000)) < 0) {	 /* latency: 0.5sec */ 
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


