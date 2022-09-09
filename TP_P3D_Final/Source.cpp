#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")


#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define _D_STORAGE

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include "LoadShader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm\glm/glm.hpp>
#include <glm\glm/gtc\type_ptr.hpp>
#include <glm\glm/gtc\matrix_transform.hpp>
#include <glm\glm/gtc\matrix_inverse.hpp> // glm::inverseTranspose() 
#include "FileLoader.h"

void printGLInfo(void);
void printError(int error, const char* description); //error
void cameraInput(GLFWwindow* window); //inputs para a camara
void Draw(glm::vec3 position, glm::vec3 orientation); //Usado para posicionar o modelo no mundo
void Send(void); //Gero os VAO e VBO 
void display(); // função para o ecra
void mouseCallback(GLFWwindow* window, double xpos, double ypos); //Movimento do rato
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset); //Zoom com o scroll do rato
void lightInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void loadTextures(vector<string> textureFiles); //Carregar as texturas

//Matrizes que formam o MVP
glm::mat4 Model, View, Projection;
glm::mat3 NormalMatrix;

//Camara
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

//Luzes
bool Ambient = true;
bool Direcional = true;
bool Pontual = true;
bool Conico = true;
float effect = 0;

//Tamanho da janela
#define WIDTH 800
#define HEIGHT 600

//Tamanho do ecra de vizualição
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

//timing
float deltaTime = 0.0f; // tempo entre frames
float lastFrame = 0.0f;

//Estado para o rato
bool firstMouse = true;

//Zoom
GLfloat ZOOM = 10.0f;

//Modelo com informaçao do ficheiro obj
//Carregar atributos 
Read objFile("Iron_Man.obj", "Iron_Man.mtl");

#define NumberVAOs 1
#define NumberBuffers 3 // Buffer para vertex, textura e normais
GLuint VAOs[NumberVAOs];
GLuint VAO;
GLuint Buffers[NumberBuffers];
GLuint Buffer;
const GLuint nVertices = objFile.atributesOBJ.countFaces; // Buscar o número de vertices

GLuint program;

void printGLInfo(void) {
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	cout << "\nOpenGL version " << major << '.' << minor << endl;

	const GLubyte* glversion = glGetString(GL_VERSION);
	const GLubyte* glvendor = glGetString(GL_VENDOR);
	const GLubyte* glrenderer = glGetString(GL_RENDERER);
	cout << "\nVersion:  " << glversion << endl <<
		"Vendor:   " << glvendor << endl <<
		"Renderer: " << glrenderer << endl;

	cout << "\nSupported GLSL versions:\n";
	const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout << "Higher supported version:\n\t" << glslversion << endl;
	GLint numglslversions;
	cout << "Other supported versions:\n";
	glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &numglslversions);
	for (int n = 0; n < numglslversions; n++) {
		cout << '\t' << glGetStringi(GL_SHADING_LANGUAGE_VERSION, n) << endl;
	}
}

void printError(int error, const char* description) {
	cout << description << endl;
}

