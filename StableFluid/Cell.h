#pragma once
#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;

class Cell {
private:
	glm::vec3 pos=glm::vec3(0);
	bool Boundary;
public:
	Cell(glm::vec3 pos, bool Boundary);
	glm::vec3 getPos();
	bool isBoundary();
};

#endif