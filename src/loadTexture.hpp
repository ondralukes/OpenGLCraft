#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

GLuint loadBmp(const char * path);
GLuint loadDDS(const char * path, bool filtering = true);
