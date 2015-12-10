#ifndef _TFTEXTURE_H_
#define _TFTEXTURE_H_

#include "gl\glew.h"
#include "FreeImage\FreeImage.h"
#include "TFHandle.h"
#include "TFObject.h"
#include <vector>
#include <fstream>

struct TFImageData : public TFRef{
	unsigned int width, height;
	GLint internalFormat;
	GLenum format;
	GLubyte *data;

	TFImageData(const char *filename) {
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
			TFEXIT();
		}
		// If image foramt is unsupportable, return failure.
		if (fif != FIF_BMP && fif != FIF_JPEG && fif != FIF_GIF && fif != FIF_PNG && fif != FIF_TARGA){
			TFLOG("Loading \"%s\" failure. Unsupportable format.", filename);
			TFEXIT();
		}

		//check that the plugin has reading capabilities and load the file
		if (FreeImage_FIFSupportsReading(fif))
			dib = FreeImage_Load(fif, filename);
		//if the image failed to load, return failure
		if (!dib){
			TFLOG("Loading \"%s\" failure. dib error.", filename);
			TFEXIT();
		}

		//unsigned int bytesPerPixel = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);
		unsigned int bitPerPixel = FreeImage_GetBPP(dib);
		switch (bitPerPixel)
		{
		case 8:
			internalFormat = GL_RED;
			format = GL_RED;
			break;
		case 24:
			internalFormat = GL_RGB;
			format = GL_BGR;
			break;
		case 32:
			internalFormat = GL_RGBA;
			format = GL_BGRA;
			break;
		default:
			TFLOG("Loading \"%s\" failure. Unsupportable format.", filename);
			TFEXIT();
		}

		//retrieve the image data
		bits = FreeImage_GetBits(dib);
		//get the image width and height
		width = FreeImage_GetWidth(dib);
		height = FreeImage_GetHeight(dib);
		//if this somehow one of these failed (they shouldn't), return failure
		if ((bits == 0) || (width == 0) || (height == 0)){
			TFLOG("Loading \"%s\" failure. error unknown.", filename);
			TFEXIT();
		}

		unsigned int dataSize = FreeImage_GetDIBSize(dib);
		data = new GLubyte[dataSize];
		memcpy(data, bits, dataSize);

		FreeImage_Unload(dib);
	}

	// This is for reading raw file.
	TFImageData(const char *filename, int width, int height, int bytePerPixel)
	: width(width), height(height)
	{
		switch (bytePerPixel)
		{
			case 1:		internalFormat = format = GL_RED;		break;
			case 3:		internalFormat = format = GL_RGB;		break;
			case 4:		internalFormat = format = GL_RGBA;		break;
			default:
				TFLOG("Can't accept this raw image type: %d byte per pixel.", bytePerPixel);
				TFEXIT();
		}

		std::ifstream stream(filename, std::ios::binary);
		if (!stream.is_open()){
			TFLOG("Can't open \"%s\".", filename);
			TFEXIT();
		}

		data = new GLubyte[width * height * bytePerPixel];
		stream.read(reinterpret_cast<char *>(data), sizeof(GLubyte) * width * height * bytePerPixel);
	}

	virtual ~TFImageData() { delete[] data; }

	static TFImageData* create(const char *filename) {
		return static_cast<TFImageData *>((new TFImageData(filename))->autorelease());
	}

	static TFImageData* createWithRawFile(const char *filename, int width, int height, int bytePerPixel){
		return static_cast<TFImageData *>((new TFImageData(filename, width, height, bytePerPixel))->autorelease());
	}

	TFImageData* flipVertical(){
		int bytePerPixel;
		switch (internalFormat)
		{
			case GL_RED:	bytePerPixel = 1;	break;
			case GL_RGB:	bytePerPixel = 3;	break;
			case GL_RGBA:	bytePerPixel = 4;	break;
			default:	TFEXIT("Can't flip the image data vertically. Unsupportable internal format.");
		}

		std::vector<GLubyte> line(width * bytePerPixel);
		int lineLength = bytePerPixel * width;
		for (int y = 0; y < (height >> 1); ++y){
			GLubyte *upper = data + y * lineLength;
			GLubyte *lower = data + (height - 1 - y) * lineLength;
			memcpy(line.data(), upper, lineLength);
			memcpy(upper, lower, lineLength);
			memcpy(lower, line.data(), lineLength);
		}

		return this;
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
		TFImageData *imageData = new TFImageData(filename);
		TFTexture1D *instance = new TFTexture1D(imageData->internalFormat, imageData->width, imageData->format, GL_UNSIGNED_BYTE, (GLvoid *)imageData->data);
		instance->autorelease();
		imageData->release();
		return instance;
	}

	virtual ~TFTexture1D() { }

	unsigned int getWidth() const { return m_width; }
};

