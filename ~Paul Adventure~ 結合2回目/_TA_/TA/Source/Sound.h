#ifndef __SOUND__
#define __SOUND__

#include <Windows.h>

#pragma comment (lib, "winmm.lib")	//音声関係

//文字列バッファのサイズ
#define STR_BUFF_MAX	256


class clsSound
{
public:
	clsSound();
	~clsSound();

	//音声ファイルを開く
	bool Open(LPCTSTR sFileName, char *sAlias, HWND hWnd);
	//音声ファイルを閉じる
	bool Close();
	//音声ファイルを再生
	bool Play( bool bNotify = false);
	//音声ファイルを停止
	bool Stop();

	//音声のステータス
	bool GetStatus(char *sStatus, char *sAlias, HWND hWnd);
	//音声は停止していることを確認
	bool IsStopped();
	//音声は再生していることを確認
	bool IsPlaying();
	//音声の再生位置を
	bool SeekToStart();

	//音量の調整
	bool SetVolume(int iVolume);
private:
	HWND	m_hWnd;		//ウィンドウハンドル
	char	m_sAlias[STR_BUFF_MAX];	//エイリアス
};

#endif //__SOUND__
