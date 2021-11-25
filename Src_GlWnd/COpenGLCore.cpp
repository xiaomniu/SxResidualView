#include "COpenGLCore.h"

#include <QCoreApplication>
#include <algorithm>

#include "FontRenderer.h"
#include "Src_Geometry/CDrawGround.h"
#include "Src_Geometry/CBaseMap.h"
#include "Src_Geometry/CRasterDraw.h"
#include "Src_Geometry/CLayerVector.h"
#include "Src_Geometry/CLayerGeoDraw.h"

//extern const char *g_ErrCantLoadOGL;
//extern const char *g_ErrCantUnloadOGL;

const color32 COLOR32_BLACK     = 0xff000000;   // Black
const color32 COLOR32_WHITE     = 0xffffffff;   // White
const color32 COLOR32_ZERO      = 0x00000000;   // Zero
const color32 COLOR32_RED       = 0xffff0000;   // Red
const color32 COLOR32_GREEN     = 0xff00ff00;   // Green
const color32 COLOR32_BLUE      = 0xff0000ff;   // Blue
//  ---------------------------------------------------------------------------

#ifdef _DEBUG
    void CheckGLCoreError(const char *file, int line, const char *func, COpenGLCore* pCore)
    {
        GLenum err=0;
        char msg[256];
        while( (err=pCore->glGetError())!=0 )
        {
            sprintf(msg, "%s(%d) : [%s] GL_CORE_ERROR=0x%x\n", file, line, func, err);
            #ifdef ANT_WINDOWS
                OutputDebugString(msg);
            #endif
            fprintf(stderr, msg);
        }
    }
#   ifdef __FUNCTION__
#       define CHECK_GL_ERROR CheckGLCoreError(__FILE__, __LINE__, __FUNCTION__, this)
#   else
#       define CHECK_GL_ERROR CheckGLCoreError(__FILE__, __LINE__, "")
#   endif
#else
#   define CHECK_GL_ERROR// ((void)(0))
#endif

//  ---------------------------------------------------------------------------
static inline glm::vec4 COLOR32_INT_TO_FLOAT_RGBA(unsigned int nColor){
    float fRed = static_cast<float>(nColor & 0xFF);
    float fGreen = static_cast<float>((nColor & 0xFF00)>>8);
    float fBlue = static_cast<float>((nColor & 0xFF0000)>>16);
    float fAlpha = static_cast<float>((nColor & 0xFF000000)>>24);
    //glm::vec4 fColor = glm::vec4(fRed / 255.0f, fGreen / 255.0f, fBlue / 255.0f, fAlpha / 255.0f);
    //float fColor[4] = {fRed / 255.0f, fGreen / 255.0f, fBlue / 255.0f, fAlpha / 255.0f};
    return glm::vec4(fRed / 255.0f, fGreen / 255.0f, fBlue / 255.0f, fAlpha / 255.0f);;
}
//  ---------------------------------------------------------------------------

//  ---------------------------------------------------------------------------

COpenGLCore::COpenGLCore()
{
    this->m_sExeFullPath = QCoreApplication::applicationFilePath().toLocal8Bit().data();
    this->m_sExeDir = this->m_sExeFullPath.substr(0, this->m_sExeFullPath.rfind('/'));//applicationDirPath

    this->InitBase();
    this->InitSence();

    this->m_pFontRender = new CFontRenderer(this);
    this->m_pFontRender->Init(0, 0);

    this->m_MatPerspectProject = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f);
    this->m_MatModel = glm::mat4(1.0f);

    //this->m_pWheelTimer = new QTimer;
    connect(&this->m_WheelTimer, &QTimer::timeout, this, &COpenGLCore::WheelTimerProc);

    this->m_pSoilTool = new CSoilTool;
    this->m_pSoilTool->m_pGlobalGLCore = this;
}

COpenGLCore::~COpenGLCore(){
    if(this->m_mapImgPath2TextureID.size() > 0){
        auto itt = this->m_mapImgPath2TextureID.begin();
        auto ittEnd = this->m_mapImgPath2TextureID.end();
        GLuint nTextureID = -1;
        for(;itt!=ittEnd;itt++){
            nTextureID = itt->second;
            glDeleteTextures(1, &nTextureID);
        }
        this->m_mapImgPath2TextureID.clear();
    }
}

//  ---------------------------------------------------------------------------

int COpenGLCore::Draw(){

    this->m_MatMVPCore = this->m_MatPerspectProject * this->m_MatView * this->m_MatModel;

    //time_t ss=clock();
    //this->TestDraw();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(this->m_nMouseStatus != COpenGLCore::e_Mouse_MIDDLE_DOWN_MOVING
            && this->m_nMouseStatus != COpenGLCore::e_Mouse_WHEEL_SCROLLING
            && this->m_nMouseStatus != COpenGLCore::e_Mouse_MOVING)
    {

        //ss=clock();
        this->FboBind();
        //printf("this->FboBind(): %lld\n", clock()-ss);


        //ss=clock();
        //this->m_pDrawGround->Draw();
        //this->m_pBaseMap->Draw();
        //printf("this->m_pBaseMap->Draw(): %lld\n", clock()-ss);

        //ss=clock();
        this->m_pLayerVector->Draw();
        //printf("this->m_pLayerVector->Draw(): %lld\n", clock()-ss);

        //this->m_pRasterDraw->Draw();
        //this->DrawImage(100, 9100, 200, 200, this->m_sImgFileFullPath.c_str());

        if(this->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance){
            this->DrawCalcDistanceTrackFBO();
        }
        //ss=clock();
        this->FboUnbind();
        //printf("this->FboUnbind(): %lld\n", clock()-ss);
        //this->m_nMouseMovingDeltaX = 0;
        //this->m_nMouseMovingDeltaY = 0;
        //this->m_nMouseMovingLastDeltaX = 0;
        //this->m_nMouseMovingLastDeltaY = 0;
        printf("---------------------RestPos\n");
        if (this->m_nMouseStatus == COpenGLCore::e_Mouse_RESET_VIEWPORT){
            this->m_nMouseStatus = this->m_nMouseStatusLast;
        }
    }
    //static int nnn = 1;
    //printf("Draw %d: %d, %d, ,%d, %d\n", nnn++,
    //       this->m_nMouseMovingDeltaX, this->m_nMouseMovingDeltaY, this->m_nMouseMovingLastDeltaX, this->m_nMouseMovingLastDeltaY);
    //this->DrawImage(0 + this->m_nMouseMovingLastDeltaX + this->m_nMouseMovingDeltaX - this->m_nWheelOffX,
    //                0 + this->m_nMouseMovingLastDeltaY + this->m_nMouseMovingDeltaY - this->m_nWheelOffY,
    //                this->m_nWndWidth * this->m_nWheelScale, this->m_nWndHeight * this->m_nWheelScale, this->m_FboColorTextureID, 1);

    //ss=clock();
    this->DrawImage(
                this->m_nViewBaseLeftBottomX,
                this->m_nViewBaseLeftBottomY,
                this->m_nViewBaseWidth,
                this->m_nViewBaseHeight,
                this->m_FboColorTextureID, 1);

    if(this->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance){
        this->DrawCalcDistanceTrack();
    }

    float fHalfW = (float)(int)(this->m_nWndWidth >> 1);
    float fHalfH = (float)(int)(this->m_nWndHeight >> 1);
    //this->DrawLine(fHalfW - 8.0f, fHalfH, fHalfW + 8.0f, fHalfH, 0xFFFFFF00);
    //this->DrawLine(fHalfW, fHalfH - 8.0f, fHalfW, fHalfH + 8.0f, 0xFFFFFF00);
    this->DrawCircle(fHalfW, fHalfH, 8.0f, 0xFFFFFF00);

    //this->DrawString(this->m_sMsg.c_str(), this->m_nMousePosX, this->m_nMousePosY, 0.5f, 0xFF0000FF);
    this->DrawString(this->m_sMsg.c_str(), 25.0f, 20.0f, 0.5f, 0xFF0000FF);
    std::string sCornorMsg = std::to_string(this->t_geo_minmax.fGeoRightTopX - this->t_geo_minmax.fGeoLeftBottomX) +
            ": "+std::to_string(this->m_fView2GeoScaleH) + ", "+std::to_string(this->m_fView2GeoScaleV) +
            ", SelEleCnt: " + std::to_string(this->m_pLayerVector->m_nOperateEleCnt);
    this->DrawString(sCornorMsg.c_str(), 25.0f, 40.0f, 0.5f, 0xFF0000FF);
    //this->DrawString("我们是共产主义接班人一二三四|—— asdf ?.", 25.0f, 200.0f, 0.5f, 0xFF33FFFF);


    if(this->m_nMouseWorkBegin == 1 && this->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select){
        this->DrawRect((float)this->m_nMouseSelectPosStartX, (float)this->m_nMouseSelectPosStartY, (float)this->m_nMouseSelectPosEndX, (float)this->m_nMouseSelectPosEndY, 0xFFFFFF00);
        double fSelectGeoStartX, fSelectGeoStartY, fSelectGeoEndX, fSelectGeoEndY;
        this->View2GeoPosition(this->m_nMouseSelectPosStartX, this->m_nMouseSelectPosStartY, &fSelectGeoStartX, &fSelectGeoStartY);
        this->View2GeoPosition(this->m_nMouseSelectPosEndX, this->m_nMouseSelectPosEndY, &fSelectGeoEndX, &fSelectGeoEndY);
        double fSelectGeoArea = (fabs(fSelectGeoEndX - fSelectGeoStartX)) * (fabs(fSelectGeoEndY - fSelectGeoStartY));
        std::string sCornorMsg1 = std::to_string(fSelectGeoArea);
        this->DrawString(sCornorMsg1.c_str(), 25.0f, 60.0f, 0.5f, 0xFF0000FF);
    }

    //printf("this->DrawString: %lld\n", clock()-ss);
    if(this->m_nMouseStatus != COpenGLCore::e_Mouse_MIDDLE_DOWN_MOVING
            && this->m_nMouseStatus != COpenGLCore::e_Mouse_WHEEL_SCROLLING){
        this->m_nMouseStatus = COpenGLCore::e_Mouse_MOVING;

        this->m_nViewBaseLeftBottomX = 0;
        this->m_nViewBaseLeftBottomY = 0;
        this->m_nViewBaseWidth = this->m_nWndWidth;
        this->m_nViewBaseHeight = this->m_nWndHeight;

        if(this->m_pLayerVector->m_nBeingEditor == 1 && this->m_pLayerVector->m_pGeoLayerEdit)
        {
            this->m_pLayerVector->m_pGeoLayerEdit->Draw();
        }
    }

    return 1;
}

void COpenGLCore::InitSence(){


    this->m_MatPerspectProject = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f);
    this->m_MatModel = glm::mat4(1.0f);

    //this->m_pDrawGround = new CDrawGround(this);

    //this->m_pBaseMap = new SxBaseMap(this);
    //this->m_pBaseMap->Init();

    //this->m_pRasterDraw = new SxRasterDraw(this);
    //this->m_pRasterDraw->Init();

    this->m_pLayerVector = new CLayerVector(this);
    //this->m_pLayerVector->Init();
}

void COpenGLCore::InitSceneItem(){

    //this->m_pDrawGround->InitScene();
    //this->m_pBaseMap->Init();

    //this->m_pRasterDraw->Init();

    this->m_pLayerVector->Init();

}

void COpenGLCore::CalcRotatePosition(float fSrcX, float fSrcY, float fBaseX, float fBaseY, float fTheta, float* pfDstX, float* pfDstY){
    /*
    假设对图片上任意点(x,y)，绕一个坐标点(rx0,ry0)逆时针旋转a角度后的新的坐标设为(x0, y0)，有公式：
    x0= (x - rx0)*cos(a) - (y - ry0)*sin(a) + rx0 ;
    y0= (x - rx0)*sin(a) + (y - ry0)*cos(a) + ry0 ;
    */
    *pfDstX = (fSrcX - fBaseX) * cos(fTheta) - (fSrcY - fBaseY) * sin(fTheta) + fBaseX;
    *pfDstY = (fSrcX - fBaseX) * sin(fTheta) + (fSrcY - fBaseY) * cos(fTheta) + fBaseY;
}

void COpenGLCore::InitDraw2Dd(){
    //
    const GLchar *szCircleVS[] = {
        "#version 440 core\n"
        "attribute dvec3 vertex;\n"
        //"attribute double vertex_y;\n"
        "uniform mat4 projection;\n"
        "uniform dvec4 viewgeo;\n"
        "void main() { \n"
        //"double xView = vertex_x;\n"
        //"double yView = vertex_y;\n"
        //"double xView = vertex.x;\n"
        //"double yView = vertex.y;\n"
        "double xView = ((vertex.x-viewgeo.x) * viewgeo.z);\n"
        "double yView = ((vertex.y-viewgeo.y) * viewgeo.w);\n"
        "gl_Position = projection * vec4(xView, yView, 0.0, 1.0);}"
    };
    GLuint nCircleVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(nCircleVS, 1, szCircleVS, NULL);
    CompileShader(nCircleVS);

    const GLchar *szCircleFS[] = {
        "#version 440 core\n"
        "uniform vec4 V_color;\n"
        "out vec4 outColor;\n"
        "void main() { outColor = V_color; }"
    };
    GLuint nCircleFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(nCircleFS, 1, szCircleFS, NULL);
    CompileShader(nCircleFS);

    this->m_ProgramDraw2Dd = glCreateProgram();
    glAttachShader(this->m_ProgramDraw2Dd, nCircleVS);
    glAttachShader(this->m_ProgramDraw2Dd, nCircleFS);
    //glBindAttribLocation(this->m_ProgramCircle, 0, "vertex");
    //glBindAttribLocation(this->m_ProgramCircle, 1, "fcolor");
    //
    //glBindAttribLocation(this->m_ProgramCircle, 2, "V_color");
    //CHECK_GL_ERROR;

    CHECK_GL_ERROR;
    LinkProgram(this->m_ProgramDraw2Dd);

    //CHECK_GL_ERROR;
    this->m_Draw2DdProjectID = glGetUniformLocation(this->m_ProgramDraw2Dd, "projection");
    this->m_Draw2DdVertexIDX = glGetAttribLocation(this->m_ProgramDraw2Dd, "vertex");
    this->m_Draw2DdVertexIDY = glGetAttribLocation(this->m_ProgramDraw2Dd, "vertex_y");
    this->m_Draw2DdViewGeoID = glGetUniformLocation(this->m_ProgramDraw2Dd, "viewgeo");
    this->m_Draw2DdColorID = glGetUniformLocation(this->m_ProgramDraw2Dd, "V_color");
    //printf("this->m_CircleProjectID : %d\n", this->m_CircleProjectID);
    //printf("this->m_CircleVertexID : %d\n", this->m_CircleVertexID);
    //printf("this->m_CircleColorID : %d\n", this->m_CircleColorID);
    CHECK_GL_ERROR;



    glGenVertexArrays(1, &this->m_Draw2DdVAO);
    glGenBuffers(1, &this->m_Draw2DdPosVBO);
    //glGenBuffers(1, &this->m_CircleClsVBO);
}

void COpenGLCore::DrawTrianglesd(int nPointCnt, double* pPointsX, double* pPointsY, double* pPointsZ, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);
    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramDraw2Dd);

    glm::dvec4 fViewGwo(this->t_geo_minmax.fGeoLeftBottomX, this->t_geo_minmax.fGeoLeftBottomY, this->m_fView2GeoScaleH, this->m_fView2GeoScaleV);
    glUniform4dv(this->m_Draw2DdViewGeoID, 1, glm::value_ptr(fViewGwo));

    glUniformMatrix4fv(this->m_Draw2DdProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_Draw2DdColorID, 1, glm::value_ptr(fColor));
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_Draw2DdVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_Draw2DdPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nPointCnt * 3, pPointsX, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_Draw2DdVertexIDX);
    glVertexAttribLPointer(this->m_Draw2DdVertexIDX, 3, GL_DOUBLE, 0, NULL);

    CHECK_GL_ERROR;

    glDrawArrays(GL_TRIANGLES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈
    CHECK_GL_ERROR;

    glUseProgram(0);
    glLineWidth(1.0f);
    CHECK_GL_ERROR;
}
//  ---------------------------------------------------------------------------

GLuint COpenGLCore::CompileShader(GLuint shader)
{
    glCompileShader(shader);    CHECK_GL_ERROR;

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status); CHECK_GL_ERROR;
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength); CHECK_GL_ERROR;

        GLchar strInfoLog[256];
        glGetShaderInfoLog(shader, sizeof(strInfoLog), NULL, strInfoLog); CHECK_GL_ERROR;
#ifdef ANT_WINDOWS
        OutputDebugString("Compile failure: ");
        OutputDebugString(strInfoLog);
        OutputDebugString("\n");
#endif
        fprintf(stderr, "Compile failure: %s\n", strInfoLog);
        shader = 0;
    }

    return shader;
}

GLuint COpenGLCore::LinkProgram(GLuint program)
{
    glLinkProgram(program); CHECK_GL_ERROR;

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status); CHECK_GL_ERROR;
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength); CHECK_GL_ERROR;

        GLchar strInfoLog[256];
        glGetProgramInfoLog(program, sizeof(strInfoLog), NULL, strInfoLog); CHECK_GL_ERROR;
#ifdef ANT_WINDOWS
        OutputDebugString("Linker failure: ");
        OutputDebugString(strInfoLog);
        OutputDebugString("\n");
#endif
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        program = 0;
    }

    return program;
}

