#ifndef SCALAR_H
#define SCALAR_H

#include "Cell.h"

class ScalarCell : public Cell {
private:
	double value;

public:
	ScalarCell(glm::vec3 pos, glm::vec3 cellSize, bool isBoundary);
	void updateCell(double value);
	double get();
};

#endif
