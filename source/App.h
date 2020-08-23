#pragma once
#include <G3D/G3D.h>

class CDualFilter;
class CImagePyramidFilter;

class App : public GApp
{
private:
	shared_ptr<CDualFilter> m_pDualFilter;
	shared_ptr<CImagePyramidFilter> m_pImagePyramidFilter;

	void __initFiltersIfNecessary();

public:
	App(const GApp::Settings& settings = GApp::Settings());

	virtual void onInit() override;
	virtual void onGraphics3D(RenderDevice* vRenderDevice, Array<shared_ptr<Surface>>& vAllSurfaces) override;

protected:
	void _makeGUI();
};
