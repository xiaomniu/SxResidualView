#ifndef CLAYERDRAW_H
#define CLAYERDRAW_H

#include <string>
#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Src_GlWnd/COpenGLCore.h"
#include "Src_Geometry/CLayerVector.h"
#include "Src_Core/BaseVectorStruct.h"
#include "Src_Core/CGlobal.h"
//#include "Src_Geometry/SxRasterThumbnail.h"

class CHitParam{
public:
    CHitParam(){}
    virtual ~CHitParam(){}

    double m_fGeoMouseX = 0.0;
    double m_fGeoMouseY = 0.0;
    int m_nPointIndxPrev = -1;
    int m_nPointIndx = -1;
    int m_nPointIndxNext = -1;

    glm::dvec3 m_pointPrev;
    glm::dvec3 m_point;
    glm::dvec3 m_pointNext;
};

typedef union _CHUNK_ATTRIBUTE_VALUE{
    char m_ByteValue;
    int m_IntValue;
    long m_LongValue;
    long long m_LLongValue;

    float m_FloatValue;
    double m_DoubleValue;

    unsigned char m_uByteValue;
    unsigned int m_uIntValue;
    unsigned long m_uLongValue;
    unsigned long long m_uLLongValue;

    void* m_BinValue;
    char* m_StrValue;
    wchar_t* m_WStrValue;
    //std::string m_sValue;
}CHUNK_ATTRIBUTE_VALUE;

class CChunkAttributeInfo {
public:
    ECHUNK_ATTRIBUTE_TYPE m_eAttributeType;
    CHUNK_ATTRIBUTE_VALUE m_nChunkAttributeValue;
    CChunkAttributeInfo(){
        this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unknown;
    }
    ~CChunkAttributeInfo(){
        //if(this->m_eAttributeType == ECHUNK_ATTRIBUTE_TYPE::e_char_ptr){
        //    delete [] this->m_nChunkAttributeValue.m_StrValue;
        //    this->m_nChunkAttributeValue.m_StrValue = nullptr;
        //}
    }
    CChunkAttributeInfo(char a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_char; this->m_nChunkAttributeValue.m_ByteValue = a;}
    CChunkAttributeInfo(int a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_int; this->m_nChunkAttributeValue.m_IntValue = a;}
    CChunkAttributeInfo(long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_long; this->m_nChunkAttributeValue.m_LongValue = a;}
    CChunkAttributeInfo(long long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_long_long; this->m_nChunkAttributeValue.m_LLongValue = a;}

    CChunkAttributeInfo(float a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_float; this->m_nChunkAttributeValue.m_FloatValue = a;}
    CChunkAttributeInfo(double a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_double; this->m_nChunkAttributeValue.m_DoubleValue = a;}

    CChunkAttributeInfo(unsigned char a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_char; this->m_nChunkAttributeValue.m_uByteValue = a;}
    CChunkAttributeInfo(unsigned int a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_int; this->m_nChunkAttributeValue.m_uIntValue = a;}
    CChunkAttributeInfo(unsigned long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long; this->m_nChunkAttributeValue.m_uLongValue = a;}
    CChunkAttributeInfo(unsigned long long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long_long; this->m_nChunkAttributeValue.m_uLLongValue = a;}

    CChunkAttributeInfo(void* a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_void_ptr; this->m_nChunkAttributeValue.m_BinValue = a;}
    CChunkAttributeInfo(char* a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr; this->m_nChunkAttributeValue.m_StrValue = a;}
    CChunkAttributeInfo(wchar_t* a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_wchar_ptr; this->m_nChunkAttributeValue.m_WStrValue = a;}

    void SetAttributeValue(char a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_char; this->m_nChunkAttributeValue.m_ByteValue = a;}
    void SetAttributeValue(int a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_int; this->m_nChunkAttributeValue.m_IntValue = a;}
    void SetAttributeValue(long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_long; this->m_nChunkAttributeValue.m_LongValue = a;}
    void SetAttributeValue(long long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_long_long; this->m_nChunkAttributeValue.m_LLongValue = a;}

    void SetAttributeValue(float a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_float; this->m_nChunkAttributeValue.m_FloatValue = a;}
    void SetAttributeValue(double a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_double; this->m_nChunkAttributeValue.m_DoubleValue = a;}

