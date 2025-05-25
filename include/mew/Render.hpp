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
#include "Physics.hpp"
#include <btBulletDynamicsCommon.h>

namespace MEW {
	struct RenderComponent {
		std::shared_ptr<std::optional<Model>> model;
		bool cast_shadows;
		RenderComponent() : model(std::make_shared<std::optional<Model>>()), cast_shadows(false) {};
	};

	struct WaterComponent {
		std::shared_ptr<std::optional<Mesh>> mesh;
		WaterComponent() : mesh(std::make_shared<std::optional<Mesh>>()) {};
	};


	class PhysicsRenderSystem : public btIDebugDraw {
	private:
		GLuint m_debugVAO;
		GLuint m_debugVBO;
		bool isInitialized;

		int m_debugMode;

	public:
		PhysicsRenderSystem();
		~PhysicsRenderSystem();
		void operator()();
		void InitializeDebugDrawing();

		// Bullet Debug Drawer interface
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color= btVector3(1.0f,0.0f,0.0f)) override;
		virtual void reportErrorWarning(const char* warningString) override;
		virtual void draw3dText(const btVector3& location, const char* textString) override {};
		virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int /*lifeTime*/, const btVector3& color) override {};
		virtual void setDebugMode(int debugMode) override;
		virtual int getDebugMode() const override;
		void SetShaderAndCamera(Shader* shader, CameraComponent* cam);
	private:
		Shader* m_shader = nullptr;
		CameraComponent* m_cam = nullptr;
		void DrawLine(const glm::vec3& from, const glm::vec3& to, Shader& shader, CameraComponent& camComp, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f), float lineWidth = 2.0f);
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

	class RenderSystemLitSSAO {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			const std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader,
			std::optional<CameraComponent>& camComp,
			std::optional<TransformComponent>& camCompT);
	};

	class ForwardRenderSystemLit {
	public:
		void operator()(
			const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			const std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader,
			std::optional<CameraComponent>& camComp,
			std::optional<TransformComponent>& camCompT);
	};

	class ForwardLightSystem {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, Shader& shaderCube,
			std::optional<CameraComponent>& camComp);
	};



	class LightSystem {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, Shader& shaderCube, std::optional<CameraComponent>& camComp,std::optional<TransformComponent>& camComptT);
	};

	class LightSystemSSAO {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, Shader& shaderCube, Shader& shaderHDR, std::optional<CameraComponent>& camComp, std::optional<TransformComponent>& camComptT);
	};

	class RenderSSAOTexture {
	public:
		void operator()(Shader& shader,
										Shader& shaderBlur,
										std::optional<CameraComponent>& cameraCamera,
										std::optional<TransformComponent>& cameraTransform);
	};


	class DepthMaps {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, Shader& shaderCube, std::optional<CameraComponent>& camComp, std::optional<TransformComponent>& camT);
	};

	class DepthMapsSSAO {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
			const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
			std::vector<std::optional<MEW::LightComponent>>& vecLight,
			Shader& shader, Shader& shaderCube, std::optional<CameraComponent>& camComp, std::optional<TransformComponent>& camT);
	};

	class WaterRenderSystem {
	public:
		void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
			const std::vector<std::optional<MEW::WaterComponent>>& vecWater,
			Shader& shader,
			std::optional<CameraComponent>& camComp,float deltatime);
	};


	class ModelObject {
	public:
		MEW::TransformComponent* GetTransformComponent();
		MEW::RenderComponent* GetRenderComponent();

		ModelObject(MEW::ECSManager& ecs);
		size_t GetEntity();

	private:
		bool isPhysicsEnabled;
		size_t entity_;
		MEW::ECSManager* ecs_;
	};

}

#endif //__Render__