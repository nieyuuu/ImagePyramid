#include "App.h"
#include "Renderer.h"


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

	return CApp(settings).run();
}

CApp::CApp(const GApp::Settings& vSettings) : GApp(vSettings) {}

void CApp::onInit()
{
	GApp::onInit();

	setFrameDuration(1.0f / 240.0f);

	showRenderingStats = false;

	loadScene("G3D Sponza (Statue)");

	m_pRenderer = CRenderer::create();
	m_pRenderer->setDeferredShading(true);
	m_pRenderer->setOrderIndependentTransparency(true);

	m_renderer = m_pRenderer;

	_makeGUI();

	developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
	developerWindow->videoRecordDialog->setCaptureGui(false);
}

void CApp::onGraphics3D(RenderDevice* vRenderDevice, Array<shared_ptr<Surface>>& vAllSurfaces)
{
	FilmSettings& FilmSettings = activeCamera()->filmSettings();
	if (FilmSettings.temporalAntialiasingEnabled())
	{
		//Disable TAA
		FilmSettings.setTemporalAntialiasingEnabled(false);
	}
	if (FilmSettings.bloomStrength() > 0.0f)
	{
		//Disable Bloom
		FilmSettings.setBloomStrength(0.0f);
	}
	if (activeCamera()->depthOfFieldSettings().enabled())
	{
		//Disable Depth of Field
		activeCamera()->depthOfFieldSettings().setEnabled(false);
	}

	GApp::onGraphics3D(vRenderDevice, vAllSurfaces);
}

bool CApp::onEvent(const GEvent& vEvent)
{
	if (vEvent.type == GEventType::KEY_DOWN && vEvent.key.keysym.sym == GKey::TAB)
	{
		m_pConfigWindow->setVisible(!m_pConfigWindow->visible());
	}

	return GApp::onEvent((vEvent));
}

void CApp::_makeGUI()
{
	developerWindow->setVisible(false);
	developerWindow->cameraControlWindow->setVisible(false);
	developerWindow->sceneEditorWindow->setVisible(false);

	m_pConfigWindow = GuiWindow::create("Settings", debugWindow->theme(), Rect2D::xywh(0.0f, 0.0f, 50.0f, 50.0f), GuiTheme::NORMAL_WINDOW_STYLE, GuiWindow::HIDE_ON_CLOSE);

	m_pRenderer->makeGUI(m_pConfigWindow.get());

	addWidget(m_pConfigWindow);
}
