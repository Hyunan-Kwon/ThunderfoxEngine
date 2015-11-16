#pragma once

#include "..\tinyobjloader\tiny_obj_loader.h"
#include "TFObject.h"
#include "TFHandle.h"
#include "TFBuffer.h"
#include "TFTransform.h"
#include "TFTexture.h"
#include "TFCommon.h"
#include <vector>
#include <string>

struct TFMesh{
	std::string name;
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<unsigned int> indices;
	std::vector<int> material_ids; // per-mesh material ID
};

struct TFMaterial{
	std::string name;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 transmittance;
	glm::vec3 emission;
	float shininess;
	float ior;      // index of refraction
	float dissolve; // 1 == opaque; 0 == fully transparent
	int illum;

	std::string ambient_texname;            // map_Ka
	std::string diffuse_texname;            // map_Kd
	std::string specular_texname;           // map_Ks
	std::string specular_highlight_texname; // map_Ns
	std::string bump_texname;               // map_bump, bump
	std::string displacement_texname;       // disp
	std::string alpha_texname;              // map_d

	TFTexture *ambient_texture;
	TFTexture *diffuse_texture;
	TFTexture *specular_texture;
	TFTexture *specular_highlight_texture;
	TFTexture *bump_texture;
	TFTexture *displacement_texture;
	TFTexture *alpha_texture;

	TFMaterial() :
	ambient(0.0f), diffuse(0.0f), specular(0.0f), transmittance(0.0f), emission(0.0f),
	shininess(0.0f), ior(0.0f), dissolve(0.0f), illum(0),
	ambient_texture(nullptr), diffuse_texture(nullptr), specular_texture(nullptr), specular_highlight_texture(nullptr), bump_texture(nullptr), displacement_texture(nullptr), alpha_texture(nullptr) { }
};

class TFModel : public TFTransform{
private:
	void checkAndCreateTexture(const std::string &directory, const std::string &texname, TFTexture *&texture){
		if (!texname.empty()
			&& m_textures.getValue(texname, &texture) == false)
		{
			texture = TFTexture2D::createWithFile((directory + texname).c_str());
			texture->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
			texture->setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			texture->generateMipmap();
			texture->retain();
			m_textures.add(texname, texture);
		}
	}
protected:
	std::vector<TFMesh> m_meshes;
	std::vector<TFMaterial> m_materials;
	TFDictionary<std::string, TFTexture *> m_textures;

	TFModel(const std::string &filename, std::vector<tinyobj::shape_t> &shapes, const std::vector<tinyobj::material_t> &materials)
	: m_meshes(shapes.size()), m_materials(materials.size()){
		// Assign meshes.
		for (int i = 0; i < shapes.size(); ++i){
			auto &m_mesh = m_meshes[i];
			auto &o_shape = shapes[i];

			m_mesh.name = o_shape.name;
			m_mesh.positions.assign(o_shape.mesh.positions.begin(), o_shape.mesh.positions.end());
			m_mesh.normals.assign(o_shape.mesh.normals.begin(), o_shape.mesh.normals.end());
			m_mesh.texcoords.assign(o_shape.mesh.texcoords.begin(), o_shape.mesh.texcoords.end());
			m_mesh.indices.assign(o_shape.mesh.indices.begin(), o_shape.mesh.indices.end());
			m_mesh.material_ids.assign(o_shape.mesh.material_ids.begin(), o_shape.mesh.material_ids.end());
		}

		// Assign materials.
		for (int i = 0; i < materials.size(); ++i){
			auto &m_material = m_materials[i];
			auto &o_material = materials[i];

			m_material.name = o_material.name;
			memcpy(&m_material.ambient, o_material.ambient, sizeof(glm::vec3));
			memcpy(&m_material.diffuse, o_material.diffuse, sizeof(glm::vec3));
			memcpy(&m_material.specular, o_material.specular, sizeof(glm::vec3));
			memcpy(&m_material.transmittance, o_material.transmittance, sizeof(glm::vec3));
			memcpy(&m_material.emission, o_material.emission, sizeof(glm::vec3));
			m_material.shininess = o_material.shininess;
			m_material.ior = o_material.ior;
			m_material.dissolve = o_material.dissolve;
			m_material.illum = o_material.illum;
			m_material.ambient_texname = o_material.ambient_texname;
			m_material.diffuse_texname = o_material.diffuse_texname;
			m_material.specular_texname = o_material.specular_texname;
			m_material.specular_highlight_texname = o_material.specular_highlight_texname;
			m_material.bump_texname = o_material.bump_texname;
			m_material.displacement_texname = o_material.displacement_texname;
			m_material.alpha_texname = o_material.alpha_texname;

			std::string directory = filename.substr(0, filename.find_last_of('/') + 1);
			checkAndCreateTexture(directory, m_material.ambient_texname, m_material.ambient_texture);
			checkAndCreateTexture(directory, m_material.diffuse_texname, m_material.diffuse_texture);
			checkAndCreateTexture(directory, m_material.specular_texname, m_material.specular_texture);
			checkAndCreateTexture(directory, m_material.specular_highlight_texname, m_material.specular_highlight_texture);
			checkAndCreateTexture(directory, m_material.bump_texname, m_material.bump_texture);
			checkAndCreateTexture(directory, m_material.displacement_texname, m_material.displacement_texture);
			checkAndCreateTexture(directory, m_material.alpha_texname, m_material.alpha_texture);
		}
	}
public:
	static TFModel* createWithOBJFile(std::string filename){
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
		bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str());
		TFLOG("%s", err.c_str());
		if (ret){
			TFEXIT("Laoading obj file failure.");
		}

		//return static_cast<TFModel *>(instance->autorelease());
	}

	virtual ~TFModel() { }
};