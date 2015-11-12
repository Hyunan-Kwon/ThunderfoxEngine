#ifndef _TFTHUNDERFOX_H_
#define _TFTHUNDERFOX_H_

#define GLFW_DLL
#include "gl\glew.h"
#include "gl\glfw3.h"
#include "glm\glm.hpp"
#include "TFObject.h"
#include "TFCommon.h"
#include "TFControl.h"
#include "TFShader.h"
#include "TFModel.h"
#include "TFTextureManager.h"
#include "TFGizmo.h"
#include "TFHandle.h"
#include "TFShader.h"
#include "TFShaderManager.h"
#include "TFFont.h"
#include "TFTransform.h"
#include "TFBuffer.h"
#include "TFAutoreleasePool.h"
#include "TFPrimitive.h"
#include "TFTimer.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <deque>

#pragma comment(lib, "gl\\glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gl\\glfw3dll.lib")
#pragma comment(lib, "FreeImage\\FreeImage.lib")

enum TFWindowSize{
	TF_WINDOWSIZE_1024_768,
};

class TFThunderFox{
private:
	std::string m_windowTitle;
	TFWindowSize m_windowSize;

	//double m_lastTime = 0.0;
	//float m_deltaTime;
	TFFrameRateTimer *m_framerateTimer;

	GLFWwindow *m_window;

