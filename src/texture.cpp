#include "texture.hpp"

Texture::Texture(const char *image, GLenum texType, GLenum slot, GLenum pixelType)
{
	// Assigns the type of the texture ot the texture object
	type = texType;
	// Assigns the unit slot of the texture ot the texture object
	unit = slot;

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char *bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
	if (bytes)
	{
		if (numColCh == 1)
			format = GL_RED;
		else if (numColCh == 3)
			format = GL_RGB;
		else if (numColCh == 4)
			format = GL_RGBA;

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(texType, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
		// Generates MipMaps
		glGenerateMipmap(texType);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);
}

void Texture::texUnit(Shader &shader, const char *uniform, GLuint unit)
{
	// Shader needs to be activated before changing the value of a uniform
	shader.use();
	// Sets the value of the uniform
	shader.setInt(uniform, unit);
}

void Texture::bind()
{
	glActiveTexture(unit);
	glBindTexture(type, ID);
}

void Texture::unbind()
{
	glBindTexture(type, 0);
}

void Texture::remove()
{
	glDeleteTextures(1, &ID);
}