#pragma once
#include"framebuffer.h"
#include"../ew/external/glad.h"
#include<iostream>
//heavy credit to LearnOpenGL FrameBuffer Tutorial

namespace ben
{
	
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat)
	{
		
		unsigned int fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		
		unsigned int colTexture;
		glGenTextures(1, &colTexture);
		glBindTexture(GL_TEXTURE_2D, colTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, colorFormat, width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colTexture, 0);

		
		unsigned int depthTexture;
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
		
		Framebuffer fbtoRet;
		fbtoRet.fbo = fbo;
		fbtoRet.colorBuffer[0] = colTexture;
		fbtoRet.depthBuffer = depthTexture;
		fbtoRet.width = width;
		fbtoRet.height = height;
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			printf("Framebuffer incomplete: %d", fboStatus);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);


		return fbtoRet;
	}
}
