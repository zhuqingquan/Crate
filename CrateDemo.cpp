//***************************************************************************************
// CrateDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates texturing a box.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "ImageSourceHelper.h"
#include "d3dAdapterOutputEnumerator.h"
#include "YUVTexture_Planar.h"
#include "YUVTexture_Packed.h"
#include "YUVTexture_NV12.h"

class CrateApp : public D3DApp
{
public:
	CrateApp(HINSTANCE hInstance);
	CrateApp(HINSTANCE hInstance, int posX, int posY, int width, int height);
	~CrateApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3D11ShaderResourceView* mDiffuseMapSRV;
	ID3D11Texture2D* mTexturesRes;
	HANDLE mTexturesResSharedHandle;
	//SOA::Render::YUVTexture_Planar* m_yuvTexture;  //display yuv420
	//SOA::Render::YUVTexture_Packed* m_yuvTexture;    //display yuy2
	SOA::Render::YUVTexture_NV12* m_yuvTexture;    //display yuy2
	//zTools::ImageSourceHelper mImgResources;
	int mCurFrameIndex;

	DirectionalLight mDirLights[3];
	Material mBoxMat;

	XMFLOAT4X4 mTexTransform;
	XMFLOAT4X4 mBoxWorld;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	BasicEffect* mEffect;
	ID3D11InputLayout* mInputLayout;
};

static std::auto_ptr<zTools::ImageSourceHelper> gImgSrcHelper;
zTools::ImageSourceHelper* zTools::ImageSourceHelper::Instance()
{
	if(gImgSrcHelper.get()==NULL)
	{
		gImgSrcHelper = std::auto_ptr<zTools::ImageSourceHelper>(new zTools::ImageSourceHelper());
		gImgSrcHelper->init("D:\\代码黑洞\\datasource\\new\\1920X1080.rgb", 1920, 1080);
	}
	return gImgSrcHelper.get();
}

struct CreateCrateAppThreadParam
{
	CreateCrateAppThreadParam(HINSTANCE hIst, int iPosX, int iPosY, int iWidth, int iHeight)
		: hInstance(hIst), posX(iPosX), posY(iPosY), width(iWidth), height(iHeight)
	{
	}

	HINSTANCE hInstance;
	int posX;
	int posY;
	int width;
	int height;
};

DWORD WINAPI createCrateAppThreadWork(LPVOID pParam)
{
	CreateCrateAppThreadParam* param = (CreateCrateAppThreadParam*)pParam;
	if(param==NULL)
		return -1;
	CrateApp theApp(param->hInstance, param->posX, param->posY, param->width, param->height);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
//				   PSTR cmdLine, int showCmd)
int main(int argc, char** argv)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	
	if(zTools::ImageSourceHelper::Instance()->getImgWidth()<=0)
	{
		MessageBox(NULL, L"Faile to open argb file.\n", 0, 0);
		return false;
	}

	/*std::vector<IDXGIAdapter*> adapters;
	HR(DXGI_getAdapters(adapters));
	for(int i=0; i<adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdptDesc;
		IDXGIAdapter1* adpt1 = static_cast<IDXGIAdapter1*>(adapters[i]);
		assert(adpt1);
		adpt1->GetDesc1(&dxgiAdptDesc);
		std::vector<IDXGIOutput*> outputs;
		HR(DXGI_getOutputs(adapters[i], outputs));
		for(int outIndex=0; outIndex<outputs.size(); )
		{
			DXGI_OUTPUT_DESC outputDesc;
			outputs[outIndex]->GetDesc(&outputDesc);
			outIndex++;
		}
	}*/

	std::vector<CreateCrateAppThreadParam> windowRects;
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	windowRects.push_back(CreateCrateAppThreadParam(hInstance, 0, 0, 1024, 576));
	//windowRects.push_back(CreateCrateAppThreadParam(hInstance, 630, 0, 600, 300));
	//windowRects.push_back(CreateCrateAppThreadParam(hInstance, 0, 310, 600, 400));
	//windowRects.push_back(CreateCrateAppThreadParam(hInstance, 630, 310, 600, 400));

	std::vector<HANDLE> threads;
	for(int i=0; i<windowRects.size(); i++)
	{
		HANDLE th = CreateThread(NULL, 0, createCrateAppThreadWork, &windowRects[i], 0, 0);
		assert(th!=INVALID_HANDLE_VALUE);
		threads.push_back(th);
	}

	//CreateCrateAppThreadParam appParam;
	//appParam.hInstance = hInstance;
	//appParam.posX = 0;
	//appParam.posY = 0;
	//appParam.width = 600/*1366*/;
	//appParam.height = 768;
	//HANDLE th = CreateThread(NULL, 0, createCrateAppThreadWork, &appParam, 0, 0);

	//CreateCrateAppThreadParam appParam2;
	//appParam2.hInstance = hInstance;
	//appParam2.posX = 630;
	//appParam2.posY = 0;
	//appParam2.width = 600;
	//appParam2.height = 768;
	//HANDLE th2 = CreateThread(NULL, 0, createCrateAppThreadWork, &appParam2, 0, 0);
	while(true)
		Sleep(1);

	for(int i=0; i<threads.size(); i++)
	{
		WaitForSingleObject(threads[i], INFINITE);
		CloseHandle(threads[i]);
	}

	return 0;
	CrateApp theApp(hInstance, 0, 0, 1360, 760);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}
 

