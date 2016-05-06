#ifndef _TFMODEL_H_
#define _TFMODEL_H_

#include "TFObject.h"
#include "TFTransform.h"
#include "TFBuffer.h"
#include "TFTexture.h"
#include "TFHandle.h"
#include "TFCommon.h"
#include "TFDictionary.h"
#include <vector>
#include <fstream>

struct TFMaterialInfo{
	unsigned int start, end, id;
};

struct TFTFMData{
	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<float> normals;
	std::vector<unsigned int> indices;
	std::vector<TFMaterialInfo> materialInfos;
};

struct TFMaterial{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	TFTexture2D *tex_ambient;
	TFTexture2D *tex_diffuse;

	TFMaterial() : tex_ambient(nullptr), tex_diffuse(nullptr) { }
};

struct TFMesh{
	TFArrayBuffer *vertexBuffer;
	TFArrayBuffer *uvBuffer;
	TFArrayBuffer *normalBuffer;
	TFArrayBuffer *elementBuffer;
	std::vector<TFMaterialInfo> materialInfos;

	TFMesh() : vertexBuffer(nullptr), uvBuffer(nullptr), normalBuffer(nullptr), elementBuffer(nullptr) { }
};

class TFModel : public TFTransform {
protected:
	std::vector<TFMesh> m_meshes;
	std::vector<TFMaterial> m_materials;
	TFDictionary<std::string, TFTexture2D *> m_textures;
public:
	TFModel(std::string filename){
		// Check type size.
		TFASSERT(sizeof(float) == sizeof(uint32_t), "Size of data type does not match. float =/= uint32_t.");

		// Import tfm file.
		std::ifstream stream(filename.c_str(), std::ios::binary);
		if (!stream.is_open()){
			TFLOG("Impossible to open \"%s\". Are you in the right directory?", filename.c_str());
			return;
		}

		uint32_t nShapes;
		stream.read((char *)&nShapes, sizeof(uint32_t));
		std::vector<TFTFMData> shapes(nShapes);
		//for (uint32_t i = 0; i < nShapes; ++i){
		for (auto &shape : shapes){
			uint32_t nVertices, nTexcoords, nNormals, nIndices;
			stream.read((char *)&nVertices, sizeof(uint32_t));
			stream.read((char *)&nTexcoords, sizeof(uint32_t));
			stream.read((char *)&nNormals, sizeof(uint32_t));
			stream.read((char *)&nIndices, sizeof(uint32_t));
			shape.vertices.resize(nVertices);
			shape.uvs.resize(nTexcoords);
			shape.normals.resize(nNormals);
			shape.indices.resize(nIndices);
			stream.read((char *)shape.vertices.data(), nVertices * sizeof(uint32_t));
			stream.read((char *)shape.uvs.data(), nTexcoords * sizeof(uint32_t));
			stream.read((char *)shape.normals.data(), nNormals * sizeof(uint32_t));
			stream.read((char *)shape.indices.data(), nIndices * sizeof(uint32_t));
			
			uint32_t nRanges, nIds;
			stream.read((char *)&nRanges, sizeof(uint32_t));
			stream.read((char *)&nIds, sizeof(uint32_t));
			std::vector<unsigned int> ranges(nRanges);
			std::vector<unsigned int> ids(nIds);
			stream.read((char *)ranges.data(), nRanges * sizeof(uint32_t));
			stream.read((char *)ids.data(), nIds * sizeof(uint32_t));
			shape.materialInfos.resize(nRanges - 1);
			for (int i = 0; i < nRanges - 1; ++i){
				shape.materialInfos[i].start = ranges[i];
				shape.materialInfos[i].end = ranges[i + 1];
				shape.materialInfos[i].id = ids[i];
			}
		}
		
		std::string directory = filename.substr(0, filename.find_last_of('/') + 1);
		uint32_t nMaterials;
		stream.read((char *)&nMaterials, sizeof(uint32_t));
		m_materials.resize(nMaterials);
		for (auto &material : m_materials){
			stream.read((char *)&material.ambient, 3 * sizeof(uint32_t));
			stream.read((char *)&material.diffuse, 3 * sizeof(uint32_t));
			stream.read((char *)&material.specular, 3 * sizeof(uint32_t));
			stream.read((char *)&material.shininess, sizeof(uint32_t));

			uint32_t nAmbientTexname, nDiffuseTexname;
			stream.read((char *)&nAmbientTexname, sizeof(uint32_t));
			stream.read((char *)&nDiffuseTexname, sizeof(uint32_t));
			std::string texname = "";
			TFTexture2D *texture = nullptr;
			if (nAmbientTexname != 0){
				texname.resize(nAmbientTexname + 1);
				stream.read((char *)texname.data(), nAmbientTexname * sizeof(int8_t));
				if (m_textures.getValue(texname, &texture) == false){
					texture = TFTexture2D::createWithFile((directory + texname).c_str());
					texture->retain();
					texture->setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
					texture->generateMipmap();
					m_textures.add(texname, texture);
				}
				material.tex_ambient = texture;
			}
			if (nDiffuseTexname != 0){
				texname.resize(nDiffuseTexname + 1);
				stream.read((char *)texname.data(), nDiffuseTexname * sizeof(int8_t));
				if (m_textures.getValue(texname, &texture) == false){
					texture = TFTexture2D::createWithFile((directory + texname).c_str());
					texture->retain();
					texture->setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
					texture->generateMipmap();
					m_textures.add(texname, texture);
				}
				material.tex_diffuse = texture;
			}
		}

		stream.close();

		// Generate buffers.
		m_meshes.resize(shapes.size());
		for (int i = 0; i < shapes.size(); ++i){
			TFTFMData &shape = shapes[i];
			TFMesh &mesh = m_meshes[i];
			TFArrayBuffer *buffer;

			buffer = TFArrayBuffer::create(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(float), (GLvoid *)shape.vertices.data(), GL_STATIC_DRAW);
			buffer->retain();
			mesh.vertexBuffer = buffer;

			if (!shape.uvs.empty()){
				buffer = TFArrayBuffer::create(GL_ARRAY_BUFFER, shape.uvs.size() * sizeof(float), (GLvoid *)shape.uvs.data(), GL_STATIC_DRAW);
				buffer->retain();
				mesh.uvBuffer = buffer;
			}

			buffer = TFArrayBuffer::create(GL_ARRAY_BUFFER, shape.normals.size() * sizeof(float), (GLvoid *)shape.normals.data(), GL_STATIC_DRAW);
			buffer->retain();
			mesh.normalBuffer = buffer;

			buffer = TFArrayBuffer::create(GL_ELEMENT_ARRAY_BUFFER, shape.indices.size() * sizeof(float), (GLvoid *)shape.indices.data(), GL_STATIC_DRAW);
			buffer->retain();
			mesh.elementBuffer = buffer;

			mesh.materialInfos = shape.materialInfos;
		}
	}

