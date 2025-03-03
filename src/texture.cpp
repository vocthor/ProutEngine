#include "texture.hpp"

Texture::Texture(const std::string image, const std::string texType, GLenum pixelType)
{
	path = image;
	type = texType;

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char *bytes = stbi_load(image.c_str(), &widthImg, &heightImg, &numColCh, 0);
	if (bytes)
	{
		if (numColCh == 1)
			format = GL_RED;
		else if (numColCh == 3)
			format = GL_RGB;
		else if (numColCh == 4)
			format = GL_RGBA;

		// Assigns the texture to a Texture Unit
		glBindTexture(GL_TEXTURE_2D, ID);
		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
		// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
	}
	else
	{
		std::cout << "Failed to load texture : " << image << std::endl;
	}

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader &shader, const char *uniform, GLuint slot)
{
	unit = slot;
	// Shader needs to be activated before changing the value of a uniform
	shader.use();
	// Sets the value of the uniform
	shader.setInt(uniform, unit);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::remove()
{
	glDeleteTextures(1, &ID);
}