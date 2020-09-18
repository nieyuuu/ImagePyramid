#include "Renderer.h"
#include "DualFilter.h"
#include "ImagePyramidFilter.h"
#include "GaussianFilter.h"

void CRenderer::render(RenderDevice* vRenderDevice, const shared_ptr<Camera>& vCamera, const shared_ptr<Framebuffer>& vFramebuffer, const shared_ptr<Framebuffer>& vDepthPeelFramebuffer, LightingEnvironment& vLightingEnvironment, const shared_ptr<GBuffer>& vGBuffer, const Array<shared_ptr<Surface>>& vAllSurfaces)
{
	DefaultRenderer::render(vRenderDevice, vCamera, vFramebuffer, vDepthPeelFramebuffer, vLightingEnvironment, vGBuffer, vAllSurfaces);

	switch (m_CurrentMode)
	{
	case EMode::None: break;
	case EMode::Dual_Filter: {
		Texture::copy(m_pDualFilter->Apply(vRenderDevice, vFramebuffer->texture(0), m_DualFilterSettings.DownSampleIterations, m_DualFilterSettings.DownSampleUVOffset, m_DualFilterSettings.UpSampleUVOffset), vFramebuffer->texture(0));
	} break;
	case EMode::Image_Pyramid_Filter: {
		Texture::copy(m_pImagePyramidFilter->Apply(vRenderDevice, vFramebuffer->texture(0), m_ImagePyramidFilterSettings.DownSampleIterations, m_ImagePyramidFilterSettings.DownSampleUVOffset, m_ImagePyramidFilterSettings.Sigma), vFramebuffer->texture(0));
	} break;
	case EMode::Gaussian_Filter: {
		Texture::copy(m_pGaussianFilter->Apply(vRenderDevice, vFramebuffer->texture(0), m_GaussianFilterSettings.FilterWidth), vFramebuffer->texture(0));
	} break;
	case EMode::All: {
		shared_ptr<Texture> pDualFilterResult, pImagePyramidResult, pGaussianFilterResult;

		pDualFilterResult = m_pDualFilter->Apply(vRenderDevice, vFramebuffer->texture(0), m_DualFilterSettings.DownSampleIterations, m_DualFilterSettings.DownSampleUVOffset, m_DualFilterSettings.UpSampleUVOffset);
		pImagePyramidResult = m_pImagePyramidFilter->Apply(vRenderDevice, vFramebuffer->texture(0), m_ImagePyramidFilterSettings.DownSampleIterations, m_ImagePyramidFilterSettings.DownSampleUVOffset, m_ImagePyramidFilterSettings.Sigma);
		pGaussianFilterResult = m_pGaussianFilter->Apply(vRenderDevice, vFramebuffer->texture(0), m_GaussianFilterSettings.FilterWidth);

		shared_ptr<Framebuffer> pMergeFramebuffer = Framebuffer::create("MergeFramebuffer");
		pMergeFramebuffer->set(Framebuffer::COLOR0, Texture::createEmpty("MergeResult", vFramebuffer->width(), vFramebuffer->height(), vFramebuffer->texture(0)->format()));

		Point2int16 TargetSize = Point2int16(pMergeFramebuffer->width(), pMergeFramebuffer->height());

		vRenderDevice->push2D(pMergeFramebuffer); {
			vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));
			vRenderDevice->clear(true, false, false);

			Args args;
			args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize)));
			args.setUniform("DualFilterResult", pDualFilterResult, Sampler::buffer());
			args.setUniform("ImagePyramidFilterResult", pImagePyramidResult, Sampler::buffer());
			args.setUniform("GaussianFilterResult", pGaussianFilterResult, Sampler::buffer());
			args.setUniform("SceneColor", vFramebuffer->texture(0), Sampler::buffer());
			args.setUniform("TargetSize", TargetSize);

			LAUNCH_SHADER("shaders/Merge.pix", args);
		} vRenderDevice->pop2D();

		pMergeFramebuffer->blitTo(vRenderDevice, vFramebuffer, false, false, false, false, true);
	} break;
	default: break;
	}
}

void CRenderer::makeGUI(GuiWindow* vGUIWindow)
{
	vGUIWindow->pane()->addPane("Current Mode", GuiTheme::PaneStyle::ORNATE_PANE_STYLE)->addEnumClassRadioButtons<EMode>("", &m_CurrentMode);

	m_DualFilterSettings.makeGUI(vGUIWindow->pane()->addPane("Dual Filter", GuiTheme::PaneStyle::ORNATE_PANE_STYLE));
	m_ImagePyramidFilterSettings.makeGUI(vGUIWindow->pane()->addPane("Image Pyramid Filter", GuiTheme::PaneStyle::ORNATE_PANE_STYLE));
	m_GaussianFilterSettings.makeGUI(vGUIWindow->pane()->addPane("Gaussian Filter", GuiTheme::PaneStyle::ORNATE_PANE_STYLE));

	vGUIWindow->pane()->pack();
}

CRenderer::CRenderer()
{
	m_pDualFilter = CDualFilter::create();
	m_pImagePyramidFilter = CImagePyramidFilter::create();
	m_pGaussianFilter = CGaussianFilter::create();
}
