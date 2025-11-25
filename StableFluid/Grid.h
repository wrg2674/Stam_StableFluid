#pragma once
#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


using namespace std;

class Grid {
protected:
	glm::vec3 origin;
	glm::vec3 length;
	glm::vec3 number;
	glm::vec3 cellSize;
public:
	Grid(glm::vec3 origin, glm::vec3 length, glm::vec3 number);
	void initCell();
	virtual void createCell(int x, int y, int z, glm::vec3 cellPos, glm::vec3 cellSize, bool isBoundary = false);
	glm::vec3 getNumber();
	glm::vec3 getLength();
};
#endif