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
#include <cmath>

#include "VectorCell.h"
#include "ScalarCell.h"
#include "VectorGrid.h"
#include "ScalarGrid.h"

inline glm::vec3 trace(VectorGrid& prevField, VectorCell vc, int i, int j, int k, double dt) {
	glm::vec3 currentPos = glm::vec3(prevField.cell[i][j][k].getPos().x, prevField.cell[i][j][k].getPos().y, prevField.cell[i][j][k].getPos().z);
	glm::vec3 prevPos = glm::vec3(currentPos.x - vc.get().x * dt, currentPos.y - vc.get().y * dt, currentPos.z - vc.get().z * dt);

	return prevPos;
}
inline glm::vec3 trace(ScalarGrid& prevField, VectorCell vc, int i, int j, int k, double dt) {
	glm::vec3 currentPos = glm::vec3(prevField.cell[i][j][k].getPos().x, prevField.cell[i][j][k].getPos().y, prevField.cell[i][j][k].getPos().z);
	glm::vec3 prevPos = glm::vec3(currentPos.x - vc.get().x * dt, currentPos.y - vc.get().y * dt, currentPos.z - vc.get().z * dt);

	return prevPos;
}
inline double clamp(double value, double min, double max) {
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}

inline glm::vec3 interpolate(VectorGrid& vf, glm::vec3 prevPos) {
	double prevPosIndexX = (prevPos.x - vf.getOrigin().x) / vf.cell[0][0][0].getCellSize().x-0.5;
	double prevPosIndexY = (prevPos.y - vf.getOrigin().y) / vf.cell[0][0][0].getCellSize().y-0.5;
	double prevPosIndexZ = (prevPos.z - vf.getOrigin().z) / vf.cell[0][0][0].getCellSize().z-0.5;

	prevPosIndexX = clamp(prevPosIndexX, 1, vf.getNumber().x - 2);
	prevPosIndexY = clamp(prevPosIndexY, 1, vf.getNumber().y - 2);
	prevPosIndexZ = clamp(prevPosIndexZ, 1, vf.getNumber().z - 2);

	int i = (int)floor(prevPosIndexX);
	int j = (int)floor(prevPosIndexY);
	int k = (int)floor(prevPosIndexZ);

	double weightElementX = prevPosIndexX - i;
	double weightElementY = prevPosIndexY - j;
	double weightElementZ = prevPosIndexZ - k;

	weightElementX = clamp(weightElementX, 0, 1);
	weightElementY = clamp(weightElementY, 0, 1);
	weightElementZ = clamp(weightElementZ, 0, 1);

	glm::vec3 weightX = glm::vec3(weightElementX);
	glm::vec3 weightY = glm::vec3(weightElementY);
	glm::vec3 weightZ = glm::vec3(weightElementZ);

	glm::vec3 result;
	glm::vec3 interpolateX1 = (glm::vec3(1) - weightX) * vf.cell[i][j][k].get() + (weightX)*vf.cell[i + 1][j][k].get();
	glm::vec3 interpolateX2 = (glm::vec3(1) - weightX) * vf.cell[i][j + 1][k].get() + (weightX)*vf.cell[i + 1][j + 1][k].get();
	glm::vec3 interpolateX3 = (glm::vec3(1) - weightX) * vf.cell[i][j][k + 1].get() + (weightX)*vf.cell[i + 1][j][k + 1].get();
	glm::vec3 interpolateX4 = (glm::vec3(1) - weightX) * vf.cell[i][j + 1][k + 1].get() + (weightX)*vf.cell[i + 1][j + 1][k + 1].get();

	glm::vec3 interpolateY1 = (glm::vec3(1) - weightY) * interpolateX1 + (weightY)*interpolateX2;
	glm::vec3 interpolateY2 = (glm::vec3(1) - weightY) * interpolateX3 + (weightY)*interpolateX4;

	glm::vec3 interpolateZ = (glm::vec3(1) - weightZ) * interpolateY1 + (weightZ)*interpolateY2;

	result = interpolateZ;
	return result;
}

