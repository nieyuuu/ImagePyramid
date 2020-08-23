#pragma once
#include <G3D/G3D.h>
#include <G3D-base/Array.h>

class CImagePyramidFilter :public ReferenceCountedObject
{
private:
	shared_ptr<Framebuffer> m_pDownSampleFramebuffer;
	shared_ptr<Framebuffer> m_pUpSampleFramebuffer;

	//For intermediate results
	Array<shared_ptr<Texture>> m_DownSampleChain;
	Array<shared_ptr<Texture>> m_UpSampleChain;

	bool __needRecreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration) const;
	void __recreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, const ImageFormat* vFormat, int vDownSampleIteration);
	float __MipGaussianBlendWeight(float vSigma, int vLevel);

public:
	static shared_ptr<CImagePyramidFilter> create()
	{
		return createShared<CImagePyramidFilter>();
	}

	shared_ptr<Texture> Apply(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, int vDownSampleIteration, float vSigma);

protected:
	CImagePyramidFilter()
	{
		m_pDownSampleFramebuffer = Framebuffer::create("ImagePyramidFilter::DownSampleFramebuffer");
		m_pUpSampleFramebuffer = Framebuffer::create("ImagePyramidFilter::UpSampleFramebuffer");
	}
};
