#pragma once

#ifndef UTILL_H
#define UTILL_H

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

glm::vec3 trace(VectorGrid& prevField, VectorCell vc, int i, int j, int k, double dt) {
	glm::vec3 currentPos = glm::vec3(prevField.cell[i][j][k].getPos().x, prevField.cell[i][j][k].getPos().y, prevField.cell[i][j][k].getPos().z);
	glm::vec3 prevPos = glm::vec3(currentPos.x - vc.getPos().x * dt, currentPos.y - vc.getPos().y * dt, currentPos.z - vc.getPos().z * dt);

	return prevPos;
}
glm::vec3 interpolate(VectorGrid& prevField, glm::vec3 prevPos) {
	int nX = int(prevPos.x);
	int nY = int(prevPos.y);
	int nZ = int(prevPos.z);

	if ((nX <= 0 || nX + 1 >= prevField.getNumber().x) ||
		(nY <= 0 || nY + 1 >= prevField.getNumber().y) ||
		(nZ <= 0 || nZ + 1 >= prevField.getNumber().z)) {
		return glm::vec3(0);
	}

	glm::vec3 weight = glm::vec3(prevPos.x - nX, prevPos.y - nY, prevPos.z - nZ);

	glm::vec3 interpolateElement = prevField.cell[nX][nY][nZ].get();
	glm::vec3 interpolateElementXplus = prevField.cell[nX + 1][nY][nZ].get();
	glm::vec3 interpolateElementYplus = prevField.cell[nX][nY + 1][nZ].get();
	glm::vec3 interpolateElementZplus = prevField.cell[nX][nY][nZ + 1].get();
	glm::vec3 interpolateElementXYplus = prevField.cell[nX + 1][nY + 1][nZ].get();
	glm::vec3 interpolateElementYZplus = prevField.cell[nX][nY + 1][nZ + 1].get();
	glm::vec3 interpolateElementXZplus = prevField.cell[nX + 1][nY][nZ + 1].get();
	glm::vec3 interpolateElementXYZplus = prevField.cell[nX + 1][nY + 1][nZ + 1].get();

	double interpolateInX1 = 0;
	double interpolateInX2 = 0;
	double interpolateInX3 = 0;
	double interpolateInX4 = 0;

	double interpolateInY1 = 0;
	double interpolateInY2 = 0;

	double interpolateInZ = 0;

	glm::vec3 result = glm::vec3(0);

	for (int i = 0; i < 3; i++) {

		interpolateInX1 = (1 - weight[0]) * interpolateElement[i] + (weight[0]) * interpolateElementXplus[i];
		interpolateInX2 = (1 - weight[0]) * interpolateElementYplus[i] + (weight[0]) * interpolateElementXYplus[i];
		interpolateInX3 = (1 - weight[0]) * interpolateElementZplus[i] + (weight[0]) * interpolateElementXZplus[i];
		interpolateInX4 = (1 - weight[0]) * interpolateElementYZplus[i] + (weight[0]) * interpolateElementXYZplus[i];

		interpolateInY1 = (1 - weight[1]) * interpolateInX1 + (weight[1]) * interpolateInX2;
		interpolateInY2 = (1 - weight[1]) * interpolateInX3 + (weight[1]) * interpolateInX4;

		interpolateInZ = (1 - weight[2]) * interpolateInY1 + (weight[2]) * interpolateInY2;
		result[i] = interpolateInZ;
	}
	return result;
}
double difference(double prevValue, double nextValue, double cellSize) {
	return (nextValue - prevValue) / (2.0*cellSize);
}

glm::vec3 gradient(ScalarGrid& nextSf, int i, int j, int k) {
	if (isOutOfBound(i, j, k, nextSf)) {
		return glm::vec3(0);
	}
	glm::vec3 result;
	glm::vec3 cellSize = nextSf.cell[i][j][k].getCellSize();

	double xComponent = difference(nextSf.cell[i - 1][j][k].get(), nextSf.cell[i + 1][j][k].get(), cellSize.x);
	double yComponent = difference(nextSf.cell[i][j - 1][k].get(), nextSf.cell[i][j + 1][k].get(), cellSize.y);
	double zComponent = difference(nextSf.cell[i][j][k - 1].get(), nextSf.cell[i][j][k + 1].get(), cellSize.z);

	result = glm::vec3(xComponent, yComponent, zComponent);
	return result;
}
double divergence(VectorGrid& nextVf, int i, int j, int k) {
	if (isOutOfBound(i, j, k, nextVf)) {
		return 0;
	}
	glm::vec3 cellSize = nextVf.cell[i][j][k].getCellSize();
	double x = difference(nextVf.cell[i - 1][j][k].getX(), nextVf.cell[i + 1][j][k].getX(), cellSize.x);
	double y = difference(nextVf.cell[i][j - 1][k].getY(), nextVf.cell[i][j + 1][k].getY(), cellSize.y);
	double z = difference(nextVf.cell[i][j][k - 1].getZ(), nextVf.cell[i][j][k + 1].getZ(), cellSize.z);

	return x + y + z;
}
glm::vec3 curl(VectorGrid& nextVf, int i, int j, int k) {
	if(isOutOfBound(i, j, k, nextVf)) {
		return glm::vec3(0);
	}

	glm::vec3 cellSize = nextVf.cell[i][j][k].getCellSize();

	double x1 = difference(nextVf.cell[i][j - 1][k].getZ(), nextVf.cell[i][j + 1][k].getZ(), cellSize.y);
	double x2 = difference(nextVf.cell[i][j][k - 1].getY(), nextVf.cell[i][j][k + 1].getY(), cellSize.z);
	double xComponent = x1 - x2;

	double y1 = difference(nextVf.cell[i][j][k - 1].getX(), nextVf.cell[i][j][k + 1].getX(), cellSize.z);
	double y2 = difference(nextVf.cell[i - 1][j][k].getZ(), nextVf.cell[i + 1][j][k].getZ(), cellSize.x);
	double yComponent = y1 - y2;

	double z1 = difference(nextVf.cell[i - 1][j][k].getY(), nextVf.cell[i + 1][j][k].getY(), cellSize.x);
	double z2 = difference(nextVf.cell[i][j - 1][k].getX(), nextVf.cell[i][j + 1][k].getX(), cellSize.y);
	double zComponent = z1 - z2;

	glm::vec3 result = glm::vec3(xComponent, yComponent, zComponent);

	return result;
}
glm::vec3 laplaccian(VectorGrid& vf, ScalarGrid& sf, int i, int j, int k) {
	
	return gradient(sf, i, j, k) - curl(vf, i, j, k);
}
bool isOutOfBound(int i, int j, int k, Grid grid){
	if ((i - 1) < 0 || (i + 1) >= grid.getNumber().x) {
		return true;
	}
	if ((j - 1) < 0 || (j + 1) >= grid.getNumber().y) {
		return true;
	}
	if ((k - 1) < 0 || (k + 1) >= grid.getNumber().z) {
		return true;
	}
	return false;
}

#endif