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

class ScalarGrid : public Grid {
private:
	
public:
	vector<vector<vector<ScalarCell>>> cell;
	ScalarGrid(vector<vector<vector<ScalarCell>>> cell, glm::vec3 origin, glm::vec3 length, glm::vec3 number);
	void createCell(int x, int y, int z, glm::vec3 cellPos, glm::vec3 cellSize, bool isBoundary=false);
};
#endif
