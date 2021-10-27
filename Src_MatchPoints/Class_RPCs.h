#ifndef RPCTOOL_H
#define RPCTOOL_H

#include <string>

class Class_RPCs
{
public:
	Class_RPCs();
    void initial_RPCs(const std::string& _pathname);
	~Class_RPCs();
public:
	double lefttopLatitude;
	double lefttopLongitude;
	double leftbottomLatitude;
	double leftbottomLongitude;
	double righttopLatitude;
	double righttopLongitude;
	double rightbottomlatitude;
	double rightbottomLongitude;

	double ** MatrixInter_A;//用于根据影像坐标计算物方坐标的矩阵
	double ** MatrixInter_AT;
	double ** MatrixInter_L;
	double ** MatrixInter_ATA;
	double ** MatrixInter_ATL;
	double ** MatrixInter_X;
	double m_a[20];//有理函数模型rpc系数
	double m_b[20];
	double m_c[20];
	double m_d[20];
	double m_LINE_SCALE;
	double m_SAMP_SCALE;
	double m_LINE_OFF;
	double m_SAMP_OFF;
	double m_LAT_SCALE;
	double m_LONG_SCALE;
	double m_HEIGHT_SCALE;
	double m_LAT_OFF;
	double m_LONG_OFF;
public:
	double m_HEIGHT_OFF;
private:
	double m_CL0;//仿射变换系数，暂时计算过程中没有用，以后根据需要进行添加
	double m_CLS;
	double m_CLL;
	double m_CS0;
	double m_CSS;
	double m_CSL;
    void readRPCs(const std::string& _pathname);//根据rpc文件路径读取影像rpc系数
public:
	//根据物方三维物方坐标，计算影像坐标；参数分别为纬度、经度、高程、影像x坐标、影像y坐标
	void Calculate_image_xy(double _Latitude, double _Longitude, double _Height, double& _imagex, double& _imagey);
	//根据像方坐标，计算物方三维坐标；参数分别为影像x坐标、影像y坐标、纬度、经度、高程，高程目前采用rpc系数中的m_HEIGHT_OFF
	void Calculate_Ground_XYZ(double _imagex, double _imagey, double& _Latitude, double& _Longitude, double& _Height);
private:
	//根据物方三维坐标计算一级泰勒展开式系数，Calculate_Ground_XYZ()调用
	void Calculate_Correcs(double _Latitude, double _Longitude, double _Height,
		double& _CoffN1, double& _CoffN2, double& _CoffN3,
		double& _CoffN4, double& _CoffN5, double& _CoffN6);
public:
	//矩阵相乘函数
	void Cal_MatrixMul(double** result_matrix, double** Matrix1, double** matrix2, int n1, int n2, int n3);
	//矩阵求逆
	void inverse(double** m_InMatrix, int m_number);
};
#endif // RPCTOOL_H
