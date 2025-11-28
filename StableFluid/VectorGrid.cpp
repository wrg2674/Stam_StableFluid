#include "VectorGrid.h"


VectorGrid::VectorGrid(glm::vec3 origin, glm::vec3 length, glm::vec3 number)
	:Grid<glm::vec3>(origin, length, number)
{
	int nx = (int)number.x;
	int ny = (int)number.y;
	int nz = (int)number.z;
	this->cell.resize(nx + 1, vector<vector<VectorCell>>(ny + 1, vector<VectorCell>(nz + 1)));
	initCell();

}
void VectorGrid::createCell(int x, int y, int z, glm::vec3 cellPos, glm::vec3 cellSize, glm::vec3 value, bool isBoundary) {
	//this->cell.push_back(VectorCell(cellPos, cellSize, glm::vec3(0), isBoundary));
	this->cell[x][y][z] = VectorCell(cellPos, cellSize, glm::vec3(0), isBoundary);
}
