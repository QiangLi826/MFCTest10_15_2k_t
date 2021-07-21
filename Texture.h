#pragma once
#include <gl\gl.h>
#include "constant.h"
class CTexture
{
public:
	GLubyte TextureImageBlock[TexImageHeight][TexImageHeight];
	GLubyte MipmapImage128[128][128];
	GLubyte MipmapImage64[64][64];
	GLubyte MipmapImage32[32][32];
	GLubyte MipmapImage16[16][16];
	GLubyte MipmapImage8[8][8];
	GLubyte MipmapImage4[4][4];
	GLubyte MipmapImage2[2][2];
	GLubyte MipmapImage1[1][1];

	void MakeMipImages(void);
public:
	CTexture(void);
	~CTexture(void);
};

