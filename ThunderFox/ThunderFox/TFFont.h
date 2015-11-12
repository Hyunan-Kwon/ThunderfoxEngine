#ifndef _TFFONT_H_
#define _TFFONT_H_

#include "glm\glm.hpp"
#include "TFTexture.h"
#include <string>
#include <vector>
#include "TFShader.h"
#include "TFCommon.h"
#include <cstdarg>

enum TFFontStyle {
	TF_FONT_STYLE_REGULAR, TF_FONT_STYLE_BOLD,
	TF_FONT_STYLE_ITALIC, TF_FONT_STYLE_BOLD_ITALIC,
};

const char *vertexShaderCode =
	"#version 330 core\n \
	layout(location = 0) in vec2 vertexPosition_screenspace; \
	noperspective out vec2 UV; \
	uniform sampler2D Tex_font; \
	uniform vec2 Position; \
	uniform int Char; \
	uniform ivec2 BaseCharSize; \
	uniform vec2 CharSize; \
	void main(){ \
		vec2 scale = vec2(BaseCharSize) / vec2(textureSize(Tex_font, 0)); \
		vec2 offset = vec2(float(Char % BaseCharSize.x) / float(BaseCharSize.x), \
			float(Char / BaseCharSize.y) / float(BaseCharSize.y)); \
		vec2 coord = (vertexPosition_screenspace * 0.5 + 0.5); \
		UV = coord * scale + offset; \
		offset = Position * 2.0 - 1.0 + CharSize; \
		offset.y = -offset.y; \
		gl_Position = vec4(vertexPosition_screenspace * CharSize + offset, 0.0, 1.0); \
	}";

const char *fragmentShaderCode =
	"#version 330 core\n \
	noperspective in vec2 UV; \
	out vec4 FragColor; \
	uniform sampler2D Tex_font; \
	uniform vec4 Color = vec4(1.0); \
	uniform vec4 StyleMask = vec4(0.0, 1.0, 0.0, 0.0); \
	void main(){ \
		vec4 letter = texture(Tex_font, UV) * StyleMask; \
		FragColor = Color * vec4(max(max(max(letter.r, letter.g), letter.b), letter.a)); \
	}";

class TFFont{
private:
	glm::ivec2 m_baseCharSize;
	glm::vec4 m_color;
	TFFontStyle m_style;
	glm::vec4 m_styleMask;
	TFShader *m_shader;
	TFTexture2D *m_texture;
public:
	TFFont() {
		//TFTextureManager::getInstance().loadTexture("font_base16", "Resources/Images/default16.png");
		m_baseCharSize = glm::ivec2(16, 16);
		m_color = glm::vec4(1.0f);
		setStyle(TF_FONT_STYLE_BOLD);

		m_shader = TFShader::create(TFShaderUnit::createWithString(GL_VERTEX_SHADER, vertexShaderCode), TFShaderUnit::createWithString(GL_FRAGMENT_SHADER, fragmentShaderCode));
		m_shader->getUniformLocation("Tex_font");
		m_shader->getUniformLocation("Color");
		m_shader->getUniformLocation("StyleMask");
		m_shader->getUniformLocation("Position");
		m_shader->getUniformLocation("Char");
		m_shader->getUniformLocation("BaseCharSize");
		m_shader->getUniformLocation("CharSize");
		m_shader->retain();

		m_texture = TFTexture2D::createWithFile("Resources/Images/default16.png");
		m_texture->retain();
	}

	~TFFont(){
		m_shader->release();
		m_texture->release();
	}

	inline void drawChar(int x, int y, char c){
		glm::ivec2 windowSize(1024, 768);
		drawChar(static_cast<float>(x) / static_cast<float>(windowSize.x), static_cast<float>(y) / static_cast<float>(windowSize.y), c);
	}

	void drawChar(float x, float y, char c){
		glm::vec2 windowsSize(1024.0f, 768.0f);
		glm::vec2 charSize = glm::vec2(m_baseCharSize) / windowsSize;

		glUseProgram(m_shader->getID());
		{
			glActiveTexture(GL_TEXTURE0);
			m_texture->bind();
			glUniform1i(m_shader->getUniformLocation("Tex_font"), 0);

			glUniform4f(m_shader->getUniformLocation("Color"), m_color.r, m_color.g, m_color.b, m_color.a);
			glUniform4f(m_shader->getUniformLocation("StyleMask"), m_styleMask.x, m_styleMask.y, m_styleMask.z, m_styleMask.w);
			glUniform2f(m_shader->getUniformLocation("Position"), x, y);
			glUniform1i(m_shader->getUniformLocation("Char"), static_cast<int>(c));
			glUniform2i(m_shader->getUniformLocation("BaseCharSize"), m_baseCharSize.x, m_baseCharSize.y);
			glUniform2f(m_shader->getUniformLocation("CharSize"), charSize.x, charSize.y);

			glEnableVertexAttribArray(0);
			TFFramework::vertexBuffer_quad->bind();
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glDisable(GL_BLEND);

			glDisableVertexAttribArray(0);
		}
	}

	void drawText(float x, float y, const char *text, ...){
		va_list args;
		va_start(args, text);
		char buff[1024];
		vsprintf_s(buff, text, args);
		va_end(args);

		glm::ivec2 windowSize(1024, 768);
		drawText(static_cast<int>(x * static_cast<float>(windowSize.x)), static_cast<int>(y * static_cast<float>(windowSize.y)), buff);
	}

	void drawText(int x, int y, const char *text, ...){
		va_list args;
		va_start(args, text);
		char buff[1024];
		vsprintf_s(buff, text, args);
		va_end(args);

		int _x = x;
		char *c = buff;
		while (*c != '\0'){
			if (*c == '\n'){
				_x = x;
				y += m_baseCharSize.y;
			}
			else{
				drawChar(_x, y, *c);
				_x += m_baseCharSize.x;
			}

			++c;
		}
	}

	const glm::vec4& getColor() const { return m_color; }
	void setColor(int r, int g, int b, int a){
		m_color = glm::vec4(r, g, b, a) * PER255F;
	}
	void setColor(float r, float g, float b, float a){
		m_color = glm::vec4(r, g, b, a);
	}

	TFFontStyle getStyle() const { return m_style; }
	void setStyle(TFFontStyle style){
		m_style = style;
		switch (style)
		{
		case TF_FONT_STYLE_REGULAR:
			m_styleMask = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			break;
		case TF_FONT_STYLE_BOLD:
			m_styleMask = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			break;
		case TF_FONT_STYLE_ITALIC:
			m_styleMask = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
			break;
		case TF_FONT_STYLE_BOLD_ITALIC:
			m_styleMask = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		default:
			break;
		}
	}
};

#endif