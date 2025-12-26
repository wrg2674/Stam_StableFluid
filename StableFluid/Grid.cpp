#include "Grid.h"

template <typename T>
Grid<T>::Grid(glm::vec3 origin, glm::vec3 length, glm::vec3 number) {
	this->origin = origin;
	this->length = length;
	this->number = number;
	this->cellSize = length / number;
}
template <typename T>
void Grid<T>::initCell() {
	for (int x = 0; x <= number.x; x++) {
		for (int y = 0; y <= number.y; y++) {
			for (int z = 0; z <= number.z; z++) {
				double xPos = x * cellSize.x+0.5*cellSize.x;
				double yPos = y * cellSize.y+0.5*cellSize.y;
				double zPos = z * cellSize.z+0.5*cellSize.z;
				glm::vec3 cellPos = glm::vec3(xPos, yPos, zPos);

				if (x == number.x || y == number.y || z == number.z) {
					createCell(x, y, z, cellPos, cellSize, true);
				}
				else if (x == 0 || y == 0 || z == 0) {
					createCell(x, y, z, cellPos, cellSize, true);
				}
				else {
					createCell(x, y, z, cellPos, cellSize);
				}
			}
		}
	}
}
template <typename T>
glm::vec3 Grid<T>::getNumber() {
	return number;
}
template <typename T>
glm::vec3 Grid<T>::getLength() {
	return length;
}