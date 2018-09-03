#include "Main.h"
#include "Global.h"
#include "Position.h"
#include "MyMacro.h"
#include <stdio.h>

//ｸﾞﾛｰﾊﾞﾙ変数.
clsMain* g_pClsMain = NULL;
int degree;
int radian;

bool j_flg = true;			//ジャンプを押したか

float wide = 90.0f;			//弾が広がる角度.
float center_direct = 0.0f;	//中心の方向.
float direct = 0;			//自機の向き
float j_cnt = 0.0f;			//飛ぶ時間

// ﾒｲﾝ関数.
INT WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevinstance,PSTR IpCmdLine,INT nCmdShow)
{
	g_pClsMain = new clsMain;

	if (g_pClsMain != NULL)
	{
		//ｳｨﾝﾄﾞｳ作成成功.
		if (SUCCEEDED(g_pClsMain->InitWindow(hInstance,0, 0,WND_W, WND_H,WND_TITLE)))
		{
			//DirectX11用の初期化.
			if (SUCCEEDED(g_pClsMain->Init3D()))
			{
				//ﾒｯｾｰｼﾞﾙｰﾌﾟ.
				g_pClsMain->Loop();
			}
		}
		//終了.
		g_pClsMain->DestroyD3D();
		delete g_pClsMain;//ｸﾗｽの破棄.
		g_pClsMain = NULL;
	}

	return 0;
}

// ｳｨﾝﾄﾞｳﾌﾟﾛｼｰｼﾞｬｰ.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	//ﾌﾟﾛｼｰｼﾞｬｰ.
	return g_pClsMain->MsgProc(hWnd, uMsg, wParam, lParam);
}

//ｺﾝｽﾄﾗｸﾀ.
clsMain::clsMain()
{
	ZeroMemory(this, sizeof(clsMain));

	m_hWnd = NULL;

	m_pDevice = NULL;
	m_pContext = NULL;
	m_pSwapChain = NULL;

	m_pBackBuffer_TexRTV = NULL;
	m_pBackBuffer_DSTex = NULL;
	m_pBackBuffer_DSTexDSV = NULL;

	m_pSampleLinear = NULL;
	m_pTexture = NULL;

	m_Camera.vEye = D3DXVECTOR3(0.0f, 3.7f, -10.0f);

	m_Camera.vLook = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vLight = D3DXVECTOR3(0.0f, 0.5f, -1.0f);

	bool j_flg = true;			//ジャンプを押したか


}

//ﾃﾞｽﾄﾗｸﾀ.
clsMain::~clsMain()
{
}

//ｳｨﾝﾄﾞｳ初期化関数.
HRESULT clsMain::InitWindow(HINSTANCE hInstance,INT iX, INT iY,INT iWidth, INT iHeight,LPSTR WindowName)
{
	//ｳｨﾝﾄﾞｳ定義.
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = APP_NAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//ｳｨﾝﾄﾞｳｸﾗｽをWindowsに登録.
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL,
			"ｳｨﾝﾄﾞｳｸﾗｽの登録に失敗", "エラー", MB_OK);
		return E_FAIL;
	}

	//ｳｨﾝﾄﾞｳの作成.
	m_hWnd = CreateWindow(APP_NAME,WindowName,WS_OVERLAPPEDWINDOW,0, 0,iWidth, iHeight,NULL,NULL,hInstance,NULL);
	if (!m_hWnd)
	{
		MessageBox(NULL,"ｳｨﾝﾄﾞｳ作成に失敗", "エラー", MB_OK);
		return E_FAIL;
	}

	//ｳｨﾝﾄﾞｳの表示.
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return S_OK;
}

//ｳｨﾝﾄﾞｳ関数(ﾒｯｾｰｼﾞ毎の処理)
LRESULT clsMain::MsgProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch ((char)wParam){
		case VK_ESCAPE://ESCｷｰ.
			if (MessageBox(NULL,"閉じる？","警告", MB_YESNO) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			break;
		}
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//ﾒｯｾｰｼﾞﾙｰﾌﾟとｱﾌﾟﾘｹｰｼｮﾝ処理の入り口.
void clsMain::Loop()
{
	m_pDebugText = new clsDebugText;
	D3DXVECTOR4 vColor(1.0f, 0.0f, 0.0f, 1.0f);
	if (FAILED(m_pDebugText->Init(m_pContext, WND_W, WND_H, 50.0f, vColor)))
	{
		return;
	}

	LoadModel();
	LoadSound();
	//--------------ﾌﾚｰﾑﾚｰﾄ調整準備.------------------------------
	
	float Rate = 0.0f;	//ﾚｰﾄ.
	float FPS = 60.0f;	//FPS値.
	DWORD sync_old = timeGetTime();	//過去時間.
	DWORD sync_now;					//現在時間.
	//時間処理の為、最小単位を1ﾐﾘ秒に変更.
	timeBeginPeriod(1);

	//------------------------------------------------------------

	//ﾒｯｾｰｼﾞﾙｰﾌﾟ.
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		Sleep(1);
		sync_now = timeGetTime();//現在時間を取得.

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Rate = 1000.0f / FPS;	//理想時間を算出.
		if(sync_now - sync_old >= Rate)
		{
			sync_old = sync_now;//現在時間に置き換え.
			AppMain();
		}
	}
	//ｱﾌﾟﾘｹｰｼｮﾝの終了.
	timeEndPeriod(1);
}

//ｱﾌﾟﾘｹｰｼｮﾝ処理.ｱﾌﾟﾘのﾒｲﾝ処理.
void clsMain::AppMain()
{
	//ベタ貼り確認用.
	if (GetAsyncKeyState('L') & 0x8000)
	{
		m_pSprite2D->m_Alpha += 0.01f;
		if (m_pSprite2D->m_Alpha > 1.0f)
		{
			m_pSprite2D->m_Alpha = 0.0f;
		}

	}

	if (GetAsyncKeyState('J') & 0x8000)
	{
		m_pPlayer->m_vPos.y += 0.1f;
		//m_aStandard->m_vPos.y += 0.1f;
	}
	if (GetAsyncKeyState('K') & 0x8000)
	{
		m_pPlayer->m_vPos.y -= 0.1f;
		//m_aStandard->m_vPos.y -= 0.1f;

	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		m_pPlayer->m_vPos.x = 0.0f;
		m_pPlayer->m_vPos.y = 0.0f;
		m_pPlayer->m_vPos.z = 0.0f;
	}



	/*足場行動*/
	ScaffSqu();
	UpDown();
	SlowDown();

	/*障害物行動*/
	SawControll();
	NeedleControll();
	WallControll();

	/*キャラ関連*/
	P_ACT();



	//ﾚﾝﾀﾞﾘﾝｸﾞ(描画).
	Render();


}