int main(void) {

	GLFWwindow* screen;

	glfwSetErrorCallback(printError);

	if (!glfwInit()) return -1;

	screen = glfwCreateWindow(WIDTH, HEIGHT, "P3D - Iron Mans ", NULL, NULL);
	if (screen == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(screen);

	// Inicia o gestor GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printGLInfo();

	//textura
	vector<string> textureFile{
		"Iron_Man_D.tga"
	};
	loadTextures(textureFile);


	// Um uniform especifica que a variável é transmitida aos shaders
	// pela aplicação, e é constante para o desenho de uma primitiva.
	// Apenas permite acesso de leitura. -> Matrizes

	// send Sampler from texture
	// Indicação da Unidade de Textura a ligar ao sampler TexSampler.
	GLint locationTexture = glGetProgramResourceLocation(program, GL_UNIFORM, "TexSampler");
	glProgramUniform1i(program, locationTexture, 0); // Unidade de Textura #0

	//Camara
	//tempo mandado como uniform
	float time = float(glfwGetTime());
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "time"), time);

	glfwSetKeyCallback(screen, lightInput);
	glfwSetCursorPosCallback(screen, mouseCallback);
	glfwSetScrollCallback(screen, scrollCallback);

	//GLFW regista o rato
	glfwSetInputMode(screen, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Camara
	while (!glfwWindowShouldClose(screen)) {

		//Camara
		//tempo entre frame
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Recebe o Input pra janela
		cameraInput(screen);

		//glclear()
		//Limpa o buffer de cor
		//O glClearBufferfv() é tipo glClearColor().
		//glClearBufferfv(GL_COLOR, 0, (strcut color_name));
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Draw(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		Draw(glm::vec3(-2.0f, 1.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		Draw(glm::vec3(2.0f, 1.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		//Camara
		//display();


		glfwSwapBuffers(screen); //execução de todos os comandos de render da frame
		glfwPollEvents(); //Procesar apenas eventos que já foram recebidos
	}

	glfwTerminate();
	return 0;
}

//Camara
//Responsavel pelo input da camera
void cameraInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraFront * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraFront * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

//Usado para posicionar o objecto no mundo
void Draw(glm::vec3 position, glm::vec3 orientation)
{
	Model = glm::translate(glm::mat4(1.0f), position);

	Send();
	display();
}

//VAO e VBO
void Send(void) {

	// VAOs - Vertex Array Objects
	// Um VAO guarda o formado dos dados dos atributos armazenados nos BOs que lhe sejam associados, 
	// mantendo um apontador para o primeiro elemento de cada um desses atributos.

	// Gerar nomes para VAOs.
	glGenVertexArrays(NumberVAOs, &VAOs[0]);
	// Faz bind do VAO, onde o nome está definido em "VAO" e é criado no primero bind que lhe seja feito.
	// Este VAO passa a estar ativo até que seja feito o bind a outro VAO, o bind tem com valor 0.
	glBindVertexArray(VAOs[0]);

	// -------------------------------------
	// VBOs - Vertex Buffer Objects

	// Gera "NumBuffers" nomes para VBOs, neste caso gera 1 nome
	// Esta função pode ser chamada antes de criar os VAOs.
	glGenBuffers(NumberBuffers, &Buffers[0]);

	for (int i = 0; i < NumberBuffers; i++) {

		/*	Faz bind do VBO ao buffer GL_ARRAY_BUFFER.
			GL_ARRAY_BUFFER é usado para dados de atributos de vértices.
			Um VBO é criado no primeiro bind que lhe seja feito.
			Este VBO passa a ativo até ser feito o bind a outro VBO ou seja feito o bind com valor 0.
			Para alocar a memória para os BO, não é necessário que já esteja vinculado um VAO.*/
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[i]);
		if (i == 0) {

			// Inicia o VBO (que está ativo) com memória de tamanho que nao pode ser mudado
			glBufferStorage(GL_ARRAY_BUFFER, nVertices * sizeof(glm::vec3), (GLfloat*)glm::value_ptr(objFile.atributesOBJ.vboV[0]), 0);
		}
		else if (i == 1)
		{
			// Inicia o VBO (que está ativo) com memória de tamanho que nao pode ser mudado
			glBufferStorage(GL_ARRAY_BUFFER, nVertices * sizeof(glm::vec3), (GLfloat*)glm::value_ptr(objFile.atributesOBJ.vboVN[0]), 0);
		}
		else if (i == 2)
		{
			// Inicia o VBO (que está ativo) com memória de tamanho que nao pode ser mudado
			glBufferStorage(GL_ARRAY_BUFFER, nVertices * sizeof(glm::vec2), (GLfloat*)glm::value_ptr(objFile.atributesOBJ.vboVT[0]), 0);
		}
	}
	// ---------------------------------------------------
	// Shaders

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER,   "shaders/light.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/light.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	if (!program) exit(EXIT_FAILURE);
	glUseProgram(program);

	// ----------------------------------------------------
	// Ligar Atributos aos Shaders
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

	// Obtém a localização do atributo "vPosition" no "programa".
	GLint _coords = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vPosition");

	// Obtém a localização do atributo "vNormal" no "programa".
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	GLint _normal = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vNormal");

	//UV
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	GLint _textura = glGetProgramResourceLocation(program, GL_PROGRAM_INPUT, "vTextureCoords");


	// Ativa o VBO "Buffer".
	// Os dados são então armazenados em buffer objects (BO). 
	//	No caso específico de armazenamento dedados de atributos de vértice (posição dos vértices, cor, normais, coordenadas de textura). 

	//VBO V
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	// Liga a localização do atributo "vPosition" dos shaders do "programa", ao VBO e VAO (ativos).
	// Especifica também como é que a informação do atributo "_coords" deve ser interpretada.
	// Neste caso, o atributo vai receber, por vértice, 3 elementos do tipo float. Stride de 6 floats e offset de 0 bytes.
	glVertexAttribPointer(_coords, 3 /*3 elementos por vertex*/, GL_FLOAT/*float*/, GL_FALSE, 3 /*(3 + 3)*/ * sizeof(float) /*stride*/, (void*)0 /*offset*/);

	//VNO VN
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glVertexAttribPointer(_normal, 3 /*3 elementos por vértice*/, GL_FLOAT/*float*/, GL_TRUE, 3 * sizeof(float) /*stride*/, (void*)0 /*offset*/);

	//VBO VT
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glVertexAttribPointer(_textura, 2 /*3 elementos por vértice*/, GL_FLOAT/*float*/, GL_TRUE, 2 * sizeof(float) /*stride*/, (void*)0/*offset*/);


	// Enable o atributo com localização "_coords" para o VAO ativo.
	glEnableVertexAttribArray(_coords);

	// Enable o atributo com localização "_normal" para o VAO ativo.
	glEnableVertexAttribArray(_normal);

	//UV VBO VT
	glEnableVertexAttribArray(_textura);


	// ----------------------------------------------------
	// Matrizes de transformação

	//camara
	Projection = glm::perspective(glm::radians(fov), float(WIDTH) / float(HEIGHT), 0.1f, 100.f);

	glm::mat4 ModelViewProjection = Projection * View * Model;
	glm::mat4 ModelView = View * Model;

	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));

	//Camara
	// ----------------------------------------------------
	// Uniforms
	// Valores que permanecem inalterados durante a renderização de uma primitiva -> apenas de leitura
	// 
	// valor para uniform Model (id)
	GLint _model = glGetProgramResourceLocation(program, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(program, _model, 1, GL_FALSE, glm::value_ptr(Model));

	// valor para uniform View
	GLint _view = glGetProgramResourceLocation(program, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(program, _view, 1, GL_FALSE, glm::value_ptr(View));

	// valor para uniform ModelView
	GLint _modelView = glGetProgramResourceLocation(program, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(program, _modelView, 1, GL_FALSE, glm::value_ptr(ModelView));

	// valor para uniform Projection
	GLint _projection = glGetProgramResourceLocation(program, GL_UNIFORM, "Projection");
	glProgramUniformMatrix4fv(program, _projection, 1, GL_FALSE, glm::value_ptr(Projection));

	// valor para uniform NormalMatrix
	GLint _normalView = glGetProgramResourceLocation(program, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(program, _normalView, 1, GL_FALSE, glm::value_ptr(NormalMatrix));


	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "material.emissive"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(objFile.atributesLights.Ka));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(objFile.atributesLights.kd));
	glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(objFile.atributesLights.ks));
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "material.shininess"), objFile.atributesLights.Ns);

	// ----------------------------------------------------
	// Definir a janela de visualização/viewport

	glViewport(0, 0, WIDTH, HEIGHT);

	// ----------------------------------------------------
	// Outros parâmetros do OpenGL

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Por defeito está desativado
	glCullFace(GL_BACK);
}

void display() {

	//codigo do glclear()
	//Limpa o buffer de cor
	//O glClearBufferfv() é semelhante à função glClearColor().
	//glClearBufferfv(GL_COLOR, 0, black);

	//Camara
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	Projection = glm::perspective(glm::radians(fov), float(WIDTH) / float(HEIGHT), 0.1f, 100.f);

	View = glm::lookAt(
		glm::vec3(cameraPos),
		glm::vec3(cameraPos + cameraFront),	// Direção onde a câmara esta apontada
		glm::vec3(cameraUp)		// Vector vertical
	);

	glm::mat4 ModelViewProjection = Projection * View * Model;
	glm::mat4 ModelView = View * Model;

	//sent time to .vert
	float time = float(glfwGetTime());
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "time"), time);

	GLint _mvp = glGetProgramResourceLocation(program, GL_UNIFORM, "MVP");
	glProgramUniformMatrix4fv(program, _mvp, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));


	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));

	//Camara
	// valor para uniform do Modelo
	GLint _model = glGetProgramResourceLocation(program, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(program, _model, 1, GL_FALSE, glm::value_ptr(Model));

	// valor para uniform do View
	GLint _view = glGetProgramResourceLocation(program, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(program, _view, 1, GL_FALSE, glm::value_ptr(View));

	// valor para uniform do ModeloView
	GLint _modelView = glGetProgramResourceLocation(program, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(program, _modelView, 1, GL_FALSE, glm::value_ptr(ModelView));

	// Atribui valor ao uniform NormalMatrix
	GLint _normalView = glGetProgramResourceLocation(program, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(program, _normalView, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

	// Luz ambiente global
	if (Ambient) {
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
	}
	else {
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	}

	if (Direcional)
	{
		// Luz direcional
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	}
	else {
		// Luz direcional
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	}

	if (Pontual)
	{
		// Luz pontual #1
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 5.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].constant"), 1.0f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].linear"), 0.06f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].quadratic"), 0.02f);

		// Luz pontual #2
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(-2.0, 2.0, 5.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].constant"), 1.0f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].linear"), 0.06f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].quadratic"), 0.02f);
	}
	else
	{
		// Luz pontual #1
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].constant"), 0.1f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].linear"), 0.1f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[0].quadratic"), 0.01f);

		// Luz pontual #2
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].constant"), 0.1f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].linear"), 0.1f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "pointLight[1].quadratic"), 0.01f);
	}

	if (Conico)
	{
		// Luz Conica 
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.constant"), 1.0f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.linear"), 0.06f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.quadratic"), 0.02f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.spotCutoff"), glm::cos(glm::radians(12.5f)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.spotExponent"), glm::cos(glm::radians(20.0f)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.spotDirection"), 1, glm::value_ptr(cameraFront));
	}
	else {
		
		// Luz Conica 
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.constant"), 1.0f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.linear"), 0.06f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.quadratic"), 0.02f);
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.spotCutoff"), glm::cos(glm::radians(12.5f)));
		glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.spotExponent"), glm::cos(glm::radians(20.0f)));
		glProgramUniform3fv(program, glGetProgramResourceLocation(program, GL_UNIFORM, "spotLight.spotDirection"), 1, glm::value_ptr(cameraFront));
	}

	//ativar e desativar o efeito no modelo
	glProgramUniform1f(program, glGetProgramResourceLocation(program, GL_UNIFORM, "effectModel"), effect);

	// Vincula (torna ativo) o VAO
	glBindVertexArray(VAOs[0]);

	// Envia comando para desenho de primitivas GL_TRIANGLES, que utilizará os dados do VAO vinculado.
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
}

