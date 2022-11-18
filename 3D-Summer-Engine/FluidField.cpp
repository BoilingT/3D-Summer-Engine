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

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind framebuffer
	if (target == nullptr || target->fbo == NULL)
	{
		//glViewport(0, 0, m_WIDTH, m_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}
	else
	{
		//glViewport(0, 0, target->width, target->height);
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
	float time = dt;
	project(time);
	advect(time);
	//diffuse(time);
	//addForces(time);
	//blit(m_velocity_buffer->writeBuffer(), &m_gradient_subtraction_shader);
	//m_velocity_buffer->swap();
	////Boundary conditions
	
	////////////////////////////////////////////////////////////////////////////////////

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

void FluidField::advect(float dt) {
	//Advection
	m_advection_shader.use();
	int timestepLoc = m_advection_shader.uniforms["timestep"];
	int uLoc = m_advection_shader.uniforms["u"];
	int xLoc = m_advection_shader.uniforms["x"];
	int texelLoc = m_advection_shader.uniforms["texelSize"];
	int velocityId = m_velocity_buffer->readBuffer()->setTexture(0);
	glUniform1f(timestepLoc, dt); //timestep = m_timestep
	glUniform1i(uLoc, velocityId); //u = velocity texture
	glUniform1i(xLoc, velocityId); //x = quantity texture
	glUniform2f(texelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);

	int linearFilteringLoc = m_advection_shader.uniforms["linearFiltering"];
	bool linearFiltering = 1;
	glUniform1i(linearFilteringLoc, linearFiltering);
	blit(m_velocity_buffer->writeBuffer(), &m_advection_shader);
	m_velocity_buffer->swap();
	
	//Advect dye
	glUniform1i(uLoc, m_velocity_buffer->readBuffer()->setTexture(0)); //u = velocity texture
	glUniform1i(xLoc, m_dye_buffer->readBuffer()->setTexture(1)); //x = quantity scalar texture
	glUniform2f(texelLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
	blit(m_dye_buffer->writeBuffer(), &m_advection_shader);
	m_dye_buffer->swap();
}

//Diffusion, by using jacobi iterations
void FluidField::diffuse(float dt) {
	float viscosity = 0.f;
	int jacobiIterations = 20;

	if (viscosity > 0) {
		int xLoc = m_jacobi_iteration_shader.uniforms["x"];
		int bLoc = m_jacobi_iteration_shader.uniforms["b"];
		float alphaLoc = m_jacobi_iteration_shader.uniforms["alpha"];
		float rBetaLoc = m_jacobi_iteration_shader.uniforms["rBeta"];
		int xTexelLoc = m_jacobi_iteration_shader.uniforms["xTx"];
		// Velocity
		m_jacobi_iteration_shader.use();
		glUniform2f(xTexelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
		glUniform1i(xLoc, m_velocity_buffer->readBuffer()->setTexture(0)); // x = b = u (velocity)
		glUniform1i(bLoc, m_velocity_buffer->readBuffer()->setTexture(1));
		float alpha = pow(1, 2) / (dt * viscosity);
		float rBeta = 1 / (4 + alpha);
		glUniform1f(alphaLoc, alpha);  //Alpha = pow(dx, 2)/t
		glUniform1f(rBetaLoc, 1 / (4 + alpha)); //rBeta = 1/(4+Alpha)

		for (unsigned int k = 0; k < jacobiIterations; k++) //20 to 50 iterations
		{
			blit(m_velocity_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_velocity_buffer->swap();
		}
		//Diffusion
		// Dye

		glUniform2f(xTexelLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
		glUniform1i(xLoc, m_dye_buffer->readBuffer()->setTexture(0)); // x = b = u (velocity)
		glUniform1i(bLoc, m_dye_buffer->readBuffer()->setTexture(1));
		//float alpha = pow(1, 2) / m_timestep;
		//float rBeta = 1 / (4 + alpha);
		glUniform1f(alphaLoc, alpha);  //Alpha = pow(dx, 2)/t
		glUniform1f(rBetaLoc, 1 / (4 + alpha)); //rBeta = 1/(4+Alpha)

		for (unsigned int k = 0; k < jacobiIterations; k++) //20 to 50 iterations
		{
			blit(m_dye_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_dye_buffer->swap();
		}
	}
}

void FluidField::addForces(float dt) {
	//Force Application
	//blit(m_velocity_buffer->writeBuffer(), &m_force_shader);
	//m_velocity_buffer->swap();
}

void FluidField::project(float dt) {
	//Projection, by removing any divergence
	////Solve divergence
	{
		m_divergence_shader.use();
		int wLoc = m_divergence_shader.uniforms["w"];
		int texelSizeLoc = m_divergence_shader.uniforms["texelSize"];
		glUniform1i(wLoc, m_velocity_buffer->readBuffer()->setTexture(0));
		glUniform2f(texelSizeLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
		blit(m_divergence_buffer, &m_divergence_shader);
	}

	//Clear
	{
		m_clear_shader.use();
		int valueLoc = m_clear_shader.uniforms["value"];
		int textureLoc = m_clear_shader.uniforms["uTexture"];
		int texSizeLoc = m_clear_shader.uniforms["texelSize"];
		glUniform1f(valueLoc, 0.8f);
		glUniform1i(textureLoc, m_pressure_buffer->readBuffer()->setTexture(0));
		glUniform2f(texSizeLoc, m_pressure_buffer->readBuffer()->texelSizeX, m_pressure_buffer->readBuffer()->texelSizeY);
		blit(m_pressure_buffer->writeBuffer(), &m_clear_shader);
		m_pressure_buffer->swap();
	}

	////Solve Pressure
	{
		m_jacobi_iteration_shader.use();
		int xLoc = m_jacobi_iteration_shader.uniforms["x"];
		int bLoc = m_jacobi_iteration_shader.uniforms["b"];
		int xTexelLoc = m_jacobi_iteration_shader.uniforms["texelSize"];
		int alphaLoc = m_jacobi_iteration_shader.uniforms["alpha"];
		int rBetaLoc = m_jacobi_iteration_shader.uniforms["rBeta"];


		float alpha = -pow(1, 2); //Alpha = -pow(x,2)
		float rBeta = 1.0f / 4.0f; //rBeta = 1/4
		glUniform1f(alphaLoc, alpha);
		glUniform1f(rBetaLoc, rBeta);
		glUniform2f(xTexelLoc, m_pressure_buffer->readBuffer()->texelSizeX, m_pressure_buffer->readBuffer()->texelSizeY);
		glUniform1i(bLoc, m_divergence_buffer->setTexture(0));	//b = gradient of w (divergent velocity)
		for (unsigned int k = 0; k < 40; k++)
		{
			glUniform1i(xLoc, m_pressure_buffer->readBuffer()->setTexture(1)); //x = p (pressure)
			blit(m_pressure_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_pressure_buffer->swap();
		}
	}
	{
		////Subtract Gradient of pressure field from velocity field to achieve divergence free velocity
		m_gradient_subtraction_shader.use();
		int xTexelLoc = m_gradient_subtraction_shader.uniforms["texelSize"];
		int pLoc = m_gradient_subtraction_shader.uniforms["p"];
		int wLoc = m_gradient_subtraction_shader.uniforms["w"];
		glUniform1i(pLoc, m_pressure_buffer->readBuffer()->setTexture(0)); //p = pressure
		glUniform1i(wLoc, m_velocity_buffer->readBuffer()->setTexture(1)); //w = divergent velocity
		glUniform2f(xTexelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
		blit(m_velocity_buffer->writeBuffer(), &m_gradient_subtraction_shader);
		m_velocity_buffer->swap();
	}
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
	//glBindTexture(GL_TEXTURE_2D, m_divergence_buffer->texture);
	glBindTexture(GL_TEXTURE_2D, *m_current_read_buffer);
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

void FluidField::splat(glm::vec2 pos, float r) {
	std::cout << "SPLAT!" << std::endl;
	m_splat_shader.use();
	//Uniforms
	int uTargetLoc = m_splat_shader.uniforms["uTarget"];
	int uPointLoc = m_splat_shader.uniforms["point"];
	int uColorLoc = m_splat_shader.uniforms["color"];
	int uRadiusLoc = m_splat_shader.uniforms["radius"];
	int uTexLoc = m_splat_shader.uniforms["texelSize"];
	//int uColorLoc = glGetUniformLocation(m_splat_shader.getID(), "color");
	//glBindTexture(GL_TEXTURE_2D, m_dye_buffer->readBuffer()->texture);

	float force = 6000.0f;
	glUniform1i(uTargetLoc, m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(uPointLoc, pos.x, pos.y);
	glUniform3f(uColorLoc, m_mouse.texcoord_delta.x * force, m_mouse.texcoord_delta.y * force, 0.0f);
	glUniform1f(uRadiusLoc, r);
	glUniform2f(uTexLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	blit(m_velocity_buffer->writeBuffer(), &m_splat_shader);
	m_velocity_buffer->swap();

	glUniform1i(m_splat_shader.uniforms["uTarget"], m_dye_buffer->readBuffer()->setTexture(0));
	glUniform2f(uTexLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
	glm::vec3 color = glm::vec3(m_mouse.texcoord_delta.x * force/100.f, m_mouse.texcoord_delta.y * force / 100.f, 0.3f);

	glUniform3f(uColorLoc, abs(color.r), abs(color.g), abs(color.b + (color.r+color.g)/8.f));
	blit(m_dye_buffer->writeBuffer(), &m_splat_shader);
	m_dye_buffer->swap();
}

void FluidField::updateMouse(double* mouseX, double* mouseY, bool* mouse_down)
{
	/*
	m_mouse_down = false;
	m_mouseTravelX = *mouseX - m_prevMouseX;
	m_mouse_down = *mouse_down;
	m_prevMouseX = *mouseX;
	m_prevMouseY = *mouseY;
	if (m_mouse_down && (abs(m_mouseTravelX) > 0 || abs(m_mouseTravelY) > 0))
	{
	}
	*/
	m_mouse.update(*mouseX, *mouseY, *mouse_down);
	if (m_mouse.down && (abs(m_mouse.window_delta.x) > 0 || abs(m_mouse.window_delta.y) > 0))
	{
		std::cout << "X: " << m_mouse.texcoord_pos.x << " Y: " << m_mouse.texcoord_pos.y << " down:" << m_mouse.down << std::endl;
		splat(m_mouse.texcoord_pos, 0.15f);
	}
}
