#ifndef _TFMESH_H_
#define _TFMESH_H_

#include "gl\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include <map>
#include <string>

struct PackedVertex{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator < (const PackedVertex that) const;
};

class TFMesh{
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;
	std::vector<unsigned short> m_indices;

	GLuint m_vertexBuffer;
	GLuint m_uvBuffer;
	GLuint m_normalBuffer;
	GLuint m_elementBuffer;

	void operator = (TFMesh const &);
	TFMesh(TFMesh const &);

	std::vector<std::string> split(std::string s, const char *diameters);

	bool getSimilarVertexIndex(PackedVertex &packed, std::map<PackedVertex, unsigned short> &vertexToOutIndex, unsigned short &result);
	bool loadObjFile(const char *objFilePath);
	void generateBuffers();
public:
	TFMesh(const char *objFilePath);
	~TFMesh();

	const std::vector<glm::vec3>& getVertices() const;
	const std::vector<glm::vec2>& getUvs() const;
	const std::vector<glm::vec3>& getNormals() const;
	const std::vector<unsigned short>& getIndices() const;
	GLuint getVertexBuffer() const;
	GLuint getUvBuffer() const;
	GLuint getNormalBuffer() const;
	GLuint getElementBuffer() const;
};

#endif