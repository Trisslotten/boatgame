#include "texture.hpp"
#include <GL/glew.h>
#include <iostream>

Texture::Texture()
{
	hints[GL_TEXTURE_MIN_FILTER] = GL_LINEAR_MIPMAP_LINEAR;
	hints[GL_TEXTURE_MAG_FILTER] = GL_NEAREST;
	hints[GL_TEXTURE_WRAP_S] = GL_REPEAT;
	hints[GL_TEXTURE_WRAP_T] = GL_REPEAT;
}

void Texture::hint(GLenum pname, GLint param)
{
	hints[pname] = param;
}


bool Texture::loadTexture(const std::string& file)
{
	std::vector<unsigned char> image;
	unsigned error = lodepng::decode(image, width, height, file);
	if (error != 0)
	{
		std::cout << "ERROR: Could not load texture: " << file << "\n";
		return false;
	}

	int channels = image.size() / (width * height);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	for (auto& elem : hints)
	{
		glTexParameteri(GL_TEXTURE_2D, elem.first, elem.second);
	}

	GLint format;
	switch (channels)
	{
	case 1:
		format = GL_R;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		std::cout << "ERROR: " << channels << " channels not supported\n";
		return false;
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, &image[0]);

	glGenerateMipmap(GL_TEXTURE_2D);

	image.clear();
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void Texture::bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

unsigned Texture::getID()
{
	return id;
}
