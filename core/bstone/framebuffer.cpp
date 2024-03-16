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
	Framebuffer ben::createGBuffer(unsigned int width, unsigned int height)
	{
		Framebuffer framebuffer;
		framebuffer.width = width;
		framebuffer.height = height;

		glCreateFramebuffers(1, &framebuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

		int formats[3] = {
			GL_RGB32F, //0 = World Position 
			GL_RGB16F, //1 = World Normal
			GL_RGB16F  //2 = Albedo
		};
		//Create 3 color textures
		for (size_t i = 0; i < 3; i++)
		{
			glGenTextures(1, &framebuffer.colorBuffer[i]);
			glBindTexture(GL_TEXTURE_2D, framebuffer.colorBuffer[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, formats[i], width, height);
			//Clamp to border so we don't wrap when sampling for post processing
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//Attach each texture to a different slot.
		//GL_COLOR_ATTACHMENT0 + 1 = GL_COLOR_ATTACHMENT1, etc
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, framebuffer.colorBuffer[i], 0);
		}
		//Explicitly tell OpenGL which color attachments we will draw to
		const GLenum drawBuffers[3] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
		};
		
		glDrawBuffers(3, drawBuffers);
		
		unsigned int depthTexture;
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
		
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			printf("Framebuffer incomplete: %d", fboStatus);
		}
		//Clean up global state
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return framebuffer;
	}
}
