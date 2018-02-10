

#ifndef FBO_H
#define	FBO_H

#include <GL/glew.h>

class FBO
{
public:
	FBO();

	~FBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void Bind();
    
    void Unbind();

    float ReadPixel(unsigned int x, unsigned int y);
    
private:
    GLuint m_fbo;
    GLuint m_pickingTexture;
    GLuint m_depthTexture;
};

#endif

