
#include "Class_RPCs.h"



Class_RPCs::Class_RPCs()
{
}

void Class_RPCs::initial_RPCs(const std::string& _pathname)
{
	MatrixInter_A = new double *[2];
	MatrixInter_AT = new double *[2];
	MatrixInter_L = new double *[2];
	MatrixInter_ATA = new double *[2];
	MatrixInter_ATL = new double *[2];
	MatrixInter_X = new double *[2];
	for (int i = 0; i<2; i++)
	{
		MatrixInter_A[i] = new double[2];
		MatrixInter_AT[i] = new double[2];
		MatrixInter_L[i] = new double;
		MatrixInter_ATA[i] = new double[2];
		MatrixInter_ATL[i] = new double;
		MatrixInter_X[i] = new double;
	}
	readRPCs(_pathname);
}

Class_RPCs::~Class_RPCs()
{
/*	for (int i = 0; i<2; i++)
	{
		delete[] MatrixInter_A[i];
		delete[] MatrixInter_AT[i];
		delete[] MatrixInter_L[i];
		delete[] MatrixInter_ATA[i];
		delete[] MatrixInter_ATL[i];
		delete[] MatrixInter_X[i];
	}
	delete[] MatrixInter_A;
	delete[] MatrixInter_AT;
	delete[] MatrixInter_L;
	delete[] MatrixInter_ATA;
	delete[] MatrixInter_ATL;
	delete[] MatrixInter_X;*/
}

void Class_RPCs::readRPCs(const std::string& _pathname)
{
	char disable[40];
    FILE * fp = fopen(_pathname.c_str(), ("r"));
	if (fp != NULL)
	{
		fscanf(fp, "%s %lf %s", disable, &m_LINE_OFF, disable);
		fscanf(fp, "%s %lf %s", disable, &m_SAMP_OFF, disable);
		fscanf(fp, "%s %lf %s", disable, &m_LAT_OFF, disable);
		fscanf(fp, "%s %lf %s", disable, &m_LONG_OFF, disable);
		fscanf(fp, "%s %lf %s", disable, &m_HEIGHT_OFF, disable);
		fscanf(fp, "%s %lf %s", disable, &m_LINE_SCALE, disable);
		fscanf(fp, "%s %lf %s", disable, &m_SAMP_SCALE, disable);
		fscanf(fp, "%s %lf %s", disable, &m_LAT_SCALE, disable);
		fscanf(fp, "%s %lf %s", disable, &m_LONG_SCALE, disable);
		fscanf(fp, "%s %lf %s", disable, &m_HEIGHT_SCALE, disable);

		for (int i1 = 0; i1<20; i1++)
			fscanf(fp, "%s %lf", disable, &m_a[i1]);
		for (int i2 = 0; i2<20; i2++)
			fscanf(fp, "%s %lf", disable, &m_b[i2]);
		for (int i3 = 0; i3<20; i3++)
			fscanf(fp, "%s %lf", disable, &m_c[i3]);
		for (int i4 = 0; i4<20; i4++)
			fscanf(fp, "%s %lf", disable, &m_d[i4]);

		fclose(fp);
		return;
	}
	else
	{
		return;
	}
}

