#include "CDrawGround.h"

#include <stdlib.h>
#include <thread>

#include "Src_Core/Misc.h"
#include "Src_Geometry/CReadRaster.h"

#ifdef _DEBUG
    void CheckGLCoreError(const char *file, int line, const char *func, COpenGLCore* pCore);
#   ifdef __FUNCTION__
#       define CHECK_GL_ERROR_1(a) CheckGLCoreError(__FILE__, __LINE__, __FUNCTION__, a)
#   else
#       define CHECK_GL_ERROR_1(a) CheckGLCoreError(__FILE__, __LINE__, "", a)
#   endif
#else
#   define CHECK_GL_ERROR_1(a)// ((void)(0))
#endif

CDrawGround::CDrawGround(COpenGLCore* pGLCore)
{
    srand(time(0));
    this->m_pGLCore = pGLCore;;
}

CDrawGround::~CDrawGround()
{
}



const color32 COLOR32_BLACK     = 0xff000000;   // Black
const color32 COLOR32_WHITE     = 0xffffffff;   // White
const color32 COLOR32_ZERO      = 0x00000000;   // Zero
const color32 COLOR32_RED       = 0xffff0000;   // Red
const color32 COLOR32_GREEN     = 0xff00ff00;   // Green
const color32 COLOR32_BLUE      = 0xff0000ff;   // Blue】


template <typename _T> inline const _T& TClamp(const _T& _X, const _T& _Limit1, const _T& _Limit2)
{
    if( _Limit1<_Limit2 )
        return (_X<=_Limit1) ? _Limit1 : ( (_X>=_Limit2) ? _Limit2 : _X );
    else
        return (_X<=_Limit2) ? _Limit2 : ( (_X>=_Limit1) ? _Limit1 : _X );
}

inline color32 Color32FromARGBi(int _A, int _R, int _G, int _B)
{
    return (((color32)TClamp(_A, 0, 255))<<24) | (((color32)TClamp(_R, 0, 255))<<16) | (((color32)TClamp(_G, 0, 255))<<8) | ((color32)TClamp(_B, 0, 255));
}

inline color32 Color32FromARGBf(float _A, float _R, float _G, float _B)
{
    return (((color32)TClamp(_A*256.0f, 0.0f, 255.0f))<<24) | (((color32)TClamp(_R*256.0f, 0.0f, 255.0f))<<16) | (((color32)TClamp(_G*256.0f, 0.0f, 255.0f))<<8) | ((color32)TClamp(_B*256.0f, 0.0f, 255.0f));
}

inline void Color32ToARGBi(color32 _Color, int *_A, int *_R, int *_G, int *_B)
{
    if(_A) *_A = (_Color>>24)&0xff;
    if(_R) *_R = (_Color>>16)&0xff;
    if(_G) *_G = (_Color>>8)&0xff;
    if(_B) *_B = _Color&0xff;
}

inline void Color32ToARGBf(color32 _Color, float *_A, float *_R, float *_G, float *_B)
{
    if(_A) *_A = (1.0f/255.0f)*float((_Color>>24)&0xff);
    if(_R) *_R = (1.0f/255.0f)*float((_Color>>16)&0xff);
    if(_G) *_G = (1.0f/255.0f)*float((_Color>>8)&0xff);
    if(_B) *_B = (1.0f/255.0f)*float(_Color&0xff);
}

void ColorRGBToHLSf(float _R, float _G, float _B, float *_Hue, float *_Light, float *_Saturation);

void ColorRGBToHLSi(int _R, int _G, int _B, int *_Hue, int *_Light, int *_Saturation);

void ColorHLSToRGBf(float _Hue, float _Light, float _Saturation, float *_R, float *_G, float *_B);

void ColorHLSToRGBi(int _Hue, int _Light, int _Saturation, int *_R, int *_G, int *_B);

color32 ColorBlend(color32 _Color1, color32 _Color2, float _S)
{
    float a1, r1, g1, b1, a2, r2, g2, b2;
    Color32ToARGBf(_Color1, &a1, &r1, &g1, &b1);
    Color32ToARGBf(_Color2, &a2, &r2, &g2, &b2);
    float t = 1.0f-_S;
    return Color32FromARGBf(t*a1+_S*a2, t*r1+_S*r2, t*g1+_S*g2, t*b1+_S*b2);
}


float m_Permute[3][3] = {0};   // Permute frame axis


static inline float QuatD(int w, int h)
{
    return (float)std::min(abs(w), abs(h)) - 4;
}

static inline int QuatPX(float x, int w, int h)
{
    return (int)(x*0.5f*QuatD(w, h) + (float)w*0.5f + 0.5f);
}

static inline int QuatPY(float y, int w, int h)
{
    return (int)(-y*0.5f*QuatD(w, h) + (float)h*0.5f - 0.5f);
}

void ApplyQuat(float *outX, float *outY, float *outZ, float x, float y, float z, float qx, float qy, float qz, float qs)
{
    float ps = - qx * x - qy * y - qz * z;
    float px =   qs * x + qy * z - qz * y;
    float py =   qs * y + qz * x - qx * z;
    float pz =   qs * z + qx * y - qy * x;
    *outX = - ps * qx + px * qs - py * qz + pz * qy;
    *outY = - ps * qy + py * qs - pz * qx + px * qz;
    *outZ = - ps * qz + pz * qs - px * qy + py * qx;
}

void Permute(float *outX, float *outY, float *outZ, float x, float y, float z)
{
    float px = x, py = y, pz = z;
    *outX = m_Permute[0][0]*px + m_Permute[1][0]*py + m_Permute[2][0]*pz;
    *outY = m_Permute[0][1]*px + m_Permute[1][1]*py + m_Permute[2][1]*pz;
    *outZ = m_Permute[0][2]*px + m_Permute[1][2]*py + m_Permute[2][2]*pz;
}

