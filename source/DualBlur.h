#pragma once
#include <G3D/G3D.h>
#include <G3D-base/Array.h>

class CDualBlur :public ReferenceCountedObject
{
private:
	shared_ptr<Framebuffer> m_pDownSampleFramebuffer;
	shared_ptr<Framebuffer> m_pUpSampleFramebuffer;

	Array<shared_ptr<Texture>> m_DownSampleChain;
	Array<shared_ptr<Texture>> m_UpSampleChain;

	bool __needRecreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration) const;
	void __recreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration);

public:
	static shared_ptr<CDualBlur> create()
	{
		return createShared<CDualBlur>();
	}

	shared_ptr<Texture> Blur(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, int vDownSampleIteration, float vUVOffset);

protected:
	CDualBlur()
	{
		m_pDownSampleFramebuffer = Framebuffer::create("DualBlur::DownSampleFramebuffer");
		m_pUpSampleFramebuffer = Framebuffer::create("DualBlur::UpSampleFramebuffer");
	}
};