void Class_RPCs::Calculate_image_xy(double _Latitude, double _Longitude, double _Height, double& _imagex, double& _imagey)
{
	double _P = (_Latitude - m_LAT_OFF) / m_LAT_SCALE;
	double _L = (_Longitude - m_LONG_OFF) / m_LONG_SCALE;
	double _H = (_Height - m_HEIGHT_OFF) / m_HEIGHT_SCALE;

	double _NumL = m_a[0] + m_a[1] * _L + m_a[2] * _P + m_a[3] * _H + m_a[4] * _L*_P + m_a[5] * _L*_H + m_a[6] * _P*_H + m_a[7] * _L*_L +
		m_a[8] * _P*_P + m_a[9] * _H*_H + m_a[10] * _P*_L*_H + m_a[11] * _L*_L*_L + m_a[12] * _L*_P*_P + m_a[13] * _L*_H*_H +
		m_a[14] * _L*_L*_P + m_a[15] * _P*_P*_P + m_a[16] * _P*_H*_H + m_a[17] * _L*_L*_H + m_a[18] * _P*_P*_H + m_a[19] * _H*_H*_H;
	double _DenL = m_b[0] + m_b[1] * _L + m_b[2] * _P + m_b[3] * _H + m_b[4] * _L*_P + m_b[5] * _L*_H + m_b[6] * _P*_H + m_b[7] * _L*_L +
		m_b[8] * _P*_P + m_b[9] * _H*_H + m_b[10] * _P*_L*_H + m_b[11] * _L*_L*_L + m_b[12] * _L*_P*_P + m_b[13] * _L*_H*_H +
		m_b[14] * _L*_L*_P + m_b[15] * _P*_P*_P + m_b[16] * _P*_H*_H + m_b[17] * _L*_L*_H + m_b[18] * _P*_P*_H + m_b[19] * _H*_H*_H;
	double _NumS = m_c[0] + m_c[1] * _L + m_c[2] * _P + m_c[3] * _H + m_c[4] * _L*_P + m_c[5] * _L*_H + m_c[6] * _P*_H + m_c[7] * _L*_L +
		m_c[8] * _P*_P + m_c[9] * _H*_H + m_c[10] * _P*_L*_H + m_c[11] * _L*_L*_L + m_c[12] * _L*_P*_P + m_c[13] * _L*_H*_H +
		m_c[14] * _L*_L*_P + m_c[15] * _P*_P*_P + m_c[16] * _P*_H*_H + m_c[17] * _L*_L*_H + m_c[18] * _P*_P*_H + m_c[19] * _H*_H*_H;
	double _DenS = m_d[0] + m_d[1] * _L + m_d[2] * _P + m_d[3] * _H + m_d[4] * _L*_P + m_d[5] * _L*_H + m_d[6] * _P*_H + m_d[7] * _L*_L +
		m_d[8] * _P*_P + m_d[9] * _H*_H + m_d[10] * _P*_L*_H + m_d[11] * _L*_L*_L + m_d[12] * _L*_P*_P + m_d[13] * _L*_H*_H +
		m_d[14] * _L*_L*_P + m_d[15] * _P*_P*_P + m_d[16] * _P*_H*_H + m_d[17] * _L*_L*_H + m_d[18] * _P*_P*_H + m_d[19] * _H*_H*_H;
	_imagey = (_NumL / _DenL)*m_LINE_SCALE + m_LINE_OFF;
	_imagex = (_NumS / _DenS)*m_SAMP_SCALE + m_SAMP_OFF;
}

