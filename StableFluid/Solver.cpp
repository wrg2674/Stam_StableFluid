#include "Solver.h"

#define RESOLUTION 32
Solver::Solver(double dt) : 
	prevVelocityField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(RESOLUTION)),
	nextVelocityField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(RESOLUTION)),
	prevRhoField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(RESOLUTION)),
	nextRhoField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(RESOLUTION)),
	prevPressureField(glm::vec3(0,0,-20), glm::vec3(20), glm::vec3(RESOLUTION)),
	nextPressureField(glm::vec3(0,0,-20), glm::vec3(20), glm::vec3(RESOLUTION)),
	jcbVelocityField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(RESOLUTION)),
	jcbRhoField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(RESOLUTION)),
	jcbPressureField(glm::vec3(0, 0, -20), glm::vec3(20), glm::vec3(RESOLUTION))
{
	this->dt = dt;
}

void Solver::addForce(glm::vec3 force) {
	int nx = (int)prevVelocityField.getNumber().x;
	int ny = (int)prevVelocityField.getNumber().y;
	int nz = (int)prevVelocityField.getNumber().z;
#pragma omp parallel for schedule(static)
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				VectorCell currentVc = prevVelocityField.cell[i][j][k];
				glm::vec3 value = currentVc.get() + force * (float)dt;
				prevVelocityField.cell[i][j][k].updateCell(value);
			}
		}
	}
}
void Solver::addForce(int i, int j, int k, glm::vec3 force) {
	VectorCell currentVc = prevVelocityField.cell[i][j][k];
	glm::vec3 value = currentVc.get() + force * (float)dt;
	prevVelocityField.cell[i][j][k].updateCell(value);
}
void Solver::addSource(double source) {
	int nx = (int)prevRhoField.getNumber().x;
	int ny = (int)prevRhoField.getNumber().y;
	int nz = (int)prevRhoField.getNumber().z;
#pragma omp parallel for schedule(static)
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				ScalarCell currentSc = prevRhoField.cell[i][j][k];
				double value = currentSc.get() + source * dt;
				prevRhoField.cell[i][j][k].updateCell(value);
			}
		}
	}
}
void Solver::addSource(int i, int j, int k, double source) {
	ScalarCell currentSc = prevRhoField.cell[i][j][k];
	double value = currentSc.get() + source * dt;
	prevRhoField.cell[i][j][k].updateCell(value);
}
glm::vec3 Solver::tracePrevPos(int i, int j, int k) {
	VectorCell vc = prevVelocityField.cell[i][j][k];
	glm::vec3 prevPos = trace(prevVelocityField, vc, i, j, k, dt);
	return prevPos;
}
void Solver::transportVector() {
	int nx = (int)prevVelocityField.getNumber().x;
	int ny = (int)prevVelocityField.getNumber().y;
	int nz = (int)prevVelocityField.getNumber().z;
#pragma omp parallel for schedule(static)
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				glm::vec3 prevPos = tracePrevPos(i, j, k);
				nextVelocityField.cell[i][j][k].updateCell(interpolate(prevVelocityField, prevPos));
			}
		}
	}
	
}

void Solver::transportScalar() {
	int nx = (int)prevRhoField.getNumber().x;
	int ny = (int)prevRhoField.getNumber().y;
	int nz = (int)prevRhoField.getNumber().z;
#pragma omp parallel for schedule(static)
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				glm::vec3 prevPos = tracePrevPos(i, j, k);
				nextRhoField.cell[i][j][k].updateCell(interpolate(prevRhoField, prevPos));
			}
		}
	}
	
}
void Solver::diffuseVectorField(double viscosity) {
	velocityBoundaryCondition(nextVelocityField);
	int nx = (int)prevVelocityField.getNumber().x;
	int ny = (int)prevVelocityField.getNumber().y;
	int nz = (int)prevVelocityField.getNumber().z;

	double cellSize = prevVelocityField.cell[0][0][0].getCellSize().x;
	double alpha = viscosity * dt / (cellSize * cellSize);
	velocityBoundaryCondition(prevVelocityField);

	
	// Jacobi iteration
	for (int n = 0; n < 30; n++) {
#pragma omp parallel for schedule(static)
		for (int i = 1; i < nx - 1; i++) {
			for (int j = 1; j < ny - 1; j++) {
				for (int k = 1; k < nz - 1; k++) {
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
					jcbVelocityField.cell[i][j][k].updateCell(updateVector);

				}
			}
		}
		nextVelocityField.cell.swap(jcbVelocityField.cell);
		velocityBoundaryCondition(nextVelocityField);
	}
	
}
void Solver::diffuseScalarField(double diffusion) {
	neumannBoundaryCondition(nextRhoField);
	int nx = (int)prevRhoField.getNumber().x;
	int ny = (int)prevRhoField.getNumber().y;
	int nz = (int)prevRhoField.getNumber().z;

	double cellSize = prevRhoField.cell[0][0][0].getCellSize().x;
	double alpha = diffusion * dt / (cellSize * cellSize);

	neumannBoundaryCondition(prevRhoField);

	// Jacobi iteration
	for (int n = 0; n < 30; n++) {
#pragma omp parallel for schedule(static)
		for (int i = 1; i < nx - 1; i++) {
			for (int j = 1; j < ny - 1; j++) {
				for (int k = 1; k < nz - 1; k++) {
					vector<vector<vector<ScalarCell>>>& nsc = nextRhoField.cell;
					vector<vector<vector<ScalarCell>>>& psc = prevRhoField.cell;
					double laplaccianSca = laplaccian(nextRhoField, i, j, k);
					double ns = psc[i][j][k].get() + alpha * (cellSize * cellSize * laplaccianSca + 6 * nsc[i][j][k].get());
					ns = ns / (1 + 6 * alpha);
					jcbRhoField.cell[i][j][k].updateCell(ns);
				}
			}
		}
		nextRhoField.cell.swap(jcbRhoField.cell);
		neumannBoundaryCondition(nextRhoField);
	}
}
void Solver::project(){

	int nx = (int)prevVelocityField.getNumber().x;
	int ny = (int)prevVelocityField.getNumber().y;
	int nz = (int)prevVelocityField.getNumber().z;
	double cellSize = prevVelocityField.cell[0][0][0].getCellSize().x;

	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				prevPressureField.cell[i][j][k].updateCell(0);
			}
		}
	}

	velocityBoundaryCondition(prevVelocityField);
