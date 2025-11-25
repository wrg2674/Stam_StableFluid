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

double difference(double next, double prev, double dt) {
	return (next - prev) / dt;
}
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

void decomposition(VectorCell vf) {

}
double divergence(VectorCell prevVc, VectorCell nextVc, double dt) {
	return difference(nextVc.getX(), prevVc.getX(), dt) + difference(nextVc.getY(), prevVc.getY(), dt) + difference(nextVc.getZ(), prevVc.getZ(), dt);
}


#endif