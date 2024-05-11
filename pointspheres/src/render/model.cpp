#include "model.h"

#include <cstring>

#include <stb/stb_image.h>

void Model::loadModel(const std::string& modelName)
{
	// set the extents for the model
	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> objMaterials;
	
    std::string errStr;

	bool rc =
		tinyobj::LoadObj(shapes, objMaterials, errStr, (modelDir + modelName + ".obj").c_str(), (modelDir).c_str());

	if (!rc)
	{
		std::cerr << errStr << std::endl;
	}
	else
	{
		if (shapes.size())
		{
			for (size_t i = 0; i < shapes.size(); i++) {
				Mesh sub_mesh;

				sub_mesh.vertices = shapes[i].mesh.positions;
				sub_mesh.indices = shapes[i].mesh.indices;
				sub_mesh.normals = shapes[i].mesh.normals;
				sub_mesh.texturePos = shapes[i].mesh.texcoords;

				meshes.push_back(sub_mesh);
			}
		}
		if (objMaterials.size())
		{
			for (size_t i = 0; i < objMaterials.size(); i++)
			{
				tinyobj::material_t mat = objMaterials[i];

				if (std::strcmp(mat.diffuse_texname.c_str(), "") != 0)
				{
					Texture diff;
					diff.type = "diff";
					diff.path = textureDir + modelName + "/" + objMaterials[i].diffuse_texname;
					diff.ID = loadTexture(diff.path);

					textures_loaded.push_back(diff);
				}
				if (std::strcmp(mat.specular_texname.c_str(), "") != 0)
				{
					Texture spec;
					spec.type = "spec";
					spec.path = textureDir + modelName + "/" + objMaterials[i].specular_texname;
					spec.ID = loadTexture(spec.path);

					textures_loaded.push_back(spec);
				}
				if (std::strcmp(mat.bump_texname.c_str(), "") != 0)
				{
					Texture normal;

					normal.type = "normal";
					normal.path = textureDir + modelName + "/" + objMaterials[i].bump_texname;
					normal.ID = loadTexture(normal.path);

					textures_loaded.push_back(normal);
				}
			}
		}
		/**
		posBuf = shapes[0].mesh.positions;
		norBuf = shapes[0].mesh.normals;
		texBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;
		*/
	}
}

void Model::measure()
{
	for (auto& mesh : meshes) {
		mesh.measure();
		this->minX = std::min(this->minX, mesh.minX);
		this->minY = std::min(this->minY, mesh.minY);
		this->minZ = std::min(this->minZ, mesh.minZ);

		this->maxX = std::max(this->maxX, mesh.maxX);
		this->maxY = std::max(this->maxY, mesh.maxY);
		this->maxZ = std::max(this->maxZ, mesh.maxZ);
	}
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
}

unsigned int Model::loadTexture(const std::string& path)
{
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format = GL_NONE;
		if (nrComponents == 1)
		{
			format = GL_RED;
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture at " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


void Model::init()
{
	for (auto& mesh : meshes) {
		mesh.init();
	}
}

void Model::draw(const std::shared_ptr<Shader> shader)
{
	for (auto& mesh : meshes) {
		mesh.Draw(shader, textures_loaded);
	}
}
