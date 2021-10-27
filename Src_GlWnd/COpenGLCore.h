#ifndef COPENGLCORE_H
#define COPENGLCORE_H

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLExtraFunctions>
#include <QtGui/QOpenGLFunctions_4_1_Compatibility>
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include <vector>
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "SOIL2/SOIL2.h"

#ifndef PI
#define PI 3.1415926535897932384626433832795
#define PI_2 PI/2.0
#define PI_4 PI/4.0
#endif

typedef unsigned int color32;

//struct Vec3 {
//    Vec3(float x, float y, float z){
//        Position[0] = x;
//        Position[1] = y;
//        Position[2] = z;
//    }
//    float Position[3];
//};
//struct Vec4 {
//    Vec4(float x, float y, float z, float w){
//        Position[0] = x;
//        Position[1] = y;
//        Position[2] = z;
//        Position[3] = w;
//    }
//    float Position[4];
//};

struct Vertex{
    float Position[4];
    float Color[4];
    float Texcoord[4];
    float Normal[4];
};
class VertexBuffer {
public:
    Vertex *mVertexes;
    int mVertexCount;
    GLuint mVBO;
    void SetSize(int vertexCount);
    void SetPosition(int index, float x, float y, float z, float w = 1.0f);
    void SetColor(int index, float r, float g, float b, float a = 1.0);
    void SetTexcoord(int index, float x, float y);
    void SetNormal(int index, float x, float y, float z);
    void Bind();
    void Unbind();
    Vertex&Get(int index);
};

class CFontRenderer;
class CDrawGround;
class CBaseMap;
class CRasterDraw;
class CLayerVector;
class CSoilTool;
class COpenGLCore:public QOpenGLFunctions_4_1_Compatibility, public QOpenGLWidget//QOpenGLExtraFunctions
{
public:
    COpenGLCore();
    ~COpenGLCore();
    int SetGLFuncCore();

    GLuint CompileShader(GLuint shader);
    GLuint LinkProgram(GLuint program);

    enum Cull           { CULL_NONE, CULL_CW, CULL_CCW };

public:
    CSoilTool* m_pSoilTool = nullptr;
    ////////////////////////////////

    void CalcRotatePosition(float fSrcX, float fSrcY, float fBaseX, float fBaseY, float fTheta, float* pfDstX, float* pfDstY);
    ////////////////////////////////
    virtual int Draw();
    void        InitSence();
    void        TestDraw();
    void        InitSceneItem();

    ////////////////////////////////
    CDrawGround* m_pDrawGround = nullptr;
    CFontRenderer* m_pFontRender = nullptr;
    CBaseMap* m_pBaseMap = nullptr;
    CRasterDraw* m_pRasterDraw = nullptr;
    CLayerVector* m_pLayerVector = nullptr;

    QOpenGLWidget* m_pParentWidget = nullptr;

    ////////////////////////////////
    GLuint shader001Program;
    glm::mat4 m001MVP;
    glm::mat4 m001ViewMatrix;
    GLuint mMVP001ID;
    GLuint m001VBOPos, m001VBOCls, m001VAO;
    void Test001Init();
    void Test001Draw();

    glm::mat4 m_MatOrthoProject;
    glm::mat4 m_MatPerspectProject;
    glm::mat4 m_MatModel;
    glm::mat4 m_MatView;
    glm::mat4 m_MatMVPCore;
    ////////////////////////////////
    GLuint m_ProgramCircle;
    GLuint m_CircleProjectID;
    GLint m_CircleVertexID;
    GLint m_CircleColorID;
    GLuint m_CircleVAO;
    GLuint m_CirclePosVBO;
    GLuint m_CircleClsVBO;
    //std::vector<glm::vec3> m_vecCirclePts;
    //void InitCircle();
    //void DrawCircle(float nCenterX, float nCenterY, float fRadius, unsigned int nColor);
    //void InitEllipse();
    //void DrawEllipse(float nCenterX, float nCenterY, float fRadiusLarge, float fRadiusShort, unsigned int nColor);
    //void DrawPoints(int nPointCnt, float* pPoints, unsigned int nColor);
    //void DrawPoint(float fPosX, float fPosY, unsigned int nColor);
    //void DrawLines(int nPointCnt, float* pPoints, unsigned int nColor);
    //void DrawLine(float fPosXS, float fPosYS, float fPosXE, float fPosYE, unsigned int nColor);

