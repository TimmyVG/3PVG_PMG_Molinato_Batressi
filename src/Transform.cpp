#include "mew/Transform.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>


namespace MEW {
	void TransformSystemMat::operator()(std::vector<std::optional<MEW::TransformComponent>>& vecTransform)
	{
		for (auto& ctransform : vecTransform) {
			if (!ctransform.has_value()) continue;
			auto transform = &ctransform.value();
			transform->model = glm::mat4(1.0f);

			transform->model = glm::translate(transform->model, transform->translation_);
			transform->model = glm::rotate(transform->model, transform->rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f));
			transform->model = glm::rotate(transform->model, transform->rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f));
			transform->model = glm::rotate(transform->model, transform->rotation_.z, glm::vec3(0.0f, 0.0f, 1.0f));
			transform->model = glm::scale(transform->model, transform->scale_);

		}
	}

	TransformComponent::TransformComponent()
	{
		mat_ = glm::mat4(1.0f);
		scale_ = glm::vec3(1.0f);
		rotation_ = glm::vec3(0.0f);
		translation_ = glm::vec3(0.0f);
		model = glm::mat4(1.0f);
	}

	void SetTranslation(glm::vec3 pos, TransformComponent* tc)
	{
		tc->translation_ = pos;
	}

	void Translate(glm::vec3 pos, TransformComponent* tc) {
		tc->translation_ += pos;
	}

	void TranslateX(float pos, TransformComponent* tc)
	{
		tc->translation_.x += pos;
	}

	void TranslateY(float pos, TransformComponent* tc)
	{
		tc->translation_.y += pos;
	}

	void TranslateZ(float pos, TransformComponent* tc)
	{
		tc->translation_.z += pos;
	}

	void SetRotation(glm::vec3 rot, TransformComponent* tc)
	{
		tc->rotation_ = rot;
	}

	void Rotate(glm::vec3 rot, TransformComponent* tc) {
		tc->rotation_ += rot;
	}

	void RotateX(float rot, TransformComponent* tc) {
		tc->rotation_.x += rot;
	}

	void RotateY(float rot, TransformComponent* tc) {
		tc->rotation_.y += rot;
	}

	void RotateZ(float rot, TransformComponent* tc) {
		tc->rotation_.z += rot;
	}

	void SetScale(glm::vec3 scale, TransformComponent* tc)
	{
		tc->scale_ = scale;
	}

	void Scale(glm::vec3 scale, TransformComponent* tc) {
		tc->scale_ += scale;
	}

	void ScaleX(float scale, TransformComponent* tc) {
		tc->scale_.x += scale;
	}

	void ScaleY(float scale, TransformComponent* tc) {
		tc->scale_.y += scale;
	}

	void ScaleZ(float scale, TransformComponent* tc) {
		tc->scale_.z += scale;
	}
}