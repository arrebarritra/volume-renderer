#pragma once

#define PRIMITIVE_RESTART UINT_MAX
struct SamplePolygonVertex {
	glm::vec3 pos;
	glm::vec3 texcoord;

	SamplePolygonVertex(glm::vec3 pos, glm::vec3 texcoord) : pos(pos), texcoord(texcoord) {};
};

const glm::vec4 BBoxCornerVertices[8] = {
	glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
	glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
	glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
	glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f)
};

const std::pair<int, int> edges[12] = {
	{0,1}, {1,2}, {2,3}, {3, 0},	// bottom plane
	{4,5}, {5,6}, {6,7}, {7, 4},	// top plane
	{0,4}, {1,5}, {2,6}, {3, 7},	// vertical edges
};
class Volume {
public:
	Volume(const char* path, int samples = 50);
	void Render(glm::mat4& modelview);

private:
	unsigned int VAO, VBO;
	unsigned int textureID;
	unsigned short dims[3];
	int samples;

	void loadVolumeData(const char* path);
	void getViewAlignedBBoxCorners(glm::mat4& modelview, glm::vec3 (&viewAlignedBBoxCorners)[8]);
	void getZRange(glm::vec3 (&viewAlignedBBoxCorners)[8], glm::vec2& zRange);
	void generatePolygon(const glm::mat4& modelviewinv, const glm::vec3 (&viewAlignedBBoxCorners)[8], const float z, std::vector<SamplePolygonVertex>& vertices);
	void getPlaneCubeIntersections(const glm::mat4& modelviewinv, const glm::vec3 (&viewAlignedBBoxCorners)[8], const float z, std::vector<SamplePolygonVertex>& vertices);
	void getPlaneEdgeIntersection(const glm::mat4& modelviewinv, const glm::vec3 (&viewAlignedBBoxCorners)[8], const float z, const std::pair<int, int>& edge, std::vector<SamplePolygonVertex>& vertices);
};