	TFThunderFox(TFThunderFox const&);
	TFThunderFox operator = (TFThunderFox const&);
public:
	TFThunderFox()
	:m_windowTitle("ThunderFox"), m_windowSize(TF_WINDOWSIZE_1024_768) {//, m_lastTime(0.0), m_deltaTime(0.0f){
		// Init GLFW
		if (glfwInit() == GL_FALSE){
			fprintf(stderr, "Failed to initiate GLFW\n");
			exit(EXIT_FAILURE);
		}
		glfwWindowHint(GLFW_SAMPLES, 4); // 4x Antialiasing.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Opengl 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL.

		// Open a window and create its OpenGL context.
		m_window = glfwCreateWindow(1024, 768, "ThunderFox", nullptr, nullptr);
		if (m_window == nullptr){
			fprintf(stderr, "Failed to open GLFW window, If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(m_window);
		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key.
		glfwSetCursorPos(m_window, 1024.0 / 2.0, 768.0 / 2.0);

		// Init glew.
		glewExperimental = true; // Needed for core profile.
		if (glewInit() != GLEW_OK){
			fprintf(stderr, "Failed to initiate GLEW\n");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		TFCHKGL(__FILE__, __LINE__, "GLEW error?");

		TFFramework::init();

		TFTransform *temp = TFTransform::create();
		TFTransform *temp_c1 = TFTransform::create();
		TFTransform *temp_c2 = TFTransform::create();
		temp->addChild(temp_c1);
		temp->addChild(temp_c2);
		//temp->release();

		//m_lastTime = glfwGetTime();
		m_framerateTimer = TFFrameRateTimer::create();
		m_framerateTimer->retain();
		//m_framerateTimer->start();
	}

	~TFThunderFox(){
		TFFramework::cleanup();
		m_framerateTimer->release();
		glfwTerminate();
	}

	void run(){
		TFControl control(m_window);

		//TFTexture2D *tttt = TFTexture2D::createWithFile("meshes/san-miguel/Maps/madera_barandal_esc_2.jpg");
		//TFTextureManager::getInstance().loadTexture("TEST", "meshes/san-miguel/Maps/madera_barandal_esc_2.jpg");

		GLuint vertexArrayID;
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		/////////////////////////////////// FRAMEBUFFER


		TFTexture2D *tex_render = TFTexture2D::createEmpty(GL_RGBA, 1024, 768, GL_RGBA, GL_FLOAT);
		tex_render->retain();
		TFTexture2D *tex_normal = TFTexture2D::createEmpty(GL_RGB, 1024, 768, GL_RGB, GL_FLOAT);
		tex_normal->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		tex_normal->retain();
		TFTexture2D *tex_position = TFTexture2D::createEmpty(GL_RGB, 1024, 768, GL_RGB, GL_FLOAT);
		tex_position->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		tex_position->retain();
		TFTexture2D *tex_visibility = TFTexture2D::createEmpty(GL_R32F, 1024, 768, GL_RED, GL_FLOAT);
		tex_visibility->retain();
		TFTexture2D *tex_depth = TFTexture2D::createEmpty(GL_DEPTH_COMPONENT, 1024, 768, GL_DEPTH_COMPONENT, GL_FLOAT);
		tex_depth->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		tex_depth->retain();
		TFRenderBuffer *renderbuffer_depth = TFRenderBuffer::create(GL_DEPTH_COMPONENT, 1024, 768);
		renderbuffer_depth->retain();

		TFFrameBuffer *framebufferSSAO = TFFrameBuffer::create();
		framebufferSSAO->retain();
		framebufferSSAO->attachColor(tex_render);
		framebufferSSAO->attachColor(tex_normal);
		framebufferSSAO->attachColor(tex_position);
		framebufferSSAO->attachColor(tex_visibility);
		framebufferSSAO->attachDepth(tex_depth);
		//framebufferSSAO->attachDepthStencil(tex_depth);
		//framebufferSSAO->attachDepth(renderbuffer_depth);
		//framebufferSSAO->attachDepthStencil(renderbuffer_depth);
		framebufferSSAO->windup();

		//TFTexture2D *tex_depth = TFTexture2D::createEmpty(GL_DEPTH_COMPONENT, 4096, 4096, GL_DEPTH_COMPONENT, GL_FLOAT);
		//tex_depth->retain();

		TFFrameBuffer *framebuffer_shadowmap = TFFrameBuffer::create();
		framebuffer_shadowmap->retain();
		framebuffer_shadowmap->attachDepth(tex_depth);
		framebuffer_shadowmap->windup();

		const GLubyte* a = glGetString(GL_VERSION);
		const GLubyte* b = glGetString(GL_SHADING_LANGUAGE_VERSION);

		TFCHKGL(__FILE__, __LINE__);

		TFTransform *world = TFTransform::create();
		world->retain();
		//world->scale(0.5, 0.5, 0.5);
		//world->rotate(0.0f, 0.0f, 90.0f);
		////////////////////////////////////////////////

		//TFModel model("meshes/Charizard/", "BR_Charizard.obj");
		//TFModel model("meshes/Pikachu/", "XY_PikachuM.obj");
		//TFModel model("meshes/Elephant/", "elephant.obj");
		
		//TFModel model("meshes/Head/", "head.obj");

		TFTransform *temp = TFTransform::create();
		temp->retain();
		temp->translate(0.7, -0.7, 0.0);
		TFModel model_portalgun("meshes/PortalGun/PortalGun.tfm");
		temp->addChild(&model_portalgun);
		model_portalgun.scale(0.8, 0.8, 0.8);
		model_portalgun.rotate(-30, 120, 0);

		TFModel model("meshes/crytek-sponza/sponza.tfm");
		//TFModel model("meshes/Gemini/Gemini.tfm");
		//TFModel model("meshes/bunny.tfm");
		//TFModel model("meshes/san-miguel/san-miguel.tfm");
		world->addChild((TFTransform *)&model);
		model.scale(0.005, 0.005, 0.005);
		//model.scale(2, 1, 1);
		//model.rotate(45, 0, -90);
		//model.translate(1, 0, 0);

		std::vector<GLuint> uniforms;
		TFShader *shader = TFShaderManager::getInstance()->getShader("DirectionalLight");
		uniforms.push_back(shader->getUniformLocation("MaterialAmbient"));
		uniforms.push_back(shader->getUniformLocation("MaterialDiffuse"));
		uniforms.push_back(shader->getUniformLocation("MaterialSpecular"));
		uniforms.push_back(shader->getUniformLocation("MaterialShininess"));

		glm::vec3 lightPosition(1.0f, 3.0f, 1.0f);
		glm::vec3 lightColor(1.0, 244.0 / 255.0, 229.0 / 255.0);
		GLfloat lightPower = 3.0f;
		glm::vec2 shift(5.0f / 1024.0f, 0.0f / 768.0f);

		TFCHKGL(__FILE__, __LINE__);

		TFGizmo gizmo;
		TFFont font;
		TFSphere *s = TFSphere::create(3);
		s->retain();
		s->scale(0.5, 0.5, 0.5);

		glm::mat4 depth_projMatrix = glm::ortho<float>(-5, 5, -5, 5, -10, 10);
		glm::mat4 depth_viewMatrix;
		glm::mat4 depth_MVP;

		//std::deque<float> deltaTimes;

		do{
			//double currentTime = glfwGetTime();
			//m_deltaTime = float(currentTime - m_lastTime);
			//deltaTimes.push_back(m_deltaTime);
			//if (deltaTimes.size() > 30){
			//	deltaTimes.pop_front();
			//}
			//float avg = 0.0f;
			//for (float &deltaTime : deltaTimes){
			//	avg += deltaTime;
			//}
			//avg /= deltaTimes.size();

			m_framerateTimer->start();

			control.computeMatricesFromInputs(m_framerateTimer->getDeltaTime());
			depth_viewMatrix = glm::lookAt<float>(lightPosition + control.getPosition(), control.getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));

			framebuffer_shadowmap->bind();
			glViewport(0, 0, 4096, 4096);
			glClear(GL_DEPTH_BUFFER_BIT);

			//shader = TFShaderManager::getInstance()->getShader("ShadowMap");
			//glUseProgram(shader->getID());
			//{
			//	//depth_MVP = model.getMatrix() * depth_viewMatrix * depth_projMatrix;
			//	depth_MVP = depth_projMatrix * depth_viewMatrix * model.getMatrix();
			//	glUniformMatrix4fv(shader->getUniformLocation("MVP"), 1, GL_FALSE, &depth_MVP[0][0]);

			//	glEnable(GL_DEPTH_TEST);
			//	//glEnable(GL_CULL_FACE);
			//	//glCullFace(GL_FRONT);

			//	model.draw(uniforms);
			//	//for (int i = 0; i < model.m_shapes.size(); ++i){
			//	//	TFShape &shape = model.m_shapes[i];

			//	//	glEnableVertexAttribArray(0);
			//	//	glBindBuffer(GL_ARRAY_BUFFER, model.m_vertexBuffers[i]);
			//	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			//	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.m_elementBuffers[i]);
			//	//	for (int j = 0; j < shape.mesh.materialInfo.ranges.size() - 1; ++j){
			//	//		GLuint start = shape.mesh.materialInfo.ranges[j];
			//	//		GLuint end = shape.mesh.materialInfo.ranges[j + 1];
			//	//		glDrawRangeElements(GL_TRIANGLES, 0, shape.mesh.indices.size(), end - start, GL_UNSIGNED_INT, (void *)(start * sizeof(GLuint)));
			//	//	}

			//	//	glDisableVertexAttribArray(0);
			//	//}

			//	//glDisable(GL_CULL_FACE);
			//	glDisable(GL_DEPTH_TEST);
			//}
			//TFCHKGL(__FILE__, __LINE__, "Shadowmap error", true);

			framebufferSSAO->bind();
			glViewport(0, 0, 1024, 768);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader = TFShaderManager::getInstance()->getShader("DirectionalLight");
			glUseProgram(shader->getID());
			{
				glm::mat4 biasMatrix(
					0.5, 0.0, 0.0, 0.0,
					0.0, 0.5, 0.0, 0.0,
					0.0, 0.0, 0.5, 0.0,
					0.5, 0.5, 0.5, 1.0);
				depth_MVP = biasMatrix * depth_MVP;
				glUniformMatrix4fv(shader->getUniformLocation("M"), 1, GL_FALSE, &(model.getMatrix()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("V"), 1, GL_FALSE, &(control.getViewMatirix()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("P"), 1, GL_FALSE, &(control.getProjectionMatrix()[0][0]));
				glUniform3f(shader->getUniformLocation("LightColor"), lightColor.r, lightColor.g, lightColor.b);
				//lightPosition = glm::vec3(glm::rotate<float>(glm::mat4(), glm::half_pi<float>() * 0.05f, glm::vec3(0, 1, 0)) * glm::vec4(lightPosition, 1));
				glUniform3f(shader->getUniformLocation("LightDirection_worldspace"), lightPosition.x, lightPosition.y, lightPosition.z);
				glUniformMatrix4fv(shader->getUniformLocation("DepthMVP"), 1, GL_FALSE, &depth_MVP[0][0]);
				
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);

				gizmo.drawLine(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

				//glActiveTexture(GL_TEXTURE1);
				//tex_depth->bind();
				//glUniform1i(shader->getUniformLocation("Tex_shadowMap"), 1);

				glActiveTexture(GL_TEXTURE0);
				glUniform1i(shader->getUniformLocation("Texture00"), 0);
				model.draw(uniforms);

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
			}
			{
				glm::vec3 _lightColor = lightColor * lightPower;
				glUniformMatrix4fv(shader->getUniformLocation("M"), 1, GL_FALSE, &(model_portalgun.getMatrix()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("V"), 1, GL_FALSE, &(glm::mat4()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("P"), 1, GL_FALSE, &(glm::mat4()[0][0]));
				glUniform3f(shader->getUniformLocation("LightColor"), _lightColor.r, _lightColor.g, _lightColor.b);
				glUniform3f(shader->getUniformLocation("LightDirection_worldspace"), lightPosition.x, lightPosition.y, lightPosition.z);

				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);

				model_portalgun.draw(uniforms);

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
			}
			TFCHKGL(__FILE__, __LINE__);
			
			shader = TFShaderManager::getInstance()->getShader("Compute");
			glUseProgram(shader->getID());
			{
				glUniform1f(shader->getUniformLocation("roll"), (float)m_framerateTimer->getDeltaTime());

				glActiveTexture(GL_TEXTURE0);
				//tex_depth->bind();
				glBindImageTexture(0, tex_visibility->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
				TFCHKGL(__FILE__, __LINE__);
				glUniform1i(shader->getUniformLocation("destTex"), 0);

				glDispatchCompute(1024 / 16, 768 / 16, 1);
			}
			TFCHKGL(__FILE__, __LINE__);

			shader = TFShaderManager::getInstance()->getShader("SingleColor");
			glUseProgram(shader->getID());
			{
				glUniformMatrix4fv(shader->getUniformLocation("M"), 1, GL_FALSE, &(s->getMatrix()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("V"), 1, GL_FALSE, &(control.getViewMatirix()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("P"), 1, GL_FALSE, &(control.getProjectionMatrix()[0][0]));
				glUniform3f(shader->getUniformLocation("Color"), 0.5, 0.5, 0.5);

				glEnableVertexAttribArray(0);
				s->vertexBuffer->bind();
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

				glEnableVertexAttribArray(2);
				s->normalBuffer->bind();
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

				s->elementBuffer->bind();

				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glEnable(GL_DEPTH_TEST);

				glDrawElements(GL_TRIANGLES, s->m_faces.size() * 3, GL_UNSIGNED_INT, (void *)0);

				glDepthFunc(GL_ALWAYS);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glUniform3f(shader->getUniformLocation("Color"), 0.0, 0.0, 0.0);
				glDrawElements(GL_TRIANGLES, s->m_faces.size() * 3, GL_UNSIGNED_INT, (void *)0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDepthFunc(GL_LESS);
				
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}

			shader = TFShaderManager::getInstance()->getShader("Gizmo");
			glUseProgram(shader->getID());
			{
				glUniformMatrix4fv(shader->getUniformLocation("MVP"), 1, GL_FALSE, &((control.getProjectionMatrix() * control.getViewMatirix())[0][0]));

				gizmo.flush();
			}

			// FrameBuffer
			TFFrameBuffer::bindDefault();
			glViewport(0, 0, 1024, 768);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader = TFShaderManager::getInstance()->getShader("SSAO");
			glUseProgram(shader->getID());
			{
				glUniform1i(shader->getUniformLocation("DepthTexture"), 0);
				glActiveTexture(GL_TEXTURE0);
				tex_depth->bind();

				glUniform1i(shader->getUniformLocation("NormalTexture"), 1);
				glActiveTexture(GL_TEXTURE1);
				tex_normal->bind();

				glUniform1i(shader->getUniformLocation("RenderedTexture"), 2);
				glActiveTexture(GL_TEXTURE2);
				tex_render->bind();

				glUniform1i(shader->getUniformLocation("PositionTexture"), 3);
				glActiveTexture(GL_TEXTURE3);
				tex_position->bind();

				glUniformMatrix4fv(shader->getUniformLocation("P"), 1, GL_FALSE, &control.getProjectionMatrix()[0][0]);

				glEnableVertexAttribArray(0);
				TFFramework::vertexBuffer_quad->bind();
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				glDisableVertexAttribArray(0);
			}
			TFCHKGL(__FILE__, __LINE__, "", true);

			//shader = TFShaderManager::getInstance()->getShader("Screen");
			//glUseProgram(shader->getID());
			//{
			//	glActiveTexture(GL_TEXTURE0);
			//	tex_render->bind();
			//	//tex_visibility->bind();
			//	//tex_depth->bind();
			//	glUniform1i(shader->getUniformLocation("Tex_render"), 0);

			//	glActiveTexture(GL_TEXTURE1);
			//	tex_visibility->bind();
			//	glUniform1i(shader->getUniformLocation("Tex_visibility"), 1);

			//	glEnableVertexAttribArray(0);
			//	TFFramework::vertexBuffer_quad->bind();
			//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

			//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			//	glDisableVertexAttribArray(0);
			//}

			font.setStyle(TF_FONT_STYLE_BOLD);
			font.setColor(0.0f, 0.0f, 0.0f, 1.0f);
			font.drawText(0, 0, "FPS : %.1lf", m_framerateTimer->getFPS());
			
			font.setStyle(TF_FONT_STYLE_REGULAR);
			font.setColor(1.0f, 1.0f, 1.0f, 1.0f);
			font.drawText(0, 0, "FPS : %.1lf", m_framerateTimer->getFPS());

			glfwSwapBuffers(m_window);
			glfwPollEvents();

			TFAutoreleasePool::getInstance()->clean();
			//m_lastTime = currentTime;
			m_framerateTimer->stop();
		} while (glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
			&& glfwWindowShouldClose(m_window) == 0);

		//delete tex_depth, tex_normal, tex_position, tex_render;
		framebufferSSAO->release();
		world->release();

		glDeleteVertexArrays(1, &vertexArrayID);
	}

	void setWindowTitle(const char *windowTitle){
		m_windowTitle = windowTitle;
		glfwSetWindowTitle(m_window, windowTitle);
	}
	std::string getWindowTitle() const{
		return m_windowTitle;
	}
	void setWindowSize(TFWindowSize windowSize){
		if (windowSize == m_windowSize){
			return;
		}

		switch (windowSize){
		case TF_WINDOWSIZE_1024_768:
			m_windowSize = windowSize;
			glfwSetWindowSize(m_window, 1024, 768);
			break;
		}
	}
	//float getDeltaTime() const{
	//	return m_deltaTime;
	//}
};

#endif