#include <D3D11.h>

#pragma once
#ifndef _SOA_RENDER_YUVTEXTURE_PACKED_H_
#define _SOA_RENDER_YUVTEXTURE_PACKED_H_

namespace SOA
{
namespace Render
{
	typedef enum YUVFormat_Packed
	{
		YUVFORMAT_PACKED_Unknow = 0,
		YUVFORMAT_PACKED_YUYV = 4,
		YUVFORMAT_PACKED_YVYU,
		YUVFORMAT_PACKED_UYVY,
		YUVFORMAT_PACKED_VYUY,
	};

	class YUVTexture_Packed
	{
	public:
		YUVTexture_Packed(YUVFormat_Packed yuvFmt);
		~YUVTexture_Packed();

		int create(ID3D11Device* device, int width, int height,
					const char* initData, int dataLen);

		int destroy();

		int update(const char* dataBuf, int dataBufLen, int offset);

		int getTexture(ID3D11Texture2D** outYUVTexs, int& texsCount) const;
		int getShaderResourceView(ID3D11ShaderResourceView** outYUVSRVs, int& srvsCount) const;

		int getWidth()  const { return m_width; }
		int getHeight() const { return m_height; }
	private:
		YUVTexture_Packed(const YUVTexture_Packed&);
		YUVTexture_Packed& operator=(const YUVTexture_Packed&);

		ID3D11Texture2D* m_yuvTex;
		//ID3D11Texture2D* m_posTex;
		ID3D11ShaderResourceView*	m_yuvSRV;
		//ID3D11ShaderResourceView*	m_posSRV;

		ID3D11Device* m_device;
		YUVFormat_Packed m_yuvFmt;
		int m_width;
		int m_height;
	};
}
}

#endif ///_SOA_RENDER_YUVTEXTURE_PACKED_H_