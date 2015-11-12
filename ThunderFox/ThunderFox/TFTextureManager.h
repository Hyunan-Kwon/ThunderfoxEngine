#ifndef _TFTEXTUREMANAGER_H_
#define _TFTEXTUREMANAGER_H_

#include "gl\glew.h"
#include "TFTexture.h"
#include <string>
#include <map>

class TFTextureManager {
private:
	TFTextureManager();
	TFTextureManager(TFTextureManager const&);
	void operator=(TFTextureManager const&);
	virtual ~TFTextureManager();
	
	std::map<std::string, unsigned int> mm_textures;
public:
	static TFTextureManager& getInstance();

	bool loadTexture(std::string sKey, std::string sFileName, GLint level = 0, GLint border = 0);
	bool unloadTexture(std::string sKey);
	unsigned int findTextureIdByKey(std::string sKey);
	std::string toString() const;
};

#endif