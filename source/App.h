#pragma once
#include <G3D/G3D.h>

class CRenderer;

class CApp : public GApp
{
private:
	shared_ptr<GuiWindow> m_pConfigWindow;
	shared_ptr<CRenderer> m_pRenderer;

public:
	CApp(const GApp::Settings& vSettings = GApp::Settings());

	virtual void onInit() override;
	virtual void onGraphics3D(RenderDevice* vRenderDevice, Array<shared_ptr<Surface>>& vAllSurfaces) override;
	virtual bool onEvent(const GEvent& vEvent) override;

protected:
	void _makeGUI();
};
