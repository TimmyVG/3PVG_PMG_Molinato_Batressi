#ifndef __RENDER_H__
#define __RENDER_H__ 1
#include <optional>
#include <string>
#include <vector>
#include "Shader.hpp"
#include "glm/mat4x4.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "mew/Light.hpp"
#include "mew/Object.hpp"
#include <memory>
#include "mew/Camera.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Shader.hpp"


namespace MEW {
	struct RenderComponent {
		std::shared_ptr<std::optional<Model>> model;
		bool cast_shadows;
		RenderComponent() : model(std::make_shared<std::optional<Model>>()), cast_shadows(false) {};
	};


	class RenderSystem {
	public:

		void Draw(RenderComponent* rc, TransformComponent* tc);

	};

	class RenderSystemUnlit {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender, MEW::Shader& shader,
			CameraComponent* camComp);
	};

	class RenderSystemLit {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			const std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, 
			std::optional<CameraComponent>& camComp,
			std::optional<TransformComponent>& camCompT);
	};

	class LightSystem {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, std::optional<CameraComponent>& camComp);
	};


	class ModelObject {
	public:
		MEW::TransformComponent* GetTransformComponent();
		MEW::RenderComponent* GetRenderComponent();
		ModelObject(MEW::ECSManager& ecs);
		size_t GetEntity();

	private:
		size_t entity_;
		MEW::ECSManager* ecs_;
	};

}

#endif //__Render__