CrateApp::CrateApp(HINSTANCE hInstance)
: D3DApp(hInstance, 100, 100, 800, 600), mBoxVB(0), mBoxIB(0), mDiffuseMapSRV(0), mEyePosW(0.0f, 0.0f, 0.0f), 
  mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(2.5f),
  mEffect(NULL),
  mInputLayout(NULL)
  , m_yuvTexture(NULL)
{
	mMainWndCaption = L"Crate Demo";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mBoxWorld, I);
	//Modify 2014-9-1 21:59 scale the quad by radio of the window width/height;
	mBoxWorld._11 = AspectRatio();//(float)mClientWidth/mClientHeight;
	XMStoreFloat4x4(&mTexTransform, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	mDirLights[0].Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
 
	mDirLights[1].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	mBoxMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
}

CrateApp::CrateApp(HINSTANCE hInstance, int posX, int posY, int width, int height)
: D3DApp(hInstance, posX, posY, width, height), mBoxVB(0), mBoxIB(0), mDiffuseMapSRV(0), mEyePosW(0.0f, 0.0f, 0.0f), 
  mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(2.5f),
  mEffect(NULL),
  mInputLayout(NULL)
  , m_yuvTexture(NULL)
{
	mMainWndCaption = L"Crate Demo";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mBoxWorld, I);
	//Modify 2014-9-1 21:59 scale the quad by radio of the window width/height;
	mBoxWorld._11 = AspectRatio();//(float)mClientWidth/mClientHeight;
	XMStoreFloat4x4(&mTexTransform, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	mDirLights[0].Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
 
	mDirLights[1].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	mBoxMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
}


CrateApp::~CrateApp()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mDiffuseMapSRV);

	//Effects::DestroyAll();
	delete mEffect;
	mEffect = NULL;
	ReleaseCOM(mInputLayout);
	//InputLayouts::DestroyAll();
}

