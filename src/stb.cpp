#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if !defined(__EMSCRIPTEN__) && !defined(NDEBUG)

#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#endif
