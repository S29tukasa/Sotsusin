#ifndef POSITION_H
#define POSITION_H

#pragma warning(disable:4005)




//LoadModelのm_vPosはここの定数を使用しているため.
//結合の際、必ず変更してビルドすること.
//後半のソース改良の部分で無くす可能性有.
//Default Model Position.
const float GroundX =  2.7f;
const float GroundY = -0.5f;
const float GroundZ = -5.7f;

const float PlayerX = GroundX;
const float PlayerY =  0.0f;
const float PlayerZ = -8.0f;


/***********①***********/
int scaffoldCntSqu = 0;
int Italy = L3;

const float scaffoldSquX = 1.0f;
const float scaffoldSquY = 0.1f;
const float scaffoldSquZ = 7.0f;

const float scaffoldSqumX = 4.4f;
const float scaffoldSqumY = 4.2f;
const float scaffoldSqumZ = 11.0f;


/***********②***********/
int UpDownCnt = 0;
int France = S35;
const float upX = 0.9f;
const float upY = -0.9f;
const float upZ = 24.2f;

const float downX =4.5;
const float downY = -4.8f;
const float downZ = 24.2f;

/***********③***********/
int SlowDownCnt = 0;
int FishBlock = RETURN;
bool ColorChange = false;

/***********(1)************/
int SawControllCnt = 0;
int Germany = MARK1;
const float	SawX = -0.7f;
const float	SawY = 4.3f;
const float SawZ = 15.55f;
const float SawMovingX = 5.7f;
const float SawCovX = SawX -0.1;
const float SawCovY = SawY - 0.85f;
const float SawCovZ = SawZ - 1.45f;

/***********(2)************/
int NeedleControllCnt = 0;
int American=M4SHA;
const float NeedleX = 2.55f;
const float NeedleY = 2.4f;
const float NeedleZ = 33.0f;
const float NeedleStaX = NeedleX;
const float NeedleStaY = NeedleY + 1.0f;
const float NeedleStaZ = NeedleZ;


/***********(3)***********/
int PushCnt = 0;
float PushSpeed = 0.00f;
bool PushStanby = false;
int WallCnt = 0;
#endif