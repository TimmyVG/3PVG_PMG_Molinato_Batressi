#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include <iostream>
#include <cstdlib>  // Para rand() y srand()
#include <ctime> 


int global = 0;


#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

void DrawSystem(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
	const std::vector<std::optional<MEW::RenderComponent>>& vecRender, MEW::RenderSystem& RS, MEW::Shader& shader) {

	//for (const auto& transform : vecTransform) {
	//for (std::vector<std::optional<MEW::TransformComponent>>::const_iterator itTransform = vecTransform.begin(); itTransform != vecTransform.end(); itTransform++) {
	//for (auto itTransform = vecTransform.begin(); itTransform != vecTransform.end(); itTransform++) {
	shader.UseProgram();

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 460.0f, 0.1f, 100.0f);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	auto itRender = vecRender.begin();
	auto itTransform = vecTransform.begin();
	for (; itTransform != vecTransform.end() && itRender != vecRender.end(); itTransform++, itRender++) {
		if (!itRender->has_value() || !itTransform->has_value()) continue;
		auto& render = itRender->value();
		auto& transform = itTransform->value();
		glm::mat4 modelo = glm::mat4(1.0f);

		modelo = glm::translate(modelo, transform.translation_);
		modelo = glm::rotate(modelo, transform.rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelo = glm::rotate(modelo, transform.rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelo = glm::rotate(modelo, transform.rotation_.z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelo = glm::scale(modelo, transform.scale_);



		shader.setMat4("model", modelo);

		//rc->object->model->Draw(*rc->object->shader_);
		//for (unsigned int i = 0; i < render.object->model->meshes.size(); i++)
		for (const auto& mesh : render.object->model->meshes)
		{
			// draw mesh

			unsigned int diffuseNr = 1;

			for (unsigned int j = 0; j < mesh.textures_.size(); j++) {
				glActiveTexture(GL_TEXTURE0 + j);
				std::string number;
				std::string name = mesh.textures_[j].type;
				//To simplificar
				if (name == "texture_diffuse") {
					number = std::to_string(diffuseNr++);
				}
				shader.setInt((name + number).c_str(), j);
				glBindTexture(GL_TEXTURE_2D, mesh.textures_[j].id);
			}
			glActiveTexture(GL_TEXTURE0);

			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glBindVertexArray(mesh.VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

}


int WinMain() {
	srand(static_cast<unsigned>(time(0)));

	MEW::ECSManager ecs;


	//AddComponents to ecs

	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();


	MEW::TransformSystem TS;
	MEW::RenderSystem RS;



	auto maybe_ws = MEW::WindowSystem::make();
	if (!maybe_ws)
	{
		return -1;
	}
	auto ws = maybe_ws.value();
	std::string title = "Window Example Triangle";
	auto maybe_w = MEW::Window::make(640, 460, title, ws);
	if (!maybe_w) {
		return -1;
	}
	MEW::Window w = maybe_w.value();
	MEW::Shader shader("../data/example.vs","../data/example.fs");
	
	MEW::Object objmiku(&shader);
	objmiku.model->loadModel("../data/miku/source/Miku.fbx");
	objmiku.model->loadMeshes();

	MEW::Object objsilla(&shader);
	objsilla.model->loadModel("../data/silla/WoodenChair_low.fbx");
	objsilla.model->loadMeshes();

	std::vector<size_t> entities;

	for (int i = 0; i < 100; ++i) {
		size_t entity = ecs.create_entity();
		entities.push_back(entity);
		ecs.add_component<MEW::RenderComponent>(entity);
		ecs.add_component<MEW::TransformComponent>(entity);
		if (i != 99) {
			MEW::RenderComponent* rc = &ecs.get_component<MEW::RenderComponent>(entity).value();
			*rc->object = objsilla;
			TS.Translate(glm::vec3((rand() % 50) - 25.0f, (rand() % 30) - 15.0f, -50.0f), &ecs.get_component<MEW::TransformComponent>(entity).value());

		}
		else {
			MEW::RenderComponent* rc = &ecs.get_component<MEW::RenderComponent>(entity).value();
			*rc->object = objmiku;
			TS.Translate(glm::vec3(0.00f, 0.00f, -10.0f), &ecs.get_component<MEW::TransformComponent>(entity).value());

		}
	}
	

	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };


	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		
		if (w.isKeyPressed('W')) TS.TranslateY(static_cast<float>(deltaTime) * 1,&ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		if (w.isKeyPressed('A')) TS.TranslateX(static_cast<float>(deltaTime) * -1, &ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		if (w.isKeyPressed('S')) TS.TranslateY(static_cast<float>(deltaTime) * -1, &ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		if (w.isKeyPressed('D')) TS.TranslateX(static_cast<float>(deltaTime) * 1, &ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		if (w.isKeyPressed('Q')) TS.RotateX(1 * static_cast<float>(deltaTime), &ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		if (w.isKeyPressed('E')) TS.RotateX(-1 * static_cast<float>(deltaTime), &ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		if (w.isKeyPressed('Z')) TS.Scale(glm::vec3(1 * static_cast<float>(deltaTime)), &ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		if (w.isKeyPressed('X')) TS.Scale(glm::vec3(-1 * static_cast<float>(deltaTime)), &ecs.get_component<MEW::TransformComponent>(entities.at(99)).value());
		/*
		std::vector<std::optional<MEW::RenderComponent>> *vecRender = &ecs.get_vecectorComponent<MEW::RenderComponent>();
		std::vector<std::optional<MEW::TransformComponent>> *vecTransf = &ecs.get_vecectorComponent<MEW::TransformComponent>();
		std::vector< std::optional<MEW::RenderComponent>>::iterator itRender = ecs.get_vecectorComponent<MEW::RenderComponent>().begin();
		std::vector< std::optional<MEW::TransformComponent>>::iterator itTransf = ecs.get_vecectorComponent<MEW::TransformComponent>().begin();
		while (itRender != ecs.get_vecectorComponent<MEW::RenderComponent>().end() || itTransf != ecs.get_vecectorComponent<MEW::TransformComponent>().end()) {
			if (itRender->has_value() && itTransf->has_value()) {
				RS.Draw(&itRender->value(),
					&itTransf->value());
			}
			itRender++;
			itTransf++;
		}*/
		/*
		for (int i = 0; i < vecRender->size(); i++) {
			if (itRender->has_value() && itTransf->has_value() ) {
				if (i != 99) {
					TS.Rotate(glm::vec3(0.001f, 0.003f, 0.008f), &itTransf->value());
				}
				RS.Draw(&itRender->value(),
					&itTransf->value());
			}
		}
		*/
		for (int i = 0; i < 100; i++) {
			//if (i != 99)TS.Rotate(glm::vec3(0.001f, 0.003f, 0.008f), &ecs.get_component<MEW::TransformComponent>(entities.at(i)).value());



			//RS.Draw(&ecs.get_component<MEW::RenderComponent>(entities.at(i)).value(),
			//	&ecs.get_component<MEW::TransformComponent>(entities.at(i)).value());
		}
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();

		DrawSystem(vecT,vecR,RS, shader);
		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}
