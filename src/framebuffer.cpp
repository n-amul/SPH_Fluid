#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment) {
  auto framebuffer = FramebufferUPtr(new Framebuffer());
  if (!framebuffer->InitWithColorAttachment(colorAttachment))
    return nullptr;
  return std::move(framebuffer);
}
FramebufferUPtr Framebuffer::CreateMSAA(int width, int height, uint32_t format) {
  auto framebuffer = FramebufferUPtr(new Framebuffer());
  if (!framebuffer->InitWithColorAttachmentMSAA( width,  height, format))
    return nullptr;
  return std::move(framebuffer);
}

Framebuffer::~Framebuffer() {
  if (m_depthStencilBuffer) {
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);
  }
  if (m_framebuffer) {
    glDeleteFramebuffers(1, &m_framebuffer);
  }
}

void Framebuffer::BindToDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment) {
  m_colorAttachment = colorAttachment;
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->Get(), 0);

  glGenRenderbuffers(1, &m_depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
  glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, colorAttachment->GetWidth(), colorAttachment->GetHeight());
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);

  auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    SPDLOG_ERROR("failed to create framebuffer: {}", result);
    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}

bool Framebuffer::InitWithColorAttachmentMSAA(int width, int height, uint32_t format)
{
  m_colorAttachment = Texture::CreateMSAA(width,height,format);
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_colorAttachment->Get(), 0);

  glGenRenderbuffers(1, &m_depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
  glRenderbufferStorageMultisample( GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_colorAttachment->GetWidth(), m_colorAttachment->GetHeight());//*check sample
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);

  auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    SPDLOG_ERROR("failed to create framebuffer: {}", result);
    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}
