#pragma once
#include <G3D/G3D.h>

class CDualFilter;
class CImagePyramidFilter;
class CGaussianFilter;

struct SDualFilterSettings
{
	int DownSampleIterations = 4;
	float DownSampleUVOffset = 2.0f;
	float UpSampleUVOffset = 2.0f;

	void makeGUI(GuiPane* vPane)
	{
		vPane->addNumberBox("Down Sample Iterations", &DownSampleIterations, "", GuiTheme::LINEAR_SLIDER, 1, 15, 1)->setCaptionWidth(140);
		vPane->addNumberBox("Down Sample UVOffset", &DownSampleUVOffset, "", GuiTheme::LINEAR_SLIDER, 0.0f, 25.0f, 0.01f)->setCaptionWidth(140);
		vPane->addNumberBox("Up Sample UVOffset", &UpSampleUVOffset, "", GuiTheme::LINEAR_SLIDER, 0.0f, 25.0f, 0.01f)->setCaptionWidth(140);
	}
};

struct SImagePyramidFilterSettings
{
	int DownSampleIterations = 5;
	float DownSampleUVOffset = 0.0f;
	float Sigma = 17.8f;

	void makeGUI(GuiPane* vPane)
	{
		vPane->addNumberBox("Down Sample Iterations", &DownSampleIterations, "", GuiTheme::LINEAR_SLIDER, 1, 15, 1)->setCaptionWidth(140);
		vPane->addNumberBox("Down Sample UVOffset", &DownSampleUVOffset, "", GuiTheme::LINEAR_SLIDER, 0.0f, 25.0f, 0.01f)->setCaptionWidth(140);
		vPane->addNumberBox("Sigma", &Sigma, "", GuiTheme::LINEAR_SLIDER, 0.0f, 25.0f, 0.01f)->setCaptionWidth(140);
	}
};

struct SGaussianFilterSettings
{
	int FilterWidth = 211;

	void makeGUI(GuiPane* vPane)
	{
		vPane->addNumberBox("Filter Width", &FilterWidth, "", GuiTheme::LINEAR_SLIDER, 1, 300, 1)->setCaptionWidth(140);
	}
};

G3D_BEGIN_ENUM_CLASS_DECLARATION(EMode, None, Dual_Filter, Image_Pyramid_Filter, Gaussian_Filter, All);
G3D_END_ENUM_CLASS_DECLARATION();

class CRenderer :public DefaultRenderer
{
private:
	shared_ptr<CDualFilter> m_pDualFilter;
	shared_ptr<CImagePyramidFilter> m_pImagePyramidFilter;
	shared_ptr<CGaussianFilter> m_pGaussianFilter;

	SDualFilterSettings m_DualFilterSettings;
	SImagePyramidFilterSettings m_ImagePyramidFilterSettings;
	SGaussianFilterSettings m_GaussianFilterSettings;

	EMode m_CurrentMode = EMode::None;

public:
	static shared_ptr<CRenderer> create()
	{
		return createShared<CRenderer>();
	}

	virtual void render
		(RenderDevice*					    vRenderDevice,
		const shared_ptr<Camera>&		    vCamera,
		const shared_ptr<Framebuffer>&	    vFramebuffer,
		const shared_ptr<Framebuffer>&	    vDepthPeelFramebuffer,
		LightingEnvironment&			    vLightingEnvironment,
		const shared_ptr<GBuffer>&			vGBuffer,
		const Array<shared_ptr<Surface>>&   vAllSurfaces) override;

	void makeGUI(GuiWindow* vGUIWindow);

protected:
	CRenderer();
};