const float  FLOAT_EPS     = 1.0e-7f;
const float  FLOAT_EPS_SQ  = 1.0e-14f;
const float  FLOAT_PI      = 3.14159265358979323846f;
const double DOUBLE_EPS    = 1.0e-14;
const double DOUBLE_EPS_SQ = 1.0e-28;
const double DOUBLE_PI     = 3.14159265358979323846;
//TwType CQuaternionExt::s_CustomType = TW_TYPE_UNDEF;
std::vector<float>   s_SphTri;
std::vector<color32> s_SphCol;
std::vector<int>     s_SphTriProj;
std::vector<color32> s_SphColLight;
void CDrawGround::CreateSphere()
{
    const int SUBDIV = 7;
    s_SphTri.clear();
    s_SphCol.clear();

    const float A[8*3] = { 1,0,0, 0,0,-1, -1,0,0, 0,0,1,   0,0,1,  1,0,0,  0,0,-1, -1,0,0 };
    const float B[8*3] = { 0,1,0, 0,1,0,  0,1,0,  0,1,0,   0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0 };
    const float C[8*3] = { 0,0,1, 1,0,0,  0,0,-1, -1,0,0,  1,0,0,  0,0,-1, -1,0,0, 0,0,1  };
    //const color32 COL_A[8] = { 0xffff8080, 0xff000080, 0xff800000, 0xff8080ff,  0xff8080ff, 0xffff8080, 0xff000080, 0xff800000 };
    //const color32 COL_B[8] = { 0xff80ff80, 0xff80ff80, 0xff80ff80, 0xff80ff80,  0xff008000, 0xff008000, 0xff008000, 0xff008000 };
    //const color32 COL_C[8] = { 0xff8080ff, 0xffff8080, 0xff000080, 0xff800000,  0xffff8080, 0xff000080, 0xff800000, 0xff8080ff };
    const color32 COL_A[8] = { 0xffffffff, 0xffffff40, 0xff40ff40, 0xff40ffff,  0xffff40ff, 0xffff4040, 0xff404040, 0xff4040ff };
    const color32 COL_B[8] = { 0xffffffff, 0xffffff40, 0xff40ff40, 0xff40ffff,  0xffff40ff, 0xffff4040, 0xff404040, 0xff4040ff };
    const color32 COL_C[8] = { 0xffffffff, 0xffffff40, 0xff40ff40, 0xff40ffff,  0xffff40ff, 0xffff4040, 0xff404040, 0xff4040ff };

    int i, j, k, l;
    float xa, ya, za, xb, yb, zb, xc, yc, zc, x, y, z, norm, u[3], v[3];
    color32 col;
    for( i=0; i<8; ++i )
    {
        xa = A[3*i+0]; ya = A[3*i+1]; za = A[3*i+2];
        xb = B[3*i+0]; yb = B[3*i+1]; zb = B[3*i+2];
        xc = C[3*i+0]; yc = C[3*i+1]; zc = C[3*i+2];
        for( j=0; j<=SUBDIV; ++j )
            for( k=0; k<=2*(SUBDIV-j); ++k )
            {
                if( k%2==0 )
                {
                    u[0] = ((float)j)/(SUBDIV+1);
                    v[0] = ((float)(k/2))/(SUBDIV+1);
                    u[1] = ((float)(j+1))/(SUBDIV+1);
                    v[1] = ((float)(k/2))/(SUBDIV+1);
                    u[2] = ((float)j)/(SUBDIV+1);
                    v[2] = ((float)(k/2+1))/(SUBDIV+1);
                }
                else
                {
                    u[0] = ((float)j)/(SUBDIV+1);
                    v[0] = ((float)(k/2+1))/(SUBDIV+1);
                    u[1] = ((float)(j+1))/(SUBDIV+1);
                    v[1] = ((float)(k/2))/(SUBDIV+1);
                    u[2] = ((float)(j+1))/(SUBDIV+1);
                    v[2] = ((float)(k/2+1))/(SUBDIV+1);
                }

                for( l=0; l<3; ++l )
                {
                    x = (1.0f-u[l]-v[l])*xa + u[l]*xb + v[l]*xc;
                    y = (1.0f-u[l]-v[l])*ya + u[l]*yb + v[l]*yc;
                    z = (1.0f-u[l]-v[l])*za + u[l]*zb + v[l]*zc;
                    norm = sqrtf(x*x+y*y+z*z);
                    x /= norm; y /= norm; z /= norm;
                    s_SphTri.push_back(x); s_SphTri.push_back(y); s_SphTri.push_back(z);
                    if( u[l]+v[l]>FLOAT_EPS )
                        col = ColorBlend(COL_A[i], ColorBlend(COL_B[i], COL_C[i], v[l]/(u[l]+v[l])), u[l]+v[l]);
                    else
                        col = COL_A[i];
                    //if( (j==0 && k==0) || (j==0 && k==2*SUBDIV) || (j==SUBDIV && k==0) )
                    //  col = 0xffff0000;
                    s_SphCol.push_back(col);
                }
            }
    }

    const float SPH_RADIUS = 0.75f;
    s_SphTriProj.clear();
    s_SphTriProj.resize(2*s_SphCol.size(), 0);
    s_SphColLight.clear();
    s_SphColLight.resize(s_SphCol.size(), 0);

    float qx = 0.0f, qy = 0.0f, qz = 0.0f, qs = 1.0f;
    int w = 158, h = 58;
    if( 1 )
    {
        const float *tri = &(s_SphTri[0]);
        int *triProj = &(s_SphTriProj[0]);
        const color32 *col = &(s_SphCol[0]);
        color32 *colLight = &(s_SphColLight[0]);
        const int ntri = (int)s_SphTri.size()/3;
        for(i=0; i<ntri; ++i)   // draw sphere
        {
            x = SPH_RADIUS*tri[3*i+0]; y = SPH_RADIUS*tri[3*i+1]; z = SPH_RADIUS*tri[3*i+2];
            ApplyQuat(&x, &y, &z, x, y, z, qx, qy, qz, qs);
            Permute(&x, &y, &z, x, y, z);
            triProj[2*i+0] = QuatPX(x, w, h);
            triProj[2*i+1] = QuatPY(y, w, h);
            colLight[i] = 0xFF0000FF;// ColorBlend(0xff000000, col[i], fabsf(TClamp(z / SPH_RADIUS, -1.0f, 1.0f)))& alpha;
        }
    }
}

