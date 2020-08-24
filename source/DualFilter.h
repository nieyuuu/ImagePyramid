#pragma once
#include <G3D/G3D.h>
#include <G3D-base/Array.h>

class CDualFilter :public ReferenceCountedObject
{
private:
	Array<shared_ptr<Texture>> m_DownSampleChain;
	Array<shared_ptr<Texture>> m_UpSampleChain;

	bool __needRecreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration, const ImageFormat* vFormat) const;
	void __recreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, const ImageFormat* vFormat, int vDownSampleIteration);

public:
	static shared_ptr<CDualFilter> create()
	{
		return createShared<CDualFilter>();
	}

	shared_ptr<Texture> Apply(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, int vDownSampleIteration, float vDownSampleUVOffset, float vUpSampleUVOffset);

protected:
	CDualFilter() {}
};