    void InitCircle();
    void DrawCircle(float nCenterX, float nCenterY, float fRadius, unsigned int nColor, float fLineSize = 1.0f);
    void InitEllipse();
    void DrawEllipse(float nCenterX, float nCenterY, float fRadiusLarge, float fRadiusShort, unsigned int nColor, float fLineSize = 1.0f);
    void DrawPoints(int nPointCnt, float* pPoints, unsigned int nColor, float fPointSize = 5.0f);
    void DrawPoint(float fPosX, float fPosY, unsigned int nColor, float fPointSize = 5.0f);
    void DrawLines(int nPointCnt, float* pPoints, unsigned int nColor, float fLineSize = 1.0f);
    void DrawLine(float fPosXS, float fPosYS, float fPosXE, float fPosYE, unsigned int nColor, float fLineSize = 1.0f);
    void DrawRect(float fPosXS, float fPosYS, float fPosXE, float fPosYE, unsigned int nColor, float fLineSize = 1.0f);
    void DrawPolygon(int nPointCnt, float* pPoints, unsigned int nColor, float fLineSize = 1.0f);
    void DrawTriPolygon(int nPointCnt, float* pPoints, unsigned int nColor, float fLineSize = 1.0f);

    GLuint      m_ProgramTexture;
    GLint       m_TextureProjectID;
    //GLuint      m_TextureID;
    GLuint      m_TextureVAO;
    GLuint      m_TextureVBO;
    GLint       m_TextureSampleText2D;
    void InitImage();
    void DrawImage(float xPos, float yPos, float fWidth, float fHeight, const char* szImgFileFullPath, int nReverY = 0);
    void DrawImage(int nViewPointsCnt, float* pVewPoints, GLuint nTextureID, int nReverY = 0);

    ////////////////////////////////
    void InitLinesElement();
    void DrawLinesElement(int nPointCnt, double* pPoints, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fLineSize = 1.0f);
    void DrawLinesLoopElement(int nPointCnt, double* pPoints, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fLineSize = 1.0f);
    void DrawPointsElement(int nPointCnt, double* pPoints, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fPointSize = 5.0f);

    GLuint m_ProgramLinesElement;
    GLuint m_LinesElementProjectID;
    GLint m_LinesElementVertexID;
    GLint m_LinesElementViewGeoID;
    //GLint m_LinesElementViewGeoIDx;
    //GLint m_LinesElementViewGeoIDy;
    //GLint m_LinesElementViewGeoIDz;
    //GLint m_LinesElementViewGeoIDw;
    GLint m_LinesElementGeoSelectRangeID;
    GLint m_LinesElementEnableSelecID;
    GLint m_LinesElementColorID;
    GLuint m_LinesElementVAO;
    GLuint m_LinesElementPosVBO;
    GLuint m_LinesElementClsVBO;
    GLuint m_LinesElementPosVEO;
    ////////////////////////////////

