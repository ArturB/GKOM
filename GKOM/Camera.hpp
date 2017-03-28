#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//possible movement of camera
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//default Euler angles
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
//default movement speed of camera
const GLfloat SPEED = 10.0f;
//default mouse sensitivity (rotating speed)
const GLfloat SENSITIVTY = 0.25f;

//Camera with perspective
class Camera {
public:
	//screen size
	GLuint screenWidth, screenHeight;
	//camera position
	glm::vec3 position;
	//look at 'front' point
	glm::vec3 front;
	//up and right directions vector
	glm::vec3 up, right;
	glm::vec3 worldUp;
	//Euler angles
	GLfloat yaw, pitch;
	//movement and rotating speed
	GLfloat movementSpeed, mouseSensitivity;

	//constructor
	Camera(glm::vec3 position_, int screenWidht_, int screenHeight_, glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw_ = YAW, GLfloat pitch_ = PITCH) : 
		   screenWidth(screenWidht_),
		   screenHeight(screenHeight_),
		   front(glm::vec3(0.0f, 0.0f, -1.0f)), 
		   movementSpeed(SPEED), 
		   mouseSensitivity(SENSITIVTY) {
		position = position_;
		worldUp = up_;
		yaw = yaw_;
		pitch = pitch_;
		updateCameraVectors();
	}

	//return the view matrix
	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	//move camera
	void move(Camera_Movement direction, GLfloat deltaTime) {
		GLfloat step = movementSpeed * deltaTime;
		if (direction == FORWARD)
			position += front * step;
		if (direction == BACKWARD)
			position -= front * step;
		if (direction == LEFT)
			position -= right * step;
		if (direction == RIGHT)
			position += right * step;
	}

	//rotate camera
	void rotate(GLfloat xoffset, GLfloat yoffset) {
		yaw += xoffset * mouseSensitivity;
		pitch += yoffset * mouseSensitivity;
		//Euler angles must be in valid range
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		//update camera vectors
		updateCameraVectors();
	}
	
	//send camera to shader
	void passToShader(Shader sh) {
		//generate camera matrices
		glm::mat4 view = getViewMatrix();
		glm::mat4 projection = glm::perspective(1.0f, (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f);
		//send to shader
		GLint viewLoc = glGetUniformLocation(sh.program, "view");
		GLint projLoc = glGetUniformLocation(sh.program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	}

private:
	//re-calculates camera vectors after rotating
	void updateCameraVectors() {
		//new front vector
		glm::vec3 newFront;
		newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newFront.y = sin(glm::radians(pitch));
		newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(newFront);
		//new right and up vectors
		right = glm::normalize(glm::cross(front, worldUp)); 
		up = glm::normalize(glm::cross(right, front));
	}
};
