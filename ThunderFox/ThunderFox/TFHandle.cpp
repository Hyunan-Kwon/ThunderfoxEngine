#include "TFHandle.h"
#include "gl\glew.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdarg>

std::string TFLogMessage::toString() const {
	tm tm;
	localtime_s(&tm, &time);
	std::stringstream ss;
	ss << 1900 + tm.tm_year << "-" << tm.tm_mon + 1 << "-" << tm.tm_mday << " " << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec;
	return ss.str() + " " + text;
}

std::vector<TFLogMessage> TFHandle::m_logMessages;

void TFHandle::log(const char *message, ...){
	va_list args;
	va_start(args, message);
	char buff[1024];
	vsprintf_s(buff, message, args);
	va_end(args);

	TFLogMessage logMessage = { time(nullptr), buff };
#if TF_DEBUG_MODE
	std::cout << logMessage.toString() << std::endl;
#endif
	m_logMessages.push_back(logMessage);
}

void TFHandle::exit(const char *message) {
	TFLOG("[APP STOP] %s", message);
	printLogAsFile();
	std::exit(EXIT_FAILURE);
}

void TFHandle::printLogAsFile(const char *filename) {
	std::ofstream stream(filename);
	for (std::vector<TFLogMessage>::const_iterator it = m_logMessages.begin(); it < m_logMessages.end(); ++it){
		stream << it->toString() << std::endl;
	}
}

bool TFHandle::checkOpenGL(const char *file, int line, const char *message, bool exitOnError){
	GLenum error = glGetError();
	if (error != GL_NO_ERROR){
		std::stringstream errorMessage;
		errorMessage << "[OpenGL ERROR]";
		switch (error){
		case GL_INVALID_ENUM:
			errorMessage << " Invalid enum. ";
			break;
		case GL_INVALID_VALUE:
			errorMessage << " Invalid value. ";
			break;
		case GL_INVALID_OPERATION:
			errorMessage << " Invalid operation. ";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errorMessage << " Invalid framebuffer operation. ";
			break;
		case GL_OUT_OF_MEMORY:
			errorMessage << " out of memory. ";
			break;
		}
		errorMessage << message;
#if TF_DEBUG_MODE
		errorMessage << "\n" << file << " " << line;
#endif
		//std::string temp = errorMessage.str();
		TFLOG("%s", errorMessage.str().c_str());

		if (exitOnError) { TFHandle::exit("Critical OpenGL error."); }

		return false;
	}

	return true;
}

void TFHandle::assertion(bool expression, const char *message) {
#if TF_DEBUG_MODE
	if (!expression){
		std::string _message = "Assertion failed. ";
		_message += message;
		TFEXIT(_message.c_str());
	}
#endif
}