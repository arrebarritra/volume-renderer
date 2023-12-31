#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/compatibility.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#include "volume.h"

Volume::Volume(const char* path, int samples) : samples(samples)
{
	loadVolumeData(path);

	transformCoords = glm::mat4(1.0f);
	// rotate model so axes correspond to OpenGL axes
	transformCoords = glm::rotate(transformCoords, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
	// stretch bounding cube to match volume aspect ratio
	transformCoords = glm::scale(transformCoords, glm::vec3(dims[0] / (float)dims[2], dims[1] / (float)dims[2], 1.0f));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SamplePolygonVertex), (void*)offsetof(SamplePolygonVertex, SamplePolygonVertex::pos));
	// texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SamplePolygonVertex), (void*)offsetof(SamplePolygonVertex, SamplePolygonVertex::texcoord));

	shader.use();
	shader.setInt("volume", 0);
}

void Volume::Render(const glm::mat4& projection, const glm::mat4& modelview)
{
	glm::vec3 viewAlignedBBoxCorners[8];
	glm::vec2 zRange;

	getViewAlignedBBoxCorners(modelview *  transformCoords, viewAlignedBBoxCorners);
	getZRange(viewAlignedBBoxCorners, zRange);

	// vertices + indices of all sample polygons
	glm::mat4 modelviewinv = glm::inverse(modelview * transformCoords);
	std::vector<SamplePolygonVertex> vertices; vertices.reserve(6 * samples);
	std::vector<int> start; start.reserve(samples);
	std::vector<int> count; count.reserve(samples);

	// generate sample polygons and prepare for multi draw arrays
	unsigned int startIndex;
	for (int i = 0; i < samples; i++) {
		startIndex = vertices.size();
		float z = zRange.x + (zRange.y - zRange.x) * (i / (float)samples);
		generatePolygon(modelviewinv, viewAlignedBBoxCorners, z, vertices);
		int verticesAdded = vertices.size() - startIndex;
		if (verticesAdded > 0) {
			start.push_back(startIndex);
			count.push_back(verticesAdded);
		}
	}

	// render sample polygons
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, textureID);

	shader.use();
	shader.setMat4("projection", projection);
	shader.setFloat("alphascale", alphaScale);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SamplePolygonVertex) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);

	glMultiDrawArrays(GL_TRIANGLE_FAN, &start[0], &count[0], start.size());
}

void Volume::loadVolumeData(const char* path)
{
	FILE* fp;
	errno_t e = fopen_s(&fp, path, "rb");

	if (e != 0) {
		printf_s("Error %d: Couldn't open file %s", e, path);
		return;
	}

	fread((void*)dims, 3, sizeof(unsigned short), fp);

	int uCount = int(dims[0]) * int(dims[1]) * int(dims[2]);
	unsigned short* pData = new unsigned short[uCount];
	fread((void*)pData, uCount, sizeof(unsigned short), fp);

	fclose(fp);

	// iterate through data to process and obtain stats
	float* pDataFloat = new float[uCount];
	minVal = pData[0];
	maxVal = pData[0];
	mean = 0;
	meanNonZero = 0;
	int amountNonZero = 0;
	for (int i = 0; i < uCount; i++) {
		if (pData[i] < minVal) minVal = pData[i];
		else if (pData[i] > maxVal) maxVal = pData[i];

		mean += pData[i];
		if (pData[i] > 0) {
			amountNonZero++;
			meanNonZero += pData[i];
		}
		pDataFloat[i] = pData[i] / 4095.0f;
	}
	mean /= uCount;
	meanNonZero /= amountNonZero;
	delete[] pData;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, dims[0], dims[1], dims[2], 0,
		GL_RED, GL_FLOAT, pDataFloat);
	delete[] pDataFloat;

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void Volume::getViewAlignedBBoxCorners(const glm::mat4& modelview, glm::vec3(&viewAlignedBBoxCorners)[8])
{
	for (int i = 0; i < 8; i++)
		viewAlignedBBoxCorners[i] = glm::vec3(modelview * BBoxCornerVertices[i]);
}

void Volume::getZRange(const glm::vec3(&viewAlignedBBoxCorners)[8], glm::vec2& zRange)
{
	zRange.x = viewAlignedBBoxCorners[0].z;
	zRange.y = viewAlignedBBoxCorners[0].z;
	for (int i = 1; i < 8; i++) {
		float z = viewAlignedBBoxCorners[i].z;
		if (z < zRange.x) zRange.x = z;
		if (z > zRange.y) zRange.y = z;
	}
}

float pseudoangle(float y, float x) {
	float r = y / (glm::abs(x) + glm::abs(y));
	if (x < 0) return 2.0f - r;
	else return 4.0f + r;
}

void Volume::generatePolygon(const glm::mat4& modelviewinv, const glm::vec3(&viewAlignedBBoxCorners)[8], const float z, std::vector<SamplePolygonVertex>& vertices)
{
	std::vector<SamplePolygonVertex> intersections;	intersections.reserve(6);
	getPlaneCubeIntersections(modelviewinv, viewAlignedBBoxCorners, z, intersections);

	// center of vertices calculated as their mean
	glm::vec3 center = glm::vec3(0.0f);
	for (const auto& vertex : intersections) {
		center += vertex.pos;
	}
	center /= intersections.size();

	// sort CCW by comparing pseudoangle relative to center
	std::sort(intersections.begin(), intersections.end(),
		[center](const SamplePolygonVertex& a, const SamplePolygonVertex& b) {
			float angle1 = pseudoangle(a.pos.y - center.y, a.pos.x - center.x);
			float angle2 = pseudoangle(b.pos.y - center.y, b.pos.x - center.x);
			return angle1 < angle2;
		}
	);

	vertices.insert(vertices.end(), intersections.begin(), intersections.end());
}

void Volume::getPlaneCubeIntersections(const glm::mat4& modelviewinv, const glm::vec3(&viewAlignedBBoxCorners)[8], const float z, std::vector<SamplePolygonVertex>& vertices) {
	for (const auto& edge : edges) getPlaneEdgeIntersection(modelviewinv, viewAlignedBBoxCorners, z, edge, vertices);
}

void Volume::getPlaneEdgeIntersection(const glm::mat4& modelviewinv, const glm::vec3(&viewAlignedBBoxCorners)[8], const float z, const std::pair<int, int>& edge, std::vector<SamplePolygonVertex>& vertices) {
	glm::vec3 l = viewAlignedBBoxCorners[edge.second] - viewAlignedBBoxCorners[edge.first];
	float ldotn = l.z;
	if (ldotn == 0.0f) return;

	float t = (z - viewAlignedBBoxCorners[edge.first].z) / ldotn;
	if (t >= 0 && t <= 1) {
		glm::vec3 pos = viewAlignedBBoxCorners[edge.first] + l * t;
		glm::vec3 texcoord = (modelviewinv * glm::vec4(pos, 1.0f) + glm::vec4(1.0f)) / 2.0f;
		vertices.push_back(SamplePolygonVertex(pos, texcoord));
	}
}

