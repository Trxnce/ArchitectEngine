#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "Transform.h"

ShaderProgram::ShaderProgram(const std::string& _vert, const std::string& _frag)
{
	std::string vertShader;
	std::string fragShader;

	std::ifstream file(_vert);

	if (!file.is_open())
	{
		std::cout << "Error opening file at: " << _vert << std::endl;
	}
	else
	{
		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			vertShader += line + "\n";
		}
	}
	file.close();

	file.open(_frag);

	if (!file.is_open())
	{
		std::cout << "Error opening file at: " << _frag << std::endl;
	}
	else
	{
		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			fragShader += line + "\n";
		}
	}
	file.close();

	const char* vertex = vertShader.c_str();
	const char* fragment = fragShader.c_str();

	const GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertex, NULL);
	glCompileShader(vertexShaderId);
	GLint success = 0;
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vertexShaderId, maxLength, &maxLength, &errorLog[0]);
		for (auto& i : errorLog)
			std::cout << i;
		std::cout << "Error compiling the vertex shader" << std::endl;
	}

	const GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragment, NULL);
	glCompileShader(fragmentShaderId);
	success = 0;
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fragmentShaderId, maxLength, &maxLength, &errorLog[0]);
		for (auto& i : errorLog)
			std::cout << i;
		std::cout << "Error compiling the fragment shader" << std::endl;
	}

	id = glCreateProgram();
	glAttachShader(id, vertexShaderId);
	glAttachShader(id, fragmentShaderId);

	glBindAttribLocation(id, 0, "in_Position");
	glBindAttribLocation(id, 1, "in_Normal");
	glBindAttribLocation(id, 2, "in_TexCoord");

	glLinkProgram(id);
	success = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		std::cout << "Error linking the program" << std::endl;
	}

	glDetachShader(id, vertexShaderId);
	glDeleteShader(vertexShaderId);
	glDetachShader(id, fragmentShaderId);
	glDeleteShader(fragmentShaderId);
}

void ShaderProgram::draw(const std::shared_ptr<VertexArray>& _vertexArray, GLuint _texID, std::shared_ptr<Camera> _camera, std::shared_ptr<Entity> _entity, GLuint _id)
{
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glBindTexture(GL_TEXTURE_2D, _texID);
	if (_id)
		glUseProgram(_id);
	else
		glUseProgram(id);
	glBindVertexArray(_vertexArray->GetId());

	const glm::vec3 pos = _entity->getComponent<TransformComponent>()->getPos();
	const glm::vec3 rot = _entity->getComponent<TransformComponent>()->getRot();
	const glm::vec3 sca = _entity->getComponent<TransformComponent>()->getScale();

	glm::mat4 model = _entity->getComponent<Transform>()->updateModelMatrix(model, pos, rot, sca);

	const glm::mat4 view = _camera->getViewMatrix();
	const glm::mat4 projection = glm::perspective(glm::radians(_camera->Zoom), (float)1280 / (float)720, 0.1f, 100.0f);
	SetUniform("modelMatrix", model);
	SetUniform("viewMatrix", view);
	SetUniform("projectionMatrix", projection);
	glDrawArrays(GL_TRIANGLES, 0, _vertexArray->GetVertexCount());
	glBindVertexArray(0);
	glUseProgram(0);
}

//TODO Convert all these to LearnOpenGL Tutorials
void ShaderProgram::SetUniform(const std::string& _uniform, const glm::vec3& _value)
{
	//glUseProgram(id);
	glUniform3fv(glGetUniformLocation(id, _uniform.c_str()), 1, &_value[0]);
	//glUseProgram(0);
}

void ShaderProgram::SetUniform(const std::string& _uniform, const glm::vec4& _value)
{
	//glUseProgram(id);
	glUniform4fv(glGetUniformLocation(id, _uniform.c_str()), 1, &_value[0]);
	//glUseProgram(0);
}

void ShaderProgram::SetUniform(const std::string& _uniform, const glm::mat4& _value)
{
	//glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, _uniform.c_str()), 1, GL_FALSE, &_value[0][0]);
	//glUseProgram(0);
}

void ShaderProgram::SetUniform(const std::string& _uniform, const int _value)
{
	//glUseProgram(id);
	glUniform1f(glGetUniformLocation(id, _uniform.c_str()), _value);
	//glUseProgram(0);
}

GLuint ShaderProgram::GetId() const
{
	return id;
}