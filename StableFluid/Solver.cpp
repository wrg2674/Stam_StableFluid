#include "Solver.h"


Solver::Solver(double dt) : 
	prevVelocityField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64)),
	nextVelocityField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64)),
	prevRhoField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64)),
	nextRhoField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(64)),
	prevPressureField(glm::vec3(0,0,-20), glm::vec3(20), glm::vec3(64)),
	nextPressureField(glm::vec3(0,0,-20), glm::vec3(20), glm::vec3(64))
{
	this->dt = dt;
}

void Solver::addForce(VectorCell& nextVc, glm::vec3 force) {
	double nX = nextVc.getX() + dt * force.x;
	double nY = nextVc.getY() + dt * force.y;
	double nZ = nextVc.getZ() + dt * force.z;

	nextVc.updateCell(glm::vec3(nX, nY, nZ));
}
void Solver::addSource(ScalarCell& nextSc, double source) {
	double result = nextSc.get() + dt * source;
	nextSc.updateCell(result);
}
void Solver::transport(VectorGrid& vf, VectorCell& nextVc, int i, int j, int k, double dt) {
	glm::vec3 prevPos = trace(vf, nextVc, i, j, k, dt);
	nextVc.updateCell(interpolate(vf, prevPos));
}
void Solver::transport(ScalarGrid& sf, VectorCell& nextVc, ScalarCell& nextSc, int i, int j, int k, double dt) {
	glm::vec3 prevPos = trace(sf, nextVc, i, j, k, dt);
	nextSc.updateCell(interpolate(sf, prevPos));
}
void Solver::diffuseVectorField(double viscosity) {
	double cellSize = prevVelocityField.cell[0][0][0].getCellSize().x;
	double alpha = viscosity * dt / (cellSize * cellSize);
	velocityBoundaryCondition(prevVelocityField);
	nextVelocityField.cell = prevVelocityField.cell;
	vector<vector<vector<VectorCell>>>jcbVec = prevVelocityField.cell;
	
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

				}
			}
		}
		nextVelocityField.cell = jcbVec;
		velocityBoundaryCondition(nextVelocityField);
	}
	
}
void Solver::diffuseScalarField(double diffusion) {
	double cellSize = prevRhoField.cell[0][0][0].getCellSize().x;
	double alpha = diffusion * dt / (cellSize * cellSize);

	neumannBoundaryCondition(prevRhoField);
	nextRhoField.cell = prevRhoField.cell;
	vector<vector<vector<ScalarCell>>>jcbSca = prevRhoField.cell;

	// Jacobi iteration
	for (int n = 0; n < 30; n++) {
		for (int i = 1; i < prevRhoField.getNumber().x - 1; i++) {
			for (int j = 1; j < prevRhoField.getNumber().y - 1; j++) {
				for (int k = 1; k < prevRhoField.getNumber().z - 1; k++) {
					vector<vector<vector<ScalarCell>>>& nsc = nextRhoField.cell;
					vector<vector<vector<ScalarCell>>>& psc = prevRhoField.cell;
					double laplaccianSca = laplaccian(nextRhoField, i, j, k);
					double ns = psc[i][j][k].get() + alpha * (cellSize * cellSize * laplaccianSca + 6 * nsc[i][j][k].get());
					ns = ns / (1 + 6 * alpha);
					jcbSca[i][j][k].updateCell(ns);
				}
			}
		}
		nextRhoField.cell = jcbSca;
		neumannBoundaryCondition(nextRhoField);
	}
}
void Solver::project(){

	int nx = (int)prevVelocityField.getNumber().x;
	int ny = (int)prevVelocityField.getNumber().y;
	int nz = (int)prevVelocityField.getNumber().z;
	double cellSize = prevVelocityField.cell[0][0][0].getCellSize().x;
	ScalarGrid divergenceF = prevPressureField;
	VectorGrid jcbVf = prevVelocityField;
	ScalarGrid jcbSf = prevPressureField;

	velocityBoundaryCondition(prevVelocityField);
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				divergenceF.cell[i][j][k].updateCell(divergence(prevVelocityField, i, j, k));
			}
		}
	}
	// Jacobi iteration
	for (int n = 0; n < 30; n++) {
		for (int i = 1; i < nx - 1; i++) {
			for (int j = 1; j < ny - 1; j++) {
				for (int k = 1; k < nz - 1; k++) {
					double neighbor = neighborSum(prevPressureField, i, j, k);
					double value = neighbor - divergenceF.cell[i][j][k].get() * cellSize * cellSize;
					value = value / 6;
					jcbSf.cell[i][j][k].updateCell(value);
				}
			}
		}
		prevPressureField = jcbSf;
		neumannBoundaryCondition(prevPressureField);
	}
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				glm::vec3 value = prevVelocityField.cell[i][j][k].get() - gradient(prevPressureField, i, j, k);
				jcbVf.cell[i][j][k].updateCell(value);
			}
		}
	}
	prevVelocityField = jcbVf;
	velocityBoundaryCondition(prevVelocityField);
}

