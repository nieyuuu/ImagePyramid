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

	m_pDualFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_DualFilterSettings.DownSampleIterations, m_DualFilterSettings.DownSampleUVOffset, m_DualFilterSettings.UpSampleUVOffset);
	m_pImagePyramidFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_ImagePyramidFilterSettings.DownSampleIterations, m_ImagePyramidFilterSettings.DownSampleUVOffset, m_ImagePyramidFilterSettings.Sigma);
	m_pGaussianFilter->Apply(vRenderDevice, m_framebuffer->texture(0), m_GaussianFilterSettings.FilterWidth);
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
