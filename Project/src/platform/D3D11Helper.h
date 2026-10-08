#pragma once

#include <Windows.h>
#include <d3d11_4.h>
#include <iostream>
#include <sstream>
#include <ctime>

#include "debug/Debug.h"

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#define ErrMessage( msg )																	\
{																							\
   std::wostringstream os_;																	\
   os_ << msg;																				\
   OutputDebugStringW( os_.str().c_str() );													\
}

#if DEBUG
	#ifndef HR
	#define HR(x)																								\
		{																										\
			HRESULT hr = (x);																					\
			if(FAILED(hr))																						\
			{																									\
				ErrMessage("\n[D3D11 Error] " << __FILE__ << ":" << __LINE__ << " \"" << hr << "\"\n\n");		\
				__debugbreak();																					\
			}																									\
		}
	#endif // !HR
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif // !DEBUG


#ifndef RELEASECOM
#define ReleaseCOM(x) { if (x) { x->Release(); x=nullptr;} }
#endif // !RELEASECOM

#ifndef CSO_PATH
#define CSO_PATH "res\\Shaders\\csoFiles"
#endif // !CSO_PATH

#ifndef SHADER_PATH
#define SHADER_PATH "res\\Shaders"
#endif // !SHADER_PATH

#ifndef SHADER_EXTENSION
#define SHADER_EXTENSION "hlsl"
#endif // !SHADER_EXTENSION



bool SetupD3D11(UINT width, UINT height, HWND widnow, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain,
				ID3D11RenderTargetView*&, ID3D11UnorderedAccessView*& uav, ID3D11ShaderResourceView*& srv, D3D11_VIEWPORT& viewport);

bool InitializeBackBuffer(IDXGISwapChain* swapChain);