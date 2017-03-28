#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Polygon.hpp"
#include "Tornado.hpp"

//window size
GLuint screenWidth = 1366, screenHeight = 768;
// Camera
Camera camera(glm::vec3(0.0f, 10.0f, 60.0f), screenWidth, screenHeight);
//pressed keys array
bool keys[1024];
//mouse position in last iteration
GLfloat lastX = (GLfloat)screenHeight / 2, lastY = (GLfloat)screenWidth / 2;
//indicate if it is first mouse move
bool firstMouse = true;
//tornado rotation speed regulation
GLint speedReg = 45;
//height and widht of the scene
GLfloat sceneHeight = 1000.0;
GLfloat sceneWidht = 5000.0;

//init GLFW window
GLFWwindow* initWindow(int antiAliasLevel, GLFWkeyfun key_callback, GLFWcursorposfun mouse_callback) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, antiAliasLevel);
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "BrodzkiGKOM", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();
	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);
	//anti alias
	glEnable(GL_MULTISAMPLE);
	return window;
}

//call when key is pressed
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//call when mouse is moved
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)	{
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		firstMouse = false;
	}
	GLfloat xoffset = GLfloat(xpos - lastX);
	GLfloat yoffset = GLfloat(lastY - ypos);  // Reversed since y-coordinates go from bottom to left
	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;
	camera.rotate(xoffset, yoffset);
}

// The MAIN function
int main() {
	// Init GLFW
	GLFWwindow* window = initWindow(8, key_callback, mouse_callback);
	// Setup and compile our shaders
	Shader ourShader("vertex.shd", "fragment.shd");
	//define objects on the scene
	//vertices
	GLfloat grassVs[] = { 1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,1500.0f,  0.0f,
						  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,1500.0f,1500.0f,
						 -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f,   0.0f,
						 -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f,1500.0f,
						  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,1500.0f,1500.0f,
						 -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f,   0.0f,
	};
	GLfloat cloudsVs[] = { 1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
						   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
						  -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
						  -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
						   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
						  -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	};
	//textures
	Texture grassText("grassSeam.png");
	Texture cloudsText("skySeam.png");	
	Texture skyText("skySeam.png"); 
	//create grass
	Polygon grass(grassVs, 6, grassText, 0.2f, ourShader);
	grass.init();
	grass.rotate(90, glm::vec3(1, 0, 0));
	grass.scale(glm::vec3(sceneWidht, sceneWidht, 1.0));
	//create clouds and sky
	Polygon clouds(cloudsVs, 6, cloudsText, 0.2f, ourShader);

	Polygon cloud1(cloudsVs, 6, skyText, 0.2f, ourShader);
	Polygon cloud2(cloudsVs, 6, skyText, 0.2f, ourShader);
	Polygon cloud3(cloudsVs, 6, skyText, 0.2f, ourShader);
	Polygon cloud4(cloudsVs, 6, skyText, 0.2f, ourShader);

	clouds.init();
	clouds.translate(glm::vec3(0, sceneHeight, 0));
	clouds.rotate(90, glm::vec3(1, 0, 0));
	clouds.scale(glm::vec3(sceneWidht, sceneWidht, 1.0));

	cloud1.init();
	cloud1.translate(glm::vec3(0, sceneHeight / 2, 0));
	cloud1.translate(glm::vec3(0, 0, sceneWidht));
	cloud1.scale(glm::vec3(sceneWidht, sceneHeight / 2, 1.0));

	cloud2.init();
	cloud2.translate(glm::vec3(0, sceneHeight / 2, 0));
	cloud2.translate(glm::vec3(0, 0, -1 * sceneWidht));
	cloud2.scale(glm::vec3(sceneWidht, sceneHeight / 2, 1.0));

	cloud3.init();
	cloud3.translate(glm::vec3(0, sceneHeight / 2, 0));
	cloud3.translate(glm::vec3(sceneWidht, 0, 0));
	cloud3.rotate(90, glm::vec3(0, 1, 0));
	cloud3.scale(glm::vec3(sceneWidht, sceneHeight / 2, 1.0));

	cloud4.init();
	cloud4.translate(glm::vec3(0, sceneHeight / 2, 0));
	cloud4.translate(glm::vec3(-1 * sceneWidht, 0, 0));
	cloud4.rotate(90, glm::vec3(0, 1, 0));
	cloud4.scale(glm::vec3(sceneWidht, sceneHeight / 2, 1.0));

	//create tornado
	Tornado tornado(1000, 0.02f, 0.1f, 20, 360, Texture("leaf.png"), ourShader, 0.1f, 4);

	//program loop
	GLfloat lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		//set Z-buffer test and blending
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//set frame time
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		GLfloat deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//check and call events
		glfwPollEvents();
		if (keys[GLFW_KEY_W])
			camera.move(FORWARD, deltaTime);
		if (keys[GLFW_KEY_S])
			camera.move(BACKWARD, deltaTime);
		if (keys[GLFW_KEY_A])
			camera.move(LEFT, deltaTime);
		if (keys[GLFW_KEY_D])
			camera.move(RIGHT, deltaTime);
		if (keys[GLFW_KEY_EQUAL])
			tornado.setSpeed(tornado.getSpeed() + speedReg);
		if (keys[GLFW_KEY_MINUS])
			tornado.setSpeed(tornado.getSpeed() - speedReg);
		//print current camera position
		system("cls");
		std::cout << "Camera: x = " << camera.position.x << ", y = " << camera.position.y << ", z = " << camera.position.z << "\nFPS = " << 1 / deltaTime << std::endl;
		//clear background color
		glClearColor(0.25f, 0.36f, 0.5f, 0.4f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//activate camera
		camera.passToShader(ourShader);
		//activate shader
		ourShader.Use();
		//draw objects
		grass.draw();
		clouds.draw();
		cloud1.draw();
		cloud2.draw();
		cloud3.draw();
		cloud4.draw();
		tornado.rotate(deltaTime);
		tornado.draw();
		//swap buffers
		glfwSwapBuffers(window);
	}
	//terminate GLFW
	glfwTerminate();
	return 0;
}

