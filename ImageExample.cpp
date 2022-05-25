#include <fstream>
#include <vector>
#include "ImageExample.h"

HRESULT ImageExample::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	D2DFramework::Initialize(hInstance, title, width, height);

	LoadBMP(L"Data/32.bmp", mspBitmap.GetAddressOf());

	return S_OK;
}

void ImageExample::Render()
{
	mspRenderTarget->BeginDraw();
	mspRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightSalmon));

	mspRenderTarget->DrawBitmap(mspBitmap.Get());

	mspRenderTarget->EndDraw();
}

HRESULT ImageExample::LoadBMP(LPCWSTR filename, ID2D1Bitmap** ppBitmap)
{
	// 1. ���Ͽ���
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// 2. BITMAPFILEHEADER �б�
	BITMAPFILEHEADER bfh;
	file.read(reinterpret_cast<char*>( & bfh), sizeof(BITMAPFILEHEADER));

	// 3. BITMAPINFOHEADER �б�
	BITMAPINFOHEADER bih;
	file.read(reinterpret_cast<char*>(&bih), sizeof(BITMAPINFOHEADER));

	if (bfh.bfType != 0X4D42)
	{
		return E_FAIL;
	}
	if (bih.biBitCount != 32)
	{
		return E_FAIL;
	}

	// 4. bfOffsets ��ŭ �ǳʶٱ�
	file.seekg(bfh.bfOffBits);

	// 5. �ȼ� �迭 �б�
	std::vector<char> pPixels(bih.biSizeImage);
	
	int pitch = bih.biWidth * (bih.biBitCount / 8);

	// bih.biheight ���� : ����, ��� : ������
	//if (bih.biHeight < 0)
	//{
	//	// file.read(&pPixels[0], bih.biSizeImage);
	//}

	

	for (int y = bih.biHeight -1 ; y >= 0;y--)
	{
		file.read(&pPixels[y * pitch], pitch);
	}

	file.close();


	// 6. 5�� �����ͷ� BITMAP �����
	HRESULT hr = mspRenderTarget->CreateBitmap(
		D2D1::SizeU(bih.biWidth, bih.biHeight),
		D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), // little endian ������ BGRA�̴�.
		ppBitmap
		);
	ThrowIfFailed(hr);
	
	(*ppBitmap)->CopyFromMemory(nullptr, &pPixels[0], pitch);

	return S_OK;
}
