#include "FluidField.h"

void FluidField::Init() {

}

void FluidField::Draw(glm::vec3 origin) {

}

void FluidField::DrawCellField(glm::vec3 o, Shader* visualizeShader) {
	Plane q(glm::vec3(0.0f), glm::vec3(3.0f), glm::vec3(0.0f));
	for (unsigned int r = 0; r < m_fieldWidth; r++)
	{
		for (unsigned int c = 0; c < m_fieldWidth; c++)
		{
			int vLocation = glGetUniformLocation(visualizeShader->getID(), "color");
			glUniform4f(vLocation, (c + 1.f) / m_fieldWidth, 0.0f, (r + 1.f) / m_fieldWidth, 1.0f);
			//Plane q = Quads[r * gridWidth + c];
			q.transform.dim = glm::vec3(m_WIDTH / (float)m_fieldWidth, m_HEIGHT / (float)m_fieldWidth, 0.0f);
			q.transform.pos.x = o.x + q.transform.dim.x / 2 + q.transform.dim.x * c;
			q.transform.pos.y = o.y - q.transform.dim.y / 2.f - q.transform.dim.y * r;
			q.Draw(*visualizeShader);
		}
	}
}

void FluidField::timeStep() {

}