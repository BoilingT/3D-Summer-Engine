#include "FluidField.h"

void FluidField::Init() {
	//Fill the cellgrid with the corresponding x- and y-values
}

void FluidField::Draw(glm::vec3 origin) {

}

void FluidField::DrawCellField(glm::vec3 o, Shader* visualizeShader) {
	int vLocation = glGetUniformLocation(visualizeShader->getID(), "color");
	for (unsigned int r = 0; r < m_fieldWidth; r++)
	{
		for (unsigned int c = 0; c < m_fieldWidth; c++)
		{
			glUniform4f(vLocation, (c + 1.f) / m_fieldWidth, 0.0f, (r + 1.f) / m_fieldWidth, 1.0f);
			//Plane m_quad = Quads[r * gridWidth + c];
			m_quad->transform.dim = glm::vec3(m_WIDTH / (float)m_fieldWidth, m_HEIGHT / (float)m_fieldWidth, 0.0f);
			m_quad->transform.pos.x = o.x + m_quad->transform.dim.x / 2.f + m_quad->transform.dim.x * c;
			m_quad->transform.pos.y = o.y + m_quad->transform.dim.y / 2.f + m_quad->transform.dim.y * r;
			m_quad->Draw(*visualizeShader);
			/*glUniform4f(vLocation, 0.0f, 1.0f, 0.0f, 1.0f);
			glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
 			m_line->set(m_quad->transform.pos + offset, m_quad->transform.pos + offset + glm::vec3(20.0f, 20.0f, 0.0f));
			m_line->Draw(*visualizeShader);
			m_line->set(m_quad->transform.pos + offset, m_quad->transform.pos + offset + glm::vec3(20.0f, -20.0f, 0.0f));
			m_line->Draw(*visualizeShader);*/
			
		}
	}
}

void FluidField::timeStep() {

}