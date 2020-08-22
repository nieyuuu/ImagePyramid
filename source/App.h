#pragma once
#include <G3D/G3D.h>

class CDualFilter;

class App : public GApp
{
private:
	shared_ptr<CDualFilter> m_pDualFilter;

	void __initFiltersIfNecessary();

public:
	App(const GApp::Settings& settings = GApp::Settings());

	virtual void onInit() override;
	virtual void onGraphics3D(RenderDevice* vRenderDevice, Array<shared_ptr<Surface>>& vAllSurfaces) override;

protected:
	void _makeGUI();
};