//ｼｰﾝ(場面)を画面にﾚﾝﾀﾞﾘﾝｸﾞ(描画).
void clsMain::Render()
{
	//画面ｸﾘｱ.
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };//ｸﾘｱ色(RGBAの順).
	//ｶﾗｰﾊﾞｯｸﾊﾞｯﾌｧ.
	m_pContext->ClearRenderTargetView(m_pBackBuffer_TexRTV, ClearColor);
	//ﾃﾞﾌﾟｽｽﾃﾝｼﾙﾊﾞｯｸﾊﾞｯﾌｧ.
	m_pContext->ClearDepthStencilView(m_pBackBuffer_DSTexDSV,D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f, 0);

	Camera();
	Proj();

//ﾒｯｼｭｸﾗｽ内でﾚﾝﾀﾞﾘﾝｸﾞ.
//m_pClsDx9Mesh->Render(mView, mProj, m_vLight, m_vCamera);

	//「地面」の表示.
	m_pGround->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	//「自機」の表示.
	m_pPlayer->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);



	for (int i = 0; i < G_MAX; i++)
	{
		m_aGround[i]->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	}


	m_aScafSqu->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	m_aScafSquM->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);

	m_aScafUp->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	m_aScafDown->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	
	switch (FishBlock)
	{
	case MOVE:
		m_aScaf_PasteOn->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);

		break;
	case RETURN:
		m_aScaf_Paste->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);

		break;

	}

	
	m_aSaw->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	m_aSawCover->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);

	m_aNeedle->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	m_aNeedleSta->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);

	m_aWallMove->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	/*
	//自機のｽﾌｨｱの表示.
	m_pSphere->m_vPos = m_pPlayer->m_vPos;	//自機の位置をｺﾋﾟｰ.
	m_pSphere->m_Scale = m_pPlayer->m_Sphere.Radius * 2.0f;
	m_pSphere->Render(mView, mProj, m_vLight, m_vCamera);
	*/


	//0.0の基準線.
	m_aStandard->Render(m_mView, m_mProj, m_vLight, m_Camera.vEye);

	//デバッグテキスト.
	char strDebugText[256];
	sprintf_s(strDebugText, "Pz[%f] SawX[%f]", m_pPlayer->m_vPos.z, m_aSaw->m_vPos.x);
	//テキスト描画.
	m_pDebugText->Render(strDebugText, 0.0f, 5.0f);

	sprintf_s(strDebugText, "Px[%f] Py[%f] Pz[%f]", m_pPlayer->m_vPos.x, m_pPlayer->m_vPos.y, m_pPlayer->m_vPos.z);
	m_pDebugText->Render(strDebugText, 0.0f, 20.0f);
	sprintf_s(strDebugText, "Ax[%f] Ay[%f] Az[%f]", m_acc.x, m_acc.y, m_acc.z);
	m_pDebugText->Render(strDebugText, 0.0f, 35.0f);

	//スプライト2D.
	m_pSprite2D->Render();


	//ﾚﾝﾀﾞﾘﾝｸﾞされたｲﾒｰｼﾞを表示.
	m_pSwapChain->Present(0, 0);
}

//カメラ関数.
void clsMain::Camera()
{
	//カメラ位置(自機の背面から」)の設定.
	m_Camera.vEye = m_Camera.vLook = m_pPlayer->m_vPos;	//自機の現在地をｺﾋﾟｰ.
	m_Camera.Yaw = m_pPlayer->m_Yaw;					//回転地をｺﾋﾟｰ.
	//Y軸回転行列.
	D3DXMatrixRotationY(&m_Camera.mRotation, m_Camera.Yaw);

	//軸ベクトルを用意する.
	D3DXVECTOR3 vecAxisZ(0.0f, 0.0f, 1.0f);
	//Z軸ベクトルそのものを回転状態により変換する.
	//D3DXVec3TransformCoord(&vecAxisZ, &vecAxisZ, &m_Camera.mRotation);
	//カメラ位置・注視位置を調整.
	m_Camera.vEye -= vecAxisZ*4.0f;		//自機の背中.
	m_Camera.vLook += vecAxisZ*2.0f;	//自機の前側.
	//それぞれの高さ調整.
	m_Camera.vEye.y += 3.5f;		//自機の背中.
	m_Camera.vLook.y += 0.2f;			//自機の前側.

	//ﾋﾞｭｰ(ｶﾒﾗ)変換.
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);	//上方位置.
	D3DXMatrixLookAtLH(
		&m_mView,	//(out)ﾋﾞｭｰ計算結果.
		&m_Camera.vEye, &m_Camera.vLook, &vUpVec);

}

//ﾌﾟﾛｼﾞｪｸｼｮﾝ関数.
void clsMain::Proj()
{
	//ﾌﾟﾛｼﾞｪｸｼｮﾝ(射影行列)変換.
	D3DXMatrixPerspectiveFovLH(
		&m_mProj,						//(out)ﾌﾟﾛｼﾞｪｸｼｮﾝ計算結果.
		D3DX_PI / 4.0,				//y方向の視野(ﾗｼﾞｱﾝ指定)数値を大きくしたら視野が狭くなる.
		(FLOAT)WND_W / (FLOAT)WND_H,//ｱｽﾍﾟｸﾄ比(幅÷高さ).
		0.1f,						//近いﾋﾞｭｰ平面のz値.
		100.0f);					//遠いﾋﾞｭｰ平面のz値.

}


/****************読込関数コーナー******************/

