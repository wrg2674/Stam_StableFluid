#ifndef SCALAR_H
#define SCALAR_H

#include "Cell.h"

class ScalarCell : public Cell {
private:
	double rho;

public:
	ScalarCell(glm::vec3 pos, glm::vec3 cellSize, double rho, bool isBoundary);
	void updateCell(double rho);
	double get();
};

#endif#pragma once
