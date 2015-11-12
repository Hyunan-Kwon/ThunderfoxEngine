//#ifndef _TFMODEL_H_
//#define _TFMODEL_H_
//
//#include "gl\glew.h"
//#include "TFTextureManager.h"
//#include "TFHandle.h"
//#include "TFTransform.h"
//#include <vector>
//#include <string>
//#include <fstream>
//
//struct TFMaterialInfo{
//	vector<unsigned int> ranges;
//	vector<unsigned int> ids;
//};
//
//struct TFMesh{
//	std::vector<float> vertices;
//	std::vector<float> uvs;
//	std::vector<float> normals;
//	std::vector<unsigned int> indices;
//	TFMaterialInfo materialInfo;
//};
//
//struct TFShape{
//	std::string name;
//	TFMesh mesh;
//};
//
//struct TFMaterial{
//	float ambient[3];
//	float diffuse[3];
//	float specular[3];
//	float shininess;
//	std::string ambient_texname;
//	std::string diffuse_texname;
//};
//
//class TFModel : public TFTransform{
////private:
//public:
//	std::vector<TFShape> m_shapes;
//	std::vector<TFMaterial> m_materials;
//	std::vector<GLuint> m_vertexBuffers;
//	std::vector<GLuint> m_uvBuffers;
//	std::vector<GLuint> m_normalBuffers;
//	std::vector<GLuint> m_elementBuffers;
//public:
//	TFModel(std::string path){
//		// Check type size.
//		TFASSERT(sizeof(float) == sizeof(uint32_t), "Size of data type does not match. float =/= uint32_t.");
//
//		// Import tfm file.
//		ifstream stream(path.c_str(), ios::binary);
//		if (!stream.is_open()){
//			TFLOG("Impossible to open \"%s\". Are you in the right directory?", path.c_str());
//			return;
//		}
//		
//		uint32_t nShapes;
//		stream.read((char *)&nShapes, sizeof(uint32_t));
//		m_shapes.resize(nShapes);
//		for (uint32_t i = 0; i < nShapes; ++i){
//			uint32_t nVertices, nTexcoords, nNormals, nIndices;
//			stream.read((char *)&nVertices, sizeof(uint32_t));
//			stream.read((char *)&nTexcoords, sizeof(uint32_t));
//			stream.read((char *)&nNormals, sizeof(uint32_t));
//			stream.read((char *)&nIndices, sizeof(uint32_t));
//			m_shapes[i].mesh.vertices.resize(nVertices);
//			m_shapes[i].mesh.uvs.resize(nTexcoords);
//			m_shapes[i].mesh.normals.resize(nNormals);
//			m_shapes[i].mesh.indices.resize(nIndices);
//			stream.read((char *)m_shapes[i].mesh.vertices.data(), nVertices * sizeof(uint32_t));
//			if (nTexcoords != 0){
//				stream.read((char *)m_shapes[i].mesh.uvs.data(), nTexcoords * sizeof(uint32_t));
//			}
//			stream.read((char *)m_shapes[i].mesh.normals.data(), nNormals * sizeof(uint32_t));
//			stream.read((char *)m_shapes[i].mesh.indices.data(), nIndices * sizeof(uint32_t));
//
//			uint32_t nRanges, nIds;
//			stream.read((char *)&nRanges, sizeof(uint32_t));
//			stream.read((char *)&nIds, sizeof(uint32_t));
//			m_shapes[i].mesh.materialInfo.ranges.resize(nRanges);
//			m_shapes[i].mesh.materialInfo.ids.resize(nIds);
//			stream.read((char *)m_shapes[i].mesh.materialInfo.ranges.data(), nRanges * sizeof(uint32_t));
//			stream.read((char *)m_shapes[i].mesh.materialInfo.ids.data(), nIds * sizeof(uint32_t));
//		}
//
//		uint32_t nMaterials;
//		stream.read((char *)&nMaterials, sizeof(uint32_t));
//		m_materials.resize(nMaterials);
//		for (uint32_t i = 0; i < nMaterials; ++i){
//			stream.read((char *)m_materials[i].ambient, 3 * sizeof(uint32_t));
//			stream.read((char *)m_materials[i].diffuse, 3 * sizeof(uint32_t));
//			stream.read((char *)m_materials[i].specular, 3 * sizeof(uint32_t));
//			stream.read((char *)&m_materials[i].shininess, sizeof(uint32_t));
//
//			uint32_t nAmbientTexname, nDiffuseTexname;
//			stream.read((char *)&nAmbientTexname, sizeof(uint32_t));
//			stream.read((char *)&nDiffuseTexname, sizeof(uint32_t));
//			if (nAmbientTexname != 0){
//				m_materials[i].ambient_texname.resize(nAmbientTexname + 1);
//				stream.read((char *)m_materials[i].ambient_texname.data(), nAmbientTexname * sizeof(int8_t));
//			}
//			if (nDiffuseTexname != 0){
//				m_materials[i].diffuse_texname.resize(nDiffuseTexname + 1);
//				stream.read((char *)m_materials[i].diffuse_texname.data(), nDiffuseTexname * sizeof(int8_t));
//			}
//		}
//
//		stream.close();
//
//		// Generate buffers.
//		for (unsigned int i = 0; i < m_shapes.size(); ++i){
//			GLuint tempBuffer;
//			TFMesh &mesh = m_shapes[i].mesh;
//
//			glGenBuffers(1, &tempBuffer);
//			glBindBuffer(GL_ARRAY_BUFFER, tempBuffer);
//			glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), &(mesh.vertices.front()), GL_STATIC_DRAW);
//			m_vertexBuffers.push_back(tempBuffer);
//
//			if (!mesh.uvs.empty()){
//				glGenBuffers(1, &tempBuffer);
//				glBindBuffer(GL_ARRAY_BUFFER, tempBuffer);
//				glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size() * sizeof(float), &(mesh.uvs.front()), GL_STATIC_DRAW);
//				m_uvBuffers.push_back(tempBuffer);
//			}
//
//			glGenBuffers(1, &tempBuffer);
//			glBindBuffer(GL_ARRAY_BUFFER, tempBuffer);
//			glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), &(mesh.normals.front()), GL_STATIC_DRAW);
//			m_normalBuffers.push_back(tempBuffer);
//
//			glGenBuffers(1, &tempBuffer);
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempBuffer);
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &(mesh.indices.front()), GL_STATIC_DRAW);
//			m_elementBuffers.push_back(tempBuffer);
//		}
//
//		// Load textures.
//		std::string directory = path.substr(0, path.find_last_of('/') + 1);
//		for (unsigned int i = 0; i < m_materials.size(); ++i){
//			if (!m_materials[i].ambient_texname.empty()){
//				TFTextureManager::getInstance().loadTexture(m_materials[i].ambient_texname, (directory + m_materials[i].ambient_texname).c_str());
//			}
//			if (!m_materials[i].diffuse_texname.empty()){
//				TFTextureManager::getInstance().loadTexture(m_materials[i].diffuse_texname, (directory + m_materials[i].diffuse_texname).c_str());
//			}
//		}
//	}
//
//	~TFModel(){
//		// Delete buffers.
//		glDeleteBuffers(m_vertexBuffers.size(), &(m_vertexBuffers.front()));
//		if (!m_uvBuffers.empty()){
//			glDeleteBuffers(m_uvBuffers.size(), &(m_uvBuffers.front()));
//		}
//		glDeleteBuffers(m_normalBuffers.size(), &(m_normalBuffers.front()));
//		glDeleteBuffers(m_elementBuffers.size(), &(m_elementBuffers.front()));
//
//		// Unload textures.
//		for (unsigned int i = 0; i < m_materials.size(); ++i){
//			TFTextureManager::getInstance().unloadTexture(m_materials[i].diffuse_texname);
//		}
//	}
//	 
//	void draw(const std::vector<GLuint> &uniforms){
//		std::set<std::string> wtf;
//
//
//		for (int i = 0; i < m_shapes.size(); ++i){
//			TFShape &shape = m_shapes[i];
//
//			glEnableVertexAttribArray(0);
//			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[i]);
//			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
//
//			if (!shape.mesh.uvs.empty()){
//				glEnableVertexAttribArray(1);
//				glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffers[i]);
//				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
//			}
//
//			glEnableVertexAttribArray(2);
//			glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffers[i]);
//			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
//
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffers[i]);
//			//glDrawElements(GL_TRIANGLES, m_shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, (void *)0);
//			for (int j = 0; j < shape.mesh.materialInfo.ranges.size() - 1; ++j){
//				if (!m_materials.empty()){
//					TFMaterial &material = m_materials[shape.mesh.materialInfo.ids[j]];
//
//					glUniform3f(uniforms[0], material.ambient[0], material.ambient[1], material.ambient[2]);
//					glUniform3f(uniforms[1], material.diffuse[0], material.diffuse[1], material.diffuse[2]);
//					glUniform3f(uniforms[2], material.specular[0], material.specular[1], material.specular[2]);
//					glUniform1f(uniforms[3], material.shininess);
//					//glUniform1f(uniforms[3], 0.0);
//					//TFLOG(material.shininess);
//
//					//if (!material.ambient_texname.empty()){
//					//	glBindTexture(GL_TEXTURE_2D, TFTextureManager::getInstance().findTextureIdByKey(material.ambient_texname));
//					//}
//					//else{
//					//	glBindTexture(GL_TEXTURE_2D, 0);
//					//}
//					if (!material.diffuse_texname.empty()){
//						wtf.insert(material.diffuse_texname);
//						//glActiveTexture(GL_TEXTURE0);
//						glBindTexture(GL_TEXTURE_2D, TFTextureManager::getInstance().findTextureIdByKey(material.diffuse_texname));
//						//glUniform1i()
//					}
//					else if (!material.ambient_texname.empty()){
//						glBindTexture(GL_TEXTURE_2D, TFTextureManager::getInstance().findTextureIdByKey(material.ambient_texname));
//					}
//					else{
//						glBindTexture(GL_TEXTURE_2D, 0);
//					}
//				}
//
//				GLuint start = shape.mesh.materialInfo.ranges[j];
//				GLuint end = shape.mesh.materialInfo.ranges[j + 1];
//				glDrawRangeElements(GL_TRIANGLES, 0, shape.mesh.indices.size(), end - start, GL_UNSIGNED_INT, (void *)(start * sizeof(GLuint)));
//			}
//
//			glDisableVertexAttribArray(0);
//			if (!shape.mesh.uvs.empty()){
//				glDisableVertexAttribArray(1);
//			}
//			glDisableVertexAttribArray(2);
//		}
//	}
//};
//
//#endif
//
////#ifndef _TFMODEL_H_
////#define _TFMODEL_H_
////
////#include "gl\glew.h"
////#include "tiny_obj_loader.h"
////#include "TFTextureManager.h"
////#include <vector>
////#include <string>
////#include <fstream>
////#include <cassert>
////
////struct TFMaterialInfo{
////	vector<GLuint> ranges;
////	vector<GLuint> ids;
////};
////
////struct TFMesh{
////	std::vector<float> vertices;
////	std::vector<float> uvs;
////	std::vector<float> normals;
////	std::vector<unsigned int> indices;
////	TFMaterialInfo materialInfo;
////};
////
////struct TFShape{
////	std::string name;
////	TFMesh mesh;
////};
////
////struct TFMaterial{
////	std::string diffuse_texname;
////};
////
////class TFModel{
////private:
////	std::string m_directory;
////	std::vector<tinyobj::shape_t> m_shapes;
////	std::vector<tinyobj::material_t> m_materials;
////	std::vector<GLuint> m_vertexBuffers;
////	std::vector<GLuint> m_uvBuffers;
////	std::vector<GLuint> m_normalBuffers;
////	std::vector<GLuint> m_elementBuffers;
////	std::vector<std::vector<GLuint> > m_materialIndices;
////public:
////
////	TFModel(std::string directory, std::string filename) : m_directory(directory){
////		// Import obj file.
////		std::string err = tinyobj::LoadObj(m_shapes, m_materials, (directory + filename).c_str(), directory.c_str());
////		if (!err.empty()){
////			return;
////		}
////
////		// Generate buffers.
////		for (unsigned int i = 0; i < m_shapes.size(); ++i){
////			GLuint tempBuffer;
////			tinyobj::mesh_t &mesh = m_shapes[i].mesh;
////
////			glGenBuffers(1, &tempBuffer);
////			glBindBuffer(GL_ARRAY_BUFFER, tempBuffer);
////			glBufferData(GL_ARRAY_BUFFER, mesh.positions.size() * sizeof(float), &(mesh.positions.front()), GL_STATIC_DRAW);
////			m_vertexBuffers.push_back(tempBuffer);
////
////			glGenBuffers(1, &tempBuffer);
////			glBindBuffer(GL_ARRAY_BUFFER, tempBuffer);
////			glBufferData(GL_ARRAY_BUFFER, mesh.texcoords.size() * sizeof(float), &(mesh.texcoords.front()), GL_STATIC_DRAW);
////			m_uvBuffers.push_back(tempBuffer);
////
////			if (mesh.normals.size() > 0){
////				glGenBuffers(1, &tempBuffer);
////				glBindBuffer(GL_ARRAY_BUFFER, tempBuffer);
////				glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), &(mesh.normals.front()), GL_STATIC_DRAW);
////				m_normalBuffers.push_back(tempBuffer);
////			}
////
////			glGenBuffers(1, &tempBuffer);
////			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempBuffer);
////			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &(mesh.indices.front()), GL_STATIC_DRAW);
////			m_elementBuffers.push_back(tempBuffer);
////
////			m_materialIndices.push_back(std::vector<GLuint>());
////			GLuint id = 0;
////			m_materialIndices[i].push_back(0);
////			for (int j = 0; j < mesh.material_ids.size(); ++j){
////				if (id != mesh.material_ids[j]){
////					id = mesh.material_ids[j];
////					m_materialIndices[i].push_back(j);
////				}
////			}
////		}
////
////		// Load textures.
////		for (unsigned int i = 0; i < m_materials.size(); ++i){
////			if (!(m_materials[i].diffuse_texname.empty())){
////				if (TFTextureManager::getInstance().loadTexture(m_materials[i].diffuse_texname, (m_directory + m_materials[i].diffuse_texname).c_str())){
////					std::cout << "Loading \"" << m_materials[i].diffuse_texname << "\" success." << std::endl;;
////				}
////				else{
////					std::cout << "Loading \"" << m_materials[i].diffuse_texname << "\" failure." << std::endl;;
////				}
////			}
////		}
////	}
////
////	~TFModel(){
////		// Delete buffers.
////		glDeleteBuffers(m_vertexBuffers.size(), &(m_vertexBuffers.front()));
////		glDeleteBuffers(m_uvBuffers.size(), &(m_uvBuffers.front()));
////		if (m_normalBuffers.size() > 0){
////			glDeleteBuffers(m_normalBuffers.size(), &(m_normalBuffers.front()));
////		}
////		glDeleteBuffers(m_elementBuffers.size(), &(m_elementBuffers.front()));
////
////		// Unload textures.
////		for (unsigned int i = 0; i < m_materials.size(); ++i){
////			TFTextureManager::getInstance().unloadTexture(m_materials[i].diffuse_texname);
////		}
////	}
////
////	void draw(){
////		for (unsigned int i = 0; i < m_shapes.size(); ++i){
////			glActiveTexture(GL_TEXTURE0);
////
////			glEnableVertexAttribArray(0);
////			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffers[i]);
////			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
////
////			glEnableVertexAttribArray(1);
////			glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffers[i]);
////			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
////
////			if (m_normalBuffers.size() > 0){
////				glEnableVertexAttribArray(2);
////				glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffers[i]);
////				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
////			}
////
////			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffers[i]);
////			for (int j = 0; j < m_materialIndices[i].size(); ++j){
////				GLuint start = m_materialIndices[i][j] * 3;
////				GLuint end;
////				if (m_materialIndices[i].size() - 1 == j){
////					end = m_shapes[i].mesh.indices.size();
////				}
////				else{
////					end = m_materialIndices[i][j + 1] * 3;
////				}
////
////				std::string materialName = m_materials[m_shapes[i].mesh.material_ids[m_materialIndices[i][j]]].diffuse_texname;
////				glBindTexture(GL_TEXTURE_2D, TFTextureManager::getInstance().findTextureIdByKey(materialName));
////				//glDrawElements(GL_TRIANGLES, m_shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, (void *)0);
////				glDrawRangeElements(GL_TRIANGLES, 0, m_shapes[i].mesh.indices.size(), end - start, GL_UNSIGNED_INT, (void *)(start * sizeof(GLuint)));
////			}
////
////
////
////			glDisableVertexAttribArray(0);
////			glDisableVertexAttribArray(1);
////			if (m_normalBuffers.size() > 0){
////				glDisableVertexAttribArray(2);
////			}
////		}
////	}
////};
////
////#endif