    void InitLinesElementEx();
    void DrawLinesElementEx(int nPointCnt, double* pPoints, int nPointFlagsCnt, float* pPointsFlags, int nPointElementCnt, unsigned int* pPointsElement, unsigned int nColor, float fLineSize = 1.0f);
    void DrawLinesElementOperate(int nPointCnt, double* pPoints,
                             int nPointFlagsCnt, float* pPointsFlags,
                             int nPointElementCnt, unsigned int* pPointsElement,
                             int nPointFlagsCntOperate, float* pPointsFlagsOperate,
                             int nPointElementCntOperate, unsigned int* pPointsElementOperate,
                             int nPointElementCntOperateNode, unsigned int* pPointsElementOperateNode,
                             int nPointElementCntOperateHit, unsigned int* pPointsElementOperateHit,
                             int nPointElementCntOperateNodeHit, unsigned int* pPointsElementOperateNodeHit,
                             unsigned int nColor, float fLineSize = 1.0f);
    void DrawPointsElementOperate(int nPointCnt, double* pPoints,
                             int nPointFlagsCnt, float* pPointsFlags,
                             int nPointElementCnt, unsigned int* pPointsElement,
                             int nPointFlagsCntOperate, float* pPointsFlagsOperate,
                             int nPointElementCntOperate, unsigned int* pPointsElementOperate,
                             int nPointElementCntOperateHit, unsigned int* pPointsElementOperateHit,
                             unsigned int nColor, float fPointSize = 6.0f);
    GLuint m_ProgramLinesElementEx;
    GLuint m_LinesElementExProjectID;
    GLint m_LinesElementExVertexID;
    GLint m_LinesElementExVertexFlagID;
    GLint m_LinesElementExViewGeoID;
    GLint m_LinesElementExViewGeoIDx;
    GLint m_LinesElementExViewGeoIDy;
    GLint m_LinesElementExViewGeoIDz;
    GLint m_LinesElementExViewGeoIDw;
    GLint m_LinesElementExGeoSelectRangeID;
    GLint m_LinesElementExEnableSelecID;
    GLint m_LinesElementExColorID;
    GLuint m_LinesElementExVAO;
    GLuint m_LinesElementExPosVBO;
    GLuint m_LinesElementExPosFlagVBO;
    GLuint m_LinesElementExPosVEO;

    ////////////////////////////////
    GLuint m_ProgramDraw2Dd;
    GLuint m_Draw2DdProjectID;
    GLint m_Draw2DdVertexID;
    GLint m_Draw2DdVertexIDX;
    GLint m_Draw2DdVertexIDY;
    GLint m_Draw2DdViewGeoID;
    GLint m_Draw2DdColorID;
    GLuint m_Draw2DdVAO;
    GLuint m_Draw2DdPosVBO;
    GLuint m_Draw2DdClsVBO;
    void InitDraw2Dd();
    void DrawTrianglesd(int nPointCnt, double* pPointsX, double* pPointsY, double* pPointsZ, unsigned int nColor, float fLineSize = 1.0f);

    ////////////////////////////////

    GLuint m_ProgramTriangle;
    glm::mat4 m_MatTriangleMVP;
    GLint  m_TriangleMVPID;
    GLint  m_TriangleVertexID;
    GLint  m_TriangleColorID;
    GLuint m_TriangleVAO;
    GLuint m_TrianglePosVBO;
    GLuint m_TriangleClsVBO;
    void InitTriangles();
    //void DrawTriangles(std::vector<glm::vec3>& vecPoints, unsigned int nColor);
    void DrawTriangles(int nTrianglesCnt, float* pPoints, unsigned int nColor);

    ////////////////////////////////
    void DrawString(const char* szContent, float fPosX, float fPosY, float fScale, unsigned int nColor);
    ////////////////////////////////

    GLuint m_ProgramLine3D;
    glm::mat4 m_MatLine3DMVP;
    GLint  m_Line3DMVPID;
    GLint m_Line3DVertexID;
    GLint m_Line3DColorID;
    GLuint m_Line3DVAO;
    GLuint m_Line3DPosVBO;
    void InitLine3D();
    void DrawLine3D(float* pPoints, unsigned int nColor);
    void DrawLines3D(int nPointCnt, float* pPoints, unsigned int nColor);
    ////////////////////////////////


    GLuint m_ProgramTexture3D;
    glm::mat4 m_MatTexture3DMVP;
    GLint m_Texture3DMVPID;
    GLint m_Texture3DVertexID;
    GLint m_Texture3DCoordID;
    GLuint m_Texture3DVAO;
    GLuint m_Texture3DPosVBO;
    GLuint m_Texture3DCoordVBO;
    GLint m_TextureSampleText3D;
    void InitImage3D();
    void DrawImage3D(float* pPoints, const char* szImgFileFullPath, int nReverY = 0);
    ////////////////////////////////

