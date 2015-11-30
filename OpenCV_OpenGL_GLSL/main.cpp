
#include <Windows.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <array>

#include "Common.h"

using namespace std;

/* Variables globales */
std::vector<cv::Mat> images;							// vector de imágenes
std::vector<std::string> images_names;					// nombres de la imágenes

int idxCurrentImage = 0;								// indice de la imagen actual

int    frameCount = 0;
double frameStartTime, frameEndTime, frameDrawTime;		// limitar los frames
double startTime;

GLint   windowWidth = 800;								// Define our window width
GLint   windowHeight = 600;								// Define our window height
GLfloat fieldOfView = 45.0f;							// FoV
GLfloat zNear = 0.1f;									// Near clip plane
GLfloat zFar = 200.0f;									// Far clip plane

GLuint GreyScaleProg;									// Programa para shader escala de grises
GLuint RedBlueProg;										// Porgrama para shader que cambia los canales rojo y azul
GLuint FilterProg;
GLuint ShaderProg;

bool isFilter = false;

int idxKernel = 0;										// indice del kernel actual
vector <std::array<float,9>> kernels;
vector <string> kernelsNames;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// If a key was pressed...
	if (action == GLFW_PRESS)
	{
		// ...act accordingly dependant on what key it was!
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_G:
			ShaderProg = GreyScaleProg;
			isFilter = false;
			break;

		case GLFW_KEY_R:
			ShaderProg = RedBlueProg;
			isFilter = false;
			break;

		case GLFW_KEY_F:
			ShaderProg = FilterProg;
			isFilter = true;
			cout << kernelsNames[idxKernel] << endl;
			break;

		case GLFW_KEY_UP:
			if (isFilter)
			{
				idxKernel++;
				if (idxKernel >= kernels.size())
					idxKernel = 0;
				cout << kernelsNames[idxKernel] << endl;
			}
			break;

		case GLFW_KEY_DOWN:
			if (isFilter)
			{
				idxKernel--;
				if (idxKernel < 0)
					idxKernel = kernels.size() - 1;
				cout << kernelsNames[idxKernel] << endl;
			}
			break;

		default:
			break;

		} // End of switch statement

	} // End of GLFW_PRESS
}

float computeKernelWeight(array <float, 9> kernel)
{
	unsigned int kernelSize = kernel.size();
	float valorAnterior = kernel[0];
	float valorActual = 0;

	for (unsigned int i = 1; i < kernelSize; i++)
	{
		valorActual = kernel[i];
		valorAnterior = valorAnterior + valorActual;
	}

	return valorAnterior <= 0 ? 1 : valorAnterior;
}

GLfloat* arrayToGlfloat(array <float, 9> a)
{
	GLfloat *k = new GLfloat[9];

	for (int i = 0; i < 9; i++)
		k[i] = a[i];

	return k;
}

void initKernels()
{
	array <float, 9> gb1 = { 0.045, 0.122, 0.045, 0.122, 0.332, 0.122, 0.045, 0.122, 0.045 };
	kernelsNames.push_back("Gaussian Blur 1");
	kernels.push_back(gb1);	

	array <float, 9> gb2 = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
	kernelsNames.push_back("Gaussian Blur 2");
	kernels.push_back(gb2);

	array <float, 9> gb3 = { 0, 1, 0, 1, 1, 1, 0, 1, 0 };
	kernelsNames.push_back("Gaussian Blur 3");
	kernels.push_back(gb3);

	array <float, 9> b = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	kernelsNames.push_back("Blur");
	kernels.push_back(b);

	array <float, 9> e = { -2, -1, 0, -1, 1, 1, 0, 1, 2 };
	kernelsNames.push_back("Emboss");
	kernels.push_back(e);

	array <float, 9> s = { -1, -1, -1, -1, 16, -1, -1, -1, -1 };
	kernelsNames.push_back("Sharpen");
	kernels.push_back(s);

	array <float, 9> us = { -1, -1, -1, -1, 9, -1, -1, -1, -1 };
	kernelsNames.push_back("UnSharpen");
	kernels.push_back(us);

	array <float, 9> sn = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
	kernelsNames.push_back("Sharpness");
	kernels.push_back(sn);

	array <float, 9> ed1 = { -0.125, -0.125, -0.125, -0.125, 1, -0.125, -0.125, -0.125, -0.125 };
	kernelsNames.push_back("Edge Detect 1");
	kernels.push_back(ed1);

	array <float, 9> ed2 = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
	kernelsNames.push_back("Edge Detect 2");
	kernels.push_back(ed2);

	array <float, 9> ed3 = { -5, 0, 0, 0, 0, 0, 0, 0, 5 };
	kernelsNames.push_back("Edge Detect 3");
	kernels.push_back(ed3);

	array <float, 9> sh = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
	kernelsNames.push_back("Sovel Horizontal");
	kernels.push_back(sh);

	array <float, 9> sv = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
	kernelsNames.push_back("Sovel Vertical");
	kernels.push_back(sv);

	array <float, 9> ph = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
	kernelsNames.push_back("Previt Horizontal");
	kernels.push_back(ph);

	array <float, 9> pv = { 1, 0, -1, 1, 0, -1, 1, 0, -1 };
	kernelsNames.push_back("Previt Vertical");
	kernels.push_back(pv);

	array <float, 9> p = { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0, 2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 };
	kernelsNames.push_back("Película Vieja");
	kernels.push_back(p);
}

