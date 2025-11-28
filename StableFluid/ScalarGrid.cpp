#include "ScalarGrid.h"


ScalarGrid::ScalarGrid(glm::vec3 origin, glm::vec3 length, glm::vec3 number)
	:Grid<double>(origin, length, number)
{
	this->cell = cell;
	int nx = (int)number.x;
	int ny = (int)number.y;
	int nz = (int)number.z;
	cell.resize(nx + 1, vector<vector<ScalarCell>>(ny + 1, vector<ScalarCell>(nz + 1)));
	initCell();

}

void ScalarGrid::createCell(int x, int y, int z, glm::vec3 cellPos, glm::vec3 cellSize, bool isBoundary) {
	this->cell[x][y][z] = ScalarCell(cellPos, cellSize, 0, isBoundary);
}
