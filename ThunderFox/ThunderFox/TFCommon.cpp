#include "TFCommon.h"
#include "TFObject.h"
#include "TFHandle.h"
#include "TFShaderManager.h"

namespace TFFramework{

	TFArrayBuffer *vertexBuffer_quad = nullptr, *uvBuffer_quad = nullptr;


	void init(){
		const GLfloat vertexData_quad[8] = {
			-1.0f, -1.0f,
			1.0f, -1.0f,
			1.0f, 1.0f,
			-1.0f, 1.0f
		};

		const GLfloat uvData_quad[8] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		vertexBuffer_quad = TFArrayBuffer::create(GL_ARRAY_BUFFER, sizeof(vertexData_quad), (GLvoid *)vertexData_quad, GL_STATIC_DRAW);
		vertexBuffer_quad->retain();
		uvBuffer_quad = TFArrayBuffer::create(GL_ARRAY_BUFFER, sizeof(uvData_quad), (GLvoid *)uvData_quad, GL_STATIC_DRAW);
		uvBuffer_quad->retain();
		
		TFShaderManager::getInstance()->init();
	};

	void cleanup(){
		vertexBuffer_quad->release();
		uvBuffer_quad->release();

		TFShaderManager::getInstance()->disenroll_all();
		TFRef::detectLeaking();
		TFRef::release_all();
		TFHandle::printLogAsFile();
	}
}