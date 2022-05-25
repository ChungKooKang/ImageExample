#include <fstream>
#include <vector>
#include "ImageExample.h"

#pragma comment(lib, "WindowsCodecs.lib")

HRESULT ImageExample::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	CoInitialize(nullptr);
	D2DFramework::Initialize(hInstance, title, width, height);

	// LoadBMP(L"Data/32.bmp", mspBitmap.GetAddressOf());
	LoadWICImage(L"Data/32.bmp", mspBitmap.GetAddressOf());
	return S_OK;
}

void ImageExample::Render()
{
	mspRenderTarget->BeginDraw();
	mspRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightSalmon));

	mspRenderTarget->DrawBitmap(mspBitmap.Get());

	mspRenderTarget->EndDraw();
}

void ImageExample::Release()
{
	D2DFramework::Release();

	mspBitmap.Reset();
	mspWICFactory.Reset();

	CoUninitialize();
}

HRESULT ImageExample::LoadBMP(LPCWSTR filename, ID2D1Bitmap** ppBitmap)
{
	// 1. 파일열기
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// 2. BITMAPFILEHEADER 읽기
	BITMAPFILEHEADER bfh;
	file.read(reinterpret_cast<char*>( & bfh), sizeof(BITMAPFILEHEADER));

	// 3. BITMAPINFOHEADER 읽기
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

	// 4. bfOffsets 만큼 건너뛰기
	file.seekg(bfh.bfOffBits);

	// 5. 픽셀 배열 읽기
	std::vector<char> pPixels(bih.biSizeImage);
	
	int pitch = bih.biWidth * (bih.biBitCount / 8);

	// bih.biheight 음수 : 정상, 양수 : 뒤집기
	//if (bih.biHeight < 0)
	//{
	//	// file.read(&pPixels[0], bih.biSizeImage);
	//}

	

	//for (int y = bih.biHeight -1 ; y >= 0;y--)
	//{
	//	file.read(&pPixels[y * pitch], pitch);
	//}

	// RGB(30, 199, 250) 즉 바탕색이면 읽지 않기

	int index{};
	for (int y = bih.biHeight - 1; y >= 0; y--)
	{
		index = y * pitch;
		for (int x = 0; x < bih.biWidth; x++)
		{
			char r{}, g{}, b{}, a{};

			file.read(&b, 1);
			file.read(&g, 1);
			file.read(&r, 1);
			file.read(&a, 1);

			if (static_cast<unsigned char>(r) == 30 && static_cast<unsigned char>(g) == 199 && static_cast<unsigned char>(b) == 250)
			{
				r = g = b = a = 0;
			}

			pPixels[index++] = b;
			pPixels[index++] = g;
			pPixels[index++] = r;
			pPixels[index++] = a;

		}
	}
	file.close();


	// 6. 5번 데이터로 BITMAP 만들기
	HRESULT hr = mspRenderTarget->CreateBitmap(
		D2D1::SizeU(bih.biWidth, bih.biHeight),
		D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), // little endian 때문에 BGRA이다.
		ppBitmap
		);
	ThrowIfFailed(hr);
	
	(*ppBitmap)->CopyFromMemory(nullptr, &pPixels[0], pitch);

	return S_OK;
}

HRESULT ImageExample::LoadWICImage(LPCWSTR filename, ID2D1Bitmap** ppBitmap)
{
	return E_NOTIMPL;
}
