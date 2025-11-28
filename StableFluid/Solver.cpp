#include "Solver.h"


Solver::Solver(double dt) : 
	prevVelocityField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(640)),
	nextVelocityField( glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(640)),
	prevRhoField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64)),
	nextRhoField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64))
{
	this->dt = dt;
}

void Solver::addForce(VectorCell& nextVc, glm::vec3 force) {
	double nX = nextVc.getX() + dt * force.x;
	double nY = nextVc.getY() + dt * force.y;
	double nZ = nextVc.getZ() + dt * force.z;

	nextVc.updateCell(glm::vec3(nX, nY, nZ));
}
void Solver::transport(VectorCell& nextVc, VectorGrid& vf, int i, int j, int k, double dt) {
	glm::vec3 prevPos = trace(vf, nextVc, i, j, k, dt);
	nextVc.updateCell(interpolate(vf, prevPos));
}
void Solver::diffuse(VectorGrid& vf, ScalarGrid& sf, double viscosity, int i, int j, int k) {
	
}
void Solver::project(VectorCell& nextVc){

}

void Solver::updateVectorField(glm::vec3 force){
	for (int i = 0; i < prevVelocityField.getNumber().x; i++) {
		for (int j = 0; j < prevVelocityField.getNumber().y; j++) {
			for (int k = 0; k < prevVelocityField.getNumber().z; k++) {
				VectorCell& currentVc = prevVelocityField.cell[i][j][k];
				if (currentVc.isBoundary()) {
					continue;
				}
				addForce(currentVc, force);
				nextVelocityField.cell[i][j][k].updateCell((currentVc.get()));
			}
		}
	}
	prevVelocityField = nextVelocityField;
	for (int i = 0; i < prevVelocityField.getNumber().x; i++) {
		for (int j = 0; j < prevVelocityField.getNumber().y; j++) {
			for (int k = 0; k < prevVelocityField.getNumber().z; k++) {
				VectorCell& currentVc = prevVelocityField.cell[i][j][k];
				if (currentVc.isBoundary()) {
					continue;
				}
				transport(currentVc, prevVelocityField, i, j, k, dt);
				nextVelocityField.cell[i][j][k].updateCell((currentVc.get()));
			}
		}
	}
	ScalarGrid sf = ScalarGrid(glm::vec3(0, 0, 0), nextVelocityField.getLength(), nextVelocityField.getNumber());
	VectorGrid vf = VectorGrid(glm::vec3(0, 0, 0), nextVelocityField.getLength(), nextVelocityField.getNumber());
	for (int i = 0; i < nextVelocityField.getNumber().x; i++) {
		for (int j = 0; j < nextVelocityField.getNumber().y; j++) {
			for (int k = 0; k < nextVelocityField.getNumber().z; k++) {
				sf.cell[i][j][k].updateCell(divergence(nextVelocityField, i, j, k));
				vf.cell[i][j][k].updateCell(curl(nextVelocityField, i, j, k));
			}
		}
	}
	for (int i = 0; i < nextVelocityField.getNumber().x; i++) {
		for (int j = 0; j < nextVelocityField.getNumber().y; j++) {
			for (int k = 0; k < nextVelocityField.getNumber().z; k++) {
				VectorCell currentVc = nextVelocityField.cell[i][j][k];
				if (currentVc.isBoundary()) {
					continue;
				}
				diffuse();
				project(currentVc);

			}
		}
	}

	
	prevVelocityField = nextVelocityField;
}
void Solver::updateScalarField(double source) {

}
