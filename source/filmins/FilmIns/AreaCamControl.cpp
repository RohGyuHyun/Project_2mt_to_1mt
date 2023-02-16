// AreaCamControl.cpp: implementation of the CAreaCamControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilmIns.h"
#include "AreaCamControl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAreaCamControl::CAreaCamControl()
{
	MilApplication = NULL;

	int i, j, k;

	for(i = 0; i < AREACAM_MAX_CAM_NUM; i++)
	{
		MilWBCoefficients[i] = NULL;
		MilDigitizer[i] = NULL;
		MilSystem[i] = NULL;

		for(j = 0; j < AREACAM_MAX_BUF_NUM; j++)
		{
			for(k = 0; k < AREACAM_MAX_IMG_NUM; k++)
			{
				if(NULL != MilGrabBuf[i][j][k])
				{
					MilBayerBuf_R[i][j][k] = NULL;
					MilBayerBuf_G[i][j][k] = NULL;
					MilBayerBuf_B[i][j][k] = NULL;
					MilBayerBuf[i][j][k] = NULL;
					MilGrabBuf[i][j][k] = NULL;
					
				}
			}

		}
	}
}

CAreaCamControl::~CAreaCamControl()
{

}

BOOL CAreaCamControl::Close()
{
	int i, j, k;

	for(i = 0; i < AREACAM_MAX_CAM_NUM; i++)
	{
		if(MilWBCoefficients[i] != NULL)
			MbufFree(MilWBCoefficients[i]);
	}

	for(i = 0; i < AREACAM_MAX_CAM_NUM; i++)
	{
		for(j = 0; j < AREACAM_MAX_BUF_NUM; j++)
		{
			for(k = 0; k < AREACAM_MAX_IMG_NUM; k++)
			{
				if(NULL != MilGrabBuf[i][j][k])
				{
					if( (1 == i) && (0 < k) )
					{
						break;
					}

					if(MilBayerBuf_R[i][j][k] != NULL){
						MbufFree(MilBayerBuf_R[i][j][k]);	
						MilBayerBuf_R[i][j][k] = NULL;
					}
					if(MilBayerBuf_G[i][j][k] != NULL){
						MbufFree(MilBayerBuf_G[i][j][k]);	
						MilBayerBuf_G[i][j][k] = NULL;
					}
					if(MilBayerBuf_B[i][j][k] != NULL){
						MbufFree(MilBayerBuf_B[i][j][k]);	
						MilBayerBuf_B[i][j][k] = NULL;
					}

					if(MilBayerBuf[i][j][k] != NULL){
						MbufFree(MilBayerBuf[i][j][k]);	
						MilBayerBuf[i][j][k] = NULL;
					}

					if(MilGrabBuf[i][j][k] != NULL){
						MbufFree(MilGrabBuf[i][j][k]);	
						MilGrabBuf[i][j][k] = NULL;
					}
				}
			}
		}
	}

	for(i = 0; i < AREACAM_MAX_CAM_NUM; i++)
	{
		if(NULL != MilDigitizer[i])
		{
			MdigFree(MilDigitizer[i]);
			MilDigitizer[i] = NULL;
		}
	}

	for(i = 0; i < AREACAM_MAX_BD_NUM; i++)
	{
		if(NULL != MilSystem[i])
		{
			MsysFree(MilSystem[i]);
			MilSystem[i] = NULL;
		}
	}

	if(NULL != MilApplication)
	{
		MappFree(MilApplication);
		MilApplication = NULL;
	}

	return TRUE;
}

