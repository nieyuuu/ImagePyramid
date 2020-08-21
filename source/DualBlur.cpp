#include "DualBlur.h"
#include <string>

bool CDualBlur::__needRecreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration) const
{
	if (m_DownSampleChain.size() == 0 || m_UpSampleChain.size() != m_DownSampleChain.size() || m_DownSampleChain.size() != vDownSampleIteration + 1)
		return true;
	if (m_DownSampleChain[0]->width() != vInputTextureWidth || m_DownSampleChain[0]->height() != vInputTextureHeight)
		return true;

	return false;
}

void CDualBlur::__recreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration)
{
	m_DownSampleChain.resize(vDownSampleIteration + 1);
	m_UpSampleChain.resize(vDownSampleIteration + 1);

	int w = vInputTextureWidth;
	int h = vInputTextureHeight;
	for (int i = 1; i <= vDownSampleIteration; ++i)
	{
		w = max(w / 2, 1);
		h = max(h / 2, 1);

		m_DownSampleChain[i] = Texture::createEmpty(("DualBlur::DownSampleChain" + std::to_string(i)).c_str(), w, h, ImageFormat::RGB32F());
	}

	w = vInputTextureWidth;
	h = vInputTextureHeight;
	for (int i = 0; i < vDownSampleIteration; ++i)
	{
		std::string Name = i == 0 ? "DualBlur::BlurResult" : "DualBlur::UpSampleChain" + std::to_string(i);
		m_UpSampleChain[i] = Texture::createEmpty((Name).c_str(), w, h, ImageFormat::RGB32F());
		w = max(w / 2, 1);
		h = max(h / 2, 1);
	}
	m_UpSampleChain[vDownSampleIteration] = m_DownSampleChain[vDownSampleIteration];
}

shared_ptr<Texture> CDualBlur::Blur(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, int vDownSampleIteration, float vUVOffset)
{
	_ASSERT(vInputTexture);

	if (__needRecreateTextureChain(vInputTexture->width(), vInputTexture->height(), vDownSampleIteration))
	{
		__recreateTextureChain(vInputTexture->width(), vInputTexture->height(), vDownSampleIteration);
	}

	m_DownSampleChain[0] = vInputTexture;

	for (int i = 1; i <= vDownSampleIteration; ++i)
	{
		m_pDownSampleFramebuffer->set(Framebuffer::COLOR0, m_DownSampleChain[i]);
		Point2int16 TargetSize = Point2int16(m_pDownSampleFramebuffer->width(), m_pDownSampleFramebuffer->height());
		vRenderDevice->push2D(m_pDownSampleFramebuffer); {
			vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));
			Args args;
			args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize.x, TargetSize.y)));
			args.setUniform("DownSampleTargetSize", TargetSize);
			args.setUniform("UVOffset", vUVOffset);
			m_DownSampleChain[i - 1]->setShaderArgs(args, "InputTexture.", Sampler::buffer());

			LAUNCH_SHADER("shaders/DualBlurDownSample.pix", args);
		} vRenderDevice->pop2D();
	}

	for (int i = vDownSampleIteration - 1; i >= 0 ; --i)
	{
		m_pUpSampleFramebuffer->set(Framebuffer::COLOR0, m_UpSampleChain[i]);
		Point2int16 TargetSize = Point2int16(m_pUpSampleFramebuffer->width(), m_pUpSampleFramebuffer->height());
		vRenderDevice->push2D(m_pUpSampleFramebuffer); {
			vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));
			Args args;
			args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize.x, TargetSize.y)));
			args.setUniform("UpSampleTargetSize", TargetSize);
			args.setUniform("UVOffset", vUVOffset);
			m_UpSampleChain[i + 1]->setShaderArgs(args, "InputTexture.", Sampler::buffer());

			LAUNCH_SHADER("shaders/DualBlurUpSample.pix", args);
		} vRenderDevice->pop2D();
	}

	return m_UpSampleChain[0];
}
