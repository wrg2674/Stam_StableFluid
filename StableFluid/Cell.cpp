#include "Cell.h"

Cell::Cell(glm::vec3 pos, bool Boundary) {
	this->pos = pos;
	this->Boundary = Boundary;
}

glm::vec3 Cell::getPos() {
	return pos;
}
bool Cell::isBoundary() {
	return isBoundary;
}