void draw(cv::Mat &camFrame)
{
	glUseProgram(0);

	// Limpiamos la pantalla y el buffer, y reseteamos la matrix ModelView
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Movemos las imagenes para que aparezcan en pantalla
	glTranslatef(0.0f, 0.0f, -8.0f);

	// Rotación alrededor del eje Y
	glRotatef(frameCount, 0.0f, 1.0f, 0.0f);

	// Rotación alrededor del eje X
	//static float rateOfChange = 0.0008f;
	//static float degreesToMoveThrough = 180.0f;
	//glRotatef(sin(frameCount * rateOfChange) * degreesToMoveThrough, 1.0f, 0.0f, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

	// Rotación alrededor del eje Z
	//glRotatef(cos(frameCount * rateOfChange) * degreesToMoveThrough, 0.0f, 1.0f, 0.0f);	

	// Ancho y alto del Quad (4:3)
	float w = 6.4f;
	float h = 4.8f;

	// Comvertimos la imagen cargada por OpenCV a una textura de OpenGL
	GLuint imageTex = matToTexture(camFrame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
	
	// Variable Textura que será cargada por el shader
	GLint text = glGetUniformLocation(ShaderProg, "Texture");

	// Shader que se llegará a usar
	glUseProgram(ShaderProg);
	
	// Textura a utilizar por el shader
	glUniform1i(text, 0);

	// si es filtro
	if (isFilter)
	{
		GLint kernelValue = glGetUniformLocation(ShaderProg, "Kernel[0]");		
		GLint kernelWeight = glGetUniformLocation(ShaderProg, "KernelWeight");
		GLint textureSize = glGetUniformLocation(ShaderProg, "TextureSize");
		
		glUniform1fv(kernelValue, 9, arrayToGlfloat(kernels[idxKernel]));
		glUniform1f(kernelWeight, computeKernelWeight(kernels[idxKernel]));
		glUniform2f(textureSize, 800, 600);
	}

	// Textura Front
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, imageTex);

	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(-w / 2, h / 2);
	glTexCoord2f(0, 1);
	glVertex2f(w / 2, h / 2);
	glTexCoord2f(0, 0);
	glVertex2f(w / 2, -h / 2);
	glTexCoord2f(1, 0);
	glVertex2f(-w / 2, -h / 2);
	glEnd();	

	// Texture Back
	glBindTexture(GL_TEXTURE_2D, imageTex);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(-w / 2, h / 2);
	glTexCoord2f(1, 0);
	glVertex2f(-w / 2, -h / 2);
	glTexCoord2f(0, 0);
	glVertex2f(w / 2, -h / 2);
	glTexCoord2f(0, 1);
	glVertex2f(w / 2, h / 2);
	glEnd();	

	// Liberamos la Textura de memoria
	glDeleteTextures(1, &imageTex);
	glUseProgram(0);
}

