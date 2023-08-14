#include "FluidField.h"

void FluidField::Draw( glm::vec3 origin )
{

	m_primary_shader->use();
	glUniform1i( m_primary_shader->uniforms["u_image"], m_dye_buffer->readBuffer()->setTexture( 0 ) );
	glUniform2f( m_primary_shader->uniforms["dyeTexelSize"], m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY );
	glUniform2f( m_primary_shader->uniforms["velTexelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	if ( m_current_buffer != m_dye_buffer->readBuffer() )
	{
		glUniform1i( m_primary_shader->uniforms["u_image_overlay"], m_current_buffer->setTexture( 1 ) );
	}
	else
	{
		glUniform1i( m_primary_shader->uniforms["u_image_overlay"], 0 );
	}
	blit( nullptr, m_primary_shader );

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
void FluidField::blit( Framebuffer *target, Shader *shader )
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
	if ( target == nullptr || target->fbo == NULL )
	{
		glViewport( 0, 0, m_mouse.width, m_mouse.height );
		glBindFramebuffer( GL_FRAMEBUFFER, NULL );
	}
	else
	{
		glViewport( 0, 0, target->width, target->height );
		glBindFramebuffer( GL_FRAMEBUFFER, target->fbo );
	}


	//Draw to framebuffer
	if ( !target->status() )
	{
		std::cout << "ERROR::BLIT::FRAMEBUFFER::STATUS::INCOMPLETE" << std::endl;
		return;
	}

	m_fieldQuad->Draw( *shader );
	//Unbind framebuffer
	int boundBuffer = 0;
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &boundBuffer );
	if ( target != nullptr && boundBuffer == target->fbo )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

//Is not being used
void FluidField::boundaryContainer( bool l, bool r, bool t, bool b, Framebuffer *target, Shader &shader )
{
	line.width( 10.0f );
	glm::mat4 projectionM = glm::mat4( 1.0f );

	//projectionM = glm::ortho(0.0f, (float)1.0f, 0.0f, (float)1.0f, -1000.0f, 1000.0f);

	shader.use();
	shader.setMat4f( "projection", projectionM );
	glBindFramebuffer( GL_FRAMEBUFFER, target->fbo );

	if ( l )
	{
		line.start( 0.0f, 0.0f, 0.0f );
		line.end( 0.0f, 1.0f, 0.0f );
		line.Draw( shader );
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
	glBindFramebuffer( GL_FRAMEBUFFER, NULL );

}

//Advection -> Diffusion -> Force Application -> Projection
void FluidField::timeStep( float dt )
{
	float time = dt * m_timestep_scalar;
	if ( m_advect )	advect( time );		//Move the fluid and its quantities
	if ( m_diffuse )	diffuse( time );		//Spread out the fluid (if viscosity > 0)
	if ( m_forces )	addForces( time );	//Add a ball in the center and gravity. Also add multiple splats
	if ( m_project )	project( time );		//Remove unwanted stuff
}

//Is not being used
void FluidField::boundary( float dt, float scale, float offset, DoubleFramebuffer *target )
{
	m_bounds_shader.use();
	int uLoc = m_bounds_shader.uniforms["u"];
	int texelSizeLoc = m_bounds_shader.uniforms["texelSize"];
	int dtLoc = m_bounds_shader.uniforms["dt"];
	int offsetLoc = m_bounds_shader.uniforms["off"];
	int scaleLoc = m_bounds_shader.uniforms["scale"];
	glUniform1i( uLoc, target->readBuffer()->setTexture( 0 ) );
	glUniform2f( texelSizeLoc, target->readBuffer()->texelSizeX, target->readBuffer()->texelSizeY );
	glUniform1f( dtLoc, dt );
	glUniform1f( offsetLoc, offset );
	glUniform1f( scaleLoc, scale );

	boundaryContainer( 1, 1, 1, 1, target->writeBuffer(), m_bounds_shader );
}

//Isn't being used
void FluidField::boundaries( float dt )
{
//Velocity
//boundary(dt, 1.0f, 0, m_dye_buffer);
//Pressure
//boundary(dt, 0, 0, m_pressure_buffer);
}

//Add a value to an entire framebuffer
void FluidField::bufferIntegrate( DoubleFramebuffer *target, glm::vec4 values )
{
	m_integrate_shader.use();
	glUniform4f( m_integrate_shader.uniforms["value"], values.x, values.y, values.z, values.w );
	glUniform1i( m_integrate_shader.uniforms["uTexture"], target->readBuffer()->setTexture( 0 ) );
	glUniform2f( m_integrate_shader.uniforms["texelSize"], target->readBuffer()->texelSizeX, target->readBuffer()->texelSizeY );
	blit( target->writeBuffer(), &m_integrate_shader );
	target->swap();
}

//Calculate and add forces due to temprature (Is not being used)
void FluidField::temperature( float dt )
{
	m_temperature_shader.use();

	glUniform1i( m_temperature_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture( 0 ) );	//Velocity
	glUniform1i( m_temperature_shader.uniforms["t"], m_pressure_buffer->readBuffer()->setTexture( 1 ) );	//Temp
	glUniform1i( m_temperature_shader.uniforms["d"], m_density_buffer->readBuffer()->setTexture( 2 ) );		//Density

	glUniform1f( m_temperature_shader.uniforms["T0"], m_ambient_temperature );
	glUniform1f( m_temperature_shader.uniforms["k"], m_mass );
	glUniform1f( m_temperature_shader.uniforms["s"], m_temperature_scalar );
	glUniform1f( m_temperature_shader.uniforms["dt"], dt );
	glUniform2f( m_temperature_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );

	blit( m_velocity_buffer->writeBuffer(), &m_temperature_shader );
	m_velocity_buffer->swap();
}

//Move the quantities in the fluid
void FluidField::advect( float dt )
{
//Advection
	m_advection_shader.use();
	glUniform1f( m_advection_shader.uniforms["timestep"], dt ); //timestep = m_timestep
	glUniform1f( m_advection_shader.uniforms["dissipation"], m_velocity_dissipation );
	glUniform1i( m_advection_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture( 0 ) ); //u = velocity texture
	glUniform1i( m_advection_shader.uniforms["x"], m_velocity_buffer->readBuffer()->setTexture( 0 ) ); //x = quantity texture
	glUniform2f( m_advection_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	glUniform2f( m_advection_shader.uniforms["sourceTexelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	glUniform1i( m_advection_shader.uniforms["linearFiltering"], 1 );

	blit( m_velocity_buffer->writeBuffer(), &m_advection_shader );
	m_velocity_buffer->swap();

	//Advect dye
	glUniform1f( m_advection_shader.uniforms["dissipation"], m_dye_dissipation );
	glUniform1i( m_advection_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture( 0 ) ); //u = velocity texture
	glUniform1i( m_advection_shader.uniforms["x"], m_dye_buffer->readBuffer()->setTexture( 1 ) ); //x = quantity scalar texture
	glUniform2f( m_advection_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	glUniform2f( m_advection_shader.uniforms["sourceTexelSize"], m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY );

	blit( m_dye_buffer->writeBuffer(), &m_advection_shader );
	m_dye_buffer->swap();

	//Advect density
	/*glUniform1f(dissipationLoc, m_dye_dissipation);
	glUniform1i(xLoc, m_density_buffer->readBuffer()->setTexture(1)); //x = quantity scalar texture
	glUniform2f(texelLoc, m_density_buffer->readBuffer()->texelSizeX, m_density_buffer->readBuffer()->texelSizeY);
	blit(m_density_buffer->writeBuffer(), &m_advection_shader);
	m_density_buffer->swap();*/
}

//Diffusion, by using jacobi iterations, spread out the fluid
void FluidField::diffuse( float dt )
{
	if ( m_viscosity > 0 )
	{
		m_jacobi_iteration_shader.use();
		// Velocity
		float alpha = 1.0f / ( dt * m_viscosity ); //Alpha = pow(x, 2)/t
		float rBeta = 1.0f / ( 4.0f + alpha );	 //rBeta = 1/(4+Alpha)
		glUniform1f( m_jacobi_iteration_shader.uniforms["alpha"], alpha );
		glUniform1f( m_jacobi_iteration_shader.uniforms["rBeta"], rBeta );
		glUniform2f( m_jacobi_iteration_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
		glUniform1i( m_jacobi_iteration_shader.uniforms["b"], m_velocity_buffer->readBuffer()->setTexture( 0 ) );

		for ( unsigned int k = 0; k < m_diffuseIterations; k++ ) //20 to 50 iterations
		{
			glUniform1i( m_jacobi_iteration_shader.uniforms["x"], m_velocity_buffer->readBuffer()->setTexture( 1 ) ); // x = b = u (velocity)
			blit( m_velocity_buffer->writeBuffer(), &m_jacobi_iteration_shader );
			m_velocity_buffer->swap();
		}
		//Diffusion
		// Dye
		glUniform2f( m_jacobi_iteration_shader.uniforms["texelSize"], m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY );
		glUniform1i( m_jacobi_iteration_shader.uniforms["b"], m_dye_buffer->readBuffer()->setTexture( 0 ) );

		for ( unsigned int k = 0; k < m_diffuseIterations; k++ ) //20 to 50 iterations
		{
			glUniform1i( m_jacobi_iteration_shader.uniforms["x"], m_dye_buffer->readBuffer()->setTexture( 1 ) ); // x = b = u (dye)
			blit( m_dye_buffer->writeBuffer(), &m_jacobi_iteration_shader );
			m_dye_buffer->swap();
		}
	}
}

//Force Application
void FluidField::addForces( float dt )
{
	m_integrate_shader.use();
	float r = m_dye_radius / 10.0f;

	splat( glm::vec2( 0.5f, 0.9f ), r, m_splats, true, false );
	bufferIntegrate( m_velocity_buffer, glm::vec4( 0.0f, -150.82f, 0.0f, 0.0f ) * dt );
	//temperature(dt);
}

//Projection, by removing any divergence
void FluidField::project( float dt )
{
	if ( m_vortitcity_scalar > 0 )
	{
//Compute a normalized vorticity vector field
		curl( dt );
		//Restore, approximate, computated and dissipated vorticity
		vorticity( dt );
	}
	divergence( dt );
	clearBuffer( m_pressure_buffer, m_pressure_dissipation );
	pressure( dt );
	gradientSubtract( dt );
}

//Compute a normalized vorticity vector field
void FluidField::curl( float dt )
{
	m_curl_shader.use();
	glUniform1i( m_curl_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture( 0 ) );
	glUniform2f( m_curl_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	glUniform1f( m_curl_shader.uniforms["dt"], dt );
	blit( m_curl_buffer, &m_curl_shader );
}

//Restore, approximate, computated and dissipated vorticity
void FluidField::vorticity( float dt )
{
	m_vorticity_shader.use();
	glUniform1i( m_vorticity_shader.uniforms["u"], m_velocity_buffer->readBuffer()->setTexture( 0 ) );
	glUniform1i( m_vorticity_shader.uniforms["v"], m_curl_buffer->setTexture( 1 ) );
	glUniform2f( m_vorticity_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	glUniform1f( m_vorticity_shader.uniforms["dt"], dt );
	glUniform1f( m_vorticity_shader.uniforms["scale"], m_vortitcity_scalar );
	blit( m_velocity_buffer->writeBuffer(), &m_vorticity_shader );
	m_velocity_buffer->swap();
}

//Solve divergence
void FluidField::divergence( float dt )
{
	m_divergence_shader.use();
	glUniform1i( m_divergence_shader.uniforms["w"], m_velocity_buffer->readBuffer()->setTexture( 0 ) );
	glUniform2f( m_divergence_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	blit( m_divergence_buffer, &m_divergence_shader );
}

void FluidField::clearBuffer( DoubleFramebuffer *target, float value )
{
	m_clear_shader.use();
	glUniform1f( m_clear_shader.uniforms["value"], value );
	glUniform1i( m_clear_shader.uniforms["uTexture"], target->readBuffer()->setTexture( 0 ) );
	glUniform2f( m_clear_shader.uniforms["texelSize"], target->readBuffer()->texelSizeX, target->readBuffer()->texelSizeY );
	blit( target->writeBuffer(), &m_clear_shader );
	target->swap();
}

void FluidField::clearBuffer( Framebuffer *target, float value )
{
	m_clear_shader.use();
	glUniform1f( m_clear_shader.uniforms["value"], value );
	glUniform1i( m_clear_shader.uniforms["uTexture"], target->setTexture( 0 ) );
	glUniform2f( m_clear_shader.uniforms["texelSize"], target->texelSizeX, target->texelSizeY );
	blit( target, &m_clear_shader );
}

//Solve Pressure
void FluidField::pressure( float dt )
{
	m_jacobi_iteration_shader.use();

	float alpha = -pow( 1.0f, 2.0f ); //Alpha = -pow(x,2)
	float rBeta = 1.0f / 4.0f; //rBeta = 1/4
	glUniform1f( m_jacobi_iteration_shader.uniforms["alpha"], alpha );
	glUniform1f( m_jacobi_iteration_shader.uniforms["rBeta"], rBeta );
	glUniform2f( m_jacobi_iteration_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	glUniform1i( m_jacobi_iteration_shader.uniforms["b"], m_divergence_buffer->setTexture( 0 ) );	//b = gradient of w (divergent velocity)
	for ( unsigned int k = 0; k < m_pressureIterations; k++ )
	{
		glUniform1i( m_jacobi_iteration_shader.uniforms["x"], m_pressure_buffer->readBuffer()->setTexture( 1 ) ); //x = p (pressure)
		blit( m_pressure_buffer->writeBuffer(), &m_jacobi_iteration_shader );
		m_pressure_buffer->swap();
	}
}

//Subtract Gradient of pressure field from velocity field to achieve divergence free velocity
void FluidField::gradientSubtract( float dt )
{
	m_gradient_subtraction_shader.use();
	glUniform1i( m_gradient_subtraction_shader.uniforms["p"], m_pressure_buffer->readBuffer()->setTexture( 0 ) ); //p = pressure
	glUniform1i( m_gradient_subtraction_shader.uniforms["w"], m_velocity_buffer->readBuffer()->setTexture( 1 ) ); //w = divergent velocity
	glUniform2f( m_gradient_subtraction_shader.uniforms["texelSize"], m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	blit( m_velocity_buffer->writeBuffer(), &m_gradient_subtraction_shader );
	m_velocity_buffer->swap();
}

void FluidField::splat( glm::vec2 pos, float r, unsigned int amount, bool dye, bool velocity )
{
	m_splat_shader.use();
	glUniform1i( m_splat_shader.uniforms["amount"], amount );
	splat( pos, r, dye, velocity );
	glUniform1i( m_splat_shader.uniforms["amount"], 1 );
}

void FluidField::splat( glm::vec2 pos, float r, bool dye, bool velocity )
{
	m_splat_shader.use();
	//Uniforms
	int uColorLoc = m_splat_shader.uniforms["color"];
	int uTexLoc = m_splat_shader.uniforms["texelSize"];

	glUniform1i( m_splat_shader.uniforms["uTarget"], m_velocity_buffer->readBuffer()->setTexture( 0 ) );
	glUniform2f( m_splat_shader.uniforms["point"], pos.x, pos.y );
	glUniform3f( uColorLoc, m_mouse.texcoord_delta.x * m_dye_force, m_mouse.texcoord_delta.y * m_dye_force, 0.0f );

	//float ratio = m_mouse.width / m_mouse.height * 0.5f;
	//glUniform1f(m_splat_shader.uniforms["radius"], ratio > 1 ? r * ratio : r);
	glUniform1f( m_splat_shader.uniforms["radius"], r );
	glUniform2f( uTexLoc, m_velocity_buffer->readBuffer()->texelSizeX, m_velocity_buffer->readBuffer()->texelSizeY );
	if ( velocity )
	{
		blit( m_velocity_buffer->writeBuffer(), &m_splat_shader );
		m_velocity_buffer->swap();
	}

	glUniform1i( m_splat_shader.uniforms["uTarget"], m_dye_buffer->readBuffer()->setTexture( 0 ) );
	glUniform2f( uTexLoc, m_dye_buffer->readBuffer()->texelSizeX, m_dye_buffer->readBuffer()->texelSizeY );

	glm::vec3 color = glm::vec3( 0.0f );
	if ( m_dye_color_acc_dependent )
	{
		color = glm::normalize( glm::vec3( m_mouse.texcoord_delta.x * m_dye_force, m_mouse.texcoord_delta.y * m_dye_force, 0.2f ) );
		color *= m_dye_brightness;
	}
	else
	{
		color = glm::vec3( m_dye_color[0], m_dye_color[1], m_dye_color[2] );
		color *= m_dye_brightness;
	}
	glUniform3f( uColorLoc, abs( color.r ), abs( color.g ), abs( color.b + ( color.r + color.g ) / 5.0f ) * 0.3f );
	if ( dye )
	{
		blit( m_dye_buffer->writeBuffer(), &m_splat_shader );
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

void FluidField::updateMouse( double *mouseX, double *mouseY, bool *left_mouse_down, bool *right_mouse_down )
{
	m_mouse.update( *mouseX, *mouseY, *left_mouse_down, *right_mouse_down );

	if ( abs( m_mouse.window_delta.x ) > 0 || abs( m_mouse.window_delta.y ) > 0 )
	{
		if ( m_mouse.right_mouse_down && m_mouse.left_mouse_down )
		{
			splat( m_mouse.texcoord_pos, m_dye_radius, true, false );
		}
		else if ( m_mouse.right_mouse_down )
		{
			splat( m_mouse.texcoord_pos, m_dye_radius, false, true );
		}
		else if ( m_mouse.left_mouse_down )
		{
			splat( m_mouse.texcoord_pos, m_dye_radius, true, true );
		}
		//std::cout << "X: " << m_mouse.window_pos.x << " Y: " << m_mouse.window_pos.y << "tX: " << m_mouse.texcoord_pos.x << " tY: " << m_mouse.texcoord_pos.y << std::endl;
	}
	else if ( m_mouse.left_mouse_down )
	{
		splat( m_mouse.texcoord_pos, m_dye_radius, true, false );
	}
}

void FluidField::setCurrentBuffer( Framebuffer *buffer )
{
	if ( m_current_buffer == buffer ) return;
	m_current_buffer = buffer;
}

void FluidField::swapBuffer( int i )
{
	if ( i < 1 ) return;
	m_primary_shader->use();

	glUniform1i( m_primary_shader->uniforms["scene"], i - 1 );
	switch ( i )
	{
		case 1:
		{
			setCurrentBuffer( m_dye_buffer->readBuffer() );
			break;
		}
		case 2:
		{
			setCurrentBuffer( m_velocity_buffer->readBuffer() );
			break;
		}
		case 3:
		{
			setCurrentBuffer( m_divergence_buffer );
			break;
		}
		case 4:
		{
			setCurrentBuffer( m_pressure_buffer->readBuffer() );
			break;
		}
		case 5:
		{
			setCurrentBuffer( m_curl_buffer );
			break;
		}
		case 6:
		{
			setCurrentBuffer( m_temperature_buffer->readBuffer() );
			break;
		}
		case 7:
		{
			setCurrentBuffer( m_density_buffer->readBuffer() );
			break;
		}
		default:
		{
			std::cout << "BUFFER::NONE" << std::endl;
			break;
		}
	}
}

void FluidField::reset()
{
	clearBuffer( m_dye_buffer, 0.0 );
	clearBuffer( m_velocity_buffer, 0.0 );
	clearBuffer( m_pressure_buffer, 0.0 );
	clearBuffer( m_density_buffer, 0.0 );
	clearBuffer( m_curl_buffer, 0.0 );
}

int FluidField::applyConfiguration( Config &configurationFile )
{
	if ( configurationFile.size() <= 0 )
	{
		std::cout << "WARNING: Unable to read values from \"" << configurationFile.getPath() << "\"" << std::endl;
		return -1;
	}
	try
	{
		m_dye_scalar = std::stof( configurationFile.getValue( FLUID.dye_scalar ) );
		m_velocity_scalar = std::stof( configurationFile.getValue( FLUID.velocity_scalar ) );
		//m_dye_color = std::stof(configurationFile.getValue(FLUID.dye_color));
		m_dye_brightness = std::stof( configurationFile.getValue( FLUID.dye_brightness ) );
		m_dye_color_acc_dependent = configurationFile.getValue( FLUID.dye_color_acc_dependent ) == "1";
		m_dye_force = std::stof( configurationFile.getValue( FLUID.dye_force ) );
		m_dye_radius = std::stof( configurationFile.getValue( FLUID.dye_radius ) );
		m_dye_dissipation = std::stof( configurationFile.getValue( FLUID.dye_dissipation ) );
		m_velocity_dissipation = std::stof( configurationFile.getValue( FLUID.velocity_dissipation ) );
		m_diffuseIterations = std::stoi( configurationFile.getValue( FLUID.diffuseIterations ) );
		m_viscosity = std::stof( configurationFile.getValue( FLUID.viscosity ) );
		m_pressureIterations = std::stoi( configurationFile.getValue( FLUID.pressureIterations ) );
		m_pressure_dissipation = std::stof( configurationFile.getValue( FLUID.pressure_dissipation ) );
		m_vortitcity_scalar = std::stof( configurationFile.getValue( FLUID.vortitcity_scalar ) );
		m_timestep_scalar = std::stof( configurationFile.getValue( FLUID.timestep_scalar ) );

		m_splats = std::stoi( configurationFile.getValue( FLUID.splats ) );
		m_advect = configurationFile.getValue( FLUID.advect ) == "1";
		m_diffuse = configurationFile.getValue( FLUID.diffuse ) == "1";
		m_forces = configurationFile.getValue( FLUID.forces ) == "1";
		m_project = configurationFile.getValue( FLUID.project ) == "1";
		m_image = configurationFile.getValue( FLUID.image ) == "1";

		return 0;
	}
	catch ( const std::exception &ex )
	{
		std::cout << "ERROR::PARSING::CONFIGURATION \"" << configurationFile.getPath() << "\" -> '" << ex.what() << "'" << std::endl;
		return -1;
	}
}

void FluidField::updateConfiguration()
{
	fluid_config.updateValues();
	FluidField::applyConfiguration( fluid_config );
}