void clsMain::LoadModel()
{
	m_pPlayer = new clsDx9Mesh;
	m_pPlayer->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Player\\player.x");
	m_pPlayer->m_vPos = D3DXVECTOR3(PlayerX, PlayerY, PlayerZ);

	m_pGround = new clsDx9Mesh;
	m_pGround->Init(m_hWnd, m_pDevice, m_pContext,"Data\\Model\\Ground\\Testground.x");
	m_pGround->m_vPos = D3DXVECTOR3(GroundX, GroundY, GroundZ);
	m_pGround->m_Scale = 2.0f;
	
	for (int i = 0; i < G_MAX; i++)
	{
		m_aGround[i] = new clsDx9Mesh;
		m_aGround[i]->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Ground\\ground.x");
		m_aGround[i]->m_vPos = D3DXVECTOR3(-5.0f, -5.0f, -5.0f);

	}
	//仮配置.
	m_aGround[0]->m_vPos = D3DXVECTOR3(GroundX, 3.4f, 15.55f);
	m_aGround[1]->m_vPos = D3DXVECTOR3(GroundX, 3.4f, 31.2f);
	m_aGround[2]->m_vPos = D3DXVECTOR3(GroundX, 3.4f, 45.2f);


	m_aScafSqu = new clsDx9Mesh;
	m_aScafSqu->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Scaffold\\square.x");
	m_aScafSqu->m_vPos = D3DXVECTOR3(scaffoldSquX, scaffoldSquY, scaffoldSquZ);
	m_aScafSqu->m_Scale = 0.6f;

	m_aScafSquM = new clsDx9Mesh;
	m_aScafSquM->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Scaffold\\square.x");
	m_aScafSquM->m_vPos = D3DXVECTOR3(scaffoldSqumX, scaffoldSqumY, scaffoldSqumZ);
	m_aScafSquM->m_Scale = 0.6f;

	m_aScafUp = new clsDx9Mesh;
	m_aScafUp->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Scaffold\\updown.x");
	m_aScafUp->m_vPos = D3DXVECTOR3(upX, upY, upZ);

	m_aScafDown = new clsDx9Mesh;
	m_aScafDown->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Scaffold\\updown.x");
	m_aScafDown->m_vPos = D3DXVECTOR3(downX, downY, downZ);

	m_aScaf_Paste = new clsDx9Mesh;
	m_aScaf_Paste->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Scaffold\\slowdown1.x");
	m_aScaf_Paste->m_vPos = D3DXVECTOR3(downX, 4.0f, 40.0f);
	m_aScaf_Paste->m_Scale = 1.2f;

	m_aScaf_PasteOn = new clsDx9Mesh;
	m_aScaf_PasteOn->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Scaffold\\slowdown2.x");
	m_aScaf_PasteOn->m_vPos = D3DXVECTOR3(downX, 4.0f, 40.0f);
	m_aScaf_PasteOn->m_Scale = 1.2f;

	m_aSaw = new clsDx9Mesh;
	m_aSaw->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Obstacle\\saw.x");
	m_aSaw->m_vPos = D3DXVECTOR3(SawX, SawY, SawZ);
	m_aSaw->m_Pitch = 80.1f;
	m_aSaw->m_Scale = 0.6f;

	m_aSawCover = new clsDx9Mesh;
	m_aSawCover->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Obstacle\\cover.x");
	m_aSawCover->m_vPos = D3DXVECTOR3(SawCovX, SawCovY, SawCovZ);

	m_aNeedle = new clsDx9Mesh;
	m_aNeedle->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Obstacle\\needle.x");
	m_aNeedle->m_vPos = D3DXVECTOR3(NeedleX, NeedleY, NeedleZ);

	m_aNeedleSta = new clsDx9Mesh;
	m_aNeedleSta->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Obstacle\\stature.x");
	m_aNeedleSta->m_vPos = D3DXVECTOR3(NeedleStaX, NeedleStaY, NeedleStaZ);

	m_aWallMove = new clsDx9Mesh;
	m_aWallMove->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Ground\\ground.x");
	m_aWallMove->m_vPos = D3DXVECTOR3(7.5f, 5.0f, 45.0f);
	m_aWallMove->m_Scale = 0.6f;


	//高さ基準(自機)のライン.
	m_aStandard = new clsDx9Mesh;
	m_aStandard->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\0.0.x");
	m_aStandard->m_vPos = D3DXVECTOR3(0.0f, 0.0f, 38.8f);


	//確認用にｽﾌｨｱをﾚﾝﾀﾞﾘﾝｸﾞする(当たり判定としては使用していない).
	m_pSphere = new clsDx9Mesh;
	m_pSphere->Init(m_hWnd, m_pDevice, m_pContext, "Data\\Model\\Sphere\\Sphere.x");

	//ﾒｯｼｭ毎にﾊﾞｳﾝﾃﾞｨﾝｸﾞｽﾌｨｱの作成.
	InitSphere(m_pPlayer);
	InitBBox(m_pPlayer);


	InitSphere(m_pSphere);
	InitBBox(m_aScaf_Paste);
	InitBBox(m_aScaf_PasteOn);

	//表示用ｽﾌｨｱの中心と半径を自機と敵機に反映.
	//m_pPlayer->m_Sphere = m_pSphere->m_Sphere;

	//スプライト情報を設定する(ベタ貼り).
	SPRITE_STATE	ss;
	ss.Width = 256.0f;
	ss.Height = 256.0f;
	ss.U = 1.0f;
	ss.V = 1.0f;

	//スプライト2Dの初期化.
	m_pSprite2D = new clsSprite2D;
	m_pSprite2D->Init(m_pDevice, m_pContext, "Data\\Texture\\UI\\particle.png", ss);
	m_pSprite2D->m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


}

void clsMain::LoadSound()
{
	//m_sTitle.Open
}

void clsMain::LoadPosition()
{

}

/**************************************************/



/*****************各キャラ処理****************/