GLFWwindow* initGL()
{
	// Definimos nuestra configuración del Buffer
	int redBits = 8, greenBits = 8, blueBits = 8;
	int alphaBits = 8, depthBits = 24, stencilBits = 8;

	// Inicializamos GLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Creamos una Ventana
	GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "CG:Image Processing - OpenCV_OpenGL_GLSL", NULL, NULL);

	if (window == NULL)
	{
		char *error = "Falló al crear la ventana!";
		exit_with_errors(error);
		glfwTerminate();
		exit(1);
	}

	// OpenGL context actual a la ventana creada
	glfwMakeContextCurrent(window);

	// Función de llamada de eventos del teclado
	glfwSetKeyCallback(window, key_callback);

	//  Inicializamos GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		char *error = "Falló al inicializar GLEW.";
		exit_with_errors(error);
		exit(1);
	}
	cout << "GLEW okay - usando la versión: " << glewGetString(GLEW_VERSION) << endl;

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		cout << "Listo para GLSL" << endl;
	else {
		char *error = "No hay soporte para GLSL";
		exit_with_errors(error);
		exit(1);
	}

	// Viewport igual al todo el tamaño de la ventana
	glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);

	// Cambios en proyeccion
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspectRatio = (windowWidth > windowHeight) ? float(windowWidth) / float(windowHeight) : float(windowHeight) / float(windowWidth);
	GLfloat fH = tan(float(fieldOfView / 360.0f * 3.14159f)) * zNear;
	GLfloat fW = fH * aspectRatio;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);

	// ----- OpenGL settings -----

	glDepthFunc(GL_LEQUAL);	
	glEnable(GL_DEPTH_TEST);    
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);     
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glfwSwapInterval(1);        // Lock para refresh vertical

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Clear Color blanco

	return window;
}

void lockFramerate(long framerate)
{
	static double allowedFrameTime = 1.0 / framerate * 1000;

	// Obtenemos el tiempo actual
	frameEndTime = glfwGetTime() * 1000;	

	// Calculamos el tiempo de dibujado de un frame
	frameDrawTime = frameEndTime - (frameStartTime * 1000);

	long sleepTime = 0.0;

	// El hilo duerme hasta el siguiente frame a procesar
	if (frameDrawTime < allowedFrameTime)
	{
		sleepTime = allowedFrameTime - frameDrawTime;		
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}
}

void initShaders()
{
	GreyScaleProg = setShaders((char *) "shaders/Reg.vert", (char *) "shaders/GrayScale.frag");
	RedBlueProg = setShaders((char *) "shaders/Reg.vert", (char *) "shaders/RedBlueSwap.frag");
	FilterProg = setShaders((char *) "shaders/Reg.vert", (char *) "shaders/Filter.frag");
	ShaderProg = GreyScaleProg;
}

cv::Mat selectImage()
{
	if (frameStartTime - startTime > 5) // Cambiamos de imagen cada 5 segundos
	{
		idxCurrentImage++;
		if (idxCurrentImage >= images.size())
			idxCurrentImage = 0;
		startTime = glfwGetTime();
	}

	return images[idxCurrentImage];
}

int main(int argc, char *argv[])
{
	locale::global(locale("spanish"));

	cout << "Escriba el directorio donde se localizan las imágenes (i.e. c:\\imagenes): " << endl;

	char* path_images = new char[500];
	cin >> path_images;

	// No se va a redimensionar las imágenes
	double downScale_factor = 1.0;

	open_imgs_dir(path_images, images, images_names, downScale_factor);
	if (images.size() == 0)
	{
		char* error = "No se puede abrir los archivos.";
		return exit_with_errors(error);
	}
		
	GLFWwindow *window = initGL();

	// Cargamos los shaders
	initShaders();
	initKernels();

	startTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		/*int width, height;
		glfwGetFramebufferSize(window, &width, &height);*/

		frameStartTime = glfwGetTime(); // Grab the time at the beginning of the frame

		cv::Mat imagen = selectImage();

		draw(imagen);

		glfwSwapBuffers(window);
		glfwPollEvents();

		frameCount++;

		// Lock our main loop to 60fps
		lockFramerate(60);		
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}