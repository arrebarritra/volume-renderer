#pragma once

#include "util.h"
#include "shader.h"

struct Material {
	unsigned int diffuse, gloss, normal, displacement;

	Material() { diffuse = 0; gloss = 0; normal = 0; displacement = 0; };
	Material(std::string directory, std::string extension) { loadMaterial(directory, extension); };

	void loadMaterial(std::string directory, std::string extension) {
		diffuse = loadTexture((directory + "/diffuse." + extension).c_str());
		gloss = loadTexture((directory + "/gloss." + extension).c_str());
		normal = loadTexture((directory + "/normal." + extension).c_str());
		displacement = loadTexture((directory + "/displacement." + extension).c_str());
	}

	static void setShaderMaterial(Shader& shader) {
		shader.use();
		shader.setInt("diffuse_map", 0);
		shader.setInt("gloss_map", 1);
		shader.setInt("normal_map", 2);
		shader.setInt("displacement_map", 3);
	}

	void bind() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gloss);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, displacement);
	}
};