void calculateBallVertex(std::vector<glm::vec3>& vecPoints, GLfloat r)
{
    //    x=x0+r sinθcosφ
    //    y=y0+r sinθsinφ
    //    z=z0+r cosθ

    vecPoints.clear();

    GLfloat radius = 0.8 * r;
    int kRow = 50, kCol = 50;
    GLfloat kHeight = 600.0f;
    GLfloat kWidth = 600.0f;
    // GLfloat delta = 2 * M_PI / 100;
    GLfloat *dest = (GLfloat *) malloc(sizeof(GLfloat) * 2 * kRow * kCol * 3);

    GLfloat k = kHeight * 1.0 / kWidth;

    float step_z = PI / kRow ;  //z方向每次步进的角度(上下，纬度)
    float step_xy = 2*PI/ kCol;  //x,y平面每次步进的角度（经度）
    float x[2],y[2],z[2];  //用来存坐标

    float angle_z = 0; //起始角度
    float angle_xy = 0;
    int i=0, j=0;

    // 90 -->  -90
    for(i = 0; i < kRow; i++)
    {
        angle_z = i * step_z;  //每次步进step_z

        for(j = 0; j < kCol ; j++)
        {
            // j++;
            angle_xy = j * step_xy;  //每次步进step_xy
            //整个的过程可以想象3D打印机，一层一层的画出来
            x[0] = k * radius * sin(angle_z) * cos(angle_xy);  //第一个小平面的第一个顶点坐标
            y[0] = radius * sin(angle_z) * sin(angle_xy);
            z[0] = radius * cos(angle_z);

            x[1] = k * radius * sin(angle_z + step_z) * cos(angle_xy);  //第一个小平面的第二个顶点坐标，下面类似
            y[1] = radius * sin(angle_z + step_z) * sin(angle_xy);
            z[1] = radius * cos(angle_z + step_z);

            //整个的过程可以想象3D打印机，一层一层的画出来
            //            x[0] = k * radius * cos(angle_z) * sin(angle_xy);  //第一个小平面的第一个顶点坐标
            //            y[0] = radius * sin(angle_z);
            //            z[0] = radius * cos(angle_z) * cos(angle_xy);
            //
            //            x[1] = k * radius * cos(angle_z + step_z) * sin(angle_xy);  //第一个小平面的第一个顶点坐标
            //            y[1] = radius * sin(angle_z + step_z);
            //            z[1] = radius * cos(angle_z + step_z) * cos(angle_xy);

            //至此得到一个平面的3个顶点
            for(int k=0; k<2; k++)
            {
                dest[i * kCol * 2 * 3 + j * 2 * 3 +  k*3+0] = x[k];
                dest[i * kCol * 2 * 3 + j * 2 * 3 +  k*3+1] = y[k];
                dest[i * kCol * 2 * 3 + j * 2 * 3 +  k*3+2] = z[k];
                vecPoints.push_back(glm::vec3(x[k], y[k], z[k]));
            }
        } //循环画出这一层的平面，组成一个环
    }  //z轴++，画出剩余层
    ;
}

float* pGround = nullptr;
std::vector<glm::vec2>* g_vecPoints = nullptr;


float CDrawGround::GetHeight_bak(int x, int z) {
    x = x < 0 ? 0 : x;
    x = x >= this->m_nPixW ? this->m_nPixW : x;
    z = z < 0 ? 0 : z;
    z = z >= this->m_nPixH ? this->m_nPixH : z;
    return float(this->m_pHeightMapDatas[(x + z * this->m_nHeightMapWidth) * 3]) / 255.0f;
}

float CDrawGround::GetHeight(int x, int z) {
    //return 0;
    x = x < 0 ? 0 : x;
    x = x >= this->m_nPixW ? this->m_nPixW : x;
    z = z < 0 ? 0 : z;
    z = z >= this->m_nPixH ? this->m_nPixH : z;
    float fHeight = float(this->m_pHeightMapDatas[(x + z * this->m_nHeightMapWidth)]);
    return fHeight;
    //return float(heightMapDatas[(x + z * heightMapWidth) * 3]) / 255.0f;
}
void CDrawGround::GetNormal(int x, int z, float *normal) {
    float heightL = GetHeight(x - 1, z);
    float heightR = GetHeight(x + 1, z);
    float heightU = GetHeight(x, z + 1);
    float heightD = GetHeight(x, z - 1);
    normal[0] = heightL - heightR;
    normal[1] = 2.0f;
    normal[2] = heightD - heightU;
    float len = sqrtf(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] /= len;
    normal[1] /= len;
    normal[2] /= len;
}

void CDrawGround::InitScene_bak(){
    //this->CreateSphere();
    //calculateBallVertex(g_vecPoints, 50.0f);


    Projection = glm::perspective(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
    View = glm::lookAt(glm::vec3(128.0f, 30.0f, 128.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f));

    const char szGroundVS[] = {
        "attribute vec4 position;\n"
        "attribute vec4 normal;\n"
        "attribute vec4 texcoord;\n"
        "uniform mat4 Model;\n"
        "uniform mat4 View;\n"
        "uniform mat4 Projection;\n"
        "varying vec4 V_Texcoord;\n"
        "void main(){\n"
        "    V_Texcoord=texcoord;\n"
        "    gl_Position=Projection*View*Model*position;}"
    };
    const char szGroundFS[] = {
        "uniform sampler2D Grass;\n"
        "varying vec4 V_Texcoord;\n"
        "void main(){\n"
        "    gl_FragColor=texture2D(Grass,V_Texcoord.xy);}"
    };

    this->m_ProgramGround = this->m_pGLCore->CreateProgram(szGroundVS, szGroundFS);
    this->m_PositionID = this->m_pGLCore->glGetAttribLocation(this->m_ProgramGround, "position");
    this->m_NormalID = this->m_pGLCore->glGetAttribLocation(this->m_ProgramGround, "normal");
    this->m_TexcoordID = this->m_pGLCore->glGetAttribLocation(this->m_ProgramGround, "texcoord");
    this->m_ModelID = this->m_pGLCore->glGetUniformLocation(this->m_ProgramGround, "Model");
    this->m_ViewID = this->m_pGLCore->glGetUniformLocation(this->m_ProgramGround, "View");
    this->m_ProjectionID = this->m_pGLCore->glGetUniformLocation(this->m_ProgramGround, "Projection");
    this->m_GrassTextureID = this->m_pGLCore->glGetUniformLocation(this->m_ProgramGround, "Grass");
    int filesize = 0;
    unsigned char * filecontent = this->m_pGLCore->LoadFileContent("D:/QT_CODE_PRJ/SuperXView01/Res/images/height.bmp", filesize);
    ///heightMapDatas = this->m_pGLCore->DecodeBMP(filecontent, heightMapWidth, heightMapHeight);
    this->m_MapTexture = this->m_pGLCore->LoadTextureFromFile("D:/QT_CODE_PRJ/SuperXView01/Res/images/grass.bmp");
    this->m_nPixW = this->m_nHeightMapWidth - 1;
    this->m_nPixH = this->m_nHeightMapHeight - 1;
    this->m_pTerrainData = new VertexData[256 * 256 * 4];
    float normal[3];
    for (int z = 0; z < 256; ++z) {
        for (int x = 0; x < 256; ++x) {
            float start_z = 128.0f - z;
            float start_x = -128.0f + x;
            float height = 10.0f;
            int quad_index = x + z * 256;
            int vertex_start_index = quad_index * 4;
            this->m_pTerrainData[vertex_start_index].Position[0] = start_x;
            this->m_pTerrainData[vertex_start_index].Position[1] = height * GetHeight(x, z);
            this->m_pTerrainData[vertex_start_index].Position[2] = start_z;
            this->m_pTerrainData[vertex_start_index].Position[3] = 1.0f;
            GetNormal(x, z, normal);
            memcpy(this->m_pTerrainData[vertex_start_index].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index].Texcoord[0] = float(x % 32) / 32.0f;
            this->m_pTerrainData[vertex_start_index].Texcoord[1] = float(z % 32) / 32.0f;

            this->m_pTerrainData[vertex_start_index + 1].Position[0] = start_x + 1.0f;
            this->m_pTerrainData[vertex_start_index + 1].Position[1] = height * GetHeight(x + 1, z);
            this->m_pTerrainData[vertex_start_index + 1].Position[2] = start_z;
            this->m_pTerrainData[vertex_start_index + 1].Position[3] = 1.0f;
            GetNormal(x + 1, z, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 1].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index+1].Texcoord[0] = float((x + 1) % 32) / 32.0f;
            this->m_pTerrainData[vertex_start_index+1].Texcoord[1] = float(z % 32) / 32.0f;

            this->m_pTerrainData[vertex_start_index + 2].Position[0] = start_x;
            this->m_pTerrainData[vertex_start_index + 2].Position[1] = height * GetHeight(x, z + 1);
            this->m_pTerrainData[vertex_start_index + 2].Position[2] = start_z - 1.0f;
            this->m_pTerrainData[vertex_start_index + 2].Position[3] = 1.0f;
            GetNormal(x, z + 1, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 2].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index + 2].Texcoord[0] = float((x) % 32) / 32.0f;
            this->m_pTerrainData[vertex_start_index + 2].Texcoord[1] = float((z + 1) % 32) / 32.0f;

            this->m_pTerrainData[vertex_start_index + 3].Position[0] = start_x + 1.0f;
            this->m_pTerrainData[vertex_start_index + 3].Position[1] = height * GetHeight(x + 1, z + 1);
            this->m_pTerrainData[vertex_start_index + 3].Position[2] = start_z - 1.0f;
            this->m_pTerrainData[vertex_start_index + 3].Position[3] = 1.0f;
            GetNormal(x + 1, z + 1, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 3].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index + 3].Texcoord[0] = float((x + 1) % 32) / 32.0f;
            this->m_pTerrainData[vertex_start_index + 3].Texcoord[1] = float((z + 1) % 32) / 32.0f;
        }
    }
    this->m_pGLCore->glGenBuffers(1, &this->m_GroundVBO);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, this->m_GroundVBO);
    this->m_pGLCore->glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * 256 * 256 * 4, this->m_pTerrainData, GL_STATIC_DRAW);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] this->m_pTerrainData;
    CHECK_GL_ERROR_1(this->m_pGLCore);
}

