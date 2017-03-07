#pragma once
#ifndef __TextureCube__
#define __TextureCube__

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>

class TextureCube
{
public:
	TextureCube();
	virtual ~TextureCube();
	void setFilenames(const std::string &negx,
					  const std::string &posx,
					  const std::string &negy,
					  const std::string &posy,
					  const std::string &negz,
					  const std::string &posz);
	void init();
	void setUnit(GLint u) { unit = u; }
	GLint getUnit() const { return unit; }
	void bind(GLint handle);
	void unbind();
	
private:
	std::string filenames[6];
	int width;
	int height;
	GLuint tid;
	GLint unit;
	
};

#endif
