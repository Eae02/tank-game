#ifdef GL_FUNC

GL_FUNC(glDisable, PFNGLDISABLEPROC)
GL_FUNC(glEnable, PFNGLENABLEPROC)

GL_FUNC(glGetIntegerv, PFNGLGETINTEGERVPROC)
GL_FUNC(glGetString, PFNGLGETSTRINGPROC)

GL_FUNC(glPixelStorei, PFNGLPIXELSTOREIPROC)
GL_FUNC(glScissor, PFNGLSCISSORPROC)
GL_FUNC(glViewport, PFNGLVIEWPORTPROC)
GL_FUNC(glDepthFunc, PFNGLDEPTHFUNCPROC)
GL_FUNC(glDepthMask, PFNGLDEPTHMASKPROC)

GL_FUNC(glBlendEquationSeparate, PFNGLBLENDEQUATIONSEPARATEPROC)
GL_FUNC(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC)

GL_FUNC(glFlush, PFNGLFLUSHPROC)
GL_FUNC(glFenceSync, PFNGLFENCESYNCPROC)
GL_FUNC(glDeleteSync, PFNGLDELETESYNCPROC)
GL_FUNC(glClientWaitSync, PFNGLCLIENTWAITSYNCPROC)
GL_FUNC(glWaitSync, PFNGLWAITSYNCPROC)

GL_FUNC(glDrawArrays, PFNGLDRAWARRAYSPROC)
GL_FUNC(glDrawElements, PFNGLDRAWELEMENTSPROC)
GL_FUNC(glDrawArraysInstanced, PFNGLDRAWARRAYSINSTANCEDPROC)

GL_FUNC(glGenTextures, PFNGLGENTEXTURESPROC)
GL_FUNC(glDeleteTextures, PFNGLDELETETEXTURESPROC)
GL_FUNC(glActiveTexture, PFNGLACTIVETEXTUREPROC)
GL_FUNC(glBindTexture, PFNGLBINDTEXTUREPROC)
GL_FUNC(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC)
GL_FUNC(glTexStorage2D, PFNGLTEXSTORAGE2DPROC)
GL_FUNC(glTexStorage3D, PFNGLTEXSTORAGE3DPROC)
GL_FUNC(glTexImage2D, PFNGLTEXIMAGE2DPROC)
GL_FUNC(glTexImage3D, PFNGLTEXIMAGE3DPROC)
GL_FUNC(glTexParameterf, PFNGLTEXPARAMETERFPROC)
GL_FUNC(glTexParameteri, PFNGLTEXPARAMETERIPROC)
GL_FUNC(glTexSubImage2D, PFNGLTEXSUBIMAGE2DPROC)
GL_FUNC(glTexSubImage3D, PFNGLTEXSUBIMAGE3DPROC)
GL_FUNC(glGetnTexImage, PFNGLGETNTEXIMAGEPROC)

GL_FUNC(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC)
GL_FUNC(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC)
GL_FUNC(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC)
GL_FUNC(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC)
GL_FUNC(glVertexAttribDivisor, PFNGLVERTEXATTRIBDIVISORPROC)
GL_FUNC(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC)

GL_FUNC(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC)
GL_FUNC(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC)
GL_FUNC(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC)
GL_FUNC(glClearBufferfv, PFNGLCLEARBUFFERFVPROC)
GL_FUNC(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC)
GL_FUNC(glDrawBuffer, PFNGLDRAWBUFFERPROC)
GL_FUNC(glDrawBuffers, PFNGLDRAWBUFFERSPROC)

GL_FUNC(glCreateShader, PFNGLCREATESHADERPROC)
GL_FUNC(glDeleteShader, PFNGLDELETESHADERPROC)
GL_FUNC(glCreateProgram, PFNGLCREATEPROGRAMPROC)
GL_FUNC(glDeleteProgram, PFNGLDELETEPROGRAMPROC)
GL_FUNC(glAttachShader, PFNGLATTACHSHADERPROC)
GL_FUNC(glGetProgramiv, PFNGLGETPROGRAMIVPROC)
GL_FUNC(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC)
GL_FUNC(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC)
GL_FUNC(glGetUniformBlockIndex, PFNGLGETUNIFORMBLOCKINDEXPROC)
GL_FUNC(glUniformBlockBinding, PFNGLUNIFORMBLOCKBINDINGPROC)
GL_FUNC(glUseProgram, PFNGLUSEPROGRAMPROC)
GL_FUNC(glLinkProgram, PFNGLLINKPROGRAMPROC)
GL_FUNC(glCompileShader, PFNGLCOMPILESHADERPROC)
GL_FUNC(glShaderSource, PFNGLSHADERSOURCEPROC)
GL_FUNC(glGetShaderiv, PFNGLGETSHADERIVPROC)
GL_FUNC(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC)

GL_FUNC(glUniform1f, PFNGLUNIFORM1FPROC)
GL_FUNC(glUniform1i, PFNGLUNIFORM1IPROC)
GL_FUNC(glUniform2f, PFNGLUNIFORM2FPROC)
GL_FUNC(glUniform2i, PFNGLUNIFORM2IPROC)
GL_FUNC(glUniform3f, PFNGLUNIFORM3FPROC)
GL_FUNC(glUniform3i, PFNGLUNIFORM3IPROC)
GL_FUNC(glUniform4f, PFNGLUNIFORM4FPROC)
GL_FUNC(glUniform4i, PFNGLUNIFORM4IPROC)
GL_FUNC(glUniform1fv, PFNGLUNIFORM1FVPROC)
GL_FUNC(glUniform1iv, PFNGLUNIFORM1IVPROC)
GL_FUNC(glUniform2fv, PFNGLUNIFORM2FVPROC)
GL_FUNC(glUniform2iv, PFNGLUNIFORM2IVPROC)
GL_FUNC(glUniform3fv, PFNGLUNIFORM3FVPROC)
GL_FUNC(glUniform3iv, PFNGLUNIFORM3IVPROC)
GL_FUNC(glUniform4fv, PFNGLUNIFORM4FVPROC)
GL_FUNC(glUniform4iv, PFNGLUNIFORM4IVPROC)
GL_FUNC(glUniformMatrix3fv, PFNGLUNIFORMMATRIX3FVPROC)
GL_FUNC(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC)

GL_FUNC(glDeleteBuffers, PFNGLDELETEBUFFERSPROC)
GL_FUNC(glGenBuffers, PFNGLGENBUFFERSPROC)
GL_FUNC(glBindBuffer, PFNGLBINDBUFFERPROC)
GL_FUNC(glBindBufferRange, PFNGLBINDBUFFERRANGEPROC)
GL_FUNC(glBindBufferBase, PFNGLBINDBUFFERBASEPROC)
GL_FUNC(glBufferData, PFNGLBUFFERDATAPROC)
GL_FUNC(glBufferSubData, PFNGLBUFFERSUBDATAPROC)
GL_FUNC(glBufferStorage, PFNGLBUFFERSTORAGEPROC)
GL_FUNC(glMapBuffer, PFNGLMAPBUFFERPROC)
GL_FUNC(glMapBufferRange, PFNGLMAPBUFFERRANGEPROC)
GL_FUNC(glFlushMappedBufferRange, PFNGLFLUSHMAPPEDBUFFERRANGEPROC)
GL_FUNC(glUnmapBuffer, PFNGLUNMAPBUFFERPROC)

GL_FUNC(glDebugMessageCallback, PFNGLDEBUGMESSAGECALLBACKPROC)
GL_FUNC(glDebugMessageControl, PFNGLDEBUGMESSAGECONTROLPROC)
#endif
