#ifndef READRESIDUALFILE_H
#define READRESIDUALFILE_H

#include <string>
#include <vector>
#include <unordered_map>
///////////////////////////////////////////////////////////////////////////////////////////////////////
// RPC改正数
class CRpcAdjustFactor {
public:
	CRpcAdjustFactor() {}
	~CRpcAdjustFactor() {}

    unsigned long long m_nPixWidth;
    unsigned long long m_nPixHeight;
    double m_fPixResolution;
    double m_arrQuadCoord[8];// leftBottom, leftTop, rightTop, rightBottom

	double m_fFactor[12];
	std::string m_sTifName;
    std::string m_sTifFileFullPath;
    std::string m_sRpcFileFullPath;
    std::string m_sPxyFileFullPath;
    std::string m_sGcpFileFullPath;
};
class CRpcAdjustFactorBlock {
public:
    CRpcAdjustFactorBlock() {}
    ~CRpcAdjustFactorBlock() {}
    std::vector < CRpcAdjustFactor*>                     m_vecRpcAdjustFactor;
    std::unordered_map<std::string, CRpcAdjustFactor*>   m_mapRpcAdjustFactor2VecIndx;
    std::unordered_map<std::string, CRpcAdjustFactor*>   m_mapRpcAdjustFactorName2VecIndx;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//连接点坐标
class CLianJieDianXiangFangWuCha;
class CLianJieDianZuoBiao {
public:
	CLianJieDianZuoBiao() {}
	~CLianJieDianZuoBiao() {}
	unsigned long long m_nDianHao;
	double m_fLng;
	double m_fLngDu;
	double m_fLngFen;
	double m_fLngMiao;
	double m_fLat;
	double m_fLatDu;
	double m_fLatFen;
	double m_fLatMiao;
	double m_fHeight;

	std::string m_sDianHao;
    CLianJieDianXiangFangWuCha* m_pLianJieDianXiangFangWuCha1 = nullptr;
    CLianJieDianXiangFangWuCha* m_pLianJieDianXiangFangWuCha2 = nullptr;
};
class CLianJieDianZuoBiaoBlock {
public:
	CLianJieDianZuoBiaoBlock() {}
	~CLianJieDianZuoBiaoBlock() {}
	std::vector < CLianJieDianZuoBiao*>                            m_vecLianJieDianZuoBiao;
	std::unordered_map<unsigned long long, CLianJieDianZuoBiao*>   m_mapLianJieDianZuoBiao2VecIndx;
};

//连接点的像方误差（pixel）
class CLianJieDianXiangFangWuCha {
public:
	CLianJieDianXiangFangWuCha() {}
	~CLianJieDianXiangFangWuCha() {}

	unsigned long long m_nDianHao;
	double m_fPix_X;
	double m_fPix_Y;
	double m_fPix_Residual_X;
	double m_fPix_Residual_Y;
	double m_fPix_Residual_Plane;
	double m_fWeight;

	std::string m_sDianHao;
	std::string m_sTifName;
};
class CLianJieDianXiangFangWuChaBlock {
public:
	CLianJieDianXiangFangWuChaBlock() {}
	~CLianJieDianXiangFangWuChaBlock() {}
	std::vector < CLianJieDianXiangFangWuCha*>                                         m_vecLianJieDianXiangFangWuCha;
	std::unordered_map<unsigned long long, std::vector<CLianJieDianXiangFangWuCha*>>   m_mapLianJieDianXiangFangWuCha2VecIndx;
};
//连接点的像方中误差（pixel）
class CLianJieDianXiangFangZhongWuCha {
public:
	CLianJieDianXiangFangZhongWuCha() {}
	~CLianJieDianXiangFangZhongWuCha() {}
	double m_fPix_Residual_X;
	double m_fPix_Residual_Y;
	double m_fPix_Residual_Plane;
};

//连接点最大像方误差（pixel）
class CLianJieDianXiangFangMaxWuCha {
public:
	CLianJieDianXiangFangMaxWuCha() {}
	~CLianJieDianXiangFangMaxWuCha() {}
	double m_fMax_Pix_Residual_X;
	double m_fMax_Pix_Residual_Y;
	double m_fMax_Pix_Residual_Plane;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//控制点的精度
class CControlDianXiangFangWuCha;
class CControlDianPrecision {
public:
	CControlDianPrecision() {}
	~CControlDianPrecision() {}

