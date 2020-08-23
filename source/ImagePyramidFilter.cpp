#include "ImagePyramidFilter.h"

bool CImagePyramidFilter::__needRecreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration) const
{
	if (m_DownSampleChain.size() == 0 || (m_UpSampleChain.size() + 1) != m_DownSampleChain.size() || m_DownSampleChain.size() != vDownSampleIteration + 1)
		return true;
	if (m_DownSampleChain[0]->width() != vInputTextureWidth || m_DownSampleChain[0]->height() != vInputTextureHeight)
		return true;

	return false;
}

void CImagePyramidFilter::__recreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, const ImageFormat* vFormat, int vDownSampleIteration)
{
	m_DownSampleChain.resize(vDownSampleIteration + 1);
	m_UpSampleChain.resize(vDownSampleIteration);

	int w = vInputTextureWidth;
	int h = vInputTextureHeight;
	for (int i = 1; i <= vDownSampleIteration; ++i)
	{
		w = max(w / 2, 1);
		h = max(h / 2, 1);

		m_DownSampleChain[i] = Texture::createEmpty(("ImagePyramidFilter::DownSampleChain" + std::to_string(i)).c_str(), w, h, vFormat);
	}

	w = vInputTextureWidth;
	h = vInputTextureHeight;
	for (int i = 0; i < vDownSampleIteration; ++i)
	{
		std::string Name = i == 0 ? "ImagePyramidFilter::FinalResult" : "ImagePyramidFilter::UpSampleChain" + std::to_string(i);
		m_UpSampleChain[i] = Texture::createEmpty((Name).c_str(), w, h, vFormat);
		w = max(w / 2, 1);
		h = max(h / 2, 1);
	}
}

float CImagePyramidFilter::__MipGaussianBlendWeight(float vSigma, int vLevel)
{
	float Sigma2 = vSigma * vSigma;

	float C = 2.0f * pi() * Sigma2;
	float Numerator = (1 << (vLevel << 2)) * log(4.0f);
	float Denorminator = C * ((1 << (vLevel << 1)) + C);

	return clamp(Numerator / Denorminator, 0.0f, 1.0f);
}

shared_ptr<Texture> CImagePyramidFilter::Apply(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, int vDownSampleIteration, float vSigma)
{
	_ASSERT(vInputTexture);

	if (__needRecreateTextureChain(vInputTexture->width(), vInputTexture->height(), vDownSampleIteration))
	{
		__recreateTextureChain(vInputTexture->width(), vInputTexture->height(), vInputTexture->format(), vDownSampleIteration);
	}

	m_DownSampleChain[0] = vInputTexture;

	Sampler s = Sampler::buffer();
	s.interpolateMode = InterpolateMode::BILINEAR_NO_MIPMAP;

	for (int i = 1; i <= vDownSampleIteration; ++i)
	{
		m_pDownSampleFramebuffer->set(Framebuffer::COLOR0, m_DownSampleChain[i]);
		Point2int16 TargetSize = Point2int16(m_pDownSampleFramebuffer->width(), m_pDownSampleFramebuffer->height());

		vRenderDevice->push2D(m_pDownSampleFramebuffer); {
			vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));

			Args args;
			args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize.x, TargetSize.y)));
			args.setUniform("DownSampleTargetSize", TargetSize);
			m_DownSampleChain[i - 1]->setShaderArgs(args, "InputTexture.", s);

			LAUNCH_SHADER("shaders/ImagePyramidFilterDownSample.pix", args);
		} vRenderDevice->pop2D();
	}

	for (int i = vDownSampleIteration - 1; i >= 0; --i)
	{
		m_pUpSampleFramebuffer->set(Framebuffer::COLOR0, m_UpSampleChain[i]);
		Point2int16 TargetSize = Point2int16(m_pUpSampleFramebuffer->width(), m_pUpSampleFramebuffer->height());

		vRenderDevice->push2D(m_pUpSampleFramebuffer); {
			vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));

			Args args;
			args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize.x, TargetSize.y)));
			args.setUniform("UpSampleTargetSize", TargetSize);

			shared_ptr<Texture> CoarserTexture = (i == vDownSampleIteration - 1) ? m_DownSampleChain[vDownSampleIteration] : m_UpSampleChain[i + 1];
			args.setUniform("CoarserTexture", CoarserTexture, s);
			args.setUniform("DownSampleTexture", m_DownSampleChain[i], s);
			args.setUniform("Sigma", vSigma);
			args.setUniform("Level", i);

			LAUNCH_SHADER("shaders/ImagePyramidFilterUpSample.pix", args);
		} vRenderDevice->pop2D();
	}

	return m_UpSampleChain[0];
}
