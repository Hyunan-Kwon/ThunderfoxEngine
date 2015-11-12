#ifndef _TFCONTROL_H_
#define _TFCONTROL_H_

#include "gl\glfw3.h"
#include "glm\gtc\matrix_transform.hpp"
#include "TFThunderFox.h"
#include <iostream>

using namespace std;

class TFControl{
private:
	GLFWwindow *ext_window;

	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	glm::vec3 m_position;
	float m_horizontalAngle;		// Toward : -z
	float m_verticalAngle;			// Look at the horizon.
	float m_initialFov;				// Field of view. 80' : very wide angle, huge deformations. 60'-45' : standard. 20' : big zoom.
	float m_speed;					// units per second;
	float m_mouseSpeed;
	glm::dvec2 m_lastMousePosition;
public:
	TFControl(GLFWwindow *window)
		:ext_window(window), m_horizontalAngle(glm::pi<float>()), m_verticalAngle(0.0f), m_initialFov(45.0f), m_speed(1.0f), m_mouseSpeed(6.0f){
	}

	void computeMatricesFromInputs(float deltaTime){
		glm::dvec2 mousePosition;
		glfwGetCursorPos(ext_window, &mousePosition.x, &mousePosition.y); // Get mouse position.

		if (glfwGetKey(ext_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			// Compute new orientation.
			m_horizontalAngle += m_mouseSpeed * glm::radians(float(m_lastMousePosition.x - mousePosition.x)) * deltaTime;
			m_verticalAngle += m_mouseSpeed * glm::radians(float(m_lastMousePosition.y - mousePosition.y)) * deltaTime;
			if (m_verticalAngle > glm::half_pi<float>()){
				m_verticalAngle = glm::half_pi<float>();
			}
			else if (m_verticalAngle < -glm::half_pi<float>()){
				m_verticalAngle = -glm::half_pi<float>();
			}

			glfwSetInputMode(ext_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			glm::ivec2 windowSize;
			glfwGetWindowSize(ext_window, &windowSize.x, &windowSize.y);
			glfwSetCursorPos(ext_window, windowSize.x * 0.5f, windowSize.y * 0.5f);
			mousePosition = glm::dvec2(windowSize.x * 0.5, windowSize.y * 0.5);
		}
		else if (glfwGetKey(ext_window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE){
			glfwSetInputMode(ext_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		m_lastMousePosition = mousePosition;

		// Direction : Spherical coordinates to Cartesian coordiantes.
		glm::vec3 direction(
			cos(m_verticalAngle) * sin(m_horizontalAngle),
			sin(m_verticalAngle),
			cos(m_verticalAngle) * cos(m_horizontalAngle)
			);
		// Right vector.
		glm::vec3 right(
			sin(m_horizontalAngle - glm::half_pi<float>()),
			0.0f,
			cos(m_horizontalAngle - glm::half_pi<float>())
			);
		// Up vector.
		glm::vec3 up = glm::cross(right, direction);

		if (glfwGetKey(ext_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			// Move forward.
			if (glfwGetKey(ext_window, GLFW_KEY_W) == GLFW_PRESS){
				m_position += direction * deltaTime * m_speed;
			}
			// Move backward;
			if (glfwGetKey(ext_window, GLFW_KEY_S) == GLFW_PRESS){
				m_position -= direction * deltaTime * m_speed;
			}
			// Strafe right.
			if (glfwGetKey(ext_window, GLFW_KEY_D) == GLFW_PRESS){
				m_position += right * deltaTime * m_speed;
			}
			// Strafe left.
			if (glfwGetKey(ext_window, GLFW_KEY_A) == GLFW_PRESS){
				m_position -= right * deltaTime * m_speed;
			}
		}

		float fov = m_initialFov;

		// 45' field of view, 4:3 ratio, display range : 0.1 unit <-> 100 units.
		m_projectionMatrix = glm::perspective(fov, 4.0f / 3.0f, 0.1f, 100.0f);
		m_viewMatrix = glm::lookAt(m_position, m_position + direction, up);
	}

	const glm::mat4& getViewMatirix() const{
		return m_viewMatrix;
	}
	const glm::mat4& getProjectionMatrix() const{
		return m_projectionMatrix;
	}
	const glm::vec3& getPosition() const{
		return m_position;
	}
};

#endif