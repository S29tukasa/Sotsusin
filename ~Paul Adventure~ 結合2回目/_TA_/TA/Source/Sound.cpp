#include "Sound.h"


clsSound::clsSound()
{
	ZeroMemory(this, sizeof(clsSound));
}

clsSound::~clsSound()
{
}

//音声ファイルを開く
bool clsSound::Open(LPCTSTR sFileName, char *sAlias, HWND hWnd){
	m_hWnd = hWnd;	//メンバーウィンドウハンドルの設定
	strcpy_s(m_sAlias, sizeof(m_sAlias), sAlias); //メンバーエイリアスの設定


	//コマンド
	char command[STR_BUFF_MAX] = "open ";

	//文字列の結合
	strcat_s(command, sizeof(command), sFileName);
	strcat_s(command, sizeof(command), " alias ");
	strcat_s(command, sizeof(command), m_sAlias);
	
	//コマンド："open [sFileName] alias [sAlias]"
	if(mciSendString(command, NULL, 0, m_hWnd) == 0){
		return true;
	}

	return false;
}

//音声ファイルを閉じる
bool clsSound::Close(){
	//コマンド
	char command[STR_BUFF_MAX] = "close ";

	//文字列の結合
	strcat_s(command, sizeof(command), m_sAlias);
	
	//コマンド："close [sAlias]"
	if(mciSendString(command, NULL, 0, m_hWnd) == 0){
		return true;
	}

	return false;
}

//音声ファイルを再生
bool clsSound::Play(bool bNotify){
	//コマンド
	char command[256] = "play ";

	//文字列の結合
	strcat_s(command, sizeof(command), m_sAlias);
	if(bNotify)
		//notify : 処理終了したときにhWndにMM_MCINOTIFYメッセージをポストする
		strcat_s(command, sizeof(command), " notify");
	
	//コマンド："play [sAlias]" または "play [sAlias] notify"
	if(mciSendString(command, NULL, 0, m_hWnd) == 0){
		return true;
	}

	return false;
}

//音声ファイルを停止
bool clsSound::Stop(){
	//コマンド
	char command[256] = "stop ";

	//文字列の結合
	strcat_s(command, sizeof(command), m_sAlias);
	
	//コマンド："stop [sAlias]"
	if(mciSendString(command, NULL, 0, m_hWnd) == 0){
		return true;
	}

	return false;
}


//音声のステータス
//　sStatusの配列数は256以下にすること
//　ただし、状態の取得する場合は、再生時に「notify」を設定し、
//　ウィンドウハンドルにメッセージを送っておく必要がある
bool clsSound::GetStatus(char *sStatus, char *sAlias, HWND hWnd){
	//コマンド
	char command[256] = "status ";

	//文字列の結合
	strcat_s(command, sizeof(command), sAlias);
	strcat_s(command, sizeof(command), " mode");

	//コマンド："status [sAlias] mode"
	if(mciSendString(command, sStatus, 256, hWnd) == 0){
		return true;
	}

	return false;
}

//音声は停止していることを確認
bool clsSound::IsStopped(){

	char sStatus[STR_BUFF_MAX] = "";

	if(GetStatus(sStatus, m_sAlias, m_hWnd)){ //状態の取得
		if(strcmp(sStatus, "stopped") == 0) //状態の文字列を比較
			return true;
	}
	return false;

}

//音声は再生していることを確認
bool clsSound::IsPlaying(){
	char sStatus[STR_BUFF_MAX] = "";

	if(GetStatus(sStatus, m_sAlias, m_hWnd)){ //状態の取得
		if(strcmp(sStatus, "playing") == 0) //状態の文字列を比較
			return true;
	}
	return false;
}

//音声の再生位置を初期化する
bool clsSound::SeekToStart(){
	//コマンド
	char command[STR_BUFF_MAX] = "seek ";

	//文字列の結合
	strcat_s(command, sizeof(command), m_sAlias);
	strcat_s(command, sizeof(command), " to start");
	
	//コマンド："seek [sAlias] to start"
	if(mciSendString(command, NULL, 0, m_hWnd) == 0){
		return true;
	}

	return false;
}


bool clsSound::SetVolume(int iVolume)
{
	
	//音量の範囲:0～1k
	if(iVolume<0)	iVolume=0;
	if(iVolume>=1000)	iVolume=1000;

	//音量を文字列に変換
	char sVolume[8] = "";
	wsprintf(sVolume,"%d",iVolume);

	//コマンド
	char command[STR_BUFF_MAX] = "setaudio ";

	//文字列の結合
	strcat_s(command, sizeof(command), m_sAlias);
	strcat_s(command, sizeof(command), " volume to ");
	strcat_s(command, sizeof(command), sVolume);


	//コマンド:"setaudio [Alias] volume to [Volume]"
	if(mciSendString(command,NULL,0,m_hWnd)==0)
	{
		return true;
	}
	return false;

};
