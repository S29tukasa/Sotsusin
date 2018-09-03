//警告についてのｺｰﾄﾞ分析を無効にする. 4005:再定義.
#pragma warning(disable:4005)

#include <Windows.h>

#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>
#include <D3D10.h>

#include "DX9Mesh.h"
#include "Sprite.h"
#include "Sprite2D.h"
#include "DebugText.h"

#include "Sound.h"

#include "costant.h"	//定数まとめ先.

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )//「D3DX～」の定義使用時に必要.

//定数.
#define WND_TITLE	"初めての3Dシューティング"
#define APP_NAME	"3DSTG"
#define PI			 3.14159265358979323846

//構造体.
//カメラ情報.
struct CAMERA
{
	D3DXVECTOR3	vEye;		//カメラ位置.
	D3DXVECTOR3 vLook;		//カメラ注視位置.
	float		Yaw;		//Y回転軸.
	D3DXMATRIX	mRotation;	//回転行列.

};
//当たり判定場所取り.
struct COLLSIONBOX
{
	D3DXVECTOR3	MinPos;
	float	m_MinX;
	float	m_MinY;
	float	m_MinZ;

	D3DXVECTOR3	MaxPos;
	float	m_MaxX;
	float	m_MaxY;
	float	m_MaxZ;

};

enum SQUARE
{
	L3,CV33,CV35,CV38
};

enum UPDOWN
{
	S35,ARL44
};

enum SAWMOVE
{
	MARK1,MARK2
};

enum NEEDLEMOVE
{
	M4SHA,M4A1SHA
};

enum SLOWDOWN
{
	MOVE,RETURN
};

enum WALLMOVE
{
	PUSH,SLOWRETURN
};
/*************************************************
*	ﾒｲﾝｸﾗｽ.
**/
class clsMain
{
public:
	clsMain();
	~clsMain();

	//ｳｨﾝﾄﾞｳ初期化関数.
	HRESULT InitWindow(HINSTANCE hInstance,INT iX, INT iY, INT iWidth, INT iHeight,LPSTR WindowName);

	LRESULT MsgProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam);

	void Loop();
	void AppMain();

private:
	HWND	m_hWnd;


//以降はDirectX関係.
public:
	HRESULT Init3D();
	void DestroyD3D();
	void Render();

	/************シーン毎の処理*************/

	void Title();		//タイトリ処理.
	void Main();		//メイン処理.
	void Clear();		//クリア処理.
	void Over();		//オーバー処理.

	void P_ACT();		//プレイヤー処理.
	void E_ACT();		//雑魚処理.
	void B_ACT();		//ボス処理.

	/***************************************/
	
	/***********************読込系****************************/

	void LoadModel();		//ﾓﾃﾞﾙ読込関数.
	void LoadSound();		//音源読込関数.
	void LoadPosition();	//足場座標読込関数.

	/**********************************************************/

	/*****************足場系*******************/
	
	void ScaffSqu();
	void UpDown();
	void SlowDown();

	/******************************************/

	/*****************障害物系*******************/

	void SawControll();
	void NeedleControll();
	void WallControll();

	/********************************************/





	//カメラ関数.
	void Camera();
	//ﾌﾟﾛｼﾞｪｸｼｮﾝ関数.
	void Proj();
	/*******************当たり判定関連*************************/
	//ｽﾌｨｱ作成.
	HRESULT InitSphere(clsDx9Mesh* pMesh);
	//●衝突判定.
	bool Collision(clsDx9Mesh* pAttacker, clsDx9Mesh* pTarget);
	//ﾊﾞｳﾝﾃﾞｨﾝｸﾞﾎﾞｯｸｽ作成.
	HRESULT	InitBBox(clsDx9Mesh* pMesh);
	//■衝突判定.
	bool BBoxCollision(clsDx9Mesh* pAttacker, clsDx9Mesh* pTarget);

	/**********************************************************/

	


private:
	ID3D11Device*	m_pDevice;
	ID3D11DeviceContext* m_pContext;
	IDXGISwapChain*		 m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBuffer_TexRTV;
	ID3D11Texture2D*	m_pBackBuffer_DSTex;
	ID3D11DepthStencilView*	m_pBackBuffer_DSTexDSV;




	ID3D11SamplerState*	m_pSampleLinear;	//ﾃｸｽﾁｬのｻﾝﾌﾟﾗ ﾃｸｽﾁｬに各種ﾌｨﾙﾀをかける.
	ID3D11ShaderResourceView* m_pTexture;	//ﾃｸｽﾁｬ.
	CAMERA			m_Camera;	//カメラ情報.
	D3DXMATRIX		m_mView;	//ビュー(カメラ行列).
	D3DXMATRIX		m_mProj;	//プロジェクション行列.
	D3DXVECTOR3		m_vLight;	//ﾗｲﾄの方向.
	D3DXVECTOR3		m_acc;		//加速
	clsDx9Mesh*		m_pClsDx9Mesh;//Dx9ﾒｯｼｭｸﾗｽ.
	
	COLLSIONBOX		CollB;

	clsDebugText*	m_pDebugText;
	clsSprite2D*	m_pSprite2D;
	clsDx9Mesh*		m_pSphere;	//ｽﾌｨｱｵﾌﾞｼﾞｪｸﾄ.

	/****************モデル宣言*****************/
	clsDx9Mesh*		m_pGround;
	clsDx9Mesh*		m_aGround[G_MAX];
	clsDx9Mesh*		m_pPlayer;

	clsDx9Mesh*		m_aStandard;

	clsDx9Mesh*		m_aScafSqu;
	clsDx9Mesh*		m_aScafSquM;
	clsDx9Mesh*		m_aScafUp;
	clsDx9Mesh*		m_aScafDown;
	clsDx9Mesh*		m_aScaf_Paste;
	clsDx9Mesh*		m_aScaf_PasteOn;

	clsDx9Mesh*		m_aSaw;
	clsDx9Mesh*		m_aSawCover;
	clsDx9Mesh*		m_aNeedle;
	clsDx9Mesh*		m_aNeedleSta;
	clsDx9Mesh*		m_aWallMove;
	/********************************************/


	/*****************音源宣言*******************/
	clsSound		m_sTitle;

	/********************************************/

};