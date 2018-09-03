#ifndef SPRITE_D_H
#define SPRITE_D_H


#include "cCommon.h"

#define ALIGN16 _declspec(align(16))

struct SPRITE_STATE
{
	float	Width;		//幅.
	float	Height;		//高さ.
	float	U;			//U座標.
	float	V;			//V座標.
};
class clsSprite2D :public clsCommon
{
public://public 外部からｱｸｾｽ可能.

	D3DXVECTOR3		m_vPos;	//位置.

	int				m_AnimCount;	//カウンタ
	bool			m_DispFlag;		//表示フラグ

	float			m_Alpha;		//ｱﾙﾌｧ値(最大値は1.0).

	clsSprite2D();	//ｺﾝｽﾄﾗｸﾀ.
	~clsSprite2D();	//ﾃﾞｽﾄﾗｸﾀ.

	HRESULT Init(ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11,
				LPSTR fileName,const SPRITE_STATE& ss);
	HRESULT InitShader();
	HRESULT InitModel(float Width,float Height,float U,float V);
	//テクスチャ作成.
	HRESULT	CreateTexture(LPSTR fileName, ID3D11ShaderResourceView** pTexture);

	void Render();


	//-----------------------------------------------
	//	構造体.
	//-----------------------------------------------
	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｱﾌﾟﾘ側の定義(Simple.hlsl).
	//※ｼｪｰﾀﾞ内のｺﾝｽﾀﾝﾄﾊﾞｯﾌｧと一致している必要あり.
	struct SPRITESHADER_CBUFFER
	{
		ALIGN16 D3DMATRIX	mW;					//ワールド行列.
		ALIGN16 float		fViewPortWidth;		//ビューポート幅.
		ALIGN16 float		fViewPortHeight;	//ビューポート高さ
		ALIGN16 float		fAlpha;				//アルファ値(透過で使用する).

	};
	//頂点の構造体.
	struct SpriteVertex
	{
		D3DXVECTOR3 vPos;	//位置.
		D3DXVECTOR2 vTex;	//ﾃｸｽﾁｬ座標(UV座標).
	};

private:

	ID3D11VertexShader* m_pVertexShader;	//頂点ｼｪｰﾀﾞ.
	ID3D11InputLayout*	m_pVertexLayout;	//頂点ﾚｲｱｳﾄ.
	ID3D11PixelShader*	m_pPixelShader;		//ﾋﾟｸｾﾙｼｪｰﾀﾞ.
	ID3D11Buffer*		m_pConstantBuffer;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
	ID3D11Buffer*		m_pVertexBuffer;	//頂点ﾊﾞｯﾌｧ.

	ID3D11SamplerState*	m_pSampleLinear;	//ﾃｸｽﾁｬのｻﾝﾌﾟﾗ.
	// ﾃｸｽﾁｬに各種ﾌｨﾙﾀをかける.
	ID3D11ShaderResourceView* m_pTexture;	//ﾃｸｽﾁｬ.


};





#endif