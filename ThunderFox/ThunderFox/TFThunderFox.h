#pragma once

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

//void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);

class TFThunderFox{
private:
	GLFWwindow *m_window;
	std::string m_windowTitle;

	TFThunderFox(TFThunderFox const&);
	TFThunderFox operator = (TFThunderFox const&);

	TFFrameRateTimer *m_framerateTimer;
	TFTransform *m_world;
	TFControl *m_control;
public:
	TFThunderFox()
	:m_windowTitle("ThunderFox") {
		// Init GLFW
		if (glfwInit() == GL_FALSE){
			fprintf(stderr, "Failed to initiate GLFW\n");
			exit(EXIT_FAILURE);
		}
		//glfwWindowHint(GLFW_SAMPLES, 4); // 4x Antialiasing.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Opengl 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL.
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		// Open a window and create its OpenGL context.
		//m_window = glfwCreateWindow(1024, 768, "ThunderFox", glfwGetPrimaryMonitor(), nullptr); // full screen.
		m_window = glfwCreateWindow(1024, 768, "ThunderFox", nullptr, nullptr);
		if (m_window == nullptr){
			glfwTerminate();
			TFEXIT("Failed to open GLFW window, If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.");
		}
		glfwMakeContextCurrent(m_window);
		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key.
		glfwSetCursorPos(m_window, 1024.0 / 2.0, 768.0 / 2.0);
		//glfwSetKeyCallback(m_window, keyboard);

		// Init glew.
		glewExperimental = true; // Needed for core profile.
		if (glewInit() != GLEW_OK){
			glfwTerminate();
			TFEXIT("Failed to initiate GLEW");
		}
		TFCHKGL(__FILE__, __LINE__, "GLEW error?");

		TFFramework::init();

		m_framerateTimer = TFFrameRateTimer::create();
		m_framerateTimer->retain();

		m_world = TFTransform::create();
		m_world->retain();
		//world->scale(0.5, 0.5, 0.5);
		//world->rotate(0.0f, 0.0f, 90.0f);

		m_control = new TFControl(m_window);
	}

	~TFThunderFox(){
		m_framerateTimer->release();
		m_world->release();
		delete m_control;

		TFFramework::cleanup();
		glfwTerminate();
	}

	void run(){
		GLuint vertexArrayID;
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);


		TFTexture2D *tex_depth = TFTexture2D::createEmpty(GL_DEPTH_COMPONENT, 4096, 4096, GL_DEPTH_COMPONENT, GL_FLOAT);
		tex_depth->retain();

		TFFrameBuffer *framebuffer_shadowmap = TFFrameBuffer::create();
		framebuffer_shadowmap->attachDepth(tex_depth);
		framebuffer_shadowmap->windup();
		framebuffer_shadowmap->retain();
		TFCHKGL(__FILE__, __LINE__);
	
		////////////////////////////////////////////////

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
		m_world->addChild((TFTransform *)&model);
		model.scale(0.005, 0.005, 0.005);
		//model.scale(2, 1, 1);
		//model.rotate(45, 0, -90);
		//model.translate(1, 0, 0);

		glm::vec3 lightPosition(1.0f, 3.0f, 1.0f);
		glm::vec3 lightColor(1.0, 244.0 / 255.0, 229.0 / 255.0);
		GLfloat lightPower = 3.0f;
		glm::vec2 shift(5.0f / 1024.0f, 0.0f / 768.0f);

		TFFont font;

		glm::mat4 depth_projMatrix = glm::ortho<float>(-5, 5, -5, 5, -10, 10);
		glm::mat4 depth_viewMatrix;
		glm::mat4 depth_MVP;


		TFTexture2DMultisample *tex_gbuffer_position = TFTexture2DMultisample::createEmpty(4, GL_RGB32F, 1024, 768, GL_TRUE);
		tex_gbuffer_position->retain();
		TFTexture2DMultisample *tex_gbuffer_albedo = TFTexture2DMultisample::createEmpty(4, GL_RGBA, 1024, 768, GL_TRUE);
		tex_gbuffer_albedo->retain();
		TFTexture2DMultisample *tex_gbuffer_normal = TFTexture2DMultisample::createEmpty(4, GL_RGB16F, 1024, 768, GL_TRUE);
		tex_gbuffer_normal->retain();

