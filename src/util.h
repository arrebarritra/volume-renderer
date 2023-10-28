#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include <iostream>
#include <vector>

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube() {
	// initialize (if necessary)
	if (cubeVAO == 0) {
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, // bottom-left
			1.0f, 1.0f, -1.0f,   // top-right
			1.0f, -1.0f, -1.0f,  // bottom-right
			1.0f, 1.0f, -1.0f,   // top-right
			-1.0f, -1.0f, -1.0f, // bottom-left
			-1.0f, 1.0f, -1.0f,  // top-left
			// front face
			-1.0f, -1.0f, 1.0f, // bottom-left
			1.0f, -1.0f, 1.0f,  // bottom-right
			1.0f, 1.0f, 1.0f,   // top-right
			1.0f, 1.0f, 1.0f,   // top-right
			-1.0f, 1.0f, 1.0f,  // top-left
			-1.0f, -1.0f, 1.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f,   // top-right
			-1.0f, 1.0f, -1.0f,  // top-left
			-1.0f, -1.0f, -1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f,  // bottom-right
			-1.0f, 1.0f, 1.0f,   // top-right
								 // right face
			1.0f, 1.0f, 1.0f,    // top-left
			1.0f, -1.0f, -1.0f,  // bottom-right
			1.0f, 1.0f, -1.0f,   // top-right
			1.0f, -1.0f, -1.0f,  // bottom-right
			1.0f, 1.0f, 1.0f,    // top-left
			1.0f, -1.0f, 1.0f,   // bottom-left
			// bottom face
			-1.0f, -1.0f, -1.0f, // top-right
			1.0f, -1.0f, -1.0f,  // top-left
			1.0f, -1.0f, 1.0f,   // bottom-left
			1.0f, -1.0f, 1.0f,   // bottom-left
			-1.0f, -1.0f, 1.0f,  // bottom-right
			-1.0f, -1.0f, -1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, // top-left
			1.0f, 1.0f, 1.0f,   // bottom-right
			1.0f, 1.0f, -1.0f,  // top-right
			1.0f, 1.0f, 1.0f,   // bottom-right
			-1.0f, 1.0f, -1.0f, // top-left
			-1.0f, 1.0f, 1.0f   // bottom-left
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			(void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renders a 1x1 quad in NDC with manually calculated tangent vectors
// ------------------------------------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
	if (quadVAO == 0) {
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent //
			// bitangent
			pos1.x,       pos1.y,       pos1.z,       nm.x,         nm.y,
			nm.z,         uv1.x,        uv1.y,        tangent1.x,   tangent1.y,
			tangent1.z,   bitangent1.x, bitangent1.y, bitangent1.z, pos2.x,
			pos2.y,       pos2.z,       nm.x,         nm.y,         nm.z,
			uv2.x,        uv2.y,        tangent1.x,   tangent1.y,   tangent1.z,
			bitangent1.x, bitangent1.y, bitangent1.z, pos3.x,       pos3.y,
			pos3.z,       nm.x,         nm.y,         nm.z,         uv3.x,
			uv3.y,        tangent1.x,   tangent1.y,   tangent1.z,   bitangent1.x,
			bitangent1.y, bitangent1.z,

			pos1.x,       pos1.y,       pos1.z,       nm.x,         nm.y,
			nm.z,         uv1.x,        uv1.y,        tangent2.x,   tangent2.y,
			tangent2.z,   bitangent2.x, bitangent2.y, bitangent2.z, pos3.x,
			pos3.y,       pos3.z,       nm.x,         nm.y,         nm.z,
			uv3.x,        uv3.y,        tangent2.x,   tangent2.y,   tangent2.z,
			bitangent2.x, bitangent2.y, bitangent2.z, pos4.x,       pos4.y,
			pos4.z,       nm.x,         nm.y,         nm.z,         uv4.x,
			uv4.y,        tangent2.x,   tangent2.y,   tangent2.z,   bitangent2.x,
			bitangent2.y, bitangent2.z };
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
			(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
			(void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
			(void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
			(void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float),
			(void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(
			GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			format == GL_RGBA
			? GL_CLAMP_TO_EDGE
			: GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent
						  // semi-transparent borders. Due to interpolation it
						  // takes texels from next repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// utility function for loading a cubemap from face textures
// ---------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data =
			stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
				0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap tex failed to load at path: " << faces[i]
				<< std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// utility function for loading volume data
// ----------------------------------------
unsigned int loadVolumeData(const char* path) {
	FILE* fp;
	errno_t e = fopen_s(&fp, path, "rb");
	
	if (e != 0) {
		printf_s("Error %d: Couldn't open file %s", e, path);
		return 0;
	}

	unsigned short vuSize[3];
	fread((void*)vuSize, 3, sizeof(unsigned short), fp);

	int uCount = int(vuSize[0]) * int(vuSize[1]) * int(vuSize[2]);
	unsigned short* pData = new unsigned short[uCount];
	fread((void*)pData, uCount, sizeof(unsigned short), fp);

	fclose(fp);

	float* pDataFloat = new float[uCount];
	for (int i = 0; i < uCount; i++) {
		pDataFloat[i] = pData[i] / 40;
	}
	delete[] pData;

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, vuSize[0], vuSize[1], vuSize[2], 0,
		GL_RED, GL_FLOAT, pDataFloat);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	delete[] pDataFloat;

	return textureID;
}