	unsigned long long m_nDianHao;
	double m_fLng;
	double m_fLat;
	double m_fHeight;
	double m_fResidual_X;
	double m_fResidual_Y;
	double m_fResidual_Plane;
	double m_fResidual_Height;
	double m_fAngle;

	std::string m_sDianHao;
    CControlDianXiangFangWuCha* m_pControlDianXiangFangWuCha = nullptr;
};
class CControlDianPrecisionBlock {
public:
	CControlDianPrecisionBlock() {}
	~CControlDianPrecisionBlock() {}
	std::vector < CControlDianPrecision*>                                                 m_vecControlDianPrecision;
	std::unordered_map <unsigned long long, CControlDianPrecision*>                       m_mapControlDianPrecision2VecIndx;

};

//控制点物方精度（m）
class CControlDianGroundPrecision {
public:
	CControlDianGroundPrecision() {}
	~CControlDianGroundPrecision() {}
	double m_fResidual_X;
	double m_fResidual_Y;
	double m_fResidual_Plane;
	double m_fResidual_Height;
};

//控制点物方最大误差（m）
class CControlDianGroundMaxWuCha {
public:
	CControlDianGroundMaxWuCha() {}
	~CControlDianGroundMaxWuCha() {}
	double m_fMax_Residual_X;
	double m_fMax_Residual_Y;
	double m_fMax_Residual_Plane;
	double m_fMax_Residual_Height;
};

//控制点的像方误差（pixel）
class CControlDianXiangFangWuCha {
public:
	CControlDianXiangFangWuCha() {}
	~CControlDianXiangFangWuCha() {}

	unsigned long long m_nDianHao;
	double m_fPix_X;
	double m_fPix_Y;
	double m_fPix_Residual_X;
	double m_fPix_Residual_Y;
	double m_fWeight;

	std::string m_sDianHao;
	std::string m_sTifName;
};
class CControlDianXiangFangWuChaBlock {
public:
	CControlDianXiangFangWuChaBlock() {}
	~CControlDianXiangFangWuChaBlock() {}
	std::vector < CControlDianXiangFangWuCha*>                                                 m_vecControlDianXiangFangWuCha;
	std::unordered_map <unsigned long long, CControlDianXiangFangWuCha*>                       m_mapControlDianXiangFangWuCha2VecIndx;

};

//控制点的像方中误差（pixel）
class CControlDianXiangFangZhongWuCha {
public:
	CControlDianXiangFangZhongWuCha() {}
	~CControlDianXiangFangZhongWuCha() {}

	double m_fPix_Residual_X;
	double m_fPix_Residual_Y;
	double m_fPix_Residual_Plane;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//检查点的精度
class CCheckDianXiangFangWuCha;
class CCheckDianPrecision {
public:
	CCheckDianPrecision() {}
	~CCheckDianPrecision() {}

	unsigned long long m_nDianHao;
	double m_fLng;
	double m_fLat;
	double m_fHeight;
	double m_fResidual_X;
	double m_fResidual_Y;
	double m_fResidual_Plane;
	double m_fResidual_Height;
	double m_fAngle;

