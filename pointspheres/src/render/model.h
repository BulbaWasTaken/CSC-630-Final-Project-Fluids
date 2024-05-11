#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <utility>
#include <cassert>

#include <tiny_obj_loader/tiny_obj_loader.h>

#include "shader.h"
#include "mesh.h"

class Model
{

public:
	void loadModel(const std::string& modelName);
	void init();
	void measure();
	unsigned int loadTexture(const std::string& path);
	// used to iterate through scene graph
	void draw(const std::shared_ptr<Shader> shader);

	float xExtent, yExtent, zExtent;

	float scaleX, scaleY, scaleZ;
	float shiftX, shiftY, shiftZ;
	float epsilon = 0.001f;
private:
	std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;

	std::string modelDir = "./res/objects/";
	std::string textureDir = "./res/textures/";

	// extent of entire model set by meshes

	float minX, minY, minZ;
	float maxX, maxY, maxZ;
};

#endif