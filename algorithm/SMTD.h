#pragma once
#include <vector>

int calculateSMTDs(double L, double D, double l, std::vector<double>& incsv, double& SMTD);

void calculateSMTD(int n, std::vector<int>& incsv, int j, int* x, double* smtd);

void calculateSMTD_mean(int J, double* smtd, int Jv);

int main_smdt();
