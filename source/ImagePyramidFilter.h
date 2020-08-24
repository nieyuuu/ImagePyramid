#pragma once
#include <G3D/G3D.h>
#include <G3D-base/Array.h>

class CImagePyramidFilter :public ReferenceCountedObject
{
private:
	Array<shared_ptr<Texture>> m_DownSampleChain;
	Array<shared_ptr<Texture>> m_UpSampleChain;

	bool __needRecreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration, const ImageFormat* vFormat) const;
	void __recreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, const ImageFormat* vFormat, int vDownSampleIteration);
	float __mipGaussianBlendWeight(float vSigma, int vLevel);

public:
	static shared_ptr<CImagePyramidFilter> create()
	{
		return createShared<CImagePyramidFilter>();
	}

	shared_ptr<Texture> Apply(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, int vDownSampleIteration, float vDownSampleUVOffset, float vSigma);

protected:
	CImagePyramidFilter() {}
};
