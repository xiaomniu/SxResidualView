#ifndef COPENGLCORESUB_H
#define COPENGLCORESUB_H

#include "Src_GlWnd/COpenGLCore.h"

class COpenGLCoreSub: public COpenGLCore//QOpenGLExtraFunctions
{
public:
    COpenGLCoreSub();
    ~COpenGLCoreSub();

public:
    ////////////////////////////////

    void CalcRotatePosition(float fSrcX, float fSrcY, float fBaseX, float fBaseY, float fTheta, float* pfDstX, float* pfDstY);
    ////////////////////////////////
    virtual int Draw();
    virtual void MouseMoving(int xPos, int yPos);
};

#endif
