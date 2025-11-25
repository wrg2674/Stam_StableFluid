#include "Solver.h"


Solver::Solver(double dt) {
	this->dt = dt;
	
	vector<vector<vector<VectorCell>>> prevVc = vector<vector<vector<VectorCell>>>();
	vector<vector<vector<VectorCell>>> nextVc = vector<vector<vector<VectorCell>>>();
	vector<vector<vector<ScalarCell>>> prevSc = vector<vector<vector<ScalarCell>>>();
	vector<vector<vector<ScalarCell>>> nextSc = vector<vector<vector<ScalarCell>>>();

	VectorGrid prevVelocityField = VectorGrid(prevVc, glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(640));
	VectorGrid nextVelocityField = VectorGrid(nextVc, glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(640));
	ScalarGrid prevRhoField = ScalarGrid(prevSc, glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(640));
	ScalarGrid nextRhoField = ScalarGrid(nextSc, glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(640));
	
	glm::vec3 force = glm::vec3(0);

	for (int i = 0; i < prevVelocityField.getNumber().x; i++) {
		for (int j = 0; j < prevVelocityField.getNumber().y; j++) {
			for (int k = 0; k < prevVelocityField.getNumber().z; k++) {
				VectorCell currentVc = prevVelocityField.cell[i][j][k];
				if (currentVc.isBoundary()) {
					continue;
				}
				addForce(currentVc, force);
				transport(prevVelocityField, currentVc, i, j, k, dt);
			}
		}
	}
}

void Solver::addForce(VectorCell& prevVc, glm::vec3 force) {
	double nX = prevVc.getX() + dt * force.x;
	double nY = prevVc.getY() + dt * force.y;
	double nZ = prevVc.getZ() + dt * force.z;

	prevVc.updateCell(glm::vec3(nX, nY, nZ));
}
void Solver::transport(VectorGrid& vf, VectorCell& prevVc, int i, int j, int k, double dt) {
	glm::vec3 prevPos = trace(vf, prevVc, i, j, k, dt);
	prevVc.updateCell(interpolate(vf , prevPos));
}
void Solver::diffuse() {

}
void Solver::project(VectorCell vc) {

}

void Solver::updateVectorField(glm::vec3 force){
	
}
void Solver::updateScalarField(double source) {

}
