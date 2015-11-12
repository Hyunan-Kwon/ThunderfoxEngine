//#ifndef _TFCAMERA_H_
//#define _TFCAMERA_H_
//
//#include "gl\glfw3.h"
//#include "TFTransform.h"
//
//class TFCamera : public TFTransform{
//private:
//	GLFWwindow *ext_window;
//protected:
//	float m_fov;
//	float m_speed;					// units per second;
//	float m_mouseSpeed;
//	glm::dvec2 m_lastMousePosition;
//public:
//	TFCamera(GLFWwindow *window)
//		:ext_window(window), m_horizontalAngle(glm::pi<float>()), m_verticalAngle(0.0f), m_fov(45.0f), m_speed(1.0f), m_mouseSpeed(6.0f){
//	}
//
//	void computeMatricesFromInputs(float deltaTime){
//		glm::dvec2 mousePosition;
//		glfwGetCursorPos(ext_window, &mousePosition.x, &mousePosition.y); // Get mouse position.
//
//		float &horizontalAngle = m_rotation.y;
//		float &verticalAngle = m_rotation.x;
//
//		if (glfwGetKey(ext_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
//			// Compute new orientation.
//			horizontalAngle += m_mouseSpeed * deltaTime * float(m_lastMousePosition.x - mousePosition.x);
//			verticalAngle += m_mouseSpeed * deltaTime * float(m_lastMousePosition.y - mousePosition.y);
//			if (verticalAngle > glm::half_pi<float>()){
//				verticalAngle = glm::half_pi<float>();
//			}
//			else if (verticalAngle < -glm::half_pi<float>()){
//				verticalAngle = -glm::half_pi<float>();
//			}
//
//			glfwSetInputMode(ext_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
//			glm::ivec2 windowSize;
//			glfwGetWindowSize(ext_window, &windowSize.x, &windowSize.y);
//			glfwSetCursorPos(ext_window, windowSize.x * 0.5f, windowSize.y * 0.5f);
//			mousePosition = glm::dvec2(windowSize.x * 0.5, windowSize.y * 0.5);
//		}
//		else if (glfwGetKey(ext_window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE){
//			glfwSetInputMode(ext_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//		}
//
//		m_lastMousePosition = mousePosition;
//
//		// Direction : Spherical coordinates to Cartesian coordiantes.
//		glm::vec3 direction(
//			cos(verticalAngle) * sin(horizontalAngle),
//			sin(verticalAngle),
//			cos(verticalAngle) * cos(horizontalAngle)
//			);
//		// Right vector.
//		glm::vec3 right(
//			sin(horizontalAngle - glm::half_pi<float>()),
//			0.0f,
//			cos(horizontalAngle - glm::half_pi<float>())
//			);
//		// Up vector.
//		glm::vec3 up = glm::cross(right, direction);
//
//		if (glfwGetKey(ext_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
//			// Move forward.
//			if (glfwGetKey(ext_window, GLFW_KEY_W) == GLFW_PRESS){
//				m_position += direction * deltaTime * m_speed;
//			}
//			// Move backward;
//			if (glfwGetKey(ext_window, GLFW_KEY_S) == GLFW_PRESS){
//				m_position -= direction * deltaTime * m_speed;
//			}
//			// Strafe right.
//			if (glfwGetKey(ext_window, GLFW_KEY_D) == GLFW_PRESS){
//				m_position += right * deltaTime * m_speed;
//			}
//			// Strafe left.
//			if (glfwGetKey(ext_window, GLFW_KEY_A) == GLFW_PRESS){
//				m_position -= right * deltaTime * m_speed;
//			}
//		}
//
//		// 45' field of view, 4:3 ratio, display range : 0.1 unit <-> 100 units.
//		m_projectionMatrix = glm::perspective(m_fov, 4.0f / 3.0f, 0.1f, 100.0f);
//		m_viewMatrix = glm::lookAt(m_position, m_position + direction, up);
//	}
//
//	glm::mat4 getViewMatrix(){
//	}
//};
//
//
//#endif