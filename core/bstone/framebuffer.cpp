#pragma once
#include"framebuffer.h"
#include"../ew/external/glad.h"
//heavy credit to LearnOpenGL FrameBuffer Tutorial

namespace ben
{
	
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat)
	{
		
		unsigned int fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		
		unsigned int tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, colorFormat, GL_UNSIGNED_BYTE,nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		unsigned int texcolbuffer;
		glGenTextures(1, &texcolbuffer);
		
		
		return Framebuffer();
	}
}