	virtual ~TFModel() {
		for (auto &mesh : m_meshes)
		{
			mesh.vertexBuffer->release();
			if (mesh.uvBuffer != nullptr){
				mesh.uvBuffer->release();
			}
			mesh.normalBuffer->release();
			mesh.elementBuffer->release();
		}
		for (auto &word : m_textures){
			word.value->release();
		}
	}

	void draw(const std::vector<GLuint> &uniforms){
		for (auto &mesh : m_meshes){
			glEnableVertexAttribArray(0);
			mesh.vertexBuffer->bind();
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			if (mesh.uvBuffer != nullptr){
				glEnableVertexAttribArray(1);
				mesh.uvBuffer->bind();
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
			}

			glEnableVertexAttribArray(2);
			mesh.normalBuffer->bind();
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			mesh.elementBuffer->bind();
			for (auto &materialInfo : mesh.materialInfos){
				if (!m_materials.empty()){
					TFMaterial &material = m_materials[materialInfo.id];
					glUniform3f(uniforms[0], material.ambient.r, material.ambient.g, material.ambient.b);
					glUniform3f(uniforms[1], material.diffuse.r, material.diffuse.g, material.diffuse.b);
					glUniform3f(uniforms[2], material.specular.r, material.specular.g, material.specular.b);
					glUniform1f(uniforms[3], material.shininess);

					glActiveTexture(GL_TEXTURE0);
					if (material.tex_diffuse != nullptr){
						material.tex_diffuse->bind();
					}
					else{
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}

				glDrawRangeElements(GL_TRIANGLES, materialInfo.start, materialInfo.end, materialInfo.end - materialInfo.start, GL_UNSIGNED_INT, (void *)0);
			}

			glDisableVertexAttribArray(0);
			if (mesh.uvBuffer != nullptr){
				glDisableVertexAttribArray(1);
			}
			glDisableVertexAttribArray(2);
		}
	}
};

#endif