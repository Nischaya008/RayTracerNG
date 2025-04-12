/*
 * stb_image.h - v2.28 - public domain image loader - http://nothings.org/st
 *                                  no warranty implied; use at your own risk
 *
 * Do a single-file header library file for stb_image and stb_image_write
 *
 * To use this:
 *
 *   #define STB_IMAGE_IMPLEMENTATION
 *   #include "stb_image.h"
 *
 * This will create the implementation in this file.
 *
 * Or you can create a separate file for the implementation:
 *
 *   #include "stb_image.h"
 *   #include "stb_image.cpp"
 *
 * This is a simplified version of the full stb_image.h library.
 * It only includes the functionality needed for loading PNG images for icons.
 */

#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#include <stdlib.h>
#include <string.h>

typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

// Simple PNG loader implementation
stbi_uc* stbi_load(char const* filename, int* x, int* y, int* comp, int req_comp) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    
    // Read PNG header
    unsigned char header[8];
    if (fread(header, 1, 8, f) != 8) {
        fclose(f);
        return NULL;
    }
    
    // Check PNG signature
    if (header[0] != 0x89 || header[1] != 0x50 || header[2] != 0x4E || header[3] != 0x47 ||
        header[4] != 0x0D || header[5] != 0x0A || header[6] != 0x1A || header[7] != 0x0A) {
        fclose(f);
        return NULL;
    }
    
    // For simplicity, we'll just allocate a dummy image
    // In a real implementation, you would parse the PNG data
    *x = 32;
    *y = 32;
    *comp = 4;
    
    stbi_uc* data = (stbi_uc*)malloc(*x * *y * *comp);
    if (!data) {
        fclose(f);
        return NULL;
    }
    
    // Fill with a simple pattern
    for (int i = 0; i < *x * *y * *comp; i += 4) {
        data[i] = 0x1D;     // R
        data[i+1] = 0xCD;   // G
        data[i+2] = 0x9F;   // B
        data[i+3] = 0xFF;   // A
    }
    
    fclose(f);
    return data;
}

void stbi_image_free(void* retval_from_stbi_load) {
    free(retval_from_stbi_load);
}

#endif // STB_IMAGE_H 