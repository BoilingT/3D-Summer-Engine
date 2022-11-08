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

	m_primary_shader->use();
	m_primary_shader->setMat4f("view", viewM);
	m_primary_shader->setMat4f("projection", projectionM);


	m_compute_shader = new Compute(p_COMPUTE_SHADER, glm::vec2(m_fieldWidth, m_fieldWidth));
	m_compute_shader->use();
	//glDeleteTextures(1, &compute_texture);

	std::vector<unsigned char> quantityImage((m_fieldWidth * m_fieldWidth) * 3 /* bytes per pixel */);
	unsigned int coordinate = (m_fieldWidth/2 + (m_fieldWidth/2 * m_fieldWidth)) * 3;
	quantityImage[coordinate + 0] = 255 * 0.5f;;
	quantityImage[coordinate + 1] = 255 * 0.5f;
	quantityImage[coordinate + 2] = 255 * 0.5f;

	std::vector<unsigned char> pressureImage((m_fieldWidth * m_fieldWidth) * 3 /* bytes per pixel */);
	std::vector<unsigned char> velocityImage((m_fieldWidth * m_fieldWidth) * 3 /* bytes per pixel */);

	for (unsigned int row = 0; row < m_fieldWidth; row++)
	{
		for (unsigned int col = 0; col < m_fieldWidth; col++)
		{
			unsigned int location = (col + (row * m_fieldWidth)) * 3;
			if (row <= m_fieldWidth / 2)
			{
				glm::vec3 v = glm::vec3(0.0f, 1.0f, 0.0f);
				v *= 255;
				velocityImage[location + 0] = v.x;	 // X
				velocityImage[location + 1] = v.y;	 // Y
				velocityImage[location + 2] = v.z;	 // Z
			}
			else {
				glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f);
				
				velocityImage[location + 0] = v.x;	 // X
				velocityImage[location + 1] = v.y;	 // Y
				velocityImage[location + 2] = v.z;	 // Z
			}
			//velocityImage[location + 0] = (float) col/m_fieldWidth * 255;	 // R
			//velocityImage[location + 1] = 0;								 // G
			//velocityImage[location + 2] = (float) row / m_fieldWidth * 255;	 // B
		}
 	}
	/*int c = 0;
	int r = 0;
	unsigned int location = (c + (r * m_fieldWidth))*3;
	image[location + 0] = 1; // R
	image[location + 1] = 1; // G
	image[location + 2] = 0; // B*/
	m_compute_shader->setValues(quantityImage.data());
}

void FluidField::Draw(glm::vec3 origin) {
	m_primary_shader->use();
	float time = glfwGetTime();
	int uTimeLocation = glGetUniformLocation(m_primary_shader->getID(), "u_time");
	glUniform1f(uTimeLocation, time);
	
	m_compute_shader->use();
	m_compute_shader->dispatch();
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	uTimeLocation = glGetUniformLocation(m_compute_shader->getID(), "t");
	glUniform1f(uTimeLocation, time);
	//m_compute_shader->setFloat("textureWidth", m_fieldWidth);
	m_fieldQuad->setTexture(m_compute_shader->getTexture());
	m_fieldQuad->Draw(*m_primary_shader);
}

//Simulate for the next timestep
void FluidField::timeStep() {
	
}

//Draw a visual representation of the dimensions of a grid containing data
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

void FluidField::updateMouse(double* mouseX, double* mouseY, bool* mouse_down)
{
	m_mouse_down = false;
	m_mouseTravelX = *mouseX - m_prevMouseX;
	m_mouse_down = *mouse_down;
	m_prevMouseX = *mouseX;
	m_prevMouseY = *mouseY;

	if (m_mouse_down && (abs(m_mouseTravelX) > 0 || abs(m_mouseTravelY) > 0))
	{
		std::cout << "X: " << m_prevMouseX << " Y: " << m_prevMouseY << " down:" << m_mouse_down << std::endl;
	}
}
