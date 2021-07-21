#include "StdAfx.h"
#include "POINT3.h"
#include "math.h"

POINT3::POINT3(void)
{
	
}
POINT3::POINT3(float a, float b, float c)
{
	x = a;
	y = b;
	z = c;
}

POINT3::~POINT3(void)
{
}
void POINT3::normal()
{
	float d = 0.0;

	d += x*x;
	d += y*y;
	d += z*z;

	d = sqrt(d);
	if (d>0.0)
	{
		x = (x * 1.0) / d;
		y = (y * 1.0) / d;
		z = (z * 1.0) / d;

	}

}