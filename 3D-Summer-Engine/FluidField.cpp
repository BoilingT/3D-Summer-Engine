#include "FluidField.h"

//Fill the cellgrid with the corresponding x- and y-values
void FluidField::Init() {
	glm::mat4 modelM = glm::mat4(1.0f);
	glm::mat4 viewM = glm::mat4(1.0f);
	glm::mat4 projectionM = glm::mat4(1.0f);

	projectionM = glm::ortho(0.0f, (float)m_WIDTH, 0.0f, (float)m_HEIGHT, -1000.0f, 1000.0f);
	
	m_visualise_grid_shader->use();
	m_visualise_grid_shader->setMat4f("view", viewM);
	m_visualise_grid_shader->setMat4f("projection", projectionM);
}

void FluidField::Draw(glm::vec3 origin) {
	m_shader->use();
	glm::mat4 projectionM = glm::mat4(1.0f);
	m_shader->setMat4f("projection", projectionM);
}

void FluidField::DrawCellField(glm::vec3 o) {
	m_visualise_grid_shader->use();
	
	int uniformLocation = glGetUniformLocation(m_visualise_grid_shader->getID(), "resolution");
	glUniform1i(uniformLocation, m_resolution);
	
	m_quad->transform.dim = glm::vec3(m_WIDTH / (float)m_fieldWidth, m_HEIGHT / (float)m_fieldWidth, 0.0f);
	m_quad->transform.pos.x = o.x + m_quad->transform.dim.x / 2.f + m_quad->transform.dim.x * 0;
	m_quad->transform.pos.y = o.y + m_quad->transform.dim.y / 2.f + m_quad->transform.dim.y * 0;
	for (unsigned int r = 0; r < m_fieldWidth; r++)
	{
		for (unsigned int c = 0; c < m_fieldWidth; c++)
		{
			//glUniform4f(cLocation, (c + 1.f) / m_fieldWidth, 0.0f, (r + 1.f) / m_fieldWidth, 1.0f);
			//Rect m_quad = Quads[r * gridWidth + c];
			//m_quad->transform.pos.x = o.x + m_quad->transform.dim.x / 2.f + m_quad->transform.dim.x * c;
			//m_quad->transform.pos.y = o.y + m_quad->transform.dim.y / 2.f + m_quad->transform.dim.y * r;
			//glm::vec3 pos = glm::vec3(o.x + m_quad->transform.dim.x / 2.f + m_quad->transform.dim.x * c, o.y + m_quad->transform.dim.y / 2.f + m_quad->transform.dim.y * r, 0.0f);
			//m_translations[r * m_fieldWidth + c] = pos;
			m_translations[(double) r * m_fieldWidth + (double) c] = glm::vec2(c, r);
			
			/*glUniform4f(vLocation, 0.0f, 1.0f, 0.0f, 1.0f);
			glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
 			m_line->set(m_quad->transform.pos + offset, m_quad->transform.pos + offset + glm::vec3(20.0f, 20.0f, 0.0f));
			m_line->Draw(*m_visualise_grid_shader);
			m_line->set(m_quad->transform.pos + offset, m_quad->transform.pos + offset + glm::vec3(20.0f, -20.0f, 0.0f));
			m_line->Draw(*m_visualise_grid_shader);*/
			
		}
	}
	/*for (unsigned int i = 0; i < m_resolution; i++)
	{
		int tLocation = glGetUniformLocation(m_visualise_grid_shader->getID(), ("offsets[" + std::to_string(i) + "]").c_str());
		glUniform2f(tLocation, m_translations[i].x, m_translations[i].y);
	}*/
	//m_line->DrawInstanced(*m_visualise_grid_shader, m_translations, m_resolution);
 	m_quad->DrawInstanced(*m_visualise_grid_shader, m_translations.data(), m_resolution);
}

void FluidField::timeStep() {

}