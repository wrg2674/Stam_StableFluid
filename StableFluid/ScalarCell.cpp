#include "ScalarCell.h"

ScalarCell::ScalarCell(glm::vec3 pos, glm::vec3 cellSize, bool isBoundary):Cell(pos, cellSize, isBoundary) {
	this->rho = 0;
}

void ScalarCell::updateCell(double rho)
{
	this->rho = rho;
}
double ScalarCell::get() {
	return rho;
}