void Solver::solveVectorField(glm::vec3 force){
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
				VectorCell currentVc = prevVelocityField.cell[i][j][k];
				if (currentVc.isBoundary()) {
					continue;
				}
				transport(prevVelocityField, currentVc, i, j, k, dt);
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
	prevFieldUpdate();
	double viscocity = 0.2;
	diffuseVectorField(viscocity);
	prevFieldUpdate();
	project();
	nextFieldUpdate();
}
void Solver::solveScalarField(double source) {
	for (int i = 1; i < prevRhoField.getNumber().x - 1; i++) {
		for (int j = 1; j < prevRhoField.getNumber().y - 1; j++) {
			for (int k = 1; k < prevRhoField.getNumber().z - 1; k++) {
				ScalarCell& currentSc = prevRhoField.cell[i][j][k];
				addSource(currentSc, source);
				nextRhoField.cell[i][j][k].updateCell((currentSc.get()));
			}
		}
	}
	prevRhoField = nextRhoField;
	for (int i = 1; i < prevRhoField.getNumber().x - 1; i++) {
		for (int j = 1; j < prevRhoField.getNumber().y - 1; j++) {
			for (int k = 1; k < prevRhoField.getNumber().z - 1; k++) {
				VectorCell currentVc = prevVelocityField.cell[i][j][k];

				transport(prevRhoField, currentVc, i, j, k, dt);
				ScalarCell currentSc = prevRhoField.cell[i][j][k];
				nextRhoField.cell[i][j][k].updateCell((currentSc.get()));
			}
		}
	}
	ScalarGrid sf = ScalarGrid(glm::vec3(0, 0, 0), nextRhoField.getLength(), nextRhoField.getNumber());
	VectorGrid vf = VectorGrid(glm::vec3(0, 0, 0), nextRhoField.getLength(), nextRhoField.getNumber());

	prevFieldUpdate();
	double viscocity = 0.5;
	diffuseScalarField(viscocity);
	prevFieldUpdate();

}

void Solver::prevFieldUpdate() {
	prevVelocityField = nextVelocityField;
	prevRhoField = nextRhoField;
	prevPressureField = nextPressureField;
}
void Solver::nextFieldUpdate() {
	nextVelocityField = prevVelocityField;
	nextRhoField = prevRhoField;
	nextPressureField = prevPressureField;
}
glm::vec3 Solver::getGridNumber() {
	return nextVelocityField.getNumber();
}
void Solver::injection(int i, int j, int k, int radius, double densityPerSec, glm::vec3 forcePerSec) {
	int nx = (int)prevRhoField.getNumber().x;
	int ny = (int)prevRhoField.getNumber().y;
	int nz = (int)prevRhoField.getNumber().z;

	i = clamp(i, 1, nx - 2);
	j = clamp(j, 1, ny - 2);
	k = clamp(k, 1, nz - 2);
	radius = clamp(radius, 1, nx - 1);
	for (int x = i - radius; x <= i + radius; x++) {
		for (int y = j - radius; y <= j + radius; y++) {
			for (int z = k - radius; z <= k + radius; z++) {
				if (x < 1 || y < 1 || z < 1 || x >= nx - 1 || y >= ny - 1 || z >= nz - 1) {
					continue;
				}
				double dx = i - x;
				double dy = j - y;
				double dz = k - z;
				double dist = sqrt(dx * dx + dy * dy + dz * dz);
				double weight = 1 - (dist / radius);
				weight = clamp(weight, 0, 1);
				//prevRhoField.cell[x][y][z].updateCell(densityPerSec * weight);
				//prevVelocityField.cell[x][y][z].updateCell(forcePerSec * glm::vec3(weight));
				addSource(prevRhoField.cell[x][y][z], densityPerSec * weight);
				addForce(prevVelocityField.cell[x][y][z], forcePerSec * (float)weight);
			}
		}
	}
	neumannBoundaryCondition(prevRhoField);
	velocityBoundaryCondition(prevVelocityField);
}