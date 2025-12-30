#ifndef VECTORCELL_H
#define VECTORCELL_H

#include "Cell.h"

class VectorCell : public Cell {
private:
	glm::vec3 value;
public:
	VectorCell(glm::vec3 pos, glm::vec3 cellSize, bool isBoundary);
	void updateCell(glm::vec3 value);
	double getX();
	double getY();
	double getZ();
	glm::vec3 get();
};

#endif