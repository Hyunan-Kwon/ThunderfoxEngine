#include "TFMesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstring>

bool PackedVertex::operator < (const PackedVertex that) const{
	return memcmp((void *)this, (void *)&that, sizeof(PackedVertex)) > 0;
}

std::vector<std::string> TFMesh::split(std::string s, const char *diameters){
	std::vector<std::string> tokens;
	size_t start = -1, end = -1;
	while ((start = s.find_first_not_of(diameters, end + 1)) != std::string::npos){
		end = s.find_first_of(diameters, start + 1);
		if (end == std::string::npos){
			end = s.size() + 1;
		}
		tokens.push_back(s.substr(start, end - start));
	}
	return tokens;
}

bool TFMesh::getSimilarVertexIndex(PackedVertex &packed, std::map<PackedVertex, unsigned short> &vertexToOutIndex, unsigned short &result){
	std::map<PackedVertex, unsigned short>::iterator it = vertexToOutIndex.find(packed);
	if (it == vertexToOutIndex.end()){
		return false;
	}
	else{
		result = it->second;
		return true;
	}
}

struct PackedIndex{
	unsigned int vertexIndex, uvIndex, normalIndex;

	bool operator == (const PackedIndex packedIndex) const{
		return memcmp((void *)this, (void *)&packedIndex, sizeof(PackedIndex)) == 0;
	}
};

unsigned int fixIndex(int index, unsigned int size){
	if (index > 0){
		return index - 1;
	}
	else if (index == 0){
		return 0;
	}
	else{
		return index + size;
	}
}

bool TFMesh::loadObjFile(const char *objFilePath){
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	// Open file stream.
	std::ifstream objFileStream(objFilePath, std::ios::in);
	if (objFileStream.is_open() == false){
		std::cout << "Impossible to open \"" << objFilePath << "\". Are you in the right directory?" << std::endl;
		return false;
	}

	int hit = 0;
	std::string head;
	while (objFileStream >> head){
		if (head.compare("v") == 0){
			glm::vec3 vertex;
			objFileStream >> vertex.x >> vertex.y >> vertex.z;
			temp_vertices.push_back(vertex);
		}
		else if (head.compare("vt") == 0){
			glm::vec2 uv;
			objFileStream >> uv.x >> uv.y;
			//uv.y = -uv.y; // Only for DDS texture.
			temp_uvs.push_back(uv);
		}
		else if (head.compare("vn") == 0){
			glm::vec3 normals;
			objFileStream >> normals.x >> normals.y >> normals.z;
			temp_normals.push_back(normals);
		}
		else if (head.compare("f") == 0){
			++hit;
			std::stringstream ss;
			std::string line;
			std::getline(objFileStream, line);
			std::vector<std::string> values = split(line, " /");
			if (values.size() != 9){
				continue;
				//std::cout << "File can't be read by this parser." << std::endl;
				//objFileStream.close();
				//return false;
			}
			for (int i = 0; i < 3; ++i){
				int value;
				ss << values[i * 3];
				ss >> value;
				ss.clear();
				vertexIndices.push_back(fixIndex(value, temp_vertices.size()));
				ss << values[i * 3 + 1];
				ss >> value;
				ss.clear();
				uvIndices.push_back(fixIndex(value, temp_uvs.size()));
				ss << values[i * 3 + 2];
				ss >> value;
				ss.clear();
				normalIndices.push_back(fixIndex(value, temp_normals.size()));
			}
		}
		else{
			//std::cout << head << std::endl;
			objFileStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
	objFileStream.close();

	std::vector<PackedIndex> usedIndices;

	for (unsigned int i = 0; i < vertexIndices.size(); ++i){
		PackedIndex packedIndex = { vertexIndices[i], uvIndices[i], normalIndices[i] };
		std::vector<PackedIndex>::iterator found = std::find(usedIndices.begin(), usedIndices.end(), packedIndex);
		if (found == usedIndices.end()){
			// Not used index
			m_vertices.push_back(temp_vertices[packedIndex.vertexIndex]);
			m_uvs.push_back(temp_uvs[packedIndex.uvIndex]);
			m_normals.push_back(temp_normals[packedIndex.normalIndex]);
			m_indices.push_back(m_vertices.size() - 1);
		}
		else{
			// Used index
			m_indices.push_back(found - usedIndices.begin());
		}
	}


	//std::vector<glm::vec3> unindexedVertices;
	//std::vector<glm::vec2> unindexedUvs;
	//std::vector<glm::vec3> unindexedNormals;

	//for (unsigned int i = 0; i < vertexIndices.size(); ++i){
	//	unsigned int vertexIndex = vertexIndices[i];
	//	unsigned int uvIndex = uvIndices[i];
	//	unsigned int normalIndex = normalIndices[i];

	//	glm::vec3 vertex = temp_vertices[vertexIndex - 1];
	//	glm::vec2 uv = temp_uvs[uvIndex - 1];
	//	glm::vec3 normal = temp_normals[normalIndex - 1];

	//	unindexedVertices.push_back(vertex);
	//	unindexedUvs.push_back(uv);
	//	unindexedNormals.push_back(normal);
	//}

	//std::map<PackedVertex, unsigned short> vertexToOutIndex;

	//// For each input vertex.
	//for (unsigned int i = 0; i < unindexedVertices.size(); ++i){
	//	PackedVertex packed = { unindexedVertices[i], unindexedUvs[i], unindexedNormals[i] };

	//	//Try to find a similar vertex
	//	unsigned short index;
	//	bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);
	//	if (found){
	//		m_indices.push_back(index);
	//	}
	//	else{
	//		m_vertices.push_back(unindexedVertices[i]);
	//		m_uvs.push_back(unindexedUvs[i]);
	//		m_normals.push_back(unindexedNormals[i]);
	//		unsigned short newIndex = (unsigned short)m_vertices.size() - 1;
	//		m_indices.push_back(newIndex);
	//		vertexToOutIndex[packed] = newIndex;
	//	}
	//}

	return true;
}

void TFMesh::generateBuffers() {
	// Generate vertex buffer.
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices.front(), GL_STATIC_DRAW);
	
	// Generate uv buffer.
	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2), &m_uvs.front(), GL_STATIC_DRAW);

	// Generate normal buffer.
	glGenBuffers(1, &m_normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals.front(), GL_STATIC_DRAW);

	// Generate element buffer.
	glGenBuffers(1, &m_elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned short), &m_indices.front(), GL_STATIC_DRAW);
}

TFMesh::TFMesh(const char *objFilePath)
	:m_vertexBuffer(0u), m_uvBuffer(0u), m_normalBuffer(0u), m_elementBuffer(0u){
	if (loadObjFile(objFilePath)){
		generateBuffers();
	}
}

TFMesh::~TFMesh(){
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_uvBuffer);
	glDeleteBuffers(1, &m_normalBuffer);
	glDeleteBuffers(1, &m_elementBuffer);
}

const std::vector<glm::vec3>& TFMesh::getVertices() const{
	return m_vertices;
}
const std::vector<glm::vec2>& TFMesh::getUvs() const{
	return m_uvs;
}
const std::vector<glm::vec3>& TFMesh::getNormals() const{
	return m_normals;
}
const std::vector<unsigned short>& TFMesh::getIndices() const{
	return m_indices;
}
GLuint TFMesh::getVertexBuffer() const{
	return m_vertexBuffer;
}
GLuint TFMesh::getUvBuffer() const{
	return m_uvBuffer;
}
GLuint TFMesh::getNormalBuffer() const{
	return m_normalBuffer;
}
GLuint TFMesh::getElementBuffer() const{
	return m_elementBuffer;
}
