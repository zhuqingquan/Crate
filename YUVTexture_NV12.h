#pragma once
#ifndef _SOA_RENDER_YUVTEXTURE_NV12_H_
#define _SOA_RENDER_YUVTEXTURE_NV12_H_

#include "YUVTexture_Planar.h"
namespace SOA
{
namespace Render
{
	class YUVTexture_NV12
	{
	public:
		YUVTexture_NV12(YUVFormat_Planar yuvFmt);
		~YUVTexture_NV12();

		int create(ID3D11Device* device, int width, int height,
					const char* initData, int dataLen);

		int destroy();

		int update(const char* dataBuf, int dataBufLen, int offset);

		int getTexture(ID3D11Texture2D** outYUVTexs, int& texsCount) const;
		int getShaderResourceView(ID3D11ShaderResourceView** outYUVSRVs, int& srvsCount) const;
	private:
		YUVTexture_NV12(const YUVTexture_NV12&);
		YUVTexture_NV12& operator=(const YUVTexture_NV12&);

		ID3D11Texture2D* m_yTex;
		ID3D11Texture2D* m_uvTex;
		ID3D11ShaderResourceView*	m_ySRV;
		ID3D11ShaderResourceView*	m_uvSRV;

		ID3D11Device* m_device;
		YUVFormat_Planar m_yuvFmt;
		int m_width;
		int m_height;
	};
}
}

#endif //_SOA_RENDER_YUVTEXTURE_NV12_H_