void CDrawGround::Draw_bak(){

    ////enum Cull           { CULL_NONE, CULL_CW, CULL_CCW };
    //COpenGLCore::Cull cull = COpenGLCore::Cull::CULL_CCW;
    //int *triProj = &(s_SphTriProj[0]);
    //color32 *colLight = &(s_SphColLight[0]);
    ////this->m_pGLCore->DrawTriangles((int)s_SphTri.size()/9, triProj, colLight, cull);
    //this->m_pGLCore->DrawTriangles(g_vecPoints, 0xFFFF00FF);


    this->m_pGLCore->glEnable(GL_CULL_FACE);
    this->m_pGLCore->glPolygonMode(GL_FRONT, GL_LINE);
    CHECK_GL_ERROR_1(this->m_pGLCore);

    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glUseProgram(this->m_ProgramGround);
    this->m_pGLCore->glActiveTexture(GL_TEXTURE0);
    this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, this->m_MapTexture);
    this->m_pGLCore->glUniform1i(this->m_GrassTextureID,0);
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glUniformMatrix4fv(this->m_ModelID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatModel));
    this->m_pGLCore->glUniformMatrix4fv(this->m_ViewID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatView));
    this->m_pGLCore->glUniformMatrix4fv(this->m_ProjectionID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatPerspectProject));
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glEnable(GL_DEPTH_TEST);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, this->m_GroundVBO);
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glEnableVertexAttribArray(this->m_PositionID);
    this->m_pGLCore->glVertexAttribPointer(this->m_PositionID, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    //this->m_pGLCore->glEnableVertexAttribArray(normalLocation);
    //this->m_pGLCore->glVertexAttribPointer(normalLocation, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 4));
    this->m_pGLCore->glEnableVertexAttribArray(this->m_TexcoordID);
    this->m_pGLCore->glVertexAttribPointer(this->m_TexcoordID, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 8));

    CHECK_GL_ERROR_1(this->m_pGLCore);
    for (int z = 0; z < 256; ++z) {
        for (int x = 0; x < 256; ++x) {
            int quad_index = x + z * 256;
            int vertex_start_index = quad_index * 4;
            this->m_pGLCore->glDrawArrays(GL_TRIANGLE_STRIP, vertex_start_index, 4);
        }
    }
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glUseProgram(0);
    CHECK_GL_ERROR_1(this->m_pGLCore);

    this->m_pGLCore->glPolygonMode(GL_FRONT, GL_FILL);
    CHECK_GL_ERROR_1(this->m_pGLCore);
}


#include <QtGui/QImage>

void CDrawGround::InitShader(){

    const char szGroundVS[] = {
        "attribute vec4 position;\n"
        "attribute vec4 normal;\n"
        "attribute vec4 texcoord;\n"
        "uniform mat4 Model;\n"
        "uniform mat4 View;\n"
        "uniform mat4 Projection;\n"
        "varying vec4 V_Texcoord;\n"
        "void main(){\n"
        "    V_Texcoord=texcoord;\n"
        "    gl_Position=Projection*View*Model*position;}"
    };
    const char szGroundFS[] = {
        "uniform sampler2D Grass;\n"
        "varying vec4 V_Texcoord;\n"
        "void main(){\n"
        //"    gl_FragColor=vec4(vec3(1.0), 1.0);}"
        "    gl_FragColor=texture2D(Grass,vec2(V_Texcoord.x, 1.0 - V_Texcoord.y));}"
    };

    this->m_ProgramGround = this->m_pGLCore->CreateProgram(szGroundVS, szGroundFS);
    this->m_PositionID = this->m_pGLCore->glGetAttribLocation(m_ProgramGround, "position");
    this->m_NormalID = this->m_pGLCore->glGetAttribLocation(m_ProgramGround, "normal");
    this->m_TexcoordID = this->m_pGLCore->glGetAttribLocation(m_ProgramGround, "texcoord");

    this->m_ModelID = this->m_pGLCore->glGetUniformLocation(m_ProgramGround, "Model");
    this->m_ViewID = this->m_pGLCore->glGetUniformLocation(m_ProgramGround, "View");
    this->m_ProjectionID = this->m_pGLCore->glGetUniformLocation(m_ProgramGround, "Projection");
    this->m_GrassTextureID = this->m_pGLCore->glGetUniformLocation(m_ProgramGround, "Grass");

}

