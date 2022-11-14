#include "FluidField.h"

//Fill the cellgrid with the corresponding x- and y-values
void FluidField::Init() {
	/*glm::mat4 modelM = glm::mat4(1.0f);
	glm::mat4 viewM = glm::mat4(1.0f);
	glm::mat4 projectionM = glm::mat4(1.0f);

	projectionM = glm::ortho(0.0f, (float)m_WIDTH, 0.0f, (float)m_HEIGHT, -1000.0f, 1000.0f);
	
	m_visualise_grid_shader->use();
	m_visualise_grid_shader->setMat4f("view", viewM);
	m_visualise_grid_shader->setMat4f("projection", projectionM);

	m_primary_shader->use();
	m_primary_shader->setMat4f("view", viewM);
	m_primary_shader->setMat4f("projection", projectionM);
	*/

	m_compute_shader = new Compute(p_COMPUTE_SHADER, glm::vec2(m_fieldWidth, m_fieldWidth));
	m_compute_shader->use();
	//glDeleteTextures(1, &compute_texture);

	std::vector<unsigned char> quantityImage((m_fieldWidth * m_fieldWidth) * 3 /* bytes per pixel */);
	unsigned int coordinate = (m_fieldWidth/2 + (m_fieldWidth/2 * m_fieldWidth)) * 3;
	quantityImage[coordinate + 0] = 255 * 0.5f;;
	quantityImage[coordinate + 1] = 0;
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

void FluidField::blit(Framebuffer* target, Shader* shader) {
	glm::mat4 modelM = glm::mat4(1.0f);
	glm::mat4 viewM = glm::mat4(1.0f);
	glm::mat4 projectionM = glm::mat4(1.0f);

	projectionM = glm::ortho(0.0f, (float)m_WIDTH, 0.0f, (float)m_HEIGHT, -1000.0f, 1000.0f);

	shader->use();
	shader->setMat4f("view", viewM);
	shader->setMat4f("projection", projectionM);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind framebuffer
	if (target == nullptr || target->fbo == NULL)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, target->fbo);
	}

	//Draw to framebuffer
	if (!target->status())
	{
		std::cout << "ERROR::BLIT::FRAMEBUFFER::STATUS::INCOMPLETE" << std::endl;
	}

	m_fieldQuad->Draw(*shader);
	int boundBuffer = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundBuffer);
	if (target != nullptr && boundBuffer == target->fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void FluidField::timeStep(float dt) {
	//Order of operations:
	//Advection -> Diffusion -> Force Application -> Projection

	//Advection
	//timestep = m_timestep
	//u = velocity texture
	//x = quantity texture
	//rdx = 1 / gridscale 
	blit(m_velocity_buffer->writeBuffer(), &m_advection_shader);
	m_velocity_buffer->swap();
	//Advect dye
	//timestep = m_timestep
	//u = velocity texture
	//x = quantity scalar texture
	//rdx = 1 / gridscale
	blit(m_dye_buffer->writeBuffer(), &m_advection_shader);
	m_dye_buffer->swap();

	//Diffusion, by using jacobi iterations
	// x = b = u (velocity)
	//Alpha = pow(x, 2)/t
	//rBeta = 1/(4+Alpha)
	for (unsigned int i = 0; i < 50; i++) //20 to 50 iterations
	{
		blit(m_velocity_buffer->writeBuffer(), &m_jacobi_iteration_shader);
		m_velocity_buffer->swap();
	}
	//Force Application / Dye Manipulation
	blit(m_velocity_buffer->writeBuffer(), &m_force_shader);
	m_velocity_buffer->swap();
	//Projection, by removing any divergence
	////Solve Pressure
	//x = p (pressure)
	//b = gradient of w (divergent velocity)
	//Alpha = -pow(x,2)
	//rBeta = 1/4
	for (unsigned int i = 0; i < 20; i++)
	{
		blit(m_pressure_buffer->writeBuffer(), &m_jacobi_iteration_shader);
		m_pressure_buffer->swap();
	}
	////Subtract Gradient of pressure field from velocity field to achieve divergence free velocity
	//p = pressure
	//w = divergent velocity
	blit(m_velocity_buffer->writeBuffer(), &m_gradient_subtraction_shader);
	m_velocity_buffer->swap();
	////Boundary conditions

	/*
	// Curl <- Curl
	//blit(m_curl_buffer, m_curl_shader);
	// Vorticity <- Veloctiy
	//blit(m_velocity_buffer->writeBuffer(), m_vorticity_shader);
	//m_velocity_buffer->swap();
	// Divergence <- Divergence
	blit(m_divergence_buffer, m_divergence_shader);
	// Clear <- Pressure
	
	// Pressure <- Pressure
	for (unsigned int i = 0; i < 1; i++)
	{
		blit(m_pressure_buffer->writeBuffer(), m_jacobi_iteration_shader);
		m_pressure_buffer->swap();
	}
	// Gradient Subtract <- Velocity
	blit(m_velocity_buffer->writeBuffer(), m_gradient_subtraction_shader);
	m_velocity_buffer->swap();
	// Advection <- Velocity
	blit(m_velocity_buffer->writeBuffer(), m_advection_shader);
	m_velocity_buffer->swap();
	// Dye
	blit(m_dye_buffer->writeBuffer(), m_advection_shader);
	m_dye_buffer->swap();
	*/
}

void FluidField::Draw(glm::vec3 origin) {
	
	float time = glfwGetTime();
	
	m_compute_shader->use();
	//glBindTexture(GL_TEXTURE_2D, *m_compute_shader->getTexture());
	m_compute_shader->dispatch();
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	int uTimeLocation = glGetUniformLocation(m_compute_shader->getID(), "t");
	glUniform1f(uTimeLocation, time);
	//m_compute_shader->setFloat("textureWidth", m_fieldWidth);
	//m_fieldQuad->setTexture(m_compute_shader->getTexture());
	
	m_primary_shader->use();
	uTimeLocation = glGetUniformLocation(m_primary_shader->getID(), "u_time");
	glUniform1f(uTimeLocation, time);

	//blit(nullptr, &m_gradient_subtraction_shader);
	glBindTexture(GL_TEXTURE_2D, m_velocity_buffer->readBuffer()->texture);
	blit(nullptr, m_primary_shader);

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
