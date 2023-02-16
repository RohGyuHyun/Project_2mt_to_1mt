// AreaCamControl.h: interface for the CAreaCamControl class.
#include "mil.h"

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREACAMCONTROL_H__0E7126F1_A258_423A_8E1C_4920C8594D2D__INCLUDED_)
#define AFX_AREACAMCONTROL_H__0E7126F1_A258_423A_8E1C_4920C8594D2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define     AREACAM_MAX_BD_NUM		1
#define		AREACAM_MAX_CAM_NUM		1
#define     AREACAM_MAX_IMG_NUM		12
#define     AREACAM_MAX_BUF_NUM		2
#define		ARECAM_WB_COEFFICIENTS_NUM 3

class CAreaCamControl  
{
public:
	BOOL Open(short fr_idx, short ch, BOOL isColor = FALSE);
	BOOL Grab(short cam_idx, short buf_idx, short img_idx, Hobject *halcon_object, BOOL isColor = FALSE, Hobject *halcon_Redobject = NULL, Hobject *halcon_Greenobject = NULL, Hobject *halcon_Blueobject = NULL);//상하부 카메라
	BOOL Close();

	CAreaCamControl();
	virtual ~CAreaCamControl();

private:

	MIL_ID MilApplication;
	MIL_ID MilSystem[AREACAM_MAX_BD_NUM];
	MIL_ID MilDigitizer[AREACAM_MAX_CAM_NUM];

	MIL_ID MilGrabBuf[AREACAM_MAX_CAM_NUM][AREACAM_MAX_BUF_NUM][AREACAM_MAX_IMG_NUM];
	MIL_ID MilBayerBuf[AREACAM_MAX_CAM_NUM][AREACAM_MAX_BUF_NUM][AREACAM_MAX_IMG_NUM];
	MIL_ID MilBayerBuf_R[AREACAM_MAX_CAM_NUM][AREACAM_MAX_BUF_NUM][AREACAM_MAX_IMG_NUM];
	MIL_ID MilBayerBuf_G[AREACAM_MAX_CAM_NUM][AREACAM_MAX_BUF_NUM][AREACAM_MAX_IMG_NUM];
	MIL_ID MilBayerBuf_B[AREACAM_MAX_CAM_NUM][AREACAM_MAX_BUF_NUM][AREACAM_MAX_IMG_NUM];

	MIL_ID MilWBCoefficients[AREACAM_MAX_CAM_NUM];
	float WBCoefficients[AREACAM_MAX_CAM_NUM][ARECAM_WB_COEFFICIENTS_NUM];
};

#endif // !defined(AFX_AREACAMCONTROL_H__0E7126F1_A258_423A_8E1C_4920C8594D2D__INCLUDED_)
