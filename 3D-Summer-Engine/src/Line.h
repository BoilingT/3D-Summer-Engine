#pragma once
#include "Object.h"

class Line : public Object
{
private:
	glm::vec3 m_start, m_end;
	float *m_vertices;
	float m_width;

	void Init()
	{
		transform.dim = glm::vec3(1.0f);
		transform.rot = glm::vec3(0.0f);
		updateVertices();
	}

	void updateVertices()
	{
		transform.pos = m_start;

		float vertices[6] = {
			m_end.x - m_start.x, m_end.y - m_start.y, m_end.z - m_start.z,
			m_start.x, m_start.y, m_start.z
		};

		m_vertices = vertices;
		SetupMesh(m_vertices, sizeof(vertices));
	}

public:
	void Draw(Shader &shader)
	{
		GLfloat tempWidth;
		glGetFloatv(GL_LINE_WIDTH, &tempWidth);

		glLineWidth(m_width);
		Object::Draw(shader);
		glLineWidth(tempWidth);
	}

	Line()
	{
		set(glm::vec3(0.0f), glm::vec3(0.0f));
		Init();
	}

	Line(glm::vec3 start, glm::vec3 end, float w)
	{
		set(start, end);
		m_width = w;
		Init();
	}

	Line(glm::vec2 start, glm::vec2 end, float w)
	{
		set(start, end);
		m_width = w;
		Init();
	}

	Line(float x1, float y1, float x2, float y2, float w)
	{
		set(x1, y1, x2, y2);
		m_width = w;
		Init();
	}

	Line(float x1, float y1, float z1, float x2, float y2, float z2, float w)
	{
		set(x1, y1, z1, x2, y2, z2);
		m_width = w;
		Init();
	}

	void set(glm::vec3 start, glm::vec3 end)
	{
		m_start = start;
		m_end = end;
		updateVertices();
	}

	void set(glm::vec2 start, glm::vec2 end)
	{
		m_start = glm::vec3(start.x, start.y, 0.0f);
		m_end = glm::vec3(end.x, end.y, 0.0f);
		updateVertices();
	}

	void set(float x1, float y1, float x2, float y2)
	{
		m_start = glm::vec3(x1, y1, 0.0f);
		m_end = glm::vec3(x2, y2, 0.0f);
		updateVertices();
	}

	void set(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		m_start = glm::vec3(x1, y1, z1);
		m_end = glm::vec3(x2, y2, z2);
		updateVertices();
	}

	glm::vec3 start()
	{
		return m_start;
	}

	void start(float x, float y, float z)
	{
		m_start = glm::vec3(x, y, z);
		updateVertices();
	}

	void start(float x, float y)
	{
		m_start = glm::vec3(x, y, m_start.z);
		updateVertices();
	}

	glm::vec3 end()
	{
		return m_end;
	}

	void end(float x, float y, float z)
	{
		m_end = glm::vec3(x, y, z);
		updateVertices();
	}

	void end(float x, float y)
	{
		m_end = glm::vec3(x, y, m_end.z);
		updateVertices();
	}

	void width(float w)
	{
		m_width = w;
		updateVertices();
	}

	double length()
	{
		glm::vec3 v = m_end - m_start;
		return v.length();
	}
};

