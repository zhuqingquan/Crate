#include "YUVTexture_Planar.h"
#include <fstream>
#include "d3dUtil.h"

using namespace std;
using namespace SOA::Render;

const float factorToFrameBufLen_yuvPlanar = 1.5;

#define FrameSize_YUVPlanar(x, y) ((x)*(y)*(factorToFrameBufLen_yuvPlanar))

YUVTexture_Planar::YUVTexture_Planar(YUVFormat_Planar yuvFmt)
	: m_device(NULL)
	, m_yTex(NULL), m_uTex(NULL), m_vTex(NULL)
	, m_ySRV(NULL), m_uSRV(NULL), m_vSRV(NULL)
	, m_yuvFmt(yuvFmt), m_width(0), m_height(0)
{
}

YUVTexture_Planar::~YUVTexture_Planar()
{
	destroy();
}

int YUVTexture_Planar::create(ID3D11Device* device, int width, int height, const char* initData, int dataLen)
{
	if(device==NULL || width<=0 || height<=0)
		return -1;
	int frameSize = FrameSize_YUVPlanar(width, height);
	if(frameSize<=0)
		return -2;
	
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = texDesc.MipLevels = 1;
	texDesc.Height = height;
	texDesc.Width = width;
	texDesc.Format = DXGI_FORMAT_R8_UNORM;
	texDesc.SampleDesc.Quality = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA initSubData;
	ZeroMemory(&initSubData, sizeof(D3D11_SUBRESOURCE_DATA));
	if(initData && dataLen>=frameSize)
	{
		initSubData.pSysMem = initData;
		initSubData.SysMemPitch = width;
	}
	if(FAILED(device->CreateTexture2D(&texDesc, &initSubData, &m_yTex)))
		goto ErrorEnd;

	texDesc.Height = height / 2;
	texDesc.Width = width / 2;
	ZeroMemory(&initSubData, sizeof(D3D11_SUBRESOURCE_DATA));
	if(initData && dataLen>=frameSize)
	{
		initSubData.pSysMem = initData + width * height;
		initSubData.SysMemPitch = width / 2;
	}
	if(FAILED(device->CreateTexture2D(&texDesc, &initSubData, &m_uTex)))
		goto ErrorEnd;

	ZeroMemory(&initSubData, sizeof(D3D11_SUBRESOURCE_DATA));
	if(initData && dataLen>=frameSize)
	{
		initSubData.pSysMem = initData + width * height * 5 / 4;
		initSubData.SysMemPitch = width / 2;
	}
	if(FAILED(device->CreateTexture2D(&texDesc, &initSubData, &m_vTex)))
		goto ErrorEnd;

	D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
	ZeroMemory(&srvDesc,sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	if(FAILED(device->CreateShaderResourceView(m_yTex, &srvDesc, &m_ySRV)))
		goto ErrorEnd;

	if(FAILED(device->CreateShaderResourceView(m_uTex, &srvDesc, &m_uSRV)))
		goto ErrorEnd;

	if(FAILED(device->CreateShaderResourceView(m_vTex, &srvDesc, &m_vSRV)))
		goto ErrorEnd;

	m_device = device;
	m_width = width;
	m_height = height;
	return 0;
ErrorEnd:
	destroy();
	return -3;
}

int YUVTexture_Planar::destroy()
{
	SAFE_RELEASE(m_ySRV);
	SAFE_RELEASE(m_ySRV);
	SAFE_RELEASE(m_ySRV);
	SAFE_RELEASE(m_yTex);
	SAFE_RELEASE(m_yTex);
	SAFE_RELEASE(m_yTex);
	m_device = NULL;
	m_width = 0;
	m_height = 0;
	m_yuvFmt = YUVFORMAT_PLANAR_Unknow;
	return 0;
}

int YUVTexture_Planar::update(const char* dataBuf, int dataBufLen, int offset)
{
	return 0;
}

int YUVTexture_Planar::getTexture(ID3D11Texture2D** outYUVTexs, int& texsCount) const
{
	if(texsCount<3 || outYUVTexs==NULL)
		return -1;
	if(m_yTex==NULL)
		return -2;

	*(outYUVTexs+0) = m_yTex;
	*(outYUVTexs+1) = m_uTex;
	*(outYUVTexs+2) = m_vTex;
	texsCount = 3;

	return 0;
}

int YUVTexture_Planar::getShaderResourceView(ID3D11ShaderResourceView** outYUVSRVs, int& srvsCount) const
{
	if(srvsCount<3 || outYUVSRVs==NULL)
		return -1;
	if(m_ySRV==NULL)
		return -2;

	*(outYUVSRVs+0) = m_ySRV;
	*(outYUVSRVs+1) = m_uSRV;
	*(outYUVSRVs+2) = m_vSRV;
	srvsCount = 3;

	return 0;
}
