#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include <vector>

#include "Shader.h"
#include "camera.h"
#include "Solver.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void injectionSmoke(GLFWwindow* window, Solver& solver);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool rightMouseDown = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct Data {
	glm::vec3 offset;
	float density;
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "StableFluid", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to Create Window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD\n";
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	Shader fluidShader("Shader/fluid_color.vs", "Shader/fluid_color.fs");
	glFrontFace(GL_CCW);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double solverDt = 1.0 / 60.0;
	Solver solver(solverDt);

	float s = 10.5f;

	// 8°³ ²ÀÁþÁ¡ (x,y,z)
	float vertices[] = {
		-0.5f, -0.5f, -0.5f, // 0
		 0.5f, -0.5f, -0.5f, // 1
		 0.5f,  0.5f, -0.5f, // 2
		-0.5f,  0.5f, -0.5f, // 3
		-0.5f, -0.5f,  0.5f, // 4
		 0.5f, -0.5f,  0.5f, // 5
		0.5f,  0.5f,  0.5f, // 6
		-0.5f,  0.5f,  0.5f  // 7
	};

	// 12 triangles = 36 indices (CCW ±âÁØ)
	unsigned int indices[] = {
		// back (-z)
		0, 1, 2,  2, 3, 0,
		// front (+z)
		4, 5, 6,  6, 7, 4,
		// left (-x)
		0, 3, 7,  7, 4, 0,
		// right (+x)
		1, 5, 6,  6, 2, 1,
		// top (+y)
		3, 2, 6,  6, 7, 3,
		// bottom (-y)
		0, 4, 5,  5, 1, 0
	};

	unsigned int VBO, VAO, EBO;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	int gridNx = (int)solver.getGridNumber().x;
	int gridNy = (int)solver.getGridNumber().y;
	int gridNz = (int)solver.getGridNumber().z;

	vector<Data> instanceData;
	instanceData.reserve((size_t)gridNx * (size_t)gridNy * (size_t)gridNz);


	for (int i = 1; i < gridNx - 1; i++) {
		for (int j = 1; j < gridNy - 1; j++) {
			for (int k = 1; k < gridNz - 1; k++) {
				glm::vec3 cellPos = solver.getCellPos(i, j, k);
				double rho = solver.getRho(i, j, k);
				Data data;
				data.offset = cellPos;
				data.density = rho;
				instanceData.push_back(data);

			}
		}
	}

	glBindVertexArray(VAO);

	unsigned int instanceVBO = 0;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(Data), instanceData.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Data), (void*)offsetof(Data, offset));
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Data), (void*)offsetof(Data, density));
	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	camera.Position = solver.getCenter();

	const GLsizei instanceCount = (GLsizei)instanceData.size();

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		deltaTime = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		fluidShader.use();

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			injectionSmoke(window, solver);
		}

		glm::vec3 forcePerSec(0.0f, 0.0f, 0.0f);
		double viscosity = 0.0001;
		double diffusion = 1;
		double sourcePerSec = 0.0;

		solver.solveVectorField(forcePerSec, viscosity);
		solver.solveScalarField(sourcePerSec, diffusion);

		glBindVertexArray(VAO);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0);

		fluidShader.setMat4("projection", projection);
		fluidShader.setMat4("view", view);
		fluidShader.setMat4("model", model);
		
		glm::vec3 cellSize = solver.getCellSize();
		fluidShader.setVec3("cellSize", cellSize);
		int idx = 0;
		for (int i = 1; i < gridNx - 1; i++) {
			for (int j = 1; j < gridNy - 1; j++) {
				for (int k = 1; k < gridNz - 1; k++) {
					instanceData[idx].density = (float)solver.getRho(i, j, k);
					idx++;
				}
			}
		}
		glDepthMask(GL_FALSE);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(Data), instanceData.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instanceCount);
		glDepthMask(GL_TRUE);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button != GLFW_MOUSE_BUTTON_RIGHT) {
		return;
	}

	if (action == GLFW_PRESS) {
		rightMouseDown = true;
		firstMouse = true; // ´­·¶À» ¶§ Ä«¸Þ¶ó Æ¦ ¹æÁö
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (action == GLFW_RELEASE) {
		rightMouseDown = false;
		firstMouse = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
void mouse_callback(GLFWwindow* window, double xposln, double yposln) {
	
	if (!rightMouseDown) {
		return;
	}
	float xpos = static_cast<float>(xposln);
	float ypos = static_cast<float>(yposln);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
static bool intersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& bmin, const glm::vec3& bmax, float& tHit) {
	float tmin = -FLT_MAX;
	float tmax = FLT_MAX;
	for (int axis = 0; axis < 3; axis++) {
		float o = rayOrigin[axis];
		float d = rayDir[axis];
		float mn = bmin[axis];
		float mx = bmax[axis];

		if (fabs(d) < 1e-8f) {
			if (o < mn || o > mx) return false;
		}
		else {
			float invD = 1.0f / d;
			float t1 = (mn - o) * invD;
			float t2 = (mx - o) * invD;
			if (t1 > t2) {
				swap(t1, t2);
			}
			tmin = max(tmin, t1);
			tmax = min(tmax, t2);
			if (tmin > tmax) return false;
		}

	}
	if (tmax < 0.0f) return false;
	tHit = (tmin >= 0.0f) ? tmin : tmax;
	return true;
}
void injectionSmoke(GLFWwindow* window, Solver& solver) {
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);

	int winW, winH;
	glfwGetWindowSize(window, &winW, &winH);
	if (winW <= 0 || winH <= 0) return;

	float xNdc = (float)(2.0 * mx / (double)winW - 1.0);
	float yNdc = (float)(1.0 - 2.0 * my / (double)winH);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)winW / (float)winH, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();

	glm::mat4 invVP = glm::inverse(projection * view);

	glm::vec4 pNear = invVP * glm::vec4(xNdc, yNdc, -1.0f, 1.0f);
	glm::vec4 pFar = invVP * glm::vec4(xNdc, yNdc, 1.0f, 1.0f);

	pNear /= pNear.w;
	pFar /= pFar.w;

	glm::vec3 rayOrigin = glm::vec3(pNear);
	glm::vec3 rayDir = glm::normalize(glm::vec3(pFar - pNear));

	glm::vec3 cellSize = solver.getCellSize();
	glm::vec3 number = solver.getGridNumber();

	glm::vec3 origin = solver.getCellPos(0, 0, 0) - 0.5f * cellSize;
	glm::vec3 bmin = origin;
	glm::vec3 bmax = origin + number * cellSize;

	float tHit = 0.0f;
	if (!intersectRayAABB(rayOrigin, rayDir, bmin, bmax, tHit)) {
		return;
	}

	glm::vec3 hitPos = rayOrigin + tHit * rayDir;
	glm::vec3 rel = (hitPos - origin) / cellSize;

	int nx = (int)number.x;
	int ny = (int)number.y;
	int nz = (int)number.z;

	int i = (int)floor(rel.x);
	int j = (int)floor(rel.y);
	int k = (int)floor(rel.z);

	i = clamp(i, 1, nx - 2);
	j = clamp(j, 1, ny - 2);
	k = clamp(k, 1, nz - 2);

	int radius = 3;
	double densityPerSec = 60.0;
	glm::vec3 forcePerSec(0.0f, 40.0f, 0.0f);

	solver.injection(i, j, k, radius, densityPerSec, forcePerSec);
}