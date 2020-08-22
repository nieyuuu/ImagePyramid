#include "App.h"
#include "DualFilter.h"

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

	m_pDualFilter->Apply(vRenderDevice, m_framebuffer->texture(0), 3, 1.0f);
}

void App::_makeGUI()
{

}
