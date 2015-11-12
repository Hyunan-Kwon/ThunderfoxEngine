#ifndef _TFHANDLE_H_
#define _TFHANDLE_H_

#include <vector>
#include <string>
#include <ctime>

#define TF_DEBUG_MODE _DEBUG

#define TFLOG TFHandle::log
#define TFCHKGL TFHandle::checkOpenGL
#define TFEXIT TFHandle::exit
#define TFASSERT TFHandle::assertion

struct TFLogMessage{
	time_t		time;
	std::string text;

	std::string toString() const;
};

class TFHandle {
private:
	static std::vector<TFLogMessage> m_logMessages;
public:
	static void log(const char *message, ...);
	static void exit(const char *message = "");
	static void printLogAsFile(const char *filename = "log.txt");
	static bool checkOpenGL(const char *file, int line, const char *message = "", bool exitOnError = false);
	static void assertion(bool expression, const char *message = "");
};

#endif