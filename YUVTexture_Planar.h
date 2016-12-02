#include <D3D11.h>

#pragma once
#ifndef _SOA_RENDER_YUVTEXTURE_H_
#define _SOA_RENDER_YUVTEXTURE_H_

namespace SOA
{
namespace Render
{
	typedef enum YUVFormat_Planar
	{
		YUVFORMAT_PLANAR_Unknow = 0,
		/*YUVFORMAT_PLANAR_YUYV,
		YUVFORMAT_PLANAR_YVYU,
		YUVFORMAT_PLANAR_UYVY,
		YUVFORMAT_PLANAR_VYUY,*/
		YUVFORMAT_PLANAR_YV12,
		YUVFORMAT_PLANAR_I420,
		YUVFORMAT_PLANAR_NV12,
	};

	class YUVTexture_Planar
	{
	public:
		YUVTexture_Planar(YUVFormat_Planar yuvFmt);
		~YUVTexture_Planar();

		int create(ID3D11Device* device, int width, int height,
					const char* initData, int dataLen);

		int destroy();

		int update(const char* dataBuf, int dataBufLen, int offset);

		int getTexture(ID3D11Texture2D** outYUVTexs, int& texsCount) const;
		int getShaderResourceView(ID3D11ShaderResourceView** outYUVSRVs, int& srvsCount) const;
	private:
		YUVTexture_Planar(const YUVTexture_Planar&);
		YUVTexture_Planar& operator=(const YUVTexture_Planar&);

		ID3D11Texture2D* m_yTex;
		ID3D11Texture2D* m_uTex;
		ID3D11Texture2D* m_vTex;
		ID3D11ShaderResourceView*	m_ySRV;
		ID3D11ShaderResourceView*	m_uSRV;
		ID3D11ShaderResourceView*	m_vSRV;

		ID3D11Device* m_device;
		YUVFormat_Planar m_yuvFmt;
		int m_width;
		int m_height;
	};
}
}

#endif //_SOA_RENDER_YUVTEXTURE_H_