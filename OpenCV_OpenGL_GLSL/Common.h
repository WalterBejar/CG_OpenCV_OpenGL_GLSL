
#pragma once

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <GL\freeglut_ext.h>
#include <GLFW\glfw3.h>
#include <opencv2\core\core.hpp>
#include <vector>

int exit_with_errors(char* error);

void open_imgs_dir(char* dir_name, std::vector<cv::Mat>& images, std::vector<std::string>& images_names, double downscale_factor);

char *textFileRead(char *fn);

void printShaderLog(GLuint prog);

void printProgramLog(GLuint shad);

GLuint setShaders(char * vert, char * frag);

GLuint matToTexture(cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);