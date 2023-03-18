#include "app/app.hpp"

#include <ogl/ogl.hpp>
#include <ogl/renderer/pipeline.hpp>
#include <ogl/renderer/renderer.hpp>
#include <glfw/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

class BasicCameraController : public ogl::System {
public:
	BasicCameraController(ogl::CameraComponent& camera) : m_camera(&camera) {
		glfwSetInputMode(ogl::Pipeline::get()->get_window()->get_internal(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	virtual void on_late_update(entt::registry& registry) override {
		float move_speed = ogl::Input::pressed_key(ogl::InputKeyCode_LeftShift) ? m_move_speed * 2.0f : m_move_speed;

		if (ogl::Input::pressed_key(ogl::InputKeyCode_S)) {
			m_camera->position -= move_speed * m_camera->forward * ogl::Time::get_delta();
		}

		if (ogl::Input::pressed_key(ogl::InputKeyCode_W)) {
			m_camera->position += move_speed * m_camera->forward * ogl::Time::get_delta();
		}

		if (ogl::Input::pressed_key(ogl::InputKeyCode_D)) {
			m_camera->position += move_speed * glm::cross(m_camera->forward, m_camera->up) * ogl::Time::get_delta();
		}

		if (ogl::Input::pressed_key(ogl::InputKeyCode_A)) {
			m_camera->position -= move_speed * glm::cross(m_camera->forward, m_camera->up) * ogl::Time::get_delta();
		}

		if (ogl::Input::pressed_key(ogl::InputKeyCode_LeftControl)) {
			m_camera->position.y -= move_speed * ogl::Time::get_delta();
		}

		if (ogl::Input::pressed_key(ogl::InputKeyCode_Space)) {
			m_camera->position.y += move_speed * ogl::Time::get_delta();
		}

		glm::vec2 mouse_pos = ogl::Input::get_mouse_position();
		static glm::vec2 last_mouse_position = mouse_pos;
		glm::vec2 mouse_movement = mouse_pos - last_mouse_position;
		last_mouse_position = mouse_pos;

		mouse_movement.x *= m_sensitivity;
		mouse_movement.y *= m_sensitivity;

		m_yaw += mouse_movement.x;
		m_pitch -= mouse_movement.y;
		m_pitch = std::clamp(m_pitch, -89.0f, 90.0f);
		
		m_camera->forward = glm::vec3(
			std::cosf(glm::radians(m_yaw)) * std::cosf(glm::radians(m_pitch)),
			std::sinf(glm::radians(m_pitch)),
			std::sinf(glm::radians(m_yaw)) * std::cosf(glm::radians(m_pitch))
		);
	}

private:
	ogl::CameraComponent* m_camera{ nullptr };
	float m_move_speed{ 5.0f };
	float m_sensitivity{ 0.1f };
	float m_yaw{ 0.0f };
	float m_pitch{ 0.0f };

};

App::App() : ogl::Application() {
	push_layer(new ogl::Debug());

	ogl::Window* window_layer = static_cast<ogl::Window*>(get_layer(OGL_RENDERER_WINDOW_LAYER_NAME));

	ogl::Pipeline::get()->push_renderer(new ogl::BasicRenderer());

	ogl::Texture* texture = ogl::AssetHandler::get()->load_texture_into_memory("container", "example/assets/textures/container.png");
	ogl::Material* material = ogl::AssetHandler::get()->load_material_into_memory("container");
	material->textures.push_back(std::make_tuple(texture, ogl::MaterialTextureType_Diffuse));

	ogl::SceneManager::get()->push("empty scene");
	ogl::SceneManager::get()->set_active("empty scene");
	ogl::Scene* scene = ogl::SceneManager::get()->get_active_scene();

	entt::entity camera = scene->get_registry().create();
	ogl::CameraComponent& camera_comp = scene->get_registry().emplace<ogl::CameraComponent>(camera);
	camera_comp.is_main = true;
	camera_comp.projection_type = ogl::CameraProjection_Perspective;
	camera_comp.projection_matrix = glm::perspective(
		glm::radians(45.0f), static_cast<float>(window_layer->get_width()) / static_cast<float>(window_layer->get_height()), 0.1f, 100.0f
	);
	camera_comp.clear_color = glm::vec4(0.2f, 0.5f, 0.9f, 1.0f);

	entt::entity backpack = scene->get_registry().create();
	ogl::TransformComponent& backpack_transform = scene->get_registry().emplace<ogl::TransformComponent>(backpack);
	ogl::ModelRendererComponent& backpack_model_renderer = scene->get_registry().emplace<ogl::ModelRendererComponent>(backpack);
	backpack_model_renderer.model = ogl::AssetHandler::get()->load_model_into_memory("example/assets/models/backpack", ogl::ModelFileType_Obj);
	backpack_transform.rotation = glm::vec4(0.0f, 1.0f, 0.0f, -90.0f);
	backpack_transform.position.x = 4.0f;

	ogl::SceneManager::get()->get_active_scene()->push_system<BasicCameraController>(camera_comp);
}