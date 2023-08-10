#include "FluidField.h"

void FluidField::Draw(glm::vec3 origin) {

	m_primary_shader->use();
	glUniform1i(m_primary_shader->uniforms["u_image"], m_dye_buffer->readBuffer()->setTexture(0));
	glUniform2f(m_primary_shader->uniforms["dyeTexelSize"], m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
	glUniform2f(m_primary_shader->uniforms["velTexelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	if (m_current_buffer != m_dye_buffer->readBuffer())
	{
		glUniform1i(m_primary_shader->uniforms["u_image_overlay"], m_current_buffer->setTexture(1));
	}
	else
	{
		glUniform1i(m_primary_shader->uniforms["u_image_overlay"], 0);
	}
	blit(nullptr, m_primary_shader);

	glm::mat4 viewM = glm::mat4(1.0f);
	glm::mat4 projectionM = glm::mat4(1.0f);

	projectionM = glm::ortho(0.0f, (float)m_WIDTH, 0.0f, (float)m_HEIGHT, -1000.0f, 1000.0f);

	m_object_shader.use();
	m_object_shader.setMat4f("view", viewM);
	m_object_shader.setMat4f("projection", projectionM);

	rectangle.transform.dim = glm::vec3(50.0f);
	rectangle.transform.pos = glm::vec3(300.0f, 300.0f, 0.0f);
	//rectangle.Draw(m_object_shader);
}

//Draw to specified framebuffer with specified shader
void FluidField::blit(Framebuffer* target, Shader* shader) {
	//glm::mat4 modelM = glm::mat4(1.0f);
	//glm::mat4 viewM = glm::mat4(1.0f);
	//glm::mat4 projectionM = glm::mat4(1.0f);

	//projectionM = glm::ortho(0.0f, (float)m_WIDTH, 0.0f, (float)m_HEIGHT, -1000.0f, 1000.0f);

	shader->use();
	//shader->setMat4f("view", viewM);
	//shader->setMat4f("projection", projectionM);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	//Bind framebuffer
	if (target == nullptr || target->fbo == NULL)
	{
		glViewport(0, 0, m_WIDTH, m_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}
	else
	{
		glViewport(0, 0, target->width, target->height);
		glBindFramebuffer(GL_FRAMEBUFFER, target->fbo);
	}


	//Draw to framebuffer
	if (!target->status())
	{
		std::cout << "ERROR::BLIT::FRAMEBUFFER::STATUS::INCOMPLETE" << std::endl;
		return;
	}

	m_fieldQuad->Draw(*shader);
	//Unbind framebuffer
	int boundBuffer = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundBuffer);
	if (target != nullptr && boundBuffer == target->fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

//Is not being used
void FluidField::boundaryContainer(bool l, bool r, bool t, bool b, Framebuffer* target, Shader& shader)
{
	line.width(10.0f);
	glm::mat4 projectionM = glm::mat4(1.0f);

	//projectionM = glm::ortho(0.0f, (float)1.0f, 0.0f, (float)1.0f, -1000.0f, 1000.0f);

	shader.use();
	shader.setMat4f("projection", projectionM);
	glBindFramebuffer(GL_FRAMEBUFFER, target->fbo);

	if (l)
	{
		line.start(0.0f, 0.0f, 0.0f);
		line.end(0.0f, 1.0f, 0.0f);
		line.Draw(shader);
	}
	//if (t)
	//{
	//	line.start(0.0f, 1.0f, 0.0f);
	//	line.end(1.0f, 1.0f, 0.0f);
	//	line.Draw(shader);
	//}
	//if (r)
	//{
	//	line.start(1.0f, 1.0f, 0.0f);
	//	line.end(1.0f, 0.0f, 0.0f);
	//	line.Draw(shader);
	//}
	//if (b)
	//{
	//	line.start(1.0f, 0.0f, 0.0f);
	//	line.end(0.0f, 0.0f, 0.0f);
	//	line.Draw(shader);
	//}
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

}

//Advection -> Diffusion -> Force Application -> Projection
void FluidField::timeStep(float dt) {
	float time = dt * m_timestep_scalar; 
	glDisable(GL_BLEND);
	if(m_advect)	advect(time);		//Move the fluid and its quantities
	if(m_diffuse)	diffuse(time);		//Spread out the fluid (if viscosity > 0)
	if(m_forces)	addForces(time);	//Add a ball in the center and gravity. Also add multiple splats
	if(m_project)	project(time);		//Remove unwanted stuff
}

//Is not being used
void FluidField::boundary(float dt, float scale, float offset, DoubleFramebuffer* target) {
	m_bounds_shader.use();
	int uLoc = m_bounds_shader.uniforms["u"];
	int texelSizeLoc = m_bounds_shader.uniforms["texelSize"];
	int dtLoc = m_bounds_shader.uniforms["dt"];
	int offsetLoc = m_bounds_shader.uniforms["off"];
	int scaleLoc = m_bounds_shader.uniforms["scale"];
	glUniform1i(uLoc, target->readBuffer()->setTexture(0));
	glUniform2f(texelSizeLoc, target->readBuffer()->texelSizeX, target->readBuffer()->texelSizeY);
	glUniform1f(dtLoc, dt);
	glUniform1f(offsetLoc, offset);
	glUniform1f(scaleLoc, scale);

	boundaryContainer(1, 1, 1, 1, target->writeBuffer(), m_bounds_shader);
}

//Isn't being used
void FluidField::boundaries(float dt) {
	//Velocity
	//boundary(dt, 1.0f, 0, m_dye_buffer);
	//Pressure
	//boundary(dt, 0, 0, m_pressure_buffer);
}

//Add a value to an entire framebuffer
void FluidField::bufferIntegrate(DoubleFramebuffer* target, glm::vec4 values)
{
	m_integrate_shader.use();
	int valueLoc = m_integrate_shader.uniforms["value"];
	int textureLoc = m_integrate_shader.uniforms["uTexture"];
	int texSizeLoc = m_integrate_shader.uniforms["texelSize"];
	glUniform4f(valueLoc, values.x, values.y, values.z, values.w);
	glUniform1i(textureLoc, target->readBuffer()->setTexture(0));
	glUniform2f(texSizeLoc, target->readBuffer()->texelSizeX, target->readBuffer()->texelSizeY);
	blit(target->writeBuffer(), &m_integrate_shader);
	target->swap();
}

//Calculate and add forces due to temprature (Is not being used)
void FluidField::temperature(float dt) {
	m_temperature_shader.use();
	int uLoc = m_temperature_shader.uniforms["u"];
	int dLoc = m_temperature_shader.uniforms["d"];
	int tLoc = m_temperature_shader.uniforms["t"];
	int tempLoc = m_temperature_shader.uniforms["T0"];
	int scalarLoc = m_temperature_shader.uniforms["s"];
	int massLoc = m_temperature_shader.uniforms["k"];
	int dtLoc = m_temperature_shader.uniforms["dt"];
	int texelLoc = m_temperature_shader.uniforms["texelSize"];

	glUniform1i(uLoc, m_velocity_buffer->readBuffer()->setTexture(0));		//Velocity
	glUniform1i(tLoc, m_pressure_buffer->readBuffer()->setTexture(1));		//Temp
	glUniform1i(dLoc, m_density_buffer->readBuffer()->setTexture(2));		//Density

	glUniform1f(tempLoc, m_ambient_temperature);
	glUniform1f(massLoc, m_mass);
	glUniform1f(scalarLoc, m_temperature_scalar);
	glUniform1f(dtLoc, dt);
	glUniform2f(texelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);

	blit(m_velocity_buffer->writeBuffer(), &m_temperature_shader);
	m_velocity_buffer->swap();
}

//Move the quantities in the fluid
void FluidField::advect(float dt) {
	//Advection
	m_advection_shader.use();
	int timestepLoc = m_advection_shader.uniforms["timestep"];
	int uLoc = m_advection_shader.uniforms["u"];
	int xLoc = m_advection_shader.uniforms["x"];
	int texelLoc = m_advection_shader.uniforms["texelSize"];
	int sourceTexelLoc = m_advection_shader.uniforms["sourceTexelSize"];
	int dissipationLoc = m_advection_shader.uniforms["dissipation"];
	int velocityId = m_velocity_buffer->readBuffer()->setTexture(0);
	glUniform1f(timestepLoc, dt); //timestep = m_timestep
	glUniform1f(dissipationLoc, m_velocity_dissipation);
	glUniform1i(uLoc, velocityId); //u = velocity texture
	glUniform1i(xLoc, velocityId); //x = quantity texture
	glUniform2f(texelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	glUniform2f(sourceTexelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);

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
	glUniform2f(sourceTexelLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);

	blit(m_dye_buffer->writeBuffer(), &m_advection_shader);
	m_dye_buffer->swap();

	//Advect density
	/*glUniform1f(dissipationLoc, m_dye_dissipation);
	glUniform1i(xLoc, m_density_buffer->readBuffer()->setTexture(1)); //x = quantity scalar texture
	glUniform2f(texelLoc, m_density_buffer->readBuffer()->texelSizeX, m_density_buffer->readBuffer()->texelSizeY);
	blit(m_density_buffer->writeBuffer(), &m_advection_shader);
	m_density_buffer->swap();*/
}

//Diffusion, by using jacobi iterations, spread out the fluid
void FluidField::diffuse(float dt) {
	if (m_viscosity > 0) {
		m_jacobi_iteration_shader.use();
		int xLoc = m_jacobi_iteration_shader.uniforms["x"];
		int bLoc = m_jacobi_iteration_shader.uniforms["b"];
		int xTexelLoc = m_jacobi_iteration_shader.uniforms["texelSize"];
		int alphaLoc = m_jacobi_iteration_shader.uniforms["alpha"];
		int rBetaLoc = m_jacobi_iteration_shader.uniforms["rBeta"];
		// Velocity
		float alpha = 1.0f / (dt * m_viscosity); //Alpha = pow(x, 2)/t
		float rBeta = 1.0f / (4.0f + alpha);	 //rBeta = 1/(4+Alpha)
		glUniform1f(alphaLoc, alpha);
		glUniform1f(rBetaLoc, rBeta); 
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
		//glUniform1f(alphaLoc, alpha);  //Alpha = pow(dx, 2)/t
		//glUniform1f(rBetaLoc, rBeta); //rBeta = 1/(4+Alpha)
		glUniform2f(xTexelLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);
		glUniform1i(bLoc, m_dye_buffer->readBuffer()->setTexture(0));

		for (unsigned int k = 0; k < m_diffuseIterations; k++) //20 to 50 iterations
		{
			glUniform1i(xLoc, m_dye_buffer->readBuffer()->setTexture(1)); // x = b = u (dye)
			blit(m_dye_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_dye_buffer->swap();
		}
	}
}

//Force Application
void FluidField::addForces(float dt) {
	float r = 0.0358f;
	float streams = m_splats;

	//for (int stream = 0; stream < streams; stream++)
	//{
	//	splat(glm::vec2((1.0f) / streams * ((stream + 1.0f)) - ((1.0f) / streams / 2.0f), 0.9f), r, true, false);
	//	//splat(glm::vec2(0.1f, 1.0f - (1.0f) / streams * ((stream + 1.0f)) + ((1.0f) / streams / 2.0f)), r, true, false);
	//}
	splat(glm::vec2(0.5f, 0.9f), r, streams, true, false);
	m_integrate_shader.use();
	//float o = 0.8f;
	//float value = (sin(glfwGetTime() * 0.1f) + 1) / 2.0f * o + (1 - o) / 2.0f;
	//glUniform1f(m_integrate_shader.uniforms["time"], value);
	bufferIntegrate(m_velocity_buffer, glm::vec4(0.0f, -90.82f, 0.0f, 0.0f) * dt);
	//temperature(dt);
}

//Projection, by removing any divergence
void FluidField::project(float dt) {
	if (m_vortitcity_scalar > 0) {
		//Compute a normalized vorticity vector field
		curl(dt);
		//Restore, approximate, computated and dissipated vorticity
		vorticity(dt);
	}
	divergence(dt);
	clearBuffer(m_pressure_buffer, m_pressure_dissipation);
	pressure(dt);
	gradientSubtract(dt);
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
	int scaleLoc = m_vorticity_shader.uniforms["scale"];
	glUniform1i(uLoc, m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform1i(vLoc, m_curl_buffer->setTexture(1));
	glUniform2f(texelSizeLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	glUniform1f(dtLoc, dt);
	glUniform1f(scaleLoc, m_vortitcity_scalar);
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
		glUniform2f(xTexelLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
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

void FluidField::splat(glm::vec2 pos, float r, unsigned int amount, bool dye, bool velocity) {
	m_splat_shader.use();
	glUniform1i(m_splat_shader.uniforms["amount"], amount);
	splat(pos, r, dye, velocity);
	glUniform1i(m_splat_shader.uniforms["amount"], 1);
}

void FluidField::splat(glm::vec2 pos, float r, bool dye, bool velocity) {
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
	//std::cout << "X: " << m_mouse.texcoord_delta.x << " Y: " << m_mouse.texcoord_delta.y << std::endl;
	float ratio = m_WIDTH / m_HEIGHT;
	glUniform1f(uRadiusLoc, ratio > 1 ? r * ratio : r);
	glUniform2f(uTexLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY);
	if (velocity)
	{
		blit(m_velocity_buffer->writeBuffer(), &m_splat_shader);
		m_velocity_buffer->swap();
	}
	//glUniform1f(uRadiusLoc, ratio > 1 ? r * ratio * 5 : r);

	glUniform1i(m_splat_shader.uniforms["uTarget"], m_dye_buffer->readBuffer()->setTexture(0));
	glUniform2f(uTexLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY);

	glm::vec3 color = glm::vec3(0.0f);
	if (m_dye_color_acc_dependent) {
		color = glm::vec3(m_mouse.texcoord_delta.x * m_dye_force /100.f, m_mouse.texcoord_delta.y * m_dye_force / 100.f, 0.2f);
		color *= 0.5f;
	}
	else {
		color = glm::vec3(m_dye_color[0], m_dye_color[1], m_dye_color[2]);
		color *= 0.5f;
	}
	glUniform3f(uColorLoc, abs(color.r), abs(color.g), abs(color.b + (color.r+color.g)/5.0f) * 0.3f);
	if (dye)
	{
		blit(m_dye_buffer->writeBuffer(), &m_splat_shader);
		m_dye_buffer->swap();
		//Add density
		/*color = glm::vec3(m_density, 0.0f, 0.0f);
		glUniform3f(uColorLoc, color.r, 0.0f, 0.0f);
		glUniform1i(m_splat_shader.uniforms["uTarget"], m_density_buffer->readBuffer()->setTexture(0));
		glUniform2f(uTexLoc, m_density_buffer->readBuffer()->texelSizeX, m_density_buffer->readBuffer()->texelSizeY);
		blit(m_density_buffer->writeBuffer(), &m_splat_shader);
		m_density_buffer->swap();*/
	}

	
}

void FluidField::updateMouse(double* mouseX, double* mouseY, bool* left_mouse_down, bool* right_mouse_down)
{
	m_mouse.update(*mouseX, *mouseY, *left_mouse_down, *right_mouse_down);

	if (abs(m_mouse.window_delta.x) > 0 || abs(m_mouse.window_delta.y) > 0)
	{
		if (m_mouse.left_mouse_down) {
			splat(m_mouse.texcoord_pos, m_dye_radius, true, true);
		}
		else if (m_mouse.right_mouse_down) {
			splat(m_mouse.texcoord_pos, m_dye_radius, false, true);
		}
		//std::cout << "X: " << m_mouse.texcoord_pos.x << " Y: " << m_mouse.texcoord_pos.y << " down:" << m_mouse.down << std::endl;
	}
}

void FluidField::swapBuffer(int i) {
	m_primary_shader->use();
	if (i == 1)
	{
		if (m_current_buffer == m_dye_buffer->readBuffer()) return;
		std::cout << "BUFFER::DYE" << std::endl;
		m_current_buffer = m_dye_buffer->readBuffer();
		glUniform1i(m_primary_shader->uniforms["scene"], 0);
	}
	else if (i == 2)
	{
		if (m_current_buffer == m_velocity_buffer->readBuffer()) return;
		std::cout << "BUFFER::VELOCITY" << std::endl;
		m_current_buffer = m_velocity_buffer->readBuffer();
		glUniform1i(m_primary_shader->uniforms["scene"], 1);
	}
	else if (i == 3) {
		if (m_current_buffer == m_divergence_buffer) return;
		std::cout << "BUFFER::DIVERGENCE" << std::endl;
		m_current_buffer = m_divergence_buffer;
		glUniform1i(m_primary_shader->uniforms["scene"], 2);
	}
	else if (i == 4) {
		if (m_current_buffer == m_pressure_buffer->readBuffer() || m_current_buffer == m_pressure_buffer->writeBuffer()) return;
		std::cout << "BUFFER::PRESSURE" << std::endl;
		m_current_buffer = m_pressure_buffer->readBuffer();
		glUniform1i(m_primary_shader->uniforms["scene"], 3);
	}
	else if (i == 5) {
		if (m_current_buffer == m_curl_buffer) return;
		std::cout << "BUFFER::CURL" << std::endl;
		m_current_buffer = m_curl_buffer;
		glUniform1i(m_primary_shader->uniforms["scene"], 4);
	}
	else if (i == 6) {
		if (m_current_buffer == m_temperature_buffer->readBuffer()) return;
		std::cout << "BUFFER::TEMPERATURE" << std::endl;
		m_current_buffer = m_temperature_buffer->readBuffer();
		glUniform1i(m_primary_shader->uniforms["scene"], 5);
	}
	else if (i == 7) {
		if (m_current_buffer == m_density_buffer->readBuffer()) return;
		std::cout << "BUFFER::DENSITY" << std::endl;
		m_current_buffer = m_density_buffer->readBuffer();
		glUniform1i(m_primary_shader->uniforms["scene"], 6);
	}
}

void FluidField::reset()
{
	clearBuffer(m_dye_buffer, 0.0);
	clearBuffer(m_velocity_buffer, 0.0);
	clearBuffer(m_pressure_buffer, 0.0);
	clearBuffer(m_density_buffer, 0.0);
	clearBuffer(m_curl_buffer, 0.0);
}

int FluidField::applyConfiguration(Config &configurationFile)
{
	if (configurationFile.values.size() <= 0) {
		std::cout << "Configuration file has not been read" << std::endl;
		return -1;
	}
	try
	{
		m_dye_scalar = std::stof(configurationFile.values[FLUID.dye_scalar]);
		m_velocity_scalar = std::stof(configurationFile.values[FLUID.velocity_scalar]);
		//m_dye_color = std::stof(configurationFile.values[FLUID.dye_color]);
		m_dye_color_acc_dependent = configurationFile.values[FLUID.dye_color_acc_dependent] == "1";
		m_dye_force = std::stof(configurationFile.values[FLUID.dye_force]);
		m_dye_radius = std::stof(configurationFile.values[FLUID.dye_radius]);
		m_dye_dissipation = std::stof(configurationFile.values[FLUID.dye_dissipation]);
		m_velocity_dissipation = std::stof(configurationFile.values[FLUID.velocity_dissipation]);
		m_diffuseIterations = std::stoi(configurationFile.values[FLUID.diffuseIterations]);
		m_viscosity = std::stof(configurationFile.values[FLUID.viscosity]);
		m_pressureIterations = std::stoi(configurationFile.values[FLUID.pressureIterations]);
		m_pressure_dissipation = std::stof(configurationFile.values[FLUID.pressure_dissipation]);
		m_vortitcity_scalar = std::stof(configurationFile.values[FLUID.vortitcity_scalar]);
		m_timestep_scalar = std::stof(configurationFile.values[FLUID.timestep_scalar]);

		m_splats = std::stoi(configurationFile.values[FLUID.splats]);
		m_advect = configurationFile.values[FLUID.advect] == "1";
		m_diffuse = configurationFile.values[FLUID.diffuse] == "1";
		m_forces = configurationFile.values[FLUID.forces] == "1";
		m_project = configurationFile.values[FLUID.project] == "1";
		m_image = configurationFile.values[FLUID.image] == "1";

		return 0;
	}
	catch (const std::exception&)
	{
		std::cout << "Configuration file could not be applied" << std::endl;
		return -1;
	}
}

void FluidField::updateConfiguration()
{
	fluid_config_file.updateValues();
	FluidField::applyConfiguration(fluid_config_file);
}
