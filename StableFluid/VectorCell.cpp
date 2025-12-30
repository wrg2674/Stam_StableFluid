#include "VectorCell.h"

VectorCell::VectorCell(glm::vec3 pos, glm::vec3 cellSize, bool isBoundary):Cell(pos, cellSize, isBoundary){
	this->value = glm::vec3(0);
}

void VectorCell::updateCell(glm::vec3 value)
{
	this->value = value;
}
double VectorCell::getX() {
	return value.x;
}
double VectorCell::getY() {
	return value.y;
}
double VectorCell::getZ() {
	return value.z;
}
glm::vec3 VectorCell::get() {
	return value;
}