		TFRenderBufferMultisample *renderbuffer_gbuffer_depth = TFRenderBufferMultisample::create(4, GL_DEPTH24_STENCIL8, 1024, 768);
		renderbuffer_gbuffer_depth->retain();

		TFFrameBuffer *framebuffer_gbuffer = TFFrameBuffer::create();
		framebuffer_gbuffer->attachColor(tex_gbuffer_position);
		framebuffer_gbuffer->attachColor(tex_gbuffer_albedo);
		framebuffer_gbuffer->attachColor(tex_gbuffer_normal);
		framebuffer_gbuffer->attachDepthStencil(renderbuffer_gbuffer_depth);
		framebuffer_gbuffer->windup();
		framebuffer_gbuffer->retain();

		TFTexture2DMultisample *tex_final_render = TFTexture2DMultisample::createEmpty(4, GL_RGBA, 1024, 768, GL_TRUE);
		tex_final_render->retain();

		TFRenderBufferMultisample *renderbuffer_final_depth = TFRenderBufferMultisample::create(4, GL_DEPTH24_STENCIL8, 1024, 768);
		renderbuffer_final_depth->retain();
		
		TFFrameBuffer *framebuffer_final = TFFrameBuffer::create();
		framebuffer_final->attachColor(tex_final_render);
		framebuffer_final->attachDepthStencil(renderbuffer_final_depth);
		framebuffer_final->windup();
		framebuffer_final->retain();

		TFShader *shader_gbuffer = TFShader::createWithFile("shaders/GBuffer.vs.glsl", "shaders/Gbuffer.fs.glsl");
		std::vector<GLuint> uniforms;
		uniforms.push_back(shader_gbuffer->getUniformLocation("MaterialAmbient"));
		uniforms.push_back(shader_gbuffer->getUniformLocation("MaterialDiffuse"));
		uniforms.push_back(shader_gbuffer->getUniformLocation("MaterialSpecular"));
		uniforms.push_back(shader_gbuffer->getUniformLocation("MaterialShininess"));
		shader_gbuffer->retain();
		TFCHKGL(__FILE__, __LINE__, "", true);

		TFShader *shader_gbuffer_test = TFShader::createWithFile("shaders/Screen.vert", "shaders/DeferredTest.fs.glsl");
		shader_gbuffer_test->setUniform("viewport", glm::vec2(1024.0f, 768.0f));
		shader_gbuffer_test->setUniform("nSamples", 4);
		shader_gbuffer_test->setUniform("lightColor", lightColor);
		shader_gbuffer_test->retain();
		TFCHKGL(__FILE__, __LINE__, "", true);

		do{
			m_framerateTimer->elapse();

			m_control->computeMatricesFromInputs(m_framerateTimer->getDeltaTime());
			depth_viewMatrix = glm::lookAt<float>(lightPosition + m_control->getPosition(), m_control->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
			TFCHKGL(__FILE__, __LINE__, "", true);

			framebuffer_gbuffer->bind();
			GLfloat clearcolor_rgb[] = { 1, 1, 1 };
			GLfloat clearcolor_rgba[] = { 1, 1, 1, 1 };
			glClearBufferfv(GL_COLOR, 0, clearcolor_rgb);
			glClearBufferfv(GL_COLOR, 1, clearcolor_rgba);
			glClearBufferfv(GL_COLOR, 2, clearcolor_rgb);
			glClearDepth(1.0);
			glClear(GL_DEPTH_BUFFER_BIT);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			shader_gbuffer->bind();
			{
				glm::mat4 VM = model_portalgun.getMatrix();
				shader_gbuffer->setUniform("M", VM);
				//shader_gbuffer->setUniform("PV", m_control->getProjectionMatrix());
				shader_gbuffer->setUniform("PV", glm::mat4());
				shader_gbuffer->setUniform("M_normal", glm::inverse(VM), GL_TRUE);

				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_ALWAYS, 1, 0xff);
				glStencilMask(0xff);

				model_portalgun.draw(uniforms);

				glDisable(GL_STENCIL_TEST);
				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
			}
			{
				glm::mat4 VM = m_control->getViewMatirix() * model.getMatrix();
				shader_gbuffer->setUniform("M", VM);
				shader_gbuffer->setUniform("PV", m_control->getProjectionMatrix());
				shader_gbuffer->setUniform("M_normal", glm::inverse(VM), GL_TRUE);

				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_NOTEQUAL, 1, 0xff);
				glStencilMask(0x00);

				model.draw(uniforms);

				glDisable(GL_STENCIL_TEST);
				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
			}
			TFCHKGL(__FILE__, __LINE__, "", true);