class TFTexture2D : public TFTexture{
protected:
	unsigned int m_width, m_height;

	TFTexture2D(GLint internalFormat, unsigned int width, unsigned int height, GLenum format, GLenum type, GLvoid *data)
	: TFTexture(GL_TEXTURE_2D), m_width(width), m_height(height) {
		// This is for the not aligned image data. No need with FreeImage.
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
		TFImageData *imageData = new TFImageData(filename);
		TFTexture2D *instance = new TFTexture2D(imageData->internalFormat, imageData->width, imageData->height, imageData->format, GL_UNSIGNED_BYTE, imageData->data);
		instance->autorelease();
		imageData->release();
		return instance;
	}

	virtual ~TFTexture2D() { }

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
};

class TFTexture2DMultisample : public TFTexture{
protected:
	unsigned int m_width, m_height, m_samples;

	TFTexture2DMultisample(GLsizei samples, GLint internalFormat, unsigned int width, unsigned int height, GLboolean fixedsamplelocations)
		: TFTexture(GL_TEXTURE_2D_MULTISAMPLE), m_width(width), m_height(height), m_samples(samples) {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, fixedsamplelocations);
	}
public:
	static inline TFTexture2DMultisample* createEmpty(GLsizei samples, GLint internalFormat, unsigned int width, unsigned int height, GLboolean fixedsamplelocations){
		return static_cast<TFTexture2DMultisample *>((new TFTexture2DMultisample(samples, internalFormat, width, height, fixedsamplelocations))->autorelease());
	}

	virtual ~TFTexture2DMultisample() { }

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
	unsigned int getSamples() const { return m_samples; }
};

class TFTextureCubemap : public TFTexture{
protected:
	unsigned int m_width, m_height;

	TFTextureCubemap(const std::vector<TFImageData *> &imageDataSet)
		: TFTexture(GL_TEXTURE_CUBE_MAP), m_width(imageDataSet[0]->width), m_height(imageDataSet[0]->height)
	{
		for (int i = 0; i < 6; ++i){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imageDataSet[i]->internalFormat, imageDataSet[i]->width, imageDataSet[i]->height, 0, imageDataSet[i]->format, GL_UNSIGNED_BYTE, imageDataSet[i]->data);
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
		std::vector<TFImageData *> imageDataSet;
		imageDataSet.push_back(right->flipVertical());
		imageDataSet.push_back(left->flipVertical());
		imageDataSet.push_back(top->flipVertical());
		imageDataSet.push_back(bottom->flipVertical());
		imageDataSet.push_back(front->flipVertical());
		imageDataSet.push_back(back->flipVertical());
		return static_cast<TFTextureCubemap *>((new TFTextureCubemap(imageDataSet))->autorelease());
	}

	static inline TFTextureCubemap* createWithFiles(const char *right, const char *left,
												const char *top, const char *bottom,
												const char *front, const char *back)
	{
		std::vector<TFImageData *> imageDataSet;
		imageDataSet.push_back((new TFImageData(right))->flipVertical());
		imageDataSet.push_back((new TFImageData(left))->flipVertical());
		imageDataSet.push_back((new TFImageData(top))->flipVertical());
		imageDataSet.push_back((new TFImageData(bottom))->flipVertical());
		imageDataSet.push_back((new TFImageData(front))->flipVertical());
		imageDataSet.push_back((new TFImageData(back))->flipVertical());
		TFTextureCubemap *instance = new TFTextureCubemap(imageDataSet);
		instance->autorelease();
		for (auto &imageData : imageDataSet){
			imageData->release();
		}
		return instance;
	}

	virtual ~TFTextureCubemap() { }

	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
};

#endif