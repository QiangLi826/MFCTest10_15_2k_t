#include "StdAfx.h"
#include "Texture.h"


CTexture::CTexture(void)
{
	int i,j;

	for (i=0;i<TexImageHeight;i++)
	{
		for (j=0;j<TexImageHeight;j++)
		{
				TextureImageBlock[i][j]=125;
		}	

	}
}


CTexture::~CTexture(void)
{
}
void CTexture::MakeMipImages(void)
{
	int size;
	int i, j;
    
	size = 128;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{
			
//			MipmapImage128[i][j]=size;
			MipmapImage128[i][j]=(TextureImageBlock[2*i][2*j] + TextureImageBlock[2*i][2*j+1] 
			                    + TextureImageBlock[2*i+1][2*j] + TextureImageBlock[2*i+1][2*j+1])/4;

		}
	}
	size = 64;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

//			MipmapImage64[i][j]=size;
			MipmapImage64[i][j]=(MipmapImage128[2*i][2*j] + MipmapImage128[2*i][2*j+1] 
			                   + MipmapImage128[2*i+1][2*j] + MipmapImage128[2*i+1][2*j+1])/4;

		}
	}
	size = 32;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

//			MipmapImage32[i][j]=size;
			MipmapImage32[i][j]=(MipmapImage64[2*i][2*j] + MipmapImage64[2*i][2*j+1] 
			                   + MipmapImage64[2*i+1][2*j] + MipmapImage64[2*i+1][2*j+1])/4;

		}
	}
	size = 16;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

//			MipmapImage16[i][j]=size;
			MipmapImage16[i][j]=(MipmapImage32[2*i][2*j] + MipmapImage32[2*i][2*j+1] 
			                   + MipmapImage32[2*i+1][2*j] + MipmapImage32[2*i+1][2*j+1])/4;

		}
	}
	size = 8;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

//			MipmapImage8[i][j]=size;
			MipmapImage8[i][j]=(MipmapImage16[2*i][2*j] + MipmapImage16[2*i][2*j+1] 
			                   + MipmapImage16[2*i+1][2*j] + MipmapImage16[2*i+1][2*j+1])/4;

		}
	}	
	size = 4;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

//			MipmapImage4[i][j]=size;
			MipmapImage4[i][j]=(MipmapImage8[2*i][2*j] + MipmapImage8[2*i][2*j+1] 
			                  + MipmapImage8[2*i+1][2*j] + MipmapImage8[2*i+1][2*j+1])/4;

		}
	}
	size = 2;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

//			MipmapImage2[i][j]=size;
			MipmapImage2[i][j]=(MipmapImage4[2*i][2*j] + MipmapImage4[2*i][2*j+1] 
			+ MipmapImage4[2*i+1][2*j] + MipmapImage4[2*i+1][2*j+1])/4;

		}
	}	
			MipmapImage1[0][0]=(MipmapImage2[0][0] + MipmapImage2[0][1]
			                  + MipmapImage2[1][0] + MipmapImage2[1][1])/4;

}