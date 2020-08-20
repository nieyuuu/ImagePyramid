#pragma once
#include <G3D/G3D.h>

class CBlurRenderer;

class App : public GApp
{
private:
	shared_ptr<CBlurRenderer> m_pBlurRenderer = nullptr;

public:
	App(const GApp::Settings& settings = GApp::Settings());

	virtual void onInit() override;

protected:
	void makeGUI();
};
