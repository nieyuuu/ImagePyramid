#pragma once
#include <G3D/G3D.h>

class CGaussianFilter :public ReferenceCountedObject
{
private:
	shared_ptr<Framebuffer> m_pIntermediateFramebuffer;
	shared_ptr<Framebuffer> m_pResultFramebuffer;

	void __createOrResizeFramebuffers(int vInputTextureWidth, int vInputTextureHeight, const ImageFormat* vFormat);

public:
	static shared_ptr<CGaussianFilter> create()
	{
		return createShared<CGaussianFilter>();
	}

	shared_ptr<Texture> Apply(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, uint vFilterWidth);

protected:
	CGaussianFilter() {}
};