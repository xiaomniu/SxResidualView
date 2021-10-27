#ifndef CDRAWGROUND_H
#define CDRAWGROUND_H

class COpenGLCore;
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Src_GlWnd/COpenGLCore.h"

class CDrawGround
{
public:
    CDrawGround(COpenGLCore* pGLCore);
    ~CDrawGround();
    COpenGLCore* m_pGLCore = nullptr;

    void CreateSphere();

    void InitScene_bak();
    void Draw_bak();

    void SetTifFilePath(const char* szTifFileFullPath, const char* szDemFileFullPath);
    void InitShader();
    void InitGroundMap();
    void InitGround();
    void BindGround();
    void InitScene(const char* szTifFileFullPath = nullptr, const char* szDemFileFullPath = nullptr);
    void Draw();

    void DrawBound();
    void DrawTiff();


    float GetHeight(int x, int z);
    float GetHeight_bak(int x, int z);
    void  GetNormal(int x, int z, float *normal) ;


    float m_ambientColor[4] = { 0.4f,0.4f,0.4f,1.0f };
    float m_diffuseColor[4] = { 1.0f,1.0f,1.0f,1.0f };
    float m_specularColor[4] = { 1.0f,1.0f,1.0f,1.0f };
    float m_ambientMaterial[4] = { 0.1f,0.1f,0.1f,1.0f };
    float m_diffuseMaterial[4] = { 0.6f,0.6f,0.6f,1.0f };
    float m_specularMaterial[4] = { 0.0f,0.0f,0.0f,1.0f };
    float m_lightPos[4] = {0.0f,1.0f,1.0f,0.0f};

    glm::mat4 Model, View, Projection;
    GLuint m_MapTexture;
    GLint  m_ModelID, m_ViewID, m_ProjectionID, m_GrassTextureID;
    GLint m_PositionID, m_NormalID, m_TexcoordID;
    GLuint m_ProgramGround;
    GLuint m_GroundVBO;

    struct VertexData{
        float Position[4];
        float Normal[4];
        float Texcoord[4];
    };
    std::string m_sTextImgFile;
    std::string m_sTifFileFullPath, m_sDemFileFullPath;
    int m_nPixW, m_nPixH;
    float *m_pHeightMapDatas = nullptr;
    int m_nHeightMapWidth, m_nHeightMapHeight;
    VertexData* m_pTerrainData = nullptr;

    int m_nDrawReady = 0;
    static int DrawPoint(void* pParam);
};

#endif // CDRAWGROUND_H