void Class_RPCs::Calculate_Correcs(double _Latitude, double _Longitude, double _Height,
	double& _CoffN1, double& _CoffN2, double& _CoffN3,
	double& _CoffN4, double& _CoffN5, double& _CoffN6)
{
	double _P = (_Latitude - m_LAT_OFF) / m_LAT_SCALE;
	double _L = (_Longitude - m_LONG_OFF) / m_LONG_SCALE;
	double _H = (_Height - m_HEIGHT_OFF) / m_HEIGHT_SCALE;

	double _NumL_Lat, _NumL_Long, _NumL_Height, _DenL_Lat, _DenL_Long, _DenL_Height;
	double _NumS_Lat, _NumS_Long, _NumS_Height, _DenS_Lat, _DenS_Long, _DenS_Height;

	double _NumL = m_a[0] + m_a[1] * _L + m_a[2] * _P + m_a[3] * _H + m_a[4] * _L*_P + m_a[5] * _L*_H + m_a[6] * _P*_H + m_a[7] * _L*_L +
		m_a[8] * _P*_P + m_a[9] * _H*_H + m_a[10] * _P*_L*_H + m_a[11] * _L*_L*_L + m_a[12] * _L*_P*_P + m_a[13] * _L*_H*_H +
		m_a[14] * _L*_L*_P + m_a[15] * _P*_P*_P + m_a[16] * _P*_H*_H + m_a[17] * _L*_L*_H + m_a[18] * _P*_P*_H + m_a[19] * _H*_H*_H;
	double _DenL = m_b[0] + m_b[1] * _L + m_b[2] * _P + m_b[3] * _H + m_b[4] * _L*_P + m_b[5] * _L*_H + m_b[6] * _P*_H + m_b[7] * _L*_L +
		m_b[8] * _P*_P + m_b[9] * _H*_H + m_b[10] * _P*_L*_H + m_b[11] * _L*_L*_L + m_b[12] * _L*_P*_P + m_b[13] * _L*_H*_H +
		m_b[14] * _L*_L*_P + m_b[15] * _P*_P*_P + m_b[16] * _P*_H*_H + m_b[17] * _L*_L*_H + m_b[18] * _P*_P*_H + m_b[19] * _H*_H*_H;
	double _NumS = m_c[0] + m_c[1] * _L + m_c[2] * _P + m_c[3] * _H + m_c[4] * _L*_P + m_c[5] * _L*_H + m_c[6] * _P*_H + m_c[7] * _L*_L +
		m_c[8] * _P*_P + m_c[9] * _H*_H + m_c[10] * _P*_L*_H + m_c[11] * _L*_L*_L + m_c[12] * _L*_P*_P + m_c[13] * _L*_H*_H +
		m_c[14] * _L*_L*_P + m_c[15] * _P*_P*_P + m_c[16] * _P*_H*_H + m_c[17] * _L*_L*_H + m_c[18] * _P*_P*_H + m_c[19] * _H*_H*_H;
	double _DenS = m_d[0] + m_d[1] * _L + m_d[2] * _P + m_d[3] * _H + m_d[4] * _L*_P + m_d[5] * _L*_H + m_d[6] * _P*_H + m_d[7] * _L*_L +
		m_d[8] * _P*_P + m_d[9] * _H*_H + m_d[10] * _P*_L*_H + m_d[11] * _L*_L*_L + m_d[12] * _L*_P*_P + m_d[13] * _L*_H*_H +
		m_d[14] * _L*_L*_P + m_d[15] * _P*_P*_P + m_d[16] * _P*_H*_H + m_d[17] * _L*_L*_H + m_d[18] * _P*_P*_H + m_d[19] * _H*_H*_H;

	_NumL_Lat = (1 / m_LAT_SCALE)*(m_a[2] + m_a[4] * _L + m_a[6] * _H + 2 * m_a[8] * _P + m_a[10] * _L*_H + 2 * m_a[12] * _L*_P +
		m_a[14] * _L*_L + 3 * m_a[15] * _P*_P + m_a[16] * _H*_H + 2 * m_a[18] * _P*_H);
	_DenL_Lat = (1 / m_LAT_SCALE)*(m_b[2] + m_b[4] * _L + m_b[6] * _H + 2 * m_b[8] * _P + m_b[10] * _L*_H + 2 * m_b[12] * _L*_P +
		m_b[14] * _L*_L + 3 * m_b[15] * _P*_P + m_b[16] * _H*_H + 2 * m_b[18] * _P*_H);
	_NumL_Long = (1 / m_LONG_SCALE)*(m_a[1] + m_a[4] * _P + m_a[5] * _H + 2 * m_a[7] * _L + m_a[10] * _P*_H + 3 * m_a[11] * _L*_L +
		m_a[12] * _P*_P + m_a[13] * _H*_H + 2 * m_a[14] * _L*_P + 2 * m_a[17] * _L*_H);
	_DenL_Long = (1 / m_LONG_SCALE)*(m_b[1] + m_b[4] * _P + m_b[5] * _H + 2 * m_b[7] * _L + m_b[10] * _P*_H + 3 * m_b[11] * _L*_L +
		m_b[12] * _P*_P + m_b[13] * _H*_H + 2 * m_b[14] * _L*_P + 2 * m_b[17] * _L*_H);
	_NumL_Height = (1 / m_HEIGHT_SCALE)*(m_a[3] + m_a[5] * _L + m_a[6] * _P + 2 * m_a[9] * _H + m_a[10] * _P*_L + 2 * m_a[13] * _L*_H +
		2 * m_a[16] * _P*_H + m_a[17] * _L*_L + m_a[18] * _P*_P + 3 * m_a[19] * _H*_H);
	_DenL_Height = (1 / m_HEIGHT_SCALE)*(m_b[3] + m_b[5] * _L + m_b[6] * _P + 2 * m_b[9] * _H + m_b[10] * _P*_L + 2 * m_b[13] * _L*_H +
		2 * m_b[16] * _P*_H + m_b[17] * _L*_L + m_b[18] * _P*_P + 3 * m_b[19] * _H*_H);

	_NumS_Lat = (1 / m_LAT_SCALE)*(m_c[2] + m_c[4] * _L + m_c[6] * _H + 2 * m_c[8] * _P + m_c[10] * _L*_H + 2 * m_c[12] * _L*_P +
		m_c[14] * _L*_L + 3 * m_c[15] * _P*_P + m_c[16] * _H*_H + 2 * m_c[18] * _P*_H);
	_DenS_Lat = (1 / m_LAT_SCALE)*(m_d[2] + m_d[4] * _L + m_d[6] * _H + 2 * m_d[8] * _P + m_d[10] * _L*_H + 2 * m_d[12] * _L*_P +
		m_d[14] * _L*_L + 3 * m_d[15] * _P*_P + m_d[16] * _H*_H + 2 * m_d[18] * _P*_H);
	_NumS_Long = (1 / m_LONG_SCALE)*(m_c[1] + m_c[4] * _P + m_c[5] * _H + 2 * m_c[7] * _L + m_c[10] * _P*_H + 3 * m_c[11] * _L*_L +
		m_c[12] * _P*_P + m_c[13] * _H*_H + 2 * m_c[14] * _L*_P + 2 * m_c[17] * _L*_H);
	_DenS_Long = (1 / m_LONG_SCALE)*(m_d[1] + m_d[4] * _P + m_d[5] * _H + 2 * m_d[7] * _L + m_d[10] * _P*_H + 3 * m_d[11] * _L*_L +
		m_d[12] * _P*_P + m_d[13] * _H*_H + 2 * m_d[14] * _L*_P + 2 * m_d[17] * _L*_H);
	_NumS_Height = (1 / m_HEIGHT_SCALE)*(m_c[3] + m_c[5] * _L + m_c[6] * _P + 2 * m_c[9] * _H + m_c[10] * _P*_L + 2 * m_c[13] * _L*_H +
		2 * m_c[16] * _P*_H + m_c[17] * _L*_L + m_c[18] * _P*_P + 3 * m_c[19] * _H*_H);
	_DenS_Height = (1 / m_HEIGHT_SCALE)*(m_d[3] + m_d[5] * _L + m_d[6] * _P + 2 * m_d[9] * _H + m_d[10] * _P*_L + 2 * m_d[13] * _L*_H +
		2 * m_d[16] * _P*_H + m_d[17] * _L*_L + m_d[18] * _P*_P + 3 * m_d[19] * _H*_H);

	_CoffN1 = m_LINE_SCALE*(_NumL_Lat*_DenL - _DenL_Lat*_NumL) / (_DenL*_DenL);
	_CoffN2 = m_LINE_SCALE*(_NumL_Long*_DenL - _DenL_Long*_NumL) / (_DenL*_DenL);
	_CoffN3 = m_LINE_SCALE*(_NumL_Height*_DenL - _DenL_Height*_NumL) / (_DenL*_DenL);
	_CoffN4 = m_SAMP_SCALE*(_NumS_Lat*_DenS - _DenS_Lat*_NumS) / (_DenS*_DenS);
	_CoffN5 = m_SAMP_SCALE*(_NumS_Long*_DenS - _DenS_Long*_NumS) / (_DenS*_DenS);
	_CoffN6 = m_SAMP_SCALE*(_NumS_Height*_DenS - _DenS_Height*_NumS) / (_DenS*_DenS);
}

