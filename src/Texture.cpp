#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"

using namespace std;

/*
 * getint and getshort are help functions to load the bitmap byte by byte
 */
static unsigned int getint(FILE *fp) {
    int c, c1, c2, c3;

    /*  get 4 bytes */
    c = getc(fp);
    c1 = getc(fp);
    c2 = getc(fp);
    c3 = getc(fp);

    return ((unsigned int) c) +
            (((unsigned int) c1) << 8) +
            (((unsigned int) c2) << 16) +
            (((unsigned int) c3) << 24);
}

static unsigned int getshort(FILE *fp) {
    int c, c1;

    /* get 2 bytes*/
    c = getc(fp);
    c1 = getc(fp);

    return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

Texture::Texture() :
filename(""),
tid(0) {

}

Texture::~Texture() {

}

void Texture::init() {
    // Load texture
    int w, h, ncomps;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &w, &h, &ncomps, 0);
    if (!data) {
        cerr << filename << " not found" << endl;
    }
    if (ncomps != 3) {
        //cerr << filename << " must have 3 components (RGB)" << endl;
        //cerr << ncomps << endl;
    }
    if ((w & (w - 1)) != 0 || (h & (h - 1)) != 0) {
        cerr << filename << " must be a power of 2" << endl;
    }
    width = w;
    height = h;

    // Generate a texture buffer object
    glGenTextures(1, &tid);
    GLSL::checkError(GET_FILE_LINE);

    // Bind the current texture to be the newly generated texture object
    glBindTexture(GL_TEXTURE_2D, tid);
    GLSL::checkError(GET_FILE_LINE);

    // Load the actual texture data
    // Base level is 0, number of channels is 3, and border is 0.
    if (ncomps == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (ncomps == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    GLSL::checkError(GET_FILE_LINE);

    // Generate image pyramid
    glGenerateMipmap(GL_TEXTURE_2D);
    // Set texture wrap modes for the S and T directions
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GLSL::checkError(GET_FILE_LINE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLSL::checkError(GET_FILE_LINE);

    // Set filtering mode for magnification and minimification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLSL::checkError(GET_FILE_LINE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GLSL::checkError(GET_FILE_LINE);

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    GLSL::checkError(GET_FILE_LINE);

    // Free image, since the data is now on the GPU
    stbi_image_free(data);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */

int Texture::ImageLoad(const char *filename, Image *image) {
    FILE *file;
    unsigned long size; /*  size of the image in bytes. */
    unsigned long i; /*  standard counter. */
    unsigned short int planes; /*  number of planes in image (must be 1)  */
    unsigned short int bpp; /*  number of bits per pixel (must be 24) */
    char temp; /*  used to convert bgr to rgb color. */

    /*  make sure the file is there. */
    if ((file = fopen(filename, "rb")) == NULL) {
        printf("File Not Found : %s\n", filename);
        return 0;
    }

    /*  seek through the bmp header, up to the width height: */
    fseek(file, 18, SEEK_CUR);

    /*  No 100% errorchecking anymore!!! */

    /*  read the width */ image->sizeX = getint(file);

    /*  read the height */
    image->sizeY = getint(file);

    /*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
    size = image->sizeX * image->sizeY * 3;

    /*  read the planes */
    planes = getshort(file);
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }

    /*  read the bpp */
    bpp = getshort(file);
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }

    /*  seek past the rest of the bitmap header. */
    fseek(file, 24, SEEK_CUR);

    /*  read the data.  */
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }

    for (i = 0; i < size; i += 3) { /*  reverse all of the colors. (bgr -> rgb) */
        temp = image->data[i];
        image->data[i] = image->data[i + 2];
        image->data[i + 2] = temp;
    }

    fclose(file); /* Close the file and release the filedes */

    /*  we're done. */
    return 1;
}

void Texture::initBMP() {
    Texture::Image *image0 = (Texture::Image *)malloc(sizeof (Texture::Image));
    if (image0 == NULL) {
        printf("Error allocating space for image");
    }
    if (!Texture::ImageLoad(filename.c_str(), image0)) {
        printf("Error loading texture image\n");
    }

    // Set active texture to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    // Generate a texture buffer object
    glGenTextures(1, &tid);
    // Bind the current texture to be the newly generated texture object
    glBindTexture(GL_TEXTURE_2D, tid);
    // Load the actual texture data
    // Base level is 0, number of channels is 3, and border is 0.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image0->sizeX, image0->sizeY,
            0, GL_RGB, GL_UNSIGNED_BYTE, image0->data);
    // Generate image pyramid
    glGenerateMipmap(GL_TEXTURE_2D);
    // Set texture wrap modes for the S and T directions
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set filtering mode for magnification and minimification
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    // Free image, since the data is now on the GPU
    free(image0);
    cout << "WHOO" << endl;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

void Texture::loadDDS(const char * imagepath) {

    unsigned char header[124];

    FILE *fp;

    /* try to open the file */
    fp = fopen(imagepath, "rb");
    if (fp == NULL) {
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
        getchar();

    }

    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);

    }

    /* get the surface desc */
    fread(&header, 124, 1, fp);

    unsigned int height = *(unsigned int*) &(header[8 ]);
    unsigned int width = *(unsigned int*) &(header[12]);
    unsigned int linearSize = *(unsigned int*) &(header[16]);
    unsigned int mipMapCount = *(unsigned int*) &(header[24]);
    unsigned int fourCC = *(unsigned int*) &(header[80]);


    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*) malloc(bufsize * sizeof (unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC) {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            free(buffer);

    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
        unsigned int size = ((width + 3) / 4)*((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                0, size, buffer + offset);

        offset += size;
        width /= 2;
        height /= 2;

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if (width < 1) width = 1;
        if (height < 1) height = 1;

    }

    free(buffer);
    cout << "herezzzz" << endl;
    tid = textureID;


}

void Texture::setWrapModes(GLint wrapS, GLint wrapT) {
    // Must be called after init()
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::bind(GLint handle) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, tid);
    glUniform1i(handle, unit);
}

void Texture::unbind() {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}
