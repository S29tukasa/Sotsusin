#ifndef GLOBAL_H	//ｲﾝｸﾙｰﾄﾞｶﾞｰﾄﾞ.
#define GLOBAL_H

//警告についてのｺｰﾄﾞ分析を無効にする. 4005:再定義.
#pragma warning(disable:4005)

#include <Windows.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>
#include <D3D10.h>

//ﾗｲﾌﾞﾗﾘ.
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )


//定数.
#define WND_W	1280	//ｳｨﾝﾄﾞｳ幅.
#define WND_H	720		//ｳｨﾝﾄﾞｳ高さ.
#define M_PI	3.14159265358979323846


#endif//#ifndef GLOBAL_H
