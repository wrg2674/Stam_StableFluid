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

	double dt;
	void addForce(VectorCell& nextVc, glm::vec3 force);
	void transport(VectorCell& nextVc, int i, int j, int k, double dt);
	void diffuse(VectorGrid& vf, ScalarGrid& sf, double viscosity);
	void project(VectorCell& nextVc);
public:
	Solver(double dt);
	void updateVectorField(glm::vec3 force);
	void updateScalarField(double source);


};


#endif