    int         InitBase();
    int         Shut();
    void        BeginDraw(int _WndWidth, int _WndHeight);
    void        EndDraw();
    bool        IsDrawing();
    void        Restore();//
    void        DrawLine(GLfloat ffx , GLfloat ffy, GLfloat ffx1, GLfloat ffy1, unsigned int _Color0, unsigned int _Color1, bool _AntiAliased=false);
    void        DrawLine(int _X0, int _Y0, int _X1, int _Y1, unsigned int _Color0, unsigned int _Color1, bool _AntiAliased=false);
    void        DrawLine(int _X0, int _Y0, int _X1, int _Y1, unsigned int _Color, bool _AntiAliased=false) { DrawLine(_X0, _Y0, _X1, _Y1, _Color, _Color, _AntiAliased); }
    void        DrawRect(int _X0, int _Y0, int _X1, int _Y1, unsigned int _Color00, unsigned int _Color10, unsigned int _Color01, unsigned int _Color11);
    void        DrawRect(int _X0, int _Y0, int _X1, int _Y1, unsigned int _Color) { DrawRect(_X0, _Y0, _X1, _Y1, _Color, _Color, _Color, _Color); }
    void        DrawTriangles(int _NumTriangles, int *_Vertices, unsigned int *_Colors, Cull _CullMode);

    void *      NewTextObj();
    void        DeleteTextObj(void *_TextObj);
   // void        BuildText(void *_TextObj, const std::string *_TextLines, unsigned int *_LineColors, unsigned int *_LineBgColors, int _NbLines, const CTexFont *_Font, int _Sep, int _BgWidth);
    void        DrawText(void *_TextObj, int _X, int _Y, unsigned int _Color, unsigned int _BgColor);

    void        ChangeViewport(int _X0, int _Y0, int _Width, int _Height, int _OffsetX, int _OffsetY);
    void        RestoreViewport();
    void        SetScissor(int _X0, int _Y0, int _Width, int _Height);

protected:
    bool                m_Drawing;
    GLuint              m_FontTexID;
    //const CTexFont *    m_FontTex;

    GLfloat             m_PrevLineWidth;
    GLint               m_PrevActiveTexture;
    GLint               m_PrevTexture;
    GLint               m_PrevVArray;
    GLboolean           m_PrevLineSmooth;
    GLboolean           m_PrevCullFace;
    GLboolean           m_PrevDepthTest;
    GLboolean           m_PrevBlend;
    GLint               m_PrevSrcBlend;
    GLint               m_PrevDstBlend;
    GLboolean           m_PrevScissorTest;
    GLint               m_PrevScissorBox[4];
    GLint               m_PrevViewport[4];
    GLuint              m_PrevProgramObject;

    GLuint              m_LineRectVS;
    GLuint              m_LineRectFS;
    GLuint              m_LineRectProgram;
    GLuint              m_LineRectVArray;
    GLuint              m_LineRectVertices;
    GLuint              m_LineRectColors;
    GLuint              m_TriVS;
    GLuint              m_TriFS;
    GLuint              m_TriProgram;
    GLuint              m_TriUniVS;
    GLuint              m_TriUniFS;
    GLuint              m_TriUniProgram;
    GLuint              m_TriTexVS;
    GLuint              m_TriTexFS;
    GLuint              m_TriTexProgram;
    GLuint              m_TriTexUniVS;
    GLuint              m_TriTexUniFS;
    GLuint              m_TriTexUniProgram;
    GLuint              m_TriVArray;
    GLuint              m_TriVertices;
    GLuint              m_TriUVs;
    GLuint              m_TriColors;
    GLint               m_TriLocationOffset;
    GLint               m_TriLocationWndSize;
    GLint               m_TriUniLocationOffset;
    GLint               m_TriUniLocationWndSize;
    GLint               m_TriUniLocationColor;
    GLint               m_TriTexLocationOffset;
    GLint               m_TriTexLocationWndSize;
    GLint               m_TriTexLocationTexture;
    GLint               m_TriTexUniLocationOffset;
    GLint               m_TriTexUniLocationWndSize;
    GLint               m_TriTexUniLocationColor;
    GLint               m_TriTexUniLocationTexture;
    size_t              m_TriBufferSize;


    struct Vec2         { GLfloat x, y; Vec2(){} Vec2(GLfloat _X, GLfloat _Y):x(_X),y(_Y){} Vec2(int _X, int _Y):x(GLfloat(_X)),y(GLfloat(_Y)){} };
    struct CTextObj
    {
        std::vector<Vec2>       m_TextVerts;
        std::vector<Vec2>       m_TextUVs;
        std::vector<Vec2>       m_BgVerts;
        std::vector<unsigned int>    m_Colors;
        std::vector<unsigned int>    m_BgColors;
    };
    void                ResizeTriBuffers(size_t _NewSize);

public:
    void UpdateWidgets();
    void ViewPortResize(int nWndWidth, int nWndHeight);
    void SetWndSize(int nWndWidth, int nWndHeight);