void Class_RPCs::Calculate_Ground_XYZ(double _imagex, double _imagey, double& _Latitude, double& _Longitude, double& _Height)
{
//	_Height = m_HEIGHT_OFF;

	double tempImagex, tempImagey;
	Calculate_image_xy(_Latitude, _Longitude, _Height, tempImagex, tempImagey);
	MatrixInter_L[0][0] = _imagey - tempImagey;
	MatrixInter_L[1][0] = _imagex - tempImagex;

	double _CoffN1, _CoffN2, _CoffN3, _CoffN4, _CoffN5, _CoffN6;
	Calculate_Correcs(_Latitude, _Longitude, _Height, _CoffN1, _CoffN2,
		_CoffN3, _CoffN4, _CoffN5, _CoffN6);
	MatrixInter_A[0][0] = _CoffN1;
	MatrixInter_A[0][1] = _CoffN2;
	MatrixInter_A[1][0] = _CoffN4;
	MatrixInter_A[1][1] = _CoffN5;
	for (int i1 = 0; i1<2; i1++)
		for (int j1 = 0; j1<2; j1++)
			MatrixInter_AT[j1][i1] = MatrixInter_A[i1][j1];
	Cal_MatrixMul(MatrixInter_ATA, MatrixInter_AT, MatrixInter_A, 2, 2, 2);
	Cal_MatrixMul(MatrixInter_ATL, MatrixInter_AT, MatrixInter_L, 2, 2, 1);
	inverse(MatrixInter_ATA, 2);
	Cal_MatrixMul(MatrixInter_X, MatrixInter_ATA, MatrixInter_ATL, 2, 2, 1);
	_Latitude += MatrixInter_X[0][0];
	_Longitude += MatrixInter_X[1][0];
	double t_X1 = MatrixInter_X[0][0];
	double t_X2 = MatrixInter_X[1][0];
	if (fabs(t_X1)>0.00000001 || fabs(t_X2)>0.00000001)
	{
		Calculate_Ground_XYZ(_imagex, _imagey, _Latitude, _Longitude, _Height);
	}
}

