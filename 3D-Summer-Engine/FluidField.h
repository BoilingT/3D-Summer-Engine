#pragma once
#include <iostream>
#include "Shader.h"
#include "Plane.h"
#include "Line.h"

class FluidField
{
private:
	//Field
	float		 m_WIDTH, m_HEIGHT;
	const int	 m_resolution;
	const int	 m_fieldWidth;

	//Forcefield containing vector information
	float*					m_cellField;
	std::vector<glm::vec2>	m_translations;
	Plane*					m_quad;
	Line*					m_line;

	//Initialize the grid
	void Init();

public:
	FluidField(const float WIDTH, const float HEIGHT, const int resolution) : m_resolution(resolution), m_fieldWidth(sqrt(resolution)) {
		m_WIDTH = WIDTH;
		m_HEIGHT = HEIGHT;
		m_quad = new Plane(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
		//m_line = new Line(0.0f, 0.0f, 0.0f, 0.0f);
		m_translations.resize(resolution);
		Init();
	}

	~FluidField() {
		delete(m_quad);
		delete(m_line);
	}

	//Move forward in time, update values
	void timeStep(); 
	//Draw the fluid
	void Draw(glm::vec3 origin); //Should be used with a template?
	//Visualize the Cell Field
	void DrawCellField(glm::vec3 origin, Shader* fieldVisualiser);
};
