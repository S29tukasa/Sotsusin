#ifndef CCOMMON_H
#define	CCOMMON_H

#include "Global.h"

//共通クラス.
class clsCommon
{
public:
	clsCommon(){};
	virtual ~clsCommon(){};
	//クラス名(){}とすると中の処理をcppで書かずに済む.
	//継承させるものにはvirtualを付ける(仮想関数).
	
	//透過(ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ)設定の切り替え.
	void SetBlend(bool bFlag);

	//変数.
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11BlendState*		m_pBlendState;

};

#endif //#ifndef CCOMMON_H