void COpenGLCore::ResizeTriBuffers(size_t _NewSize)
{
    m_TriBufferSize = _NewSize;

    glBindVertexArray(m_TriVArray);

    glBindBuffer(GL_ARRAY_BUFFER, m_TriVertices);
    glBufferData(GL_ARRAY_BUFFER, m_TriBufferSize*sizeof(Vec2), 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_TriUVs);
    glBufferData(GL_ARRAY_BUFFER, m_TriBufferSize*sizeof(Vec2), 0, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_TriColors);
    glBufferData(GL_ARRAY_BUFFER, m_TriBufferSize*sizeof(unsigned int), 0, GL_DYNAMIC_DRAW);

    CHECK_GL_ERROR;
}

void COpenGLCore::InitEllipse(){
    //
    const GLchar *szCircleVS[] = {
        "#version 330 core\n"
        "attribute vec2 vertex;\n"
        "uniform mat4 projection;\n"
        "void main() { gl_Position = projection * vec4(vertex, 0.0, 1.0);}"
    };
    GLuint nCircleVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(nCircleVS, 1, szCircleVS, NULL);
    CompileShader(nCircleVS);

    const GLchar *szCircleFS[] = {
        "#version 330 core\n"
        "precision highp float;\n"
        "uniform vec4 V_color;\n"
        "out vec4 outColor;\n"
        "void main() { outColor = V_color; }"
    };
    GLuint nCircleFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(nCircleFS, 1, szCircleFS, NULL);
    CompileShader(nCircleFS);

    this->m_ProgramCircle = glCreateProgram();
    glAttachShader(this->m_ProgramCircle, nCircleVS);
    glAttachShader(this->m_ProgramCircle, nCircleFS);
    //glBindAttribLocation(this->m_ProgramCircle, 0, "vertex");
    //glBindAttribLocation(this->m_ProgramCircle, 1, "fcolor");
    //
    //glBindAttribLocation(this->m_ProgramCircle, 2, "V_color");
    //CHECK_GL_ERROR;

    CHECK_GL_ERROR;
    LinkProgram(this->m_ProgramCircle);

    //CHECK_GL_ERROR;
    this->m_CircleProjectID = glGetUniformLocation(this->m_ProgramCircle, "projection");
    this->m_CircleVertexID = glGetAttribLocation(this->m_ProgramCircle, "vertex");
    //this->m_CircleColorID = glGetAttribLocation(this->m_ProgramCircle, "fcolor");
    this->m_CircleColorID = glGetUniformLocation(this->m_ProgramCircle, "V_color");
    //printf("this->m_CircleProjectID : %d\n", this->m_CircleProjectID);
    //printf("this->m_CircleVertexID : %d\n", this->m_CircleVertexID);
    //printf("this->m_CircleColorID : %d\n", this->m_CircleColorID);
    CHECK_GL_ERROR;



    glGenVertexArrays(1, &this->m_CircleVAO);
    glGenBuffers(1, &this->m_CirclePosVBO);
    //glGenBuffers(1, &this->m_CircleClsVBO);
}

void COpenGLCore::DrawCircle(float nCenterX, float nCenterY, float fRadius, unsigned int nColor, float fLineSize/* = 1.0f*/){
    this->DrawEllipse(nCenterX, nCenterY, fRadius, fRadius, nColor, fLineSize);
}

void COpenGLCore::DrawEllipse(float nCenterX, float nCenterY, float fRadiusX, float fRadiusY, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    float fCircleLen = 2.0f * PI * (std::max(fRadiusX, fRadiusY)) * 1.0f;
    int nCirclePtCnt = static_cast<int>(fCircleLen / 0.1f + 1.0f);
    float delta = 2.0f * PI / (static_cast<float>(nCirclePtCnt));
    const GLfloat z = 0.0f;
    //float fScale = fRadiusX / fRadiusX * fRadiusY;
    std::vector<glm::vec2> vecCirclePts;
    //this->m_vecCirclePts.clear();
    for (int i = 0; i < nCirclePtCnt; i++) {
        GLfloat x = fRadiusX * cos(delta * i) + nCenterX;
        GLfloat y = fRadiusY * sin(delta * i) + nCenterY;
        vecCirclePts.push_back(glm::vec2(x, y));
        //vecCircleClrs.push_back(Vec4(fColor[0], fColor[1], fColor[2], fColor[3]));
    }

    nCirclePtCnt = static_cast<int>(vecCirclePts.size());

    //this->DrawPoints(nCirclePtCnt, (float*)m_vecCirclePts.data(), nColor);
    //return;

    //glEnable(GL_LINE_SMOOTH);
    //glLineWidth(fLineSize);
    //glEnable(GL_ALPHA_TEST);
    //glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    //glAlphaFunc(GL_NOTEQUAL, 1.0f);//glBlendEquationSeparate
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramCircle);


    glUniformMatrix4fv(this->m_CircleProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_CircleColorID, 1, glm::value_ptr(fColor));
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_CircleVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_CirclePosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 2, vecCirclePts.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_CircleVertexID);
    glVertexAttribPointer(this->m_CircleVertexID, 2, GL_FLOAT, GL_TRUE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glDrawArrays(GL_LINE_LOOP, 0, nCirclePtCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_CircleVertexID);
    CHECK_GL_ERROR;

    glUseProgram(0);
    glDisable(GL_LINE_SMOOTH);
    //glDisable(GL_POINT_SMOOTH);
    //glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    //glLineWidth(1.0f);
    glPointSize(1.0f);
    CHECK_GL_ERROR;
}

void COpenGLCore::DrawPoint(float fPosX, float fPosY, unsigned int nColor, float fPointSize/* = 5.0f*/){
    float pPoints[2] = {fPosX, fPosY};
    this->DrawPoints(1, pPoints, nColor, fPointSize);
}
void COpenGLCore::DrawPoints(int nPointCnt, float* pPoints, unsigned int nColor, float fPointSize/* = 5.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    ////glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //
    glEnable(GL_POINT_SMOOTH);
    glPointSize(fPointSize);
    glUseProgram(this->m_ProgramCircle);


    glUniformMatrix4fv(this->m_CircleProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_CircleColorID, 1, glm::value_ptr(fColor));
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_CircleVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_CirclePosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointCnt * 2, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_CircleVertexID);
    glVertexAttribPointer(this->m_CircleVertexID, 2, GL_FLOAT, GL_TRUE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glDrawArrays(GL_POINTS, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_CircleVertexID);
    CHECK_GL_ERROR;

    glUseProgram(0);
    ////glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    //glDisable(GL_ALPHA_TEST);
    //glDisable(GL_BLEND);
    ////glLineWidth(1.0f);
    glPointSize(1.0f);
    CHECK_GL_ERROR;
}
void COpenGLCore::DrawLine(float fPosXS, float fPosYS, float fPosXE, float fPosYE, unsigned int nColor, float fLineSize/* = 1.0f*/){
    float pPoints[4] = {fPosXS, fPosYS, fPosXE, fPosYE};
    this->DrawLines(2, pPoints, nColor, fLineSize);
}

void COpenGLCore::DrawRect(float fPosXS, float fPosYS, float fPosXE, float fPosYE, unsigned int nColor, float fLineSize/* = 1.0f*/){
    float pPoints[16] = {
        fPosXS, fPosYS, fPosXS, fPosYE,
        fPosXS, fPosYE, fPosXE, fPosYE,
        fPosXE, fPosYE, fPosXE, fPosYS,
        fPosXE, fPosYS, fPosXS, fPosYS,};
    this->DrawLines(8, pPoints, nColor, fLineSize);
}
void COpenGLCore::DrawLines(int nPointCnt, float* pPoints, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramCircle);

    glUniformMatrix4fv(this->m_CircleProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_CircleColorID, 1, glm::value_ptr(fColor));
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_CircleVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_CirclePosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointCnt * 2, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_CircleVertexID);
    glVertexAttribPointer(this->m_CircleVertexID, 2, GL_FLOAT, GL_TRUE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_CircleVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glLineWidth(1.0f);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

}

void COpenGLCore::DrawPolygon(int nPointCnt, float* pPoints, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramCircle);

    glUniformMatrix4fv(this->m_CircleProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_CircleColorID, 1, glm::value_ptr(fColor));
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_CircleVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_CirclePosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointCnt * 2, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_CircleVertexID);
    glVertexAttribPointer(this->m_CircleVertexID, 2, GL_FLOAT, GL_TRUE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glDrawArrays(GL_LINE_LOOP, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_CircleVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

}

void COpenGLCore::DrawTriPolygon(int nPointCnt, float* pPoints, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramCircle);

    glUniformMatrix4fv(this->m_CircleProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_CircleColorID, 1, glm::value_ptr(fColor));
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_CircleVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_CirclePosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointCnt * 2, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_CircleVertexID);
    glVertexAttribPointer(this->m_CircleVertexID, 2, GL_FLOAT, GL_TRUE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glDrawArrays(GL_TRIANGLES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_CircleVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

}

void COpenGLCore::InitCircle(){
}

void COpenGLCore::InitImage() {
    //int nFileSize = 0;
    //const char* vs = "D:/QT_CODE_PRJ/SuperXView01/Res/shaders/sampleTexture.vs";
    //const char* fs = "D:/QT_CODE_PRJ/SuperXView01/Res/shaders/sampleTexture.fs";
    //const char* vsCode = (char*)LoadFileContent(vs, nFileSize);
    //const char* fsCode = (char*)LoadFileContent(fs, nFileSize);

    const char vsCode[] = {
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 V_TexCoords;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "    V_TexCoords = vertex.zw; }\n"
    };
    const char fsCode[] = {
        "#version 330 core\n"
        "in vec2 V_TexCoords;\n"
        "out vec4 color;\n"
        "uniform sampler2D text2D;\n"
        "void main() {\n"
        "    vec4 sampled = texture(text2D, V_TexCoords);\n"
        "    color = sampled;}\n"
    };
    GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vsCode);
    if (vsShader == 0) {
        return;
    }
    GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsCode);
    if (fsShader == 0) {
        return;
    }
    this->m_ProgramTexture = CreateProgram(vsShader, fsShader);
    glDeleteShader(vsShader);
    glDeleteShader(fsShader);
    if (this->m_ProgramTexture == 0) {
        printf("Font Program create faild!\n");
        return;
    }

    this->m_TextureProjectID = glGetUniformLocation(this->m_ProgramTexture, "projection");
    this->m_TextureSampleText2D = glGetUniformLocation(this->m_ProgramTexture, "text2D");

    glGenVertexArrays(1, &this->m_TextureVAO);
    glGenBuffers(1, &this->m_TextureVBO);

    glBindVertexArray(0);
}

void COpenGLCore::DrawImage(float xPos, float yPos, float fWidth, float fHeight, GLuint nTextureID, int nReverY/* = 0*/){
    //float xpos = 10.0f, ypos = 20.0f;
    //float w = 400.0f, h = 400.0f;
    float nPosY0 = 0.0f;
    float nPosY1 = 1.0f;
    if(nReverY == 1){
        nPosY0 = 1.0f;
        nPosY1 = 0.0f;
    }
    GLfloat vertices[6][4] = {
        { xPos,          yPos + fHeight,   0.0, nPosY0 },
        { xPos,          yPos,             0.0, nPosY1 },
        { xPos + fWidth, yPos,             1.0, nPosY1 },

        { xPos,          yPos + fHeight,   0.0, nPosY0 },
        { xPos + fWidth, yPos,             1.0, nPosY1 },
        { xPos + fWidth, yPos + fHeight,   1.0, nPosY0 }
    };

    glUseProgram(this->m_ProgramTexture);

    glBindVertexArray(this->m_TextureVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_TextureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, vertices, GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_TextureVBO);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 4, vertices);
    CHECK_GL_ERROR;
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_TRUE, 0, NULL);
    CHECK_GL_ERROR;
    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);


    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_GL_ERROR;

    glUniformMatrix4fv(this->m_TextureProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nTextureID);
    glUniform1i(this->m_TextureSampleText2D, 0);
    CHECK_GL_ERROR;

    glDrawArrays(GL_TRIANGLES, 0, 6);
    CHECK_GL_ERROR;


    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glUseProgram(0);
}

void COpenGLCore::DrawImage(float xPos, float yPos, float fWidth, float fHeight, const char* szImgFileFullPath, int nReverY/* = 0*/){
    //GLuint nTextureID = this->CreateTexture2DFromBMP(szImgFileFullPath);
    //GLuint nTextureID = this->CreateTexture2DFromPNG(szImgFileFullPath);
    GLuint nTextureID = this->LoadTextureFromFile(szImgFileFullPath);
    this->DrawImage(xPos, yPos, fWidth, fHeight, nTextureID, nReverY);
}

void COpenGLCore::DrawImage(int nViewPointsCnt, float* pVewPoints, GLuint nTextureID, int nReverY/* = 0*/){
    //float xpos = 10.0f, ypos = 20.0f;
    //float w = 400.0f, h = 400.0f;

    glUseProgram(this->m_ProgramTexture);

    glBindVertexArray(this->m_TextureVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_TextureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nViewPointsCnt * 4, pVewPoints, GL_DYNAMIC_DRAW);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * nViewPointsCnt * 4, pVewPoints);
    CHECK_GL_ERROR;
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_TRUE, 0, NULL);
    CHECK_GL_ERROR;
    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);


    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_GL_ERROR;

    glUniformMatrix4fv(this->m_TextureProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nTextureID);
    glUniform1i(this->m_TextureSampleText2D, 0);
    CHECK_GL_ERROR;

    glDrawArrays(GL_TRIANGLES, 0, nViewPointsCnt);
    CHECK_GL_ERROR;


    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;

    glDisable(GL_BLEND);
    //glDisable(GL_CULL_FACE);

    glUseProgram(0);
}

void COpenGLCore::InitTriangles(){
    const GLchar *szTriangleVS[] = {
        "#version 330 core\n"
        "in vec3 vertex;\n"
        //"in vec4 fColor;\n"   V_Color = fColor;
        "uniform mat4 MVP;\n"
        //"out vec4 V_Color;\n"
        "void main() { gl_Position = MVP*vec4(vertex, 1);}"
    };
    GLuint TriangleVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(TriangleVS, 1, szTriangleVS, NULL);
    CompileShader(TriangleVS);

    //"uniform sampler2D text2D;\n"
    //"uniform vec4 fcolor;\n"
    const GLchar *szTriangleFS[] = {
        "#version 330 core\n"
        "precision highp float;\n"
        //"in vec4 V_Color;\n"
        "uniform vec4 fcolor;\n"
        "out vec4 outColor;\n"
        "void main() { outColor = fcolor; }"
    };
    GLuint TriangleFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(TriangleFS, 1, szTriangleFS, NULL);
    CompileShader(TriangleFS);

    this->m_ProgramTriangle = glCreateProgram();
    glAttachShader(this->m_ProgramTriangle, TriangleVS);
    glAttachShader(this->m_ProgramTriangle, TriangleFS);

    LinkProgram(this->m_ProgramTriangle);


    this->m_TriangleVertexID = glGetAttribLocation(this->m_ProgramTriangle, "vertex");
    this->m_TriangleColorID = glGetUniformLocation(this->m_ProgramTriangle, "fcolor");
    this->m_TriangleMVPID = glGetUniformLocation(this->m_ProgramTriangle, "MVP");
    //printf("this->m_CircleVertexID : %d\n", this->m_TriangleVertexID);
    //printf("this->m_CircleColorID : %d\n", this->m_TriangleColorID);
    //printf("this->m_TriangleMVPID : %d\n", this->m_TriangleMVPID);
    CHECK_GL_ERROR;

    glGenVertexArrays(1, &this->m_TriangleVAO);
    glGenBuffers(1, &this->m_TrianglePosVBO);
    //glGenBuffers(1, &this->m_TriangleClsVBO);
}

void COpenGLCore::DrawTriangles(int nTrianglesCnt, float* pPoints, unsigned int nColor){
    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);
    //this->glLineWidth(2);
    glUseProgram(this->m_ProgramTriangle);
    CHECK_GL_ERROR;

    glFrontFace(GL_CCW);

    //glm::mat4 matMVP = this->m_MatPerspectProject * this->m_MatView * this->m_MatModel; // Remember, matrix multiplication is the other way around
    glUniformMatrix4fv(this->m_TriangleMVPID, 1, GL_FALSE, glm::value_ptr(this->m_MatMVPCore));

    CHECK_GL_ERROR;
    glBindVertexArray(this->m_TriangleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_TrianglePosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nTrianglesCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_TriangleVertexID);
    glVertexAttribPointer(this->m_TriangleVertexID, 3, GL_FLOAT, GL_TRUE, 0, NULL);

    //glBindBuffer(GL_ARRAY_BUFFER, this->m_TriangleClsVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecColors.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_TriangleColorID);
    //glVertexAttribPointer(this->m_TriangleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glUniform4fv(this->m_TriangleColorID, 1, glm::value_ptr(fColor));
    CHECK_GL_ERROR;

    glDrawArrays(GL_LINE_LOOP, 0, nTrianglesCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈

    glDisableVertexAttribArray(this->m_TriangleVertexID);
    glUseProgram(0);

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;
}

void COpenGLCore::DrawString(const char* szContent, float fPosX, float fPosY, float fScale, unsigned int nColor){

    this->m_pFontRender->RenderText(szContent, fPosX, fPosY, fScale, COLOR32_INT_TO_FLOAT_RGBA(nColor));
}

