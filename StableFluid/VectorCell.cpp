#include "VectorCell.h"

VectorCell::VectorCell(glm::vec3 pos, glm::vec3 vec, bool isBoundary) :Cell(pos, isBoundary) {
	this->vec = vec;
	if (isBoundary == true) {
		this->vec = glm::vec3(0);
	}
}

void VectorCell::updateCell(glm::vec3 vec)
{
	this->vec = vec;
}
double VectorCell::getX() {
	return vec.x;
}
double VectorCell::getY() {
	return vec.y;
}
double VectorCell::getZ() {
	return vec.z;
}
glm::vec3 VectorCell::get() {
	return vec;
}