    void SetAttributeValue(unsigned char a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_char; this->m_nChunkAttributeValue.m_uByteValue = a;}
    void SetAttributeValue(unsigned int a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_int; this->m_nChunkAttributeValue.m_uIntValue = a;}
    void SetAttributeValue(unsigned long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long; this->m_nChunkAttributeValue.m_uLongValue = a;}
    void SetAttributeValue(unsigned long long a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_unsigned_long_long; this->m_nChunkAttributeValue.m_uLLongValue = a;}

    void SetAttributeValue(void* a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_void_ptr; this->m_nChunkAttributeValue.m_BinValue = a;}
    void SetAttributeValue(char* a){
        this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_char_ptr;
        int nStrLen = (int)strlen(a);
        char* szBuf = new char[nStrLen + 1];
        memset(szBuf, 0, nStrLen + 1);
        memcpy(szBuf, a, nStrLen);
        this->m_nChunkAttributeValue.m_StrValue = szBuf;
    }
    void SetAttributeValue(wchar_t* a){this->m_eAttributeType = ECHUNK_ATTRIBUTE_TYPE::e_wchar_ptr; this->m_nChunkAttributeValue.m_WStrValue = a;}


    inline char GetByteValue(){return this->m_nChunkAttributeValue.m_ByteValue;}
    inline int GetIntValue(){return this->m_nChunkAttributeValue.m_IntValue;}
    inline long GetLongValue(){return this->m_nChunkAttributeValue.m_LongValue;}
    inline long long GetLLongValue(){return this->m_nChunkAttributeValue.m_LLongValue;}

    inline float GetFloatValue(){return this->m_nChunkAttributeValue.m_FloatValue;}
    inline double GetDoubleValue(){return this->m_nChunkAttributeValue.m_DoubleValue;}

    inline unsigned char GetuByteValue(){return this->m_nChunkAttributeValue.m_uByteValue;}
    inline unsigned int GetuIntValue(){return this->m_nChunkAttributeValue.m_uIntValue;}
    inline unsigned long GetuLongValue(){return this->m_nChunkAttributeValue.m_uLongValue;}
    inline unsigned long long GetuLLongValue(){return this->m_nChunkAttributeValue.m_uLLongValue;}

    inline void* GetBinValue(){return this->m_nChunkAttributeValue.m_BinValue;}
    inline char* GetStrValue(){return this->m_nChunkAttributeValue.m_StrValue;}
    inline wchar_t* GetWStrValue(){return this->m_nChunkAttributeValue.m_WStrValue;}

