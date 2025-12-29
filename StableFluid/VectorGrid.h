#pragma once
#ifndef VECTORGRID_H
#define VECTORGRID_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Grid.h"
#include "VectorCell.h"

using namespace std;

class VectorGrid : public Grid<VectorCell>{
private:

public:
	VectorGrid(glm::vec3 origin, glm::vec3 length, glm::vec3 number);
};
#endif