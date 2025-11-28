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

class VectorGrid : public Grid<glm::vec3>{
private:
	
public:
	vector<vector<vector<VectorCell>>> cell;
	VectorGrid(glm::vec3 origin, glm::vec3 length, glm::vec3 number);
	void createCell(int x, int y, int z, glm::vec3 cellPos,glm::vec3 cellSize, glm::vec3 value, bool isBoundary=false) override;
};
#endif