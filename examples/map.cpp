#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include <iostream>
#include <cstdlib>  // Para rand() y srand()
#include <ctime> 
#include "mew/Camera.hpp"
#include <stb_image.h>
#include "mew/Inspector.hpp"
#include <mew/Identity.hpp>
#include <mew/SceneSerializer.hpp>



int global = 0;


	int main() {
		srand(static_cast<unsigned>(time(0)));
		bool showImgui = false;
		MEW::ECSManager ecs;

		//AddComponents to ecs

		ecs.add_component_type<MEW::TransformComponent>();
		ecs.add_component_type<MEW::RenderComponent>();
		ecs.add_component_type<MEW::LightComponent>();
		ecs.add_component_type<MEW::CameraComponent>();
		ecs.add_component_type<MEW::IdentityComponent>();
		ecs.add_component_type<MEW::RigidBodyComponent>();


		MEW::PhysicsWorld physicsWorld(&ecs);
		size_t ground = ecs.create_entity();
		MEW::TransformComponent groundTransform;
		groundTransform.translation_ = glm::vec3(0, 3.0f, 0);
		groundTransform.scale_ = glm::vec3(50, 1, 50);
		auto& groundTransComp = ecs.add_component<MEW::TransformComponent>(ground);
		groundTransComp.value() = groundTransform;
		glm::vec3 halfExtents(0.5f, 0.5f, 0.5f);
		btRigidBody* groundBody = physicsWorld.AddBox(0, groundTransform, halfExtents);
		auto& rigidbodycomponent = ecs.add_component<MEW::RigidBodyComponent>(ground);
		rigidbodycomponent.value().body = groundBody;

		auto maybe_ws = MEW::WindowSystem::make();
		if (!maybe_ws)
		{
			return -1;
		}
		auto ws = maybe_ws.value();
		std::string title = "Window Deferred";
		auto maybe_w = MEW::Window::make(1280, 720, title, ws);
		if (!maybe_w) {
			return -1;
		}
		MEW::Window w = maybe_w.value();
		MEW::Shader shader("../data/exampleLight.vs", "../data/exampleLight.fs");
		MEW::Shader shaderDepth("../data/exampleDepth.vs", "../data/exampleDepth.fs");
		MEW::Shader shaderDepthCube("../data/exampleDepthCube.vs", "../data/exampleDepthCube.fs", "../data/exampleDepth.gs");
		MEW::Shader shaderDeferredCamera("../data/deferredCameraSSAO.vs", "../data/deferredCameraSSAO.fs");
		MEW::Shader shaderdeferredShading("../data/deferredShading.vs", "../data/ssao_lighting.fs" );
		MEW::Shader shaderSSAO("../data/ssao.vs","../data/ssao.fs");
		MEW::Shader shaderBlur("../data/ssao.vs","../data/ssao_blur.fs");
		MEW::Shader shaderHDR("../data/hdr.vs","../data/hdr.fs");

		MEW::TransformComponent boxTransform;
		boxTransform.translation_ = glm::vec3(0, 10.0f, -10);
		btRigidBody* boxBody = physicsWorld.AddBox(1.0f, boxTransform, glm::vec3(1, 1, 1));



		std::optional<MEW::Model> CorvModel;
		std::vector<MEW::MeshData> CorvMeshData = MEW::loadModel("../data/sponza/sponza.obj").value();
		CorvModel = MEW::Model(CorvMeshData);
		MEW::ModelObject objmiku(ecs);
		*objmiku.GetRenderComponent()->model = CorvModel;
		objmiku.GetTransformComponent()->scale_ = glm::vec3(0.025f, 0.025f, 0.025f);
		objmiku.GetTransformComponent()->rotation_ = glm::vec3(-180.0f, 0.0f, 0.0f);
		objmiku.GetTransformComponent()->translation_ = glm::vec3(0.0f, 0.0f, 0.0f);
		//auto& mikuRb = ecs.add_component<MEW::RigidBodyComponent>(objmiku.GetEntity());
		//mikuRb.value().body = boxBody;
		std::optional<MEW::Model> StatueMesh;
		std::vector<MEW::MeshData> StatueData = MEW::loadModel("../data/crate_box/scene.gltf").value();
		StatueMesh = MEW::Model(StatueData);



		//Add lights
		MEW::Light directional(ecs, MEW::KTypeLight::Directional);
		//MEW::Light directional1(ecs, MEW::KTypeLight::Spot);

		auto light = &ecs.get_component<MEW::TransformComponent>(directional.entity);
		light->value().translation_ = glm::vec3(0.0f, 0.0f, 0.00f);
		light->value().rotation_ = glm::vec3(-80.0f, 0.0f, 0.00f);

		MEW::Light spot(ecs, MEW::KTypeLight::Spot);
		//MEW::Light directional1(ecs, MEW::KTypeLight::Spot);

		auto light1 = &ecs.get_component<MEW::TransformComponent>(spot.entity);
		light1->value().translation_ = glm::vec3(0.0f, 0.0f, 0.00f);
		light1->value().rotation_ = glm::vec3(-80.0f, 0.0f, 0.00f);

		MEW::Light point(ecs, MEW::KTypeLight::Point);
	
		//MEW::Light directional1(ecs, MEW::KTypeLight::Spot);

		auto light2 = &ecs.get_component<MEW::TransformComponent>(point.entity);
		light2->value().translation_ = glm::vec3(0.0f, 0.0f, 0.00f);
		light2->value().rotation_ = glm::vec3(-80.0f, 0.0f, 0.00f);
		auto lightpointlight = &ecs.get_component<MEW::LightComponent>(point.entity);
		MEW::Light ambient(ecs, MEW::KTypeLight::Ambient);
		auto lightAmbient = &ecs.get_component<MEW::LightComponent>(ambient.entity);
		lightAmbient->value().specular = glm::vec3(0.3f, 0.3f, 0.3f);
		lightAmbient->value().fSpecular = 0.3f;

		const float color[3] = { 0.25f,0.3f,0.4f };
		const float color2[3] = { 0.4f,0.3f,0.25f };


		bool done = false;
		const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
		double deltaTime;


		MEW::Input input(w.window_);
		input.assign(MEW::Input::Buttons::KEY_A, MEW::CAMERA_LEFT);
		input.assign(MEW::Input::Buttons::KEY_LEFT, MEW::CAMERA_LEFT);
		input.assign(MEW::Input::Buttons::KEY_D, MEW::CAMERA_RIGHT);
		input.assign(MEW::Input::Buttons::KEY_RIGHT, MEW::CAMERA_RIGHT);
		input.assign(MEW::Input::Buttons::KEY_W, MEW::CAMERA_FORWARD);
		input.assign(MEW::Input::Buttons::KEY_UP, MEW::CAMERA_FORWARD);
		input.assign(MEW::Input::Buttons::KEY_S, MEW::CAMERA_BACK);
		input.assign(MEW::Input::Buttons::KEY_DOWN, MEW::CAMERA_BACK);
		input.assign(MEW::Input::Buttons::MOUSE_2, MEW::CAMERA_ROTATE);
		input.assign(MEW::Input::Buttons::MOUSE_2, MEW::ActionsInspector::CLICK_OUT);
		input.assign(MEW::Input::Buttons::KEY_O, SaveActions::Save);
		input.assign(MEW::Input::Buttons::KEY_P, SaveActions::Load);
		input.assign(MEW::Input::Buttons::KEY_N, 60000);
		input.assign(MEW::Input::Buttons::KEY_M, 60001);
		input.assign(MEW::Input::Buttons::KEY_K, 60002);
		input.assign(MEW::Input::Buttons::KEY_L, 60003);
		input.assign(MEW::Input::Buttons::KEY_B, 60004);
		MEW::Camera cameraTest(ecs, 1280.0f / 720.0f, MEW::CameraType::CAMERA_PERSPECTIVE,
			50.0f, 0.05f, 100.0f, 10.0f);

		auto& camtr = ecs.get_component<MEW::TransformComponent>(cameraTest.entity_);
		camtr.value().translation_ = glm::vec3(-10, 25, -14);
		camtr.value().rotation_ = glm::vec3(-48, 160, 0);


		
		MEW::Shader debugShader("../data/debug.vs", "../data/debug.fs");
		MEW::PhysicsRenderSystem renderSystem;
		renderSystem.SetShaderAndCamera(&debugShader, cameraTest.GetCameraComponent());
		physicsWorld.GetDynamicsWorld()->setDebugDrawer(&renderSystem);

		MEW::Inspector inspector(w);
		inspector.LinkECS(ecs);

	
		while (!done) {
			input.newframe();
			w.newframe(backgroundcolor);
			inspector.NewFrame();

			deltaTime = w.deltaTime();

			cameraTest.update(deltaTime, input);
			inspector.update(deltaTime, input);
			physicsWorld.StepSimulation(deltaTime);
		//	physicsWorld.UpdateTransform(boxBody, boxTransform);
		//	physicsWorld.UpdateTransform(boxBody1, boxTransform);
			//save
			if (input.isKeyPressed(Save))SerializeScene(ecs);
			if (input.isKeyPressed(Load))DeserializeScene(ecs);
			if (input.isKeyPressed(60000))showImgui = false;
			if (input.isKeyPressed(60001))showImgui = true;
			if (input.isKeyPressed(60002)) {
				cameraTest.GetCameraComponent()->ssao = 1;
				cameraTest.GetCameraComponent()->blur = 1;
			}
			if (input.isKeyPressed(60003)) {
				cameraTest.GetCameraComponent()->ssao = 0;
				cameraTest.GetCameraComponent()->blur = 0;
			}

			if (input.isKeyDown(60004)) {
				MEW::TransformComponent boxTransform;
				boxTransform.translation_ = glm::vec3(cos(rand()) * 5, 40.0f, sin(rand()) * 5);
				boxTransform.rotation_ = glm::vec3(3.14f, 00.0f, 0.0f);
				btRigidBody* boxBody = physicsWorld.AddBox(1.0f, boxTransform, glm::vec3(0.75f));
				MEW::ModelObject StatueObj(ecs);
				*StatueObj.GetRenderComponent()->model = StatueMesh;
				StatueObj.GetTransformComponent()->scale_ = glm::vec3(0.025f, 0.025f, 0.025f);
				StatueObj.GetTransformComponent()->rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
				StatueObj.GetTransformComponent()->translation_ = glm::vec3(0.0f, 100.0f, 0.0f);
				auto& StatueRB = ecs.add_component<MEW::RigidBodyComponent>(StatueObj.GetEntity());
				StatueRB.value().body = boxBody;
			}
			MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());


			physicsWorld.GetDynamicsWorld()->debugDrawWorld();

			if(showImgui)inspector.WindowEntities(cameraTest);
			physicsWorld.GetDynamicsWorld()->debugDrawWorld();

			MEW::UpdateLights()(ecs.get_vectorComponent<MEW::TransformComponent>(),
				ecs.get_vectorComponent<MEW::LightComponent>(),
				ecs.get_component<MEW::TransformComponent>(cameraTest.entity_),
				ecs.get_component<MEW::CameraComponent>(cameraTest.entity_));

			MEW::DepthMapsSSAO()(ecs.get_vectorComponent<MEW::TransformComponent>(),
				ecs.get_vectorComponent<MEW::RenderComponent>(),
				ecs.get_vectorComponent<MEW::LightComponent>(),
				shaderDepth, shaderDepthCube,
				ecs.get_component<MEW::CameraComponent>(cameraTest.entity_),
				ecs.get_component<MEW::TransformComponent>(cameraTest.entity_));
				
			MEW::RenderSystemLitSSAO()(ecs.get_vectorComponent<MEW::TransformComponent>(),
				ecs.get_vectorComponent<MEW::RenderComponent>(),
				ecs.get_vectorComponent<MEW::LightComponent>(),
				shaderDeferredCamera,
				ecs.get_component<MEW::CameraComponent>(cameraTest.entity_),
				ecs.get_component<MEW::TransformComponent>(cameraTest.entity_));

			MEW::RenderSSAOTexture()(shaderSSAO,shaderBlur,
				ecs.get_component<MEW::CameraComponent>(cameraTest.entity_),
				ecs.get_component<MEW::TransformComponent>(cameraTest.entity_));

			MEW::LightSystemSSAO()(ecs.get_vectorComponent<MEW::TransformComponent>(),
										ecs.get_vectorComponent<MEW::RenderComponent>(),
										ecs.get_vectorComponent<MEW::LightComponent>(),
				shaderdeferredShading, shaderdeferredShading,shaderHDR,
										ecs.get_component<MEW::CameraComponent>(cameraTest.entity_),ecs.get_component<MEW::TransformComponent>(cameraTest.entity_));

			physicsWorld.GetDynamicsWorld()->debugDrawWorld();




			inspector.Render();
			physicsWorld.GetDynamicsWorld()->debugDrawWorld();

			bool closePressed = w.closedPressed();
			bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
			if (closePressed || escPressed) done = true;
			w.endWindowFrame();
		}


		return 0;
	}
