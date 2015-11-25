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
		std::string strType;
		switch (type)
		{
		case GL_VERTEX_SHADER:
			strType = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			strType = "fragment";
			break;
		case GL_GEOMETRY_SHADER:
			strType = "geometry";
			break;
		case GL_COMPUTE_SHADER:
			strType = "compute";
			break;
		default:
			TFLOG("Compiling shader failure. Unsupported shader type.");
			return nullptr;
		}
		TFLOG("Compiling %s shader with string...", strType.c_str());

		return static_cast<TFShaderUnit *>((new TFShaderUnit(type, code))->autorelease());
	}

	static TFShaderUnit* createWithFile(GLenum type, const char *filename){
		// Read the shader code from the file.
		std::string code;
		std::ifstream stream(filename, std::ios::in);
		if (stream.is_open()){
			std::string line = "";
			while (std::getline(stream, line)){
				code += "\n" + line;
			}
			stream.close();
		}
		else{
			TFLOG("Impossible to open \"%s\". Are you in the right directory?", filename);
			TFEXIT();
		}

		std::string strType;
		switch (type)
		{
		case GL_VERTEX_SHADER:
			strType = "vertex";
			break;
		case GL_FRAGMENT_SHADER:
			strType = "fragment";
			break;
		case GL_GEOMETRY_SHADER:
			strType = "geometry";
			break;
		case GL_COMPUTE_SHADER:
			strType = "compute";
			break;
		default:
			TFEXIT("Compiling shader failure. Unsupported shader type.");
		}

		// Compile vertex shader.
		TFLOG("Compiling %s shader with \"%s\"...", strType.c_str(), filename);

		return static_cast<TFShaderUnit *>((new TFShaderUnit(type, code.c_str()))->autorelease());
	}

	virtual ~TFShaderUnit(){
		glDeleteShader(m_id);
	}
};

class TFShader : public TFRef, public TFGLObject{
protected:
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
			TFEXIT("Compiling shader program.");
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

	virtual ~TFShader(){
		glDeleteProgram(m_id);
	}

	GLint getUniformLocation(const char *uniformLocation) const{
		return glGetUniformLocation(m_id, uniformLocation);
	}
};

#endif