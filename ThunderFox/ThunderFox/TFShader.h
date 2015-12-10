#ifndef _TFSHADER_H_
#define _TFSHADER_H_

#include "gl\glew.h"
#include "TFHandle.h"
#include "TFObject.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class TFShaderUnit : public TFRef, public TFGLObject{
protected:
	TFShaderUnit(GLenum type, const char *code){
		// Create shader from code.
		m_id = glCreateShader(type);
		glShaderSource(m_id, 1, &code, nullptr);
		glCompileShader(m_id);
	}
public:
	static TFShaderUnit* createWithString(GLenum type, const char *code){
		TFLOG("Compiling %s shader with string...", getShaderTypeString(type));
		return static_cast<TFShaderUnit *>((new TFShaderUnit(type, code))->autorelease());
	}

	static TFShaderUnit* createWithFile(GLenum type, const char *filename){
		// Read the shader code from the file.
		std::vector<char> code;
		std::ifstream stream(filename, std::ios::binary | std::ios::ate);
		if (stream.is_open()){
			unsigned int fileSize = stream.tellg();
			stream.close();
			stream.clear();
			stream.open(filename, std::ios::binary);
			code.resize(fileSize + 1);
			stream.read(code.data(), fileSize);
			stream.close();
		}
		else{
			TFLOG("Impossible to open \"%s\". Are you in the right directory?", filename);
			TFEXIT();
		}

		TFLOG("Compiling %s shader with \"%s\"...", getShaderTypeString(type), filename);
		return static_cast<TFShaderUnit *>((new TFShaderUnit(type, code.data()))->autorelease());
	}

	virtual ~TFShaderUnit(){
		glDeleteShader(m_id);
	}

	static const char * getShaderTypeString(GLenum type)
	{
		switch (type)
		{
		case GL_VERTEX_SHADER:			return "vertex";
		case GL_FRAGMENT_SHADER:		return "fragment";
		case GL_GEOMETRY_SHADER:		return "geometry";
		case GL_COMPUTE_SHADER:			return "compute";
		default:
			TFEXIT("Compiling shader failure. Unsupportable shader type.");
			return "";
		}
	}
};

class TFShader : public TFRef, public TFGLObject{
protected:
	std::map<std::string, GLint> m_uniformIDs;

	TFShader(const std::vector<TFShaderUnit *> &shaderUnits){
		// Link the program.
		TFLOG("Linking program...");
		m_id = glCreateProgram();
		for(auto &shaderUnit : shaderUnits){
			glAttachShader(m_id, shaderUnit->getID());
		}
		glLinkProgram(m_id);

		// Check the program
		GLint result = GL_FALSE;
		int infoLogLength;
		glGetProgramiv(m_id, GL_LINK_STATUS, &result);
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		//if (infoLogLength > 1){
		if (result == GL_FALSE){
			std::vector<char> errorMessage(infoLogLength);
			glGetProgramInfoLog(m_id, infoLogLength, nullptr, &errorMessage[0]);
			TFLOG(&errorMessage[0]);
			TFEXIT("Compling shader failed.");
		}
	}
public:
	static TFShader* create(TFShaderUnit *shaderUnit1, TFShaderUnit *shaderUnit2, TFShaderUnit *shaderUnit3 = nullptr){
		std::vector<TFShaderUnit *> shaderUnits;
		shaderUnits.push_back(shaderUnit1);
		shaderUnits.push_back(shaderUnit2);
		if (shaderUnit3 != nullptr){
			shaderUnits.push_back(shaderUnit3);
		}
		return static_cast<TFShader *>((new TFShader(shaderUnits))->autorelease());
	}

	static TFShader* create(TFShaderUnit *shaderUnit){
		return static_cast<TFShader *>((new TFShader(std::vector<TFShaderUnit *>(1, shaderUnit)))->autorelease());
	}

	static TFShader* createWithFile(const char *vertex_shader_filename, const char *fragment_shader_filename, const char *geometry_shader_filename = nullptr){
		std::vector<TFShaderUnit *> shaderUnits;
		shaderUnits.push_back(TFShaderUnit::createWithFile(GL_VERTEX_SHADER, vertex_shader_filename));
		shaderUnits.push_back(TFShaderUnit::createWithFile(GL_FRAGMENT_SHADER, fragment_shader_filename));
		if (geometry_shader_filename != nullptr){
			shaderUnits.push_back(TFShaderUnit::createWithFile(GL_GEOMETRY_SHADER, geometry_shader_filename));
		}
		return static_cast<TFShader *>((new TFShader(shaderUnits))->autorelease());
	}

	virtual ~TFShader(){
		glDeleteProgram(m_id);
	}

	void bind() const{
		glUseProgram(m_id);
	}
	void bindDefault() const{
		glUseProgram(0);
	}

	//@ Get uniform ID using glUniformLocation().
	GLint getUniformLocation(const char *uniformLocation) const{
		return glGetUniformLocation(m_id, uniformLocation);
	}

	//@ Save and map uniform ID
	void setUniformLocation(const char *uniformLcoation){
		m_uniformIDs[uniformLcoation] = glGetUniformLocation(m_id, uniformLcoation);
	}

	//@ Get mapped uniform ID.
	GLint getUniformID(const char *uniformLocation) const{
		//return m_uniformIDs[uniformLocation];
		auto &res = m_uniformIDs.find(uniformLocation);
		TFASSERT(res != m_uniformIDs.end(), "Getting uniform ID failed.");
		return res->second;
	}
};

#endif