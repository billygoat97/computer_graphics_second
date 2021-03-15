// HW2_2015147506 Make 3D GAME

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <shader.h>
#include <cube.h>
#include <arcball.h>
#include <stb_image.h>
#include <text.h>
#include <Model.h>
#include <Windows.h>
#include <mmsystem.h>
#include <mass.h>
#include <plane.h>
#include "keyframe.h"
#include <cstdlib>
#include <glm/gtx/string_cast.hpp>
#pragma comment(lib,"winmm.lib")
#define STB_IMAGE_IMPLEMENTATION

using namespace std;
//initialize funciton
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
//void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(string);
void startgame();
void render();
void start(float currentTime);
void create_BG();
void create_BG2();
void create_E2();
void create_E3();
void create_A2();
int songflag = 0;
float waittime;
///////////////////////
void initKeyframes();
void updateAnimData();
enum RenderMode { INIT, ANIM, STOP };
RenderMode renderMode;                  // current rendering mode
float beginT;                           // animation beginning time (in sec)
float timeT;                            // current time (in sec)
float animEndTime = 5.0f;               // ending time of animation (in sec)
float xTrans, yTrans, zTrans;           // current translation factors
float xAngle, yAngle, zAngle;           // current rotation factors
KeyFraming xTKF(4), yTKF(4), zTKF(4);   // translation keyframes
KeyFraming xRKF(4), yRKF(4), zRKF(4);   // rotation keyframes
glm::vec3 lightSize(0.2f, 0.2f, 0.2f);
glm::vec3 lightPos(-3.2f, 30.0f, 7.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 objectColor(0.8f, 0.8f, 0.f);
float ambientStrenth = 0.2f;
////////////////


float RNG(float min, float max)
{

	return min + float(float(rand() % 120) / 120)*((max - min));
}

// initialize variables
const float PI = acos(-1);
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
Shader *textShader = NULL;
Shader *shader;
Shader *groundShader = NULL;
Shader *backShader = NULL;
Text *text = NULL;
Model *ourModel;
Model *enemy;
Model *fruit;
Model *pack;
static unsigned int background, background2;
unsigned int SCR_WIDTH = 900;
unsigned int SCR_HEIGHT = 900;
unsigned int VBO[3], VAO[3], EBO[3]; //
glm::mat4 projection, view, model, emodel, fmodel, gmodel,pmodel;
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool arcballCamRot = true;
int startflag = 0;
float endTime = 0;
int countfruit = 0;
float ex, ez;
float fx, fz;
glm::vec3 cameraOrigPos(0.0f, -10.0f, 3.0f);
glm::vec3 cameraPos;
glm::vec3 modelPan(0.0f, -3.0f, 0.0f);
glm::vec3 Pan(0.0f, -3.0f, 0.0f);
glm::vec3 enemyPan;
glm::vec3 enemyPan2;
glm::vec3 enemyPan3; 
glm::vec3 fruitPan;
glm::vec3 gcolor(0.1f, 0.2f, 0.3f);
bool intermission = FALSE;
bool death = FALSE;
Plane *ground;
Plane *back;
float groundY = -1.0f;                            // ground's y coordinates
float groundScale = 20.0f;                        // ground's scale (x and z)
void create_G();
void create_A();
void create_E();
glm::vec3 fruitCoord[2];
glm::vec3 enemyCoord[3];
int enemyflag1 = 0, enemyflag2 = 0, enemyflag3 = 0;
int fruitflag[2];
int waitflag = 0;
int main()
{
	mainWindow = glAllInit();
	// model shading
	shader = new Shader("res/shaders/modelLoading.vs", "res/shaders/modelLoading.frag");
	ourModel = new Model((GLchar *)"res/models/pacman/PacMan.fbx");
	enemy = new Model((GLchar *)"res/models/pacman/ghost.fbx");
	fruit = new Model((GLchar *)"res/models/pacman/Apple_Final.fbx");
	groundShader = new Shader("ground.vs", "ground.fs");
	backShader = new Shader("background.vs", "background.fs");
	background = loadTexture("background/bam.jpg");
	background2 = loadTexture("background/pack.png");
	// text arrange
	textShader = new Shader("text.vs", "text.fs");
	text = new Text((char*)"fonts/arial.ttf", textShader, SCR_WIDTH, SCR_HEIGHT);
	projection = glm::perspective(glm::radians(45.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader->use();
	shader->setMat4("projection", projection);
	cameraPos = cameraOrigPos;

	projection = glm::perspective(glm::radians(45.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	groundShader->use();
	groundShader->setMat4("projection", projection);
	ground = new Plane(0.0f, 0.0f, 0.0f, groundScale);
	backShader->use();
	backShader->setMat4("projection", projection);
	back = new Plane(0.0f, 0.0f, 0.0f, groundScale);
	//////////////
	globalShader = new Shader("global.vs", "global.fs");
	globalShader->use();
	pack = new Model((GLchar *)"res/models/pacman/PacMan.fbx");
	globalShader->use();
	globalShader->setVec3("objectColor", objectColor);
	globalShader->setVec3("lightColor", lightColor);
	globalShader->setVec3("lightPos", lightPos);
	globalShader->setFloat("ambientStrenth", ambientStrenth);

	// projection matrix
	projection = glm::perspective(glm::radians(45.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	globalShader->setMat4("projection", projection);

	// initialize animation data
	for (int i = 0; i < 10; i++) {
		initKeyframes();
		timeT = 0.0f;
		updateAnimData();
		renderMode = INIT;
	}



	/////////////
	while (!glfwWindowShouldClose(mainWindow)) {
		srand((unsigned int)time(NULL));
		render();
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}

GLFWwindow *glAllInit()
{
	GLFWwindow *window;

	// glfw: initialize and configure
	if (!glfwInit()) {
		printf("GLFW initialisation failed!");
		glfwTerminate();
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// glfw window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2_2015147506", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// OpenGL states
	//glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glewExperimental = GL_TRUE;
	// Allow modern extension features
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "GLEW initialisation failed!" << endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(-1);
	}
	// Define the viewport dimensions
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Set OpenGL options
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return window;
}

unsigned int loadTexture(string texFileName) {
	unsigned int texture;

	// Create texture ids.
	glGenTextures(1, &texture);

	// All upcomming GL_TEXTURE_2D operations now on "texture" object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture parameters for wrapping.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);   // vertical flip the texture
	unsigned char *image = stbi_load(texFileName.c_str(), &width, &height, &nrChannels, 0);
	if (!image) {
		cout << "texture " << texFileName << " loading error" << endl;
	}
	else cout << "texture " << texFileName << " loaded" << endl;

	GLenum format;
	if (nrChannels == 1) format = GL_RED;
	else if (nrChannels == 3) format = GL_RGB;
	else if (nrChannels == 4) format = GL_RGBA;
	else {
		cout << "loadTexture in main.cpp error ... illegal nrChannels: " << nrChannels << endl;
		exit(-1);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	view = glm::lookAt(cameraPos, glm::vec3(.0f, .0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = view * camArcBall.createRotationMatrix();

	float currentTime = glfwGetTime()-15; // initialize time

	if (startflag == 0) {
		glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		startgame();
	}
	else if (startflag == 1) { // game start
		glClearColor(0.f, 0.00f, 0.0f, 1.0f);
		start(currentTime);
	}
	else if (startflag == 2) { // win the game
		PlaySound(TEXT(".\\sound\\pacman_intermission.wav"), NULL, SND_FILENAME | SND_ASYNC);
		startflag = 4;
	}
	else if (startflag == 3) { // lose the game
		PlaySound(TEXT(".\\sound\\pacman_death.wav"), NULL, SND_FILENAME | SND_ASYNC);
		startflag = 5;
	}
	else if (startflag == 4) { // the good end
		text->RenderText("Congratulation! Your Record is", 300.0f, 550.0f, 0.5f, glm::vec3(1.0, 1.0f, 1.0f));
		text->RenderText(to_string(endTime), 400.0f, 450.0f, 0.5f, glm::vec3(0.0, 1.0f, 1.0f));
	}
	else if (startflag == 5) { // bad end
		text->RenderText("Game Over!", 400.0f, 550.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.0f));
	}
	if (startflag == 1 && songflag == 0) {
		PlaySound(TEXT(".\\sound\\pacman_chomp.wav"), NULL, SND_FILENAME | SND_ASYNC|SND_LOOP);
		songflag = 1;
	}
	glfwSwapBuffers(mainWindow);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	projection = glm::perspective(glm::radians(45.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader->use();
	shader->setMat4("projection", projection);

	groundShader->use();
	groundShader->setMat4("projection", projection);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
		modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
		cameraPos = cameraOrigPos;
		//modelPan[0] = modelPan[1] = modelPan[2] = 0.0f;
	}


	else if (key == GLFW_KEY_LEFT && modelPan[0] > -2.5) {
		modelPan[0] -= 0.2;
	}
	else if (key == GLFW_KEY_RIGHT && modelPan[0] < 2.5) {
		modelPan[0] += 0.2;
	}
	
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {

	if (arcballCamRot)
		camArcBall.mouseButtonCallback(window, button, action, mods);
	else
		modelArcBall.mouseButtonCallback(window, button, action, mods);

}
void cursor_position_callback(GLFWwindow *window, double x, double y) {
	if (arcballCamRot) {
		//camArcBall.cursorCallback(window, x, y);
	}

	else {
		//modelArcBall.cursorCallback(window, x, y);
	}
}
void startgame() {
	if (glfwGetTime() < 1) {
		text->RenderText("Welcome", 75.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 1.5)) {
		text->RenderText("Welcome to ", 75.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 2)) {
		text->RenderText("Welcome to Kim ", 75.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 2.5)) {
		text->RenderText("Welcome to Kim Ki", 75.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 3)) {
		text->RenderText("Welcome to Kim Ki Hyun's", 75.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 3.5)) {
		text->RenderText("Welcome to Kim Ki Hyun's Game!", 75.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 5)) {
		text->RenderText("PAC MAN", 325.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 0.0f));
	}
	else if ((glfwGetTime() < 7.5)) {
		text->RenderText("Look For Fruit To Win The Game", 100.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 10)) {
		text->RenderText("Dodge Ghosts To Survive", 200.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 1.0f));
	}
	else if ((glfwGetTime() < 14)) {
		text->RenderText("READY", 350.f, 400.f, 1.f, glm::vec3(1.0, 1.0f, 0.0f));

	}
	else if ((glfwGetTime() < 15)) {
		text->RenderText("GO!", 400.f, 400.f, 1.f, glm::vec3(0.0, 1.0f, 1.0f));
	}
	create_BG2();
	if ((glfwGetTime() < 10.05&&glfwGetTime() > 10)) {
		PlaySound(TEXT(".\\sound\\pacman_beginning.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	if ((glfwGetTime() > 15)) {
		startflag = 1;
	}
}


void start(float currentTime) {
	string s = to_string(currentTime);
	if (currentTime > 0) {
		text->RenderText(s, 800.0f, 880.0f, 0.5f, glm::vec3(1.0, 0.8f, 0.2f));
		text->RenderText("Collected Fruit: " + to_string(countfruit) + "/5", 0.0f, 880.0f, 0.5f, glm::vec3(1.0, 0.8f, 0.2f));
	}
	if (currentTime > 0) {
		create_BG();
		create_G();
		////////
		glm::mat4 transform;
		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		view = view * camArcBall.createRotationMatrix();
		


		globalShader->use();
		globalShader->setMat4("view", view);
		//////////////////////
		if (renderMode == INIT) {
			float cTime = (float)glfwGetTime(); // current time
			timeT = cTime - beginT;
			updateAnimData();
		}
		if (renderMode == STOP) {
			renderMode = INIT;
			beginT = glfwGetTime();
		}
		globalShader->use();
		globalShader->setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(xTrans, yTrans, zTrans));
		glm::vec3 eulerAngles(glm::radians(xAngle), glm::radians(yAngle), glm::radians(zAngle));
		glm::quat q(eulerAngles);
		glm::mat4 rotMatrix = q.operator glm::mat4x4();
		model = model * rotMatrix;
		globalShader->setMat4("model", model);
		pack->Draw(globalShader);
		///////////////
		shader->use();
		shader->setMat4("view", view);
		// Draw the loaded model
		glm::mat4 model(1.0);
		// Rotate model by arcball and panning
		model = glm::translate(model, modelPan);
		model = model * modelArcBall.createRotationMatrix();
		// It's a bit too big for our scene, so scale it down
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		shader->setMat4("model", model);
		ourModel->Draw(shader);
		ez = 0;
		if (enemyflag1 == 0 && int(currentTime) % 4 == 0) {
			srand((unsigned int)time(NULL));
			ex = RNG(-3.f, 3.f);

			enemyPan = glm::vec3(ex, 40.0f, ez);

			create_E();
			enemyflag1 = 1;
			enemyCoord[0] = enemyPan;
		}
		if (enemyflag2 == 0 && int(currentTime) % 4 == 1) {
			srand((unsigned int)time(NULL));
			ex = RNG(-3.f, 3.f);
			enemyPan2 = glm::vec3(ex, 40.0f, ez);
			create_E2();
			enemyflag2 = 1;
			enemyCoord[1] = enemyPan2;
		}
		if (enemyflag3 == 0 && int(currentTime) % 4 == 2) {
			srand((unsigned int)time(NULL));
			ex = RNG(-3.f, 3.f);
			
			enemyPan3 = glm::vec3(ex, 40.0f, ez);
			create_E3();
			enemyflag3 = 1;
			enemyCoord[2] = enemyPan3;

		}
		if (fruitflag[0] == 0 && int(currentTime) % 4 == 3) {
			srand((unsigned int)time(NULL));
			fx = RNG(-3.f, 3.f);
			fruitPan = glm::vec3(fx, 40.0f, ez);
			create_A();
			fruitCoord[0] = fruitPan;
			fruitflag[0] = 1;

		}

		if (enemyflag1 == 1) {
			enemyCoord[0].y -= 0.4;
			if (rand() % 2 == 0) {
				if (enemyCoord[0].x < 3)
					enemyCoord[0].x += 0.03;
			}
			else {
				if (enemyCoord[0].x > -3)
					enemyCoord[0].x -= 0.03;
			}
			create_E();
			if (enemyCoord[0].y < -5) {
				enemyflag1 = 0;
			}
			if (fabs(enemyCoord[0].y - modelPan.y) < 0.4 && fabs(enemyCoord[0].x - modelPan[0]) < 1) {
				death = TRUE;
			}
		}

		if (enemyflag2 == 1) {
			enemyCoord[1].y -= 0.4;
			if (rand() % 2 == 0) {
				if(enemyCoord[1].x<3)
				enemyCoord[1].x += 0.03;
			}
			else {
				if (enemyCoord[1].x >- 3)
				enemyCoord[1].x -= 0.03;
			}
			create_E2();
			if (enemyCoord[1].y < -5) {
				enemyflag2 = 0;
			}
			if (fabs(enemyCoord[1].y - modelPan.y) < 0.4 && fabs(enemyCoord[1].x - modelPan[0]) < 1) {
				death = TRUE;
			}
		}
		if (enemyflag3 == 1) {
			enemyCoord[2].y -= 0.4;
			if (rand() % 2 == 1) {
				if (enemyCoord[2].x < 3)
					enemyCoord[2].x += 0.03;
			}
			else {
				if (enemyCoord[2].x > -3)
					enemyCoord[2].x -= 0.03;
			}
			create_E3();
			if (enemyCoord[2].y < -5) {
				enemyflag3 = 0;
			}
			if (fabs(enemyCoord[2].y - modelPan.y) < 0.4 && fabs(enemyCoord[2].x - modelPan[0]) < 1) {
				death = TRUE;
			}
		}
		for (int i = 0; i < 1; i++) {
			if (fruitflag[i] == 1) {
				if (fruitCoord[0].x < 3){
					fruitCoord[0].x += 0.03;
				}
				else {
					if (fruitCoord[0].x > -3)
						fruitCoord[0].x -= 0.03;
				}
				fruitCoord[i].y -= 0.4;
				if (i == 0) {
					create_A();
				}
				if (fruitCoord[0].y < -5) {
					fruitflag[i] = 0;
				}
				if (fabs(fruitCoord[i].y - modelPan.y) < 0.4 && fabs(fruitCoord[i].x - modelPan[0]) < 1) {
					intermission = TRUE;
					fruitflag[i] = 0;
				}
			}
		}
	}
	if (intermission == TRUE) {
		PlaySound(TEXT(".\\sound\\pacman_eatfruit.wav"), NULL, SND_ASYNC);
		intermission = FALSE;
		countfruit++;
		waitflag = 1;
		waittime = glfwGetTime();
		//songflag = 0;
	}
	if (waitflag == 1) {
		if (glfwGetTime() - waittime > 1) {
			songflag = 0;
			waitflag = 0;
		}
	}
	if (death == TRUE) {
		startflag = 3;
	}
	if (countfruit >= 5) {
		endTime = currentTime;
		startflag = 2;
	}
}




void create_G() {
	gmodel = glm::mat4(1.0);
	groundShader->use();
	groundShader->setMat4("view", view);
	gmodel = glm::translate(gmodel, glm::vec3(0.0f, 0.0f, groundY));
	gmodel = glm::scale(gmodel, glm::vec3(0.5f, 5.0f, 1.0f));
	//gmodel = glm::rotate(gmodel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	groundShader->setMat4("model", gmodel);
	groundShader->setVec3("gcolor", gcolor);
	ground->draw(groundShader);

}
void create_BG() {
	gmodel = glm::mat4(1.0);
	backShader->use();
	backShader->setMat4("view", view);
	gmodel = glm::translate(gmodel, glm::vec3(0.0f, 50.0f, 0.f));
	gmodel = glm::scale(gmodel, glm::vec3(3.0f, 1.914f, 1.0f));
	gmodel = glm::rotate(gmodel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	backShader->setMat4("model", gmodel);
	glBindTexture(GL_TEXTURE_2D, background);
	//backShader->setVec3("gcolor", 0,0,0);
	back->draw(backShader);

}
void create_BG2() {
	gmodel = glm::mat4(1.0);
	backShader->use();
	backShader->setMat4("view", view);
	gmodel = glm::translate(gmodel, glm::vec3(0.0f, 50.0f, 0));
	gmodel = glm::scale(gmodel, glm::vec3(4.0f, 4.0f, 4.0f));
	gmodel = glm::rotate(gmodel, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	backShader->setMat4("model", gmodel);
	glBindTexture(GL_TEXTURE_2D, background2);
	//backShader->setVec3("gcolor", 0,0,0);
	back->draw(backShader);

}
void create_A() {
	glm::mat4 fmodel(1.0);
	fmodel = glm::translate(fmodel, fruitCoord[0]);
	fmodel = fmodel * modelArcBall.createRotationMatrix();
	//transform = glm::mat4(1.0f);
	//transform = glm::rotate(transform, glm::radians(45.0f)*currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	fmodel = glm::scale(fmodel, glm::vec3(0.5f, 0.5f, 0.5f));
	shader->setMat4("model", fmodel);
	fruit->Draw(shader);
}
void create_A2() {
	glm::mat4 fmodel(1.0);
	fmodel = glm::translate(fmodel, fruitCoord[1]);
	fmodel = fmodel * modelArcBall.createRotationMatrix();
	//transform = glm::mat4(1.0f);
	//transform = glm::rotate(transform, glm::radians(45.0f)*currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	fmodel = glm::scale(fmodel, glm::vec3(0.5f, 0.5f, 0.5f));
	shader->setMat4("model", fmodel);
	fruit->Draw(shader);
}
void create_E() {
	glm::mat4 emodel(1.0);
	// Rotate model by arcball and panning
	emodel = glm::translate(emodel, enemyCoord[0]);
	emodel = emodel * modelArcBall.createRotationMatrix();
	// It's a bit too big for our scene, so scale it down
	emodel = glm::scale(emodel, glm::vec3(0.03f, 0.03f, 0.03f));
	shader->setMat4("model", emodel);
	enemy->Draw(shader);
}
void create_E2() {
	glm::mat4 emodel(1.0);
	// Rotate model by arcball and panning
	emodel = glm::translate(emodel, enemyCoord[1]);
	emodel = emodel * modelArcBall.createRotationMatrix();
	// It's a bit too big for our scene, so scale it down
	emodel = glm::scale(emodel, glm::vec3(0.03f, 0.03f, 0.03f));
	shader->setMat4("model", emodel);
	enemy->Draw(shader);
}
void create_E3() {
	glm::mat4 emodel(1.0);
	// Rotate model by arcball and panning
	emodel = glm::translate(emodel, enemyCoord[2]);
	emodel = emodel * modelArcBall.createRotationMatrix();
	// It's a bit too big for our scene, so scale it down
	emodel = glm::scale(emodel, glm::vec3(0.03f, 0.03f, 0.03f));
	shader->setMat4("model", emodel);
	enemy->Draw(shader);
}

void initKeyframes() {

	// x-translation keyframes
	xTKF.setKey(0, 0, -30.0);
	xTKF.setKey(1, 1.5, -10.0);
	xTKF.setKey(2, 3.0, 10.0);
	xTKF.setKey(3, animEndTime, 30.0);
	xTKF.computeTangents();

	// y-translation keyframes
	yTKF.setKey(0, 0, 40);
	yTKF.setKey(1, 1.5, 40);
	yTKF.setKey(2, 3.0, 40);
	yTKF.setKey(3, animEndTime, 40);
	yTKF.computeTangents();

	// z-translation keyframes
	zTKF.setKey(0, 0, 5);
	zTKF.setKey(1, 1.5, 7);
	zTKF.setKey(2, 3.0,7);
	zTKF.setKey(3, animEndTime, 5);
	zTKF.computeTangents();

	// x-rotation keyframes
	xRKF.setKey(0, 0, 0.0);
	xRKF.setKey(1, 1.5, 0.0);
	xRKF.setKey(2, 3.0, 0.0);
	xRKF.setKey(3, animEndTime, 0.0);
	xRKF.computeTangents();

	// y-rotation keyframes
	yRKF.setKey(0, 0, 0.0);
	yRKF.setKey(1, 1.5, 0.0);
	yRKF.setKey(2, 3.0, 0.0);
	yRKF.setKey(3, animEndTime, 0.0);
	yRKF.computeTangents();

	// z-rotation keyframes
	zRKF.setKey(0, 0, 0.0);
	zRKF.setKey(1, 1.5, -90.0);
	zRKF.setKey(2, 3.0, -90.0);
	zRKF.setKey(3, animEndTime, -90.0);
	zRKF.computeTangents();
}

void updateAnimData() {
	if (timeT > animEndTime) {
		renderMode = STOP;
		timeT = animEndTime;
	}

	xTrans = xTKF.getValCatmullRom(timeT);
	yTrans = yTKF.getValCatmullRom(timeT);
	zTrans = zTKF.getValCatmullRom(timeT);
	xAngle = xRKF.getValCatmullRom(timeT);
	yAngle = yRKF.getValCatmullRom(timeT);
	zAngle = zRKF.getValCatmullRom(timeT);
}