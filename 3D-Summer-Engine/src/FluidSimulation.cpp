#include "FluidSimulation.h"

FluidSimulation::FluidSimulation(const unsigned int WIDTH, const unsigned int HEIGHT, const unsigned int resolution) :
	m_resolution(resolution),
	m_WIDTH(WIDTH),
	m_HEIGHT(HEIGHT),
	m_mouse(WIDTH, HEIGHT),
	rectangle(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f)),
	line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f),

	fluid_config(p_CONFIG_FILE),

	m_object_shader(p_OBJECT_VERTEX_SHADER, p_OBJECT_FRAGMENT_SHADER),
	m_advection_shader(p_VERTEX_SHADER, p_advection_shader),
	m_jacobi_iteration_shader(p_VERTEX_SHADER, p_jacobi_shader),
	m_force_shader(p_VERTEX_SHADER, p_force_shader),
	m_divergence_shader(p_VERTEX_SHADER, p_divergence_shader),
	m_clear_shader(p_VERTEX_SHADER, p_clear_shader),
	m_integrate_shader(p_VERTEX_SHADER, p_integrate_shader),
	m_gradient_subtraction_shader(p_VERTEX_SHADER, p_gradient_subtraction_shader),
	m_vorticity_shader(p_VERTEX_SHADER, p_vorticity_shader),
	m_curl_shader(p_VERTEX_SHADER, p_curl_shader),
	m_bounds_shader(p_VERTEX_SHADER, p_bounds_shader),
	m_splat_shader(p_VERTEX_SHADER, p_splat_shader),
	m_apply_shader(p_VERTEX_SHADER, p_apply_shader),
	m_primary_shader(p_VERTEX_SHADER, p_FRAGMENT_SHADER)
{
	std::cout << "APPLYING::CONFIGURATIONS" << std::endl;

	if (fluid_config.fileExists()) applyConfiguration(fluid_config);

	std::cout << "INITIALIZING::FLUIDFIELD" << std::endl;

	m_texture = new Texture2D(p_TEXTURE);

	//The simulation is simply a texture drawn on this rectangle
	m_fieldQuad	= new Rect(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), m_texture->get());

	GLenum textureType = GL_UNSIGNED_BYTE;	//Field type
	TexFormat rgba(GL_RGBA32F, GL_RGBA);	//Quantity field
	TexFormat rg(GL_RG32F, GL_RG);			//Vector field
	TexFormat r(GL_R32F, GL_RED);			//Scalar field
	glDisable(GL_BLEND);

	std::cout << "Framebuffer: Height: " << m_HEIGHT << " Width: " << m_WIDTH << std::endl;

	const unsigned int velocityResolution = (unsigned int) (m_resolution * m_velocity_resolution_scalar), dyeResolution = (unsigned int) (m_resolution * m_dye_resolution_scalar);


	//Buffers that store the calculated results
	m_dye_buffer        = new DoubleFramebuffer(dyeResolution, m_WIDTH, m_HEIGHT, rgba.internal, rgba.format, textureType, GL_LINEAR);
	//m_dye_buffer->readBuffer()->setTextureSource(p_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_LINEAR);
	//m_dye_buffer->writeBuffer()->setTextureSource(p_TEXTURE, m_WIDTH, m_HEIGHT, GL_RGB32F, GL_RGB, textureType, GL_LINEAR);
	m_velocity_buffer   = new DoubleFramebuffer(velocityResolution, m_WIDTH, m_HEIGHT, rg.internal, rg.format, textureType, GL_LINEAR);
	m_pressure_buffer   = new DoubleFramebuffer(velocityResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
	m_curl_buffer       = new Framebuffer(velocityResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);
	m_divergence_buffer = new Framebuffer(velocityResolution, m_WIDTH, m_HEIGHT, r.internal, r.format, textureType, GL_NEAREST);

	m_render_buffer = m_dye_buffer->readBuffer();

	std::cout
		<< "-Velocity resolution: " << velocityResolution
		<< "\n-Velocity buffer size: " << m_velocity_buffer->readBuffer()->width << "x" << m_velocity_buffer->readBuffer()->height
		<< "\n-Dye resolution: " << dyeResolution
		<< "\n-Dye buffer size: " << m_dye_buffer->readBuffer()->width << "x" << m_dye_buffer->readBuffer()->height << std::endl;

	std::cout << "SUCCESS::INITIALIZATION::FLUIDFIELD" << std::endl;
}

