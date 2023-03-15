#include "app/app.hpp"

#include <ogl/ogl.hpp>
#include <ogl/renderer/pipeline.hpp>
#include <ogl/renderer/renderer.hpp>

App::App() : ogl::Application() {
	push_layer(new ogl::Debug());

	ogl::Window* window_layer = static_cast<ogl::Window*>(get_layer(OGL_RENDERER_WINDOW_LAYER_NAME));
	window_layer->set_clear_color(glm::vec4(0.2f, 0.3f, 0.6f, 1.0f));

	ogl::Pipeline::get()->push_renderer(new ogl::BasicRenderer());

	ogl::AssetHandler::get()->load_texture_into_memory("container", "project/assets/textures/container.png"); // TODO(Ewan): still need to test whether it actually works

	ogl::SceneManager::get()->push("empty scene");
	ogl::SceneManager::get()->set_active("empty scene");
	ogl::Scene* scene = ogl::SceneManager::get()->get_active_scene();


	entt::entity ground = scene->get_registry().create();
	ogl::TransformComponent& ground_transform = scene->get_registry().emplace<ogl::TransformComponent>(ground);
	ogl::ModelRendererComponent& ground_model_renderer = scene->get_registry().emplace<ogl::ModelRendererComponent>(ground);
	ground_model_renderer.model = ogl::AssetHandler::get()->load_model_into_memory("ogl/assets/models/plane", ogl::ModelFileType_Obj);
	ground_transform.position.z = -5;
	ground_transform.position.y = -1.5;

	entt::entity sphere = scene->get_registry().create();
	ogl::TransformComponent& sphere_transform = scene->get_registry().emplace<ogl::TransformComponent>(sphere);
	ogl::ModelRendererComponent& sphere_model_renderer = scene->get_registry().emplace<ogl::ModelRendererComponent>(sphere);
	sphere_model_renderer.model = ogl::AssetHandler::get()->load_model_into_memory("ogl/assets/models/sphere", ogl::ModelFileType_Obj);
	sphere_transform.position.z = -5;
}