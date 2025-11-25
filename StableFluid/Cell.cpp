#include "Cell.h"

Cell::Cell(glm::vec3 pos, glm::vec3 cellSize = glm::vec3(1, 1, 1), bool Boundary) {
	this->pos = pos;
	this->Boundary = Boundary;
	this->cellSize = cellSize;
}

glm::vec3 Cell::getPos() {
	return pos;
}
glm::vec3 Cell::getCellSize() {
	return cellSize;
}
bool Cell::isBoundary() {
	return isBoundary;
}