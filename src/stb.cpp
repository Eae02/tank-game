#ifndef __EMSCRIPTEN__
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#endif

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>