#include "ScalarGrid.h"

ScalarGrid::ScalarGrid(vector<vector<vector<ScalarCell>>> cell, glm::vec3 origin, glm::vec3 length, glm::vec3 number) :Grid(origin, length, number)
{
	this->cell = cell;
	initCell();

}
void ScalarGrid::createCell(int x, int y, int z, glm::vec3 cellPos, bool isBoundary=false) {
	this->cell[x][y][z] = ScalarCell(cellPos, 0, isBoundary);
}