void FluidSimulation::resizeViewport(unsigned int width, unsigned int height)
{
	m_mouse.updateMousearea(width, height);
	std::cout << "Set viewport to: Height: " << height << " Width: " << width << std::endl;
}

void FluidSimulation::Draw(glm::vec3 origin)
{

	m_primary_shader.use();
	glUniform1i(m_primary_shader.uniforms["u_image"], m_dye_buffer->readBuffer()->setTexture(0));
	glUniform2f(m_primary_shader.uniforms["viewportBufferSize"], m_mouse.width, m_mouse.height);
	glUniform2f(m_primary_shader.uniforms["dyeTexelSize"], m_dye_buffer->readBuffer()->texelSize.x, m_dye_buffer->readBuffer()->texelSize.y);
	glUniform2f(m_primary_shader.uniforms["velTexelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	if (m_render_buffer != m_dye_buffer->readBuffer())
	{
		glUniform1i(m_primary_shader.uniforms["u_image_overlay"], m_render_buffer->setTexture(1));
	}
	else
	{
		glUniform1i(m_primary_shader.uniforms["u_image_overlay"], 0);
	}
	blit(nullptr, &m_primary_shader);

	/*glm::mat4 viewM = glm::mat4(1.0f);
	glm::mat4 projectionM = glm::mat4(1.0f);

	projectionM = glm::ortho(0.0f, (float)m_WIDTH, 0.0f, (float)m_HEIGHT, -1000.0f, 1000.0f);

	m_object_shader.use();
	m_object_shader.setMat4f("view", viewM);
	m_object_shader.setMat4f("projection", projectionM);

	//m_fieldQuad->transform.dim = glm::vec3(50.0f);
	//m_fieldQuad->transform.pos = glm::vec3(300.0f, 300.0f, 0.0f);
	m_fieldQuad->Draw(m_object_shader);*/
}

//Draw to specified framebuffer with specified shader
void FluidSimulation::blit(Framebuffer* target, Shader* shader)
{
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
		glViewport(0, 0, m_mouse.width, (GLsizei) m_mouse.height);
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

//Advection -> Diffusion -> Force Application -> Projection
void FluidSimulation::timeStep(float deltaTime)
{
	float time = deltaTime * m_timestep_scalar;

	if (m_advect)	advect(time);		//Move the fluid and its quantities
	if (m_diffuse)	diffuse(time);		//Spread out the fluid (if viscosity > 0)
	if (m_forces)	addForces(time);	//Add a ball in the center and gravity. Also add multiple splats
	if (m_project)	project(time);		//Remove unwanted stuff
	if (m_visualize_change) displayFluidMotion();



}

void FluidSimulation::splat()
{
	if (abs(m_mouse.window_delta.x) > 0 || abs(m_mouse.window_delta.y) > 0)
	{
		if (m_mouse.right_mouse_down && m_mouse.left_mouse_down)
		{
			splat(m_mouse.texcoord_pos, m_splat_radius, true, false);
		}
		else if (m_mouse.right_mouse_down)
		{
			splat(m_mouse.texcoord_pos, m_splat_radius, false, true);
		}
		else if (m_mouse.left_mouse_down)
		{
			splat(m_mouse.texcoord_pos, m_splat_radius, true, true);
		}
		//std::cout << "X: " << m_mouse.window_pos.x << " Y: " << m_mouse.window_pos.y << "tX: " << m_mouse.texcoord_pos.x << " tY: " << m_mouse.texcoord_pos.y << std::endl;
	}
	else if (m_mouse.left_mouse_down)
	{
		splat(m_mouse.texcoord_pos, m_splat_radius, true, false);
	}
}

//Add a value to an entire framebuffer
void FluidSimulation::bufferIntegrate(DoubleFramebuffer* target, glm::vec4 values)
{
	m_integrate_shader.use();
	glUniform4f(m_integrate_shader.uniforms["value"], values.x, values.y, values.z, values.w);
	glUniform1i(m_integrate_shader.uniforms["uTexture"], target->readBuffer()->setTexture(0));
	glUniform2f(m_integrate_shader.uniforms["texelSize"], target->readBuffer()->texelSize.x, target->readBuffer()->texelSize.y);
	blit(target->writeBuffer(), &m_integrate_shader);
	target->swap();
}

void FluidSimulation::displayFluidMotion()
{
	m_apply_shader.use();
	glUniform3f(m_apply_shader.uniforms["value"], 3.5f, 0.0, 0);
	glUniform1i(m_apply_shader.uniforms["divergenceTexture"], m_divergence_buffer->setTexture(0));
	glUniform1i(m_apply_shader.uniforms["dyeTexture"], m_dye_buffer->readBuffer()->setTexture(1));
	blit(m_dye_buffer->writeBuffer(), &m_apply_shader);
	m_dye_buffer->swap();
}

//Move the quantities in the fluid
void FluidSimulation::advect(float dt)
{
	//Advection
	m_advection_shader.use();
	glUniform1f(m_advection_shader.uniforms["timestep"], dt); //timestep = m_timestep
	glUniform1f(m_advection_shader.uniforms["dissipation"], m_velocity_dissipation);
	glUniform1i(m_advection_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture(0)); //u = velocity texture
	glUniform1i(m_advection_shader.uniforms["x"], m_velocity_buffer->readBuffer()->setTexture(0)); //x = quantity texture
	glUniform2f(m_advection_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	glUniform2f(m_advection_shader.uniforms["sourceTexelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	glUniform1i(m_advection_shader.uniforms["linearFiltering"], 1);

	blit(m_velocity_buffer->writeBuffer(), &m_advection_shader);
	m_velocity_buffer->swap();

	//Advect dye
	glUniform1f(m_advection_shader.uniforms["dissipation"], m_dye_dissipation);
	glUniform1i(m_advection_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture(0)); //u = velocity texture
	glUniform1i(m_advection_shader.uniforms["x"], m_dye_buffer->readBuffer()->setTexture(1)); //x = quantity scalar texture
	glUniform2f(m_advection_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	glUniform2f(m_advection_shader.uniforms["sourceTexelSize"], m_dye_buffer->readBuffer()->texelSize.x, m_dye_buffer->readBuffer()->texelSize.y);

	blit(m_dye_buffer->writeBuffer(), &m_advection_shader);
	m_dye_buffer->swap();
}

//Diffusion, by using jacobi iterations, spread out the fluid
void FluidSimulation::diffuse(float dt)
{
	if (m_viscosity > 0)
	{
		m_jacobi_iteration_shader.use();
		//Velocity
		float alpha = 1.0f / (dt * m_viscosity); //Alpha = pow(x, 2)/t
		float rBeta = 1.0f / (4.0f + alpha);	 //rBeta = 1/(4+Alpha)
		glUniform1f(m_jacobi_iteration_shader.uniforms["alpha"], alpha);
		glUniform1f(m_jacobi_iteration_shader.uniforms["rBeta"], rBeta);
		glUniform2f(m_jacobi_iteration_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
		glUniform1i(m_jacobi_iteration_shader.uniforms["b"], m_velocity_buffer->readBuffer()->setTexture(0));

		for (unsigned int k = 0; k < m_diffuseIterations; k++) //20 to 50 iterations
		{
			glUniform1i(m_jacobi_iteration_shader.uniforms["x"], m_velocity_buffer->readBuffer()->setTexture(1)); // x = b = u (velocity)
			blit(m_velocity_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_velocity_buffer->swap();
		}
		//Diffusion
		//Dye
		glUniform2f(m_jacobi_iteration_shader.uniforms["texelSize"], m_dye_buffer->readBuffer()->texelSize.x, m_dye_buffer->readBuffer()->texelSize.y);
		glUniform1i(m_jacobi_iteration_shader.uniforms["b"], m_dye_buffer->readBuffer()->setTexture(0));

		for (unsigned int k = 0; k < m_diffuseIterations; k++) //20 to 50 iterations
		{
			glUniform1i(m_jacobi_iteration_shader.uniforms["x"], m_dye_buffer->readBuffer()->setTexture(1)); // x = b = u (dye)
			blit(m_dye_buffer->writeBuffer(), &m_jacobi_iteration_shader);
			m_dye_buffer->swap();
		}
	}
}

//Force Application
void FluidSimulation::addForces(float dt)
{
	m_integrate_shader.use();
	float r = m_splat_radius / 10.0f;

	splat(glm::vec2(0.5f, 0.9f), r, m_splats, true, false);
	bufferIntegrate(m_velocity_buffer, glm::vec4(0.0f, -90.82f, 0.0f, 0.0f) * (float) dt);
}

//Projection, by removing any divergence
void FluidSimulation::project(float dt)
{
	if (m_vortitcity_scalar > 0)
	{
		curl(dt);
		vorticity(dt);
	}
	divergence(dt);
	clearBuffer(m_pressure_buffer, m_pressure_dissipation);
	pressure(dt);
	gradientSubtract(dt);
}

//Compute a normalized vorticity vector field
void FluidSimulation::curl(float dt)
{
	m_curl_shader.use();
	glUniform1i(m_curl_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(m_curl_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	glUniform1f(m_curl_shader.uniforms["dt"], dt);
	blit(m_curl_buffer, &m_curl_shader);
}

//Restore, approximate, computated and dissipated vorticity
void FluidSimulation::vorticity(float dt)
{
	m_vorticity_shader.use();
	glUniform1i(m_vorticity_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform1i(m_vorticity_shader.uniforms["v"], m_curl_buffer->setTexture(1));
	glUniform2f(m_vorticity_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	glUniform1f(m_vorticity_shader.uniforms["dt"], dt);
	glUniform1f(m_vorticity_shader.uniforms["scale"], m_vortitcity_scalar);
	blit(m_velocity_buffer->writeBuffer(), &m_vorticity_shader);
	m_velocity_buffer->swap();
}

//Solve divergence
void FluidSimulation::divergence(float dt)
{
	m_divergence_shader.use();
	glUniform1i(m_divergence_shader.uniforms["w"], m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(m_divergence_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	blit(m_divergence_buffer, &m_divergence_shader);
}

void FluidSimulation::clearBuffer(DoubleFramebuffer* target, float value)
{
	m_clear_shader.use();
	glUniform1f(m_clear_shader.uniforms["value"], value);
	glUniform1i(m_clear_shader.uniforms["uTexture"], target->readBuffer()->setTexture(0));
	glUniform2f(m_clear_shader.uniforms["texelSize"], target->readBuffer()->texelSize.x, target->readBuffer()->texelSize.y);
	blit(target->writeBuffer(), &m_clear_shader);
	target->swap();
}

void FluidSimulation::clearBuffer(Framebuffer* target, float value)
{
	m_clear_shader.use();
	glUniform1f(m_clear_shader.uniforms["value"], value);
	glUniform1i(m_clear_shader.uniforms["uTexture"], target->setTexture(0));
	glUniform2f(m_clear_shader.uniforms["texelSize"], target->texelSize.x, target->texelSize.y);
	blit(target, &m_clear_shader);
}

//Solve Pressure
void FluidSimulation::pressure(float dt)
{
	m_jacobi_iteration_shader.use();

	float alpha = -pow(1.0f, 2.0f); //Alpha = -pow(x,2)
	float rBeta = 1.0f / 4.0f; //rBeta = 1/4
	glUniform1f(m_jacobi_iteration_shader.uniforms["alpha"], alpha);
	glUniform1f(m_jacobi_iteration_shader.uniforms["rBeta"], rBeta);
	glUniform2f(m_jacobi_iteration_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	glUniform1i(m_jacobi_iteration_shader.uniforms["b"], m_divergence_buffer->setTexture(0));	//b = gradient of w (divergent velocity)
	for (unsigned int k = 0; k < m_pressureIterations; k++)
	{
		glUniform1i(m_jacobi_iteration_shader.uniforms["x"], m_pressure_buffer->readBuffer()->setTexture(1)); //x = p (pressure)
		blit(m_pressure_buffer->writeBuffer(), &m_jacobi_iteration_shader);
		m_pressure_buffer->swap();
	}
}

//Subtract Gradient of pressure field from velocity field to achieve divergence free velocity
void FluidSimulation::gradientSubtract(float dt)
{
	m_gradient_subtraction_shader.use();
	glUniform1i(m_gradient_subtraction_shader.uniforms["p"], m_pressure_buffer->readBuffer()->setTexture(0)); //p = pressure
	glUniform1i(m_gradient_subtraction_shader.uniforms["w"], m_velocity_buffer->readBuffer()->setTexture(1)); //w = divergent velocity
	glUniform2f(m_gradient_subtraction_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);
	blit(m_velocity_buffer->writeBuffer(), &m_gradient_subtraction_shader);
	m_velocity_buffer->swap();
}

void FluidSimulation::splat(glm::vec2 pos, float r, unsigned int amount, bool dye, bool velocity)
{
	m_splat_shader.use();
	glUniform1i(m_splat_shader.uniforms["amount"], amount);
	splat(pos, r, dye, velocity);
	glUniform1i(m_splat_shader.uniforms["amount"], 1);
}

void FluidSimulation::splat(glm::vec2 pos, float r, bool dye, bool velocity)
{
	m_splat_shader.use();
	//Uniforms
	int uColorLoc = m_splat_shader.uniforms["color"];
	int uTexLoc = m_splat_shader.uniforms["texelSize"];

	glUniform1i(m_splat_shader.uniforms["uTarget"], m_velocity_buffer->readBuffer()->setTexture(0));
	glUniform2f(m_splat_shader.uniforms["point"], pos.x, pos.y);
	glUniform3f(uColorLoc, m_mouse.texcoord_delta.x * m_splat_force, m_mouse.texcoord_delta.y * m_splat_force, 0.0f);

	//float ratio = m_mouse.width / m_mouse.height * 0.5f;
	//glUniform1f(m_splat_shader.uniforms["radius"], ratio > 1 ? r * ratio : r);

	glUniform1f(m_splat_shader.uniforms["radius"], r);
	glUniform2f(uTexLoc, m_velocity_buffer->readBuffer()->texelSize.x, m_velocity_buffer->readBuffer()->texelSize.y);

	if (velocity)
	{
		blit(m_velocity_buffer->writeBuffer(), &m_splat_shader);
		m_velocity_buffer->swap();
	}

	glUniform1i(m_splat_shader.uniforms["uTarget"], m_dye_buffer->readBuffer()->setTexture(0));
	glUniform2f(uTexLoc, m_dye_buffer->readBuffer()->texelSize.x, m_dye_buffer->readBuffer()->texelSize.y);

	glm::vec3 color = glm::vec3(0.0f);

	if (m_splat_color_acc_dependent)
	{
		float scalar = 1 / 0.01f;
		color = (glm::vec3(abs(m_mouse.texcoord_delta.x) * scalar, abs(m_mouse.texcoord_delta.y) * scalar, 0.1f));
		color *= 0.7f;
		std::cout << "dX: " << m_mouse.texcoord_delta.x << ", dY: " << m_mouse.texcoord_delta.y << std::endl;
	}
	else
	{
		color = glm::vec3(m_splat_color[0], m_splat_color[1], m_splat_color[2]);
	}

	color *= m_splat_brightness;

	glUniform3f(uColorLoc, abs(color.r), abs(color.g), abs(color.b));

	if (dye)
	{
		blit(m_dye_buffer->writeBuffer(), &m_splat_shader);
		m_dye_buffer->swap();
	}
}

void FluidSimulation::updateMouse(double* mouseX, double* mouseY, bool* left_mouse_down, bool* right_mouse_down)
{
	m_mouse.update(*mouseX, *mouseY, *left_mouse_down, *right_mouse_down);
	splat();
}

void FluidSimulation::setDisplayBuffer(Framebuffer* buffer)
{
	if (m_render_buffer == buffer) return;
	m_render_buffer = buffer;
}

void FluidSimulation::displayTexture(int i)
{
	if (i < 1) return;
	m_primary_shader.use();

	glUniform1i(m_primary_shader.uniforms["scene"], i - 1);
	switch (i)
	{
		case 1:
		{
			setDisplayBuffer(m_dye_buffer->readBuffer());
			break;
		}
		case 2:
		{
			setDisplayBuffer(m_velocity_buffer->readBuffer());
			break;
		}
		case 3:
		{
			setDisplayBuffer(m_divergence_buffer);
			break;
		}
		case 4:
		{
			setDisplayBuffer(m_pressure_buffer->readBuffer());
			break;
		}
		case 5:
		{
			setDisplayBuffer(m_curl_buffer);
			break;
		}
		default:
		{
			std::cout << "BUFFER::NONE" << std::endl;
			break;
		}
	}
}

void FluidSimulation::clearSimulationBuffers()
{
	clearBuffer(m_dye_buffer, 0.0);
	clearBuffer(m_velocity_buffer, 0.0);
	clearBuffer(m_pressure_buffer, 0.0);
	clearBuffer(m_curl_buffer, 0.0);
}

int FluidSimulation::applyConfiguration(Config& configurationFile)
{
	if (configurationFile.size() <= 0)
	{
		std::cout << "WARNING: Unable to read values from \"" << configurationFile.getPath() << "\"" << std::endl;
		return -1;
	}
	try
	{
		m_dye_resolution_scalar = std::stof(configurationFile.getValue(FLUID.dye_scalar));
		m_velocity_resolution_scalar = std::stof(configurationFile.getValue(FLUID.velocity_scalar));
		//m_splat_color = std::stof(configurationFile.getValue(FLUID.dye_color));
		m_splat_brightness = std::stof(configurationFile.getValue(FLUID.dye_brightness));
		m_splat_color_acc_dependent = configurationFile.getValue(FLUID.dye_color_acc_dependent) == "1";
		m_splat_force = std::stof(configurationFile.getValue(FLUID.dye_force));
		m_splat_radius = std::stof(configurationFile.getValue(FLUID.dye_radius));
		m_dye_dissipation = std::stof(configurationFile.getValue(FLUID.dye_dissipation));
		m_velocity_dissipation = std::stof(configurationFile.getValue(FLUID.velocity_dissipation));
		m_diffuseIterations = std::stoi(configurationFile.getValue(FLUID.diffuseIterations));
		m_viscosity = std::stof(configurationFile.getValue(FLUID.viscosity));
		m_pressureIterations = std::stoi(configurationFile.getValue(FLUID.pressureIterations));
		m_pressure_dissipation = std::stof(configurationFile.getValue(FLUID.pressure_dissipation));
		m_vortitcity_scalar = std::stof(configurationFile.getValue(FLUID.vortitcity_scalar));
		m_timestep_scalar = std::stof(configurationFile.getValue(FLUID.timestep_scalar));

		m_splats = std::stoi(configurationFile.getValue(FLUID.splats));
		m_advect = configurationFile.getValue(FLUID.advect) == "1";
		m_diffuse = configurationFile.getValue(FLUID.diffuse) == "1";
		m_forces = configurationFile.getValue(FLUID.forces) == "1";
		m_project = configurationFile.getValue(FLUID.project) == "1";
		m_image = configurationFile.getValue(FLUID.image) == "1";
		m_visualize_change = configurationFile.getValue(FLUID.visualize_change) == "1";

		return 0;
	}
	catch (const std::exception& ex)
	{
		std::cout << "ERROR::PARSING::CONFIGURATION \"" << configurationFile.getPath() << "\" -> '" << ex.what() << "'" << std::endl;
		return -1;
	}
}

void FluidSimulation::updateConfiguration()
{
	fluid_config.updateValues();
	FluidSimulation::applyConfiguration(fluid_config);
}
