#include "ScalarGrid.h"


ScalarGrid::ScalarGrid(glm::vec3 origin, glm::vec3 length, glm::vec3 number)
	:Grid(origin, length, number)
{
	initCell();
}
