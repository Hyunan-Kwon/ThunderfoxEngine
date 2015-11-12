#ifndef _TFPRIMITIVE_H_
#define _TFPRIMITIVE_H_

#include "glm\glm.hpp"
#include "TFTransform.h"
#include "TFCommon.h"
#include <set>
#include <algorithm>

class TFPrimitive : public TFTransform{
protected:
public:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::uvec3> m_faces;
	TFArrayBuffer *vertexBuffer, *normalBuffer, *elementBuffer;
};

class TFSphere : public TFPrimitive{
private:
	TFDictionary<uint64_t, unsigned int> m_indicesCache;

	unsigned int addVertex(const glm::vec3 &v){
		m_vertices.push_back(glm::normalize(v));
		return m_vertices.size() - 1;
	}
	unsigned int getMiddlePoint(unsigned int v1, unsigned int v2){
		uint64_t _v1 = v1, _v2 = v2, key;
		if (_v1 < _v2){
			key = (_v1 << 32) + _v2;
		}
		else{
			key = (_v2 << 32) + _v1;
		}

		unsigned int ret;
		if (m_indicesCache.getValue(key, &ret)){
			return ret;
		}

		unsigned int index = addVertex(m_vertices[v1] + m_vertices[v2]);
		m_indicesCache.add(key, index);
		return index;
	}
protected:
	TFSphere(int recursionLevel){
		// Create 12 vertices of a icosahedron.
		float t = (1.0f + sqrtf(5.0f)) * 0.5f;

		addVertex(glm::vec3(-1.0f,  t, 0.0f));
		addVertex(glm::vec3( 1.0f,  t, 0.0f));
		addVertex(glm::vec3(-1.0f, -t, 0.0f));
		addVertex(glm::vec3( 1.0f, -t, 0.0f));

		addVertex(glm::vec3(0.0f, -1.0f,  t));
		addVertex(glm::vec3(0.0f,  1.0f,  t));
		addVertex(glm::vec3(0.0f, -1.0f, -t));
		addVertex(glm::vec3(0.0f,  1.0f, -t));

		addVertex(glm::vec3( t, 0.0f, -1.0f));
		addVertex(glm::vec3( t, 0.0f,  1.0f));
		addVertex(glm::vec3(-t, 0.0f, -1.0f));
		addVertex(glm::vec3(-t, 0.0f,  1.0f));

		m_faces.push_back(glm::uvec3(0, 11, 5));
		m_faces.push_back(glm::uvec3(0, 5, 1));
		m_faces.push_back(glm::uvec3(0, 1, 7));
		m_faces.push_back(glm::uvec3(0, 7, 10));
		m_faces.push_back(glm::uvec3(0, 10, 11));

		m_faces.push_back(glm::uvec3(1, 5, 9));
		m_faces.push_back(glm::uvec3(5, 11, 4));
		m_faces.push_back(glm::uvec3(11, 10, 2));
		m_faces.push_back(glm::uvec3(10, 7, 6));
		m_faces.push_back(glm::uvec3(7, 1, 8));

		m_faces.push_back(glm::uvec3(3, 9, 4));
		m_faces.push_back(glm::uvec3(3, 4, 2));
		m_faces.push_back(glm::uvec3(3, 2, 6));
		m_faces.push_back(glm::uvec3(3, 6, 8));
		m_faces.push_back(glm::uvec3(3, 8, 9));

		m_faces.push_back(glm::uvec3(4, 9, 5));
		m_faces.push_back(glm::uvec3(2, 4, 11));
		m_faces.push_back(glm::uvec3(6, 2, 10));
		m_faces.push_back(glm::uvec3(8, 6, 7));
		m_faces.push_back(glm::uvec3(9, 8, 1));

		for (int i = 0; i < recursionLevel; ++i){
			std::vector<glm::uvec3> newFaces;
			for each (auto &face in m_faces){
				unsigned int a = getMiddlePoint(face.x, face.y);
				unsigned int b = getMiddlePoint(face.y, face.z);
				unsigned int c = getMiddlePoint(face.z, face.x);

				newFaces.push_back(glm::uvec3(face.x, a, c));
				newFaces.push_back(glm::uvec3(face.y, b, a));
				newFaces.push_back(glm::uvec3(face.z, c, b));
				newFaces.push_back(glm::uvec3(a, b, c));
			}
			m_faces = newFaces;
		}

		std::multimap<unsigned int, glm::vec3> mm;
		for each (auto &face in m_faces){
			glm::vec3 e1 = m_vertices[face.y] - m_vertices[face.x];
			glm::vec3 e2 = m_vertices[face.z] - m_vertices[face.x];
			glm::vec3 n = glm::normalize(glm::cross(e1, e2));
			mm.insert(std::pair<unsigned int, glm::vec3>(face.x, n));
			mm.insert(std::pair<unsigned int, glm::vec3>(face.y, n));
			mm.insert(std::pair<unsigned int, glm::vec3>(face.z, n));
		}
		m_normals = std::vector<glm::vec3>(m_vertices.size(), glm::vec3(0.0f));
		for each(auto &pair in mm){
			m_normals[pair.first] += pair.second;
		}
		//for each(auto &normal in m_normals){
		for (auto it = m_normals.begin(); it != m_normals.end(); ++it){
			*it = glm::normalize(*it);
		}

		vertexBuffer = TFArrayBuffer::create(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);
		vertexBuffer->retain();
		normalBuffer = TFArrayBuffer::create(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), m_normals.data(), GL_STATIC_DRAW);
		normalBuffer->retain();
		elementBuffer = TFArrayBuffer::create(GL_ELEMENT_ARRAY_BUFFER, m_faces.size() * sizeof(glm::uvec3), m_faces.data(), GL_STATIC_DRAW);
		elementBuffer->retain();

		m_indicesCache.clear();
	}

public:
	static TFSphere* create(int recursionLevel = 0){
		return static_cast<TFSphere *>((new TFSphere(recursionLevel))->autorelease());
	}
};

#endif