	std::string m_sDianHao;
    CCheckDianXiangFangWuCha* m_pCheckDianXiangFangWuCha = nullptr;
};
class CCheckDianPrecisionBlock {
public:
	CCheckDianPrecisionBlock() {}
	~CCheckDianPrecisionBlock() {}
	std::vector < CCheckDianPrecision*>                                                 m_vecCheckDianPrecision;
	std::unordered_map <unsigned long long, CCheckDianPrecision*>                       m_mapCheckDianPrecision2VecIndx;

};

//检查点物方精度（m）
class CCheckDianGroundPrecision {
public:
	CCheckDianGroundPrecision() {}
	~CCheckDianGroundPrecision() {}
	double m_fResidual_X;
	double m_fResidual_Y;
	double m_fResidual_Plane;
	double m_fResidual_Height;
};

//检查点的像方误差（pixel）
class CCheckDianGroundMaxWuCha {
public:
	CCheckDianGroundMaxWuCha() {}
	~CCheckDianGroundMaxWuCha() {}
	double m_fMax_Residual_X;
	double m_fMax_Residual_Y;
	double m_fMax_Residual_Plane;
	double m_fMax_Residual_Height;
};

//检查点的像方误差（pixel）
class CCheckDianXiangFangWuCha {
public:
	CCheckDianXiangFangWuCha() {}
	~CCheckDianXiangFangWuCha() {}

	unsigned long long m_nDianHao;
	double m_fPix_X;
	double m_fPix_Y;
	double m_fPix_Residual_X;
	double m_fPix_Residual_Y;
	double m_fWeight;

	std::string m_sDianHao;
	std::string m_sTifName;
};
class CCheckDianXiangFangWuChaBlock {
public:
	CCheckDianXiangFangWuChaBlock() {}
	~CCheckDianXiangFangWuChaBlock() {}
	std::vector < CCheckDianXiangFangWuCha*>                                                 m_vecCheckDianXiangFangWuCha;
	std::unordered_map <unsigned long long, CCheckDianXiangFangWuCha*>                       m_mapCheckDianXiangFangWuCha2VecIndx;

};

//检查点的像方中误差（pixel）
class CCheckDianXiangFangZhongWuCha {
public:
	CCheckDianXiangFangZhongWuCha() {}
	~CCheckDianXiangFangZhongWuCha() {}

	double m_fPix_Residual_X;
	double m_fPix_Residual_Y;
	double m_fPix_Residual_Plane;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CResidualInfo {
public:
	CResidualInfo();
	~CResidualInfo();

    CRpcAdjustFactorBlock                       m_RpcAdjustFactorBlock;
	//>>>>>>>>>>>>>>>>>>>>>>>
	CLianJieDianZuoBiaoBlock                    m_LianJieDianZuoBiaoBlock;
	CLianJieDianXiangFangWuChaBlock             m_LianJieDianXiangFangWuChaBlock;
	CLianJieDianXiangFangZhongWuCha             m_LianJieDianXiangFangZhongWuCha;
	CLianJieDianXiangFangMaxWuCha               m_LianJieDianXiangFangMaxWuCha;
	//>>>>>>>>>>>>>>>>>>>>>>>
	CControlDianPrecisionBlock                  m_ControlDianPrecisionBlock;
	CControlDianGroundPrecision                 m_ControlDianGroundPrecision;
	CControlDianGroundMaxWuCha                  m_ControlDianGroundMaxWuCha;
	CControlDianXiangFangWuChaBlock             m_ControlDianXiangFangWuChaBlock;
	CControlDianXiangFangZhongWuCha             m_ControlDianXiangFangZhongWuCha;
	//>>>>>>>>>>>>>>>>>>>>>>>
	CCheckDianPrecisionBlock                    m_CheckDianPrecisionBlock;
	CCheckDianGroundPrecision                   m_CheckDianGroundPrecision;
	CCheckDianGroundMaxWuCha                    m_CheckDianGroundMaxWuCha;
	CCheckDianXiangFangWuChaBlock               m_CheckDianXiangFangWuChaBlock;
	CCheckDianXiangFangZhongWuCha               m_CheckDianXiangFangZhongWuCha;

    void Clear();
    void ClearFileInfo();

    void GetRasterQuad(const std::string& sRpcFileFullPath, double* pQuadCoord, unsigned long long _width, unsigned long long _height,  double& _resolution);
	std::string& Replace_str(std::string& str, const std::string& to_replaced, const std::string& newchars);
	int SplitString(std::vector<std::string>& resultStringVector, const std::string& srcStrIn, const std::string& delimStr, bool repeatedCharIgnored);
	int LoadResidualFile(const std::string& sResidualFileFullPath);
};

#endif // !READRESIDUALFILE_H

