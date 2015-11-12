#ifndef _TFTEXTURE_H_
#define _TFTEXTURE_H_

#include "gl\glew.h"
#include "FreeImage\FreeImage.h"
#include "TFHandle.h"
#include "TFObject.h"
#include <cassert>
#include <vector>

struct TFImageData : public TFRef{
	unsigned int width, height;
	GLint internalFormat;
	GLenum format;
	GLubyte *data;

	virtual ~TFImageData() { delete[] data; }

	static TFImageData* create(const char *filename) {
		TFASSERT(sizeof(BYTE) == sizeof(GLubyte), "BYTE != GLubyte");

		//image format
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		//pointer to the image, once loaded
		FIBITMAP *dib(0);
		//pointer to the image data
		BYTE* bits(0);
		//image width and height

		//check the file signature and deduce its format
		fif = FreeImage_GetFileType(filename, 0);
		//if still unknown, try to guess the file format from the file extension
		if (fif == FIF_UNKNOWN){
			fif = FreeImage_GetFIFFromFilename(filename);
		}
		//if still unkown, return failure
		if (fif == FIF_UNKNOWN){
			TFLOG("Loading \"%s\" failure. fif unknown.", filename);
			return nullptr;
		}
		// If image foramt is unsupportable, return failure.
		if (fif != FIF_BMP && fif != FIF_JPEG && fif != FIF_GIF && fif != FIF_PNG && fif != FIF_TARGA){
			TFLOG("Loading \"%s\" failure. Unsupportable format.", filename);
			return nullptr;
		}

		//check that the plugin has reading capabilities and load the file
		if (FreeImage_FIFSupportsReading(fif))
			dib = FreeImage_Load(fif, filename);
		//if the image failed to load, return failure
		if (!dib){
			TFLOG("Loading \"%s\" failure. dib error.", filename);
			return nullptr;
		}

		TFImageData *instance = new TFImageData();
		instance->autorelease();

		//unsigned int bytesPerPixel = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);
		unsigned int bitPerPixel = FreeImage_GetBPP(dib);
		switch (bitPerPixel)
		{
		case 8:
			instance->internalFormat = GL_RED;
			instance->format = GL_RED;			
			break;
		case 24:
			instance->internalFormat = GL_RGB;
			instance->format = GL_BGR;			
			break;
		case 32:
			instance->internalFormat = GL_RGBA;
			instance->format = GL_BGRA;
			break;
		default:
			TFLOG("Loading \"%s\" failure. Unsupportable format.", filename);
			return nullptr;
		}

		//retrieve the image data
		bits = FreeImage_GetBits(dib);
		//get the image width and height
		instance->width = FreeImage_GetWidth(dib);
		instance->height = FreeImage_GetHeight(dib);
		//if this somehow one of these failed (they shouldn't), return failure
		if ((bits == 0) || (instance->width == 0) || (instance->height == 0)){
			TFLOG("Loading \"%s\" failure. error unknown.", filename);
			return nullptr;
		}

		unsigned int dataSize = FreeImage_GetDIBSize(dib);
		instance->data = new GLubyte[dataSize / sizeof(GLubyte)];
		memcpy(instance->data, bits, dataSize);

		FreeImage_Unload(dib);

		return instance;
	}
};


class TFTexture : public TFRef, public TFGLObject{
protected:
	GLenum m_target;

	TFTexture(GLenum target) : m_target(target){
		glGenTextures(1, &m_id);
		glBindTexture(m_target, m_id);
	}
public:
	virtual ~TFTexture(){
		glDeleteTextures(1, &m_id);
	}

	inline void bind() const {
		glBindTexture(m_target, m_id);
	}

	inline void setWrapS(GLfloat mode) const {
		glTexParameterf(m_target, GL_TEXTURE_WRAP_S, mode);
	}
	inline void setWrapT(GLfloat mode) const  {
		glTexParameterf(m_target, GL_TEXTURE_WRAP_T, mode);
	}
	inline void setWrap(GLfloat s, GLfloat t) const {
		glTexParameterf(m_target, GL_TEXTURE_WRAP_S, s);
		glTexParameterf(m_target, GL_TEXTURE_WRAP_T, t);
	}
	inline void setMinFilter(GLfloat mode) const {
		glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, mode);
	}
	inline void setMagFilter(GLfloat mode) const {
		glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, mode);
	}
	inline void setFilter(GLfloat min, GLfloat mag) const {
		glTexParameterf(m_target, GL_TEXTURE_MIN_FILTER, min);
		glTexParameterf(m_target, GL_TEXTURE_MAG_FILTER, mag);
	}
	inline void generateMipmap() const{
		glGenerateMipmap(m_target);
	}
};

