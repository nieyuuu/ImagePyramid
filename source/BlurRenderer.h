#pragma once
#include <G3D/G3D.h>

class CBlurRenderer :public DefaultRenderer
{
private:

public:
	static shared_ptr<CBlurRenderer> create()
	{
		return createShared<CBlurRenderer>();
	}

protected:
	CBlurRenderer() {}

	virtual void render(
		RenderDevice*					  vRenderDevice,
		const shared_ptr<Camera>&		  vCamera,
		const shared_ptr<Framebuffer>&	  vFramebuffer,
		const shared_ptr<Framebuffer>&	  vDepthPeelFramebuffer,
		LightingEnvironment&			  vLightingEnvironment,
		const shared_ptr<GBuffer>&		  vGBuffer,
		const Array<shared_ptr<Surface>>& vAllSurfaces) override;
};