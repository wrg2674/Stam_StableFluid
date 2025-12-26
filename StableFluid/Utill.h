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
	glm::vec3 prevPos = glm::vec3(currentPos.x - vc.get().x * dt, currentPos.y - vc.get().y * dt, currentPos.z - vc.get().z * dt);

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
	glm::vec3 result;
	glm::vec3 cellSize = nextSf.cell[i][j][k].getCellSize();

	double xComponent = difference(nextSf.cell[i - 1][j][k].get(), nextSf.cell[i + 1][j][k].get(), cellSize.x);
	double yComponent = difference(nextSf.cell[i][j - 1][k].get(), nextSf.cell[i][j + 1][k].get(), cellSize.y);
	double zComponent = difference(nextSf.cell[i][j][k - 1].get(), nextSf.cell[i][j][k + 1].get(), cellSize.z);

	result = glm::vec3(xComponent, yComponent, zComponent);
	return result;
}
double divergence(VectorGrid& nextVf, int i, int j, int k) {
	glm::vec3 cellSize = nextVf.cell[i][j][k].getCellSize();
	double x = difference(nextVf.cell[i - 1][j][k].getX(), nextVf.cell[i + 1][j][k].getX(), cellSize.x);
	double y = difference(nextVf.cell[i][j - 1][k].getY(), nextVf.cell[i][j + 1][k].getY(), cellSize.y);
	double z = difference(nextVf.cell[i][j][k - 1].getZ(), nextVf.cell[i][j][k + 1].getZ(), cellSize.z);

	return x + y + z;
}
glm::vec3 curl(VectorGrid& nextVf, int i, int j, int k) {

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

glm::vec3 laplaccian(VectorGrid& nextVf, int i, int j, int k) {
	vector<vector<vector<VectorCell>>>& nvc = nextVf.cell;
	double cellSize = nvc[i][j][k].getCellSize().x;

	double nx = nvc[i - 1][j][k].get().x + nvc[i + 1][j][k].get().x + nvc[i][j - 1][k].get().x + nvc[i][j + 1][k].get().x + nvc[i][j][k - 1].get().x + nvc[i][j][k + 1].get().x;
	double ny = nvc[i - 1][j][k].get().y + nvc[i + 1][j][k].get().y + nvc[i][j - 1][k].get().y + nvc[i][j + 1][k].get().y + nvc[i][j][k - 1].get().y + nvc[i][j][k + 1].get().y;
	double nz = nvc[i - 1][j][k].get().z + nvc[i + 1][j][k].get().z + nvc[i][j - 1][k].get().z + nvc[i][j + 1][k].get().z + nvc[i][j][k - 1].get().z + nvc[i][j][k + 1].get().z;
	
	nx = nx - 6 * nvc[i][j][k].get().x;
	ny = ny - 6 * nvc[i][j][k].get().y;
	nz = nz - 6 * nvc[i][j][k].get().z;

	nx = nx / (cellSize * cellSize);
	ny = ny / (cellSize * cellSize);
	nz = nz / (cellSize * cellSize);
	return glm::vec3(nx, ny, nz);
}

double laplaccian(ScalarGrid& nextSf, int i, int j, int k) {
	vector<vector<vector<ScalarCell>>>& nvc = nextSf.cell;
	double cellSize = nvc[i][j][k].getCellSize().x;

	double nv = nvc[i - 1][j][k].get() + nvc[i + 1][j][k].get() + nvc[i][j - 1][k].get() + nvc[i][j + 1][k].get() + nvc[i][j][k - 1].get() + nvc[i][j][k + 1].get();
	nv = nv - 6 * nvc[i][j][k].get();


	nv = nv / (cellSize * cellSize);
	return nv;
}

void neumannBoundaryCondition(VectorGrid& vf) {
	double nx, ny, nz;
	int bx, by, bz;
	glm::vec3 avg;
	bx = vf.getNumber().x+1;
	by = vf.getNumber().y + 1;
	bz = vf.getNumber().z + 1;

	for (int j = 0; j < vf.getNumber().y; j++) {
		for (int k = 0; k < vf.getNumber().z; k++) {
			nx = -vf.cell[1][j][k].getX();
			ny = vf.cell[1][j][k].getY();
			nz = vf.cell[1][j][k].getZ();
			vf.cell[0][j][k].updateCell(glm::vec3(nx, ny, nz));

			nx = -vf.cell[bx - 1][j][k].getX();
			ny = vf.cell[bx - 1][j][k].getY();
			nz = vf.cell[bx - 1][j][k].getZ();
			vf.cell[bx][j][k].updateCell(glm::vec3(nx, ny, nz));
		}
	}
	for (int i = 0; i < vf.getNumber().x; i++) {
		for (int k = 0; k < vf.getNumber().z; k++) {
			nx = vf.cell[i][1][k].getX();
			ny = -vf.cell[i][1][k].getY();
			nz = vf.cell[i][1][k].getZ();
			vf.cell[i][0][k].updateCell(glm::vec3(nx, ny, nz));

			nx = vf.cell[i][by-1][k].getX();
			ny = -vf.cell[i][by-1][k].getY();
			nz = vf.cell[i][by-1][k].getZ();
			vf.cell[i][by][k].updateCell(glm::vec3(nx, ny, nz));
		}
	}

	for (int i = 0; i < vf.getNumber().x; i++) {
		for (int j = 0; j < vf.getNumber().y; j++) {
			nx = vf.cell[i][j][1].getX();
			ny = vf.cell[i][j][1].getY();
			nz = -vf.cell[i][j][1].getZ();
			vf.cell[i][j][0].updateCell(glm::vec3(nx, ny, nz));

			nx = vf.cell[i][j][bz-1].getX();
			ny = vf.cell[i][j][bz-1].getY();
			nz = -vf.cell[i][j][bz-1].getZ();
			vf.cell[i][j][bz].updateCell(glm::vec3(nx, ny, nz));
		}
	}
	// 코너 처리
	avg = (vf.cell[1][0][0].get() + vf.cell[0][1][0].get() + vf.cell[0][0][1].get())/glm::vec3(3);
	vf.cell[0][0][0].updateCell(avg);

	avg = (vf.cell[bx-1][0][0].get()+vf.cell[bx][1][0].get()+vf.cell[bx][0][1].get())/glm::vec3(3);
	vf.cell[bx][0][0].updateCell(avg);

	avg = (vf.cell[1][by][0].get() + vf.cell[0][by-1][0].get() + vf.cell[0][by][1].get()) / glm::vec3(3);
	vf.cell[0][by][0].updateCell(avg);

	avg = (vf.cell[1][0][bz].get() + vf.cell[0][1][bz].get() + vf.cell[0][0][bz-1].get()) / glm::vec3(3);
	vf.cell[0][0][bz].updateCell(avg);



	avg = (vf.cell[bx-1][by][0].get() + vf.cell[bx][by-1][0].get() + vf.cell[bx][by][1].get()) / glm::vec3(3);
	vf.cell[bx][by][0].updateCell(avg);

	avg = (vf.cell[1][by][bz].get() + vf.cell[0][by-1][bz].get() + vf.cell[0][by][bz-1].get()) / glm::vec3(3);
	vf.cell[0][by][bz].updateCell(avg);

	avg = (vf.cell[bx][1][bz].get() + vf.cell[bx-1][0][bz].get() + vf.cell[bx][0][bz-1].get()) / glm::vec3(3);
	vf.cell[bx][0][bz].updateCell(avg);

	avg = (vf.cell[bx-1][by][bz].get() + vf.cell[bx][by-1][bz].get() + vf.cell[bx][by][bz - 1].get()) / glm::vec3(3);
	vf.cell[bx][by][bz].updateCell(avg);
}
#endif