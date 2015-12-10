//#pragma once
//
//#include "gl\glfw3.h"
//#include "glm\glm.hpp"
//#include "glm\gtc\matrix_transform.hpp"
//#include "TFObject.h"
//
//class TFCamera : public TFRef{
//protected:
//	GLFWwindow *m_window;
//
//	glm::vec3 m_position;
//	glm::vec3 m_direction;
//
//	glm::mat4 m_viewMatrix;
//	glm::mat4 m_projectionMatrix;
//	glm::mat4 m_invProjectionMatrix;
//
//	TFCamera(GLFWwindow *window)
//		: m_window(window),
//		m_projectionMatrix(glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f)), m_invProjectionMatrix(glm::inverse(m_projectionMatrix))
//	{
//
//	}
//public:
//	static TFCamera* create(GLFWwindow *window){
//		return static_cast<TFCamera *>((new TFCamera(window))->autorelease());
//	}
//
//	void setViewport(GLint x, GLint y, GLsizei width, GLsizei height) const{
//		glViewport(x, y, width, height);
//	}
//
//	//@ Set the projection mode as pespective.
//	void setPerspective(float fov, float aspect, float near, float far){
//		m_projectionMatrix = glm::perspective(fov, aspect, near, far);
//		m_invProjectionMatrix = glm::inverse(m_projectionMatrix);
//	}
//
//	//@ Set the projection mode as orthogonal.
//	void setOrthogonal(float left, float right, float bottom, float top, float near, float far){
//		m_projectionMatrix = glm::ortho(left, right, top, bottom, near, far);
//		m_invProjectionMatrix = glm::inverse(m_projectionMatrix);
//	}
//
//	//@ Get the view matrix which is calculated at every frame.
//	const glm::mat4& getViewMatrix() const			{ return m_viewMatrix;					}
//
//	//@ Caculate an inverse view matrix and get it.
//	const glm::mat4& getInvViewMatrix() const		{ return glm::inverse(m_viewMatrix);	}
//
//	//@ Get the projection matrix.
//	const glm::mat4& getProjectionMatrix() const	{ return m_projectionMatrix;			}
//
//	//@ Get the pre-caculated projection matrix;
//	const glm::mat4& getInvProjectionMatrix() const	{ return m_invProjectionMatrix;			}
//
//	//@ Get the position of this camera.
//	const glm::vec3& getPosition() const			{ return m_position;					}
//
//	//@ Get the direction of this camera.
//	const glm::vec3& getDirection() const			{ return m_direction;					}
//};
//
//class TFMainCamera : TFCamera{
//
//};