void COpenGLCore::InitLine3D(){
    const GLchar *szLine3DVS[] = {
        "#version 330 core\n"
        "in vec3 vertex;\n"
        //"in vec4 fColor;\n"   V_Color = fColor;
        "uniform mat4 MVP;\n"
        //"out vec4 V_Color;\n"
        "void main() { gl_Position = MVP*vec4(vertex, 1);}"
    };
    GLuint Line3DVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Line3DVS, 1, szLine3DVS, NULL);
    CompileShader(Line3DVS);

    //"uniform sampler2D text2D;\n"
    //"uniform vec4 fcolor;\n"
    const GLchar *szLine3DFS[] = {
        "#version 330 core\n"
        "precision highp float;\n"
        //"in vec4 V_Color;\n"
        "uniform vec4 fcolor;\n"
        "out vec4 outColor;\n"
        "void main() { outColor = fcolor; }"
    };
    GLuint Line3DFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Line3DFS, 1, szLine3DFS, NULL);
    CompileShader(Line3DFS);

    this->m_ProgramLine3D = glCreateProgram();
    glAttachShader(this->m_ProgramLine3D, Line3DVS);
    glAttachShader(this->m_ProgramLine3D, Line3DFS);

    LinkProgram(this->m_ProgramLine3D);


    this->m_Line3DVertexID = glGetAttribLocation(this->m_ProgramLine3D, "vertex");
    this->m_Line3DColorID = glGetUniformLocation(this->m_ProgramLine3D, "fcolor");
    this->m_Line3DMVPID = glGetUniformLocation(this->m_ProgramLine3D, "MVP");
    //printf("this->m_Line3DVertexID : %d\n", this->m_Line3DVertexID);
    //printf("this->m_Line3DColorID : %d\n", this->m_Line3DColorID);
    //printf("this->m_Line3DMVPID : %d\n", this->m_Line3DMVPID);
    CHECK_GL_ERROR;

    glGenVertexArrays(1, &this->m_Line3DVAO);
    glGenBuffers(1, &this->m_Line3DPosVBO);
    //glGenBuffers(1, &this->m_Line3DClsVBO);
}
void COpenGLCore::DrawLine3D(float* pPoints, unsigned int nColor){
    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //std::vector<glm::vec3>& vecPoints
    //int nLine3DPtCnt = vecPoints.size();//static_cast<int>(vecPoints.size());

    //this->glLineWidth(2);
    glUseProgram(this->m_ProgramLine3D);
    CHECK_GL_ERROR;

    this->m_MatLine3DMVP = this->m_MatPerspectProject * this->m_MatView * this->m_MatModel; // Remember, matrix multiplication is the other way around
    glUniformMatrix4fv(this->m_Line3DMVPID, 1, GL_FALSE, glm::value_ptr(this->m_MatMVPCore));

    glUniform4fv(this->m_Line3DColorID, 1, glm::value_ptr(fColor));

    CHECK_GL_ERROR;
    glBindVertexArray(this->m_Line3DVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_Line3DPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_Line3DVertexID);
    glVertexAttribPointer(this->m_Line3DVertexID, 3, GL_FLOAT, GL_TRUE, 0, NULL);

    //glBindBuffer(GL_ARRAY_BUFFER, this->m_TriangleClsVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecColors.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_TriangleColorID);
    //glVertexAttribPointer(this->m_TriangleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    CHECK_GL_ERROR;

    glDrawArrays(GL_LINES, 0, 2); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈

    glDisableVertexAttribArray(this->m_Line3DVertexID);
    glUseProgram(0);

}
void COpenGLCore::DrawLines3D(int nPointCnt, float* pPoints, unsigned int nColor){
    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //std::vector<glm::vec3>& vecPoints,
    //int nLine3DPtCnt = vecPoints.size();//static_cast<int>(vecPoints.size());
    //this->glLineWidth(2);
    glUseProgram(this->m_ProgramLine3D);
    CHECK_GL_ERROR;

    this->m_MatLine3DMVP = this->m_MatPerspectProject * this->m_MatView * this->m_MatModel; // Remember, matrix multiplication is the other way around
    glUniformMatrix4fv(this->m_Line3DMVPID, 1, GL_FALSE, glm::value_ptr(this->m_MatMVPCore));

    glUniform4fv(this->m_Line3DColorID, 1, glm::value_ptr(fColor));

    CHECK_GL_ERROR;
    glBindVertexArray(this->m_Line3DVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_Line3DPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_Line3DVertexID);
    glVertexAttribPointer(this->m_Line3DVertexID, 3, GL_FLOAT, GL_TRUE, 0, NULL);

    //glBindBuffer(GL_ARRAY_BUFFER, this->m_TriangleClsVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecColors.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_TriangleColorID);
    //glVertexAttribPointer(this->m_TriangleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);


    CHECK_GL_ERROR;

    glDrawArrays(GL_LINE_LOOP, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈

    glDisableVertexAttribArray(this->m_Line3DVertexID);
    glUseProgram(0);

}

void COpenGLCore::InitImage3D(){
    const GLchar *szTexture3DVS[] = {
        "#version 330 core\n"
        "in vec3 vertex;\n"
        "in vec2 texCoord;\n"
        //"in vec4 fColor;\n"   V_Color = fColor;
        "uniform mat4 MVP;\n"
        "out vec2 V_texCoord;\n"
        "void main() { gl_Position = MVP*vec4(vertex, 1); V_texCoord = texCoord;}"
    };
    GLuint Texture3DVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Texture3DVS, 1, szTexture3DVS, NULL);
    CompileShader(Texture3DVS);

    //"uniform sampler2D text2D;\n"
    //"uniform vec4 fcolor;\n"
    const GLchar *szTexture3DFS[] = {
        "#version 330 core\n"
        "precision highp float;\n"
        "uniform sampler2D text2D;\n"
        "in vec2 V_texCoord;\n"
        "out vec4 outColor;\n"
        "void main() { outColor = texture(text2D, V_texCoord);}"
    };
    GLuint Texture3DFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(Texture3DFS, 1, szTexture3DFS, NULL);
    CompileShader(Texture3DFS);

    this->m_ProgramTexture3D = glCreateProgram();
    glAttachShader(this->m_ProgramTexture3D, Texture3DVS);
    glAttachShader(this->m_ProgramTexture3D, Texture3DFS);

    LinkProgram(this->m_ProgramTexture3D);

    this->m_Texture3DVertexID = glGetAttribLocation(this->m_ProgramTexture3D, "vertex");
    this->m_Texture3DCoordID = glGetAttribLocation(this->m_ProgramTexture3D, "texCoord");
    this->m_Texture3DMVPID = glGetUniformLocation(this->m_ProgramTexture3D, "MVP");
    this->m_TextureSampleText3D = glGetUniformLocation(this->m_ProgramTexture, "text2D");
    //printf("this->m_Texture3DVertexID : %d\n", this->m_Texture3DVertexID);
    //printf("this->m_Texture3DCoordID : %d\n", this->m_Texture3DCoordID);
    //printf("this->m_Texture3DMVPID : %d\n", this->m_Texture3DMVPID);
    CHECK_GL_ERROR;

    glGenVertexArrays(1, &this->m_Texture3DVAO);
    glGenBuffers(1, &this->m_Texture3DPosVBO);
    glGenBuffers(1, &this->m_Texture3DCoordVBO);
}
void COpenGLCore::DrawImage3D( float* pPoints, const char* szImgFileFullPath, int nReverY/* = 0*/){
    //std::vector<glm::vec3>& vecPoints
    //glm::vec4 fColor = unsigned int_INT_TO_FLOAT_RGBA(nColor);
    //int nTexture3DPtCnt = vecPoints.size();//static_cast<int>(vecPoints.size());
    std::vector<glm::vec2> vecTexCoords;
    if(nReverY == 1){
        vecTexCoords.push_back(glm::vec2(0.0f, 0.0f));
        vecTexCoords.push_back(glm::vec2(0.0f, 1.0f));
        vecTexCoords.push_back(glm::vec2(1.0f, 1.0f));
        vecTexCoords.push_back(glm::vec2(1.0f, 0.0f));
    }
    else{
        vecTexCoords.push_back(glm::vec2(0.0f, 1.0f));
        vecTexCoords.push_back(glm::vec2(0.0f, 0.0f));
        vecTexCoords.push_back(glm::vec2(1.0f, 0.0f));
        vecTexCoords.push_back(glm::vec2(1.0f, 1.0f));
    }
    //this->glTextureWidth(2);
    glUseProgram(this->m_ProgramTexture3D);
    CHECK_GL_ERROR;

    //this->m_MatTexture3DMVP = this->m_MatPerspectProject * this->m_MatView * this->m_MatModel; // Remember, matrix multiplication is the other way around
    glUniformMatrix4fv(this->m_Texture3DMVPID, 1, GL_FALSE, glm::value_ptr(this->m_MatMVPCore));

    GLuint nTextureID  = this->LoadTextureFromFile(szImgFileFullPath);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nTextureID);
    glUniform1i(this->m_TextureSampleText3D, 0);

    CHECK_GL_ERROR;
    glBindVertexArray(this->m_Texture3DVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_Texture3DPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_Texture3DVertexID);
    glVertexAttribPointer(this->m_Texture3DVertexID, 3, GL_FLOAT, GL_TRUE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, this->m_Texture3DCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, vecTexCoords.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_Texture3DCoordID);
    glVertexAttribPointer(this->m_Texture3DCoordID, 2, GL_FLOAT, GL_TRUE, 0, NULL);


    CHECK_GL_ERROR;

    glDrawArrays(GL_QUADS, 0, 4); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈

    glDisableVertexAttribArray(this->m_Texture3DVertexID);
    glDisableVertexAttribArray(this->m_Texture3DCoordID);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

int COpenGLCore::InitBase()
{
    bool aaa = this->initializeOpenGLFunctions();
    printf("core : %d\n", aaa);
    // Create line/rect shaders
    const GLchar *lineRectVS[] = {
        "#version 150 core\n"
        "in vec3 vertex;"
        "in vec4 color;"
        "out vec4 fcolor;"
        "void main() { gl_Position = vec4(vertex, 1); fcolor = color; }"
    };
    m_LineRectVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_LineRectVS, 1, lineRectVS, NULL);
    CompileShader(m_LineRectVS);

    const GLchar *lineRectFS[] = {
        "#version 150 core\n"
        "precision highp float;"
        "in vec4 fcolor;"
        "out vec4 outColor;"
        "void main() { outColor = fcolor; }"
    };
    m_LineRectFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_LineRectFS, 1, lineRectFS, NULL);
    CompileShader(m_LineRectFS);

    m_LineRectProgram = glCreateProgram();
    glAttachShader(m_LineRectProgram, m_LineRectVS);
    glAttachShader(m_LineRectProgram, m_LineRectFS);
    glBindAttribLocation(m_LineRectProgram, 0, "vertex");
    glBindAttribLocation(m_LineRectProgram, 1, "color");
    LinkProgram(m_LineRectProgram);

    // Create line/rect vertex buffer
    const GLfloat lineRectInitVertices[] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0 };
    const unsigned int lineRectInitColors[] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
    glGenVertexArrays(1, &m_LineRectVArray);
    glBindVertexArray(m_LineRectVArray);
    glGenBuffers(1, &m_LineRectVertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineRectVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineRectInitVertices), lineRectInitVertices, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &m_LineRectColors);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineRectColors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineRectInitColors), lineRectInitColors, GL_DYNAMIC_DRAW);

    // Create triangles shaders
    const GLchar *triVS[] = {
        "#version 150 core\n"
        "uniform vec2 offset;"
        "uniform vec2 wndSize;"
        "in vec2 vertex;"
        "in vec4 color;"
        "out vec4 fcolor;"
        "void main() { gl_Position = vec4(2.0*(vertex.x+offset.x-0.5)/wndSize.x - 1.0, 1.0 - 2.0*(vertex.y+offset.y-0.5)/wndSize.y, 0, 1); fcolor = color; }"
    };
    m_TriVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_TriVS, 1, triVS, NULL);
    CompileShader(m_TriVS);

    const GLchar *triUniVS[] = {
        "#version 150 core\n"
        "uniform vec2 offset;"
        "uniform vec2 wndSize;"
        "uniform vec4 color;"
        "in vec2 vertex;"
        "out vec4 fcolor;"
        "void main() { gl_Position = vec4(2.0*(vertex.x+offset.x-0.5)/wndSize.x - 1.0, 1.0 - 2.0*(vertex.y+offset.y-0.5)/wndSize.y, 0, 1); fcolor = color; }"
    };
    m_TriUniVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_TriUniVS, 1, triUniVS, NULL);
    CompileShader(m_TriUniVS);

    m_TriFS = m_TriUniFS = m_LineRectFS;

    m_TriProgram = glCreateProgram();
    glAttachShader(m_TriProgram, m_TriVS);
    glAttachShader(m_TriProgram, m_TriFS);
    glBindAttribLocation(m_TriProgram, 0, "vertex");
    glBindAttribLocation(m_TriProgram, 1, "color");
    LinkProgram(m_TriProgram);
    m_TriLocationOffset = glGetUniformLocation(m_TriProgram, "offset");
    m_TriLocationWndSize = glGetUniformLocation(m_TriProgram, "wndSize");

    m_TriUniProgram = glCreateProgram();
    glAttachShader(m_TriUniProgram, m_TriUniVS);
    glAttachShader(m_TriUniProgram, m_TriUniFS);
    glBindAttribLocation(m_TriUniProgram, 0, "vertex");
    glBindAttribLocation(m_TriUniProgram, 1, "color");
    LinkProgram(m_TriUniProgram);
    m_TriUniLocationOffset = glGetUniformLocation(m_TriUniProgram, "offset");
    m_TriUniLocationWndSize = glGetUniformLocation(m_TriUniProgram, "wndSize");
    m_TriUniLocationColor = glGetUniformLocation(m_TriUniProgram, "color");

    const GLchar *triTexFS[] = {
        "#version 150 core\n"
        "precision highp float;"
        "uniform sampler2D tex;"
        "in vec2 fuv;"
        "in vec4 fcolor;"
        "out vec4 outColor;"
// texture2D is deprecated and replaced by texture with GLSL 3.30 but it seems
// that on Mac Lion backward compatibility is not ensured.
#if defined(ANT_OSX) && (MAC_OS_X_VERSION_MAX_ALLOWED >= 1070)
        "void main() { outColor.rgb = fcolor.bgr; outColor.a = fcolor.a * texture(tex, fuv).r; }"
#else
        "void main() { outColor.rgb = fcolor.bgr; outColor.a = fcolor.a * texture2D(tex, fuv).r; }"
#endif
    };
    m_TriTexFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_TriTexFS, 1, triTexFS, NULL);
    CompileShader(m_TriTexFS);

    const GLchar *triTexVS[] = {
        "#version 150 core\n"
        "uniform vec2 offset;"
        "uniform vec2 wndSize;"
        "in vec2 vertex;"
        "in vec2 uv;"
        "in vec4 color;"
        "out vec2 fuv;"
        "out vec4 fcolor;"
        "void main() { gl_Position = vec4(2.0*(vertex.x+offset.x-0.5)/wndSize.x - 1.0, 1.0 - 2.0*(vertex.y+offset.y-0.5)/wndSize.y, 0, 1); fuv = uv; fcolor = color; }"
    };
    m_TriTexVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_TriTexVS, 1, triTexVS, NULL);
    CompileShader(m_TriTexVS);

    const GLchar *triTexUniVS[] = {
        "#version 150 core\n"
        "uniform vec2 offset;"
        "uniform vec2 wndSize;"
        "uniform vec4 color;"
        "in vec2 vertex;"
        "in vec2 uv;"
        "out vec4 fcolor;"
        "out vec2 fuv;"
        "void main() { gl_Position = vec4(2.0*(vertex.x+offset.x-0.5)/wndSize.x - 1.0, 1.0 - 2.0*(vertex.y+offset.y-0.5)/wndSize.y, 0, 1); fuv = uv; fcolor = color; }"
    };
    m_TriTexUniVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_TriTexUniVS, 1, triTexUniVS, NULL);
    CompileShader(m_TriTexUniVS);

    m_TriTexUniFS = m_TriTexFS;

    m_TriTexProgram = glCreateProgram();
    glAttachShader(m_TriTexProgram, m_TriTexVS);
    glAttachShader(m_TriTexProgram, m_TriTexFS);
    glBindAttribLocation(m_TriTexProgram, 0, "vertex");
    glBindAttribLocation(m_TriTexProgram, 1, "uv");
    glBindAttribLocation(m_TriTexProgram, 2, "color");
    LinkProgram(m_TriTexProgram);
    m_TriTexLocationOffset = glGetUniformLocation(m_TriTexProgram, "offset");
    m_TriTexLocationWndSize = glGetUniformLocation(m_TriTexProgram, "wndSize");
    m_TriTexLocationTexture = glGetUniformLocation(m_TriTexProgram, "tex");

    m_TriTexUniProgram = glCreateProgram();
    glAttachShader(m_TriTexUniProgram, m_TriTexUniVS);
    glAttachShader(m_TriTexUniProgram, m_TriTexUniFS);
    glBindAttribLocation(m_TriTexUniProgram, 0, "vertex");
    glBindAttribLocation(m_TriTexUniProgram, 1, "uv");
    glBindAttribLocation(m_TriTexUniProgram, 2, "color");
    LinkProgram(m_TriTexUniProgram);
    m_TriTexUniLocationOffset = glGetUniformLocation(m_TriTexUniProgram, "offset");
    m_TriTexUniLocationWndSize = glGetUniformLocation(m_TriTexUniProgram, "wndSize");
    m_TriTexUniLocationColor = glGetUniformLocation(m_TriTexUniProgram, "color");
    m_TriTexUniLocationTexture = glGetUniformLocation(m_TriTexUniProgram, "tex");

    // Create tri vertex buffer
    glGenVertexArrays(1, &m_TriVArray);
    glGenBuffers(1, &m_TriVertices);
    glGenBuffers(1, &m_TriUVs);
    glGenBuffers(1, &m_TriColors);
    ResizeTriBuffers(16384); // set initial size

    {
        this->InitDraw2Dd();
        this->InitEllipse();
        this->InitCircle();
        this->InitImage();
        this->InitTriangles();
        this->InitLine3D();
        this->InitImage3D();
        this->InitLinesElement();
        this->InitLinesElementEx();

        double arrGeoMinMax1[4] = {-180.0, -90.0, 180.0, 90.0};//左下角 - 右上角
        //double arrGeoMinMax1[4] = {-90.0, -45.0, 90.0, 45.0};//左下角 - 右上角
        //double arrGeoMinMax1[4] = {73.0, 45.0 - 50.0, 73.0 + 65.0, 45.0};//左下角 - 右上角
        //double arrGeoMinMax1[4] = {110.22, 15.35, 115.1, 17.56};//左下角 - 右上角
        memcpy(this->m_arrGeoMinMax, arrGeoMinMax1, sizeof(double)*4);
    }

    CHECK_GL_ERROR;
    return 1;
}

