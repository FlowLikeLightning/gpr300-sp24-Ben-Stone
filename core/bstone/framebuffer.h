namespace ben
{
	struct Framebuffer
	{
		unsigned int fbo;
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat);
	Framebuffer createGBuffer(unsigned int screenWidth, unsigned int screenHeight);
}