void clsMain::P_ACT()
{
	//自機移動 Dx9Meshに有.
	if (m_acc.y > 0.1f)
	{ 
	m_acc.y -= 0.011f;
	}
	else
	{
	m_acc.y = 0.00f;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_acc.x += 0.011f;
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if (direct > 135) direct += 3;
			if (direct <  45) direct += 3;
			else if (direct > 45) direct -= 3;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if (direct > 315) direct += 3;
			if (direct <  135) direct += 3;
			else if (direct > 135) direct -= 3;
		}
		if (direct < 90)	direct += 3;
		if (direct > 270)	direct += 3;
		else if (direct >= 90)	direct -= 3;

	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_acc.x -= 0.011f;
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if (direct < 135) direct -= 3;
			if (direct <  315) direct += 3;
			else if (direct > 315) direct -= 3;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if (direct > 45) direct += 3;
			if (direct > 225) direct -= 6;
			else if (direct <  225) direct += 3;
		}
		if (direct == 0)	direct = 359;
		if (direct < 270)	direct += 3;
		if (direct < 90)	direct -= 3;
		else if (direct >= 270)	direct -= 3;

	}
	else if (m_acc.x > 0) {
		m_acc.x -= m_acc.x * 0.06;
		if (m_acc.x < 0) m_acc.x = 0;
	}
	else if (m_acc.x < 0) {
		m_acc.x -= m_acc.x * 0.06;
		if (m_acc.x > 0) m_acc.x = 0;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_acc.z += 0.011f;
		if (direct > 180)	direct += 3;
		else if (direct > 0)	direct -= 3;
		if (direct >= 360)	direct = 0;
		if (direct <= 0)	direct = 0;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_acc.z -= 0.011f;
		if (direct < 180)	direct += 3;
		else if (direct >= 180)	direct -= 3;
	}
	else if (m_acc.z > 0) {
		m_acc.z -= m_acc.z * 0.06;
		if (m_acc.z < 0) m_acc.z = 0;
	}
	else if (m_acc.z < 0) {
		m_acc.z -= m_acc.z * 0.06;
		if (m_acc.z > 0) m_acc.z = 0;
	}
	m_pPlayer->m_Yaw = direct / 180 * PI;
	if (direct > 360)	direct = 1;
	if (direct < 0)		direct = 359;

	if (j_flg == true)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_acc.y += 5.00f;
			j_flg = false;
		}
	}
	else {
		if (m_acc.y > 0.00f)
		{
			m_acc.y -= m_acc.y * 0.5;
		}
	else if (m_acc.y < 0.00f)
		{
			m_acc.y += m_acc.y * 0.5;
		}
	}


	//加速度処理
	if (m_acc.x >  0.05f) m_acc.x =  0.05f;
	if (m_acc.x < -0.05f) m_acc.x = -0.05f;
	if (m_acc.y >  0.75f) m_acc.y =  0.75f;
	if (m_acc.y < -0.10f) m_acc.y = -0.10f;
	if (m_acc.z >  0.05f) m_acc.z =  0.05f;
	if (m_acc.z < -0.05f) m_acc.z = -0.05f;

	m_pPlayer->m_vPos.y -= 0.05f;

	//下の床
	if (m_pPlayer->m_vPos.x > -8.0f)
	{
	 if (m_pPlayer->m_vPos.x < -1.9f)
		{
			if (m_pPlayer->m_vPos.z > -8.75f)
			{
				if (m_pPlayer->m_vPos.z < 2.0f)
				{
					if (m_pPlayer->m_vPos.y <= -4.0f)
					{
						if (m_pPlayer->m_vPos.y >= -6.0f)
						{
							m_pPlayer->m_vPos.y = -4.0f;
							j_flg = true;
						}

					}
				}
			}
		}
	}

	//初期配置の床
	if (m_pPlayer->m_vPos.x > -3.0f)
	{
		if (m_pPlayer->m_vPos.x < 8.25f)
		{
			if (m_pPlayer->m_vPos.z > -11.5f)
			{
				if (m_pPlayer->m_vPos.z < 0.0f)
				{
					if (m_pPlayer->m_vPos.y <= 0.0f)
					{
						if (m_pPlayer->m_vPos.y >= -1.0f)
						{
							m_pPlayer->m_vPos.y = 0.0f;
							j_flg = true;
						}
					}
				}
			}
		}
	}

	//初期配置からの出っ張り
	if (m_pPlayer->m_vPos.x > 0.25f)
	{
		if (m_pPlayer->m_vPos.x < 5.35f)
		{
			if (m_pPlayer->m_vPos.z > -12.0f)
			{
				if (m_pPlayer->m_vPos.z < 5.5f)
				{
					if (m_pPlayer->m_vPos.y <= 0.0f)
					{
						if (m_pPlayer->m_vPos.y >= -1.0f)
						{
							m_pPlayer->m_vPos.y = 0.0f;
							j_flg = true;
						}
					}
				}
			}
		}
	}

	//上の足場
	if (m_pPlayer->m_vPos.x > -0.25f)
	{
		if (m_pPlayer->m_vPos.x < 5.75f)
		{
			if (m_pPlayer->m_vPos.z > 11.5f)
			{
				if (m_pPlayer->m_vPos.z < 22.4f)
				{
					if (m_pPlayer->m_vPos.y <= 4.5f)
					{
						if (m_pPlayer->m_vPos.y >= 1.5f)
						{
							m_pPlayer->m_vPos.y = 4.5f;
							j_flg = true;
						}
					}
				}
			}
		}
	}


	//上奥の足場
	if (m_pPlayer->m_vPos.x > -0.25f)
	{
		if (m_pPlayer->m_vPos.x < 5.75f)
		{
			if (m_pPlayer->m_vPos.z > 27.0f)
			{
				if (m_pPlayer->m_vPos.z < 38.0f)
				{
					if (m_pPlayer->m_vPos.y <= 4.5f)
					{
						if (m_pPlayer->m_vPos.y >= 1.5f)
						{
							m_pPlayer->m_vPos.y = 4.5f;
							j_flg = true;
						}
					}
				}
			}
		}
	}

	//上最奥の足場
	if (m_pPlayer->m_vPos.x > -0.25f)
	{
		if (m_pPlayer->m_vPos.x < 5.75f)
		{
			if (m_pPlayer->m_vPos.z > 41.2f)
			{
				if (m_pPlayer->m_vPos.z < 52.0f)
				{
					if (m_pPlayer->m_vPos.y <= 4.5f)
					{
						if (m_pPlayer->m_vPos.y >= 1.5f)
						{
							m_pPlayer->m_vPos.y = 4.5f;
							j_flg = true;
						}
					}
				}
			}
		}
	}

	//動く足場
	//(何故かm_aScafSqu->…が定義されていないので確認)
	if (m_pPlayer->m_vPos.x >= m_aScafSqu->m_vPos.x - 0.5f)
	{
		if (m_pPlayer->m_vPos.x < m_aScafSqu->m_vPos.x + 0.5f)
		{
			if (m_pPlayer->m_vPos.z >= m_aScafSqu->m_vPos.z - 0.5f)
			{
				if (m_pPlayer->m_vPos.z < m_aScafSqu->m_vPos.z + 0.5f)
				{
					if (m_pPlayer->m_vPos.y <= m_aScafSquM->m_vPos.y + 0.01f)
					{
						if (m_pPlayer->m_vPos.y >= m_aScafSquM->m_vPos.y - 0.05f)
						{
							m_pPlayer->m_vPos.y = m_aScafSquM->m_vPos.y;
							m_acc.y = 0.00f;
							j_flg = true;
						}
					}
				}
			}
		}
	}


	m_pPlayer->m_vPos.x += m_acc.x;
	m_pPlayer->m_vPos.y += m_acc.y;
	m_pPlayer->m_vPos.z += m_acc.z;



}