			TFFrameBuffer::bindDefault();
			{
				shader_gbuffer_test->bind();
				{
					glActiveTexture(GL_TEXTURE0);
					tex_gbuffer_position->bind();

					glActiveTexture(GL_TEXTURE1);
					tex_gbuffer_albedo->bind();

					glActiveTexture(GL_TEXTURE2);
					tex_gbuffer_normal->bind();

					glEnableVertexAttribArray(0);
					TFFramework::vertexBuffer_quad->bind();
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

					shader_gbuffer_test->setUniform("V", m_control->getViewMatirix());
					shader_gbuffer_test->setUniform("lightDirection_worldspace", lightPosition);

					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

					glDisableVertexAttribArray(0);
				}
			}
			TFCHKGL(__FILE__, __LINE__, "", true);
			
			
			//framebuffer_shadowmap->bind();
			//glViewport(0, 0, 4096, 4096);
			//glClear(GL_DEPTH_BUFFER_BIT);

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
			/*
			glBindFramebuffer(GL_FRAMEBUFFER, framebufferSSAO->getID());
			//framebufferSSAO->bind();
			glViewport(0, 0, 1024, 768);
			glClearColor(1, 1, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			shader = TFShaderManager::getInstance()->getShader("SSAO");
			glUseProgram(shader->getID());
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

				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_ALWAYS, 1, 0xff);
				glStencilMask(0xff);

				model_portalgun.draw(uniforms);

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_STENCIL_TEST);
			}
			{
				glm::mat4 biasMatrix(
					0.5, 0.0, 0.0, 0.0,
					0.0, 0.5, 0.0, 0.0,
					0.0, 0.0, 0.5, 0.0,
					0.5, 0.5, 0.5, 1.0);
				depth_MVP = biasMatrix * depth_MVP;
				glUniformMatrix4fv(shader->getUniformLocation("M"), 1, GL_FALSE, &(model.getMatrix()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("V"), 1, GL_FALSE, &(m_control->getViewMatirix()[0][0]));
				glUniformMatrix4fv(shader->getUniformLocation("P"), 1, GL_FALSE, &(m_control->getProjectionMatrix()[0][0]));
				glUniform3f(shader->getUniformLocation("LightColor"), lightColor.r, lightColor.g, lightColor.b);
				//lightPosition = glm::vec3(glm::rotate<float>(glm::mat4(), glm::half_pi<float>() * 0.05f, glm::vec3(0, 1, 0)) * glm::vec4(lightPosition, 1));
				glUniform3f(shader->getUniformLocation("LightDirection_worldspace"), lightPosition.x, lightPosition.y, lightPosition.z);
				glUniformMatrix4fv(shader->getUniformLocation("DepthMVP"), 1, GL_FALSE, &depth_MVP[0][0]);
				
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);


				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_NOTEQUAL, 1, 0xff);
				glStencilMask(0x00);

				//gizmo.drawLine(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

				//glActiveTexture(GL_TEXTURE1);
				//tex_depth->bind();
				//glUniform1i(shader->getUniformLocation("Tex_shadowMap"), 1);

				glActiveTexture(GL_TEXTURE0);
				glUniform1i(shader->getUniformLocation("Texture00"), 0);
				model.draw(uniforms);

				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_STENCIL_TEST);
			}
			TFCHKGL(__FILE__, __LINE__);
		
			//shader = TFShaderManager::getInstance()->getShader("Gizmo");
			//glUseProgram(shader->getID());
			//{
			//	glUniformMatrix4fv(shader->getUniformLocation("MVP"), 1, GL_FALSE, &((control.getProjectionMatrix() * control.getViewMatirix())[0][0]));

			//	gizmo.flush();
			//}
			//TFCHKGL(__FILE__, __LINE__, "", true);

			// FrameBuffer
			//TFFrameBuffer::bindDefault();
			//glViewport(0, 0, 1024, 768);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//framebufferSSAO->bind();
			//framebufferSSAO->blitToDefault();


			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferSSAO->getID());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, 1024, 768, 0, 0, 1024, 768, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//shader = TFShaderManager::getInstance()->getShader("SSAO");
			//glUseProgram(shader->getID());
			//{
			//	//glUniform1i(shader->getUniformLocation("DepthTexture"), 0);
			//	//glActiveTexture(GL_TEXTURE0);
			//	//tex_depth->bind();

			//	//glUniform1i(shader->getUniformLocation("NormalTexture"), 1);
			//	//glActiveTexture(GL_TEXTURE1);
			//	//tex_normal->bind();


			//	//glBindSampler(2, samplerID);

			//	//glUniform1i(shader->getUniformLocation("RenderedTexture"), 2);
			//	glActiveTexture(GL_TEXTURE2);
			//	tex_render->bind();
			//	TFCHKGL(__FILE__, __LINE__, "", true);

			//	//glUniform1i(shader->getUniformLocation("PositionTexture"), 3);
			//	//glActiveTexture(GL_TEXTURE3);
			//	//tex_position->bind();

			//	//glUniform1i(shader->getUniformLocation("PaperTexture"), 4);
			//	//glActiveTexture(GL_TEXTURE4);
			//	//tex_paper->bind();

			//	glUniform3f(shader->getUniformLocation("LightColor"), lightColor.x, lightColor.y, lightColor.z);
			//	glUniformMatrix4fv(shader->getUniformLocation("P"), 1, GL_FALSE, &control.getProjectionMatrix()[0][0]);
			//	glUniform1f(shader->getUniformLocation("edge0"), edge0);
			//	glUniform1f(shader->getUniformLocation("edge1"), edge1);
			//	TFCHKGL(__FILE__, __LINE__, "", true);

			//	glEnableVertexAttribArray(0);
			//	TFFramework::vertexBuffer_quad->bind();
			//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
			//	TFCHKGL(__FILE__, __LINE__, "", true);

			//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			//	TFCHKGL(__FILE__, __LINE__, "", true);

			//	glDisableVertexAttribArray(0);
			//}
			//TFCHKGL(__FILE__, __LINE__, "", true);
			//glBindSampler(2, 0);


			*/