    //////////////////////////////////////////// Shader
    struct UniformTexture {
        GLint mLocation;
        GLuint mTexture;
        UniformTexture() {
            mLocation = -1;
            mTexture = 0;
        }
    };
//class Shader {
public:
    GLuint mProgram;
    GLuint mPosition;
    GLuint mColor;
    GLuint mTexcoord;
    GLuint mNormal;
    std::map<std::string, UniformTexture*> mUniformTextures;
    GLint mModelMatrixLocation, mViewMatrixLocation, mProjectionMatrixLocation;
    GLint mPositionLocation, mColorLocation, mTexcoordLocation, mNormalLocation;
    void ShaderUse();
    void ShaderInit(const char* szFileVS, const char* szFileFS);
    void ShaderBind(float *M, float *V, float*P);
    void ShaderSetTexture(const char * name, const char*imagePath);
//};

    unsigned char * LoadFileContent(const char*path, int&filesize);
    GLuint CompileShader(GLenum shaderType, const char*shaderCode);
    GLuint CreateProgram(GLuint vsShader, GLuint fsShader);
    GLuint CreateProgram(const char* szCodeVS, const char* szCodeFS);
    float GetFrameTime();
    unsigned char * DecodeBMP(unsigned char*bmpFileData, int&width, int&height);
    GLuint CreateTexture2D(unsigned char*pixelData, int width, int height, GLenum type);
    GLuint CreateTexture2DFromBMP(const char *szBmpImgFilePath, int nChannelCnt = 3);
    GLuint CreateTexture2DFromPNG(const char *szPngJpgImgFilePath, int invertY = 0);
    GLuint CreateTexture2DFromRaster(const char *szRasterImgFilePath);
    GLuint CreateTexture2DFromImgBuffer(const char *szImgBufferName, unsigned char* pPixBuffer, int nPixWidth, int nPixHeight, int nChannelCnt = 3);
    GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data = nullptr);
    GLuint LoadTextureFromFile(const char* szImgFilePath, bool invertY = false);
    int RemoveTextureByFile(const char* szImgFilePath);
    std::unordered_map<std::string, GLuint> m_mapImgPath2TextureID;
    ////////////////////////////////////////////   Texture

public: // FrameBufferObject
    GLuint m_FboColorTextureID;
    GLuint m_FboDepthTextureID;
    GLuint m_FrameBufferObject = -1;
    GLint m_PrevFrameBuffer;
    std::map<std::string, GLuint> m_mapBuffers;
    std::vector<GLenum> m_vecDrawBuffers;
    void InitFrameBufferObj();
    void FboAttachColorBuffer(const char*bufferName,GLenum attachment,int width,int height);
    void FboAttachDepthBuffer(const char*bufferName, int width, int height);
    void FboFinish();
    void FboBind();
    void FboUnbind();
