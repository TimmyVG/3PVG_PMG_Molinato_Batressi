#include "mew/Transform.hpp"
#include <glm/glm.hpp>


namespace MEW {
	TransformComponent::TransformComponent()
	{
		mat_ = glm::mat4(1.0f);
		scale_ = glm::vec3(1.0f);
		rotation_ = glm::vec3(0.0f);
		translation_ = glm::vec3(0.0f);
	}

	void TransformSystem::SetTranslation(glm::vec3 pos, TransformComponent* tc)
	{
		tc->translation_ = pos;
	}

	void TransformSystem::Translate(glm::vec3 pos, TransformComponent* tc) {
		tc->translation_ += pos;
	}

	void TransformSystem::TranslateX(float pos, TransformComponent* tc)
	{
		tc->translation_.x += pos;
	}

	void TransformSystem::TranslateY(float pos, TransformComponent* tc)
	{
		tc->translation_.y += pos;
	}

	void TransformSystem::TranslateZ(float pos, TransformComponent* tc)
	{
		tc->translation_.z += pos;
	}

	void TransformSystem::SetRotation(glm::vec3 rot, TransformComponent* tc)
	{
		tc->rotation_ = rot;
	}

	void TransformSystem::Rotate(glm::vec3 rot, TransformComponent* tc) {
		tc->rotation_ += rot;
	}

	void TransformSystem::RotateX(float rot, TransformComponent* tc) {
		tc->rotation_.x += rot;
	}

	void TransformSystem::RotateY(float rot, TransformComponent* tc) {
		tc->rotation_.y += rot;
	}

	void TransformSystem::RotateZ(float rot, TransformComponent* tc) {
		tc->rotation_.z += rot;
	}

	void TransformSystem::SetScale(glm::vec3 scale, TransformComponent* tc)
	{
		tc->scale_ = scale;
	}

	void TransformSystem::Scale(glm::vec3 scale, TransformComponent* tc) {
		tc->scale_ += scale;
	}

	void TransformSystem::ScaleX(float scale, TransformComponent* tc) {
		tc->scale_.x += scale;
	}

	void TransformSystem::ScaleY(float scale, TransformComponent* tc) {
		tc->scale_.y += scale;
	}

	void TransformSystem::ScaleZ(float scale, TransformComponent* tc) {
		tc->scale_.z += scale;
	}

	glm::vec3 TransformSystem::GetScale( TransformComponent* tc)
	{
		return glm::vec3(tc->scale_);
	}

	glm::vec3 TransformSystem::GetRotation( TransformComponent* tc)
	{
		return glm::vec3(tc->rotation_);
	}

	glm::vec3 TransformSystem::GetTranslation( TransformComponent* tc)
	{
		return glm::vec3(tc->translation_);
	}


}