//  ---------------------------------------------------------------------------
//  ---------------------------------------------------------------------------

static inline float ToNormScreenX(float x, int wndWidth)
{
    return 2.0f*((float)x-0.5f)/wndWidth - 1.0f;
}

static inline float ToNormScreenY(float y, int wndHeight)
{
    return 1.0f - 2.0f*((float)y-0.5f)/wndHeight;
}

//  ---------------------------------------------------------------------------

void COpenGLCore::DrawLine(int _X0, int _Y0, int _X1, int _Y1, unsigned int _Color0, unsigned int _Color1, bool _AntiAliased){
    if(_X0 > this->m_nWndWidth || _X0 <0 || _Y0 > this->m_nWndHeight || _Y0 < 0
            || _X1 > this->m_nWndWidth || _X1 <0 || _Y1 > this->m_nWndHeight || _Y1 < 0){
        return;
    }
    float fX0 = (float)_X0;
    float fX1 = (float)_X1;
    float fY0 = (float)_Y0;
    float fY1 = (float)_Y1;

    fX0 = (fX0*2.0f - ((float)this->m_nWndWidth))/ ((float)this->m_nWndWidth);
    fX1 = (fX1*2.0f - ((float)this->m_nWndWidth))/ ((float)this->m_nWndWidth);
    fY0 = (fY0*2.0f - ((float)this->m_nWndHeight))/ ((float)this->m_nWndHeight);
    fY1 = (fY1*2.0f - ((float)this->m_nWndHeight))/ ((float)this->m_nWndHeight);

    this->DrawLine(fX0, fY0, fX1, fY1, _Color0, _Color1, _AntiAliased);
}
void COpenGLCore::DrawLine(GLfloat ffx , GLfloat ffy, GLfloat ffx1, GLfloat ffy1, unsigned int _Color0, unsigned int _Color1, bool _AntiAliased)
{
    CHECK_GL_ERROR;


    //GLint prevCullFaceMode, prevFrontFace;
    //glGetIntegerv(GL_CULL_FACE_MODE, &prevCullFaceMode);
    //glGetIntegerv(GL_FRONT_FACE, &prevFrontFace);
    //GLboolean prevCullEnable = glIsEnabled(GL_CULL_FACE);


    //const GLfloat dx = +0.0f;
    const GLfloat dx = 0;
    //GLfloat dy = -0.2f;
    const GLfloat dy = -0.5f;
    if( _AntiAliased )
        glEnable(GL_LINE_SMOOTH);
    else
        glDisable(GL_LINE_SMOOTH);

    glBindVertexArray(m_LineRectVArray);

    //GLfloat ffx = 0.9f, ffy = 0.7f, ffx1 = 0.8f, ffy1=0.6f;
    //ffx = 0.9f; ffy = 0.7f; ffx1 = 0.8f; ffy1=0.6f;
    GLfloat x0 = ffx;//ToNormScreenX(_X0+dx + m_OffsetX, m_WndWidth);
    GLfloat y0 = ffy;//ToNormScreenY(_Y0+dy + m_OffsetY, m_WndHeight);
    GLfloat x1 = ffx1;//ToNormScreenX(_X1+dx + m_OffsetX, m_WndWidth);
    GLfloat y1 = ffy1;//ToNormScreenY(_Y1+dy + m_OffsetY, m_WndHeight);
    GLfloat vertices[] = { x0,y0,0,  x1,y1,0 };
    glBindBuffer(GL_ARRAY_BUFFER, m_LineRectVertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    glEnableVertexAttribArray(0);

    unsigned int colors[] = { _Color0, _Color1 };
    glBindBuffer(GL_ARRAY_BUFFER, m_LineRectColors);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
    glVertexAttribPointer(1, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, 0, NULL);
    glEnableVertexAttribArray(1);

    glUseProgram(m_LineRectProgram);
    glDrawArrays(GL_LINES, 0, 2);

    glUseProgram(0);
    if( _AntiAliased )
        glDisable(GL_LINE_SMOOTH);

    CHECK_GL_ERROR;
}

//  ---------------------------------------------------------------------------

void COpenGLCore::DrawRect(int _X0, int _Y0, int _X1, int _Y1, unsigned int _Color00, unsigned int _Color10, unsigned int _Color01, unsigned int _Color11)
{
    CHECK_GL_ERROR;

    if(_X0 > this->m_nWndWidth || _X0 <0 || _Y0 > this->m_nWndHeight || _Y0 < 0
            || _X1 > this->m_nWndWidth || _X1 <0 || _Y1 > this->m_nWndHeight || _Y1 < 0){
        return;
    }
    float fX0 = (float)_X0;
    float fX1 = (float)_X1;
    float fY0 = (float)_Y0;
    float fY1 = (float)_Y1;

    fX0 = (fX0*2.0f - ((float)this->m_nWndWidth))/ ((float)this->m_nWndWidth);
    fX1 = (fX1*2.0f - ((float)this->m_nWndWidth))/ ((float)this->m_nWndWidth);
    fY0 = (fY0*2.0f - ((float)this->m_nWndWidth))/ ((float)this->m_nWndWidth);
    fY1 = (fY1*2.0f - ((float)this->m_nWndWidth))/ ((float)this->m_nWndWidth);

    // border adjustment
    if(_X0<_X1)
        ++_X1;
    else if(_X0>_X1)
        ++_X0;
    if(_Y0<_Y1)
        --_Y0;
    else if(_Y0>_Y1)
        --_Y1;

    glBindVertexArray(m_LineRectVArray);

    GLfloat x0 = fX0;//ToNormScreenX((float)_X0 + m_OffsetX, m_WndWidth);
    GLfloat y0 = fY0;//ToNormScreenY((float)_Y0 + m_OffsetY, m_WndHeight);
    GLfloat x1 = fX1;//ToNormScreenX((float)_X1 + m_OffsetX, m_WndWidth);
    GLfloat y1 = fY1;//ToNormScreenY((float)_Y1 + m_OffsetY, m_WndHeight);
    GLfloat vertices[] = { x0,y0,0, x1,y0,0, x0,y1,0, x1,y1,0 };
    glBindBuffer(GL_ARRAY_BUFFER, m_LineRectVertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    glEnableVertexAttribArray(0);

    GLuint colors[] = { _Color00, _Color10, _Color01, _Color11 };
    glBindBuffer(GL_ARRAY_BUFFER, m_LineRectColors);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
    glVertexAttribPointer(1, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, 0, NULL);
    glEnableVertexAttribArray(1);

    glUseProgram(m_LineRectProgram);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    CHECK_GL_ERROR;
}

//  ---------------------------------------------------------------------------

void COpenGLCore::DrawTriangles(int _NumTriangles, int *_Vertices, unsigned int *_Colors, Cull _CullMode)
{

    const GLfloat dx = +0.0f;
    const GLfloat dy = +0.0f;

    // Backup states
    GLint prevCullFaceMode, prevFrontFace;
    glGetIntegerv(GL_CULL_FACE_MODE, &prevCullFaceMode);
    glGetIntegerv(GL_FRONT_FACE, &prevFrontFace);
    GLboolean prevCullEnable = glIsEnabled(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    if( _CullMode==CULL_CW )
        glFrontFace(GL_CCW);
    else if( _CullMode==CULL_CCW )
        glFrontFace(GL_CW);
    else
        glDisable(GL_CULL_FACE);

    glUseProgram(m_TriProgram);
    glBindVertexArray(m_TriVArray);
    //glUniform2f(m_TriLocationOffset, (float)m_OffsetX+dx, (float)m_OffsetY+dy);
    glUniform2f(m_TriLocationOffset, (float)0, (float)0);
    glUniform2f(m_TriLocationWndSize, (float)this->m_nWndWidth, (float)this->m_nWndHeight);
    glDisableVertexAttribArray(2);

    size_t numVerts = 3*_NumTriangles;
    if( numVerts > m_TriBufferSize )
        ResizeTriBuffers(numVerts + 2048);

    glBindBuffer(GL_ARRAY_BUFFER, m_TriVertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVerts*2*sizeof(int), _Vertices);
    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, m_TriColors);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numVerts*sizeof(unsigned int), _Colors);
    glVertexAttribPointer(1, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, 0, NULL);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numVerts);

    glUseProgram(0);
    // Reset states
    //glCullFace(prevCullFaceMode);
    //glFrontFace(prevFrontFace);
    //if( prevCullEnable )
    //    glEnable(GL_CULL_FACE);
    //else
    //    glDisable(GL_CULL_FACE);

    CHECK_GL_ERROR;
}

void COpenGLCore::InitLinesElement(){
    //
    const GLchar *szLinesEleVS[] = {
        "#version 410 core\n"
        //"precision highp float;\n"
        "attribute dvec3 vertex;\n"
        "uniform dvec4 viewgeo;\n"
        //"uniform double viewgeo_x;\n"
        //"uniform double viewgeo_y;\n"
        //"uniform double viewgeo_z;\n"
        //"uniform double viewgeo_w;\n"
        "uniform mat4 projection;\n"
        "varying dvec3 V_vertex;\n"
        "void main() {\n"
        "double xView = (vertex.x-viewgeo.x) * viewgeo.z;\n"
        "double yView = (vertex.y-viewgeo.y) * viewgeo.w;\n"
        "V_vertex = vertex;\n"
        //"double xView = ((vertex.x-viewgeo_x) * viewgeo_z);\n"
        //"double yView = ((vertex.y-viewgeo_y) * viewgeo_w);\n"
        " gl_Position = projection * vec4(xView, yView, 0.0, 1.0);}"
    };
    GLuint nLinesEleVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(nLinesEleVS, 1, szLinesEleVS, NULL);
    CompileShader(nLinesEleVS);

    const GLchar *szLinesEleFS[] = {
        "#version 410 core\n"
        //"precision highp float;\n"
        "uniform vec4 V_color;\n"
        //"uniform int V_enableSelect;\n"
        "uniform dvec4 V_geoSelectRange;\n"
        "varying dvec3 V_vertex;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "outColor = V_color;}\n"
        //"if (V_geoSelectRange.x <= V_vertex.x && V_geoSelectRange.z >= V_vertex.x &&\n"
        //"    V_geoSelectRange.y <= V_vertex.y && V_geoSelectRange.w >= V_vertex.y) {\n"
        //"    outColor = vec4(1.0, 0.0, 0.0, 1.0); }\n"
        //"else {outColor = V_color;} }\n"
    };

    GLuint nLinesEleFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(nLinesEleFS, 1, szLinesEleFS, NULL);
    CompileShader(nLinesEleFS);

    this->m_ProgramLinesElement = glCreateProgram();
    glAttachShader(this->m_ProgramLinesElement, nLinesEleVS);
    glAttachShader(this->m_ProgramLinesElement, nLinesEleFS);
    //glBindAttribLocation(this->m_ProgramCircle, 0, "vertex");
    //glBindAttribLocation(this->m_ProgramCircle, 1, "fcolor");
    //
    //glBindAttribLocation(this->m_ProgramCircle, 2, "V_color");
    //CHECK_GL_ERROR;

    CHECK_GL_ERROR;
    LinkProgram(this->m_ProgramLinesElement);

    //CHECK_GL_ERROR;
    this->m_LinesElementProjectID = glGetUniformLocation(this->m_ProgramLinesElement, "projection");
    this->m_LinesElementVertexID = glGetAttribLocation(this->m_ProgramLinesElement, "vertex");
    this->m_LinesElementViewGeoID = glGetUniformLocation(this->m_ProgramLinesElement, "viewgeo");
    //this->m_LinesElementViewGeoIDx = glGetUniformLocation(this->m_ProgramLinesElement, "viewgeo_x");
    //this->m_LinesElementViewGeoIDy = glGetUniformLocation(this->m_ProgramLinesElement, "viewgeo_y");
    //this->m_LinesElementViewGeoIDz = glGetUniformLocation(this->m_ProgramLinesElement, "viewgeo_z");
    //this->m_LinesElementViewGeoIDw = glGetUniformLocation(this->m_ProgramLinesElement, "viewgeo_w");
    this->m_LinesElementGeoSelectRangeID = glGetUniformLocation(this->m_ProgramLinesElement, "V_geoSelectRange");
    //this->m_LinesElementEnableSelecID = glGetUniformLocation(this->m_ProgramLinesElement, "V_enableSelect");
    //this->m_CircleColorID = glGetAttribLocation(this->m_ProgramCircle, "fcolor");
    this->m_LinesElementColorID = glGetUniformLocation(this->m_ProgramLinesElement, "V_color");
    //printf("this->m_CircleProjectID : %d\n", this->m_CircleProjectID);
    //printf("this->m_CircleVertexID : %d\n", this->m_CircleVertexID);
    //printf("this->m_CircleColorID : %d\n", this->m_CircleColorID);
    CHECK_GL_ERROR;

    glGenVertexArrays(1, &this->m_LinesElementVAO);
    glGenBuffers(1, &this->m_LinesElementPosVBO);
    glGenBuffers(1, &this->m_LinesElementPosVEO);
    //glGenBuffers(1, &this->m_CircleClsVBO);
}

void COpenGLCore::DrawLinesElement(int nPointCnt, double* pPoints, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    ////glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    ////glEnable( GL_LINE_SMOOTH );
    ////glEnable(GL_POLYGON_SMOOTH);
    ////glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glLineWidth(fLineSize);
    ////glPolygonMode(GL_FRONT, GL_LINE);


    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramLinesElement);

    glUniformMatrix4fv(this->m_LinesElementProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_LinesElementColorID, 1, glm::value_ptr(fColor));
    glm::vec<4, double> fViewGwo(this->t_geo_minmax.fGeoLeftBottomX, this->t_geo_minmax.fGeoLeftBottomY, this->m_fView2GeoScaleH, this->m_fView2GeoScaleV);
    glUniform4dv(this->m_LinesElementViewGeoID, 1, glm::value_ptr(fViewGwo));
    //glUniform1d(this->m_LinesElementViewGeoIDx, this->t_geo_minmax.fGeoLeftBottomX);
    //glUniform1d(this->m_LinesElementViewGeoIDy, this->t_geo_minmax.fGeoLeftBottomY);
    //glUniform1d(this->m_LinesElementViewGeoIDz, this->m_fView2GeoScaleH);
    //glUniform1d(this->m_LinesElementViewGeoIDw, this->m_fView2GeoScaleV);
    //m_LinesElementEnableSelecID
    //glUniform1i(this->m_LinesElementEnableSelecID, this->m_nMouseWorkStatus);
    glUniform4dv(this->m_LinesElementGeoSelectRangeID, 1, this->m_arrMouseSelectPoints);
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_LinesElementVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nPointCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementVertexID);
    glVertexAttribLPointer(this->m_LinesElementVertexID, 3, GL_DOUBLE, 0, NULL);
    //glVertexAttribPointer(this->m_LinesElementVertexID, 3, GL_DOUBLE, GL_FALSE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementPosVEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCnt, pPointsElement, GL_DYNAMIC_DRAW);

    glDrawElements(GL_LINES, nPointElementCnt, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_LinesElementVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glLineWidth(1.0f);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

}

void COpenGLCore::DrawLinesLoopElement(int nPointCnt, double* pPoints, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    ////glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    ////glEnable( GL_LINE_SMOOTH );
    ////glEnable(GL_POLYGON_SMOOTH);
    ////glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glLineWidth(fLineSize);
    ////glPolygonMode(GL_FRONT, GL_LINE);


    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramLinesElement);

    glUniformMatrix4fv(this->m_LinesElementProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_LinesElementColorID, 1, glm::value_ptr(fColor));
    glm::vec<4, double> fViewGwo(this->t_geo_minmax.fGeoLeftBottomX, this->t_geo_minmax.fGeoLeftBottomY, this->m_fView2GeoScaleH, this->m_fView2GeoScaleV);
    glUniform4dv(this->m_LinesElementViewGeoID, 1, glm::value_ptr(fViewGwo));
    //glUniform1d(this->m_LinesElementViewGeoIDx, this->t_geo_minmax.fGeoLeftBottomX);
    //glUniform1d(this->m_LinesElementViewGeoIDy, this->t_geo_minmax.fGeoLeftBottomY);
    //glUniform1d(this->m_LinesElementViewGeoIDz, this->m_fView2GeoScaleH);
    //glUniform1d(this->m_LinesElementViewGeoIDw, this->m_fView2GeoScaleV);
    //m_LinesElementEnableSelecID
    //glUniform1i(this->m_LinesElementEnableSelecID, this->m_nMouseWorkStatus);
    glUniform4dv(this->m_LinesElementGeoSelectRangeID, 1, this->m_arrMouseSelectPoints);
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_LinesElementVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nPointCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementVertexID);
    glVertexAttribLPointer(this->m_LinesElementVertexID, 3, GL_DOUBLE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementPosVEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCnt, pPointsElement, GL_DYNAMIC_DRAW);

    glDrawElements(GL_LINE_LOOP, nPointElementCnt, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_LinesElementVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glLineWidth(1.0f);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

}

