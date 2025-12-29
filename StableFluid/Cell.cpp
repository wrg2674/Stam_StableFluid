#include "Cell.h"

Cell::Cell(glm::vec3 pos, glm::vec3 cellSize, bool boundary) {
	this->pos = pos;
	this->boundary = boundary;
	this->cellSize = cellSize;
}

glm::vec3 Cell::getPos() {
	return pos;
}
glm::vec3 Cell::getCellSize() {
	return cellSize;
}
bool Cell::isBoundary() {
	return boundary;
}