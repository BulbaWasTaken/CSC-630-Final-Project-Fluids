#include "mesh.h"

void Mesh::init()
{
	int buff_size = 8;

	if (texturePos.empty())
	{
		textured = false;
		buff_size = 6;
	}

	for (size_t i = 0; i < vertices.size() / 3; i++)
	{
		meshData.push_back(vertices[i * 3]);
		meshData.push_back(vertices[(i * 3) + 1]);
		meshData.push_back(vertices[(i * 3) + 2]);

		meshData.push_back(normals[i * 3]);
		meshData.push_back(normals[(i * 3) + 1]);
		meshData.push_back(normals[(i * 3) + 2]);

		if (textured)
		{
			meshData.push_back(texturePos[i * 2]);
			meshData.push_back(texturePos[(i * 2) + 1]);
		}
	}

	// Initialize the vertex array object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send the position array to the GPU
	glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, buff_size * sizeof(float), (void*)0);

	// Send the normal array to the GPU
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, buff_size * sizeof(float), (void*)(3*sizeof(float)));

	if (textured)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, buff_size * sizeof(float), (void*)(6*sizeof(float)));
	}

	// Unbind the arrays
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(glGetError() == GL_NO_ERROR);
}

void Mesh::Draw(const std::shared_ptr<Shader> shader, std::vector<Texture>& textures)
{
	if (textured)
	{
		shader->setBool("textured", true);
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (size_t i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;

			if (name == "diff")
				number = std::to_string(diffuseNr++);
			else if (name == "spec")
				number = std::to_string(specularNr++);
			else if (name == "normal")
				number = std::to_string(normalNr++);
			else if (name == "height")
				number = std::to_string(heightNr++);

			shader->setInt(name + number, i);
			glBindTexture(GL_TEXTURE_2D, textures[i].ID);
		}
	}
	else
	{
		shader->setBool("textured", false);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::measure()
{
	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	// Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < vertices.size() / 3; v++)
	{
		if (vertices[3 * v + 0] < minX) minX = vertices[3 * v + 0];
		if (vertices[3 * v + 0] > maxX) maxX = vertices[3 * v + 0];

		if (vertices[3 * v + 1] < minY) minY = vertices[3 * v + 1];
		if (vertices[3 * v + 1] > maxY) maxY = vertices[3 * v + 1];

		if (vertices[3 * v + 2] < minZ) minZ = vertices[3 * v + 2];
		if (vertices[3 * v + 2] > maxZ) maxZ = vertices[3 * v + 2];
	}
}