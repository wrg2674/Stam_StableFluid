#pragma once
#ifndef SCALARGRID_H
#define SCALARGRID_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Grid.h"
#include "ScalarCell.h"

using namespace std;

class ScalarGrid : public Grid<ScalarCell>{
private:

public:
	ScalarGrid(glm::vec3 origin, glm::vec3 length, glm::vec3 number);
};
#endif
