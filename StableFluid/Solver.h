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
	double dt;
	void addForce(VectorCell& prevVc, glm::vec3 force);
	void transport(VectorGrid& vf, VectorCell& prevVc, int i, int j, int k, double dt);
	void diffuse();
	void project(VectorCell vc);
public:
	Solver(double dt);
	void updateVectorField(glm::vec3 force);
	void updateScalarField(double source);


};


#endif