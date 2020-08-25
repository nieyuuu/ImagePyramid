#include "App.h"
#include "DualFilter.h"
#include "ImagePyramidFilter.h"
#include "GaussianFilter.h"

G3D_START_AT_MAIN();

int main(int argc, const char* argv[])
{
	initGLG3D(G3DSpecification());

	GApp::Settings settings(argc, argv);

	settings.window.caption = "ImagePyramid";

	settings.window.fullScreen = false;
	settings.window.width = 1024;
	settings.window.height = 1024;
	settings.window.resizable = !settings.window.fullScreen;
	settings.window.framed = !settings.window.fullScreen;
	settings.window.defaultIconFilename = "icon.png";
	settings.window.asynchronous = true;

	settings.hdrFramebuffer.depthGuardBandThickness = Vector2int16(0, 0);
	settings.hdrFramebuffer.colorGuardBandThickness = Vector2int16(0, 0);

	settings.renderer.deferredShading = true;
	settings.renderer.orderIndependentTransparency = true;

	settings.screenCapture.outputDirectory = FileSystem::currentDirectory();
	settings.screenCapture.includeAppRevision = false;
	settings.screenCapture.includeG3DRevision = false;
	settings.screenCapture.filenamePrefix = "_";

	return App(settings).run();
}

void App::__initFiltersIfNecessary()
{
	if (!m_pDualFilter) m_pDualFilter = CDualFilter::create();
	if (!m_pImagePyramidFilter) m_pImagePyramidFilter = CImagePyramidFilter::create();
	if (!m_pGaussianFilter) m_pGaussianFilter = CGaussianFilter::create();
}

App::App(const GApp::Settings& settings) : GApp(settings) {}

void App::onInit()
{
	GApp::onInit();

	setFrameDuration(1.0f / 240.0f);

	showRenderingStats = false;

	loadScene("Simple Cornell Box (Area Light)");

	_makeGUI();

	developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
	developerWindow->videoRecordDialog->setCaptureGui(false);
}

void App::onGraphics3D(RenderDevice* vRenderDevice, Array<shared_ptr<Surface>>& vAllSurfaces)
{
	FilmSettings& FilmSettings = activeCamera()->filmSettings();
	if (FilmSettings.temporalAntialiasingEnabled())
	{
		//Disable TAA
		FilmSettings.setTemporalAntialiasingEnabled(false);
	}

	GApp::onGraphics3D(vRenderDevice, vAllSurfaces);

	__initFiltersIfNecessary();

	shared_ptr<Texture> pDualFilterResult, pImagePyramidResult, pGaussianFilterResult;
	switch (m_CurrentMode)
	{
	case EMode::None:
		break;
	case EMode::Dual_Filter:
		{
			pDualFilterResult = m_pDualFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_DualFilterSettings.DownSampleIterations, m_DualFilterSettings.DownSampleUVOffset, m_DualFilterSettings.UpSampleUVOffset);
		}
		break;
	case EMode::Image_Pyramid_Filter:
		{
			pImagePyramidResult = m_pImagePyramidFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_ImagePyramidFilterSettings.DownSampleIterations, m_ImagePyramidFilterSettings.DownSampleUVOffset, m_ImagePyramidFilterSettings.Sigma);
		}
		break;
	case EMode::Gaussian_Filter:
		{
			pGaussianFilterResult = m_pGaussianFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_GaussianFilterSettings.FilterWidth);
		}
		break;
	case EMode::All:
		{
			pDualFilterResult = m_pDualFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_DualFilterSettings.DownSampleIterations, m_DualFilterSettings.DownSampleUVOffset, m_DualFilterSettings.UpSampleUVOffset);
			pImagePyramidResult = m_pImagePyramidFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_ImagePyramidFilterSettings.DownSampleIterations, m_ImagePyramidFilterSettings.DownSampleUVOffset, m_ImagePyramidFilterSettings.Sigma);
			pGaussianFilterResult = m_pGaussianFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_GaussianFilterSettings.FilterWidth);

			if (!m_pMergeFramebuffer || m_pMergeFramebuffer->width() != m_framebuffer->width() || m_pMergeFramebuffer->height() != m_framebuffer->height())
			{
				m_pMergeFramebuffer = Framebuffer::create("App::MergeFramebuffer");
				m_pMergeFramebuffer->set(Framebuffer::COLOR0, Texture::createEmpty("MergeResult", m_framebuffer->width(), m_framebuffer->height(), m_framebuffer->texture(0)->format()));
			}

			Point2int16 TargetSize = Point2int16(m_pMergeFramebuffer->width(), m_pMergeFramebuffer->height());
			vRenderDevice->push2D(m_pMergeFramebuffer); {
				vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));
				vRenderDevice->clear(true, false, false);

				Args args;
				args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize)));
				args.setUniform("DualFilterResult", pDualFilterResult, Sampler::buffer());
				args.setUniform("ImagePyramidFilterResult", pImagePyramidResult, Sampler::buffer());
				args.setUniform("GaussianFilterResult", pGaussianFilterResult, Sampler::buffer());
				args.setUniform("SceneColor", m_framebuffer->texture(0), Sampler::buffer());
				args.setUniform("TargetSize", TargetSize);

				LAUNCH_SHADER("shaders/Merge.pix", args);
			} vRenderDevice->pop2D();
		}
		break;
	default:
		break;
	}
}

bool App::onEvent(const GEvent& vEvent)
{
	if (vEvent.type == GEventType::KEY_DOWN && vEvent.key.keysym.sym == GKey::TAB)
	{
		m_pFilterConfigWindow->setVisible(!m_pFilterConfigWindow->visible());
	}

	return GApp::onEvent((vEvent));
}

void App::_makeGUI()
{
	developerWindow->setVisible(false);
	developerWindow->cameraControlWindow->setVisible(false);
	developerWindow->sceneEditorWindow->setVisible(false);

	m_pFilterConfigWindow = GuiWindow::create("Settings", debugWindow->theme(), Rect2D::xywh(0.0f, 0.0f, 50.0f, 50.0f), GuiTheme::NORMAL_WINDOW_STYLE, GuiWindow::HIDE_ON_CLOSE);

	m_pFilterConfigWindow->pane()->addPane("Current Mode", GuiTheme::PaneStyle::ORNATE_PANE_STYLE)->addEnumClassRadioButtons<EMode>("", &m_CurrentMode);

	m_DualFilterSettings.makeGUI(m_pFilterConfigWindow->pane()->addPane("Dual Filter", GuiTheme::PaneStyle::ORNATE_PANE_STYLE));
	m_ImagePyramidFilterSettings.makeGUI(m_pFilterConfigWindow->pane()->addPane("Image Pyramid Filter", GuiTheme::PaneStyle::ORNATE_PANE_STYLE));
	m_GaussianFilterSettings.makeGUI(m_pFilterConfigWindow->pane()->addPane("Gaussian Filter", GuiTheme::PaneStyle::ORNATE_PANE_STYLE));

	m_pFilterConfigWindow->pane()->pack();

	addWidget(m_pFilterConfigWindow);
}
