//グローバル変数.
//ﾃｸｽﾁｬはﾚｼﾞｽﾀｰ t(n).
Texture2D		g_texColor	:	register(t0);

//ｻﾝﾌﾟﾗはﾚｼﾞｽﾀ　s(n).
SamplerState	g_samLiner	:	register(s0);

//コンスタントバッファ.
cbuffer	global	:	register(b0)
{
	matrix	g_mW			: packoffset(c0);
	float	g_fViewPortW	: packoffset(c4);
	float	g_fViewPortH	: packoffset(c5);
	float	g_fAlpha		: packoffset(c6);
}

//頂点シェーダの出力パラメータ.
struct VS_OUTPUT
{
	float4	Pos	:	SV_Position;
	float2	UV  :	TEXCOORD;
};

//頂点シェーダ.
VS_OUTPUT VS_Main(float4 Pos : POSITION, float2 UV : TEXCOORD)
{
	//初期化.
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, g_mW);

	//スクリーン座標に合わせる計算.
	output.Pos.x = (output.Pos.x / g_fViewPortW) * 2.0f - 1.0f;
	output.Pos.y = 1.0f - (output.Pos.y / g_fViewPortH) * 2.0f;

	output.UV = UV;

	return output;
}

//ﾋﾟｸｾﾙｼｪｰﾀﾞ.
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	float4 color = g_texColor.Sample(g_samLiner, input.UV);
	color.a *= g_fAlpha;	//アルファ値を掛け合わせる(透過事故を防ぐため).
	return color;
}