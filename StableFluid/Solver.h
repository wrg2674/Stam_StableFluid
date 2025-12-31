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

	double dt;
	void addForce(VectorCell& nextVc, glm::vec3 force);
	void addSource(ScalarCell& nextSc, double source);
	void transport(VectorGrid& vf, VectorCell& nextVc, int i, int j, int k, double dt);
	void transport(ScalarGrid& sf, VectorCell& nextVc, ScalarCell& nextSc, int i, int j, int k, double dt);
	void diffuseVectorField(double viscosity);
	void diffuseScalarField(double diffusion);
	void project();
public:
	Solver(double dt);
	void solveVectorField(glm::vec3 force);
	void solveScalarField(double source);
	void prevFieldUpdate();
	void nextFieldUpdate();
	void injection(int i, int j, int k, int radius, double densityPerSec, glm::vec3 forcePerSec);
	glm::vec3 getGridNumber();
};


#endif