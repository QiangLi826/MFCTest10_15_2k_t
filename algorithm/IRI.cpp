/*
�߳�������mΪ��λ����һ��������ڶ����������11m������������Ҫ������
% Summary of this function goes here
% ��M�ļ��Ǽ������ƽ����ָ����
% IRI_result = IRI(y, dx, v)
% ��������������£�
% dx  Ϊ·��̲߳��������m����Ĭ��Ϊ0.25m
% y   Ϊ����·�θ߳����ݣ�m��
% v   1 / 4������ʻ�ٶȣ�km / h����Ĭ��Ϊ80km / h
% IRI_resultΪ���������Ϊ��������Ԫ�ص����飬�������£�
% IRI_result(1)  Ϊ����·�γ��ȣ�m��
% IRI_result(2)  Ϊ����ƽ����ָ����m / km)


% IRI_XISHU Summary of this function goes here
% Detailed explanation goes here
% �������
% st     4��4����
% pr     4��1����

% ��������
% dx     ���������m��
% v      1 / 4������ʻ�ٶȣ�km / h��
*/
#include "StdAfx.h"
#include <iostream>
//#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>
#include <vector>

using namespace std;


int IRI_xishu(double dx, double v, Eigen::MatrixXd *pr, Eigen::MatrixXd *st)
{
	int		 k1 = 653;	// ������������׼����ȡ653������˵��.DOC���ļ��ĵ�һ��
	double   k2 = 63.3;	// ������������׼����ȡ63.3������˵��.DOC���ļ��ĵ�һ��
	double   u = 0.15;	// ������������׼����ȡ0.15������˵��.DOC���ļ��ĵ�һ��
	double	 c = 6.00;	// ������������׼����ȡ6.00������˵��.DOC���ļ��ĵ�һ��

	// �ɳ���������ɵ�4��4���󣬼���˵��.DOC���ļ��ĵڶ���
	//Eigen::Matrix<double, 4, 4> A;
	Eigen::MatrixXd A(4, 4);
	A << 0, 1, 0, 0,
		-k2, -c, k2, c,
		0, 0, 0, 1,
		k2 / u, c / u, -1 * (k1 + k2) / u, -c / u;

	//std::cout << A << std::endl;

	//�ɳ���������ɵ�4��1���󣬼���˵��.DOC���ļ��ĵڶ���
	Eigen::Matrix<double, 4, 1> B;
	B << 0,0, 0, k1 / u;
	//std::cout << B << std::endl;


	//��λ���󣬼���˵��.DOC���ļ��ĵڶ���
	Eigen::Matrix<double, 4, 4> I;
	I << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	//����·��̵߳�ʱ��������dt��s����������Ϊvʱ��������ʻdx���������ʱ�䣬��λΪ��
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
	
	//���Լ����ļ�·��
	if (fopen_s(&fp, "./algorithm/yshuju.csv", "r") != 0)
	{
		printf("�޷����ļ���\n");
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

	//}//�����ʾÿ�е�����
	return 0;
}




int iri(vector<double>& incsv, double dx, double v, double * IRI_length, double* IRI_result )
{													 
	Eigen::MatrixXd pr(4, 1);
	Eigen::MatrixXd st(4, 4);
	IRI_xishu(dx, v, &pr, &st);

	//std::cout << st << endl;
	//std::cout << pr << endl;

	int k = int(0.25 / dx + 0.5) + 1; // kΪ�м�����������еġ�round()����������Ϊ����һ����������������
	if( k < 2)// % ���kС��2��k��Ϊ2
		k = 2;
	
	double bl = (k - 1) * dx;

	double z1[4];

	//  �Ӳ���·����㿪ʼ����11m��������Ǹ��߳����ݵ���ţ����߳��������Ϊ1��
	z1[0] = (incsv[1] - incsv[0]) / 11; // z1��һ��һά���飬�����ĸ�Ԫ�أ�z1(1), z1(2), z1(3), z1(4)��
	z1[1] = 0;
	z1[2] = z1[0];
	z1[3] = 0;
	double rs = 0;

	long kk = incsv.size() - k + 1;	// yΪ����·�θ߳����ݣ�m����������incsv.size()���Ǽ������·���ܵĸ߳����ݵ���

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

		rs = rs + abs(z[0] - z[2]); //% ������abs()���������ֵ

	}

	*IRI_length = (incsv.size() - 2) * dx;
	*IRI_result = rs / kk * 1000;

	return 0;
}

int main_iri()
{
	double dx = 0.25;// input('������������dx:');
	double v = 80;// input('������1/4������ʻ�ٶ�v:');
	
	vector<double> incsv;
	readFromCsv(&incsv);

	double IRI_length = 0;
	double IRI_result = 0;
	iri(incsv, dx, v, &IRI_length, &IRI_result);

	
	std::cout << "IRI_length:" << IRI_length << " IRI_result:" << IRI_result << endl;
	return 0;
}