class TFTexture1D : public TFTexture{
protected:
	unsigned int m_width;

	TFTexture1D(GLint internalFormat, unsigned int width, GLenum format, GLenum type, GLvoid *data)
	: TFTexture(GL_TEXTURE_1D), m_width(width) {
		glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, m_width, 0, format, type, data);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
public:
	static inline TFTexture1D* createEmpty(GLint internalFormat, unsigned int width, GLenum format, GLenum type){
		return static_cast<TFTexture1D *>((new TFTexture1D(internalFormat, width, format, type, nullptr))->autorelease());
	}

	static inline TFTexture1D* create(TFImageData *imageData){
		return static_cast<TFTexture1D *>((new TFTexture1D(imageData->internalFormat, imageData->width, imageData->format, GL_UNSIGNED_BYTE, (GLvoid *)imageData->data))->autorelease());
	}

	static inline TFTexture1D* createWithFile(const char *filename){
		TFImageData *imageData = TFImageData::create(filename);
		return static_cast<TFTexture1D *>((new TFTexture1D(imageData->internalFormat, imageData->width, imageData->format, GL_UNSIGNED_BYTE, (GLvoid *)imageData->data))->autorelease());
	}

	virtual ~TFTexture1D() { }

	unsigned int getWidth() const { return m_width; }
};

class TFTexture2D : public TFTexture{
protected:
	unsigned int m_width, m_height;

	TFTexture2D(GLint internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type, GLvoid *data)
	: TFTexture(GL_TEXTURE_2D), m_width(width), m_height(height) {
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		//glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, data);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
public:
	static inline TFTexture2D* createEmpty(GLint internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type){
		return static_cast<TFTexture2D*>((new TFTexture2D(internalFormat, width, height, format, type, nullptr))->autorelease());
	}

	static inline TFTexture2D* create(TFImageData *imageData){
		return static_cast<TFTexture2D*>((new TFTexture2D(imageData->internalFormat, imageData->width, imageData->height, imageData->format, GL_UNSIGNED_BYTE, imageData->data))->autorelease());
	}

	static inline TFTexture2D* createWithFile(const char *filename){
		TFImageData *imageData = TFImageData::create(filename);
		return static_cast<TFTexture2D *>((new TFTexture2D(imageData->internalFormat, imageData->width, imageData->height, imageData->format, GL_UNSIGNED_BYTE, imageData->data))->autorelease());
	}

	virtual ~TFTexture2D() { }

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
};

class TFTextureCubemap : public TFTexture{
protected:
	unsigned int m_width, m_height;

	TFTextureCubemap(const std::vector<TFImageData *> &imageData)
	: TFTexture(GL_TEXTURE_CUBE_MAP), m_width(imageData[0]->width), m_height(imageData[0]->height)
	{
		for (int i = 0; i < 6; ++i){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imageData[i]->internalFormat, imageData[i]->width, imageData[i]->height, 0, imageData[i]->format, GL_UNSIGNED_BYTE, (void *)&(imageData[i]->data));
		}
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
public:
	static inline TFTextureCubemap* create(TFImageData *right, TFImageData *left,
										TFImageData *top, TFImageData *bottom,
										TFImageData *front, TFImageData *back)
	{
		std::vector<TFImageData *> imageData;
		imageData.push_back(right);
		imageData.push_back(left);
		imageData.push_back(top);
		imageData.push_back(bottom);
		imageData.push_back(front);
		imageData.push_back(back);
		return static_cast<TFTextureCubemap *>((new TFTextureCubemap(imageData))->autorelease());
	}

	static inline TFTextureCubemap* createWithFiles(const char *right, const char *left,
												const char *top, const char *bottom,
												const char *front, const char *back)
	{
		std::vector<TFImageData *> imageData;
		imageData.push_back(TFImageData::create(right));
		imageData.push_back(TFImageData::create(left));
		imageData.push_back(TFImageData::create(top));
		imageData.push_back(TFImageData::create(bottom));
		imageData.push_back(TFImageData::create(front));
		imageData.push_back(TFImageData::create(back));
		return static_cast<TFTextureCubemap *>((new TFTextureCubemap(imageData))->autorelease());
	}

	virtual ~TFTextureCubemap() { }

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
};

#endif