void CDrawGround::SetTifFilePath(const char* szTifFileFullPath, const char* szDemFileFullPath){
    this->m_sTifFileFullPath = szTifFileFullPath;
    this->m_sDemFileFullPath = szDemFileFullPath;
}
void CDrawGround::InitGroundMap(){

    //texture = this->m_pGLCore->LoadTextureFromFile("D:/QT_CODE_PRJ/SuperXView01/Res/images/grass.bmp");
    CReadRaster* pReadTifTexture = new CReadRaster(this->m_sTifFileFullPath.c_str());
    //C:/temp/Mss_Data/Rectify/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_8Bit.tiff
    //C:\temp\ColorMap_Mss  D:/Arcgis_SaveData/AAA/hubeiMss01.tiff   YunNanMss.tiff YunNanDEM8Bit.tiff
    //C:/temp/Mss_Data/Rectify/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_8Bit.tif
    //C:/temp/ColorMap_Mss/worldmap.tif

    //int filesize = 0;
    //unsigned char * filecontent = this->m_pGLCore->LoadFileContent("D:/QT_CODE_PRJ/SuperXView01/Res/images/height.bmp", filesize);
    //heightMapDatas = this->m_pGLCore->DecodeBMP(filecontent, heightMapWidth, heightMapHeight);

    CReadRaster* pReadTifDem = new CReadRaster(this->m_sDemFileFullPath.c_str()); //YunNanDem.tiff

    double fIntersectMinLng = pReadTifTexture->m_fMinLng > pReadTifDem->m_fMinLng ? pReadTifTexture->m_fMinLng : pReadTifDem->m_fMinLng;
    double fIntersectMinLat = pReadTifTexture->m_fMinLat > pReadTifDem->m_fMinLat ? pReadTifTexture->m_fMinLat : pReadTifDem->m_fMinLat;
    double fIntersectMaxLng = pReadTifTexture->m_fMaxLng < pReadTifDem->m_fMaxLng ? pReadTifTexture->m_fMaxLng : pReadTifDem->m_fMaxLng;
    double fIntersectMaxLat = pReadTifTexture->m_fMaxLat < pReadTifDem->m_fMaxLat ? pReadTifTexture->m_fMaxLat : pReadTifDem->m_fMaxLat;


    double nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight;
    pReadTifDem->GeoPos2PixPos(fIntersectMinLng, fIntersectMinLat, fIntersectMaxLng, fIntersectMaxLat,
                                   &nSrcLT_X, &nSrcLT_Y, &nSrcWidth, &nSrcHeight);
    nSrcWidth = nSrcWidth - nSrcLT_X;
    nSrcHeight = nSrcHeight - nSrcLT_Y;

    this->m_nHeightMapWidth = nSrcWidth;
    this->m_nHeightMapHeight = nSrcHeight;
    //if(this->m_nHeightMapWidth > 3000) {
    //    this->m_nHeightMapWidth = 3000;
    //    this->m_nHeightMapHeight = ((float)nSrcHeight / (float)nSrcWidth * 3000.0f);
    //}
    //if(this->m_nHeightMapHeight > 3000) {
    //    this->m_nHeightMapHeight = 3000;
    //    this->m_nHeightMapWidth = ((float)nSrcWidth / (float)nSrcHeight * 3000.0f);
    //}

    this->m_pHeightMapDatas = (float * )pReadTifDem->ReadPixData(1, (int)nSrcLT_X, (int)nSrcLT_Y, (int)nSrcWidth, (int)nSrcHeight, this->m_nHeightMapWidth, this->m_nHeightMapHeight);

    //heightMapWidth = pReadTifDem->m_nPixWidth;
    //heightMapHeight = pReadTifDem->m_nPixHeight;
    //heightMapDatas = (float * )pReadTifDem->ReadPixData(1, 0, 0, heightMapWidth, heightMapHeight, heightMapWidth, heightMapHeight);

    //pReadTifTexture = pReadTif;
    pReadTifTexture->GeoPos2PixPos(fIntersectMinLng, fIntersectMinLat, fIntersectMaxLng, fIntersectMaxLat,
                                   &nSrcLT_X, &nSrcLT_Y, &nSrcWidth, &nSrcHeight);
    nSrcWidth = nSrcWidth - nSrcLT_X;
    nSrcHeight = nSrcHeight - nSrcLT_Y;
    //int nReadW = pReadTifTexture->m_nPixWidth;
    //int nReadH = pReadTifTexture->m_nPixHeight;

    int nReadW = (int)nSrcWidth;//this->m_nHeightMapWidth;// * 5;
    int nReadH = (int)nSrcHeight;//this->m_nHeightMapHeight;// * 5;
    //if ((nReadW < 1000 && pReadTifTexture->m_nPixWidth >= 3000) || (nReadH < 1000 && pReadTifTexture->m_nPixHeight >= 3000)){
    //    nReadW = nReadW * 3;
    //    nReadH = nReadH * 3;
    //}

    //nReadW = pReadTifTexture->m_nPixWidth;
    //nReadH = (int)((float)this->m_nHeightMapHeight / (float)this->m_nHeightMapWidth * (float)nReadW);
    ////if(nReadW > pReadTifTexture->m_nPixWidth) {
    ////}
    //if(nReadH > pReadTifTexture->m_nPixHeight) {
    //    nReadH = pReadTifTexture->m_nPixHeight;
    //    nReadW = (int)((float)this->m_nHeightMapWidth / (float)this->m_nHeightMapHeight * (float)nReadH);
    //}

    unsigned char* pPixBuf00 = (unsigned char* )pReadTifTexture->ReadPixDataFull(
                (int)nSrcLT_X, (int)nSrcLT_Y, (int)nSrcWidth, (int)nSrcHeight, 4, nReadW, nReadH);
    //unsigned char* pPixBuf00 = (unsigned char* )pReadTifTexture->ReadPixDataFull(nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, 4, nReadW, nReadH);
    //unsigned char* pBand1Buf = (unsigned char* )pReadTifTexture->ReadPixData(1, nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, nReadW, nReadH);
    //unsigned char* pBand2Buf = (unsigned char* )pReadTifTexture->ReadPixData(2, nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, nReadW, nReadH);
    //unsigned char* pBand3Buf = (unsigned char* )pReadTifTexture->ReadPixData(3, nSrcLT_X, nSrcLT_Y, nSrcWidth, nSrcHeight, nReadW, nReadH);
    //
    ////unsigned char* pBand1Buf = (unsigned char* )pReadTifTexture->ReadPixData(1, 0, 0, nReadW, nReadH, nReadW, nReadH);
    ////unsigned char* pBand2Buf = (unsigned char* )pReadTifTexture->ReadPixData(2, 0, 0, nReadW, nReadH, nReadW, nReadH);
    ////unsigned char* pBand3Buf = (unsigned char* )pReadTifTexture->ReadPixData(3, 0, 0, nReadW, nReadH, nReadW, nReadH);
    //
    ////unsigned char* pBand1Buf = (unsigned char* )pReadTifTexture->ReadPixData(
    ////            1, pReadTif->m_fMinLng, pReadTif->m_fMinLat, pReadTif->m_fMaxLng, pReadTif->m_fMaxLat, heightMapWidth, heightMapHeight);
    ////unsigned char* pBand2Buf = (unsigned char* )pReadTifTexture->ReadPixData(
    ////            2, pReadTif->m_fMinLng, pReadTif->m_fMinLat, pReadTif->m_fMaxLng, pReadTif->m_fMaxLat, heightMapWidth, heightMapHeight);
    ////unsigned char* pBand3Buf = (unsigned char* )pReadTifTexture->ReadPixData(
    ////            3, pReadTif->m_fMinLng, pReadTif->m_fMinLat, pReadTif->m_fMaxLng, pReadTif->m_fMaxLat, heightMapWidth, heightMapHeight);
    //
    //unsigned char* pPixBuf = new unsigned char[(unsigned long long)nReadW * (unsigned long long)nReadH * 4];
    //unsigned long long ii = 0, nBufSize11 = (unsigned long long)nReadW * (unsigned long long)nReadH;
    //for(ii = 0; ii<nBufSize11; ii++) {
    //    //pPixBuf[ii * 4 + 0] =pBand1Buf[ii];
    //    pPixBuf[ii * 4 + 1] =pBand1Buf[ii];
    //    pPixBuf[ii * 4 + 2] =pBand1Buf[ii];
    //    pPixBuf[ii * 4 + 3] =255;
    //}
    //QImage imgBmp(pPixBuf00, nReadW, nReadH, QImage::Format_RGBA8888);
    //g_sTextImgFile = "D:/QT_CODE_PRJ/SuperXView01/Res/images/TiffoutImg.bmp";
    //remove(g_sTextImgFile.c_str());
    //imgBmp.save(g_sTextImgFile.c_str(), "bmp");
    //texture = this->m_pGLCore->LoadTextureFromFile(g_sTextImgFile.c_str());
    //delete[] pPixBuf;
    //delete[] pBand1Buf;
    //delete[] pBand2Buf;
    //delete[] pBand3Buf;
    this->m_sTextImgFile = "GRASS.DEFPIXBUF";
    this->m_MapTexture = this->m_pGLCore->CreateTexture2DFromImgBuffer("GRASS.DEFPIXBUF", pPixBuf00, nReadW, nReadH, 4);
    if(pPixBuf00) {
        delete[] pPixBuf00;
        pPixBuf00 = nullptr;
    }

}