//BOOL Open(int devNum, int ch);
BOOL CAreaCamControl::Open(short fr_idx, short ch, BOOL isColor)
{
	MIL_INT64 width[AREACAM_MAX_BUF_NUM], height[AREACAM_MAX_BUF_NUM];
	int i, j, k;
	//MIL_INT SizeBand;

	//Matrox/////////////////////////////////////
	//Application
	MappAlloc(M_DEFAULT, &MilApplication); 

	//Framegrabber
	for(i = 0; i < AREACAM_MAX_BD_NUM; i++)
	{
		MsysAlloc(M_SYSTEM_SOLIOS, fr_idx, M_DEFAULT, &MilSystem[i]);

	}
	
	//Camera별 해상도 설정 
	width[0] = WIDTH;
	height[0] = HEIGHT;
	char temp[100];

	//Digitizer allocation
	for(i = 0; i < AREACAM_MAX_CAM_NUM; i++)
	{
		//MdigAlloc(MilSystem[i], (long)(0), "DCF\\RMV-16050-small.dcf", M_DEFAULT, &MilDigitizer[i]);
		
		//20150223 by
		sprintf(temp, "%s_%d_%d.dcf", "D:\\Project\\dcf\\RMV-16050-small", fr_idx, ch);
		//sprintf(temp, "%s_%d_%d.dcf", "D:\\Project\\dcf\\71M_TPE_medium(40Mhz)_8bit_4tap", fr_idx, ch);
		MdigAlloc(MilSystem[i], (long)ch, temp, M_DEFAULT, &MilDigitizer[i]);
		
		//HW Trigger Grab Mode 설정시
		MdigControl(MilDigitizer[i], M_GRAB_MODE, M_ASYNCHRONOUS); 
		MdigControl(MilDigitizer[i], M_GRAB_EXPOSURE_SOURCE, M_SOFTWARE);
		MdigControl(MilDigitizer[i], M_GRAB_TIMEOUT, M_INFINITE);
		
		//SizeBand = MdigInquire(MilDigitizer[i], M_SIZE_BAND, M_NULL);
		
		//Get image size & set grab buffer
		MdigInquire(MilDigitizer[i], M_SIZE_X, &width[i]);
		MdigInquire(MilDigitizer[i], M_SIZE_Y, &height[i]);	
	}
	
	for(i = 0; i < AREACAM_MAX_CAM_NUM; i++)
	{
		for(j = 0; j < AREACAM_MAX_BUF_NUM; j++)
		{
			for(k = 0; k < AREACAM_MAX_IMG_NUM; k++)
			{
				if(isColor)
				{
					MbufAllocColor(MilSystem[i], 1, width[i], height[i], 8L+M_UNSIGNED, M_IMAGE+M_GRAB, &MilGrabBuf[i][j][k]);	
					MbufClear(MilGrabBuf[i][j][k], M_NULL);
					MbufAllocColor(MilSystem[i], 3, width[i], height[i], 8L+M_UNSIGNED, M_IMAGE, &MilBayerBuf[i][j][k]);
					MbufClear(MilBayerBuf[i][j][k], M_NULL);

					MbufChildColor(MilBayerBuf[i][j][k], M_RED,   &MilBayerBuf_R[i][j][k]);
					MbufChildColor(MilBayerBuf[i][j][k], M_GREEN, &MilBayerBuf_G[i][j][k]);
					MbufChildColor(MilBayerBuf[i][j][k], M_BLUE,  &MilBayerBuf_B[i][j][k]);
				}
				else
				{
					MbufAlloc2d(MilSystem[i], width[i], height[i], 8L+M_UNSIGNED, M_IMAGE+M_GRAB, &MilGrabBuf[i][j][k]);
					MbufClear(MilGrabBuf[i][j][k], M_NULL);
				}
			}
		}
	}

	for(i = 0; i < AREACAM_MAX_CAM_NUM; i++)
	{
		if(isColor)
		{
			//White Balance 초기화
			MbufAlloc1d(MilSystem[i], 3, 32+M_FLOAT, M_ARRAY, &MilWBCoefficients[i]);

			WBCoefficients[i][0] = (float)1.30513;
			WBCoefficients[i][1] = (float)0.85743;
			WBCoefficients[i][2] = (float)1.32091;

			MbufPut(MilWBCoefficients[i], (void *)&WBCoefficients[i][0]);
		}
	}

	return TRUE;
}
		
BOOL CAreaCamControl::Grab(short cam_idx, short buf_idx, short img_idx, Hobject *rotate_halcon_object, BOOL isColor, 
						   Hobject *halcon_Redobject, Hobject *halcon_Greenobject, Hobject *halcon_Blueobject)
{
	MIL_INT64 width, height;
	BYTE *image_ptr;
	Hobject MultiChannelImage;
	//HTuple StartSecond, EndSecond;
	//long m_lGrabTime;

	//Get image size & set grab buffer
	MdigInquire(MilDigitizer[cam_idx], M_SIZE_X, &width);
	MdigInquire(MilDigitizer[cam_idx], M_SIZE_Y, &height);	
		
	MdigGrab(MilDigitizer[cam_idx], MilGrabBuf[cam_idx][buf_idx][img_idx]);	

	//20110509 lee
	//HW Trigger 설정시 
	MdigControl(MilDigitizer[cam_idx], M_GRAB_EXPOSURE+M_TIMER1, M_ACTIVATE);
	MdigGrabWait(MilDigitizer[cam_idx], M_GRAB_END);

	if(isColor)
	{
		BYTE *Rimage_ptr, *Gimage_ptr, *Bimage_ptr;

		//count_seconds(&StartSecond);	
		
		MbufBayer(MilGrabBuf[cam_idx][buf_idx][img_idx], MilBayerBuf[cam_idx][buf_idx][img_idx], MilWBCoefficients[cam_idx], M_BAYER_GR);//M_BAYER_BG);

		//count_seconds(&EndSecond);

		//EndSecond = EndSecond - StartSecond;
		//m_lGrabTime = (long)(((double)EndSecond[0]) * 1000);
		
		MbufInquire(MilBayerBuf_R[cam_idx][buf_idx][img_idx], M_HOST_ADDRESS, &Rimage_ptr);
		MbufInquire(MilBayerBuf_G[cam_idx][buf_idx][img_idx], M_HOST_ADDRESS, &Gimage_ptr);
		MbufInquire(MilBayerBuf_B[cam_idx][buf_idx][img_idx], M_HOST_ADDRESS, &Bimage_ptr);

		gen_image1_extern(halcon_Redobject, "byte", width, height, (Hlong)Rimage_ptr, (Hlong) 0);
		gen_image1_extern(halcon_Greenobject, "byte", width, height, (Hlong)Gimage_ptr, (Hlong) 0);
		gen_image1_extern(halcon_Blueobject, "byte", width, height, (Hlong)Bimage_ptr, (Hlong) 0);
	
		compose3(*halcon_Redobject, *halcon_Greenobject, *halcon_Blueobject, &MultiChannelImage);
		copy_image(MultiChannelImage, rotate_halcon_object);
	}	
	else
	{
		MbufInquire(MilGrabBuf[cam_idx][buf_idx][img_idx], M_HOST_ADDRESS, &image_ptr);
		gen_image1_extern(rotate_halcon_object, "byte", width, height, (Hlong)image_ptr, (Hlong)0);
	}

	return TRUE;
}
