#ifndef _TFSHADERMANAGER_H_
#define _TFSHADERMANAGER_H_

#include "TFObject.h"
#include "TFShader.h"
#include "TFCommon.h"

class ResourceManager{
	ResourceManager(ResourceManager const&);
	void operator = (ResourceManager const&);
protected:
	TFDictionary<std::string, TFRef *> m_resources;

	ResourceManager() { }
public:
	virtual ~ResourceManager() {
		this->disenroll_all();
	}

	void enroll(std::string name, TFRef *resource) {
		resource->retain();
		m_resources.add(name, resource);
	}

	void disenroll(std::string name){
		TFDictionary<std::string, TFRef *>::iterator found = m_resources.find(name);
		if (found != m_resources.end()){
			found->value->release();
			m_resources.remove(found);
		}
		else{
			TFLOG("\"%s\" is not enrolled in the manager.", name.c_str());
		}
	}

	void disenroll_all(){
		for (auto &resource : m_resources){
			resource.value->release();
		}
		m_resources.clear();
	}

	TFRef* getResource(std::string name) const{
		TFRef *resource = nullptr;
		if (m_resources.getValue(name, &resource) == false){
			TFLOG("Cannot find the resource \"%s\".", name.c_str());
			TFEXIT();
		}
		return resource;
	}
};

class TFShaderManager : public ResourceManager{
private:
public:
	static TFShaderManager* getInstance() {
		static TFShaderManager m_instance;
		return &m_instance;
	}

	virtual ~TFShaderManager() { }

	TFShader* getShader(std::string name) const{
		return static_cast<TFShader *>(getResource(name));
	}

	void init() {
		TFShader *shader = TFShader::create(TFShaderUnit::createWithFile(GL_VERTEX_SHADER, "Shaders/Screen.vert"),
											TFShaderUnit::createWithFile(GL_FRAGMENT_SHADER, "Shaders/TextureOnly.frag"));
		shader->getUniformLocation("Texture00");
		this->enroll("Screen", shader);

		shader = TFShader::create(TFShaderUnit::createWithFile(GL_VERTEX_SHADER, "Shaders/DirectionalLight.vert"),
								TFShaderUnit::createWithFile(GL_FRAGMENT_SHADER, "Shaders/DirectionalLight.frag"));
		shader->getUniformLocation("Texture00");
		shader->getUniformLocation("M");
		shader->getUniformLocation("V");
		shader->getUniformLocation("P");
		shader->getUniformLocation("LightColor");
		shader->getUniformLocation("LightDirection_worldspace");
		shader->getUniformLocation("MaterialAmbient");
		shader->getUniformLocation("MaterialDiffuse");
		shader->getUniformLocation("MaterialSpecular");
		shader->getUniformLocation("MaterialShininess");
		this->enroll("DirectionalLight", shader);

		shader = TFShader::create(TFShaderUnit::createWithFile(GL_VERTEX_SHADER, "Shaders/Gizmo.vert"),
								TFShaderUnit::createWithFile(GL_FRAGMENT_SHADER, "Shaders/Gizmo.frag"));
		shader->getUniformLocation("MVP");
		this->enroll("Gizmo", shader);

		shader = TFShader::create(TFShaderUnit::createWithFile(GL_VERTEX_SHADER, "Shaders/Screen.vert"),
								TFShaderUnit::createWithFile(GL_FRAGMENT_SHADER, "Shaders/SSAO.frag"));
		shader->getUniformLocation("DepthTexture");
		shader->getUniformLocation("NormalTexture");
		shader->getUniformLocation("RenderedTexture");
		shader->getUniformLocation("PositionTexture");
		shader->getUniformLocation("P");
		this->enroll("SSAO", shader);

		shader = TFShader::create(TFShaderUnit::createWithFile(GL_VERTEX_SHADER, "Shaders/simple.vert"),
								TFShaderUnit::createWithFile(GL_FRAGMENT_SHADER, "Shaders/SingleColor.frag"));
		shader->getUniformLocation("M");
		shader->getUniformLocation("V");
		shader->getUniformLocation("P");
		shader->getUniformLocation("Color");
		this->enroll("SingleColor", shader);

		shader = TFShader::create(TFShaderUnit::createWithFile(GL_VERTEX_SHADER, "Shaders/ShadowMap.vert"),
								TFShaderUnit::createWithFile(GL_FRAGMENT_SHADER, "Shaders/ShadowMap.frag"));
		shader->getUniformLocation("MVP");
		this->enroll("ShadowMap", shader);
		
		shader = TFShader::create(TFShaderUnit::createWithFile(GL_COMPUTE_SHADER, "Shaders/BilateralFilter.vert"));
		shader->getUniformLocation("roll");
		shader->getUniformLocation("destTex");
		this->enroll("Compute", shader);
	}
};

#endif