void CDrawGround::InitGround(){

    float fHalfWidth = (float)(this->m_nHeightMapWidth) / 2.0f;
    float fHalfHeight = (float)(this->m_nHeightMapHeight) / 2.0f;
    float f2HalfWidth = (float)(this->m_nHeightMapWidth);
    float f2HalfHeight = (float)(this->m_nHeightMapHeight);
    this->m_nPixW = this->m_nHeightMapWidth - 1;
    this->m_nPixH = this->m_nHeightMapHeight - 1;
    this->m_pTerrainData = new VertexData[(unsigned long long)this->m_nHeightMapWidth * (unsigned long long)this->m_nHeightMapHeight * 6];
    float normal[3];
    const float height = 1.0f;
    for (int z = 0; z < this->m_nHeightMapHeight; ++z) {
        float start_z = fHalfHeight - z;
        //printf("height : %d %f\n", z, start_z);
        for (int x = 0; x < this->m_nHeightMapWidth; ++x) {
            float start_x = x - fHalfWidth;
            if(z<=3){
                //printf("width : %d %f\n", x, start_x);
            }
            int quad_index = x + z * this->m_nHeightMapWidth;
            int vertex_start_index = quad_index * 6;
            this->m_pTerrainData[vertex_start_index].Position[0] = start_x;
            this->m_pTerrainData[vertex_start_index].Position[1] = height * GetHeight(x, this->m_nHeightMapHeight - z);
            this->m_pTerrainData[vertex_start_index].Position[2] = start_z;
            this->m_pTerrainData[vertex_start_index].Position[3] = 1.0f;
            GetNormal(x, z, normal);
            memcpy(this->m_pTerrainData[vertex_start_index].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index].Texcoord[0] = float(x) / f2HalfWidth;
            this->m_pTerrainData[vertex_start_index].Texcoord[1] = float(z) / f2HalfHeight;

            this->m_pTerrainData[vertex_start_index + 1].Position[0] = start_x + 1.0f;
            this->m_pTerrainData[vertex_start_index + 1].Position[1] = height * GetHeight(x + 1, this->m_nHeightMapHeight - z);
            this->m_pTerrainData[vertex_start_index + 1].Position[2] = start_z;
            this->m_pTerrainData[vertex_start_index + 1].Position[3] = 1.0f;
            GetNormal(x + 1, z, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 1].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index + 1].Texcoord[0] = float((x + 1)) / f2HalfWidth;
            this->m_pTerrainData[vertex_start_index + 1].Texcoord[1] = float(z) / f2HalfHeight;

            this->m_pTerrainData[vertex_start_index + 2].Position[0] = start_x;
            this->m_pTerrainData[vertex_start_index + 2].Position[1] = height * GetHeight(x, this->m_nHeightMapHeight - (z + 1));
            this->m_pTerrainData[vertex_start_index + 2].Position[2] = start_z - 1.0f;
            this->m_pTerrainData[vertex_start_index + 2].Position[3] = 1.0f;
            GetNormal(x, z + 1, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 2].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index + 2].Texcoord[0] = float((x)) / f2HalfWidth;
            this->m_pTerrainData[vertex_start_index + 2].Texcoord[1] = float((z + 1)) / f2HalfHeight;

            /////////////////

            this->m_pTerrainData[vertex_start_index + 3].Position[0] = start_x;
            this->m_pTerrainData[vertex_start_index + 3].Position[1] = height * GetHeight(x, this->m_nHeightMapHeight - (z + 1));
            this->m_pTerrainData[vertex_start_index + 3].Position[2] = start_z - 1.0f;
            this->m_pTerrainData[vertex_start_index + 3].Position[3] = 1.0f;
            GetNormal(x, z + 1, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 2].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index + 3].Texcoord[0] = float((x)) / f2HalfWidth;
            this->m_pTerrainData[vertex_start_index + 3].Texcoord[1] = float((z + 1)) / f2HalfHeight;

            this->m_pTerrainData[vertex_start_index + 4].Position[0] = start_x + 1.0f;
            this->m_pTerrainData[vertex_start_index + 4].Position[1] = height * GetHeight(x + 1, this->m_nHeightMapHeight - z);
            this->m_pTerrainData[vertex_start_index + 4].Position[2] = start_z;
            this->m_pTerrainData[vertex_start_index + 4].Position[3] = 1.0f;
            GetNormal(x + 1, z, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 1].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index + 4].Texcoord[0] = float((x + 1)) / f2HalfWidth;
            this->m_pTerrainData[vertex_start_index + 4].Texcoord[1] = float(z) / f2HalfHeight;

            this->m_pTerrainData[vertex_start_index + 5].Position[0] = start_x + 1.0f;
            this->m_pTerrainData[vertex_start_index + 5].Position[1] = height * GetHeight(x + 1, this->m_nHeightMapHeight - (z + 1));
            this->m_pTerrainData[vertex_start_index + 5].Position[2] = start_z - 1.0f;
            this->m_pTerrainData[vertex_start_index + 5].Position[3] = 1.0f;
            GetNormal(x + 1, z + 1, normal);
            memcpy(this->m_pTerrainData[vertex_start_index + 3].Normal, normal, sizeof(float) * 3);
            this->m_pTerrainData[vertex_start_index + 5].Texcoord[0] = float((x + 1)) / f2HalfWidth;
            this->m_pTerrainData[vertex_start_index + 5].Texcoord[1] = float((z + 1)) / f2HalfHeight;

            //printf("\n\n%f, %f\n", terrainData[vertex_start_index+0].Texcoord[0], terrainData[vertex_start_index+0].Texcoord[1]);
            //printf("%f, %f\n", terrainData[vertex_start_index+1].Texcoord[0], terrainData[vertex_start_index+1].Texcoord[1]);
            //printf("%f, %f\n", terrainData[vertex_start_index+2].Texcoord[0], terrainData[vertex_start_index+2].Texcoord[1]);
            //printf("%f, %f\n", terrainData[vertex_start_index+3].Texcoord[0], terrainData[vertex_start_index+3].Texcoord[1]);
            //printf("%f, %f\n", terrainData[vertex_start_index+4].Texcoord[0], terrainData[vertex_start_index+4].Texcoord[1]);
            //printf("%f, %f\n", terrainData[vertex_start_index+5].Texcoord[0], terrainData[vertex_start_index+5].Texcoord[1]);
        }
    }
}

