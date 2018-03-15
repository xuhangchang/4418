#ifndef _FUNCTION_H_
#define _FUNCTION_H_

int rgb2jpeg(unsigned char* rgb_buffer, int width, int height, int quality, unsigned char** jpeg_buffer, unsigned long* jpeg_size);

int jpeg2rgb(unsigned char* jpeg_buffer, int jpeg_size, unsigned char* rgb_buffer, int*size, int* width, int* height);

#endif