//sempre que se mexer o rato esta funcao é chamada
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	//Não deixa inverter o ecra
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

//Callback para o scroll
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;

	if (yoffset == 1)
		ZOOM += fabs(ZOOM) * 0.5f;
	else if (yoffset == -1)
		ZOOM -= fabs(ZOOM) * 0.5f;

}

//Responsavel pelo input das teclas
void lightInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Luz Ambiente
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		Ambient = !Ambient;

	//Luz Direcional
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		Direcional = !Direcional;

	//Luz Pontual
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		Pontual = !Pontual;

	//Luz Conico
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		Conico = !Conico;

	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		if (effect != 0)
		{
			//Disable
			effect = 0;
		}
		else
		{
			//Enable
			effect = 1;
		}
	}
}


void loadTextures(vector<string> textureFiles) {
	GLuint textureName = 0;

	// Gera um nome para textura
	glGenTextures(1, &textureName);

	// Ativa a unidade de textura #0
	// unidade de textura 0 já está ativa por defeito.
	// Só uma unidade de textura está ativa a cada momento.
	glActiveTexture(GL_TEXTURE0);

	// aplica esse nome de textura ao target GL_TEXTURE_2D da unidade de textura ativa.
	glBindTexture(GL_TEXTURE_2D, textureName);

	// Define os parâmetros de wrapping e ajuste de tamanho da textura
	// para a textura que está aplicada ao target GL_TEXTURE_2D da unidade de textura ativa.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Leitura do ficheiro com imagem de textura
	int width, height, nChannels;

	// Ativa a inversão vertical da imagem, quando da sua leitura para memória.
	// isto porque a textura possui coordenadas com origem diferente o que implica inverter o mapeamento
	stbi_set_flip_vertically_on_load(true);

	// Leitura da imagem para memória do CPU
	unsigned char* imageData = stbi_load(objFile.atributesLights.string.c_str(), &width, &height, &nChannels, 0);
	if (imageData) {

		// Carrega os dados da imagem para o Objeto de Textura vinculado com target GL_TEXTURE_2D da unidade de textura ativa.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// Gera o Mipmap para essa textura que possui varias imagens com varios tamanhos 
		glGenerateMipmap(GL_TEXTURE_2D);

		// Liberta a imagem da memória do CPU
		stbi_image_free(imageData);
	}
	else {

		//Erro a carregar a textura
		cout << "Error loading texture!" << endl;
	}
}