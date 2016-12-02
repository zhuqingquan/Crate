#include "YUVTexture_NV12.h"
#include <fstream>
#include "d3dUtil.h"

using namespace std;
using namespace SOA::Render;

const float factorToFrameBufLen_nv12 = 1.5;

#define FrameSize_NV12(x, y) ((x)*(y)*(factorToFrameBufLen_nv12))

YUVTexture_NV12::YUVTexture_NV12(YUVFormat_Planar yuvFmt)
	: m_device(NULL)
	, m_yTex(NULL), m_uvTex(NULL)
	, m_ySRV(NULL), m_uvSRV(NULL)
	, m_yuvFmt(yuvFmt), m_width(0), m_height(0)
{
}

YUVTexture_NV12::~YUVTexture_NV12()
{
	destroy();
}

int YUVTexture_NV12::create(ID3D11Device* device, int width, int height, const char* initData, int dataLen)
{
	if(device==NULL || width<=0 || height<=0)
		return -1;
	int frameSize = FrameSize_NV12(width, height);
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
	texDesc.Format = DXGI_FORMAT_R8G8_UNORM;
	ZeroMemory(&initSubData, sizeof(D3D11_SUBRESOURCE_DATA));
	if(initData && dataLen>=frameSize)
	{
		initSubData.pSysMem = initData + width * height;
		initSubData.SysMemPitch = width;
	}
	if(FAILED(device->CreateTexture2D(&texDesc, &initSubData, &m_uvTex)))
		goto ErrorEnd;

	D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
	ZeroMemory(&srvDesc,sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	if(FAILED(device->CreateShaderResourceView(m_yTex, &srvDesc, &m_ySRV)))
		goto ErrorEnd;

	srvDesc.Format = DXGI_FORMAT_R8G8_UNORM;
	if(FAILED(device->CreateShaderResourceView(m_uvTex, &srvDesc, &m_uvSRV)))
		goto ErrorEnd;

	m_device = device;
	m_width = width;
	m_height = height;
	return 0;
ErrorEnd:
	destroy();
	return -3;
}

int YUVTexture_NV12::destroy()
{
	SAFE_RELEASE(m_ySRV);
	SAFE_RELEASE(m_uvSRV);
	SAFE_RELEASE(m_yTex);
	SAFE_RELEASE(m_uvTex);
	m_device = NULL;
	m_width = 0;
	m_height = 0;
	m_yuvFmt = YUVFORMAT_PLANAR_Unknow;
	return 0;
}

int YUVTexture_NV12::update(const char* dataBuf, int dataBufLen, int offset)
{
	return 0;
}

int YUVTexture_NV12::getTexture(ID3D11Texture2D** outYUVTexs, int& texsCount) const
{
	if(texsCount<2 || outYUVTexs==NULL)
		return -1;
	if(m_yTex==NULL || NULL==m_uvTex || m_ySRV==NULL || m_uvSRV==NULL)
		return -2;

	*(outYUVTexs+0) = m_yTex;
	*(outYUVTexs+1) = m_uvTex;
	texsCount = 2;

	return 0;
}

int YUVTexture_NV12::getShaderResourceView(ID3D11ShaderResourceView** outYUVSRVs, int& srvsCount) const
{
	if(srvsCount<2 || outYUVSRVs==NULL)
		return -1;
	if(m_yTex==NULL || NULL==m_uvTex || m_ySRV==NULL || m_uvSRV==NULL)
		return -2;

	*(outYUVSRVs+0) = m_ySRV;
	*(outYUVSRVs+1) = m_uvSRV;
	srvsCount = 2;

	return 0;
}
