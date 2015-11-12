#ifndef _TFGIZMO_H_
#define _TFGIZMO_H_

#include "gl\glew.h"
#include "glm\glm.hpp"
#include "TFShader.h"
#include "TFHandle.h"
#include <vector>

struct GizmoPoint{
	glm::vec3 vertex;
	glm::vec3 color;
};

enum {
	MAX_GIZMO_LINES = 100,
	MAX_GIZMO_POINTS = 100,
};

class TFGizmo{
private:
	GLuint m_vertexBuffer;
	GizmoPoint m_lines[MAX_GIZMO_LINES];
	GizmoPoint m_points[MAX_GIZMO_POINTS];
	int m_lineCount, m_pointCount;
public:
	TFGizmo() {

		glGenBuffers(1, &m_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		int sss = sizeof(m_lines);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_lines), nullptr, GL_DYNAMIC_DRAW);
		m_lineCount = 0;
		m_pointCount = 0;
	}

	~TFGizmo(){
		glDeleteBuffers(1, &m_vertexBuffer);
	}

	void drawLine(glm::vec3 color, glm::vec3 p1, glm::vec3 p2){
		//m_lines.push_back({ p1, color });
		//m_lines.push_back({ p2, color });
		if (m_lineCount == MAX_GIZMO_LINES - 1){
			return;
		}
		m_lines[m_lineCount++] = { p1, color };
		m_lines[m_lineCount++] = { p2, color };
	}

	void drawPoint(glm::vec3 color, glm::vec3 p){
		if (m_pointCount == MAX_GIZMO_POINTS - 1){
			return;
		}
		m_points[m_pointCount++] = { p, color };
	}

	void flush(){

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		GLvoid *vbo_buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(vbo_buffer, m_lines, m_lineCount * sizeof(GizmoPoint));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void *)0);

		TFHandle::checkOpenGL(__FILE__, __LINE__);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void *)(sizeof(glm::vec3)));
		TFHandle::checkOpenGL(__FILE__, __LINE__);

		TFHandle::checkOpenGL(__FILE__, __LINE__);
		//glDrawArrays(GL_LINES, 0, m_lines.size() * 6);
		glDrawArrays(GL_LINES, 0, 6 * m_lineCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		TFHandle::checkOpenGL(__FILE__, __LINE__);

		m_lineCount = 0;
		m_pointCount = 0;
	}
};


#endif