inline double interpolate(ScalarGrid& sf, glm::vec3 prevPos) {
	double prevPosIndexX = (prevPos.x - sf.getOrigin().x) / sf.cell[0][0][0].getCellSize().x - 0.5;
	double prevPosIndexY = (prevPos.y - sf.getOrigin().y) / sf.cell[0][0][0].getCellSize().y - 0.5;
	double prevPosIndexZ = (prevPos.z - sf.getOrigin().z) / sf.cell[0][0][0].getCellSize().z - 0.5;

	prevPosIndexX = clamp(prevPosIndexX, 1, sf.getNumber().x - 2);
	prevPosIndexY = clamp(prevPosIndexY, 1, sf.getNumber().y - 2);
	prevPosIndexZ = clamp(prevPosIndexZ, 1, sf.getNumber().z - 2);

	int i = (int)floor(prevPosIndexX);
	int j = (int)floor(prevPosIndexY);
	int k = (int)floor(prevPosIndexZ);

	double weightX = prevPosIndexX - i;
	double weightY = prevPosIndexY - j;
	double weightZ = prevPosIndexZ - k;

	weightX = clamp(weightX, 0, 1);
	weightY = clamp(weightY, 0, 1);
	weightZ = clamp(weightZ, 0, 1);

	double result;
	double interpolateX1 = (1 - weightX) * sf.cell[i][j][k].get() + (weightX)*sf.cell[i + 1][j][k].get();
	double interpolateX2 = (1 - weightX) * sf.cell[i][j + 1][k].get() + (weightX)*sf.cell[i + 1][j + 1][k].get();
	double interpolateX3 = (1 - weightX) * sf.cell[i][j][k + 1].get() + (weightX)*sf.cell[i + 1][j][k + 1].get();
	double interpolateX4 = (1 - weightX) * sf.cell[i][j + 1][k + 1].get() + (weightX)*sf.cell[i + 1][j + 1][k + 1].get();

	double interpolateY1 = (1 - weightY) * interpolateX1 + (weightY)*interpolateX2;
	double interpolateY2 = (1 - weightY) * interpolateX3 + (weightY)*interpolateX4;

	double interpolateZ = (1 - weightZ) * interpolateY1 + (weightZ)*interpolateY2;

	result = interpolateZ;
	return result;
}

inline double difference(double prevValue, double nextValue, double cellSize) {
	return (nextValue - prevValue) / (2.0*cellSize);
}

