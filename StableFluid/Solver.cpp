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
	double cellSize = prevVelocityField.cell[0][0][0].getCellSize().x;
	double alpha = alpha = viscosity * dt / (cellSize * cellSize);
	vector<vector<vector<VectorCell>>>jcbVec = prevVelocityField.cell;
	vector<vector<vector<ScalarCell>>>jcbSca = prevRhoField.cell;

	// Jacobi iteration
	for (int n = 0; n < 30; n++) {
		for (int i = 1; i < prevVelocityField.getNumber().x - 1; i++) {
			for (int j = 1; j < prevVelocityField.getNumber().y - 1; j++) {
				for (int k = 1; k < prevVelocityField.getNumber().z - 1; k++) {
					vector<vector<vector<VectorCell>>>& nvc = nextVelocityField.cell;
					vector<vector<vector<VectorCell>>>& pvc = prevVelocityField.cell;
					glm::vec3 laplaccianVec = laplaccian(nextVelocityField, i, j, k);
					double nx = pvc[i][j][k].get().x + alpha * (cellSize * cellSize * laplaccianVec.x + 6 * nvc[i][j][k].get().x);
					double ny = pvc[i][j][k].get().y + alpha * (cellSize * cellSize * laplaccianVec.y + 6 * nvc[i][j][k].get().y);
					double nz = pvc[i][j][k].get().z + alpha * (cellSize * cellSize * laplaccianVec.z + 6 * nvc[i][j][k].get().z);
					
					nx = nx / (1 + 6 * alpha);
					ny = ny / (1 + 6 * alpha);
					nz = nz / (1 + 6 * alpha);
					glm::vec3 updateVector = glm::vec3(nx, ny, nz);
					jcbVec[i][j][k].updateCell(updateVector);

					vector<vector<vector<ScalarCell>>>& nsc = nextRhoField.cell;
					vector<vector<vector<ScalarCell>>>& psc = prevRhoField.cell;
					double laplaccianSca = laplaccian(nextRhoField, i, j, k);
					double ns = psc[i][j][k].get() + alpha * (cellSize * cellSize * laplaccianSca + 6 * nsc[i][j][k].get());
					ns = ns / (1 + 6 * alpha);
					jcbSca[i][j][k].updateCell(ns);

					
				}
			}
		}
		nextVelocityField.cell = jcbVec;
		nextRhoField.cell = jcbSca;
	}
	
}
void Solver::project(VectorGrid& vf, ScalarGrid& pf){
	int nx = (int)vf.getNumber().x;
	int ny = (int)vf.getNumber().y;
	int nz = (int)vf.getNumber().z;
	double cellSize = vf.cell[0][0][0].getCellSize().x;
	ScalarGrid divergenceF = pf;
	VectorGrid jcbVf = vf;
	ScalarGrid jcbSf = pf;
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				divergenceF.cell[i][j][k].updateCell(divergence(vf, i, j, k));
			}
		}
	}
	// Jacobi iteration
	for (int n = 0; n < 30; n++) {
		for (int i = 1; i < nx - 1; i++) {
			for (int j = 1; j < ny - 1; j++) {
				for (int k = 1; k < nz - 1; k++) {
					double neighbor = neighborSum(pf, i, j, k);
					double value = neighbor - divergenceF.cell[i][j][k].get() * cellSize * cellSize;
					value = value / 6;
					jcbSf.cell[i][j][k].updateCell(value);
				}
			}
		}
		pf = jcbSf;
	}
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				glm::vec3 value = vf.cell[i][j][k].get() - gradient(pf, i, j, k);
				jcbVf.cell[i][j][k].updateCell(value);
			}
		}
	}
	vf = jcbVf;

	// neumann 경계조건 추가해야함 (위치는 모름)
}

void Solver::updateVectorField(glm::vec3 force){
	for (int i = 1; i < prevVelocityField.getNumber().x - 1; i++) {
		for (int j = 1; j < prevVelocityField.getNumber().y - 1; j++) {
			for (int k = 1; k < prevVelocityField.getNumber().z - 1; k++) {
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
	for (int i = 1; i < prevVelocityField.getNumber().x - 1; i++) {
		for (int j = 1; j < prevVelocityField.getNumber().y - 1; j++) {
			for (int k = 1; k < prevVelocityField.getNumber().z - 1; k++) {
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
	for (int i = 1; i < nextVelocityField.getNumber().x - 1; i++) {
		for (int j = 1; j < nextVelocityField.getNumber().y - 1; j++) {
			for (int k = 1; k < nextVelocityField.getNumber().z - 1; k++) {
				sf.cell[i][j][k].updateCell(divergence(nextVelocityField, i, j, k));
				vf.cell[i][j][k].updateCell(curl(nextVelocityField, i, j, k));
			}
		}
	}
	double viscocity = 0.5;
	diffuse(viscocity);

	for (int i = 1; i < nextVelocityField.getNumber().x - 1; i++) {
		for (int j = 1; j < nextVelocityField.getNumber().y - 1; j++) {
			for (int k = 1; k < nextVelocityField.getNumber().z - 1; k++) {
				VectorCell currentVc = nextVelocityField.cell[i][j][k];
				if (currentVc.isBoundary()) {
					continue;
				}
				
				
			}
		}
	}

	
	prevVelocityField = nextVelocityField;
}
void Solver::updateScalarField(double source) {

}