void COpenGLCore::DrawPointsElement(int nPointCnt, double* pPoints, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fPointSize/* = 5.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    ////glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    ////glEnable( GL_LINE_SMOOTH );
    ////glEnable(GL_POLYGON_SMOOTH);
    ////glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glLineWidth(fLineSize);
    ////glPolygonMode(GL_FRONT, GL_LINE);

    glEnable(GL_POINT_SMOOTH);
    glPointSize(fPointSize);

    glUseProgram(this->m_ProgramLinesElement);

    glUniformMatrix4fv(this->m_LinesElementProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_LinesElementColorID, 1, glm::value_ptr(fColor));
    glm::vec<4, double> fViewGwo(this->t_geo_minmax.fGeoLeftBottomX, this->t_geo_minmax.fGeoLeftBottomY, this->m_fView2GeoScaleH, this->m_fView2GeoScaleV);
    glUniform4dv(this->m_LinesElementViewGeoID, 1, glm::value_ptr(fViewGwo));
    //glUniform1d(this->m_LinesElementViewGeoIDx, this->t_geo_minmax.fGeoLeftBottomX);
    //glUniform1d(this->m_LinesElementViewGeoIDy, this->t_geo_minmax.fGeoLeftBottomY);
    //glUniform1d(this->m_LinesElementViewGeoIDz, this->m_fView2GeoScaleH);
    //glUniform1d(this->m_LinesElementViewGeoIDw, this->m_fView2GeoScaleV);
    //glUniform1i(this->m_LinesElementEnableSelecID, this->m_nMouseWorkStatus);
    glUniform4dv(this->m_LinesElementGeoSelectRangeID, 1, this->m_arrMouseSelectPoints);
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_LinesElementVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nPointCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementVertexID);
    glVertexAttribLPointer(this->m_LinesElementVertexID, 3, GL_DOUBLE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementPosVEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCnt, pPointsElement, GL_DYNAMIC_DRAW);

    glDrawElements(GL_POINTS, nPointElementCnt, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_LinesElementVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_POINT_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glPointSize(1.0f);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

}


void COpenGLCore::InitLinesElementEx(){
    //
    const GLchar *szLinesEleVS[] = {
        "#version 410 core\n"
        //"precision highp float;\n"
        //"#extension GL_ARB_vertex_attrib_64bit : enable\n"
        //"#extension GL_ARB_gpu_shader_fp64 : enable\n"
        "attribute dvec3 vertex;\n"
        "attribute float vertexFlag;\n"
        "uniform dvec4 viewgeo;\n"
        //"uniform double viewgeo_x;\n"
        //"uniform double viewgeo_y;\n"
        //"uniform double viewgeo_z;\n"
        //"uniform double viewgeo_w;\n"
        "uniform mat4 projection;\n"
        //"varying vec3 V_vertex;\n"
        "varying float V_vertexFlag;\n"
        "void main() {\n"
        "V_vertexFlag = vertexFlag;\n"
        //"float xView = vertex.x;\n"
        //"float yView = vertex.y;\n"
        //"double xView = ((vertex.x-viewgeo_x) * viewgeo_z);\n"
        //"double yView = ((vertex.y-viewgeo_y) * viewgeo_w);\n"
        "double xView = (vertex.x-viewgeo.x) * viewgeo.z;\n"
        "double yView = (vertex.y-viewgeo.y) * viewgeo.w;\n"
        //"V_vertex = vertex;\n"
        " gl_Position = projection * vec4(xView, yView, 0.0, 1.0);}"
    };
    GLuint nLinesEleVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(nLinesEleVS, 1, szLinesEleVS, NULL);
    CompileShader(nLinesEleVS);

    const GLchar *szLinesEleFS[] = {
        "#version 410 core\n"
        //"#extension GL_ARB_vertex_attrib_64bit : enable\n"
        //"#extension GL_ARB_gpu_shader_fp64 : enable\n"
        //"precision highp float;\n"
        "uniform vec4 V_color;\n"
        //"uniform int V_enableSelect;\n"
        //"uniform vec4 V_geoSelectRange;\n"
        //"varying dvec3 V_vertex;\n"
        "varying float V_vertexFlag;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        //"if (0.0001 >= abs(V_vertexFlag - 2.0)) {\n"
        "if (V_vertexFlag > 3.0) {\n"
        "    outColor = vec4(1.0, 1.0, 0.0, 1.0); }\n"
        "else if (V_vertexFlag > 2.0) {\n"
        "    outColor = vec4(1.0, 0.0, 0.0, 1.0); }\n"
        "else if (V_vertexFlag > 1.0) {\n"
        "    outColor = vec4(0.0, 1.0, 1.0, 1.0); }\n"
        "else if (V_vertexFlag > 0.0) {\n"
        "    outColor = vec4(0.0, 0.0, 1.0, 1.0); }\n"
        "else {outColor = V_color;} }\n"
    };

    GLuint nLinesEleFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(nLinesEleFS, 1, szLinesEleFS, NULL);
    CompileShader(nLinesEleFS);

    this->m_ProgramLinesElementEx = glCreateProgram();
    glAttachShader(this->m_ProgramLinesElementEx, nLinesEleVS);
    glAttachShader(this->m_ProgramLinesElementEx, nLinesEleFS);
    //glBindAttribLocation(this->m_ProgramCircle, 0, "vertex");
    //glBindAttribLocation(this->m_ProgramCircle, 1, "fcolor");
    //
    //glBindAttribLocation(this->m_ProgramCircle, 2, "V_color");
    //CHECK_GL_ERROR;

    CHECK_GL_ERROR;
    LinkProgram(this->m_ProgramLinesElementEx);

    //CHECK_GL_ERROR;
    this->m_LinesElementExProjectID = glGetUniformLocation(this->m_ProgramLinesElementEx, "projection");
    this->m_LinesElementExVertexID = glGetAttribLocation(this->m_ProgramLinesElementEx, "vertex");
    this->m_LinesElementExVertexFlagID = glGetAttribLocation(this->m_ProgramLinesElementEx, "vertexFlag");
    this->m_LinesElementExViewGeoID = glGetUniformLocation(this->m_ProgramLinesElementEx, "viewgeo");
    //this->m_LinesElementExViewGeoIDx = glGetUniformLocation(this->m_ProgramLinesElementEx, "viewgeo_x");
    //this->m_LinesElementExViewGeoIDy = glGetUniformLocation(this->m_ProgramLinesElementEx, "viewgeo_y");
    //this->m_LinesElementExViewGeoIDz = glGetUniformLocation(this->m_ProgramLinesElementEx, "viewgeo_z");
    //this->m_LinesElementExViewGeoIDw = glGetUniformLocation(this->m_ProgramLinesElementEx, "viewgeo_w");
    //this->m_LinesElementExGeoSelectRangeID = glGetUniformLocation(this->m_ProgramLinesElementEx, "V_geoSelectRange");
    //this->m_LinesElementExEnableSelecID = glGetUniformLocation(this->m_ProgramLinesElementEx, "V_enableSelect");
    //this->m_CircleColorID = glGetAttribLocation(this->m_ProgramCircle, "fcolor");
    this->m_LinesElementExColorID = glGetUniformLocation(this->m_ProgramLinesElementEx, "V_color");
    //printf("this->m_CircleProjectID : %d\n", this->m_CircleProjectID);
    //printf("this->m_CircleVertexID : %d\n", this->m_CircleVertexID);
    //printf("this->m_CircleColorID : %d\n", this->m_CircleColorID);
    CHECK_GL_ERROR;

    glGenVertexArrays(1, &this->m_LinesElementExVAO);
    glGenBuffers(1, &this->m_LinesElementExPosVBO);
    glGenBuffers(1, &this->m_LinesElementExPosFlagVBO);
    glGenBuffers(1, &this->m_LinesElementExPosVEO);
    //glGenBuffers(1, &this->m_CircleClsVBO);
}

void COpenGLCore::DrawLinesElementEx(int nPointCnt, double* pPoints, int nPointFlagsCnt, float* pPointsFlags, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fLineSize/* = 1.0f*/){

    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    ////glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    ////glEnable( GL_LINE_SMOOTH );
    ////glEnable(GL_POLYGON_SMOOTH);
    ////glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glLineWidth(fLineSize);
    ////glPolygonMode(GL_FRONT, GL_LINE);


    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramLinesElementEx);

    glUniformMatrix4fv(this->m_LinesElementExProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_LinesElementExColorID, 1, glm::value_ptr(fColor));
    glm::vec<4, double> fViewGwo(this->t_geo_minmax.fGeoLeftBottomX, this->t_geo_minmax.fGeoLeftBottomY, this->m_fView2GeoScaleH, this->m_fView2GeoScaleV);
    glUniform4dv(this->m_LinesElementExViewGeoID, 1, glm::value_ptr(fViewGwo));
    //glUniform1d(this->m_LinesElementExViewGeoIDx, this->t_geo_minmax.fGeoLeftBottomX);
    //glUniform1d(this->m_LinesElementExViewGeoIDy, this->t_geo_minmax.fGeoLeftBottomY);
    //glUniform1d(this->m_LinesElementExViewGeoIDz, this->m_fView2GeoScaleH);
    //glUniform1d(this->m_LinesElementExViewGeoIDw, this->m_fView2GeoScaleV);
    //m_LinesElementExEnableSelecID
    //glUniform1i(this->m_LinesElementExEnableSelecID, this->m_nMouseWorkStatus);
    //glUniform4dv(this->m_LinesElementExGeoSelectRangeID, 1, this->m_arrMouseSelectPoints);
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_LinesElementExVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nPointCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementExVertexID);
    glVertexAttribLPointer(this->m_LinesElementExVertexID, 3, GL_DOUBLE, 0, NULL);
    CHECK_GL_ERROR;

//    for(int ii = 0; ii<nPointFlagsCnt; ii++){
//        printf("%d  %f\n", ii, pPointsFlags[ii]);
//    }

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCnt, pPointsFlags, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementExVertexFlagID);
    glVertexAttribPointer(this->m_LinesElementExVertexFlagID, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCnt * 2, pPointsElement, GL_DYNAMIC_DRAW);

    glDrawElements(GL_LINES, nPointElementCnt * 2, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(this->m_LinesElementExVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glLineWidth(1.0f);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

}

void COpenGLCore::DrawLinesElementOperate(
    int nPointCnt, double* pPoints,
    int nPointFlagsCnt, float* pPointsFlags,
    int nPointElementCnt, unsigned int* pPointsElement,
    int nPointFlagsCntOperate, float* pPointsFlagsOperate,
    int nPointElementCntOperate, unsigned int* pPointsElementOperate,
    int nPointElementCntOperateNode, unsigned int* pPointsElementOperateNode,
    int nPointElementCntOperateHit, unsigned int* pPointsElementOperateHit,
    int nPointElementCntOperateNodeHit, unsigned int* pPointsElementOperateNodeHit,
    unsigned int nColor, float fLineSize/* = 1.0f*/)
{
    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    ////glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    ////glEnable( GL_LINE_SMOOTH );
    ////glEnable(GL_POLYGON_SMOOTH);
    ////glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glLineWidth(fLineSize);
    ////glPolygonMode(GL_FRONT, GL_LINE);


    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(fLineSize);
    glUseProgram(this->m_ProgramLinesElementEx);

    glUniformMatrix4fv(this->m_LinesElementExProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_LinesElementExColorID, 1, glm::value_ptr(fColor));
    glm::dvec4 fViewGwo(this->t_geo_minmax.fGeoLeftBottomX, this->t_geo_minmax.fGeoLeftBottomY, this->m_fView2GeoScaleH, this->m_fView2GeoScaleV);
    glUniform4dv(this->m_LinesElementExViewGeoID, 1, glm::value_ptr(fViewGwo));
    //glUniform1d(this->m_LinesElementExViewGeoIDx, this->t_geo_minmax.fGeoLeftBottomX);
    //glUniform1d(this->m_LinesElementExViewGeoIDy, this->t_geo_minmax.fGeoLeftBottomY);
    //glUniform1d(this->m_LinesElementExViewGeoIDz, this->m_fView2GeoScaleH);
    //glUniform1d(this->m_LinesElementExViewGeoIDw, this->m_fView2GeoScaleV);
    //m_LinesElementExEnableSelecID
    //glUniform1i(this->m_LinesElementExEnableSelecID, this->m_nMouseWorkStatus);
    //glUniform4dv(this->m_LinesElementExGeoSelectRangeID, 1, this->m_arrMouseSelectPoints);
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_LinesElementExVAO);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nPointCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementExVertexID);
    glVertexAttribLPointer(this->m_LinesElementExVertexID, 3, GL_DOUBLE, 0, NULL);
    CHECK_GL_ERROR;

