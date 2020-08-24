#include "GaussianFilter.h"
#include <G3D-app/GaussianBlur.h>

void CGaussianFilter::__createOrResizeFramebuffers(int vInputTextureWidth, int vInputTextureHeight, const ImageFormat* vFormat)
{
    if (m_pIntermediateFramebuffer == nullptr || m_pResultFramebuffer == nullptr || m_pIntermediateFramebuffer->texture(0)->format() != vFormat)
    {
        m_pIntermediateFramebuffer = Framebuffer::create("GaussianFilter::IntermediateFramebuffer");
        m_pIntermediateFramebuffer->set(Framebuffer::COLOR0, Texture::createEmpty("GaussianFilter::Intermediate", vInputTextureWidth, vInputTextureHeight, vFormat));

        m_pResultFramebuffer = Framebuffer::create("GaussianFilter::ResultFramebuffer");
        m_pResultFramebuffer->set(Framebuffer::COLOR0, Texture::createEmpty("GaussianFilter::Result", vInputTextureWidth, vInputTextureHeight, vFormat));
    }

    if (m_pIntermediateFramebuffer->width() != vInputTextureWidth || m_pIntermediateFramebuffer->height() != vInputTextureHeight)
    {
        m_pIntermediateFramebuffer->resize(Vector2int32(vInputTextureWidth, vInputTextureHeight));
        m_pResultFramebuffer->resize(Vector2int32(vInputTextureWidth, vInputTextureHeight));
    }
}

shared_ptr<Texture> CGaussianFilter::Apply(RenderDevice* vRenderDevice, shared_ptr<Texture> vInputTexture, uint vFilterWidth)
{
    __createOrResizeFramebuffers(vInputTexture->width(), vInputTexture->height(), vInputTexture->format());

    vRenderDevice->push2D(m_pIntermediateFramebuffer); {
        GaussianBlur::apply(vRenderDevice, vInputTexture, Vector2(1.0f, 0.0f), vFilterWidth);
    } vRenderDevice->pop2D();

    vRenderDevice->push2D(m_pResultFramebuffer); {
        GaussianBlur::apply(vRenderDevice, m_pIntermediateFramebuffer->texture(0), Vector2(0.0f, 1.0f), vFilterWidth);
    } vRenderDevice->pop2D();

    return m_pResultFramebuffer->texture(0);
}
