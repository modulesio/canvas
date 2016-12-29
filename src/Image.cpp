#include <Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace canvas;

static InternalFormat getFormatFromChannelCount(int channels) {
  switch (channels) {
  case 1: return R8;
  case 2: return LUMINANCE_ALPHA;
  case 3: return RGB8;
  case 4: return RGBA8;
  }
  assert(0);
  return NO_FORMAT;
}

bool
Image::decode(const unsigned char * buffer, size_t size) {
  int w, h, channels;
  auto img_buffer = stbi_load_from_memory(buffer, size, &w, &h, &channels, 0);
  if (!img_buffer) {
    cerr << "Image decoding failed: " << stbi_failure_reason() << endl;
    return false;
  }
  cerr << "Image.cpp: loaded image, size = " << size << ", b = " << (void*)img_buffer << ", w = " << w << ", h = " << h << ", ch = " << channels << endl;
  assert(w && h && channels);    

  InternalFormat format = getFormatFromChannelCount(channels);
  size_t numPixels = w * h;

  if (channels == 2 || channels == 3 || channels == 4) {
    unsigned int * storage = new unsigned int[numPixels];
    for (unsigned int i = 0; i < numPixels; i++) {
      unsigned char r = img_buffer[channels * i + 0];
      unsigned char g = channels >= 2 ? img_buffer[channels * i + 1] : r;
      unsigned char b = channels >= 3 ? img_buffer[channels * i + 2] : g;
      unsigned char a = channels == 4 ? img_buffer[channels * i + 3] : 0xff;
      if (a) {
	storage[i] = (0xff * b / a) + ((0xff * g / a) << 8) + ((0xff * r / a) << 16) + (a << 24);
      } else {
	storage[i] = 0;
      }
    }
    data = std::make_shared<ImageData>((unsigned char *)storage, format, w, h);
    delete[] storage;
  } else {
    data = std::make_shared<ImageData>((unsigned char *)img_buffer, format, w, h);
  }
  stbi_image_free(img_buffer);
  
  return true;
}

void
Image::loadFile() {
  assert(!filename.empty());
  int w, h, channels;
  cerr << "trying to load " << filename << endl;
  auto img_buffer = stbi_load(filename.c_str(), &w, &h, &channels, 0);
  assert(img_buffer);
  cerr << "Image.cpp: loaded image, filename = " << filename << ", b = " << (void*)img_buffer << ", w = " << w << ", h = " << h << ", ch = " << channels << endl;
  assert(w && h && channels);    

  InternalFormat format = getFormatFromChannelCount(channels);
  size_t numPixels = w * h;

  if (channels == 2 || channels == 3 || channels == 4) {
    unsigned int * storage = new unsigned int[numPixels];
    for (unsigned int i = 0; i < numPixels; i++) {
      unsigned char r = img_buffer[channels * i + 0];
      unsigned char g = channels >= 2 ? img_buffer[channels * i + 1] : r;
      unsigned char b = channels >= 3 ? img_buffer[channels * i + 2] : g;
      unsigned char a = channels == 4 ? img_buffer[channels * i + 3] : 0xff;
      if (a) {
	storage[i] = (0xff * b / a) + ((0xff * g / a) << 8) + ((0xff * r / a) << 16) + (a << 24);
      } else {
	storage[i] = 0;
      }
    }
    data = std::make_shared<ImageData>((unsigned char *)storage, format, w, h);
    delete[] storage;
  } else {
    data = std::make_shared<ImageData>((unsigned char *)img_buffer, format, w, h);
  }
  
  stbi_image_free(img_buffer);
}
