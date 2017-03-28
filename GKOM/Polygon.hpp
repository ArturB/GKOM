#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL.h>
#include "Shader.hpp"

//object representing OpenGL texture
class Texture {
public:
	//texture low-level id
	GLuint id;

	//constructor
	//textureFile - name of file with texture
	Texture(const char* textureFile) {
		// Load and create a texture 
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		//set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//set texture filtering mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//set texture mipmaps mode
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//load, create texture and generate mipmaps
		int width, height;
		unsigned char* image = SOIL_load_image(textureFile, &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); //unbind texture when done
	}
};

//textured object -polygon
class Polygon {	
public:
	//table of vertices
	GLfloat* vertices;
	//number of vertices
	int numVertices;
	//shader associated with object
	Shader objectShader;
	GLfloat mixCoeff;
	//texture of object
	Texture& texture;
	//vertex buffer object and vertex array object
	GLuint VBO, VAO;
	//transformation matrix
	glm::mat4 transMat;

	//constructor
	Polygon(GLfloat* vertices_, int numVertices_, Texture& texture_, GLfloat mixCoeff_, Shader objectShader_) :
		objectShader(objectShader_),
		numVertices(numVertices_),
		texture(texture_),
		mixCoeff(mixCoeff_) {
		vertices = vertices_;
	}

	//init object - send it to GPU
	void init() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		//bind VBO and VAO and send vertices to GPU
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9 * numVertices, vertices, GL_STATIC_DRAW);
		//Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		//Color attribute
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		//TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		//unbind VAO
		glBindVertexArray(0);
		//unbind VBO
		glBindVertexArray(0);
	}

	//translate object
	void translate(glm::vec3 translateVect) {
		transMat = glm::translate(transMat, translateVect);
	}

	//rotate object
	void rotate(GLfloat angle, glm::vec3 axis) {
		transMat = glm::rotate(transMat, (GLfloat)(2 * 3.14159 * angle / 360), axis);
	}

	//scale object
	void scale(glm::vec3 scaleVect) {
		transMat = glm::scale(transMat, scaleVect);
	}

	//get object position
	glm::vec3 position() {
		return glm::vec3(transMat * glm::vec4(0, 0, 0, 1));
	}

	//draw object
	void draw() {
		glBindVertexArray(VAO);
		//pass data to shader
		GLint mixCoeffLoc = glGetUniformLocation(objectShader.program, "mixCoeff");
		glUniform1f(mixCoeffLoc, mixCoeff);
		GLint modelLoc = glGetUniformLocation(objectShader.program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transMat));
		//bind Textures using texture units
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glUniform1i(glGetUniformLocation(objectShader.program, "ourTexture1"), 0);
		//draw triangles
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		//unbind
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}

	//destructor
	~Polygon() {
		//properly de-allocate all resources
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
};