//    for(int ii = 0; ii<nPointFlagsCnt; ii++){
//        printf("%d  %f\n", ii, pPointsFlags[ii]);
//    }

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCnt, pPointsFlags, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementExVertexFlagID);
    glVertexAttribPointer(this->m_LinesElementExVertexFlagID, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCnt, pPointsElement, GL_DYNAMIC_DRAW);

    glDrawElements(GL_LINES, nPointElementCnt, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    if (nPointElementCntOperate > 1) {

        glLineWidth(fLineSize+1.5f);

        glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCntOperate, pPointsFlagsOperate, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(this->m_LinesElementExVertexFlagID);
        glVertexAttribPointer(this->m_LinesElementExVertexFlagID, 1, GL_FLOAT, GL_FALSE, 0, NULL);
        CHECK_GL_ERROR;
        //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
        ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glEnableVertexAttribArray(this->m_CircleColorID);
        //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCntOperate, pPointsElementOperate, GL_DYNAMIC_DRAW);

        glDrawElements(GL_LINES, nPointElementCntOperate, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
        CHECK_GL_ERROR;
    }
    if (nPointElementCntOperateNode > 0) {
        glPointSize(fLineSize + 6.0f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCntOperateNode, pPointsElementOperateNode, GL_DYNAMIC_DRAW);

        glDrawElements(GL_POINTS, nPointElementCntOperateNode, GL_UNSIGNED_INT, 0);

        glPointSize(fLineSize + 10.0f);
        glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);

        glPointSize(fLineSize + 9.0f);
        glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(4));

        if(nPointElementCntOperateNode > 2){
            glPointSize(fLineSize + 8.0f);
            glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(8));
        }
        //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
        CHECK_GL_ERROR;
    }

    if (nPointElementCntOperateHit > 1) {

        glLineWidth(fLineSize+1.5f);

        glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCntOperate, pPointsFlagsOperate, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(this->m_LinesElementExVertexFlagID);
        glVertexAttribPointer(this->m_LinesElementExVertexFlagID, 1, GL_FLOAT, GL_FALSE, 0, NULL);
        CHECK_GL_ERROR;
        //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
        ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glEnableVertexAttribArray(this->m_CircleColorID);
        //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCntOperateHit, pPointsElementOperateHit, GL_DYNAMIC_DRAW);

        glDrawElements(GL_LINES, nPointElementCntOperateHit, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
        CHECK_GL_ERROR;
    }
    if (nPointElementCntOperateNodeHit > 0) {
        glPointSize(fLineSize + 6.0f);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCntOperate, pPointsFlagsOperate, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCntOperateNodeHit, pPointsElementOperateNodeHit, GL_DYNAMIC_DRAW);

        glDrawElements(GL_POINTS, nPointElementCntOperateNodeHit, GL_UNSIGNED_INT, 0);

        glPointSize(fLineSize + 10.0f);
        glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);

        if(nPointElementCntOperateNodeHit > 1){
            glPointSize(fLineSize + 9.0f);
            glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(4));
        }

        if(nPointElementCntOperateNodeHit > 2){
            glPointSize(fLineSize + 8.0f);
            glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(8));
        }
        //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
        CHECK_GL_ERROR;
    }
    glDisableVertexAttribArray(this->m_LinesElementExVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glLineWidth(1.0f);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;
}
void COpenGLCore::DrawPointsElementOperate(int nPointCnt, double* pPoints,
    int nPointFlagsCnt, float* pPointsFlags,
    int nPointElementCnt, unsigned int* pPointsElement,
    int nPointFlagsCntOperate, float* pPointsFlagsOperate,
    int nPointElementCntOperate, unsigned int* pPointsElementOperate,
    int nPointElementCntOperateHit, unsigned int* pPointsElementOperateHit,
    unsigned int nColor, float fPointSize/* = 1.0f*/)
{
    glm::vec4 fColor = COLOR32_INT_TO_FLOAT_RGBA(nColor);

    //glEnable(GL_BLEND);
    ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_DST_COLOR, GL_DST_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    ////glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    ////glEnable( GL_LINE_SMOOTH );
    ////glEnable(GL_POLYGON_SMOOTH);
    ////glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //glLineWidth(fLineSize);
    ////glPolygonMode(GL_FRONT, GL_LINE);


    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_SRC_COLOR);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR
    //glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_DST_ALPHA);//GL_DST_ALPHA  GL_ONE GL_DST_COLOR

    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_POINT_SMOOTH);
    glPointSize(fPointSize);
    glUseProgram(this->m_ProgramLinesElementEx);

    glUniformMatrix4fv(this->m_LinesElementExProjectID, 1, GL_FALSE, glm::value_ptr(this->m_MatOrthoProject));
    CHECK_GL_ERROR;
    glUniform4fv(this->m_LinesElementExColorID, 1, glm::value_ptr(fColor));
    glm::dvec4 fViewGwo(this->t_geo_minmax.fGeoLeftBottomX, this->t_geo_minmax.fGeoLeftBottomY, this->m_fView2GeoScaleH, this->m_fView2GeoScaleV);
    glUniform4dv(this->m_LinesElementExViewGeoID, 1, glm::value_ptr(fViewGwo));
    //glUniform1d(this->m_LinesElementExViewGeoIDx, this->t_geo_minmax.fGeoLeftBottomX);
    //glUniform1d(this->m_LinesElementExViewGeoIDy, this->t_geo_minmax.fGeoLeftBottomY);
    //glUniform1d(this->m_LinesElementExViewGeoIDz, this->m_fView2GeoScaleH);
    //glUniform1d(this->m_LinesElementExViewGeoIDw, this->m_fView2GeoScaleV);
    //m_LinesElementExEnableSelecID
    //glUniform1i(this->m_LinesElementExEnableSelecID, this->m_nMouseWorkStatus);
    //glUniform4dv(this->m_LinesElementExGeoSelectRangeID, 1, this->m_arrMouseSelectPoints);
    CHECK_GL_ERROR;

    glBindVertexArray(this->m_LinesElementExVAO);
    CHECK_GL_ERROR;

    double* pViewPoints = nullptr;
    if ( 0){
        pViewPoints = new double[nPointCnt * 3];
        for(int ii = 0; ii < nPointCnt; ii++){
            printf("%d  %f, %f", ii, pPoints[ii*3], pPoints[ii*3+1]);
            double xView = ((pPoints[ii*3]-this->t_geo_minmax.fGeoLeftBottomX) * this->m_fView2GeoScaleH);
            double yView = ((pPoints[ii*3+1]-this->t_geo_minmax.fGeoLeftBottomY) * this->m_fView2GeoScaleV);
            pViewPoints[ii*3] = (double)xView;
            pViewPoints[ii*3+1] = (double)yView;
            printf(" ===> %f, %f\n", pViewPoints[ii*3], pViewPoints[ii*3+1]);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * nPointCnt * 3, pPoints, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementExVertexID);
    glVertexAttribLPointer(this->m_LinesElementExVertexID, 3, GL_DOUBLE, 0, NULL);
    CHECK_GL_ERROR;

//    for(int ii = 0; ii<nPointFlagsCnt; ii++){
//        printf("%d  %f\n", ii, pPointsFlags[ii]);
//    }

    glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCnt, pPointsFlags, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(this->m_LinesElementExVertexFlagID);
    glVertexAttribPointer(this->m_LinesElementExVertexFlagID, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    CHECK_GL_ERROR;
    //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
    ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(this->m_CircleColorID);
    //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCnt, pPointsElement, GL_DYNAMIC_DRAW);

    glDrawElements(GL_POINTS, nPointElementCnt, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
    CHECK_GL_ERROR;

    if (nPointElementCntOperate > 0) {

        glPointSize(fPointSize + 3.0f);

        glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCntOperate, pPointsFlagsOperate, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(this->m_LinesElementExVertexFlagID);
        glVertexAttribPointer(this->m_LinesElementExVertexFlagID, 1, GL_FLOAT, GL_FALSE, 0, NULL);
        CHECK_GL_ERROR;
        //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
        ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glEnableVertexAttribArray(this->m_CircleColorID);
        //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCntOperate, pPointsElementOperate, GL_DYNAMIC_DRAW);

        glDrawElements(GL_POINTS, nPointElementCntOperate, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
        CHECK_GL_ERROR;
    }

    if (nPointElementCntOperateHit > 0) {

        glPointSize(fPointSize + 3.0f);

        glBindBuffer(GL_ARRAY_BUFFER, this->m_LinesElementExPosFlagVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nPointFlagsCntOperate, pPointsFlagsOperate, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(this->m_LinesElementExVertexFlagID);
        glVertexAttribPointer(this->m_LinesElementExVertexFlagID, 1, GL_FLOAT, GL_FALSE, 0, NULL);
        CHECK_GL_ERROR;
        //glBindBuffer(GL_ARRAY_BUFFER, this->m_CircleClsVBO);
        ////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 3, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nCirclePtCnt * 4, vecCircleClrs.data(), GL_DYNAMIC_DRAW);
        //glEnableVertexAttribArray(this->m_CircleColorID);
        //glVertexAttribPointer(this->m_CircleColorID, 4, GL_FLOAT, GL_TRUE, 0, NULL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_LinesElementExPosVEO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nPointElementCntOperateHit, pPointsElementOperateHit, GL_DYNAMIC_DRAW);

        glDrawElements(GL_POINTS, nPointElementCntOperateHit, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_LINES, 0, nPointCnt); //GL_POLYGON GL_TRIANGLE_FAN 填充， GL_LINE_LOOP 圆圈  GL_POINTS  GL_LINES
        CHECK_GL_ERROR;
    }

    glDisableVertexAttribArray(this->m_LinesElementExVertexID);
    CHECK_GL_ERROR;
    glUseProgram(0);
    CHECK_GL_ERROR;
    //glDisable( GL_POINT_SMOOTH );

    glDisable(GL_POINT_SMOOTH);//
    //glDisable(GL_POLYGON_SMOOTH);
    glPointSize(1.0f);
    //glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR;

    if(pViewPoints)
    {
        delete[] pViewPoints;
        pViewPoints = nullptr;
    }
}

//  ---------------------------------------------------------------------------


unsigned char * COpenGLCore::LoadFileContent(const char *path, int &filesize) {
    unsigned char*fileContent = nullptr;
    filesize = 0;
    FILE*pFile = fopen(path, "rb");
    if (pFile) {
        fseek(pFile, 0, SEEK_END);
        int nLen = ftell(pFile);
        if (nLen > 0) {
            rewind(pFile);
            fileContent = new unsigned char[nLen + 1];
            fread(fileContent, sizeof(unsigned char), nLen, pFile);
            fileContent[nLen] = '\0';
            filesize = nLen;
        }
        fclose(pFile);
    }
    return fileContent;
}

void COpenGLCore::ShaderInit(const char* szFileVS, const char* szFileFS) {
    int nFileSize = 0;
    const char*vsCode = (char*)LoadFileContent(szFileVS,nFileSize);
    const char*fsCode = (char*)LoadFileContent(szFileFS,nFileSize);
    GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vsCode);
    if (vsShader==0){
        return;
    }
    GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsCode);
    if (fsShader == 0) {
        return;
    }
    mProgram=CreateProgram(vsShader, fsShader);
    glDeleteShader(vsShader);
    glDeleteShader(fsShader);
    if (mProgram!=0){
        mModelMatrixLocation = glGetUniformLocation(mProgram, "ModelMatrix");
        mViewMatrixLocation = glGetUniformLocation(mProgram, "ViewMatrix");
        mProjectionMatrixLocation = glGetUniformLocation(mProgram, "ProjectionMatrix");
        mPositionLocation = glGetAttribLocation(mProgram, "position");
        mColorLocation = glGetAttribLocation(mProgram, "color");
        mTexcoordLocation = glGetAttribLocation(mProgram, "texcoord");
        mNormalLocation = glGetAttribLocation(mProgram, "normal");
    }
}
void COpenGLCore::ShaderUse() {
    this->glUseProgram(mProgram);
}
void COpenGLCore::ShaderBind(float *M, float *V, float*P) {
    ShaderUse();
    glUniformMatrix4fv(mModelMatrixLocation, 1, GL_FALSE, M);
    glUniformMatrix4fv(mViewMatrixLocation, 1, GL_FALSE, V);
    glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, P);
    int iIndex = 0;
    for (auto iter = mUniformTextures.begin(); iter != mUniformTextures.end(); ++iter) {
        glActiveTexture(GL_TEXTURE0 + iIndex);
        glBindTexture(GL_TEXTURE_2D, iter->second->mTexture);
        glUniform1i(iter->second->mLocation, iIndex++);
    }
    glEnableVertexAttribArray(mPositionLocation);
    glVertexAttribPointer(mPositionLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(mColorLocation);
    glVertexAttribPointer(mColorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));
    glEnableVertexAttribArray(mTexcoordLocation);
    glVertexAttribPointer(mTexcoordLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(mNormalLocation);
    glVertexAttribPointer(mNormalLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 12));
    //glUseProgram(0);
}
void COpenGLCore::ShaderSetTexture(const char * name, const char*imagePath) {
    auto iter = mUniformTextures.find(name);
    if (iter == mUniformTextures.end()) {
        GLint location = glGetUniformLocation(mProgram, name);
        if (location != -1) {
            UniformTexture*t = new UniformTexture;
            t->mLocation = location;
            t->mTexture = CreateTexture2DFromBMP(imagePath);
            mUniformTextures.insert(std::pair<std::string, UniformTexture*>(name, t));
        }
    } else {
        glDeleteTextures(1, &iter->second->mTexture);
        iter->second->mTexture = CreateTexture2DFromBMP(imagePath);
    }
}

GLuint COpenGLCore::CompileShader(GLenum shaderType, const char*shaderCode) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);
    GLint compileResult = GL_TRUE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
    if (compileResult == GL_FALSE) {
        char szLog[1024] = { 0 };
        GLsizei logLen = 0;
        glGetShaderInfoLog(shader, 1024, &logLen, szLog);
        printf("Compile Shader fail error log : %s \nshader code :\n%s\n", szLog, shaderCode);
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}
GLuint COpenGLCore::CreateProgram(GLuint vsShader, GLuint fsShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vsShader);
    glAttachShader(program, fsShader);
    glLinkProgram(program);
    glDetachShader(program, vsShader);
    glDetachShader(program, fsShader);
    GLint nResult;
    glGetProgramiv(program, GL_LINK_STATUS, &nResult);
    if (nResult == GL_FALSE) {
        char log[1024] = { 0 };
        GLsizei writed = 0;
        glGetProgramInfoLog(program, 1024, &writed, log);
        printf("create gpu program fail,link error : %s\n", log);
        glDeleteProgram(program);
        program = -1;
    }
    return program;
}

GLuint COpenGLCore::CreateProgram(const char* szCodeVS, const char* szCodeFS) {

    GLuint vsShader = CompileShader(GL_VERTEX_SHADER, szCodeVS);
    if (vsShader==0){
        return -1;
    }
    GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, szCodeFS);
    if (fsShader==0){
        return -1;
    }
    GLuint nProgram=CreateProgram(vsShader, fsShader);
    glDeleteShader(vsShader);
    glDeleteShader(fsShader);

    return nProgram;
}

unsigned char* COpenGLCore::DecodeBMP(unsigned char*bmpFileData, int&width, int&height) {
    if (0x4D42 == *((unsigned short*)bmpFileData)) {
        int pixelDataOffset = *((int*)(bmpFileData + 10));
        width = *((int*)(bmpFileData + 18));
        height = *((int*)(bmpFileData + 22));
        unsigned char*pixelData = bmpFileData + pixelDataOffset;
        for (int i = 0; i < width*height * 3; i += 3) {
            unsigned char temp = pixelData[i];
            pixelData[i] = pixelData[i + 2];
            pixelData[i + 2] = temp;
        }
        return pixelData;
    }
    return nullptr;
}
GLuint COpenGLCore::CreateTexture2D(unsigned char*pixelData, int width, int height, GLenum type) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}
GLuint COpenGLCore::CreateTexture2DFromBMP(const char*szBmpImgFilePath, int nChannelCnt/* = 3*/) {
    int nFileSize = 0;
    unsigned char *bmpFileContent = LoadFileContent(szBmpImgFilePath, nFileSize);
    if (bmpFileContent == nullptr) {
        return 0;
    }
    int bmpWidth = 0, bmpHeight = 0;
    unsigned char*pixelData = DecodeBMP(bmpFileContent, bmpWidth, bmpHeight);
    if (bmpWidth == 0) {
        delete bmpFileContent;
        return 0;
    }

    GLenum nBufType = GL_RGB;
    switch (nChannelCnt) {
    case 3:{
        break;
    }
    case 4:{
        nBufType = GL_RGBA;
        break;
    }
    }
    GLuint texture = CreateTexture2D(pixelData, bmpWidth, bmpHeight, nBufType);
    delete bmpFileContent;

    return texture;
}
GLuint COpenGLCore::CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data /* = nullptr */) {
    GLuint object;
    glGenBuffers(1, &object);
    glBindBuffer(bufferType, object);
    glBufferData(bufferType, size, data, usage);
    glBindBuffer(bufferType, 0);
    return object;
}
GLuint COpenGLCore::CreateTexture2DFromPNG(const char *szPngJpgImgFilePath, int invertY/* = 0*/) {

    int nFileSize = 0;
    unsigned char *filecontent = LoadFileContent(szPngJpgImgFilePath, nFileSize);
    if (filecontent == nullptr) {
        return 0;
    }
    unsigned int flags = SOIL_FLAG_POWER_OF_TWO;
    if (invertY == 1) {
        flags |= SOIL_FLAG_INVERT_Y;
    }
    GLuint texture = this->m_pSoilTool->SOIL_load_OGL_texture_from_memory(filecontent, nFileSize, 0, 0, flags);
    delete filecontent;

    return texture;
}

GLuint COpenGLCore::CreateTexture2DFromRaster(const char *szRasterImgFilePath){
    return -1;
}

GLuint COpenGLCore::CreateTexture2DFromImgBuffer(const char *szImgBufferName, unsigned char* pPixBuffer, int nPixWidth, int nPixHeight, int nChannelCnt/* = 3*/) {
    if(szImgBufferName == nullptr){
        return -1;
    }
    std::string sImgFileFullPath = szImgBufferName;
    auto itt = this->m_mapImgPath2TextureID.find(sImgFileFullPath);
    if(itt != this->m_mapImgPath2TextureID.end()){
        return itt->second;
    }
    GLenum nBufType = GL_RGB;
    switch (nChannelCnt) {
    case 3:{
        break;
    }
    case 4:{
        nBufType = GL_RGBA;
        break;
    }
    }
    GLuint nTextrueID = CreateTexture2D(pPixBuffer, nPixWidth, nPixHeight, nBufType);
    this->m_mapImgPath2TextureID.insert(std::pair<std::string, GLuint>(sImgFileFullPath, nTextrueID));
    return nTextrueID;
}

GLuint COpenGLCore::LoadTextureFromFile(const char* szImgFilePath, bool invertY/* = false*/){
    std::string sImgFileFullPath = szImgFilePath;
    auto itt = this->m_mapImgPath2TextureID.find(sImgFileFullPath);
    if(itt != this->m_mapImgPath2TextureID.end()){
        return itt->second;
    }
    GLuint nTextrueID = -1;
    std::string sFileExtName = sImgFileFullPath;
    sFileExtName = sFileExtName.substr(sFileExtName.rfind('.') + 1);
    std::transform(sFileExtName.begin(), sFileExtName.end(), sFileExtName.begin(), ::toupper);
    if(sFileExtName.compare("BMP") == 0){
        nTextrueID = this->m_pSoilTool->SOIL_load_OGL_texture (szImgFilePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y );
    }
    else if(sFileExtName.compare("JPG") == 0 || sFileExtName.compare("PNG") == 0){
        nTextrueID = this->CreateTexture2DFromPNG(szImgFilePath, invertY);
    }
    else if(sFileExtName.compare("DEFPIXBUF") == 0) {
        //nTextrueID = this->CreateTexture2DFromImgBuffer(szImgFilePath);
    }
    this->m_mapImgPath2TextureID.insert(std::pair<std::string, GLuint>(sImgFileFullPath, nTextrueID));
    return nTextrueID;
}

int COpenGLCore::RemoveTextureByFile(const char* szImgFilePath){
    auto itt = this->m_mapImgPath2TextureID.find(szImgFilePath);
    if(itt == this->m_mapImgPath2TextureID.end())
        return 0;
    GLuint nTextureID = itt->second;
    glDeleteTextures(1, &nTextureID);
    this->m_mapImgPath2TextureID.erase(itt);//this->m_mapImgPath2TextureID.find(szImgFilePath));
    return 1;
}

