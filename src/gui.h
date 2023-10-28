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

	void Render(glm::vec3& lightDir, Volume& vol) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");

		ImGui::SeparatorText("Lighting");

		if (ImGui::DragFloat3("Light direction", &lightDir[0], 0.1f)) {
			if (lightDir == glm::vec3(0.0f)) lightDir = glm::vec3(0.0f, -1.0f, 0.0f);
			lightDir = glm::normalize(lightDir);
		}

		ImGui::SeparatorText("Volume rendering");

		ImGui::Text("Dimensions: (%d, %d, %d)", vol.dims[0], vol.dims[1], vol.dims[2]);
		ImGui::Text("Range: (%d, %d)", vol.minVal, vol.maxVal);
		ImGui::Text("Mean: %f", vol.mean);
		ImGui::Text("Non-zero mean: %f", vol.meanNonZero);
		ImGui::DragInt("Samples", &vol.samples);
		ImGui::DragFloat("Alpha scale", &vol.alphaScale, 0.1f, 0.0f, 10.0f, "%.1f");

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};