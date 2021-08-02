/*
高程数据以m为单位，第一个数据与第二个数据相差11m，程序运行需要几秒钟
% Summary of this function goes here
% 此M文件是计算国际平整度指数的
% IRI_result = IRI(y, dx, v)
% 输入变量意义如下：
% dx  为路面高程采样间隔（m），默认为0.25m
% y   为测量路段高程数据（m）
% v   1 / 4车辆行驶速度（km / h），默认为80km / h
% IRI_result为输出变量，为包含两个元素的数组，意义如下：
% IRI_result(1)  为测量路段长度（m）
% IRI_result(2)  为国际平整度指数（m / km)


% IRI_XISHU Summary of this function goes here
% Detailed explanation goes here
% 输出量：
% st     4×4矩阵
% pr     4×1矩阵

% 输入量：
% dx     采样间隔（m）
% v      1 / 4车辆行驶速度（km / h）
*/
#include "StdAfx.h"
#include <iostream>
//#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>
#include <vector>

using namespace std;


int IRI_xishu(double dx, double v, Eigen::MatrixXd *pr, Eigen::MatrixXd *st)
{
	int		 k1 = 653;	// 车辆参数，标准车辆取653，见“说明.DOC”文件的第一条
	double   k2 = 63.3;	// 车辆参数，标准车辆取63.3，见“说明.DOC”文件的第一条
	double   u = 0.15;	// 车辆参数，标准车辆取0.15，见“说明.DOC”文件的第一条
	double	 c = 6.00;	// 车辆参数，标准车辆取6.00，见“说明.DOC”文件的第一条

	// 由车辆参数组成的4×4矩阵，见“说明.DOC”文件的第二条
	//Eigen::Matrix<double, 4, 4> A;
	Eigen::MatrixXd A(4, 4);
	A << 0, 1, 0, 0,
		-k2, -c, k2, c,
		0, 0, 0, 1,
		k2 / u, c / u, -1 * (k1 + k2) / u, -c / u;

	//std::cout << A << std::endl;

	//由车辆参数组成的4×1矩阵，见“说明.DOC”文件的第二条
	Eigen::Matrix<double, 4, 1> B;
	B << 0,0, 0, k1 / u;
	//std::cout << B << std::endl;


	//单位矩阵，见“说明.DOC”文件的第二条
	Eigen::Matrix<double, 4, 4> I;
	I << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	//计算路面高程的时间采样间隔dt（s），即车速为v时，车辆行驶dx距离所需的时间，单位为秒
	double dt = 3.6 * dx / v; 
	
	//Eigen::MatrixXd st(4, 4);
	*st = (A * dt).exp();
	


	Eigen::MatrixXd AA(4, 4);
	AA  = A.inverse();

	Eigen::MatrixXd SSTT(4, 4);
	SSTT = *st - I;


	//Eigen::MatrixXd pr(4, 1);
	*pr = AA * SSTT * B;
	



	return 0;
}


int readFromCsv(vector<double>* incsv)
{

	double intp;
	FILE* fp;
	
	//你自己的文件路径
	if (fopen_s(&fp, "./algorithm/yshuju.csv", "r") != 0)
	{
		printf("无法打开文件。\n");
		return 1;
	}

	while (1) {		
		fscanf_s(fp, "%lf", &intp);	
		if (feof(fp))break;


		incsv->push_back(intp);		
		//cout << intp << endl;
	}
	fclose(fp);
	
	//for (int i = 0; i < incsv->size(); i++)
	//{
	//    cout << i << " " << incsv->at(i) << endl;

	//}//输出显示每行的数据
	return 0;
}




int iri(vector<double>& incsv, double dx, double v, double * IRI_length, double* IRI_result )
{													 
	Eigen::MatrixXd pr(4, 1);
	Eigen::MatrixXd st(4, 4);
	IRI_xishu(dx, v, &pr, &st);

	//std::cout << st << endl;
	//std::cout << pr << endl;

	int k = int(0.25 / dx + 0.5) + 1; // k为中间参数，本行中的“round()”函数功能为：对一个数四舍五入求整
	if( k < 2)// % 如果k小于2，k设为2
		k = 2;
	
	double bl = (k - 1) * dx;

	double z1[4];

	//  从测量路段起点开始，离11m处最近的那个高程数据的序号（起点高程数据序号为1）
	z1[0] = (incsv[1] - incsv[0]) / 11; // z1是一个一维数组，包含四个元素：z1(1), z1(2), z1(3), z1(4)；
	z1[1] = 0;
	z1[2] = z1[0];
	z1[3] = 0;
	double rs = 0;

	long kk = incsv.size() - k + 1;	// y为测量路段高程数据（m），函数“incsv.size()”是计算测量路段总的高程数据点数

	for (int i=0; i<kk; i++) 
	{
		int ixx = i;
		double *yy =  new double[k];
		for (int ix = 0; ix < k; ix++)
		{
			yy[ix] = incsv[ixx];
		
			ixx = ixx + 1;
		}

		double yp = 0.0;
		if (dx == 0.1)
			yp = ((yy[k - 1] + yy[k - 2]) / 2 - yy[0]) / 0.25;
		else
			yp = (yy[k - 1] - yy[0]) / bl;
		

		delete[] yy;
		
		double z[4];
		for (int j = 0; j < 4; j++) 
		{
			z[j] = pr(j,0) * yp;
			for (int jj = 0; jj < 4; jj++) 
			{
				z[j] = z[j] + st(j, jj) * z1[jj];
			}
		}

		for (int j = 0; j < 4; j++)
		{
			z1[j] = z[j];
		}

		rs = rs + abs(z[0] - z[2]); //% 函数“abs()”是求绝对值

	}

	*IRI_length = (incsv.size() - 2) * dx;
	*IRI_result = rs / kk * 1000;

	return 0;
}

int main_iri()
{
	double dx = 0.25;// input('请输入采样间隔dx:');
	double v = 80;// input('请输入1/4车辆行驶速度v:');
	
	vector<double> incsv;
	readFromCsv(&incsv);

	double IRI_length = 0;
	double IRI_result = 0;
	iri(incsv, dx, v, &IRI_length, &IRI_result);

	
	std::cout << "IRI_length:" << IRI_length << " IRI_result:" << IRI_result << endl;
	return 0;
}




