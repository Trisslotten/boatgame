#include "shader.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>


#define uniformDef(glFunc, glmtype, basetype) \
void ShaderProgram::uniformv(const std::string & name, GLuint count, const glmtype * values) \
{ glFunc(findUniformLocation(name), count, (basetype*) values); } \
void ShaderProgram::uniform(const std::string & name, const glmtype & value) \
{ uniformv(name, 1, &value); }

#define uniformMatrixDef(glFunc, glmtype, basetype) \
void ShaderProgram::uniformv(const std::string & name, GLuint count, const glmtype * values) \
{ glFunc(findUniformLocation(name), count, GL_FALSE, (basetype*) values); } \
void ShaderProgram::uniform(const std::string & name, const glmtype & value) \
{ uniformv(name, 1, &value); }


void checkLinkError(GLuint id, const std::string& paths)
{
	GLint success = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_size = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_size);
		if (log_size > 0)
		{
			std::vector<GLchar> error(log_size);
			glGetProgramInfoLog(id, log_size, &log_size, &error[0]);
			std::string errorstr{ &error[0] };

			std::cout << "Error in:\n" << paths << "\n" << errorstr << "\n";

			glDeleteProgram(id);
		}
		//system("pause");
	}
}


GLuint compileShader(GLenum type, const std::string& name)
{
	GLuint shader = glCreateShader(type);

	//std::string path = "assets/shaders/" + name;
	std::string path = name;
	std::ifstream shaderFile(path);
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	if (!shaderFile.is_open())
	{
		std::cout << "[ERROR] Could not open file: " << path << "\n";
		//system("pause");
		//exit(1);
	}

	shaderFile.close();
	const char* shaderTextPtr = shaderText.c_str();

	glShaderSource(shader, 1, &shaderTextPtr, nullptr);
	glCompileShader(shader);

	// Check for compile error
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_size = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
		std::vector<GLchar> error(log_size);
		glGetShaderInfoLog(shader, log_size, &log_size, &error[0]);
		std::string errorstr{ &error[0] };

		std::cout << "Error in '" << name << "':\n" << errorstr << "\n";

		glDeleteShader(shader);
		//system("pause");
	}

	return shader;
}

ShaderProgram::ShaderProgram()
{
	id = 0;
}

ShaderProgram::~ShaderProgram()
{
	for (auto& i : ids)
	{
		glDeleteShader(i.second);
	}
	glDeleteProgram(id);
}
void ShaderProgram::add(GLenum type, const std::string& path)
{
	glDeleteShader(ids[type]);
	paths[type] = SHADERS_PATH + path;
	ids[type] = 0;
}

void ShaderProgram::add(const std::string& path)
{
	
	std::string extension = path.substr(path.size() - 4, 4);

	GLenum shaderType = GL_FRAGMENT_SHADER;

	if (extension == "vert")
		shaderType = GL_VERTEX_SHADER;

	if (extension == "tesc")
		shaderType = GL_TESS_CONTROL_SHADER;

	if (extension == "tese")
		shaderType = GL_TESS_EVALUATION_SHADER;

	if (extension == "geom")
		shaderType = GL_GEOMETRY_SHADER;

	if (extension == "frag")
		shaderType = GL_FRAGMENT_SHADER;

	if (extension == "comp")
		shaderType = GL_COMPUTE_SHADER;

	add(shaderType, path);
}


void ShaderProgram::compile()
{
	//std::cout << "[DEBUG] Compiling Shaders\n";

	std::string paths_str;

	glDeleteProgram(id);
	for (auto& path : paths)
	{
		glDeleteShader(ids[path.first]);
		ids[path.first] = compileShader(path.first, path.second);

		paths_str += path.second + "\n";
	}

	id = glCreateProgram();
	for (auto& i : ids)
	{
		glAttachShader(id, i.second);
	}
	glLinkProgram(id);

	checkLinkError(id, paths_str);

	compiled = true;
}


void ShaderProgram::reload()
{
	for(auto elem : ids)
		glDeleteShader(elem.second);

	glDeleteProgram(id);

	std::cout << "[DEBUG] Reloading Shaders:\n";
	for (auto elem : paths)
	{
		std::cout << "  " << elem.second << "\n";
	}

	compile();
}

void ShaderProgram::use()
{
	glUseProgram(id);
}


GLuint ShaderProgram::findUniformLocation(const std::string & name)
{
	auto it = uniform_locations.find(name);
	GLuint uniform_location;
	if (it == uniform_locations.end())
	{
		uniform_location = glGetUniformLocation(id, name.c_str());
		if (uniform_location == -1)
		{
			std::cout << "ERROR: could not find '" << name << "' in shader\n";
		}
		uniform_locations[name] = uniform_location;
	}
	else
		uniform_location = it->second;
	return uniform_location;
}

uniformDef(glUniform1fv, GLfloat,   GLfloat);
uniformDef(glUniform2fv, glm::vec2, GLfloat);
uniformDef(glUniform3fv, glm::vec3, GLfloat);
uniformDef(glUniform4fv, glm::vec4, GLfloat);

uniformDef(glUniform1iv, GLint,      GLint);
uniformDef(glUniform2iv, glm::ivec2, GLint);
uniformDef(glUniform3iv, glm::ivec3, GLint);
uniformDef(glUniform4iv, glm::ivec4, GLint);

uniformDef(glUniform1uiv, GLuint,     GLuint);
uniformDef(glUniform2uiv, glm::uvec2, GLuint);
uniformDef(glUniform3uiv, glm::uvec3, GLuint);
uniformDef(glUniform4uiv, glm::uvec4, GLuint);

uniformMatrixDef(glUniformMatrix2fv, glm::mat2, GLfloat);
uniformMatrixDef(glUniformMatrix3fv, glm::mat3, GLfloat);
uniformMatrixDef(glUniformMatrix4fv, glm::mat4, GLfloat);

uniformMatrixDef(glUniformMatrix2x3fv, glm::mat2x3, GLfloat);
uniformMatrixDef(glUniformMatrix3x2fv, glm::mat3x2, GLfloat);

uniformMatrixDef(glUniformMatrix2x4fv, glm::mat2x4, GLfloat);
uniformMatrixDef(glUniformMatrix4x2fv, glm::mat4x2, GLfloat);

uniformMatrixDef(glUniformMatrix3x4fv, glm::mat3x4, GLfloat);
uniformMatrixDef(glUniformMatrix4x3fv, glm::mat4x3, GLfloat);