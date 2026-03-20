#include "render/texture.hpp"

#include <stb_image/stb_image.h>

#include "utils/log.hpp"

Texture::Texture(const std::string &path, ::GLenum pixelType)
	: handle_{0z, [](::GLuint h)
			  { ::glDeleteTextures(1, &h); }},
	  path_{path}, format_{GL_RGB}
{
	::glGenTextures(1, &handle_);

	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *bytes = stbi_load(path.c_str(), &widthImg, &heightImg, &numColCh, 0);
	if (bytes)
	{
		if (numColCh == 1)
			format_ = GL_RED;
		else if (numColCh == 3)
			format_ = GL_RGB;
		else if (numColCh == 4)
			format_ = GL_RGBA;

		// Assigns the texture to a Texture Unit
		::glBindTexture(GL_TEXTURE_2D, handle_);
		// Assigns the image to the OpenGL Texture object
		::glTexImage2D(GL_TEXTURE_2D, 0, format_, widthImg, heightImg, 0, format_, pixelType, bytes);
		// Generates MipMaps
		::glGenerateMipmap(GL_TEXTURE_2D);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Configures the way the texture repeats (if it does at all)
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		Log::error("Failed to load texture: {}", path);
	}

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	::glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(::GLuint unit) const
{
	::glActiveTexture(GL_TEXTURE0 + unit);
	::glBindTexture(GL_TEXTURE_2D, handle_);
}

void Texture::unbind() const
{
	::glBindTexture(GL_TEXTURE_2D, 0);
}