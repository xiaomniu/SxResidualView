#ifndef CLAYERGEODRAW_H
#define CLAYERGEODRAW_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "Src_Geometry/CLayerDraw.h"

class CLayerFilter {
public:
    CLayerFilter(){}
    ~CLayerFilter(){}

    enum {
        e_By_Unknown = 0,
        e_By_RangeNum = 1,
        e_By_RangeNum_double = 2,
    };

    int m_nFilterType = CLayerFilter::e_By_Unknown;

    int m_nRangeNum = -1;
    void SetRangeNumFilter(int nRangeNum) {
        this->m_nFilterType = CLayerFilter::e_By_RangeNum;
        this->m_nRangeNum = nRangeNum;
    }

    double m_fRangeNum = -1.0;
    void SetRangeNumFilter(double fRangeNum) {
        this->m_nFilterType = CLayerFilter::e_By_RangeNum_double;
        this->m_fRangeNum = fRangeNum;
    }
};


class CLayerGeoDraw : public CLayerDraw
{
public:
    CLayerGeoDraw();
    CLayerGeoDraw(COpenGLCore* pGLCore);
    CLayerGeoDraw(COpenGLCore* pGLCore, const std::string& sVectorFileFullPath);
    virtual ~CLayerGeoDraw();

    void InitBody();

    std::string m_sVectorImgFileFullPath;

    std::vector<glm::dvec3>* m_pVecPoints = nullptr;//存放所有的顶点坐标值信息
    std::vector<float>* m_pVecPointFlag = nullptr;//用于显示不用的颜色， 和 m_pVecPoints 元素个数一致
    std::vector<float>* m_pVecPointFlagOperate = nullptr;//被操作的图形（如所有选中的图形），用于显示不用的颜色， 和 m_pVecPoints 元素个数一致

    std::vector<unsigned int>* m_pVecVerticsIndx = nullptr;//准备Draw的顶点坐标索引

    std::vector<unsigned int>* m_pVecPointInPolyIndx = nullptr;//每个顶点在哪个polygon/polyline中，对应Polygon/polyline的下标

    std::vector<unsigned int>* m_pVecPointsIndxOperatePointNode = nullptr;//被操作的图形（如所有选中的图形），所有的节点索引，突出图形的前3个起点
    std::vector<unsigned int>* m_pVecPointsIndxOperate = nullptr;//被操作的图形（如所有选中的图形）， 上一个点链接下一个点 ，存放上一个，下一个坐标的索引，连成线

    std::vector<unsigned int>* m_pVecPointsIndxOperatePointNodeHit = nullptr;//被操作的"高亮"图形（如所有选中的图形），所有的节点索引，突出图形的前3个起点
    std::vector<unsigned int>* m_pVecPointsIndxOperateHit = nullptr;//被操作的"高亮"图形（如所有选中的图形）， 上一个点链接下一个点 ，存放上一个，下一个坐标的索引，连成线

    std::vector<int>* m_pVecEleChunksOperate = nullptr;//被操作的图形（如所有选中的图形），选中 Polygon / Polyline 的索引，对应的 m_pVecEleChunks 中的索引

    //每个Polygon、Polyline、Point的名称标志，和 m_pVecPoints（图形是Point时）或 m_pVecEleChunks（图形是Polygon/Polyline时）是一对一的， 与其元素个数一致
    std::vector<std::string>* m_pVecChunksName = nullptr;
    std::unordered_map<std::string, int>* m_pMapChunkName2Indx = nullptr;//每个名字标志，对应的 m_pVecChunksName/m_pVecEleChunks 中的一个坐标的下标，与其元素个数一致

    int m_nHasZValue = 0;
    std::string m_sCoordProject;
    double m_fMaxLng = -99999999.99999, m_fMaxLat = -99999999.99999;
    double m_fMinLng = 99999999.99999, m_fMinLat = 99999999.99999;
    unsigned int m_nLayerColor = 0xFF00FFFF;//0xAABBGGRR

    void* m_pExtData = nullptr;

    virtual void Init();
    virtual void Draw();
    virtual int UpdateGeoRanderBySelectRange();
    virtual int DeleteGeometryBySelectRange();
    virtual int RemoveAllChunks(bool bNeedDelete);
    virtual int UninitBody();

    virtual int AddChunk(int nPointCount, double* pPoints){
        return 1;
    }
    virtual int AddChunk(CChunk* pChunk){
        this->m_vecChunkDatas.push_back(pChunk);
        return 1;
    }
    virtual int GetChunkCount(){
        return (int)this->m_vecChunkDatas.size();
    }
    virtual int BeingEditor(){
        return 1;
    }
    virtual int SaveEditor(){
        return 1;
    }
    virtual void SetFresh(CChunk* ){

    }
    virtual void ClearOperateChunkState();

    virtual void ReCalcChunkBound(CChunk* pChunk);
    virtual void ReCalcLayerBoundByChunks(int nChunkIdx = -1);

    virtual CChunk* GetSelectChunk(){
        if(this->m_pVecEleChunksOperate->size() <= 0){
            return nullptr;
        }
        int nselectIndx = this->m_pVecEleChunksOperate->at(0);
        return this->m_vecChunkDatas[nselectIndx];
    }
    virtual CChunk* GetChunkByIndex(int nChunkIndex){
        int nChunkCount = (int)this->m_vecChunkDatas.size();
        if(nChunkCount <= 0 || nChunkIndex < 0 || nChunkIndex >= nChunkCount){
            return nullptr;
        }
        return this->m_vecChunkDatas[nChunkIndex];
    }


    int IsContainsByPoint(double* pSrcRect, double* pDstPoint);
    int IsContains(double* pSrcRect, double* pDstRect);
    int IsOverlaps(double* pSrcRect, double* pDstRect);
    int IsOverlapsByLine(double* pSrcRect, double* pDstLine);
    int GetFootOfPerpendicular(double* pPointStart, double* pPointEnd, double* pPointOut, double* pPointFoot);


    CLayerFilter* m_pLayerFilter = nullptr;
    void SetFilter(CLayerFilter* pLayerFilter) {
        this->m_pLayerFilter = pLayerFilter;
    }
    void FreshFilter(int nAttributeIndx = 0);
    void FreshFilter(std::vector<int> vecAttributeIndx);

    void DrawTest();
};

#endif // CLAYERGEODRAW_H
