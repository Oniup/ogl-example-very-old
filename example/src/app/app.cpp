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
	float m_move_speed{ 10.0f };
	float m_sensitivity{ 0.05f };
	float m_yaw{ 0.0f };
	float m_pitch{ 0.0f };

};

App::App() : ogl::Application() {
	push_layer(new ogl::Debug());

	ogl::Window* window_layer = static_cast<ogl::Window*>(get_layer(OGL_RENDERER_WINDOW_LAYER_NAME));

	ogl::Pipeline::get()->push_renderer(new ogl::BasicRenderer());

	ogl::SceneManager::get()->push("empty scene");
	ogl::Scene* scene = ogl::SceneManager::get()->set_active("empty scene");

	entt::entity camera = scene->get_registry().create();
	ogl::CameraComponent& camera_comp = scene->get_registry().emplace<ogl::CameraComponent>(camera);
	camera_comp.is_main = true;
	camera_comp.projection_type = ogl::CameraProjection_Perspective;
	camera_comp.projection_matrix = glm::perspective(
		glm::radians(45.0f), static_cast<float>(window_layer->get_width()) / static_cast<float>(window_layer->get_height()), 0.1f, 100.0f
	);
	camera_comp.clear_color = glm::vec4(0.05f, 0.1f, 0.2f, 1.0f);

	entt::entity backpack = scene->get_registry().create();
	ogl::TransformComponent& backpack_transform = scene->get_registry().emplace<ogl::TransformComponent>(backpack);
	ogl::MeshRendererComponent& backpack_mesh_renderer = scene->get_registry().emplace<ogl::MeshRendererComponent>(backpack);
	backpack_mesh_renderer.model = ogl::AssetHandler::get()->load_model_into_memory("example/assets/models/backpack", ogl::ModelFileType_Obj);
	backpack_transform.rotation = glm::vec4(0.0f, 1.0f, 0.0f, -90.0f);
	backpack_transform.position.x = 8.0f;
	backpack_transform.position.z = -2.0f;

	for (ogl::Mesh& mesh : backpack_mesh_renderer.model->meshes) {
		mesh.material->shininess = 32;
		mesh.material->specular_strength = 2.0f;
	}

	entt::entity sphere = scene->get_registry().create();
	ogl::TransformComponent& sphere_transform = scene->get_registry().emplace<ogl::TransformComponent>(sphere);
	ogl::MeshRendererComponent& sphere_mesh_renderer = scene->get_registry().emplace<ogl::MeshRendererComponent>(sphere);
	sphere_mesh_renderer.model = ogl::AssetHandler::get()->load_model_into_memory("ogl/assets/models/sphere", ogl::ModelFileType_Obj);
	sphere_transform.position.x = 8.0f;
	sphere_transform.position.z = 2.0f;
	ogl::Material* sphere_material = sphere_mesh_renderer.model->meshes[0].material;
	sphere_material->overlay_color = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f);
	sphere_material->shininess = 32;
	sphere_material->specular_strength = 5.0f;

	entt::entity ground = scene->get_registry().create();
	ogl::TransformComponent& ground_transform = scene->get_registry().emplace<ogl::TransformComponent>(ground);
	ogl::MeshRendererComponent& ground_mesh_renderer = scene->get_registry().emplace<ogl::MeshRendererComponent>(ground);
	ground_mesh_renderer.model = ogl::AssetHandler::get()->load_model_into_memory("ogl/assets/models/plane", ogl::ModelFileType_Obj);
	ground_transform.position = glm::vec3(5.0f, -4.0f, 0.0f);
	ground_transform.scale = glm::vec3(10.0f, 1.0f, 10.0f);
	ogl::Material* ground_material = ground_mesh_renderer.model->meshes[0].material;
	ground_material->overlay_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	ground_material->shininess = 16;
	ground_material->specular_strength = 0.5f;

	entt::entity point_light = scene->get_registry().create();
	ogl::LightComponent& point_light_comp = scene->get_registry().emplace<ogl::LightComponent>(point_light);
	ogl::TransformComponent& point_light_transform = scene->get_registry().emplace<ogl::TransformComponent>(point_light);
	ogl::MeshRendererComponent& point_light_mesh_renderer = scene->get_registry().emplace<ogl::MeshRendererComponent>(point_light);

	point_light_mesh_renderer.model = ogl::AssetHandler::get()->load_model_into_memory("ogl/assets/models/cube", ogl::ModelFileType_Obj);
	point_light_mesh_renderer.model->meshes[0].material->overlay_color = glm::vec4(1.0f, 0.7f, 0.7f, 1.0f);
	point_light_mesh_renderer.uses_lights = false;
	point_light_comp.position = glm::vec3(5.0f, 2.0f, -5.0f);
	point_light_comp.color = glm::vec4(1.0f, 0.7f, 0.7f, 1.0f);
	point_light_transform.position = glm::vec3(5.0f, 2.0f, -5.0f);
	point_light_transform.scale = glm::vec3(0.2f, 0.2f, 0.2f);

	entt::entity directional_light = scene->get_registry().create();
	ogl::LightComponent& direction_light_comp = scene->get_registry().emplace<ogl::LightComponent>(directional_light);
	direction_light_comp.type = ogl::LightType_Directional;
	direction_light_comp.color = camera_comp.clear_color * 1.2f;
	direction_light_comp.ambient_color = camera_comp.clear_color;
	direction_light_comp.direction = glm::vec3(-0.5f, 0.7f, -0.3f);

	scene->push_system<BasicCameraController>(camera_comp);
}