void clsMain::B_ACT()
{

}

void clsMain::E_ACT()
{

}

/**********************************************/


/*足場*/
void clsMain::ScaffSqu()
{
	scaffoldCntSqu++;


	switch (Italy)
	{
	case L3:
		m_aScafSqu->m_vPos.x += 0.01f;
		m_aScafSquM->m_vPos.x -= 0.01f;
		if (340 <= scaffoldCntSqu)
		{
			m_aScafSqu->m_vPos.x = 4.4f;
			m_aScafSquM->m_vPos.x = 1.0f;
			Italy = CV33;
		}
		break;
	case CV33:
		m_aScafSqu->m_vPos.z += 0.01f;
		m_aScafSqu->m_vPos.y += 0.01f;

		m_aScafSquM->m_vPos.z -= 0.01f;
		m_aScafSquM->m_vPos.y -= 0.01f;
		if (739<= scaffoldCntSqu)
		{
			m_aScafSqu->m_vPos.y = 4.2f;
			m_aScafSqu->m_vPos.z = 11.0f;
			m_aScafSquM->m_vPos.y = 0.1f;
			m_aScafSquM->m_vPos.z = 7.0f;

			Italy = CV35;
		}
		break;
	case CV35:
		m_aScafSqu->m_vPos.x -= 0.01f;
		m_aScafSquM->m_vPos.x += 0.01f;

		if (1079 <= scaffoldCntSqu)
		{
			m_aScafSqu->m_vPos.x = 1.0f;
			m_aScafSquM->m_vPos.x = 4.4f;

			Italy = CV38;
		}
		break;
	case CV38:
		m_aScafSqu->m_vPos.z -= 0.01f;
		m_aScafSqu->m_vPos.y -= 0.01f;
		m_aScafSquM->m_vPos.z += 0.01f;
		m_aScafSquM->m_vPos.y += 0.01f;
		
		if (1490 <= scaffoldCntSqu)
		{
			m_aScafSqu->m_vPos.x = 1.0f;
			m_aScafSqu->m_vPos.y = 0.1f;
			m_aScafSqu->m_vPos.z = 7.0f;
			m_aScafSquM->m_vPos.x = 4.4f;
			m_aScafSquM->m_vPos.y = 4.2f;
			m_aScafSquM->m_vPos.z = 11.0f;
			scaffoldCntSqu = 0;

			Italy = L3;
		}
		break;

	}




	


}

void clsMain::UpDown()
{
	UpDownCnt++;
	switch (France)
	{
	case S35:
		m_aScafUp->m_vPos.y -= 0.03f;
		m_aScafDown->m_vPos.y += 0.03f;
		if (UpDownCnt>130)
		{
			m_aScafUp->m_vPos.y -= 0.00f;
			m_aScafDown->m_vPos.y += 0.000f;

			m_aScafUp->m_vPos.y = -4.8f;
			m_aScafDown->m_vPos.y = -0.89f;
			if (UpDownCnt > 150)
			{
				France = ARL44;

			}
		}

		break;
	case ARL44:

		m_aScafUp->m_vPos.y += 0.03f;
		m_aScafDown->m_vPos.y -= 0.03f;
		if (UpDownCnt>280)
		{
			m_aScafUp->m_vPos.y -= 0.00f;
			m_aScafDown->m_vPos.y += 0.000f;

			m_aScafUp->m_vPos.y = -0.89f;
			m_aScafDown->m_vPos.y = -4.8f;
			if (UpDownCnt > 300)
			{
				UpDownCnt = 0;
				France = S35;

			}
		}


		break;
	}

}

void clsMain::SlowDown()
{
	if (BBoxCollision(m_pPlayer, m_aScaf_Paste))
	{
		ColorChange = true;
	}
	else
	{
		ColorChange = false;
	}

	if (ColorChange)			FishBlock = MOVE;
	if (!ColorChange)			FishBlock = RETURN;


	switch (FishBlock)
	{
	case MOVE:

		SlowDownCnt++;
		if (SlowDownCnt > 250)
		{
			m_aScaf_PasteOn->m_vPos.y -= 0.07;
			if (SlowDownCnt>300)
			{
				m_aScaf_PasteOn->m_vPos.y = 4.0f;
				FishBlock = RETURN;
			}
		}


		break;
	case RETURN:

		SlowDownCnt = 0;
		m_aScaf_PasteOn->m_vPos.y = 4.0f;
		if (BBoxCollision(m_pPlayer, m_aScaf_Paste))
		{
			FishBlock = MOVE;
		}

		break;
	}

}

/*障害物*/
void clsMain::SawControll()
{
	m_aSaw->m_Roll += 0.5f;
	SawControllCnt++;

	switch (Germany)
	{
	case MARK1:

		m_aSaw->m_vPos.x += 0.023f;
		if (280 < SawControllCnt)
		{
			m_aSaw->m_vPos.x = SawMovingX;//3.6
			Germany = MARK2;
		}


		break;
	case MARK2:
		m_aSaw->m_vPos.x -= 0.023f;
		if (560 < SawControllCnt)
		{
			m_aSaw->m_vPos.x = SawX;
			SawControllCnt = 0;
			Germany = MARK1;
		}
		break;
	}

}

