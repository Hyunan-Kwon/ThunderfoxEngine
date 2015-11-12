#include "TFTextureManager.h"
#include "FreeImage\FreeImage.h"
#include "TFHandle.h"
#include <iostream>
#include <sstream>

TFTextureManager::TFTextureManager() {
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
	#endif
}

TFTextureManager::~TFTextureManager() {
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
	#endif

	std::map<std::string, unsigned int>::iterator it = mm_textures.begin();
	while (it != mm_textures.end()){
		glDeleteTextures(1, &(it->second));
		++it;
	}
	mm_textures.clear();
}

TFTextureManager& TFTextureManager::getInstance() {
	static TFTextureManager rm;
	return rm;
}

bool TFTextureManager::loadTexture(std::string sKey, std::string sFileName, GLint level, GLint border) {
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);

	// If key exists, return failure.
	if (mm_textures.find(sKey) != mm_textures.end()){
		//TFLOG("Loading \"%s\" failure. The key is already exist.", sFileName.c_str());
		return false;
	}

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(sFileName.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN){
		fif = FreeImage_GetFIFFromFilename(sFileName.c_str());
	}
	//if still unkown, return failure
	if (fif == FIF_UNKNOWN){
		TFLOG("Loading \"%s\" failure. fif unknown.", sFileName.c_str());
		return false;
	}
	// If image foramt is unsupportable, return failure.
	if (fif != FIF_BMP && fif != FIF_JPEG && fif != FIF_GIF && fif != FIF_PNG && fif != FIF_TARGA){
		TFLOG("Loading \"%s\" failure. unsupportable format.", sFileName.c_str());
		return false;
	}

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, sFileName.c_str());
	//if the image failed to load, return failure
	if (!dib){
		TFLOG("Loading \"%s\" failure. dib error.", sFileName.c_str());
		return false;
	}

	// If the format is 8bits, convert it to 32bits.
	if (FreeImage_GetLine(dib) / FreeImage_GetWidth(dib) == 1){
		FIBITMAP *_dib = FreeImage_ConvertTo32Bits(dib);
		FreeImage_Unload(dib);
		dib = _dib;
	}

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if ((bits == 0) || (width == 0) || (height == 0)){
		TFLOG("Loading \"%s\" failure. error unknown.", sFileName.c_str());
		return false;
	}

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &(mm_textures[sKey]));
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, mm_textures[sKey]);

	switch (fif){
	case FIF_BMP:
	case FIF_JPEG:
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, border, GL_BGR, GL_UNSIGNED_BYTE, (GLvoid *)bits);
		break;
	case FIF_PNG:
	case FIF_GIF:
	case FIF_TARGA:
		if (FreeImage_GetBPP(dib) == 24){
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, border, GL_BGR, GL_UNSIGNED_BYTE, (GLvoid *)bits);
		}
		else{
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, border, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid *)bits);
		}
		break;
	}
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	//return success
	return true;
}

bool TFTextureManager::unloadTexture(std::string sKey){
	std::map<std::string, unsigned int>::iterator found = mm_textures.find(sKey);
	if (found != mm_textures.end()) {
		glDeleteTextures(1, &(found->second));
		mm_textures.erase(sKey);
		return true;
	}
	return false;
}

unsigned int TFTextureManager::findTextureIdByKey(std::string sKey){
	std::map<std::string, unsigned int>::iterator found = mm_textures.find(sKey);
	if (found == mm_textures.end()){
		return 0;
	}
	else{
		return found->second;
	}
}

std::string TFTextureManager::toString() const {
	std::stringstream ss;
	ss << "TFTextureManager(" << mm_textures.size() << " textures : ";
	for (std::map<std::string, unsigned int>::const_iterator it = mm_textures.begin(); it != mm_textures.end(); ++it){
		ss << it->first << ", ";
	}
	ss << ")";
	return ss.str();
}