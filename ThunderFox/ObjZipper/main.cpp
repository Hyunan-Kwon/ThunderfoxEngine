#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <map>
#include <set>

using namespace std;
using namespace tinyobj;

struct MaterialInfo{
	vector<uint32_t> ranges;
	vector<uint32_t> ids;
};

struct vec3{
	float x, y, z;

	vec3() : x(0.0f), y(0.0f), z(0.0f) { }
	vec3(float x, float y, float z) : x(x), y(y), z(z) { }
	vec3 operator - (vec3 &v){
		vec3 r;
		r.x = this->x - v.x;
		r.y = this->y - v.y;
		r.z = this->z - v.z;
		return r;
	}
	vec3 operator * (vec3 &v){
		vec3 r;
		r.x = this->y * v.z - this->z * v.y;
		r.y = this->z * v.x - this->x * v.z;
		r.z = this->x * v.y - this->y * v.x;
		return r;
	}
	void operator += (vec3 &v){
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
	}
	void normalize(){
		if (x + y + z == 0.0f){
			//cerr << "This is zero vector. Can't be normalized." << endl;
			return;
		}
		float length = sqrt(x * x + y * y + z * z);
		x = x / length;
		y = y / length;
		z = z / length;
	}
};

int main(int argc, char **argv){
	if (argc != 2){
		cerr << "objZipper <input filename>" << endl;
		return 0;
	}
	else if (sizeof(float) != sizeof(uint32_t)){
		cerr << "This system is unsupportable." << endl;
		return 0;
	}

	string path = argv[1];
	size_t filenameStartIndex = path.find_last_of('/') + 1;
	string directory = path.substr(0, filenameStartIndex);
	string filenameWithoutExt = path.substr(filenameStartIndex, path.find_last_of('.') - filenameStartIndex);

	vector<shape_t> shapes;
	vector<material_t> materials;
	string err = LoadObj(shapes, materials, path.c_str(), directory.c_str());
	if (!err.empty()){
		cout << err << endl;
		return 0;
	}

	const uint32_t nShapes = (uint32_t)shapes.size();
	const uint32_t nMaterials = (uint32_t)materials.size();

	// Gen material information.
	vector<MaterialInfo> materialInfos;
	for (uint32_t i = 0; i<nShapes; ++i){
		MaterialInfo materialInfo = { vector<uint32_t>(1, 0), vector<uint32_t>(1, 0) };
		vector<int> &material_ids = shapes[i].mesh.material_ids;
		if (!materials.empty()){
			for (size_t j = 0; j<material_ids.size(); ++j){
				if (materialInfo.ids.back() != material_ids[j]){
					materialInfo.ids.push_back(material_ids[j]);
					materialInfo.ranges.push_back(j * 3);
				}
			}
		}
		materialInfo.ranges.push_back(material_ids.size() * 3);
		materialInfos.push_back(materialInfo);
	}

	// Gen normals if not exist.
	for (uint32_t i = 0; i<nShapes; ++i){
		mesh_t &mesh = shapes[i].mesh;
		if (!mesh.normals.empty()){
			continue;
		}
		multimap<unsigned int, vec3> mm;
		for (size_t j = 0; j<mesh.indices.size(); j += 3){
			vec3 v1(mesh.positions[mesh.indices[j] * 3],
				mesh.positions[mesh.indices[j] * 3 + 1],
				mesh.positions[mesh.indices[j] * 3 + 2]);
			vec3 v2(mesh.positions[mesh.indices[j + 1] * 3],
				mesh.positions[mesh.indices[j + 1] * 3 + 1],
				mesh.positions[mesh.indices[j + 1] * 3 + 2]);
			vec3 v3(mesh.positions[mesh.indices[j + 2] * 3],
				mesh.positions[mesh.indices[j + 2] * 3 + 1],
				mesh.positions[mesh.indices[j + 2] * 3 + 2]);
			vec3 e1 = v2 - v1;
			vec3 e2 = v3 - v1;
			vec3 n = e1 * e2;
			n.normalize();
			mm.insert(pair<unsigned int, vec3>(mesh.indices[j], n));
			mm.insert(pair<unsigned int, vec3>(mesh.indices[j + 1], n));
			mm.insert(pair<unsigned int, vec3>(mesh.indices[j + 2], n));
		}
		vector<vec3> normals(mesh.positions.size() / 3, vec3());
		multimap<unsigned int, vec3>::iterator it = mm.begin();
		for (; it != mm.end(); ++it){
			normals[it->first] += it->second;
		}
		for (vector<vec3>::iterator it = normals.begin(); it < normals.end(); ++it){
			it->normalize();
			mesh.normals.push_back(it->x);
			mesh.normals.push_back(it->y);
			mesh.normals.push_back(it->z);
		}
	}

	ofstream stream((filenameWithoutExt + ".tfm").c_str(), ios::binary);

	stream.write((const char *)&nShapes, sizeof(uint32_t));
	for (uint32_t i = 0; i<nShapes; ++i){
		const uint32_t nVertices = (uint32_t)shapes[i].mesh.positions.size();
		const uint32_t nTexcoords = (uint32_t)shapes[i].mesh.texcoords.size();
		const uint32_t nNormals = (uint32_t)shapes[i].mesh.normals.size();
		const uint32_t nIndices = (uint32_t)shapes[i].mesh.indices.size();

		stream.write((const char *)&nVertices, sizeof(uint32_t));
		stream.write((const char *)&nTexcoords, sizeof(uint32_t));
		stream.write((const char *)&nNormals, sizeof(uint32_t));
		stream.write((const char *)&nIndices, sizeof(uint32_t));
		stream.write((const char *)shapes[i].mesh.positions.data(), nVertices * sizeof(uint32_t));
		stream.write((const char *)shapes[i].mesh.texcoords.data(), nTexcoords * sizeof(uint32_t));
		stream.write((const char *)shapes[i].mesh.normals.data(), nNormals * sizeof(uint32_t));
		stream.write((const char *)shapes[i].mesh.indices.data(), nIndices * sizeof(uint32_t));

		const uint32_t nRanges = (uint32_t)materialInfos[i].ranges.size();
		const uint32_t nIds = (uint32_t)materialInfos[i].ids.size();

		stream.write((const char *)&nRanges, sizeof(uint32_t));
		stream.write((const char *)&nIds, sizeof(uint32_t));
		stream.write((const char *)materialInfos[i].ranges.data(), nRanges * sizeof(uint32_t));
		stream.write((const char *)materialInfos[i].ids.data(), nIds * sizeof(uint32_t));
	}

	stream.write((const char *)&nMaterials, sizeof(uint32_t));
	for (uint32_t i = 0; i<nMaterials; ++i){
		stream.write((const char *)materials[i].ambient, 3 * sizeof(uint32_t));
		stream.write((const char *)materials[i].diffuse, 3 * sizeof(uint32_t));
		stream.write((const char *)materials[i].specular, 3 * sizeof(uint32_t));
		stream.write((const char *)&materials[i].shininess, sizeof(uint32_t));

		const uint32_t nAmbientTexname = (uint32_t)materials[i].ambient_texname.size();
		const uint32_t nDiffuseTexname = (uint32_t)materials[i].diffuse_texname.size();

		stream.write((const char *)&nAmbientTexname, sizeof(uint32_t));
		stream.write((const char *)&nDiffuseTexname, sizeof(uint32_t));
		stream.write((const char *)materials[i].ambient_texname.data(), nAmbientTexname * sizeof(int8_t));
		stream.write((const char *)materials[i].diffuse_texname.data(), nDiffuseTexname * sizeof(int8_t));
	}

	stream.close();

	cout << "Converting was successful." << endl;
	return 0;
}
