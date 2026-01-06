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
#include "Cell.h"

using namespace std;

template<class T>
class Grid {
protected:
	glm::vec3 origin;
	glm::vec3 length;
	glm::vec3 number;
	glm::vec3 cellSize;

public:
	vector<vector<vector<T>>> cell;
	Grid(glm::vec3 origin, glm::vec3 length, glm::vec3 number) {
		this->origin = origin;
		this->length = length;
		this->number = number;
		this->cellSize = length / number;
	};
	void initCell() {
		int ix = (int)number.x;
		int iy = (int)number.y;
		int iz = (int)number.z;
		vector<vector<vector<T>>> vx;
		for (int x = 0; x < ix; x++) {
			vector<vector<T>>vy;
			for (int y = 0; y < iy; y++) {
				vector<T> vz;
				for (int z = 0; z < iz; z++) {
					double xPos = x * cellSize.x + 0.5 * cellSize.x;
					double yPos = y * cellSize.y + 0.5 * cellSize.y;
					double zPos = z * cellSize.z + 0.5 * cellSize.z;
					glm::vec3 cellPos = origin + glm::vec3(xPos, yPos, zPos);

					if (x == ix - 1 || y == iy - 1 || z == iz - 1) {
						vz.push_back(T(cellPos, cellSize, true));
					}
					else if (x == 0 || y == 0 || z == 0) {
						vz.push_back(T(cellPos, cellSize, true));
					}
					else {
						vz.push_back(T(cellPos, cellSize, false));
					}
				}
				vy.push_back(vz);
			}
			vx.push_back(vy);
		};
		cell = move(vx);
	};
	glm::vec3 getNumber() {
		return number;
	};
	glm::vec3 getLength() {
		return length;
	};
	glm::vec3 getOrigin() {
		return origin;
	}

};
#endif