void clsMain::NeedleControll()
{
	NeedleControllCnt++;
	switch (American)
	{
	case M4SHA:
		if (NeedleControllCnt > 70)
		{
			m_aNeedle->m_vPos.y = 3.1f;

			American = M4A1SHA;
		}

		break;
	case M4A1SHA:
		if (NeedleControllCnt > 110)
		{
			m_aNeedle->m_vPos.y -= 0.01f;
			if (m_aNeedle->m_vPos.y < 2.4f)
			{
				NeedleControllCnt = 0;
				American = M4SHA;
			}
		}

		break;
	}

}

void clsMain::WallControll()
{


	switch (WallCnt)
	{
	case PUSH:
			PushCnt++;
			PushSpeed += 0.02;
			if (PushCnt > 100)
			{
				m_aWallMove->m_vPos.x -= 0.1*PushSpeed;

				if (m_aWallMove->m_vPos.x < 2.0f)
				{
					WallCnt = SLOWRETURN;
				}
			}

		

		break;
	case SLOWRETURN:
			m_aWallMove->m_vPos.x += 0.02;
			if (m_aWallMove->m_vPos.x > 8.0)
			{
				PushCnt = 0;
				PushSpeed = 0.00f;
				WallCnt = MOVE;
			}
		

		break;
	}


}









