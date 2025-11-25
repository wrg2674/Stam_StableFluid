#include "Grid.h"

Grid::Grid(glm::vec3 origin, glm::vec3 length, glm::vec3 number) {
	this->origin = origin;
	this->length = length;
	this->number = number;
	this->cellSize = length / number;
}
void Grid::initCell() {
	for (int x = 0; x <= number.x; x++) {
		for (int y = 0; y <= number.y; y++) {
			for (int z = 0; z <= number.z; z++) {
				glm::vec3 cellPos = glm::vec3(x, y, z);
				if (x == number.x || y == number.y || z == number.z) {
					createCell(x, y, z, cellPos, true);
				}
				if (x == 0 || y == 0 || z == 0) {
					createCell(x, y, z, cellPos, true);
				}
				else {
					createCell(x, y, z, cellPos);
				}
			}
		}
	}
}
glm::vec3 Grid::getNumber() {
	return number;
}