//****************************************
    int m_nWndWidth = 0;
    int m_nWndHeight = 0;
    int m_nWndWidthLast = -999;
    int m_nWndHeightLast = -999;
    int m_nOffsetX = 0;
    int m_nOffsetY = 0;

    enum EMOUSE_STATUS{
        e_Mouse_Unknow = 0,
        e_Mouse_MIDDLE_DOWN_MOVING,
        e_Mouse_WHEEL_SCROLLING,
        e_Mouse_MOVING,
        e_Mouse_RESET_VIEWPORT,
        e_Mouse_RIGHT_DOWN_MOVING,
        e_Camera_MOVING,
    };
    enum EMOUSE_WORK_STATUS{
        e_Mouse_Work_Unknow = 0,
        e_Mouse_Work_Select,
        e_Mouse_Work_CalcDistance,
        e_Mouse_Work_RollerBlind,
        e_Mouse_Work_SelectOnePoint,
    };
    //  e_Mouse_Work_CalcDistance /////////////
    int m_nCalcDistLastSelectPoint;
    double m_fCalcDistance = 0.0;
    std::vector<glm::dvec3> m_vecCalcDisPoint;
    int DrawCalcDistanceTrackFBO();
    int DrawCalcDistanceTrack();

    double CalcPointDistance(double fLng0, double fLat0, double fLng1, double fLat1);
    //--------------------
    unsigned long long m_nExtParamData = 0;
    int m_nRollerBlindArrow = 0;
    int m_nMouseClickFlag = 0;
    int m_nMouseWorkBegin = 0;
    int m_nMouseStatus = 0, m_nMouseStatusLast = 0;
    int m_nMouseMovingDeltaX = 0, m_nMouseMovingDeltaY = 0;
    int m_nMouseMovingLastDeltaX = 0, m_nMouseMovingLastDeltaY = 0;
    float m_fMouseMovingDeltaX = 0, m_fMouseMovingDeltaY = 0;
    float m_nWheelOffXSrc = 0, m_nWheelOffYSrc = 0;
    float m_nWheelOffX = 0, m_nWheelOffY = 0;
    float m_nWheelScale = 1.0f, m_nWheelScaleLast = 0;
    int m_nMouseWorkStatus = 0, m_nMouseWorkStatusLast = 0;
    double m_nMouseSelectPosStartX = 0, m_nMouseSelectPosStartY = 0, m_nMouseSelectPosEndX = 0, m_nMouseSelectPosEndY = 0;
    double m_arrMouseSelectPoints[4];
    //QTimer* m_pWheelTimer = nullptr;
    QTimer m_WheelTimer;

    float m_nViewBaseLeftBottomX = 0, m_nViewBaseLeftBottomY = 0, m_nViewBaseWidth = 0, m_nViewBaseHeight = 0;

    int m_nWheelFlag = 0;
    int m_nWheelPow = 0;
    float m_fPowNumMul = 0;
    float m_fPowNumDev = 0;

    void ResetMidleDownMovingPos();
    void UpdateMidleDownMovingPos();
    void MidleDownMoving(int xDelta, int yDelta);
    void MidleDownMovingEnd();
    void WheelEvent(int nDelta, int xMousePos, int yMousePos);
    void WheelEventEnd();
    bool WheelTimerProc();
    void MouseMoving(int xPos, int yPos);
    void MousePress(int nBtnType, int xMousePos, int yMousePos);
    void MouseDbPress(int nBtnType, int xMousePos, int yMousePos);
    void MouseRelease(int nBtnType, int xMousePos, int yMousePos);
//****************************************
    union{
        struct T_GEO_MINMAX{
            double fGeoLeftBottomX;
            double fGeoLeftBottomY;
            double fGeoRightTopX;
            double fGeoRightTopY;
        }t_geo_minmax;
        double m_arrGeoMinMax[4];
    };
    double m_arrScreenGeoBound[8];
    //double m_arrGeoMinMax1[4] = {-180.0, -90.0, 180.0, 90.0};//左下角 - 右上角
    int    m_arrViewMinMax[4];//左下角 - 右上角
    double m_fView2GeoScaleH;
    double m_fView2GeoScaleV;
    double m_fGeo2ViewScaleH;
    double m_fGeo2ViewScaleV;
    double m_fGeoWidthHeightScale = 0.0;

    std::string m_sMsg;
    int m_nMousePosX, m_nMousePosY;
    double m_nGeoMousePosX, m_nGeoMousePosY;
    int UpdateCursorType(int nCursorType);

    int m_nNeedUpdate = 0;
    void InitGeometry();

    void View2GeoPosition(double nViewX, double nViewY, double* pLng, double* pLat);
    void Geo2ViewPosition(double fLng, double fLat, double* pViewX, double* pViewY);
//****************************************

    double* GetScreenGeoBound();
    int GetGeoIntersectRangeByView(double* pOtherRnage, double* pIntersectRange);
    int GetGeoIntersectRangeByViewEx(double* pOtherRnage, double* pIntersectRange);
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************
//****************************************

    std::string m_sExeDir;
    std::string m_sExeFullPath;
//private:
    void DrawImage(float xPos, float yPos, float fWidth, float fHeight, GLuint nTextureID, int nReverY = 0);
};

#endif // COPENGLCORE_H
