#include "VectorGrid.h"


VectorGrid::VectorGrid(glm::vec3 origin, glm::vec3 length, glm::vec3 number)
	:Grid(origin, length, number)
{
	initCell();
}