void CDrawGround::BindGround(){

    this->m_pGLCore->glGenBuffers(1, &this->m_GroundVBO);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, this->m_GroundVBO);
    this->m_pGLCore->glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * (unsigned long long)this->m_nHeightMapWidth * (unsigned long long)this->m_nHeightMapHeight * 6, this->m_pTerrainData, GL_DYNAMIC_DRAW);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete[] this->m_pTerrainData;
    delete[] this->m_pHeightMapDatas;
    this->m_pTerrainData = nullptr;
    this->m_pHeightMapDatas = nullptr;
    CHECK_GL_ERROR_1(this->m_pGLCore);
}

void CDrawGround::InitScene(const char* szTifFileFullPath/* = nullptr*/, const char* szDemFileFullPath/* = nullptr*/) {
    //this->CreateSphere();
    //calculateBallVertex(g_vecPoints, 50.0f);

    /// //C:/temp/Mss_Data/Rectify/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_fuse_8bit.tiff
    /// //"C:/temp/GF1_SaveDir/ColorMap/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_fuse_8Bit.tiff"
    /// //D:/hqq20200828/BJDZ/GF1/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_fuse_8Bit03.tiff
    /// std::string sTifFullPath = "C:/temp/GF1_SaveDir/Rectify/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_fuse_8bit.tiff";
    /// //D:/hqq20200828/BJDZ/DemHuBei/hubei_dem_median.tif
    /// this->SetTifFilePath(sTifFullPath.c_str(), "D:/SxDataProduce/HuBeiDem/hubei_dem_median.tif");
    /// //this->SetTifFilePath("D:/hqq20200828/BJDZ/GF1/GF1_PMS2_E115.0_N30.7_20200314_L1A0004672004-MSS2_rectify_fuse_8Bit03.tiff",
    /// //                     "D:/hqq20200828/BJDZ/DemHuBei/hubei_dem_median.tif");
    this->m_nDrawReady = 0;
    if(szTifFileFullPath != nullptr && szDemFileFullPath != nullptr) {
        this->SetTifFilePath(szTifFileFullPath, szDemFileFullPath);
    }
    if(CMisc::CheckFileExists(this->m_sTifFileFullPath.c_str()) == 0 || CMisc::CheckFileExists(this->m_sDemFileFullPath.c_str()) == 0){
        return;
    }

    this->InitShader();

    this->InitGroundMap();

    this->InitGround();
    this->BindGround();

    this->m_nDrawReady = 1;
    return;
    g_vecPoints = new  std::vector<glm::vec2>[10];
    for(int j = 0; j<10; j++) {
        g_vecPoints[j].clear();
        float fOffMov = 0.0f;
        float fOff = 0.0f;
        for(int i = 0; i < 100; i++) {
            fOffMov = ((float)(rand() % 100));
            //vecPoints.push_back(glm::vec3(562.0f - fOff + fOffMov, 440.0f, 0.0f));
            //vecPoints.push_back(glm::vec3(562.0f - fOff + fOffMov, 440.0f, 0.0f));
            //vecPoints.push_back(glm::vec3(561.0f + fOff + fOffMov, 330.0f, 0.0f));
            //vecPoints.push_back(glm::vec3(561.0f + fOff + fOffMov, 330.0f, 0.0f));
            //this->m_pGLCore->DrawLines3D(4, (float*)vecPoints.data(), 0xFF00FFFF);

            g_vecPoints[j].push_back(glm::vec2(((float)(rand() % 100)) + fOffMov, ((float)(rand() % 200)) + fOffMov));
            //this->m_pGLCore->DrawEllipse(((float)(rand() % 20)) + 200.0f, ((float)(rand() % 10)) + 200.0f, ((float)(rand() % 20)) + 50.0f, ((float)(rand() % 10)) + 60.0f, 0xFF0000FF);
        }
    }
}

