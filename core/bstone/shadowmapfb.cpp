#pragma once
#include"shadowmapfb.h"
#include"../ew/external/glad.h"
#include<iostream>
namespace ben
{

	Framebuffer createShadowFramebuffer(unsigned int width, unsigned int height, int depthFormat)
	{

		unsigned int shadowfbo;
		glGenFramebuffers(1, &shadowfbo);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowfbo);
		

		unsigned int depthTexture;
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, depthFormat, width, height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//Pixels outside of frustum should have max distance (white)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		Framebuffer fbtoRet;
		fbtoRet.fbo = shadowfbo;
		fbtoRet.depthBuffer = depthTexture;
		fbtoRet.width = width;
		fbtoRet.height = height;
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
			printf("Framebuffer incomplete: %d", fboStatus);
		}
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		//unbinding
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		return fbtoRet;
	}
}