inline glm::vec3 gradient(ScalarGrid& nextSf, int i, int j, int k) {
	glm::vec3 result;
	glm::vec3 cellSize = nextSf.cell[i][j][k].getCellSize();

	double xComponent = difference(nextSf.cell[i - 1][j][k].get(), nextSf.cell[i + 1][j][k].get(), cellSize.x);
	double yComponent = difference(nextSf.cell[i][j - 1][k].get(), nextSf.cell[i][j + 1][k].get(), cellSize.y);
	double zComponent = difference(nextSf.cell[i][j][k - 1].get(), nextSf.cell[i][j][k + 1].get(), cellSize.z);

	result = glm::vec3(xComponent, yComponent, zComponent);
	return result;
}
inline double divergence(VectorGrid& nextVf, int i, int j, int k) {
	glm::vec3 cellSize = nextVf.cell[i][j][k].getCellSize();
	double x = difference(nextVf.cell[i - 1][j][k].getX(), nextVf.cell[i + 1][j][k].getX(), cellSize.x);
	double y = difference(nextVf.cell[i][j - 1][k].getY(), nextVf.cell[i][j + 1][k].getY(), cellSize.y);
	double z = difference(nextVf.cell[i][j][k - 1].getZ(), nextVf.cell[i][j][k + 1].getZ(), cellSize.z);

	return x + y + z;
}
inline glm::vec3 curl(VectorGrid& nextVf, int i, int j, int k) {

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

inline glm::vec3 neighborSum(VectorGrid& vf, int i, int j, int k) {
	return vf.cell[i - 1][j][k].get() + vf.cell[i + 1][j][k].get() + vf.cell[i][j - 1][k].get() + vf.cell[i][j + 1][k].get() + vf.cell[i][j][k - 1].get() + vf.cell[i][j][k + 1].get();
}
inline double neighborSum(ScalarGrid& sf, int i, int j, int k) {
	return sf.cell[i - 1][j][k].get() + sf.cell[i + 1][j][k].get() + sf.cell[i][j - 1][k].get() + sf.cell[i][j + 1][k].get() + sf.cell[i][j][k - 1].get() + sf.cell[i][j][k + 1].get();
}

inline glm::vec3 laplaccian(VectorGrid& nextVf, int i, int j, int k) {
	vector<vector<vector<VectorCell>>>& nvc = nextVf.cell;
	glm::vec3 cellSize = nvc[i][j][k].getCellSize();

	glm::vec3 neighbor = neighborSum(nextVf, i, j, k);
	glm::vec3 result = neighbor - glm::vec3(6) * nvc[i][j][k].get();

	result = result / (cellSize * cellSize);
	return result;
}

inline double laplaccian(ScalarGrid& nextSf, int i, int j, int k) {
	vector<vector<vector<ScalarCell>>>& nvc = nextSf.cell;
	double cellSize = nvc[i][j][k].getCellSize().x;

	double neighbor = neighborSum(nextSf, i, j, k);
	double result = neighbor - 6 * nvc[i][j][k].get();

	result = result / (cellSize * cellSize);
	return result;
}

inline void velocityBoundaryCondition(VectorGrid& vf) {
	double x, y, z;
	int bx, by, bz;
	glm::vec3 avg;
	bx = (int)vf.getNumber().x - 1;
	by = (int)vf.getNumber().y - 1;
	bz = (int)vf.getNumber().z - 1;

	// 搁贸府
	for (int j = 1; j < by + 1; j++) {
		for (int k = 1; k < bz + 1; k++) {
			x = -vf.cell[1][j][k].getX();
			y = vf.cell[1][j][k].getY();
			z = vf.cell[1][j][k].getZ();
			vf.cell[0][j][k].updateCell(glm::vec3(x, y, z));

			x = -vf.cell[bx - 1][j][k].getX();
			y = vf.cell[bx - 1][j][k].getY();
			z = vf.cell[bx - 1][j][k].getZ();
			vf.cell[bx][j][k].updateCell(glm::vec3(x, y, z));
		}
	}
	for (int i = 1; i < bx + 1; i++) {
		for (int k = 1; k < bz + 1; k++) {
			x = vf.cell[i][1][k].getX();
			y = -vf.cell[i][1][k].getY();
			z = vf.cell[i][1][k].getZ();
			vf.cell[i][0][k].updateCell(glm::vec3(x, y, z));

			x = vf.cell[i][by-1][k].getX();
			y = -vf.cell[i][by-1][k].getY();
			z = vf.cell[i][by-1][k].getZ();
			vf.cell[i][by][k].updateCell(glm::vec3(x, y, z));
		}
	}
	for (int i = 1; i < bx + 1; i++) {
		for (int j = 1; j < by + 1; j++) {
			x = vf.cell[i][j][1].getX();
			y = vf.cell[i][j][1].getY();
			z = -vf.cell[i][j][1].getZ();
			vf.cell[i][j][0].updateCell(glm::vec3(x, y, z));

			x = vf.cell[i][j][bz-1].getX();
			y = vf.cell[i][j][bz-1].getY();
			z = -vf.cell[i][j][bz-1].getZ();
			vf.cell[i][j][bz].updateCell(glm::vec3(x, y, z));
		}
	}

	// 咖瘤 贸府
	for (int i = 1; i < bx + 1; i++) {
		avg = (vf.cell[i][1][0].get() + vf.cell[i][0][1].get()) / glm::vec3(2);
		vf.cell[i][0][0].updateCell(avg);

		avg = (vf.cell[i][by - 1][0].get() + vf.cell[i][by][1].get()) / glm::vec3(2);
		vf.cell[i][by][0].updateCell(avg);

		avg = (vf.cell[i][1][bz].get() + vf.cell[i][0][bz - 1].get()) / glm::vec3(2);
		vf.cell[i][0][bz].updateCell(avg);

		avg = (vf.cell[i][by - 1][bz].get() + vf.cell[i][by][bz - 1].get()) / glm::vec3(2);
		vf.cell[i][by][bz].updateCell(avg);
	}
	for (int j = 1; j < by + 1; j++) {
		avg = (vf.cell[1][j][0].get() + vf.cell[0][j][1].get()) / glm::vec3(2);
		vf.cell[0][j][0].updateCell(avg);

		avg = (vf.cell[bx - 1][j][0].get() + vf.cell[bx][j][1].get()) / glm::vec3(2);
		vf.cell[bx][j][0].updateCell(avg);

		avg = (vf.cell[1][j][bz].get() + vf.cell[0][j][bz - 1].get()) / glm::vec3(2);
		vf.cell[0][j][bz].updateCell(avg);

		avg = (vf.cell[bx - 1][j][bz].get() + vf.cell[bx][j][bz - 1].get()) / glm::vec3(2);
		vf.cell[bx][j][bz].updateCell(avg);
	}
	for (int k = 1; k < bz + 1; k++) {
		avg = (vf.cell[1][0][k].get() + vf.cell[0][1][k].get()) / glm::vec3(2);
		vf.cell[0][0][k].updateCell(avg);

		avg = (vf.cell[bx - 1][0][k].get() + vf.cell[bx][1][k].get()) / glm::vec3(2);
		vf.cell[bx][0][k].updateCell(avg);

		avg = (vf.cell[1][by][k].get() + vf.cell[0][by - 1][k].get()) / glm::vec3(2);
		vf.cell[0][by][k].updateCell(avg);

		avg = (vf.cell[bx - 1][by][k].get() + vf.cell[bx][by - 1][k].get()) / glm::vec3(2);
		vf.cell[bx][by][k].updateCell(avg);
	}

	// 内呈 贸府
	for (int i = 0; i < bx + 1; i += bx) {
		for (int j = 0; j < by + 1; j += by) {
			for (int k = 0; k < bz + 1; k += bz) {
				int addX = -1;
				int addY = -1;
				int addZ = -1;
				if (i == 0) {
					addX *= -1;
				}
				if (j == 0) {
					addY *= -1;
				}
				if (k == 0) {
					addZ *= -1;
				}
				avg = (vf.cell[i + addX][j][k].get() + vf.cell[i][j + addY][k].get() + vf.cell[i][j][k + addZ].get()) / glm::vec3(3);
				vf.cell[i][j][k].updateCell(avg);
			}
		}
	}
}

inline void neumannBoundaryCondition(ScalarGrid& sf) {
	int bx, by, bz;
	double avg;
	bx = (int)sf.getNumber().x - 1;
	by = (int)sf.getNumber().y - 1;
	bz = (int)sf.getNumber().z - 1;

	// 搁贸府
	for (int j = 1; j < by + 1; j++) {
		for (int k = 1; k < bz + 1; k++) {
			sf.cell[0][j][k].updateCell(sf.cell[1][j][k].get());
			sf.cell[bx][j][k].updateCell(sf.cell[bx - 1][j][k].get());
		}
	}
	for (int i = 1; i < bx + 1; i++) {
		for (int k = 1; k < bz + 1; k++) {
			sf.cell[i][0][k].updateCell(sf.cell[i][1][k].get());
			sf.cell[i][by][k].updateCell(sf.cell[i][by - 1][k].get());
		}
	}
	for (int i = 1; i < bx + 1; i++) {
		for (int j = 1; j < by + 1; j++) {
			sf.cell[i][j][0].updateCell(sf.cell[i][j][1].get());
			sf.cell[i][j][bz].updateCell(sf.cell[i][j][bz - 1].get());
		}
	}

	// 咖瘤 贸府
	for (int i = 1; i < bx + 1; i++) {
		avg = (sf.cell[i][1][0].get() + sf.cell[i][0][1].get()) /2.0;
		sf.cell[i][0][0].updateCell(avg);

		avg = (sf.cell[i][by - 1][0].get() + sf.cell[i][by][1].get()) / 2.0;
		sf.cell[i][by][0].updateCell(avg);

		avg = (sf.cell[i][1][bz].get() + sf.cell[i][0][bz - 1].get()) / 2.0;
		sf.cell[i][0][bz].updateCell(avg);

		avg = (sf.cell[i][by - 1][bz].get() + sf.cell[i][by][bz - 1].get()) / 2.0;
		sf.cell[i][by][bz].updateCell(avg);
	}
	for (int j = 1; j < by + 1; j++) {
		avg = (sf.cell[1][j][0].get() + sf.cell[0][j][1].get()) / 2.0;
		sf.cell[0][j][0].updateCell(avg);

		avg = (sf.cell[bx - 1][j][0].get() + sf.cell[bx][j][1].get()) / 2.0;
		sf.cell[bx][j][0].updateCell(avg);

		avg = (sf.cell[1][j][bz].get() + sf.cell[0][j][bz - 1].get()) / 2.0;
		sf.cell[0][j][bz].updateCell(avg);

		avg = (sf.cell[bx - 1][j][bz].get() + sf.cell[bx][j][bz - 1].get()) / 2.0;
		sf.cell[bx][j][bz].updateCell(avg);
	}
	for (int k = 1; k < bz + 1; k++) {
		avg = (sf.cell[1][0][k].get() + sf.cell[0][1][k].get()) / 2.0;
		sf.cell[0][0][k].updateCell(avg);

		avg = (sf.cell[bx - 1][0][k].get() + sf.cell[bx][1][k].get()) / 2.0;
		sf.cell[bx][0][k].updateCell(avg);

		avg = (sf.cell[1][by][k].get() + sf.cell[0][by - 1][k].get()) / 2.0;
		sf.cell[0][by][k].updateCell(avg);

		avg = (sf.cell[bx - 1][by][k].get() + sf.cell[bx][by - 1][k].get()) / 2.0;
		sf.cell[bx][by][k].updateCell(avg);
	}

	// 内呈 贸府
	for (int i = 0; i < bx + 1; i += bx) {
		for (int j = 0; j < by + 1; j += by) {
			for (int k = 0; k < bz + 1; k += bz) {
				int addX = -1;
				int addY = -1;
				int addZ = -1;
				if (i == 0) {
					addX *= -1;
				}
				if (j == 0) {
					addY *= -1;
				}
				if (k == 0) {
					addZ *= -1;
				}
				avg = (sf.cell[i + addX][j][k].get() + sf.cell[i][j + addY][k].get() + sf.cell[i][j][k + addZ].get()) / 3.0;
				sf.cell[i][j][k].updateCell(avg);
			}
		}
	}
}



#endif