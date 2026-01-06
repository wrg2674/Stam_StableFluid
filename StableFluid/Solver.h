#pragma once

#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "VectorCell.h"
#include "ScalarCell.h"
#include "VectorGrid.h"
#include "ScalarGrid.h"
#include "Utill.h"

using namespace std;

class Solver {
private:
	VectorGrid prevVelocityField;
	VectorGrid nextVelocityField;
	ScalarGrid prevRhoField;
	ScalarGrid nextRhoField;
	ScalarGrid prevPressureField;
	ScalarGrid nextPressureField;

	VectorGrid jcbVelocityField;
	ScalarGrid jcbRhoField;
	ScalarGrid jcbPressureField;

	double dt;

	void addForce(glm::vec3 force);
	void addForce(int i, int j, int k, glm::vec3 force);
	void addSource(double source);
	void addSource(int i, int j, int k, double source);
	void transportVector();
	void transportScalar();
	void diffuseVectorField(double viscosity);
	void diffuseScalarField(double diffusion);
	void project();
	glm::vec3 tracePrevPos(int i, int j, int k);
public:
	Solver(double dt);
	void solveVectorField(glm::vec3 force, double viscosity);
	void solveScalarField(double source, double diffusion);
	void injection(int i, int j, int k, int radius, double densityPerSec, glm::vec3 forcePerSec);
	glm::vec3 getGridNumber();
	glm::vec3 getCellPos(int i, int j, int k);
	glm::vec3 getCellSize();
	glm::vec3 getCenter();
	double getRho(int i, int j, int k);

};

#endif