bool CrateApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	mEffect = new BasicEffect(md3dDevice, L"FX/Basic.fxo");
	assert(mEffect);
	D3DX11_PASS_DESC passDesc;
	mEffect->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(InputLayoutDesc::Basic32, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &mInputLayout));
	//Effects::InitAll(md3dDevice);
	//InputLayouts::InitAll(md3dDevice);


	//modify 2014-9-5 11:23 使用Texture2D创建图片显示
	/*D3D11_TEXTURE2D_DESC desc;
	desc.Width = 1920;
	desc.Height = 1080;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_R8G8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	HR(md3dDevice_createTex->CreateTexture2D(&desc, NULL, &mTexturesRes));//创建Texture，但没有初始化的数据
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	ZeroMemory(&srvdesc,sizeof(srvdesc));
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;
	HR(md3dDevice_createTex->CreateShaderResourceView(mTexturesRes, &srvdesc, &mDiffuseMapSRV));

	IDXGIResource* pOtherResource(NULL);
	HR(mTexturesRes->QueryInterface( __uuidof(IDXGIResource), (void**)&pOtherResource ));
	pOtherResource->GetSharedHandle(&mTexturesResSharedHandle);
	//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
	//	L"Textures/WoodCrate01.dds", 0, 0, &mDiffuseMapSRV, 0 ));
	*/
	/////////////////////////////// display yuv420/////////////////////////////////////////
	/*ifstream i420FileStream( "D:\\代码黑洞\\datasource\\frame-i420.i420", ios::in | ios::binary);
	if(!i420FileStream)
		return false;
	int width = 720;
	int height = 576;
	int i420FrameDataLen = width * height * 1.5;
	char* frameData = (char*)malloc(i420FrameDataLen);
	assert(frameData);
	i420FileStream.read(frameData, i420FrameDataLen);
	i420FileStream.close();
	m_yuvTexture = new SOA::Render::YUVTexture_Planar(SOA::Render::YUVFORMAT_PLANAR_I420);
	if(0!=m_yuvTexture->create(md3dDevice, width, height, frameData, i420FrameDataLen))
		return false;

	free(frameData);*/

	/////////////////////////////display yuy2///////////////////////////////////
	/*ifstream yuy2FileStream( "D:\\代码黑洞\\datasource\\Frame-720X576.yuy2", ios::in | ios::binary);
	if(!yuy2FileStream)
		return false;
	int width = 720;
	int height = 576;
	int yuy2FrameDataLen = width * height * 2;
	char* frameData = (char*)malloc(yuy2FrameDataLen);
	assert(frameData);
	yuy2FileStream.read(frameData, yuy2FrameDataLen);
	yuy2FileStream.close();
	m_yuvTexture = new SOA::Render::YUVTexture_Packed(SOA::Render::YUVFORMAT_PACKED_YUYV);
	if(0!=m_yuvTexture->create(md3dDevice, width, height, frameData, yuy2FrameDataLen))
		return false;
*/
	/////////////////////////////display NV12///////////////////////////////////
	ifstream yuy2FileStream( "D:\\代码黑洞\\datasource\\frame-nv12.nv12", ios::in | ios::binary);
	if(!yuy2FileStream)
		return false;
	int width = 720;
	int height = 576;
	int yuy2FrameDataLen = width * height * 1.5;
	char* frameData = (char*)malloc(yuy2FrameDataLen);
	assert(frameData);
	yuy2FileStream.read(frameData, yuy2FrameDataLen);
	yuy2FileStream.close();
	m_yuvTexture = new SOA::Render::YUVTexture_NV12(SOA::Render::YUVFORMAT_PLANAR_NV12);
	if(0!=m_yuvTexture->create(md3dDevice, width, height, frameData, yuy2FrameDataLen))
		return false;

	free(frameData);
 
	BuildGeometryBuffers();

	mCurFrameIndex = 0;
	return true;
}

