#ifndef MESH_H
#define MESH_H

#include <vector>
#include <memory>
#include <cassert>

#include "shader.h"

struct Texture {
	unsigned int ID;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	void init();
	void Draw(const std::shared_ptr<Shader> prog, std::vector<Texture>& texture);
	void measure();

	std::vector<float> meshData;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<float> normals;
	std::vector<float> texturePos;

	bool textured = true;

	unsigned int posBufID = 0;
	unsigned int eleBufID = 0;
	unsigned int norBufID = 0;
	unsigned int texBufID = 0;
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	// extent of the mesh

	float minX, minY, minZ;
	float maxX, maxY, maxZ;
};

#endif