void Class_RPCs::Cal_MatrixMul(double** result_matrix, double** Matrix1, double** matrix2, int n1, int n2, int n3)
{
	for (int m = 0; m<n1; m++)
		for (int n = 0; n<n3; n++)
			result_matrix[m][n] = 0.0;
	for (int r = 0; r<n1; r++)
		for (int s = 0; s<n3; s++)
			for (int p = 0; p<n2; p++)
				result_matrix[r][s] = result_matrix[r][s] + Matrix1[r][p] * matrix2[p][s];
}

void Class_RPCs::inverse(double** m_InMatrix, int m_number)
{
	double** m_InA;
	m_InA = new double *[m_number];
	for (int m_i = 0; m_i<m_number; m_i++)
		m_InA[m_i] = new double[m_number];
	for (int m_ii = 0; m_ii<m_number; m_ii++)
		for (int m_jj = 0; m_jj<m_number; m_jj++)
		{
			if (m_ii == m_jj)
				m_InA[m_ii][m_jj] = 1.0;
			else
				m_InA[m_ii][m_jj] = 0.0;
		}
	for (int m = 1; m<m_number; m++)
	{
		for (int i = m; i<m_number; i++)
		{
			double n = -m_InMatrix[i][m - 1] / m_InMatrix[m - 1][m - 1];
			for (int j = 0; j<m_number; j++)
			{
				m_InMatrix[i][j] = m_InMatrix[m - 1][j] * n + m_InMatrix[i][j];
				m_InA[i][j] = m_InA[m - 1][j] * n + m_InA[i][j];
			}
		}
	}
	for (int mm = m_number - 2; mm >= 0; mm--)
	{
		for (int ii = mm; ii >= 0; ii--)
		{
			double nn = -m_InMatrix[ii][mm + 1] / m_InMatrix[mm + 1][mm + 1];
			for (int jj = 0; jj<m_number; jj++)
			{
				m_InMatrix[ii][jj] = m_InMatrix[mm + 1][jj] * nn + m_InMatrix[ii][jj];
				m_InA[ii][jj] = m_InA[mm + 1][jj] * nn + m_InA[ii][jj];
			}
		}
	}
	for (int p = 0; p<m_number; p++)
		for (int q = 0; q<m_number; q++)
			m_InA[p][q] = m_InA[p][q] / m_InMatrix[p][p];
	for (int m_p = 0; m_p<m_number; m_p++)
		for (int m_q = 0; m_q<m_number; m_q++)
			m_InMatrix[m_p][m_q] = m_InA[m_p][m_q];
	for (int m_k = 0; m_k<m_number; m_k++)
		delete[]m_InA[m_k];
	delete[]m_InA;
}


