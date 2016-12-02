#include "YUVTexture_Packed.h"
#include "d3dUtil.h"

using namespace SOA::Render;

const float factorToFrameBufLen_yuvPacked = 2;

#define FrameSize_YUVPacked(x, y) ((x)*(y)*(factorToFrameBufLen_yuvPacked))

YUVTexture_Packed::YUVTexture_Packed(YUVFormat_Packed yuvFmt)
	: m_device(NULL), m_yuvFmt(yuvFmt)
	, m_height(0), m_width(0)
	, m_yuvTex(NULL), m_yuvSRV(NULL)
//	, m_posTex(NULL), m_posSRV(NULL)
{

}

YUVTexture_Packed::~YUVTexture_Packed()
{
	destroy();
}


int YUVTexture_Packed::create(ID3D11Device* device, int width, int height,
								const char* initData, int dataLen)
{
	if(device==NULL || width<=0 || height<=0)
		return -1;
	int frameSize = FrameSize_YUVPacked(width, height);
	if(frameSize<=0)
		return -2;
	char* posBuf = NULL;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = texDesc.MipLevels = 1;
	texDesc.Height = height;
	texDesc.Width = width;
	texDesc.Format = DXGI_FORMAT_R8G8_UNORM;
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
		initSubData.SysMemPitch = width * factorToFrameBufLen_yuvPacked;
	}

	if(FAILED(device->CreateTexture2D(&texDesc, &initSubData, &m_yuvTex)))
		goto ErrorEnd;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
	ZeroMemory(&srvDesc,sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	if(FAILED(device->CreateShaderResourceView(m_yuvTex, &srvDesc, &m_yuvSRV)))
		goto ErrorEnd;

	//texDesc.Height = height;
	//texDesc.Width = width / 2;
	//texDesc.Format = DXGI_FORMAT_R8_UNORM;
	//texDesc.Usage = D3D11_USAGE_DEFAULT;
	//texDesc.CPUAccessFlags = 0;
	//posBuf = (char*)malloc(texDesc.Height * texDesc.Width);
	//memset(posBuf, 0, texDesc.Height * texDesc.Width);
	//for(int i=0; i<texDesc.Height; i++)
	//	for(int j=1; j<texDesc.Width; j+=2)
	//		*(posBuf+i*texDesc.Width+j) = 255;
	//ZeroMemory(&initSubData, sizeof(D3D11_SUBRESOURCE_DATA));
	//initSubData.pSysMem = initData;
	//initSubData.SysMemPitch = width / 2;
	//if(FAILED(device->CreateTexture2D(&texDesc, &initSubData, &m_posTex)))
	//{
	//	free(posBuf);
	//	goto ErrorEnd;
	//}

	//srvDesc.Format = texDesc.Format;
	//if(FAILED(device->CreateShaderResourceView(m_posTex, &srvDesc, &m_posSRV)))
	//	goto ErrorEnd;

	free(posBuf);
	m_device = device;
	m_width = width;
	m_height = height;
	return 0;
ErrorEnd:
	destroy();
	return -3;
}

int YUVTexture_Packed::destroy()
{
//	SAFE_RELEASE(m_posTex);
//	SAFE_RELEASE(m_posSRV);
	SAFE_RELEASE(m_yuvSRV);
	SAFE_RELEASE(m_yuvTex);
	m_device = NULL;
	m_width = 0;
	m_height = 0;
	return 0;
}

int YUVTexture_Packed::update(const char* dataBuf, int dataBufLen, int offset)
{
	return -1;
}

int YUVTexture_Packed::getTexture(ID3D11Texture2D** outYUVTexs, int& texsCount) const
{
	if(texsCount<1 || outYUVTexs==NULL)
		return -1;
	if(m_yuvTex==NULL)
		return -2;

	*(outYUVTexs) = m_yuvTex;
//	*(outYUVTexs+1) = m_posTex;
	texsCount = 1;

	return 0;
}

int YUVTexture_Packed::getShaderResourceView(ID3D11ShaderResourceView** outYUVSRVs, int& srvsCount) const
{
	if(srvsCount<1 || outYUVSRVs==NULL)
		return -1;
	if(m_yuvSRV==NULL)
		return -2;

	*(outYUVSRVs+0) = m_yuvSRV;
//	*(outYUVSRVs+1) = m_posSRV;
	srvsCount = 1;

	return 0;
}