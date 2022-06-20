#include <GLES3/gl32.h>
#include <GLES3/gl2ext.h>

typedef double GLclampd;
typedef float GLclampf;
#include <GL/glext.h>

#define glTexStorage2D  ((void(*)(...))nullptr)
#define glTexStorage3D  ((void(*)(...))nullptr)
#define glBufferStorage ((void(*)(...))nullptr)
#define glGetnTexImage  ((void(*)(...))nullptr)
#define glDrawBuffer    ((void(*)(...))nullptr)
#define glMapBuffer     ((void*(*)(...))nullptr)
#define glUnmapBuffer   ((GLboolean(*)(...))nullptr)

#define APIENTRY