    inline int GetAttrType(){return (int)this->m_eAttributeType;}
    inline bool IsEmpty(){return (this->m_eAttributeType == ECHUNK_ATTRIBUTE_TYPE::e_unknown);}
    std::string GetValueAsString(int nPrecesion = 9){
        std::string sValue;
        char szFmt[32]={0};
        char szValue[128]={0};
        sprintf(szFmt, "%%.%df", nPrecesion);
        switch (this->m_eAttributeType) {
        case ECHUNK_ATTRIBUTE_TYPE::e_char:{
            sValue = std::to_string(this->m_nChunkAttributeValue.m_ByteValue);
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_int:{
            sValue = std::to_string(this->m_nChunkAttributeValue.m_IntValue);
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_long:{
            sValue = std::to_string(this->m_nChunkAttributeValue.m_LongValue);
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_long_long:{
            sValue = std::to_string(this->m_nChunkAttributeValue.m_LLongValue);
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_float:{
            sprintf(szValue, (const char*)szFmt, this->m_nChunkAttributeValue.m_FloatValue);
            sValue = szValue;
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_double:{
            sprintf(szValue, (const char*)szFmt, this->m_nChunkAttributeValue.m_DoubleValue);
            sValue = szValue;
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_char_ptr:{
            sValue = this->m_nChunkAttributeValue.m_StrValue;
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_wchar_ptr:{
            break;
        }
        case ECHUNK_ATTRIBUTE_TYPE::e_void_ptr:{
            break;
        }
        default:{
        }
        }
        return sValue;
    }
};

class CLayerDraw;
class CChunk{
public:
    CChunk();
    virtual ~CChunk();

    unsigned char m_nOperateState = ECHUNK_OPERATE_TYPE::e_Chunk_UnKnowState; //
    unsigned char m_nDisplayState = ESHOWORHIDE::e_show;
    int m_nIndxInEleChunks = -1;
    int m_nChunkType = 0;
    CLayerDraw* m_pBelongLayer = nullptr;
    glm::dvec4 m_chunkBound;
    std::vector<glm::dvec3> m_vecChunkPoints;
    std::vector<int> m_vecChunkPointsIndx; //在整个图层中 所有 Points中的索引
    std::vector<CChunkAttributeInfo> m_vecAttributeValues;
    std::map<std::string, CChunkAttributeInfo> m_mapChunkData;

    CChunkAttributeInfo GetAttributeByIndex(int nFldIndx);
    CChunkAttributeInfo GetAttributeByName(const std::string& sFldName);
    int GetAttributeIndxByName(const std::string& sFldName);
    void Clone(CChunk* pOtherChunk);
    void SetFresh();

    //GetAttribute();
    //GetChunkData();
};

class CLayerField{
public:
    CLayerField(){
    }
    ~CLayerField(){
    }
    std::string m_sFieldName;
    int m_nFieldType = -1;
    int m_nFieldWidth = 0;
    int m_nPrecision = 0;
    //ECHUNK_ATTRIBUTE_TYPE m_eFiledType = ECHUNK_ATTRIBUTE_TYPE::e_unknown;
};

class CLayerDraw
{
public:
    CLayerDraw();
    CLayerDraw(COpenGLCore* pGLCore);
    CLayerDraw(COpenGLCore* pGLCore, const std::string& sRasterFileFullPath);
    virtual ~CLayerDraw();
    COpenGLCore* m_pGLCore = nullptr;
    CLayerVector* m_pLayerVector = nullptr;
    std::vector<CChunk*> m_vecChunkDatas;
    std::vector<CLayerField> m_vecLayerFields;
    std::unordered_map<std::string, int> m_mapName2FldIndx;

    virtual void Init();
    virtual void Draw() = 0;
    virtual int UpdateGeoRanderBySelectRange() = 0;
    virtual int DeleteGeometryBySelectRange() = 0;
    virtual int MouseMoving(double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound) = 0;
    virtual int MouseRelease(int nBtnType, double fGeoMouseX, double fGeoMouseY, double* arrMousePointBound){
        return 1;
    }
    virtual int BeingEditor(){
        return 1;
    }
    virtual int AddChunk(CChunk* pChunk){
        this->m_vecChunkDatas.push_back(pChunk);
        return 1;
    }
    virtual int MakeUpChunks(){
        return 1;
    }
    virtual void SetFresh(CChunk* ){

    }

    CChunk* m_pHitChunk = nullptr;

    void AddLayerField(const std::string& sFieldName, int nFieldWidth, int nPrecision, int nFiledType);
    void MakeupFields();
    void DrawTest();

    void ShowEnable(int nShowOrHide);
    int ShowState();

    unsigned long long m_nOperateEleCnt = 0;
    int m_nVecRasterItemCnt = 0;
    std::vector<CRasterDraw*> m_vecRasterItem;

    double m_arrGeoIntersect[4];

    int m_nEnableEdit = 0;
    int m_nRollerBlinding = 0;
    int m_nBeingEditor = 0;
    int m_nShowMosaicLine = 0;
    int m_nLayerType = 0;

    int m_nShowOrHide = ESHOWORHIDE::e_show;
    int m_nShowOrHideLast = ESHOWORHIDE::e_show;

    int m_nMovingPoint = 0;
    int m_nHitParamFlag = 0;
    CHitParam* m_pHitParam = nullptr;

    std::string m_sLayerName;
    CLayerDraw* m_pRelationLayer = nullptr;

    int IntersectionLine(const t_Line2D &l1, const t_Line2D &l2);
    int GetIntersectionPoint(const t_Line2D &l1, const t_Line2D &l2, glm::dvec2& ptRet);
    int GetIntersectionPoint(glm::dvec2& pt1, glm::dvec2& pt2, glm::dvec2& pt3, glm::dvec2& pt4, glm::dvec2& ptRet);
};

#endif // CLAYERDRAW_H