//Direct3D初期化.
HRESULT clsMain::Init3D()
{
	//-----------------------------------------------
	//	ﾃﾞﾊﾞｲｽとｽﾜｯﾌﾟﾁｪｰﾝ関係.
	//-----------------------------------------------

	//ｽﾜｯﾌﾟﾁｪｰﾝ構造体.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount	= 1;//ﾊﾞｯｸﾊﾞｯﾌｧの数.
	sd.BufferDesc.Width = WND_W;//ﾊﾞｯｸﾊﾞｯﾌｧの幅.
	sd.BufferDesc.Height = WND_H;//ﾊﾞｯｸﾊﾞｯﾌｧの高さ.
	//ﾌｫｰﾏｯﾄ(32ﾋﾞｯﾄｶﾗｰ)
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//ﾘﾌﾚｯｼｭﾚｰﾄ(分母) ※FPS:60
	sd.BufferDesc.RefreshRate.Numerator = 60;
	//ﾘﾌﾚｯｼｭﾚｰﾄ(分子)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	//使い方(表示先)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ.
	sd.SampleDesc.Count = 1;//ﾏﾙﾁｻﾝﾌﾟﾙ数.
	sd.SampleDesc.Quality = 0;//ﾏﾙﾁｻﾝﾌﾟﾙのｸｵﾘﾃｨ
	sd.Windowed = TRUE;//ｳｨﾝﾄﾞｳﾓｰﾄﾞ(ﾌﾙｽｸﾘｰﾝ時はFALSE)

	//作成を試みる機能ﾚﾍﾞﾙの優先を指定.
	//	(GPUがｻﾎﾟｰﾄする機能ｾｯﾄの定義)
	//	D3D_FEATURE_LEVEL列挙型の配列.
	//	D3D_FEATURE_LEVEL_10_1：Direct3D 10.1のGPUﾚﾍﾞﾙ.
	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL* pFeatureLevel = NULL;//配列の要素数.

	//ﾃﾞﾊﾞｲｽとｽﾜｯﾌﾟﾁｪｰﾝの作成.
	//ﾊｰﾄﾞｳｪｱ(GPU)ﾃﾞﾊﾞｲｽで作成.
	if (FAILED(	D3D11CreateDeviceAndSwapChain(
		NULL,	//ﾋﾞﾃﾞｵｱﾀﾞﾌﾟﾀへのﾎﾟｲﾝﾀ.
		D3D_DRIVER_TYPE_HARDWARE,//作成するﾃﾞﾊﾞｲｽの種類.
		NULL,	//ｿﾌﾄｳｪｱ ﾗｽﾀﾗｲｻﾞｰを実装するDLLのﾊﾝﾄﾞﾙ.
		0,	//有効にするﾗﾝﾀｲﾑﾚｲﾔｰ.
		&pFeatureLevels,//作成を試みる機能ﾚﾍﾞﾙの順序を指定する配列へのﾎﾟｲﾝﾀ.
		1,	//↑の要素数.
		D3D11_SDK_VERSION,//SKDのﾊﾞｰｼﾞｮﾝ.
		&sd,	//ｽﾜｯﾌﾟﾁｪｰﾝの初期化ﾊﾟﾗﾒｰﾀのﾎﾟｲﾝﾀ.
		&m_pSwapChain,//(out)ﾚﾝﾀﾞﾘﾝｸﾞに使用するｽﾜｯﾌﾟﾁｪｰﾝ.
		&m_pDevice,	//(out)作成されたﾃﾞﾊﾞｲｽ.
		pFeatureLevel,//機能ﾚﾍﾞﾙの配列にある最初の要素を表すﾎﾟｲﾝﾀ.
		&m_pContext)))//(out)ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ.
	{
		//WARPﾃﾞﾊﾞｲｽの作成.(Windows Advanced Rasterization Platform)
		// D3D_FEATURE_LEVEL_9_1～D3D_FEATURE_LEVEL_10_1
		if (FAILED(
			D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_WARP,//作成するﾃﾞﾊﾞｲｽの種類.
			NULL, 0, &pFeatureLevels, 1, D3D11_SDK_VERSION,
			&sd, &m_pSwapChain, &m_pDevice, pFeatureLevel, &m_pContext)))
		{
			//ﾘﾌｧﾚﾝｽﾃﾞﾊﾞｲｽの作成.
			// DirectX SKDがｲﾝｽﾄｰﾙされてないと使えない.
			if (FAILED(
				D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_REFERENCE,//作成するﾃﾞﾊﾞｲｽの種類.
				NULL, 0, &pFeatureLevels, 1, D3D11_SDK_VERSION,
				&sd, &m_pSwapChain, &m_pDevice, pFeatureLevel, &m_pContext)))
			{
				MessageBox(NULL,
					"ﾃﾞﾊﾞｲｽとｽﾜｯﾌﾟﾁｪｰﾝの作成に失敗",
					"error(main.cpp)", MB_OK);
				return E_FAIL;
			}
		}
	}

	//各種ﾃｸｽﾁｬと、それに付帯する各種ﾋﾞｭｰを作成.

	//-----------------------------------------------
	//	ﾊﾞｯｸﾊﾞｯﾌｧ準備：ｶﾗｰﾊﾞｯﾌｧ設定.
	//-----------------------------------------------

	//ﾊﾞｯｸﾊﾞｯﾌｧﾃｸｽﾁｬを取得(既にあるので作成ではない)
	ID3D11Texture2D* pBackBuffer_Tex;
	m_pSwapChain->GetBuffer(0,
		__uuidof(ID3D11Texture2D),//__uuidof：式に関連付けされたGUIDを取得.
								  //Texture2Dの唯一の物として扱う.
		(LPVOID*)&pBackBuffer_Tex);//(out)ﾊﾞｯｸﾊﾞｯﾌｧﾃｸｽﾁｬ.

	//そのﾃｸｽﾁｬに対しﾚﾝﾀﾞｰﾀｰｹﾞｯﾄﾋﾞｭｰ(RTV)を作成.
	m_pDevice->CreateRenderTargetView(
		pBackBuffer_Tex,
		NULL,
		&m_pBackBuffer_TexRTV);//(out)RTV.
	//ﾊﾞｯｸﾊﾞｯﾌｧﾃｸｽﾁｬを開放.
	SAFE_RELEASE(pBackBuffer_Tex);


	//-----------------------------------------------
	//	ﾊﾞｯｸﾊﾞｯﾌｧ準備:ﾃﾞﾌﾟｽ(深度)ｽﾃﾝｼﾙ関係.
	//-----------------------------------------------
	//ﾃﾞﾌﾟｽ(深さor深度)ｽﾃﾝｼﾙﾋﾞｭｰ用のﾃｸｽﾁｬを作成.
	D3D11_TEXTURE2D_DESC descDepth;

	descDepth.Width = WND_W;//幅.
	descDepth.Height = WND_H;//高さ.
	descDepth.MipLevels = 1;//ﾐｯﾌﾟﾏｯﾌﾟﾚﾍﾞﾙ:1.
	descDepth.ArraySize = 1;//配列数:1.
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;//32ﾋﾞｯﾄﾌｫｰﾏｯﾄ.
	descDepth.SampleDesc.Count = 1;//ﾏﾙﾁｻﾝﾌﾟﾙの数.
	descDepth.SampleDesc.Quality = 0;//ﾏﾙﾁｻﾝﾌﾟﾙのｸｵﾘﾃｨ.
	descDepth.Usage = D3D11_USAGE_DEFAULT;//使用方法:ﾃﾞﾌｫﾙﾄ.
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;//深度(ｽﾃﾝｼﾙとして使用).
	descDepth.CPUAccessFlags = 0;//CPUからはｱｸｾｽしない.
	descDepth.MiscFlags = 0;//その他:設定なし.

	if (FAILED(
		m_pDevice->CreateTexture2D(
		&descDepth,
		NULL,
		&m_pBackBuffer_DSTex)))//(out)ﾃﾞﾌﾟｽｽﾃﾝｼﾙ用ﾃｸｽﾁｬ.
	{
		MessageBox(NULL,
			"ﾃﾞﾌﾟｽｽﾃﾝｼﾙ用ﾃｸｽﾁｬ作成失敗", "error", MB_OK);
		return E_FAIL;
	}
	//そのﾃｸｽﾁｬに対しﾃﾞﾌﾟｽｽﾃﾝｼﾙﾋﾞｭｰ(DSV)を作成.
	if (FAILED(
		m_pDevice->CreateDepthStencilView(
		m_pBackBuffer_DSTex,
		NULL,
		&m_pBackBuffer_DSTexDSV)))//(out)ﾃﾞﾌﾟｽｽﾃﾝｼﾙﾋﾞｭｰ(DSV).
	{
		MessageBox(NULL,
			"ﾃﾞﾌﾟｽｽﾃﾝｼﾙﾋﾞｭｰ作成失敗", "error", MB_OK);
		return E_FAIL;
	}

	//ﾚﾝﾀﾞｰﾀｰｹﾞｯﾄﾋﾞｭｰとﾃﾞﾌﾟｽｽﾃﾝｼﾙﾋﾞｭｰをﾊﾟｲﾌﾟﾗｲﾝにｾｯﾄ.
	m_pContext->OMSetRenderTargets(
		1,
		&m_pBackBuffer_TexRTV,
		m_pBackBuffer_DSTexDSV);

	//ﾋﾞｭｰﾎﾟｰﾄの設定.
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)WND_W;	//幅.
	vp.Height = (FLOAT)WND_H;	//高さ.
	vp.TopLeftX = 0.0f;	//左上位置x.
	vp.TopLeftY = 0.0f;	//左上位置y.
	vp.MinDepth = 0.0f;	//最小深度(手前).
	vp.MaxDepth = 1.0f;	//最大深度(奥).
	m_pContext->RSSetViewports(1, &vp);

	//ﾗｽﾀﾗｲｽﾞ(面の塗り潰し方)設定.
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.FillMode = D3D11_FILL_SOLID;//塗り潰し(ｿﾘｯﾄﾞ).
									//D3D11_FILL_WIREFRAME:ﾜｲﾔｰﾌﾚｰﾑ.
	rdc.CullMode = D3D11_CULL_NONE;	//D3D11_CULL_NONE:ｶﾘﾝｸﾞを切る
									//(正面背面を描画する)
									//D3D11_CULL_FRONT:正面を描画しない.
									//D3D11_CULL_BACK :背面を描画しない.
	rdc.FrontCounterClockwise = FALSE;//ﾎﾟﾘｺﾞﾝの裏表を決定するﾌﾗｸﾞ.
									  //TRUE:左回りなら前向き.右回りなら後ろ向き.
									  //FALSE:逆になる.
	rdc.DepthClipEnable = TRUE;	//距離についてのｸﾘｯﾋﾟﾝｸﾞ有効.
	
	//ﾗｽﾀﾗｲｻﾞ作成.
	ID3D11RasterizerState* pIr = NULL;
	m_pDevice->CreateRasterizerState(&rdc, &pIr);
	m_pContext->RSSetState(pIr);
	SAFE_RELEASE(pIr);


	//ﾒｯｼｭﾌｧｲﾙ読込.
	//m_pClsDx9Mesh = new clsDx9Mesh;
	//m_pClsDx9Mesh->Init(m_hWnd, m_pDevice, m_pContext,"Data\\model_pnt.x");

	return S_OK;
}

