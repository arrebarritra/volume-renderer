#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class GUI {
public:
	ImGuiIO* io;

	GUI(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = &ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	~GUI() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Render(glm::vec3& lightDir, int& samples) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");

		if (ImGui::DragFloat3("Light direction", &lightDir[0], 0.1f)) {
			if (lightDir == glm::vec3(0.0f)) lightDir = glm::vec3(0.0f, -1.0f, 0.0f);
			lightDir = glm::normalize(lightDir);
		}

		ImGui::DragInt("Samples", &samples);

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};