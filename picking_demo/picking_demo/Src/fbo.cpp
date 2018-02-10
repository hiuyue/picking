

#include <stdio.h>

#include "fbo.h"

FBO::FBO()
{
    m_fbo = 0;
    m_pickingTexture = 0;
    m_depthTexture = 0;
}

FBO::~FBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_pickingTexture != 0) {
        glDeleteTextures(1, &m_pickingTexture);
    }
    
    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
    }
}


bool FBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);    
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Create the texture object for the primitive information buffer
    glGenTextures(1, &m_pickingTexture);
    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);    

    // Create the texture object for the depth buffer
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);    

	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Verify that the FBO is correct
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }
    
    // Restore the default framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return (glGetError() == GL_NO_ERROR);
}


void FBO::Bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}


void FBO::Unbind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


float FBO::ReadPixel(unsigned int x, unsigned int y)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    float Pixel[3];
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, Pixel);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	printf("pixel %f %f %f \n", Pixel[0], Pixel[1], Pixel[2]);

	return Pixel[0];
}