int CDrawGround::DrawPoint(void* pParam) {
    COpenGLCore* pGLCore = (COpenGLCore*)pParam;
    int j = rand()%10;
    pGLCore->DrawPoints(g_vecPoints[j].size(), (float*)g_vecPoints[j].data(), 0xFFFF00FF);

    return 1;
}
void CDrawGround::DrawBound() {

    float fOff = 1.0f;
    std::vector<glm::vec3> vecPoints;
    vecPoints.push_back(glm::vec3(-562.0f - fOff, 0.0f, -516.0f - fOff));
    vecPoints.push_back(glm::vec3(561.0f + fOff, 0.0f, -516.0f - fOff));
    vecPoints.push_back(glm::vec3(561.0f + fOff, 0.0f, 517.0f + fOff));
    vecPoints.push_back(glm::vec3(-562.0f - fOff, 0.0f, 517.0f + fOff));
    //this->m_pGLCore->DrawLines3D(4, (float*)(vecPoints.data()), 0xFF00FFFF);


    float fOffMov = 0.0f;
    this->m_pGLCore->DrawImage(10, 10, 200, 200, this->m_sTextImgFile.c_str());
    for(int ii = 0; ii < 10; ii ++) {
        fOffMov = fOffMov + 0.5f+((float)(rand() % 10))/10.0f;
        this->m_pGLCore->DrawImage(-100 + fOffMov, -100 + fOffMov, 200, 200, this->m_sTextImgFile.c_str());
    }
    //vecPoints.push_back(glm::vec3(-12.0f, 0.0f, -16.0f));
    //vecPoints.push_back(glm::vec3(-12.0f, 0.0f, 17.0f));
    //this->m_pGLCore->DrawLine3D(vecPoints, 0xFF00FFFF);
    vecPoints.clear();
    vecPoints.push_back(glm::vec3(-562.0f - fOff, 0.0f, -516.0f - fOff));
    vecPoints.push_back(glm::vec3(-562.0f - fOff, 0.0f, 517.0f + fOff));
    vecPoints.push_back(glm::vec3(561.0f + fOff, 0.0f, 517.0f + fOff));
    vecPoints.push_back(glm::vec3(561.0f + fOff, 0.0f, -516.0f - fOff));
    //this->m_pGLCore->DrawImage3D((float*)vecPoints.data(), this->m_sTextImgFile.c_str(), 1);

    fOffMov = 0.0f;
    for(int i = 0; i < 10; i++) {
        fOffMov = fOffMov + 1.0f+((float)(rand() % 10))/10.0f;
        vecPoints.clear();
        vecPoints.push_back(glm::vec3(-562.0f - fOff + fOffMov, 0.0f, -516.0f - fOff + fOffMov));
        vecPoints.push_back(glm::vec3(-562.0f - fOff + fOffMov, 0.0f, 517.0f + fOff + fOffMov));
        vecPoints.push_back(glm::vec3(561.0f + fOff + fOffMov, 0.0f, 517.0f + fOff + fOffMov));
        vecPoints.push_back(glm::vec3(561.0f + fOff + fOffMov, 0.0f, -516.0f - fOff + fOffMov));
        this->m_pGLCore->DrawImage3D((float*)vecPoints.data(), this->m_sTextImgFile.c_str(), 1);
    }


    CDrawGround::DrawPoint(this->m_pGLCore);
    //std::thread* pThread = new std::thread(CDrawGround::DrawPoint, this->m_pGLCore);
    //pThread->join();

    //float fCircleLen = 2.0f * PI * (std::max(56, 78));
    //int nCirclePtCnt = static_cast<int>(fCircleLen / 0.1f + 1.0f);
    //float delta = 2.0f * PI / (static_cast<float>(nCirclePtCnt));
    //const GLfloat z = 0.0f;
    ////float fScale = fRadiusX / fRadiusX * fRadiusY;
    //std::vector<glm::vec3> vecCirclePts;
    //for (int i = 0; i < nCirclePtCnt; i++) {
    //    GLfloat x = 56 * cos(delta * i) + 500;
    //    GLfloat y = 78 * sin(delta * i) + 600;
    //    vecCirclePts.push_back(glm::vec3(x, y, z));
    //    //vecCircleClrs.push_back(Vec4(fColor[0], fColor[1], fColor[2], fColor[3]));
    //}
    //
    //nCirclePtCnt = static_cast<int>(vecCirclePts.size());
    //this->m_pGLCore->DrawPoints(nCirclePtCnt, (float*)vecCirclePts.data(), 0xFFFF00FF);
}

void CDrawGround::DrawTiff() {
    if(this->m_nDrawReady == 0) {
        return;
    }
    ////enum Cull           { CULL_NONE, CULL_CW, CULL_CCW };
    //COpenGLCore::Cull cull = COpenGLCore::Cull::CULL_CCW;
    //int *triProj = &(s_SphTriProj[0]);
    //color32 *colLight = &(s_SphColLight[0]);
    ////this->m_pGLCore->DrawTriangles((int)s_SphTri.size()/9, triProj, colLight, cull);
    //this->m_pGLCore->DrawTriangles(g_vecPoints, 0xFFFF00FF);

    this->m_pGLCore->glEnable(GL_DEPTH_TEST);
    this->m_pGLCore->glEnable(GL_CULL_FACE);
    //this->m_pGLCore->glPolygonMode(GL_FRONT, GL_LINE);
    CHECK_GL_ERROR_1(this->m_pGLCore);

    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glUseProgram(this->m_ProgramGround);
    this->m_pGLCore->glActiveTexture(GL_TEXTURE0);
    this->m_pGLCore->glBindTexture(GL_TEXTURE_2D, this->m_MapTexture);
    this->m_pGLCore->glUniform1i(this->m_GrassTextureID,0);
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glUniformMatrix4fv(this->m_ModelID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatModel));
    this->m_pGLCore->glUniformMatrix4fv(this->m_ViewID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatView));
    this->m_pGLCore->glUniformMatrix4fv(this->m_ProjectionID, 1, GL_FALSE, glm::value_ptr(this->m_pGLCore->m_MatPerspectProject));
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glEnable(GL_DEPTH_TEST);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, this->m_GroundVBO);
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glEnableVertexAttribArray(this->m_PositionID);
    this->m_pGLCore->glVertexAttribPointer(this->m_PositionID, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);
    //this->m_pGLCore->glEnableVertexAttribArray(this->m_NormalID);
    //this->m_pGLCore->glVertexAttribPointer(this->m_NormalID, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 4));
    this->m_pGLCore->glEnableVertexAttribArray(this->m_TexcoordID);
    this->m_pGLCore->glVertexAttribPointer(this->m_TexcoordID, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 8));

    CHECK_GL_ERROR_1(this->m_pGLCore);
    //for (int z = 0; z < heightMapHeight; ++z) {
    //    for (int x = 0; x < heightMapWidth; ++x) {
    //        int quad_index = x + z * heightMapWidth;
    //        int vertex_start_index = quad_index * 4;
    //        this->m_pGLCore->glDrawArrays(GL_TRIANGLE_STRIP, vertex_start_index, 4);
    //    }
    //}

    this->m_pGLCore->glDrawArrays(GL_TRIANGLES, 0, (unsigned long long)this->m_nHeightMapWidth * (unsigned long long)this->m_nHeightMapHeight * 6);

    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR_1(this->m_pGLCore);
    this->m_pGLCore->glUseProgram(0);
    CHECK_GL_ERROR_1(this->m_pGLCore);

    this->m_pGLCore->glPolygonMode(GL_FRONT, GL_FILL);
    this->m_pGLCore->glDisable(GL_DEPTH_TEST);
    CHECK_GL_ERROR_1(this->m_pGLCore);

}

void CDrawGround::Draw() {
    //this->DrawBound();
    this->DrawTiff();
}
