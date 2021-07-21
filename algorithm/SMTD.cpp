// SMTD.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "StdAfx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include "SMTD.h"

using namespace std;

int readCsv(vector<int> *incsv)
{
    
    int intp;
    FILE* fp;
     
    //你自己的文件路径
    if (fopen_s(&fp, "./algorithm/yzhi.csv", "r") != 0)
    {
        printf("无法打开文件。\n");
        return 1;
    }

    while (1) {
        fscanf_s(fp, "%d", &intp);
        if (feof(fp))break;

        incsv->push_back(intp);        
    }
    fclose(fp);
    //for (int i = 0; i < incsv.size(); i++)
    //{
      //  cout << incsv[i] << endl;

    //}//输出显示每行的数据
    return 0;
}


/*
* 计算一个SMTD
*/
void calculateSMTD(int n, std::vector<int>& incsv, int j, int* x, double* smtd)
{
    int o = 0; int p = 0; int q = 0; int sum = 0;
    for (int i = 0; i < n; i++)
    {
        o = o + incsv[i + n * j];
        p = p + pow((double)incsv[i + n * j], 2);
        q = q + pow((double)x[i], 2) * incsv[i + n * j];
        sum = sum + x[i] * incsv[i + n * j];
    }
    double P = 5 * pow(((pow((double)n, 2) - 1) * o - 12 * q), 2) / (4 * (pow((double)n, 2) - 4));

    *smtd = pow(((n * p - pow((double)o, 2) - (12 * pow((double)sum, 2) + P) / (pow((double)n, 2) - 1)) / pow((double)n, 2)), 0.5);

    printf("第%d段计算长度内构造深度值smtd=%f\n", j, *smtd);
}


void calculateSMTD_mean(int J, double* smtd, int Jv, double* SMTD)
{
    // 计算统计长度L内构造深度SMTD的值
    double sam = 0;
    for (int i = 0; i < J; i++)
        sam = smtd[i] + sam;

    *SMTD = sam / Jv;
    printf("统计长度L内构造深度值SMTD=%f\n", *SMTD);
}

int calculateSMTDs()
{
    int L = 600;
    int D = 300;
    int l = 1;

	//四舍五入
    int m = int((double)D / l + 0.5) + 1;

    if (m < 151)
        printf("纵断面取样数据不足，请修改相关数据。\n"); //判断每段计算长度内的采样个数

    int J = floor((double)(L / D));

    int Jv = J;
    printf("SMTD中含有%d个有效smtd\n", J);
    // 每一个计算长度内的采样个数，按最近奇数取整
    int c = m % 2;
    int n = 0;
    c == 1 ? n = m : n = m - 1;


    int* x = new int[n];
    for (int i = 0; i < n; i++) 
    {
        x[i] = (2 * (i+1) - n - 1) / 2;
        //std::cout << x[i];
    }

    vector<int> incsv;
    readCsv(&incsv);
    /*for (int i = 0; i < incsv.size(); i++)
    {
        cout << incsv[i];
    }*///输出显示每行的数据

    double* smtd = new double[J];
    for (int j=0; j < J; j++) 
    {
        calculateSMTD(n, incsv, j, x, &smtd[j]);
    }

    double SMTD = 0.0;
    calculateSMTD_mean(J, smtd, Jv, &SMTD);


    delete[] x;
    delete[] smtd;
    return 0;
}




int main_smdt()
{
    std::cout << "Start!\n";

    calculateSMTDs();
    std::cout << "Edn!\n";
	return 0;
}