void CrateApp::OnResize()
{
	D3DApp::OnResize();

	//修改透视方式
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	//XMMATRIX P = XMMatrixOrthographicLH(2/**AspectRatio()*/, 2, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void CrateApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	// Build the view matrix.
	//XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	//Modify 2014-9-1 21:47 使用固定的Eye position
	XMVECTOR pos = XMVectorSet(mEyePosW.x, mEyePosW.y, mEyePosW.z, 1.0f);
	//XMVECTOR pos = XMVectorSet(0, 0, -5, 1.0f);
	//mEyePosW = XMFLOAT3(0, 0, -5);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

#include "TimerStopWatch.h"
#include "memcpy_sse41.h"

void CrateApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout/*InputLayouts::Basic32*/);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;
 
	////////////////reflect with eye pos change///////////////////////
	XMVECTOR pos = XMVectorSet(mEyePosW.x, mEyePosW.y, mEyePosW.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
	/////////////////////////////////////////////////

	XMMATRIX view  = XMLoadFloat4x4(&mView);
	XMMATRIX proj  = XMLoadFloat4x4(&mProj);
	//XMMATRIX viewProj = view*proj;

	// Set per frame constants.
	mEffect->SetDirLights(mDirLights);
	mEffect->SetEyePosW(mEyePosW);
 
	ID3DX11EffectTechnique* activeTech = mEffect->Light2TexTech;

	//modify 2014-9-5 11:49 使用map方式修改Texture的内容
	int imgWidth = zTools::ImageSourceHelper::Instance()->getImgWidth();
	int imgHeight = zTools::ImageSourceHelper::Instance()->getImgHeight();
	const char* imgDataBuf = zTools::ImageSourceHelper::Instance()->getFrameImage(mCurFrameIndex);
	assert(imgDataBuf);
	++mCurFrameIndex;
	/*ID3D11Texture2D *sharedTex = NULL;
	HR(md3dDevice->OpenSharedResource(mTexturesResSharedHandle, __uuidof(ID3D11Resource), (void**)&sharedTex));
	IDXGIKeyedMutex* mutex = NULL;
	sharedTex->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&mutex);
	HRESULT hr = mutex->AcquireSync(0, INFINITE);
	int rowPitch = imgWidth*4;
	int depthPitch = rowPitch * imgHeight;
	md3dImmediateContext->UpdateSubresource(sharedTex, 0, NULL, imgDataBuf, rowPitch, depthPitch);
	//D3D11_MAPPED_SUBRESOURCE subRes;
	//HR(md3dImmediateContext->Map(sharedTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes));
	//char* pDes = (char*)subRes.pData;
	//int dataPitch = imgWidth*4;
	//assert(subRes.RowPitch==dataPitch);
	//
	//for(int i=0; i<imgHeight; i++)
	//{
	//	memcpy(pDes, imgDataBuf, dataPitch);
	//	pDes += subRes.RowPitch;
	//	imgDataBuf += dataPitch;
	//}
	//md3dImmediateContext->Unmap(mTexturesRes, 0);
	hr = mutex->ReleaseSync(1);
	md3dImmediateContext->Flush();*/
	/////////////////////////////////////////////////////////////

    D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		// Draw the box.
		//mBoxWorld._11 = AspectRatio();
		XMMATRIX world = XMLoadFloat4x4(&mBoxWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		mEffect->SetWorld(world);
		mEffect->SetWorldInvTranspose(worldInvTranspose);
		mEffect->SetWorldViewProj(worldViewProj);
		mEffect->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
		mEffect->SetMaterial(mBoxMat);

		////////////////// display ARGB ////////////////////////////
		//mEffect->SetDiffuseMap(mDiffuseMapSRV);//set texture
		////////////////// display yuv420 //////////////////////////
		/*int srvCount = 3;
		ID3D11ShaderResourceView* srvList[3] = {NULL, NULL, NULL};
		m_yuvTexture->getShaderResourceView(srvList, srvCount);
		mEffect->SetTexture_Y(srvList[0]);
		mEffect->SetTexture_U(srvList[1]);
		mEffect->SetTexture_V(srvList[2]);*/

		/////////////////////// display yuy2 ////////////////////////
		/*int srvCount = 2;
		ID3D11ShaderResourceView* srvList[2] = {NULL, NULL};
		m_yuvTexture->getShaderResourceView(srvList, srvCount);
		mEffect->SetTexture_Y(srvList[0]);
		//mEffect->SetTexture_U(srvList[1]);
		int width = m_yuvTexture->getWidth();
		float dx = 1 / (float)width;
		mEffect->SetDx(dx);*/

		/////////////////////// display yuy2 ////////////////////////
		int srvCount = 3;
		ID3D11ShaderResourceView* srvList[3] = {NULL, NULL, NULL};
		m_yuvTexture->getShaderResourceView(srvList, srvCount);
		mEffect->SetTexture_Y(srvList[0]);
		mEffect->SetTexture_U(srvList[1]);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
    }
	if (1)
	{

		//for test draw GDI
		IDXGISurface1* bkbufDxgiSurface = NULL;
		HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&bkbufDxgiSurface);
		HDC outHDC = NULL;
		if (SUCCEEDED(hr))
		{
			hr = bkbufDxgiSurface->GetDC(FALSE, &outHDC);
			if (SUCCEEDED(hr))
			{
				TextOutA(outHDC, 100, 100, "Hello", 5);
				bkbufDxgiSurface->ReleaseDC(NULL);
			}
			else
			{
				SAFE_RELEASE(bkbufDxgiSurface);
			}
		}
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
		ReleaseCOM(bkbufDxgiSurface);
	}

	HR(mSwapChain->Present(0, 0));

	if (0)
	{
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	D3D11_TEXTURE2D_DESC backbufDesc;
	backBuffer->GetDesc(&backbufDesc);
	ID3D11Texture2D* outputTexture = mOutputTexture.front();
	mOutputTexture.pop_front();
	md3dImmediateContext->CopyResource(outputTexture, backBuffer);
	mCopyedTexture.push_back(outputTexture);

	if (mOutputTexture.size() <= 0)
	{
		ID3D11Texture2D* copyedTexture = mCopyedTexture.front();
		mCopyedTexture.pop_front();
		mOutputTexture.push_back(copyedTexture);

	D3D11_MAPPED_SUBRESOURCE resource;
	unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
	HRESULT hr = md3dImmediateContext->Map(copyedTexture, subresource, D3D11_MAP_READ/*D3D11_MAP_READ_WRITE*/, 0, &resource);
	//resource.pData; // TEXTURE DATA IS HERE
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC outbufDesc;
		copyedTexture->GetDesc(&outbufDesc);
		const int pitch = resource.RowPitch;
		static unsigned int framecount = 0;
		static unsigned int timespend = 0;
		static Util::TimerStopWatch timer;
		if (framecount == 0)
		{
			timer.start();
		}
		const unsigned char* source = static_cast< const unsigned char* >(resource.pData);
		char strFileName[128] = { 0 };
		sprintf(strFileName, "outfile_%d_%d_%u.rgb", outbufDesc.Width, outbufDesc.Height, framecount);
		static std::ofstream* imgfile = new std::ofstream(strFileName, std::ios::out | std::ios::binary);
		static char* tmp = (char*)_aligned_malloc(pitch*outbufDesc.Height, 32);
		for (int i = 0; i < outbufDesc.Height; ++i)
		{
			const unsigned char* psrc = source + i*pitch;
			//imgfile->write((const char*)psrc, pitch);
			memcpy(tmp + i*pitch, psrc, pitch);
			//memcpy_uncached_load_sse41(tmp + i*pitch, psrc, pitch);
		}
		//delete imgfile;
		//_aligned_free(tmp);
		framecount++;
		if (framecount % 300 == 0)
		{
			timer.stop();
			timespend = timer.interval();
			char msg[512] = { 0 };
			sprintf_s(msg, 512, "Copy %u frame in %u millsec. FrameRate=%f\n", framecount, timespend, (float)framecount / (timespend / 1000));
			OutputDebugStringA(msg);
		}
		md3dImmediateContext->Unmap(copyedTexture, subresource);
	}
	}

	ReleaseCOM(backBuffer);
	}//if(0)
}

void CrateApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void CrateApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CrateApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.01f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 1.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CrateApp::BuildGeometryBuffers()
{
	//modify 2014-9-1 17:27 更换创建Box的顶点的逻辑为创建正方形的顶点
	GeometryGenerator::MeshData box;
	//GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	//geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	//geoGen.CreateFullscreenQuad(quad);
	geoGen.CreateSphere(1.0f, 20, 20, box);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset      = 0;

	// Cache the index count of each object.
	mBoxIndexCount      = box.Indices.size();
	//mBoxIndexCount      = quad.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset      = 0;
	
	UINT totalVertexCount = box.Vertices.size();
	//UINT totalVertexCount = quad.Vertices.size();

	UINT totalIndexCount = mBoxIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	//for(size_t i = 0; i < quad.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex    = box.Vertices[i].TexC;
		//vertices[k].Pos    = quad.Vertices[i].Position;
		//vertices[k].Normal = quad.Vertices[i].Normal;
		//vertices[k].Tex    = quad.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	//indices.insert(indices.end(), quad.Indices.begin(), quad.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}
 
