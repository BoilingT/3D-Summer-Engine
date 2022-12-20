#include "FluidField.h"

//Fill the cellgrid with the corresponding x- and y-values
void FluidField::Init() {

}

void FluidField::Draw(glm::vec3 origin) {

	m_primary_shader->use();
	glUniform1f(m_primary_shader->uniforms["u_time"], (GLfloat) glfwGetTime());

	glUniform1i(m_primary_shader->uniforms["u_image"], m_dye_buffer->readBuffer()->setTexture(0));
	if (m_current_buffer != nullptr)
	{
		glUniform1i(m_primary_shader->uniforms["u_image_overlay"], m_current_buffer->setTexture(1));
		glUniform2f(m_primary_shader->uniforms["texelSize"], m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
	}
	else
	{
		glUniform1i(m_primary_shader->uniforms["u_image_overlay"], 0);
	}
	blit(nullptr, m_primary_shader);
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
	glClear(GL_COLOR_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		return;
	}

	m_fieldQuad->Draw(*shader);
	int boundBuffer = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundBuffer);
	if (target != nullptr && boundBuffer == target->fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

//Advection -> Diffusion -> Force Application -> Projection
void FluidField::timeStep(float dt) {
	float time = dt * m_timestep_scalar;
	advect(time);
	diffuse(time);
	//addForces(time);
	project(time);
}

void FluidField::advect(float dt) {
	//Advection
	m_advection_shader.use();
	int timestepLoc = m_advection_shader.uniforms["timestep"];
	int uLoc = m_advection_shader.uniforms["u"];
	int xLoc = m_advection_shader.uniforms["x"];
	int texelLoc = m_advection_shader.uniforms["texelSize"];
	int dissipationLoc = m_advection_shader.uniforms["dissipation"];
	int velocityId = m_velocity_buffer->readBuffer()->setTexture(0);
	glUniform1f(timestepLoc, dt); //timestep = m_timestep
	glUniform1f(dissipationLoc, m_velocity_dissipation);
	glUniform1i(uLoc, velocityId); //u = velocity texture
	glUniform1i(xLoc, velocityId); //x = quantity texture
	glUniform2f(texelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);

	int linearFilteringLoc = m_advection_shader.uniforms["linearFiltering"];
	bool linearFiltering = 1;
	glUniform1i(linearFilteringLoc, linearFiltering);
	blit(m_velocity_buffer->writeBuffer(), &m_advection_shader);
	m_velocity_buffer->swap();
	
	//Advect dye
	glUniform1f(dissipationLoc, m_dye_dissipation);
	glUniform1i(uLoc, m_velocity_buffer->readBuffer()->setTexture(0)); //u = velocity texture
	glUniform1i(xLoc, m_dye_buffer->readBuffer()->setTexture(1)); //x = quantity scalar texture
	glUniform2f(texelLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
	blit(m_dye_buffer->writeBuffer(), &m_advection_shader);
	m_dye_buffer->swap();
}

//Diffusion, by using jacobi iterations
void FluidField::diffuse(float dt) {
	if (m_viscosity > 0) {
		m_jacobi_iteration_shader.use();
		int xLoc = m_jacobi_iteration_shader.uniforms["x"];
		int bLoc = m_jacobi_iteration_shader.uniforms["b"];
		int xTexelLoc = m_jacobi_iteration_shader.uniforms["texelSize"];
		int alphaLoc = m_jacobi_iteration_shader.uniforms["alpha"];
		int rBetaLoc = m_jacobi_iteration_shader.uniforms["rBeta"];
		// Velocity
		float alpha = ((float) pow(1, 2)) / (dt * m_viscosity);
		float rBeta = 1.0f / (4.0f + alpha);
		glUniform1f(alphaLoc, alpha);  //Alpha = pow(dx, 2)/t
		glUniform1f(rBetaLoc, rBeta); //rBeta = 1/(4+Alpha)
		glUniform2f(xTexelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
		glUniform1i(bLoc, m_velocity_buffer->readBuffer()->setTexture(0));

		for (unsigned int k = 0; k < m_diffuseIterations; k++) //20 to 50 iterations
		{
			glUniform1i(xLoc, m_velocity_buffer->readBuffer()->setTexture(1)); // x = b = u (velocity)
			blit(m_velocity_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_velocity_buffer->swap();
		}
		//Diffusion
		// Dye

		//float alpha = pow(1, 2) / m_timestep;
		//float rBeta = 1 / (4 + alpha);
		glUniform1f(alphaLoc, alpha);  //Alpha = pow(dx, 2)/t
		glUniform1f(rBetaLoc, rBeta); //rBeta = 1/(4+Alpha)
		glUniform2f(xTexelLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
		glUniform1i(bLoc, m_dye_buffer->readBuffer()->setTexture(0));

		for (unsigned int k = 0; k < m_diffuseIterations; k++) //20 to 50 iterations
		{
			glUniform1i(xLoc, m_dye_buffer->readBuffer()->setTexture(1)); // x = b = u (velocity)
			blit(m_dye_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_dye_buffer->swap();
		}
	}
}

//Force Application
void FluidField::addForces(float dt) {
	//blit(m_velocity_buffer->writeBuffer(), &m_force_shader);
	//m_velocity_buffer->swap();
}

//Projection, by removing any divergence
void FluidField::project(float dt) {
	//boundary(dt);
	//Compute a normalized vorticity vector field
	//curl(dt);
	//Restore, approximate, computated and dissipated vorticity
	//vorticity(dt);
	divergence(dt);
	clearBuffer(m_pressure_buffer, m_pressure_dissipation);
	pressure(dt);
	gradientSubtract(dt);
}

void FluidField::boundary(float dt) {
	m_bounds_shader.use();
	float scale = 1.0f;
	float offset = 1.0f;
	int uLoc = m_curl_shader.uniforms["u"];
	int texelSizeLoc = m_bounds_shader.uniforms["texelSize"];
	int dtLoc = m_bounds_shader.uniforms["dt"];
	int offsetLoc = m_bounds_shader.uniforms["off"];
	int scaleLoc = m_bounds_shader.uniforms["scale"];
	glUniform1i(uLoc, m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(texelSizeLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	glUniform1f(dtLoc, dt);
	glUniform1f(offsetLoc, offset);
	glUniform1f(scaleLoc, scale);
	blit(m_velocity_buffer->writeBuffer(), &m_bounds_shader);
	m_velocity_buffer->swap();
}

//Compute a normalized vorticity vector field
void FluidField::curl(float dt) {
	m_curl_shader.use();
	int uLoc = m_curl_shader.uniforms["u"];
	int texelSizeLoc = m_curl_shader.uniforms["texelSize"];
	int dtLoc = m_curl_shader.uniforms["dt"];
	glUniform1i(uLoc, m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(texelSizeLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	glUniform1f(dtLoc, dt);
	blit(m_curl_buffer, &m_curl_shader);
}

//Restore, approximate, computated and dissipated vorticity
void FluidField::vorticity(float dt) {
	m_vorticity_shader.use();
	int uLoc = m_vorticity_shader.uniforms["u"]; //Velocity
	int vLoc = m_vorticity_shader.uniforms["v"]; //Curl
	int texelSizeLoc = m_vorticity_shader.uniforms["texelSize"];
	int dtLoc = m_vorticity_shader.uniforms["dt"];
	glUniform1i(uLoc, m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform1i(vLoc, m_curl_buffer->setTexture(1));
	glUniform2f(texelSizeLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	glUniform1f(dtLoc, dt);
	blit(m_velocity_buffer->writeBuffer(), &m_vorticity_shader);
	m_velocity_buffer->swap();
}

//Solve divergence
void FluidField::divergence(float dt) {
	m_divergence_shader.use();
	int wLoc = m_divergence_shader.uniforms["w"];
	int texelSizeLoc = m_divergence_shader.uniforms["texelSize"];
	glUniform1i(wLoc, m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(texelSizeLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	blit(m_divergence_buffer, &m_divergence_shader);
}

void FluidField::clearBuffer(DoubleFramebuffer* target, float value)
{
	m_clear_shader.use();
	int valueLoc = m_clear_shader.uniforms["value"];
	int textureLoc = m_clear_shader.uniforms["uTexture"];
	int texSizeLoc = m_clear_shader.uniforms["texelSize"];
	glUniform1f(valueLoc, value);
	glUniform1i(textureLoc, target->readBuffer()->setTexture(0));
	glUniform2f(texSizeLoc, target->readBuffer()->texelSizeX, target->readBuffer()->texelSizeY);
	blit(target->writeBuffer(), &m_clear_shader);
	target->swap();
}

void FluidField::clearBuffer(Framebuffer* target, float value)
{
	m_clear_shader.use();
	int valueLoc = m_clear_shader.uniforms["value"];
	int textureLoc = m_clear_shader.uniforms["uTexture"];
	int texSizeLoc = m_clear_shader.uniforms["texelSize"];
	glUniform1f(valueLoc, value);
	glUniform1i(textureLoc, target->setTexture(0));
	glUniform2f(texSizeLoc, target->texelSizeX, target->texelSizeY);
	blit(target, &m_clear_shader);
}

void FluidField::pressure(float dt)
{	//Solve Pressure
	{
		m_jacobi_iteration_shader.use();
		int xLoc = m_jacobi_iteration_shader.uniforms["x"];
		int bLoc = m_jacobi_iteration_shader.uniforms["b"];
		int xTexelLoc = m_jacobi_iteration_shader.uniforms["texelSize"];
		int alphaLoc = m_jacobi_iteration_shader.uniforms["alpha"];
		int rBetaLoc = m_jacobi_iteration_shader.uniforms["rBeta"];

		float alpha = -pow(1.0f, 2.0f); //Alpha = -pow(x,2)
		float rBeta = 1.0f / 4.0f; //rBeta = 1/4
		glUniform1f(alphaLoc, alpha);
		glUniform1f(rBetaLoc, rBeta);
		glUniform2f(xTexelLoc, m_pressure_buffer->readBuffer()->texelSizeX, m_pressure_buffer->readBuffer()->texelSizeY);
		glUniform1i(bLoc, m_divergence_buffer->setTexture(0));	//b = gradient of w (divergent velocity)
		for (unsigned int k = 0; k < m_pressureIterations; k++)
		{
			glUniform1i(xLoc, m_pressure_buffer->readBuffer()->setTexture(1)); //x = p (pressure)
			blit(m_pressure_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_pressure_buffer->swap();
		}
	}
}

//Subtract Gradient of pressure field from velocity field to achieve divergence free velocity
void FluidField::gradientSubtract(float dt)
{
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

void FluidField::splat(glm::vec2 pos, float r) {
	m_splat_shader.use();
	//Uniforms
	int uTargetLoc = m_splat_shader.uniforms["uTarget"];
	int uPointLoc = m_splat_shader.uniforms["point"];
	int uColorLoc = m_splat_shader.uniforms["color"];
	int uRadiusLoc = m_splat_shader.uniforms["radius"];
	int uTexLoc = m_splat_shader.uniforms["texelSize"];

	glUniform1i(uTargetLoc, m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(uPointLoc, pos.x, pos.y);
	glUniform3f(uColorLoc, m_mouse.texcoord_delta.x * m_dye_force, m_mouse.texcoord_delta.y * m_dye_force, 0.0f);
	glUniform1f(uRadiusLoc, r);
	glUniform2f(uTexLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	blit(m_velocity_buffer->writeBuffer(), &m_splat_shader);
	m_velocity_buffer->swap();

	glUniform1i(m_splat_shader.uniforms["uTarget"], m_dye_buffer->readBuffer()->setTexture(0));
	glUniform2f(uTexLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
	glm::vec3 color = glm::vec3(m_mouse.texcoord_delta.x * m_dye_force /100.f, m_mouse.texcoord_delta.y * m_dye_force / 100.f, 0.2f);
	color *= 0.5f;
	glUniform3f(uColorLoc, abs(color.r), abs(color.g), abs(color.b + (color.r+color.g)/5.0f));
	blit(m_dye_buffer->writeBuffer(), &m_splat_shader);
	m_dye_buffer->swap();
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
			m_translations[(double)r * m_fieldWidth + (double)c] = glm::vec2(c, r);
		}
	}
	m_quad->DrawInstanced(*m_visualise_grid_shader, m_translations.data(), m_resolution);
}

void FluidField::updateMouse(double* mouseX, double* mouseY, bool* mouse_down)
{
	m_mouse.update(*mouseX, *mouseY, *mouse_down);
	if (m_mouse.down && (abs(m_mouse.window_delta.x) > 0 || abs(m_mouse.window_delta.y) > 0))
	{
		//std::cout << "X: " << m_mouse.texcoord_pos.x << " Y: " << m_mouse.texcoord_pos.y << " down:" << m_mouse.down << std::endl;
		splat(m_mouse.texcoord_pos, m_dye_radius);
	}
}

void FluidField::swapBuffer(int i) {
	if (i == 1)
	{
		if (m_current_buffer == 0) return;
		std::cout << "BUFFER::DYE" << std::endl;
		m_current_buffer = 0;
	}
	else if (i == 2)
	{
		if (m_current_buffer == m_velocity_buffer->readBuffer()) return;
		std::cout << "BUFFER::VELOCITY" << std::endl;
		m_current_buffer = m_velocity_buffer->readBuffer();
	}
	else if (i == 3) {
		if (m_current_buffer == m_divergence_buffer) return;
		std::cout << "BUFFER::DIVERGENCE" << std::endl;
		m_current_buffer = m_divergence_buffer;

	}
	else if (i == 4) {
		if (m_current_buffer == m_pressure_buffer->readBuffer() || m_current_buffer == m_pressure_buffer->writeBuffer()) return;
		std::cout << "BUFFER::PRESSURE" << std::endl;
		m_current_buffer = m_pressure_buffer->readBuffer();

	}
	else if (i == 5) {
		if (m_current_buffer == m_curl_buffer) return;
		std::cout << "BUFFER::CURL" << std::endl;
		m_current_buffer = m_curl_buffer;
	}
}

void FluidField::reset()
{
}
