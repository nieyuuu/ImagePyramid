#include "ImagePyramidFilter.h"

bool CImagePyramidFilter::__needRecreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, int vDownSampleIteration, const ImageFormat* vFormat) const
{
	if (m_DownSampleChain.size() == 0 || m_UpSampleChain.size() != m_DownSampleChain.size() || m_DownSampleChain.size() != vDownSampleIteration + 1)
		return true;
	if (m_DownSampleChain[0]->width() != vInputTextureWidth || m_DownSampleChain[0]->height() != vInputTextureHeight || m_DownSampleChain[0]->format() != vFormat)
		return true;

	return false;
}

void CImagePyramidFilter::__recreateTextureChain(int vInputTextureWidth, int vInputTextureHeight, const ImageFormat* vFormat, int vDownSampleIteration)
{
	m_DownSampleChain.resize(vDownSampleIteration + 1);
	m_UpSampleChain.resize(vDownSampleIteration + 1);

	int w = vInputTextureWidth;
	int h = vInputTextureHeight;
	for (int i = 1; i <= vDownSampleIteration; ++i)
	{
		w = max(w / 2, 1);
		h = max(h / 2, 1);

		std::string Name = std::string("ImagePyramidFilter::DownSample::") + std::to_string(w) + std::string("*") + std::to_string(h);
		m_DownSampleChain[i] = Texture::createEmpty(Name.c_str(), w, h, vFormat);
	}

	w = vInputTextureWidth;
	h = vInputTextureHeight;
	for (int i = 0; i < vDownSampleIteration; ++i)
	{
		std::string Name = std::string("ImagePyramidFilter::UpSample::") + std::to_string(w) + std::string("*") + std::to_string(h);
		m_UpSampleChain[i] = Texture::createEmpty(Name.c_str(), w, h, vFormat);

		w = max(w / 2, 1);
		h = max(h / 2, 1);
	}

	m_UpSampleChain[vDownSampleIteration] = m_DownSampleChain[vDownSampleIteration];
}

float CImagePyramidFilter::__mipGaussianBlendWeight(float vSigma, int vLevel)
{
	float C = 2.0f * pi() * vSigma * vSigma;
	float Numerator = (1 << (vLevel << 2)) * log(4.0f);
	float Denorminator = C * ((1 << (vLevel << 1)) + C);

	return clamp(Numerator / Denorminator, 0.0f, 1.0f);
}

shared_ptr<Texture> CImagePyramidFilter::Apply(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, int vDownSampleIteration, float vDownSampleUVOffset, float vSigma)
{
	_ASSERT(vInputTexture);

	if (__needRecreateTextureChain(vInputTexture->width(), vInputTexture->height(), vDownSampleIteration, vInputTexture->format()))
	{
		__recreateTextureChain(vInputTexture->width(), vInputTexture->height(), vInputTexture->format(), vDownSampleIteration);
	}

	m_DownSampleChain[0] = vInputTexture;

	Sampler s = Sampler::buffer();
	s.interpolateMode = InterpolateMode::BILINEAR_NO_MIPMAP;

	shared_ptr<Framebuffer> pIntermediateFramebuffer = Framebuffer::create("ImagePyramidFilter::IntermediateFramebuffer");

	for (int i = 1; i <= vDownSampleIteration; ++i)
	{
		pIntermediateFramebuffer->set(Framebuffer::COLOR0, m_DownSampleChain[i]);
		Point2int16 TargetSize = Point2int16(pIntermediateFramebuffer->width(), pIntermediateFramebuffer->height());

		vRenderDevice->push2D(pIntermediateFramebuffer); {
			vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));

			Args args;
			args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize.x, TargetSize.y)));
			args.setUniform("DownSampleTargetSize", TargetSize);
			args.setUniform("UVOffset", vDownSampleUVOffset);
			m_DownSampleChain[i - 1]->setShaderArgs(args, "InputTexture.", s);

			LAUNCH_SHADER("shaders/ImagePyramidFilterDownSample.pix", args);
		} vRenderDevice->pop2D();
	}

	for (int i = vDownSampleIteration - 1; i >= 0; --i)
	{
		pIntermediateFramebuffer->set(Framebuffer::COLOR0, m_UpSampleChain[i]);
		Point2int16 TargetSize = Point2int16(pIntermediateFramebuffer->width(), pIntermediateFramebuffer->height());

		vRenderDevice->push2D(pIntermediateFramebuffer); {
			vRenderDevice->setColorClearValue(Color4(0.0f, 0.0f, 0.0f, 0.0f));

			Args args;
			args.setRect(Rect2D(Point2(0, 0), Point2(TargetSize.x, TargetSize.y)));
			args.setUniform("UpSampleTargetSize", TargetSize);

			args.setUniform("CoarserTexture", m_UpSampleChain[i + 1], s);
			args.setUniform("DownSampleTexture", m_DownSampleChain[i], s);
			args.setUniform("BlendWeight", __mipGaussianBlendWeight(vSigma, i));

			LAUNCH_SHADER("shaders/ImagePyramidFilterUpSample.pix", args);
		} vRenderDevice->pop2D();
	}

	return m_UpSampleChain[0];
}
