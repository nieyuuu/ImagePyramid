#include "App.h"

G3D_START_AT_MAIN();

int main(int argc, const char* argv[])
{
	initGLG3D(G3DSpecification());

	GApp::Settings settings(argc, argv);

	settings.window.caption = argv[0];

	settings.window.fullScreen = false;
	if (settings.window.fullScreen)
	{
		settings.window.width = 1920;
		settings.window.height = 1080;
	}
	else
	{
		settings.window.width = OSWindow::primaryDisplayWindowSize().x;
		settings.window.height = OSWindow::primaryDisplayWindowSize().y;

		// Make even
		settings.window.height -= settings.window.height & 1;
	}
	settings.window.resizable = !settings.window.fullScreen;
	settings.window.framed = !settings.window.fullScreen;
	settings.window.defaultIconFilename = "icon.png";

	settings.window.asynchronous = true;

	settings.hdrFramebuffer.setGuardBandsAndSampleRate(64, 0, 1.0f);

	settings.renderer.deferredShading = true;
	settings.renderer.orderIndependentTransparency = true;

	settings.dataDir = FileSystem::currentDirectory();

	if (!FileSystem::exists(settings.screenCapture.outputDirectory))
	{
		settings.screenCapture.outputDirectory = "";
	}
	settings.screenCapture.includeAppRevision = false;
	settings.screenCapture.includeG3DRevision = false;
	settings.screenCapture.filenamePrefix = "_";


	return App(settings).run();
}

App::App(const GApp::Settings& settings) : GApp(settings) {}

void App::onInit()
{
	GApp::onInit();

	setFrameDuration(1.0f / 240.0f);

	showRenderingStats = false;

	loadScene("G3D Simple Cornell Box (Area Light)");

	makeGUI();

	developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
	developerWindow->videoRecordDialog->setCaptureGui(false);
}

void App::makeGUI()
{

}
