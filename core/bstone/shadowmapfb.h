#pragma once

#include"framebuffer.h"
namespace ben
{
	Framebuffer createShadowFramebuffer(unsigned int width, unsigned int height, int depthFormat);
}