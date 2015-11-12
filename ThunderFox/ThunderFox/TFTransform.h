#ifndef _TFTRANSFORM_H_
#define _TFTRANSFORM_H_

#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp"
#include "TFObject.h"
#include <list>

class TFTransform : public TFRef{
private:
	void calculateMatrix() {
		m_matrix = glm::rotate(glm::mat4(), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		m_matrix = glm::scale(m_matrix, m_scale);

		m_matrix = glm::translate(m_matrix, m_position);
	}

	TFTransform *m_parent;
	std::list<TFTransform *> m_childs;
protected:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
	glm::mat4 m_matrix;

	TFTransform()
	: m_parent(nullptr), m_scale(glm::vec3(1.0f)) { }
public:
	static TFTransform* create() {
		TFTransform *instance = new TFTransform();
		instance->autorelease();
		return instance;
	}

	virtual ~TFTransform() {
		if (m_parent != nullptr){
			m_parent->removeChild(this);
		}
		for (std::list<TFTransform *>::iterator it = m_childs.begin(); it != m_childs.end(); ++it){
			(*it)->m_parent = nullptr;
			(*it)->release();
		}
	}

	glm::mat4 getMatrix() const {
		if (m_parent == nullptr){
			return m_matrix;
		}
		else{
			return m_parent->getMatrix() * m_matrix;
		}
	}

	void addChild(TFTransform *child){
		child->m_parent = this;
		child->retain();
		m_childs.push_back(child);
	}

	void removeChild(TFTransform *child){
		child->m_parent = nullptr;
		child->release();
		m_childs.remove(child);
	}

	void removeFromParent(){
		if (m_parent != nullptr){
			m_parent->removeChild(this);
		}
	}
	
	void rotate(float x, float y, float z){
		m_rotation += glm::vec3(x, y, z);
		calculateMatrix();
	}
	void setRotation(float x, float y, float z){
		m_rotation = glm::vec3(x, y, z);
		calculateMatrix();
	}
	const glm::vec3& getRotation() const{
		return m_rotation;
	}

	void scale(float x, float y, float z){
		m_scale *= glm::vec3(x, y, z);
		calculateMatrix();
	}
	void setScale(float x, float y, float z){
		m_scale = glm::vec3(x, y, z);
		calculateMatrix();
	}
	const glm::vec3& getScale() const{
		return m_scale;
	}

	void translate(float x, float y, float z){
		m_position += glm::vec3(x, y, z);
		calculateMatrix();
	}
	void setPosition(float x, float y, float z){
		m_position = glm::vec3(x, y, z);
		calculateMatrix();
	}
	const glm::vec3& getPosition() const{
		return m_position;
	}
};

#endif