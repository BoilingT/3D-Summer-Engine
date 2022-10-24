#pragma once
#include <iostream>
#include "Shader.h"
#include "Plane.h"

class FluidField
{
private:
	//Field
	float m_WIDTH, m_HEIGHT;
	int m_resolution;
	float m_fieldWidth;
	//Forcefield containing vector information
	float* m_cellField;

	//Initialize the grid
	void Init();

public:
	FluidField(const float WIDTH, const float HEIGHT, int resolution) {
		m_WIDTH = WIDTH;
		m_HEIGHT = HEIGHT;
		m_resolution = resolution;
		m_fieldWidth = sqrt(resolution);
		Init();
	}

	~FluidField() {

	}

	//Move forward in time, update values
	void timeStep(); 
	//Draw the fluid
	void Draw(glm::vec3 origin); //Should be used with a template?
	//Visualize the Cell Field
	void DrawCellField(glm::vec3 origin, Shader* fieldVisualiser);
};