// //////////////////////////////////////////////// FrameBufferObject
void COpenGLCore::InitFrameBufferObj() {
    glDeleteFramebuffers(1, &this->m_FrameBufferObject);
    glGenFramebuffers(1, &this->m_FrameBufferObject);

    CHECK_GL_ERROR;
    FboAttachColorBuffer("fbo_color", GL_COLOR_ATTACHMENT0, this->m_nWndWidth, this->m_nWndHeight);

    CHECK_GL_ERROR;
    FboAttachDepthBuffer("fbo_depth", this->m_nWndWidth, this->m_nWndHeight);

    CHECK_GL_ERROR;
    FboFinish();

    CHECK_GL_ERROR;
    //sphere.Init("Res/Sphere.obj");
    //sphere.SetTexture(fbo->GetBuffer("color"));
    //sphere.mModelMatrix=glm::scale(4.0f,4.0f,4.0f)*glm::rotate(150.0f,0.0f,1.0f,0.0f);
}
void COpenGLCore::FboAttachColorBuffer(const char*bufferName, GLenum attachment, int width, int height){
    GLuint colorBuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_FrameBufferObject);
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECK_GL_ERROR;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR;
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, colorBuffer, 0);
    CHECK_GL_ERROR;
    this->m_vecDrawBuffers.clear();
    this->m_vecDrawBuffers.push_back(attachment);
    this->m_mapBuffers.insert(std::pair<std::string,GLuint>(bufferName,colorBuffer));
    this->m_mapImgPath2TextureID.insert(std::pair<std::string, GLuint>(bufferName,colorBuffer));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CHECK_GL_ERROR;
    this->m_FboColorTextureID = colorBuffer;
}
void COpenGLCore::FboAttachDepthBuffer(const char*bufferName, int width, int height){
    GLuint depthMap;
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_FrameBufferObject);
    CHECK_GL_ERROR;
    glGenTextures(1, &depthMap);
    CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, depthMap);
    CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    CHECK_GL_ERROR;
    this->m_mapBuffers.insert(std::pair<std::string, GLuint>(bufferName, depthMap));
    this->m_mapImgPath2TextureID.insert(std::pair<std::string, GLuint>(bufferName,depthMap));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->m_FboDepthTextureID = depthMap;
}
void COpenGLCore::FboFinish(){
    int nCount = (int)this->m_vecDrawBuffers.size();
    if (nCount>0){
        GLenum *buffers = new GLenum[nCount];
        int i = 0;
        while (i<nCount){
            buffers[i] = this->m_vecDrawBuffers[i];
            i++;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, this->m_FrameBufferObject);
        CHECK_GL_ERROR;
        glDrawBuffers(nCount, buffers);
        CHECK_GL_ERROR;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CHECK_GL_ERROR;
    }
}
void COpenGLCore::FboBind(){
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &this->m_PrevFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_FrameBufferObject);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearColor(0.5f, 0.3f, 0.7f, 1.0f);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void COpenGLCore::FboUnbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_PrevFrameBuffer);
}


// //////////////////////////////////////////////// FrameBufferObject
void COpenGLCore::ResetMidleDownMovingPos() {

    this->m_nMouseMovingLastDeltaX = this->m_nMouseMovingDeltaX =
    this->m_nMouseMovingLastDeltaY = this->m_nMouseMovingDeltaY = 0;

}
void COpenGLCore::UpdateMidleDownMovingPos() {
    this->m_nMouseMovingLastDeltaX += this->m_nMouseMovingDeltaX;
    this->m_nMouseMovingLastDeltaY += this->m_nMouseMovingDeltaY;
}

void COpenGLCore::MidleDownMoving(int xDelta, int yDelta) {
    this->m_nMouseMovingDeltaX = xDelta;
    this->m_nMouseMovingDeltaY = yDelta;

    //double fOffsetX = -1.0 * this->m_fGeo2ViewScaleH * xDelta;
    //double fOffsetY = -1.0 * this->m_fGeo2ViewScaleV * yDelta;
    //
    //this->m_fMouseMovingDeltaX = fOffsetX;
    //this->m_fMouseMovingDeltaY = fOffsetY;

    //this->t_geo_minmax.fGeoRightTopX += fOffsetX;
    //this->t_geo_minmax.fGeoLeftBottomY += fOffsetY;
    //this->t_geo_minmax.fGeoRightTopY += fOffsetY;
    //this->InitGeometry();

    this->m_nViewBaseLeftBottomX = this->m_nMouseMovingDeltaX;
    this->m_nViewBaseLeftBottomY = this->m_nMouseMovingDeltaY;
    this->m_nViewBaseWidth = this->m_nWndWidth;
    this->m_nViewBaseHeight = this->m_nWndHeight;
}

void COpenGLCore::MidleDownMovingEnd(){
    //return;
    double fOffsetX = -1.0 * this->m_fGeo2ViewScaleH * this->m_nMouseMovingDeltaX;
    double fOffsetY = -1.0 * this->m_fGeo2ViewScaleV * this->m_nMouseMovingDeltaY;

    this->t_geo_minmax.fGeoLeftBottomX += fOffsetX;
    this->t_geo_minmax.fGeoRightTopX += fOffsetX;
    this->t_geo_minmax.fGeoLeftBottomY += fOffsetY;
    this->t_geo_minmax.fGeoRightTopY += fOffsetY;
    this->InitGeometry();
    this->m_nMouseMovingDeltaX = 0;
    this->m_nMouseMovingDeltaY = 0;

    this->m_nViewBaseLeftBottomX = 0;
    this->m_nViewBaseLeftBottomY = 0;
    this->m_nViewBaseWidth = this->m_nWndWidth;
    this->m_nViewBaseHeight = this->m_nWndHeight;
}

void COpenGLCore::WheelEventEnd(){    
    if(this->m_nWheelPow >= 0 && (this->m_fView2GeoScaleH < 0.000001 || this->m_fView2GeoScaleV < 0.000001 )){
        printf("WheelRes 最小 22 \n");
        this->UpdateWidgets();
        return;
    }

    if(this->m_nWheelPow == 0)
        return;
    if(this->m_nWheelPow <= 0 && fabs(this->t_geo_minmax.fGeoRightTopX - this->t_geo_minmax.fGeoLeftBottomX) <= 0.0000001){
        printf("WheelRes 最大 22 \n");
        this->UpdateWidgets();
        return;
    }
    double fGeoLenX = this->t_geo_minmax.fGeoRightTopX - this->t_geo_minmax.fGeoLeftBottomX;
    double fGeoLenY = this->t_geo_minmax.fGeoRightTopY - this->t_geo_minmax.fGeoLeftBottomY;

    double fMouseGeoX = 0, fMouseGeoY = 0;
    this->View2GeoPosition(this->m_nWheelOffXSrc, this->m_nWheelOffYSrc, &fMouseGeoX, &fMouseGeoY);
    float xMouse2LeftBottom = (float)(fMouseGeoX - this->t_geo_minmax.fGeoLeftBottomX);
    float yMouse2LeftBottom = (float)(fMouseGeoY - this->t_geo_minmax.fGeoLeftBottomY);
    float nPow = (1.0f - pow(this->m_fPowNumMul, this->m_nWheelPow));
    nPow = 1.0f;
    if(this->m_nWheelPow > 0){
        nPow = pow(this->m_fPowNumDev, this->m_nWheelPow);
    }
    else {//(this->m_nWheelPow < 0){
        this->m_nWheelPow = this->m_nWheelPow * -1;
        nPow = pow(this->m_fPowNumMul, this->m_nWheelPow);
    }

    double fGeoWheelOffX = xMouse2LeftBottom * (1.0f - nPow);
    double fGeoWheelOffY = yMouse2LeftBottom * (1.0f - nPow);
    fGeoLenX = fGeoLenX * nPow;
    fGeoLenY = fGeoLenY * nPow;
    this->t_geo_minmax.fGeoRightTopX = this->t_geo_minmax.fGeoLeftBottomX + fGeoLenX;
    this->t_geo_minmax.fGeoRightTopY = this->t_geo_minmax.fGeoLeftBottomY + fGeoLenY;

    this->t_geo_minmax.fGeoRightTopX += fGeoWheelOffX;
    this->t_geo_minmax.fGeoRightTopY += fGeoWheelOffY;
    this->t_geo_minmax.fGeoLeftBottomX += fGeoWheelOffX;
    this->t_geo_minmax.fGeoLeftBottomY += fGeoWheelOffY;

    this->InitGeometry();

    this->m_nWheelFlag = 0;
    this->m_nWheelPow = 0;
    this->m_fPowNumMul = 0;
    this->m_fPowNumDev = 0;
    this->m_nWheelOffX = 0;
    this->m_nWheelOffY = 0;
    this->m_nWheelScale = 1.0f;

    this->m_nViewBaseLeftBottomX = 0;
    this->m_nViewBaseLeftBottomY = 0;
    this->m_nViewBaseWidth = this->m_nWndWidth;
    this->m_nViewBaseHeight = this->m_nWndHeight;
    this->m_nMouseStatus = 0;

    printf("WheelRes\n");
    this->UpdateWidgets();
}

bool COpenGLCore::WheelTimerProc(){
    //printf("Timer Proc End\n");
    this->m_WheelTimer.stop();
    this->WheelEventEnd();
    return true;
}
void COpenGLCore::WheelEvent(int nDelta, int xMousePos, int yMousePos){
    //printf("MouseWheelDelta: %d %d %d\n", nDelta, xMousePos, yMousePos);
    if(nDelta > 0 && (this->m_fView2GeoScaleH < 0.000001 || this->m_fView2GeoScaleV < 0.000001 )){
        printf("WheelRes 最小\n");
        this->UpdateWidgets();
        return;
    }
    if(nDelta < 0 && fabs(this->t_geo_minmax.fGeoRightTopX - this->t_geo_minmax.fGeoLeftBottomX) <= 0.0000001){
        printf("WheelRes 最大\n");
        this->UpdateWidgets();
        return;
    }

    this->m_nMouseStatus = COpenGLCore::e_Mouse_WHEEL_SCROLLING;
    float fViewPosX = (float)xMousePos;
    float fViewPosY = (float)(this->m_nWndHeight - yMousePos);

    if(this->m_nWheelFlag == 0) {
        this->m_nWheelOffXSrc = fViewPosX;
        this->m_nWheelOffYSrc = fViewPosY;
        this->m_nWheelFlag= 1;
        this->m_nWheelPow = 0;

        this->m_fPowNumMul = 0.8f;
        this->m_fPowNumDev = 1.25f;
    }
    float nPow = 0;
    if(nDelta > 0){

        this->m_nWheelPow++;
        this->m_nWheelScale *= this->m_fPowNumMul;

    }
    else{

        this->m_nWheelPow--;
        this->m_nWheelScale *= this->m_fPowNumDev;

    }
    nPow = (1.0f - pow(this->m_fPowNumMul, this->m_nWheelPow));
    this->m_nWheelOffX = this->m_nWheelOffXSrc * nPow;
    this->m_nWheelOffY = this->m_nWheelOffYSrc * nPow;

    this->m_nViewBaseLeftBottomX = this->m_nWheelOffX;
    this->m_nViewBaseLeftBottomY = this->m_nWheelOffY;
    this->m_nViewBaseWidth = this->m_nWndWidth * this->m_nWheelScale;
    this->m_nViewBaseHeight = this->m_nWndHeight * this->m_nWheelScale;

    if(this->m_WheelTimer.isActive()){
        this->m_WheelTimer.stop();
        //printf("Timer Stop\n");
    }else{
        //printf("Timer New Start");
    }
    this->m_WheelTimer.start(400);

    //this->timeRefreshView = new QTimer(this);
    //connect(this->timeRefreshView, &QTimer::timeout, this, &SuperXMainDlg::reFreshPixMapOnView);
    return;

    double fGeoPosX = 0.0, fGeoPosY = 0.0;
    this->View2GeoPosition(fViewPosX, fViewPosY, &fGeoPosX, &fGeoPosY);

    double xGeoCenter = (this->t_geo_minmax.fGeoLeftBottomX + this->t_geo_minmax.fGeoRightTopX) / 2.0;
    double yGeoCenter = (this->t_geo_minmax.fGeoLeftBottomY + this->t_geo_minmax.fGeoRightTopY) / 2.0;


    double yTopLen = -0.1 * (xGeoCenter - fGeoPosY);
    double xTopLen = -0.1 * (yGeoCenter - fGeoPosX);
    double fGeoLenX = this->t_geo_minmax.fGeoRightTopX - this->t_geo_minmax.fGeoLeftBottomX;
    double fGeoLenY = this->t_geo_minmax.fGeoRightTopY - this->t_geo_minmax.fGeoLeftBottomY;
    double fOffsetX = fGeoLenX * 0.1;
    double fOffsetY = fGeoLenY * 0.1;
    if(nDelta > 0){
        this->t_geo_minmax.fGeoLeftBottomX = this->t_geo_minmax.fGeoLeftBottomX - fOffsetX;
        this->t_geo_minmax.fGeoRightTopX = this->t_geo_minmax.fGeoRightTopX + fOffsetX;
        this->t_geo_minmax.fGeoLeftBottomY = this->t_geo_minmax.fGeoLeftBottomY - fOffsetY;
        this->t_geo_minmax.fGeoRightTopY = this->t_geo_minmax.fGeoRightTopY + fOffsetY;
    }else{
        this->t_geo_minmax.fGeoLeftBottomX = this->t_geo_minmax.fGeoLeftBottomX + fOffsetX;
        this->t_geo_minmax.fGeoRightTopX = this->t_geo_minmax.fGeoRightTopX - fOffsetX;
        this->t_geo_minmax.fGeoLeftBottomY = this->t_geo_minmax.fGeoLeftBottomY + fOffsetY;
        this->t_geo_minmax.fGeoRightTopY = this->t_geo_minmax.fGeoRightTopY - fOffsetY;
    }
    this->t_geo_minmax.fGeoLeftBottomX += fGeoPosX;
    this->t_geo_minmax.fGeoRightTopX += fGeoPosX;
    this->t_geo_minmax.fGeoLeftBottomY += fGeoPosY;
    this->t_geo_minmax.fGeoRightTopY += fGeoPosY;

    this->InitGeometry();

    double fNewGeoPosX = 0.0, fNewGeoPosY = 0.0;
    this->View2GeoPosition(fViewPosX, fViewPosY, &fNewGeoPosX, &fNewGeoPosY);

    xTopLen = fNewGeoPosX - fGeoPosX;
    yTopLen = fNewGeoPosY - fGeoPosY;
    this->t_geo_minmax.fGeoLeftBottomX -= xTopLen;
    this->t_geo_minmax.fGeoRightTopX -= xTopLen;
    this->t_geo_minmax.fGeoLeftBottomY -= yTopLen;
    this->t_geo_minmax.fGeoRightTopY -= yTopLen;
    this->InitGeometry();
}

void COpenGLCore::MouseMoving(int xPos, int yPos){
    this->m_nMousePosX = xPos;
    this->m_nMousePosY = this->m_nWndHeight - yPos;

    double fGeoX, fGeoY;
    this->View2GeoPosition(this->m_nMousePosX, this->m_nMousePosY, &fGeoX, &fGeoY);
    char szGeoXY[64] = {0};
    sprintf(szGeoXY, "%.9f, %.9f", fGeoX, fGeoY);
    this->m_sMsg = std::string(szGeoXY) + " : "+std::to_string(this->m_nMousePosX)+", "+std::to_string(this->m_nMousePosY);
    this->m_nGeoMousePosX = fGeoX;
    this->m_nGeoMousePosY = fGeoY;

    switch (this->m_nMouseWorkStatus) {
    case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select:{
        if(this->m_nMouseWorkBegin == 1) {
            this->m_nMouseSelectPosEndX = this->m_nMousePosX;
            this->m_nMouseSelectPosEndY = this->m_nMousePosY;
        }
        break;
    }
    case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_RollerBlind:{
        if(this->m_nMouseClickFlag == 1 && this->m_pLayerVector->m_nLayerIndxRollerBind != -1)
        {
            this->UpdateWidgets();
        }

        break;
    }
    }
    double fOffsetWidth = 5.0 * this->m_fGeo2ViewScaleH;
    double fOffsetHeight = 5.0 * this->m_fGeo2ViewScaleV;
    double arrMousePointBound[4] = {
        this->m_nGeoMousePosX - fOffsetWidth, this->m_nGeoMousePosY - fOffsetHeight,
        this->m_nGeoMousePosX + fOffsetWidth, this->m_nGeoMousePosY + fOffsetHeight,
    };
    this->m_pLayerVector->MouseMoving(this->m_nGeoMousePosX, this->m_nGeoMousePosY, arrMousePointBound);
    //this->m_nMouseStatus = COpenGLCore::e_MOUSE_MOVING;
}

void COpenGLCore::MousePress(int nBtnType, int xMousePos, int yMousePos) {
    if(nBtnType == 1){//Left Btn
        switch (this->m_nMouseWorkStatus) {
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select:{
            this->m_nMouseWorkBegin = 1;
            this->m_nMouseSelectPosStartX = xMousePos;
            this->m_nMouseSelectPosStartY = this->m_nWndHeight - yMousePos;
            this->m_nMouseSelectPosEndX = 0;
            this->m_nMouseSelectPosEndY = 0;
            break;
        }
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance:{
            break;
        }
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_RollerBlind:{
            this->m_nMouseClickFlag = 1;

            int nHalfWidth = this->m_nWndWidth;
            int nHalfHeight = this->m_nWndHeight;
            nHalfWidth = (nHalfWidth >> 1);
            nHalfHeight = (nHalfHeight >> 1);
            int nHalf2Width = (nHalfWidth >> 1);
            if(this->m_nMousePosX < nHalf2Width){
                this->m_nRollerBlindArrow = 3;//Left
            } else if(this->m_nMousePosX > nHalf2Width + nHalfWidth){
                this->m_nRollerBlindArrow = 4;//Right
            } else if(this->m_nMousePosX >= nHalf2Width && this->m_nMousePosX <= nHalf2Width + nHalfWidth){
                if(this->m_nMousePosY > nHalfHeight){
                    this->m_nRollerBlindArrow = 1;//Up
                }else{
                    this->m_nRollerBlindArrow = 2;//Down
                }
            }
            break;
        }
        }
    }else if(nBtnType == 2){//Middle Btn

    }else if(nBtnType == 3){//Left Btn

    }
}

