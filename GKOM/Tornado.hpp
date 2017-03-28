#pragma once

#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

//simple, 'one-level' tornado
class SimpleTornado {
public:
	//number of leafs in the tornado
	unsigned leafsN;
	//rozbie¿noœæ tornada
	GLfloat radius;
	//szybkosc narastania w gore tornada
	GLfloat height;
	//kat obrotu listka tornada
	GLfloat angle;
	//szybkosc obracania tornada
	GLfloat speed;
	//tornado shader
	Shader tornShader;
	//leaf texture
	Texture leafText;
	//leafs vector
	std::vector<Polygon> leafs;
	//leaf geometry
	static GLfloat leafVs[54];
	//noise level
	GLfloat noiseLvl;

	//constructor
	SimpleTornado(unsigned leafsN_, GLfloat radius_, GLfloat height_, GLfloat angle_, GLuint speed_, Texture& leafText_, Shader tornShader_, GLfloat noiseLvl_) : 
		leafsN(leafsN_),
		radius(radius_),
		height(height_),
		angle(angle_),
		speed(speed_),
		leafText(leafText_),
		tornShader(tornShader_),
		noiseLvl(noiseLvl_) {
	}

	//init tornado - generate and send to GPU
	void init() {
		//tornado is partially random
		std::normal_distribution<GLfloat> gaussAngle(0.0, angle * noiseLvl);
		std::normal_distribution<GLfloat> gaussRadius(0.0, radius * noiseLvl);
		std::normal_distribution<GLfloat> gaussHeight(0.0, height * noiseLvl);

		for (unsigned i = 0; i < leafsN; ++i) {
			GLfloat randNoiseAngle = gaussAngle(std::random_device());
			GLfloat randNoiseRadius = gaussRadius(std::random_device());
			GLfloat randNoiseHeight = gaussHeight(std::random_device());
			//generate leaf
			leafs.push_back(Polygon(leafVs, 6, leafText, 0.0f, tornShader));
			leafs[i].init();
			//place leaf
			leafs[i].translate(glm::vec3(0, (height + randNoiseHeight) * i, 0)); //move leafs up
			leafs[i].translate(glm::vec3((radius + randNoiseRadius) * i, 0, 0));
			leafs[i].rotate((angle + randNoiseAngle) * i, glm::vec3(0, 1, 0));    //rotate leafs with increasing radius
			leafs[i].translate(glm::vec3(-radius * i, 0, 0));
		}
	}

	//rotate tornado
	void rotate(GLfloat deltaTime) {
		std::normal_distribution<GLfloat> gaussAngle(0.0, 1.1 * angle * noiseLvl);
		/*GLfloat angle = gaussAngle(std::random_device());
		GLfloat AxisX = gaussAngle(std::random_device());
		GLfloat AxisY = gaussAngle(std::random_device());
		GLfloat AxisZ = gaussAngle(std::random_device());*/
		for (unsigned i = 0; i < leafsN; ++i) {
			/*angle += gaussAngle(std::random_device());
			AxisX += gaussAngle(std::random_device());
			AxisY += gaussAngle(std::random_device());
			AxisZ += gaussAngle(std::random_device());
			glm::vec3 Axis = glm::vec3(AxisX, AxisY, AxisZ);
			*/
			

			leafs[i].translate(glm::vec3(radius * i, 0, 0));
			leafs[i].rotate(speed * deltaTime, glm::vec3(0, 1, 0));
			leafs[i].translate(glm::vec3(-radius * i, 0, 0));

			//leafs[i].rotate(angle, Axis);
			
		}
	}

	//draw tornado
	void draw() {
		for (unsigned i = 0; i < leafsN; ++i)
			leafs[i].draw();
	}
};

//tornado lead geometry
GLfloat SimpleTornado::leafVs[] = {
	0.5f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.75f, 0.0f,
	0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.75f, 1.0f,
   -0.5f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.25f, 0.0f,
   -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.25f, 1.0f,
	0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.75f, 1.0f,
   -0.5f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.25f, 0.0f
};

//big tornado, containning many nested simple tornados
class Tornado {
public:
	/*
		PARAMETERS of each simple tornado in the big tornado
	*/
	//number of leafs in the tornado
	unsigned leafsN;
	//rozbie¿noœæ tornada
	GLfloat radius;
	//szybkosc narastania w gore tornada
	GLfloat height;
	//kat obrotu listka tornada
	GLuint angle;
	//szybkosc obracania tornada
	GLint speed;
	//tornado shader
	Shader tornShader;
	//leaf texture
	Texture leafText;
	//leafs vector
	std::vector<Polygon> leafs;
	//leaf geometry
	static GLfloat leafVs[54];
	//noise level
	GLfloat noiseLvl;

	//VECTOR OF SIMPLE TORNADOS
	std::vector<SimpleTornado> tornados;

	//constructor
	//density - number of simple tornados in the big tornado
	Tornado(unsigned leafsN_, GLfloat radius_, GLfloat height_, GLfloat angle_, GLuint speed_, Texture& leafText_, Shader tornShader_, GLfloat noiseLvl_, unsigned density) :
		leafsN(leafsN_),
		radius(radius_),
		height(height_),
		angle(angle_),
		speed(speed_),
		leafText(leafText_),
		tornShader(tornShader_),
		noiseLvl(noiseLvl_) {
		GLfloat rStep = radius / density;
		for (unsigned i = 0; i < density; ++i) {
			tornados.push_back(SimpleTornado(leafsN, ((radius / 2) + (rStep * i)), height, angle, speed, leafText, tornShader, noiseLvl));
			tornados[i].init();
		}
	}

	//set rotating speed
	void setSpeed(GLuint speed_) {
		speed = speed_;
		for (unsigned i = 0; i < tornados.size(); ++i)
			tornados[i].speed = speed_;
	}

	//get rotating speed
	GLuint getSpeed() {
		return speed;
	}

	//rotate tornado
	void rotate(GLfloat deltaTime) {
		for (unsigned i = 0; i < tornados.size(); ++i)
			tornados[i].rotate(deltaTime);
	}

	//draw tornado
	void draw() {
		for (unsigned i = 0; i < tornados.size(); ++i)
			tornados[i].draw();
	}
};