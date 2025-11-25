#include "VectorGrid.h"

VectorGrid::VectorGrid(vector<vector<vector<VectorCell>>> cell, glm::vec3 origin, glm::vec3 length, glm::vec3 number):Grid(origin, length, number)
{
	this->cell = cell;
	initCell();

}
void VectorGrid::createCell(int x, int y, int z, glm::vec3 cellPos, glm::vec3 cellSize, glm::vec3 value, bool isBoundary=false) {
	this->cell[x][y][z] = VectorCell(cellPos, cellSize, glm::vec3(0), isBoundary);
}