			font.setStyle(TF_FONT_STYLE_BOLD);
			font.setColor(0.0f, 0.0f, 0.0f, 1.0f);
			font.drawText(0, 0, "FPS : %.1lf", m_framerateTimer->getFPS());
			font.drawText(0, 15, "Depta time : %lf", m_framerateTimer->getDeltaTime());
			font.drawText(0, 30, "ms per frame : %.1lfms", m_framerateTimer->getMPF());

			font.setStyle(TF_FONT_STYLE_REGULAR);
			font.setColor(1.0f, 1.0f, 1.0f, 1.0f);
			font.drawText(0, 0, "FPS : %.1lf", m_framerateTimer->getFPS());
			font.drawText(0, 15, "Depta time : %lf", m_framerateTimer->getDeltaTime());
			font.drawText(0, 30, "ms per frame : %.1lfms", m_framerateTimer->getMPF());


			TFAutoreleasePool::getInstance()->clean();

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		} while (glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
			&& glfwWindowShouldClose(m_window) == 0);

		glDeleteVertexArrays(1, &vertexArrayID);
	}

	void setWindowTitle(const char *windowTitle){
		m_windowTitle = windowTitle;
		glfwSetWindowTitle(m_window, windowTitle);
	}
	std::string getWindowTitle() const{
		return m_windowTitle;
	}
};