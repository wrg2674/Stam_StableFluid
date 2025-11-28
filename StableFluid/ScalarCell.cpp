#include "ScalarCell.h"

ScalarCell::ScalarCell(glm::vec3 pos,glm::vec3 cellSize ,double rho, bool isBoundary):Cell(pos, cellSize, isBoundary) {
	this->rho = rho;
	if (isBoundary == true) {
		this->rho = 0;
	}
}

void ScalarCell::updateCell(double rho)
{
	this->rho = rho;
}
double ScalarCell::get() {
	return rho;
}
