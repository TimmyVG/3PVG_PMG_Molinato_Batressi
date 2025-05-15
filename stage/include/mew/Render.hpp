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
#include "mew/Shader.hpp"


namespace MEW {
	struct RenderComponent {
		RenderComponent();
		std::shared_ptr<Object> object;

	};

	class RenderSystem {
	public:

		void Draw(RenderComponent* rc, TransformComponent* tc);

	};

	class RenderSystemUnlit {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender, MEW::RenderSystem& RS, MEW::Shader& shader,
			CameraComponent* camComp);
	};

	class RenderSystemLit {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			const std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, 
			std::optional<CameraComponent>* camComp,
			std::optional<TransformComponent>* camCompT);
	};

	class LightSystem {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, std::optional<CameraComponent>* camComp);
	};


}

#endif //__Render__