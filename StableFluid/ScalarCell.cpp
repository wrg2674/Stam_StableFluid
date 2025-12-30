#include "ScalarCell.h"

ScalarCell::ScalarCell(glm::vec3 pos, glm::vec3 cellSize, bool isBoundary):Cell(pos, cellSize, isBoundary) {
	this->value = 0;
}

void ScalarCell::updateCell(double value)
{
	this->value = value;
}
double ScalarCell::get() {
	return value;
}
