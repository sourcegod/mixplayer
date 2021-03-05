#ifndef ALSA_DRIVER_HPP
#define ALSA_DRIVER_HPP

int openDevice();
void closeDevice();
void writeBuf(float* buf, int nbFrames, int nbTimes);
void writeFile();
#endif // ALSA_DRIVER_HPP