//Direct3D終了処理.
void clsMain::DestroyD3D()
{

	//ﾓﾃﾞﾙの解放.
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSampleLinear);

	//Direct3Dｵﾌﾞｼﾞｪｸﾄの解放.
	SAFE_RELEASE(m_pBackBuffer_DSTexDSV);
	SAFE_RELEASE(m_pBackBuffer_DSTex);
	SAFE_RELEASE(m_pBackBuffer_TexRTV);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);

}


//ｽﾌｨｱ作成.
HRESULT clsMain::InitSphere(clsDx9Mesh* pMesh)
{
	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;//頂点ﾊﾞｯﾌｧ.
	void*		pVertices = NULL;//頂点.
	D3DXVECTOR3	vCenter;	//中心.
	float		Radius;		//半径.

	//頂点ﾊﾞｯﾌｧを取得.
	if (FAILED(pMesh->m_pMesh->GetVertexBuffer(&pVB)))
	{
		return E_FAIL;
	}
	//ﾒｯｼｭの頂点ﾊﾞｯﾌｧをﾛｯｸする.
	if (FAILED(pVB->Lock(0, 0, &pVertices, 0)))
	{
		SAFE_RELEASE(pVB);
		return E_FAIL;
	}

	//ﾒｯｼｭの外接円の中心と半径を計算する.
	D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)pVertices,
		pMesh->m_pMesh->GetNumVertices(),	//頂点数.
		D3DXGetFVFVertexSize(pMesh->m_pMesh->GetFVF()),//頂点情報.
		&vCenter,	//(out)中心座標.
		&Radius);	//(out)半径.

	//ｱﾝﾛｯｸ.
	pVB->Unlock();
	SAFE_RELEASE(pVB);

	//中心と半径を構造体に設定.
	pMesh->m_Sphere.vCenter = vCenter;
	pMesh->m_Sphere.Radius = Radius;

	return S_OK;
}

//衝突判定.
bool clsMain::Collision(clsDx9Mesh* pAttacker, clsDx9Mesh* pTarget)
{
	//２つの物体の中心間の距離を求める.
	D3DXVECTOR3 vLength
		= pTarget->m_vPos - pAttacker->m_vPos;
	//長さに変換する.
	float Length = D3DXVec3Length(&vLength);

	//２つの物体の距離が、２つの物体の半径をたしたものより小さいということは.
	//ｽﾌｨｱ同士が重なっている(衝突している)ということ.
	if (Length <= pAttacker->m_Sphere.Radius + pTarget->m_Sphere.Radius)
	{
		return true;//衝突.
	}
	return false;//衝突していない.
}

//バウンディングボックス作成.
HRESULT	clsMain::InitBBox(clsDx9Mesh* pMesh)
{
	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	VOID* pVertices = NULL;
	D3DXVECTOR3 Max, Min;

	//ﾒｯｼｭの頂点バッファをロックする.
	if (FAILED(pMesh->m_pMesh->GetVertexBuffer(&pVB)))
	{
		MessageBox(NULL, "頂点バッファ取得失敗", "error", MB_OK);
		return E_FAIL;
	}
	if (FAILED(pVB->Lock(0, 0, &pVertices, 0)))
	{
		MessageBox(NULL, "頂点バッファロック失敗", "error", MB_OK);
		return E_FAIL;

	}
	//メッシュ内の頂点位置の最大と最小を検索する
	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices,
		pMesh->m_pMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(pMesh->m_pMesh->GetFVF()),
		&Min, &Max);//出力：最小、最大頂点.

	pVB->Unlock();//アンロック.
	SAFE_RELEASE(pVB);

	pMesh->m_BBox.vPosMax = Max;
	pMesh->m_BBox.vPosMin = Min;

	//軸ベクトル・軸の長さ(この場合、ボックスの各半径)を初期化する.
	pMesh->m_BBox.LengthX = (Max.x - Min.x) / 2.0f;
	pMesh->m_BBox.LengthY = (Max.y - Min.y) / 2.0f;
	pMesh->m_BBox.LengthZ = (Max.z - Min.z) / 2.0f;

	return S_OK;
}
//■衝突判定.
bool clsMain::BBoxCollision(clsDx9Mesh* pAttacker, clsDx9Mesh* pTarget)
{
	//攻撃用に2つの頂点を用意し初期化する.
	D3DXVECTOR3 vecAtkMax, vecAtkMin;	//最大、最小頂点.
	vecAtkMax = pAttacker->m_BBox.vPosMax + pAttacker->m_vPos;
	vecAtkMin = pAttacker->m_BBox.vPosMin + pAttacker->m_vPos;

	/*当たり判定のMin/Maxの数値を確保*/
	CollB.m_MinX = vecAtkMin.x;
	CollB.m_MinY = vecAtkMin.y;
	CollB.m_MinZ = vecAtkMin.z;

	CollB.m_MaxX = vecAtkMax.x;
	CollB.m_MaxY = vecAtkMax.y;
	CollB.m_MaxZ = vecAtkMax.z;
	/*********************************/


	//目標用用に2つの頂点を用意し初期化する.
	D3DXVECTOR3 vecTrgMax, vecTrgMin;	//最大、最小頂点.
	vecTrgMax = pTarget->m_BBox.vPosMax + pTarget->m_vPos;
	vecTrgMin = pTarget->m_BBox.vPosMin + pTarget->m_vPos;


	//2つのメッシュそれぞれの最大、最小位置を使う.
	//4つの情報があれば、衝突を検出できる.
	if (vecAtkMin.x<vecTrgMax.x &&vecAtkMax.x>vecTrgMin.x
		&&vecAtkMin.y<vecTrgMax.y &&vecAtkMax.y>vecTrgMin.y
		&&vecAtkMin.z<vecTrgMax.z &&vecAtkMax.z>vecTrgMin.z)
	{
		return true;	//衝突している.
	}
	return false;		//衝突していない.
}

//人間は愚か
