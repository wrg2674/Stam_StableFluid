#version 430 core

flat in float vDensity;

out vec4 FragColor;

void main(){
	float rho = vDensity;

	if(vDensity > 1.0f){
		rho = 1.0f;
	}
	if(vDensity <= 0.0f){
		rho = 0.00f;
	}
	FragColor = vec4(1.0f, 1.0f, 1.0f, rho);
}