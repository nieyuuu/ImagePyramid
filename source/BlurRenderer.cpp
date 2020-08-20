#include "BlurRenderer.h"

void CBlurRenderer::render(RenderDevice* vRenderDevice, const shared_ptr<Camera>& vCamera, const shared_ptr<Framebuffer>& vFramebuffer, const shared_ptr<Framebuffer>& vDepthPeelFramebuffer, LightingEnvironment& vLightingEnvironment, const shared_ptr<GBuffer>& vGBuffer, const Array<shared_ptr<Surface>>& vAllSurfaces)
{
	DefaultRenderer::render(vRenderDevice, vCamera, vFramebuffer, vDepthPeelFramebuffer, vLightingEnvironment, vGBuffer, vAllSurfaces);
}