void COpenGLCore::MouseDbPress(int nBtnType, int xMousePos, int yMousePos){
    if(nBtnType == 1){//Left Btn
        switch (this->m_nMouseWorkStatus) {
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select:{
            break;
        }
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance:{
            this->m_fCalcDistance = 0.0;
            this->m_nCalcDistLastSelectPoint = 0;
            this->m_vecCalcDisPoint.clear();
            this->m_nMouseWorkStatus = this->m_nMouseWorkStatusLast;
            break;
        }
        }
    }else if(nBtnType == 2){//Middle Btn

    }else if(nBtnType == 3){//Left Btn

    }
}

void COpenGLCore::MouseRelease(int nBtnType, int xMousePos, int yMousePos){
    if(nBtnType == 1){//Left Btn
        switch (this->m_nMouseWorkStatus) {
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_Select:{
            this->m_arrMouseSelectPoints[0] = this->m_nMouseSelectPosStartX;
            this->m_arrMouseSelectPoints[1] = this->m_nMouseSelectPosStartY;
            this->m_arrMouseSelectPoints[2] = xMousePos;
            this->m_arrMouseSelectPoints[3] = this->m_nWndHeight - yMousePos;

            double fSelectRangeGeoMinX = 0, fSelectRangeGeoMinY = 0, fSelectRangeGeoMaxX = 0, fSelectRangeGeoMaxY = 0;
            this->View2GeoPosition(this->m_nMouseSelectPosStartX, this->m_nMouseSelectPosStartY,
                                   &fSelectRangeGeoMinX, &fSelectRangeGeoMinY);

            this->View2GeoPosition(xMousePos, this->m_nWndHeight - yMousePos,
                                   &fSelectRangeGeoMaxX, &fSelectRangeGeoMaxY);

            if(fSelectRangeGeoMaxX < fSelectRangeGeoMinX){
                double fTmp = fSelectRangeGeoMinX;
                fSelectRangeGeoMinX = fSelectRangeGeoMaxX;
                fSelectRangeGeoMaxX = fTmp;
            }
            if(fSelectRangeGeoMaxY < fSelectRangeGeoMinY){
                double fTmp = fSelectRangeGeoMinY;
                fSelectRangeGeoMinY = fSelectRangeGeoMaxY;
                fSelectRangeGeoMaxY = fTmp;
            }
            this->m_arrMouseSelectPoints[0] = fSelectRangeGeoMinX;
            this->m_arrMouseSelectPoints[1] = fSelectRangeGeoMinY;
            this->m_arrMouseSelectPoints[2] = fSelectRangeGeoMaxX;
            this->m_arrMouseSelectPoints[3] = fSelectRangeGeoMaxY;

            this->m_nMouseSelectPosStartX = 0;
            this->m_nMouseSelectPosStartY = 0;
            this->m_nMouseSelectPosEndX = 0;
            this->m_nMouseSelectPosEndY = 0;

            this->m_pLayerVector->UpdateGeoRanderBySelectRange();

            this->UpdateWidgets();
            //this->m_nMouseWorkStatus = this->m_nMouseWorkStatusLast;
            break;
        }
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_CalcDistance:{
            this->m_nCalcDistLastSelectPoint++;
            this->m_vecCalcDisPoint.push_back(glm::dvec3(this->m_nGeoMousePosX, this->m_nGeoMousePosY, 0.0));
            if(this->m_nCalcDistLastSelectPoint > 1){
                int nVecCalcDisPointCnt = (int)this->m_vecCalcDisPoint.size();
                glm::dvec3* pPoint0 = &this->m_vecCalcDisPoint[nVecCalcDisPointCnt-2];
                glm::dvec3* pPoint1 = &this->m_vecCalcDisPoint[nVecCalcDisPointCnt-1];
                this->m_fCalcDistance = this->m_fCalcDistance + this->CalcPointDistance(pPoint0->x, pPoint0->y, pPoint1->x, pPoint1->y);
            }
            this->UpdateWidgets();
            break;
        }
        case COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_RollerBlind:{
            this->m_nMouseClickFlag = 0;
            if(this->m_pLayerVector->m_nLayerIndxRollerBind != -1)
            {
                this->UpdateWidgets();
            }
            break;
        }
        }
    }else if(nBtnType == 2){//Middle Btn

    }else if(nBtnType == 3){//Left Btn

    }
    this->m_nMouseWorkBegin = 0;

    double fOffsetWidth = 5.0 * this->m_fGeo2ViewScaleH;
    double fOffsetHeight = 5.0 * this->m_fGeo2ViewScaleV;
    double arrMousePointBound[4] = {
        this->m_nGeoMousePosX - fOffsetWidth, this->m_nGeoMousePosY - fOffsetHeight,
        this->m_nGeoMousePosX + fOffsetWidth, this->m_nGeoMousePosY + fOffsetHeight,
    };
    this->m_pLayerVector->MouseRelease(nBtnType, this->m_nGeoMousePosX, this->m_nGeoMousePosY, arrMousePointBound);
}

int COpenGLCore::UpdateCursorType(int nCursorType){
    switch (nCursorType) {
    case 0:{// 捕捉节点
        this->m_pParentWidget->setCursor(Qt::CursorShape::ArrowCursor);
        break;
    }
    case 1:{// 捕捉节点
        this->m_pParentWidget->setCursor(Qt::CursorShape::SizeAllCursor);
        break;
    }
    case 2:{// 捕捉线上
        this->m_pParentWidget->setCursor(Qt::CursorShape::CrossCursor);
        break;
    }
    }
    return 1;
}
//***********************************************************

double COpenGLCore::CalcPointDistance(double fLng0, double fLat0, double fLng1, double fLat1){

    double fMinLat = (fLat0 + fLat1) / 2.0;
    const double fLngLen = 2.0 * PI * 6371000.0;
    double fLatLen = fLngLen * cos((fabs(fMinLat)) / 180.0 * PI);

    double fChaLat = fabs(fLat1 - fLat0) * fLngLen / 360.0;
    double fChaLng = fabs(fLng1 - fLng0) * fLatLen / 360.0;

    double fLenMinMax = sqrt(fChaLat * fChaLat + fChaLng * fChaLng);
    return fLenMinMax;
}

void COpenGLCore::UpdateWidgets(){
    this->m_nMouseStatus = COpenGLCore::e_Mouse_RESET_VIEWPORT;
    this->m_pParentWidget->update();
}
void COpenGLCore::ViewPortResize(int nWndWidth, int nWndHeight) {
    this->m_nWndWidth = nWndWidth;
    this->m_nWndHeight = nWndHeight;

    if(this->m_nWndWidthLast == -999){
        this->m_fGeo2ViewScaleH = 360.0 / 1200.0; //(600.0 * 1920.0 / 1080.0);
        this->m_fGeo2ViewScaleV = 180.0 / 600.0;
    }
    this->InitFrameBufferObj();
    this->InitGeometry();
    this->m_MatOrthoProject = glm::ortho(0.0f, static_cast<GLfloat>(this->m_nWndWidth), 0.0f, static_cast<GLfloat>(this->m_nWndHeight));
    this->glViewport(0, 0, nWndWidth, nWndHeight);

    this->m_nWndWidthLast = this->m_nWndWidth;
    this->m_nWndHeightLast = this->m_nWndHeight;

    this->m_nViewBaseLeftBottomX = 0;
    this->m_nViewBaseLeftBottomY = 0;
    this->m_nViewBaseWidth = this->m_nWndWidth;
    this->m_nViewBaseHeight = this->m_nWndHeight;
    this->m_nMouseStatusLast = this->m_nMouseStatus;
    this->m_nMouseStatus = COpenGLCore::e_Mouse_RESET_VIEWPORT;
    this->UpdateWidgets();
}

void COpenGLCore::SetWndSize(int nWndWidth, int nWndHeight){
    this->m_nWndWidth = nWndWidth;
    this->m_nWndHeight = nWndHeight;

    this->m_MatOrthoProject = glm::ortho(0.0f, static_cast<GLfloat>(this->m_nWndWidth), 0.0f, static_cast<GLfloat>(this->m_nWndHeight));
}

void COpenGLCore::InitGeometry() {

    //this->m_fView2GeoScaleH = (1200.0) / (360.0);
    //this->m_fView2GeoScaleV = (600.0) / (180.0);
    //this->m_fGeo2ViewScaleH = (360.0) / (1200.0);
    //this->m_fGeo2ViewScaleV = (180.0) / (600.0);

    int arrViewMinMax[4] = {0, 0, this->m_nWndWidth, this->m_nWndHeight};
    memcpy(this->m_arrViewMinMax, arrViewMinMax, sizeof(int)*4);

    if(this->m_nWndWidth != this->m_nWndWidthLast || this->m_nWndHeight != this->m_nWndHeightLast) {
        this->m_arrGeoMinMax[2] = this->m_arrGeoMinMax[0] + this->m_fGeo2ViewScaleH * ((double)this->m_nWndWidth);
        this->m_arrGeoMinMax[1] = this->m_arrGeoMinMax[3] - this->m_fGeo2ViewScaleV * ((double)this->m_nWndHeight);
    }

    double fGeoWidth = this->m_arrGeoMinMax[2] - this->m_arrGeoMinMax[0];
    double fGeoHeight = this->m_arrGeoMinMax[3] - this->m_arrGeoMinMax[1];
    this->m_fView2GeoScaleH = ((double)this->m_nWndWidth) / (fGeoWidth);
    this->m_fView2GeoScaleV = ((double)this->m_nWndHeight) / (fGeoHeight);
    this->m_fGeo2ViewScaleH = (fGeoWidth) / ((double)this->m_nWndWidth);
    this->m_fGeo2ViewScaleV = (fGeoHeight) / ((double)this->m_nWndHeight);
    this->m_fGeoWidthHeightScale = fGeoWidth / fGeoHeight;

    this->m_arrScreenGeoBound[0] = this->t_geo_minmax.fGeoLeftBottomX;
    this->m_arrScreenGeoBound[1] = this->t_geo_minmax.fGeoLeftBottomY;

    this->m_arrScreenGeoBound[2] = this->t_geo_minmax.fGeoRightTopX;
    this->m_arrScreenGeoBound[3] = this->t_geo_minmax.fGeoLeftBottomX;

    this->m_arrScreenGeoBound[4] = this->t_geo_minmax.fGeoRightTopX;
    this->m_arrScreenGeoBound[5] = this->t_geo_minmax.fGeoRightTopY;

    this->m_arrScreenGeoBound[6] = this->t_geo_minmax.fGeoLeftBottomX;
    this->m_arrScreenGeoBound[7] = this->t_geo_minmax.fGeoRightTopY;
}
void COpenGLCore::View2GeoPosition(double nViewX, double nViewY, double* pLng, double* pLat){
    *pLng = ((double)nViewX) / this->m_fView2GeoScaleH + this->t_geo_minmax.fGeoLeftBottomX;
    *pLat = ((double)nViewY) / this->m_fView2GeoScaleV + this->t_geo_minmax.fGeoLeftBottomY;

}
void COpenGLCore::Geo2ViewPosition(double fLng, double fLat, double* pViewX, double* pViewY){

    double fViewX = ((fLng - this->t_geo_minmax.fGeoLeftBottomX) * this->m_fView2GeoScaleH);
    double fViewY = ((fLat - this->t_geo_minmax.fGeoLeftBottomY) * this->m_fView2GeoScaleV);
    *pViewX = fViewX;
    *pViewY = fViewY;
}

//***********************************************************

double* COpenGLCore::GetScreenGeoBound(){
    return this->m_arrScreenGeoBound;
    //double* pBound;
    //pBound[0] = this->t_geo_minmax.fGeoLeftBottomX;
    //pBound[1] = this->t_geo_minmax.fGeoLeftBottomY;
    //
    //pBound[2] = this->t_geo_minmax.fGeoRightTopX;
    //pBound[3] = this->t_geo_minmax.fGeoLeftBottomX;
    //
    //pBound[4] = this->t_geo_minmax.fGeoRightTopX;
    //pBound[5] = this->t_geo_minmax.fGeoRightTopY;
    //
    //pBound[6] = this->t_geo_minmax.fGeoLeftBottomX;
    //pBound[7] = this->t_geo_minmax.fGeoRightTopY;
}

int COpenGLCore::GetGeoIntersectRangeByView(double* pOtherRnage, double* pIntersectRange){

    double xMin = pOtherRnage[0];
    double yMin = pOtherRnage[1];
    double xMax = pOtherRnage[2];
    double yMax = pOtherRnage[3];

    if(this->t_geo_minmax.fGeoLeftBottomX >= xMax || this->t_geo_minmax.fGeoLeftBottomY >= yMax ||
            this->t_geo_minmax.fGeoRightTopX <= xMin || this->t_geo_minmax.fGeoRightTopY <= yMin){
        return 0;
    }

    if(this->t_geo_minmax.fGeoLeftBottomX > xMin){
        xMin = this->t_geo_minmax.fGeoLeftBottomX;
    }
    if(this->t_geo_minmax.fGeoLeftBottomY > yMin){
        yMin = this->t_geo_minmax.fGeoLeftBottomY;
    }
    if(this->t_geo_minmax.fGeoRightTopX < xMax){
        xMax = this->t_geo_minmax.fGeoRightTopX;
    }
    if(this->t_geo_minmax.fGeoRightTopY < yMax){
        yMax = this->t_geo_minmax.fGeoRightTopY;
    }
    pIntersectRange[0] = xMin;
    pIntersectRange[1] = yMin;
    pIntersectRange[2] = xMax;
    pIntersectRange[3] = yMax;

    return 1;
}

int COpenGLCore::GetGeoIntersectRangeByViewEx(double* pOtherRnage, double* pIntersectRange){

    double xMin = pOtherRnage[0];
    double yMin = pOtherRnage[1];
    double xMax = pOtherRnage[2];
    double yMax = pOtherRnage[3];

    double fGeoLeftBottomPosX = 0;
    double fGeoLeftBottomPosY = 0;
    double fGeoRightTopPosX = 0;
    double fGeoRightTopPosY = 0;
    fGeoLeftBottomPosX = this->t_geo_minmax.fGeoLeftBottomX;
    fGeoLeftBottomPosY = this->t_geo_minmax.fGeoLeftBottomY;
    fGeoRightTopPosX = this->t_geo_minmax.fGeoRightTopX;
    fGeoRightTopPosY = this->t_geo_minmax.fGeoRightTopY;
    if (this->m_nMouseClickFlag == 1 && this->m_nMouseWorkStatus == COpenGLCore::EMOUSE_WORK_STATUS::e_Mouse_Work_RollerBlind){
        switch (this->m_nRollerBlindArrow) {
        case 1:{//上
            fGeoRightTopPosY = this->m_nGeoMousePosY;
            break;
        }
        case 2:{//下
            fGeoLeftBottomPosY = this->m_nGeoMousePosY;
            break;
        }
        case 3:{//左
            fGeoLeftBottomPosX = this->m_nGeoMousePosX;
            break;
        }
        case 4:{//右
            fGeoRightTopPosX = this->m_nGeoMousePosX;
            break;
        }
        }
    }

    if(fGeoLeftBottomPosX >= xMax || fGeoLeftBottomPosY >= yMax ||
            fGeoRightTopPosX <= xMin || fGeoRightTopPosY <= yMin){
        return 0;
    }

    if(fGeoLeftBottomPosX > xMin){
        xMin = fGeoLeftBottomPosX;
    }
    if(fGeoLeftBottomPosY > yMin){
        yMin = fGeoLeftBottomPosY;
    }
    if(fGeoRightTopPosX < xMax){
        xMax = fGeoRightTopPosX;
    }
    if(fGeoRightTopPosY < yMax){
        yMax = fGeoRightTopPosY;
    }
    pIntersectRange[0] = xMin;
    pIntersectRange[1] = yMin;
    pIntersectRange[2] = xMax;
    pIntersectRange[3] = yMax;

    return 1;
}

//***********************************************************
int COpenGLCore::DrawCalcDistanceTrack(){
    int nPointCnt = (int)this->m_vecCalcDisPoint.size();
    if(nPointCnt <= 0){
        return 0;
    }

    glm::dvec3* pPoint = &this->m_vecCalcDisPoint[nPointCnt - 1];
    double fViewX = 0.0, fViewY = 0.0;
    this->Geo2ViewPosition(pPoint->x, pPoint->y, &fViewX, &fViewY);

    this->DrawLine((float)this->m_nMousePosX, (float)this->m_nMousePosY, (float)fViewX, (float)fViewY, 0xFF00FFFF);


    this->View2GeoPosition(this->m_nMousePosX, this->m_nMousePosY, &fViewX, &fViewY);
    double fTmpDist = this->CalcPointDistance(pPoint->x, pPoint->y, fViewX, fViewY);

    this->m_sMsg = this->m_sMsg + ", CalcDis: " + std::to_string(this->m_fCalcDistance + fTmpDist);
    return 0;
}

int COpenGLCore::DrawCalcDistanceTrackFBO(){
    int nPointCnt = (int)this->m_vecCalcDisPoint.size();
    if(nPointCnt < 2){
        return 0;
    }
    int i = 0;
    std::vector<unsigned int> vecDrawPointIndx;
    for (i = 1; i<nPointCnt; i++){
        vecDrawPointIndx.push_back(i - 1);
        vecDrawPointIndx.push_back(i);
    }
    int nPointElementCnt = (int)vecDrawPointIndx.size();
    this->DrawLinesElement(nPointCnt, (double*)this->m_vecCalcDisPoint.data(), nPointElementCnt, vecDrawPointIndx.data(), 0xFF00FFFF);
    return 1;
}
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
//***********************************************************
