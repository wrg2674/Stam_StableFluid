#include "Solver.h"


Solver::Solver(double dt) : 
	prevVelocityField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64)),
	nextVelocityField( glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64)),
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
void Solver::diffuse(double viscosity) {
	double alpha = alpha = viscosity * dt / (prevVelocityField.cell.size() * prevVelocityField.cell.size());
	for (int n = 0; n < 30; n++) {
		for (int i = 0; i < prevVelocityField.getNumber().x; i++) {
			for (int j = 0; j < prevVelocityField.getNumber().y; j++) {
				for (int k = 0; k < prevVelocityField.getNumber().z; k++) {
					vector<vector<vector<VectorCell>>>& nvc = nextVelocityField.cell;
					double nx = nvc[i - 1][j][k].get().x + nvc[i + 1][j][k].get().x + nvc[i][j - 1][k].get().x + nvc[i][j + 1][k].get().x + nvc[i][j][k - 1].get().x + nvc[i][j][k + 1].get().x;
					double ny = nvc[i - 1][j][k].get().y + nvc[i + 1][j][k].get().y + nvc[i][j - 1][k].get().y + nvc[i][j + 1][k].get().y + nvc[i][j][k - 1].get().y + nvc[i][j][k + 1].get().y;
					double nz = nvc[i - 1][j][k].get().z + nvc[i + 1][j][k].get().z + nvc[i][j - 1][k].get().z + nvc[i][j + 1][k].get().z + nvc[i][j][k - 1].get().z + nvc[i][j][k + 1].get().z;
					nx = (alpha * nx + prevVelocityField.cell[i][j][k].get().x) / (1 + 6 * alpha);
					ny = (alpha * ny + prevVelocityField.cell[i][j][k].get().y) / (1 + 6 * alpha);
					nz = (alpha * nz + prevVelocityField.cell[i][j][k].get().z) / (1 + 6 * alpha);

					glm::vec3 updateVector = glm::vec3(nx, ny, nz);
					nvc[i][j][k].updateCell(updateVector);

					vector<vector<vector<ScalarCell>>>& nsc = nextRhoField.cell;
					double ns = nsc[i - 1][j][k].get() + nsc[i + 1][j][k].get() + nsc[i][j - 1][k].get() + nsc[i][j + 1][k].get() + nsc[i][j][k - 1].get() + nsc[i][j][k + 1].get();
					ns = (alpha * ns + prevRhoField.cell[i][j][k].get()) / (1 + 6 * alpha);

					nsc[i][j][k].updateCell(ns);
				}
			}
		}
	}
	
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