#pragma omp parallel for schedule(static)
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				//divergence를 저장하는 버퍼로 잠시 nextPressureField를 재사용 (깊은 복사 줄이기 위함)
				nextPressureField.cell[i][j][k].updateCell(divergence(prevVelocityField, i, j, k));
			}
		}
	}
	// Jacobi iteration
	for (int n = 0; n < 30; n++) {
#pragma omp parallel for schedule(static)
		for (int i = 1; i < nx - 1; i++) {
			for (int j = 1; j < ny - 1; j++) {
				for (int k = 1; k < nz - 1; k++) {
					double neighbor = neighborSum(prevPressureField, i, j, k);
					double value = neighbor - nextPressureField.cell[i][j][k].get() * cellSize * cellSize;
					value = value / 6;
					jcbPressureField.cell[i][j][k].updateCell(value);
				}
			}
		}
		prevPressureField.cell.swap(jcbPressureField.cell);
		neumannBoundaryCondition(prevPressureField);
	}
#pragma omp parallel for schedule(static)
	for (int i = 1; i < nx - 1; i++) {
		for (int j = 1; j < ny - 1; j++) {
			for (int k = 1; k < nz - 1; k++) {
				glm::vec3 value = prevVelocityField.cell[i][j][k].get() - gradient(prevPressureField, i, j, k);
				nextVelocityField.cell[i][j][k].updateCell(value);
			}
		}
	}
	prevVelocityField.cell.swap(nextVelocityField.cell);
	velocityBoundaryCondition(prevVelocityField);
}

void Solver::solveVectorField(glm::vec3 force, double viscosity){
	addForce(force);
	velocityBoundaryCondition(prevVelocityField);
	transportVector();
	velocityBoundaryCondition(nextVelocityField);
	prevVelocityField.cell.swap(nextVelocityField.cell);
	diffuseVectorField(viscosity);
	prevVelocityField.cell.swap(nextVelocityField.cell);
	project();
}
void Solver::solveScalarField(double source, double diffusion) {
	addSource(source);
	neumannBoundaryCondition(prevRhoField);
	transportScalar();
	neumannBoundaryCondition(nextRhoField);
	prevRhoField.cell.swap(nextRhoField.cell);
	diffuseScalarField(diffusion);
	prevRhoField.cell.swap(nextRhoField.cell);
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
#pragma omp parallel for schedule(static)
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
				addForce(x, y, z, forcePerSec * (float)weight);
				addSource(x, y, z, densityPerSec * weight);
			}
		}
	}
	neumannBoundaryCondition(prevRhoField);
	velocityBoundaryCondition(prevVelocityField);
}

glm::vec3 Solver::getCellPos(int i, int j, int k) {
	return prevRhoField.cell[i][j][k].getPos();
}
glm::vec3 Solver::getCellSize() {
	return prevRhoField.cell[0][0][0].getCellSize();
}
glm::vec3 Solver::getCenter() {
	glm::vec3 origin = prevRhoField.getOrigin();
	glm::vec3 idxCenter = prevRhoField.getNumber() / (float)2.0;
	glm::vec3 cellSize = prevRhoField.cell[0][0][0].getCellSize();
	glm::vec3 center = origin + idxCenter * cellSize;
	center.z = -origin.z;
	return center;
}
double Solver::getRho(int i, int j, int k) {
	return prevRhoField.cell[i][j][k].get();
}