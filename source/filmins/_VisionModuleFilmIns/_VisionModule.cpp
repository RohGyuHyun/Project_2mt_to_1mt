// _VisionModule.cpp: implementation of the C_VisionModule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "include/_FuncModuleBasic.h"
#include "include/_FuncModuleHal.h"
#include "_VisionModuleFilmIns.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Vision Data
//////////////////////////////////////////////////////////////////////
C_VisionData::C_VisionData()
{

}

C_VisionData::~C_VisionData()
{

}

/////////////////////////////////////////////////
//Inspection function
//
BOOL C_VisionModule::FindPuchingChip(Hlong win, Hobject *image, Hobject sr_region, RECT ins_region, RECT ins_chip_region, 
									 int pf_edit_thresh, int pf_edit_tol, int pf_pitch,
									 int min_punch_area, int max_punch_area, int bur_tol, int pos_error, int pos_error2,
									 int pf_Count, 
									 int edit_thresh, int edit_tol,
									 BOOL *PuchErr, Hobject *rslt_region, Hlong *rslt_area)
{	//20100115LSS
	Hobject ConvxRegion2, TransRegion, ConvxRegion, Region, ImageReduced, ConnectedRegions, SelectedRegions;
	HTuple AreaCheck1, AreaCheck2, Num, CRow0,CCol0,CRow1,CCol1, Row, Col, Row2, Col2, Dist; 
	Hlong DiffCheckArea = 0;
	Herror herr;
	Hlong chip_cen_posx, chip_cen_posy, Area;
	Hlong wd, ht;
	HTuple _area, _row, _col;
	Hobject SelectedRegionsCircle;

	int thresh;
	double CRow,CCol;

	CString status;
	CRect rect;

	BOOL is_circle = FALSE;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_21!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	rect = ins_region;

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	thresh = pf_edit_thresh;
	if(0 > thresh)
	{
		thresh = 0;
	}
	else if(254 < thresh)//CHOKI20110603
	{
		thresh = 254;
	}

	*PuchErr = FALSE;


	//Punching inspection////////////////////////
	erosion_rectangle1(Region, &Region, 100, 3);

	reduce_domain(*image,Region,&ImageReduced);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	set_check("~give_error");

	//디버그 필요 중요++
	switch(m_iVisIdx)
	{
	case VISION_CAM_TOP1:
		herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255));
		break;
	case VISION_CAM_TOP2:
		herr = threshold(ImageReduced,&Region,HTuple(0), HTuple(thresh));
		break;
	case VISION_CAM_BOTTOM:
		herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255)); 
		break;
	default:
		AfxMessageBox("C_VisionModule::FindPuchingChip() Failed ! Unknown Vision Index.");
		return FALSE;
		break;
	}
	/*
	switch(m_iVisIdx)
	{
	case VISION_CAM_TOP1:
		herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255));
		break;
	case VISION_CAM_BOTTOM1:
		herr = threshold(ImageReduced,&Region,HTuple(0), HTuple(thresh));
		break;
	case VISION_CAM_TOP2:
		herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255)); 
		break;
	case VISION_CAM_BOTTOM2:
		herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255)); 
		break;
	default:
		AfxMessageBox("C_VisionModule::FindPuchingChip() Failed ! Unknown Vision Index.");
		return FALSE;
		break;
	}
	*/

	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_19!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);

	//20150223 by
	//원형 펀치 검사 //20100725 lee
	select_shape(ConnectedRegions,&SelectedRegions, HTuple("width").Concat("height"), HTuple("and"), HTuple(150).Concat(150), HTuple(300).Concat(300));
	//71M 원형펀치 검사 사이즈 up 20150127 by
	//select_shape(ConnectedRegions,&SelectedRegions, HTuple("width").Concat("height"), HTuple("and"), HTuple(300).Concat(300), HTuple(800).Concat(800));
	select_shape(SelectedRegions,&SelectedRegionsCircle,HTuple("circularity"),HTuple("and"),HTuple(0.7),HTuple(1.0)); //20110530 lee 0.9->0.7
	count_obj(SelectedRegionsCircle, &Num);
	if(1 == (Hlong)Num[0])  //한개 이상 있다? 그럼 원형 펀치!! 이건 위치 않보고 그냥 펀치!
	{

		//원형 Punch가 있는데 사각 Punch가 없으면 우선 불량..향후 원형만 있을 경우 조치 필요함. //20100811 lee
		//사각 펀치 검사 
		select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple(max_punch_area));
		count_obj(SelectedRegions, &Num);

		if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
		{
			//원형 펀치만 있을때

			*PuchErr = TRUE;  //원형 Punch긴 하나 사각 Punch가 없으므로 편집 불량으로 간다. //20100811 lee

			union2(SelectedRegionsCircle, *rslt_region, rslt_region);

			return TRUE;  //Punch 면 TRUE
		}
		else
		{
			//union2(SelectedRegionsCircle, *rslt_region, rslt_region);
			is_circle = TRUE;
			difference(ConnectedRegions, SelectedRegionsCircle, &ConnectedRegions);//사각펀치가 있으면 원형영역은 제거
			//Circle Punching
			//return TRUE;    //Punch 면 TRUE
		}
		
		//return TRUE;
	}
	//원형 펀치 검사 //20100725 lee

	//사각 펀치 검사 
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple("max"));

	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(SelectedRegions, win);
	}

	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		return FALSE;  //Punch 아니며 FALSE
	}


	//////////////////////////////////////////////////////////////////////////////////////////
	//PF위치기준으로 편집 불량 검출
	//
	int high_threshold, low_threshold;
	Hobject UP_PFRegion, DN_PFRegion, UP_ImageReduced, DN_ImageReduced, UP_RsltRegion, DN_RsltRegion;
	Hobject UP_DN_DiffRegion;
	HTuple UP_Row0 ,UP_Col0, UP_Row1, UP_Col1;
	HTuple DN_Row0 ,DN_Col0, DN_Row1, DN_Col1;
	Hlong num;

	gen_rectangle1(&UP_DN_DiffRegion, ins_region.top, 
								 ins_chip_region.left - ((ins_chip_region.right - ins_chip_region.left) * 0.5), 
								 ins_region.bottom, 
								 ins_chip_region.right + ((ins_chip_region.right - ins_chip_region.left) * 0.5));

	gen_rectangle1(&UP_PFRegion, ins_region.top, 
								 ins_region.left, 
								 ins_region.top + 200, 
								 ins_region.right);

	gen_rectangle1(&DN_PFRegion, ins_region.bottom - 200, 
								 ins_region.left, 
								 ins_region.bottom, 
								 ins_region.right);
	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(UP_PFRegion, win);
		disp_obj(DN_PFRegion, win);
	}
	
	difference(UP_PFRegion, UP_DN_DiffRegion, &UP_PFRegion);
	difference(DN_PFRegion, UP_DN_DiffRegion, &DN_PFRegion);
	reduce_domain(*image, UP_PFRegion, &UP_ImageReduced);
	reduce_domain(*image, DN_PFRegion, &DN_ImageReduced);

	high_threshold = edit_thresh;
	low_threshold = high_threshold - 10;
	if(0 > low_threshold)
	{
		low_threshold = 1;
		high_threshold = 10;
	}
	else if(255 < low_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	else if(0 > high_threshold)
	{
		low_threshold = 1;
		high_threshold = 10;
	}
	else if(255 < high_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}

	threshold(UP_ImageReduced, &UP_RsltRegion, HTuple(pf_edit_thresh), HTuple(255));
	closing_circle(UP_RsltRegion, &UP_RsltRegion,HTuple(3.5));
	connection(UP_RsltRegion, &UP_RsltRegion);
	select_shape(UP_RsltRegion, &UP_RsltRegion, HTuple("width"), HTuple("and"), HTuple(30), HTuple("max"));
	union1(UP_RsltRegion, &UP_RsltRegion);
	closing_circle(UP_RsltRegion, &UP_RsltRegion, HTuple(10.5));

	threshold(DN_ImageReduced, &DN_RsltRegion, HTuple(pf_edit_thresh), HTuple(255));
	closing_circle(DN_RsltRegion, &DN_RsltRegion,HTuple(3.5));
	select_shape(DN_RsltRegion ,&DN_RsltRegion, HTuple("width"), HTuple("and"), HTuple(30), HTuple("max"));
	union1(DN_RsltRegion, &DN_RsltRegion);
	closing_circle(DN_RsltRegion, &DN_RsltRegion, HTuple(10.5));

	smallest_rectangle1(UP_RsltRegion, &UP_Row0, &UP_Col0, &UP_Row1, &UP_Col1);
	smallest_rectangle1(DN_RsltRegion, &DN_Row0, &DN_Col0, &DN_Row1, &DN_Col1);

	num = 0;
	num = (Hlong)UP_Row0.Num();
	if(0 < num)
	{
		gen_rectangle1(&UP_PFRegion, (Hlong)UP_Row1[0] + 15,//25, 
									 ins_region.left + 475,
									 (Hlong)UP_Row1[0] + 17,//35, 
									 ins_region.right - 475);
	}
	else
	{
		union2(UP_PFRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}
	
	num = 0;
	num = (Hlong)DN_Row0.Num();
	if(0 < num)
	{
		gen_rectangle1(&DN_PFRegion, (Hlong)DN_Row0[0] - 17,//35, 
								 ins_region.left + 475,
								 (Hlong)DN_Row0[0] - 15,//25, 
								 ins_region.right - 475);
	}
	else
	{
		union2(DN_PFRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}
	
	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(UP_PFRegion, win);
		disp_obj(DN_PFRegion, win);
	}

	reduce_domain(*image, UP_PFRegion, &UP_ImageReduced);
	reduce_domain(*image, DN_PFRegion, &DN_ImageReduced);

	hysteresis_threshold(UP_ImageReduced, &UP_RsltRegion, HTuple(low_threshold), HTuple(high_threshold), HTuple(30));
	hysteresis_threshold(DN_ImageReduced, &DN_RsltRegion, HTuple(low_threshold), HTuple(high_threshold), HTuple(30));

	closing_circle(UP_RsltRegion, &UP_RsltRegion, HTuple(3.5));
	connection(UP_RsltRegion, &UP_RsltRegion);
	select_shape(UP_RsltRegion ,&UP_RsltRegion, HTuple("height"), HTuple("and"), HTuple(2), HTuple("max"));
	select_shape(UP_RsltRegion ,&UP_RsltRegion, HTuple("width"), HTuple("and"), HTuple(edit_tol), HTuple("max"));

	num = 0;
	count_obj(UP_RsltRegion, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(UP_RsltRegion, &UP_RsltRegion);
		union2(UP_RsltRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}

	closing_circle(DN_RsltRegion, &DN_RsltRegion, HTuple(3.5));
	connection(DN_RsltRegion, &DN_RsltRegion);
	select_shape(DN_RsltRegion ,&DN_RsltRegion, HTuple("height"), HTuple("and"), HTuple(2), HTuple("max"));
	select_shape(DN_RsltRegion ,&DN_RsltRegion, HTuple("width"), HTuple("and"), HTuple(edit_tol), HTuple("max"));

	count_obj(DN_RsltRegion, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(DN_RsltRegion, &DN_RsltRegion);
		union2(DN_RsltRegion, *rslt_region, rslt_region);
		*PuchErr = TRUE;
	}

	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(UP_RsltRegion, win);
		disp_obj(DN_RsltRegion, win);
	}

	//
	//
	///////////////////////////////////////////////////////////////////////////////////////////


	//이하 Punch가 있는 경우 
	//punch 중심 위치와 Chip 중심 위치를 확인하여 편차발생 시 불량 
	//area_center(SelectedRegions, &Area, &CRow, &CCol);

	//20150204 by : 펀치가 두 개 이상일 경우 배열처리 해야함.
	area_center(SelectedRegions, &_area, &_row, &_col);
	
	Area = (Hlong)_area[0];
	CRow = (double)_row[0];
	CCol = (double)_col[0];
	
	*rslt_area = (Hlong)Area;

	chip_cen_posx = ins_chip_region.left + abs(ins_chip_region.right - ins_chip_region.left) / 2;
	if(abs(CCol - chip_cen_posx) > pos_error2) //CHOKI20110824
	{	
		//copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
		union2(SelectedRegions, *rslt_region, rslt_region);
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
	}
	chip_cen_posy = ins_chip_region.top + abs(ins_chip_region.bottom - ins_chip_region.top) / 2; 
	if(abs(CRow - chip_cen_posy) > pos_error)
	{	
		//copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
		union2(SelectedRegions, *rslt_region, rslt_region);
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
	}

	//CHOKI20110703 펀치면적기준 초과시 불량 확인
	if((Hlong)Area > (Hlong)max_punch_area) 
	{	
		//copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
		union2(SelectedRegions, *rslt_region, rslt_region);
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
	}

	//CHOKI20120103 형태구분 추가작업
	//Punch burr검사/////////////////////////////
	union1(SelectedRegions, &SelectedRegions);	
	shape_trans(SelectedRegions, &ConvxRegion, "convex");
	shape_trans(SelectedRegions, &TransRegion, "rectangle1");
	erosion_circle(ConvxRegion, &ConvxRegion2, 5.5);
	difference(ConvxRegion2, SelectedRegions, &SelectedRegions);
	connection(SelectedRegions, &SelectedRegions);

	select_shape(SelectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(bur_tol),HTuple("max"));
	
	union1(SelectedRegions, &SelectedRegions);
	count_obj(SelectedRegions, &Num);
	
	if(0 < (Hlong)Num[0]) 
	{
		area_center(ConvxRegion, &AreaCheck1,_,_);
		area_center(TransRegion, &AreaCheck2,_,_);
		DiffCheckArea = (Hlong)AreaCheck2[0] - (Hlong)AreaCheck1[0];
		
		if(1000 < DiffCheckArea) //CHOKI20120104 확인이 필요한 부분.
		{
			//편집 불량으로 처리 
			smallest_rectangle1(SelectedRegions,&CRow0,&CCol0,&CRow1,&CCol1);
			
			if(((Hlong)CRow1[0]-(Hlong)CRow0[0])> 50)
			{
				//copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
				union2(SelectedRegions, *rslt_region, rslt_region);
				*PuchErr = TRUE;
				return TRUE;  //Punch 면 TRUE
			}
		}
		else
		{
			//편집 불량으로 처리 
			//copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시
			union2(SelectedRegions, *rslt_region, rslt_region);
			*PuchErr = TRUE;
			return TRUE;  //Punch 면 TRUE
		}	
	}//CHOKI20120103 형태구분 추가작업

	Hobject CheckDoublePunchRgn, FillupDoublePunchRgn, ThreshDoublePunch, ConnectedRegionsDoublePunch;
	Hobject SelectedRegionsDoublePunch;
	int nCheckDoublePunch;

	shape_trans(sr_region, &FillupDoublePunchRgn, "convex");
	fill_up(FillupDoublePunchRgn,&FillupDoublePunchRgn);
	reduce_domain(*image,FillupDoublePunchRgn,&CheckDoublePunchRgn);
	threshold(CheckDoublePunchRgn,&ThreshDoublePunch,HTuple(thresh),HTuple(255));
	if(is_circle)
	{
		difference(ThreshDoublePunch, SelectedRegionsCircle, &ThreshDoublePunch);//사각펀치가 있으면 원형영역은 제거
	}
	connection(ThreshDoublePunch, &ConnectedRegionsDoublePunch);
	nCheckDoublePunch = bur_tol-50;
	if((bur_tol-50)<1)
	{
		nCheckDoublePunch = 50;
	}
	select_shape(ConnectedRegionsDoublePunch,&SelectedRegionsDoublePunch,HTuple("area"),HTuple("and"),HTuple(nCheckDoublePunch),HTuple("max"));
	count_obj(SelectedRegionsDoublePunch, &Num);
	
	if(2 < (Hlong)Num[0]) 
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(SelectedRegions, win);
		}

		//편집 불량으로 처리 
		//copy_obj(SelectedRegionsDoublePunch, rslt_region, 1, -1);//불량 표시
		union2(SelectedRegionsDoublePunch, *rslt_region, rslt_region);
		*PuchErr = TRUE;
		return TRUE;  //Punch 면 TRUE
			
	}
	//CHOKI20120905

	return TRUE;
}

//20100702
//PPunch(펀칭부 검사)
BOOL C_VisionModule::FindPPuchingChip(Hlong win, Hobject *image, RECT ins_region, 
									 int min_punch_area, int max_punch_area, int bur_tol,
									 Hobject *rslt_region)
{
	//20100115LSS
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions, RegionConvex, SelComplactRegion;
	HTuple Range, Num;

	Hobject LeftRegion, RightRegion, LeftImageReduced, RightImageReduced, LeftRsltRgn, RightRsltRgn;

	Hobject ConvxRegion;

	CRect rect;
 
	Hlong wd, ht;

	BOOL rslt = TRUE; 

	HTuple Row, Col, Row2, Col2, Dist;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_22!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	rect = ins_region;

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	//Punching inspection////////////////////////

	reduce_domain(*image,Region,&ImageReduced);


	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	//min_max_gray(Region, ImageReduced, 0, _, _, &Range);
	set_check("~give_error");
#ifdef AVI_TYPE_1
	herr = threshold(ImageReduced,&Region,HTuple(180),HTuple(255)); //20100712 lee	//AVI_TYPE_1//
#else	
	herr = threshold(ImageReduced,&Region,HTuple(160),HTuple(255)); //20100712 lee
#endif
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_20!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}


	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);
	//크기는 기준의 1.5배 이상이면 불량이므로 임시로 4000000을 상한치로 설정한다. 큰 사이지는 SR 찢김으로 검출 
	//Model별  Punch Size Teaching 기능 추가 필요 
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple(max_punch_area));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		rslt = FALSE;
		return rslt;
	}
	else
	{
		if(win)
		{
			set_color(win, HTuple("green"));
			disp_obj(SelectedRegions, win);
		}
	}

	//Punch burr검사/////////////////////////////
	union1(SelectedRegions, &SelectedRegions);
	shape_trans(SelectedRegions, &ConvxRegion, "convex");
	erosion_circle(ConvxRegion, &ConvxRegion, 5.5);
	difference(ConvxRegion, SelectedRegions, &SelectedRegions);
	connection(SelectedRegions, &SelectedRegions);
	select_shape(SelectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(bur_tol),HTuple("max"));


	union1(SelectedRegions, &SelectedRegions);
	count_obj(SelectedRegions, &Num);

	if(0 < (Hlong)Num[0]) //
	{
		//편집 불량으로 처리 
		copy_obj(SelectedRegions, rslt_region, 1, -1);//불량 표시

		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(SelectedRegions, win);
		}

		rslt = FALSE;
	}

	return rslt;
}

//CHOKI20111013
//PPunch(펀칭부 위치 검사)
BOOL C_VisionModule::FindPPuchingChipPos(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_region)
{
	//20100115LSS
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions, RegionConvex, SelComplactRegion;
	HTuple Range, Num;

	Hobject LeftRegion, RightRegion, LeftImageReduced, RightImageReduced, LeftRsltRgn, RightRsltRgn;

	Hobject ConvxRegion;

	CRect rect;
 
	Hlong wd, ht;

	BOOL rslt = TRUE; 

	HTuple Area, Row, Col, Row2, Col2, Dist;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_22!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	rect = ins_region;

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	reduce_domain(*image,Region,&ImageReduced);

	

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	
	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(0),HTuple(3)); //20100712 lee
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_2011!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	connection(Region,&ConnectedRegions);
	
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(2000),HTuple(7000));
	count_obj(SelectedRegions, &Num);

	if(0 == (Hlong)Num[0] )  //if(1 != (Hlong)Num[0] )
	{
		//No Chip Or Error
		rslt = FALSE;
	}
	else if(1 >(Hlong)Num[0])
	{
		rslt = FALSE;
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(SelectedRegions, win);
		}
	}
	else
	{
		area_center(SelectedRegions, &Area, &Row, &Col); 
		if((Hlong)Col[0] > 280 && (Hlong)Col[0] < 360)
		{
			rslt = TRUE;
			if(win)
			{
				set_color(win, HTuple("green"));
				disp_obj(SelectedRegions, win);
			}
		}
		else
		{
			rslt = FALSE;
			if(win)
			{
				set_color(win, HTuple("red"));
				disp_obj(SelectedRegions, win);
			}
		}
		
	}

	return rslt;
}
//CHOKI20111013

//CHOKI20100712
BOOL C_VisionModule::CheckPuchingChip(Hlong win, Hobject *image, RECT ins_region, 
									 int min_punch_area, int max_punch_area, int bur_tol,
									 Hobject *rslt_region)
{
		//20100115LSS
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions, RegionConvex, SelComplactRegion;
	HTuple Range, Num;

	Hobject LeftRegion, RightRegion, LeftImageReduced, RightImageReduced, LeftRsltRgn, RightRsltRgn;

	Hobject ConvxRegion;

	CRect rect;

	Hlong wd, ht;
	HTuple Row, Col, Row2, Col2, Dist;

	Hobject LeftSelectRgn, RightSelectRgn;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_23!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	rect = ins_region;

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);


	//Punching inspection////////////////////////
	erosion_rectangle1(Region, &Region, 100, 3);

	reduce_domain(*image,Region,&ImageReduced);

	//mean_image(ImageReduced, &ImageReduced, 10, 10);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	//min_max_gray(Region, ImageReduced, 0, _, _, &Range);
	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(220),HTuple(255)); //20100408 lee
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_21!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);
	//크기는 기준의 1.5배 이상이면 불량이므로 임시로 4000000을 상한치로 설정한다. 큰 사이지는 SR 찢김으로 검출 
	//Model별  Punch Size Teaching 기능 추가 필요 
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple(max_punch_area));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		return FALSE;
	}
	else
	{
		return TRUE;
	}


}

//CHOKI20110725
BOOL C_VisionModule::CheckPuchingChipAuto(Hlong win, Hobject *image, RECT ins_region, 
									 int min_punch_area, int max_punch_area, int bur_tol,
									 Hobject *rslt_region)
{
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions, RegionConvex, SelComplactRegion;
	HTuple Range, Num;

	Hobject LeftRegion, RightRegion, LeftImageReduced, RightImageReduced, LeftRsltRgn, RightRsltRgn;

	Hobject ConvxRegion;

	CRect rect;

	Hlong wd, ht;
	HTuple Row, Col, Row2, Col2, Dist;

	Hobject LeftSelectRgn, RightSelectRgn;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_36!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	rect = ins_region;

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);


	//Punching inspection////////////////////////
	erosion_rectangle1(Region, &Region, 100, 3);

	reduce_domain(*image,Region,&ImageReduced);

	//mean_image(ImageReduced, &ImageReduced, 10, 10);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	//min_max_gray(Region, ImageReduced, 0, _, _, &Range);
	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(220),HTuple(255)); //20100408 lee
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_64!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	//Punch 유무 검사 
	connection(Region,&ConnectedRegions);
	//크기는 기준의 1.5배 이상이면 불량이므로 임시로 4000000을 상한치로 설정한다. 큰 사이지는 SR 찢김으로 검출 
	//Model별  Punch Size Teaching 기능 추가 필요 
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(min_punch_area),HTuple(max_punch_area*2));
	count_obj(SelectedRegions, &Num);

	if(1 > (Hlong)Num[0])  //아무 것도 없다? Punch가 아니다.!!
	{
		//Not Punching
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL C_VisionModule::FindPuchingChipSR(Hlong win, Hobject *image, RECT ins_region)
{
	Hobject Region, ImageReduced, ConnectedRegions, SelectedRegions;
	HTuple Mean, Num;

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	reduce_domain(*image,Region,&ImageReduced);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(0),HTuple(100)); //20100405 lee
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_22!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region,&ConnectedRegions);
	select_shape(ConnectedRegions,&SelectedRegions,HTuple("area"),HTuple("and"),HTuple(200000),HTuple("max"));
	select_shape(SelectedRegions,&SelectedRegions,HTuple("compactness"),HTuple("and"),HTuple(0),HTuple(2.5));
	
	count_obj(SelectedRegions, &Num);
	if(1 > (Hlong)Num[0])
	{
		//Not punching
		return FALSE;
	}


	return TRUE;
}

BOOL C_VisionModule::FindChip(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;


	rslt = _Hal_FindChip(win, image, ins_region, rslt_chip_region,
					    min_area, min_width, min_height);

	HTuple Temp1, Temp2, Temp3, Temp4;
	smallest_rectangle1(*rslt_chip_region,&Temp1,&Temp2,&Temp3,&Temp4); //CHOKI20110504

	return rslt;
}

BOOL C_VisionModule::FindFirstChip(Hlong win, Hobject *image, int thresh, Hlong *chip_offset)
{
	BOOL find;
	int i, j;
	HTuple Width, Height, Pointer, Type;
	BYTE *pointer;
	Hlong idx, pos;
	int width, height;

	//Find chip(위에서 아래로 검사 밝은 부분이 있는지 검사)
	get_image_pointer1(*image, &Pointer, &Type, &Width, &Height);
	pointer = (BYTE*)((Hlong)Pointer[0]);
	pos = 0;
	width = (Hlong)Width[0];
	height = (Hlong)Height[0];

	for(i = (width - 1) ; i > 0; i--)
	{	
		find = TRUE;

		for(j = 0; j < 2; j++)
		{
			idx = i + ((400 + (j * 100)) * width);
			if( (*(pointer + idx) < thresh) && 
				(*(pointer + idx + (10 * width)) < thresh) && 
				(*(pointer + idx + (20 * width)) < thresh) && 
				(*(pointer + idx + (30 * width)) < thresh) && 
				(*(pointer + idx + (40 * width)) < thresh) && 
				(*(pointer + idx + (50 * width)) < thresh) && 
				(*(pointer + idx + (60 * width)) < thresh) && 
				(*(pointer + idx + (70 * width)) < thresh) && 
				(*(pointer + idx + (80 * width)) < thresh) && 
				(*(pointer + idx + (90 * width)) < thresh) && 
				(*(pointer + idx + (100 * width)) < thresh) )
			{
				find = FALSE;
				break;
			}
		}

		if(find)
		{
			//찾았음
			pos = i;
			break;
		}
	}	
	if(find)
	{
		if(win)
		{
			//찾았음
			set_color(win, HTuple("green"));
			for(i = 0 ; i < 2; i++)
			{
				disp_line(win, (400 + (i * 100)), 0, (400 + (i * 100)), pos);
			}

		}
	}
	else
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			for(i = 0 ; i < 2; i++)
			{
				disp_line(win, (400 + (i * 100)), 0, (400 + (i * 100)), width);
			}

		}
	}

	*chip_offset = pos;

	return find;
/*
	BOOL find;
	int i, j;
	HTuple Width, Height, Pointer, Type;
	BYTE *pointer;
	Hlong idx, pos;
	int width, height;

	//Find chip(위에서 아래로 검사 밝은 부분이 있는지 검사)
	get_image_pointer1(*image, &Pointer, &Type, &Width, &Height);
	pointer = (BYTE*)((Hlong)Pointer[0]);
	pos = 0;
	width = (Hlong)Width[0];
	height = (Hlong)Height[0];

	for(i = (width - 1) ; i > 0; i--)
	{	
		find = TRUE;

		for(j = 0; j < 20; j++)
		{
			idx = i + ((500 + (j * 100)) * width);
			if( (*(pointer + idx) < thresh) && 
				(*(pointer + idx + (10 * width)) < thresh) && 
				(*(pointer + idx + (20 * width)) < thresh) && 
				(*(pointer + idx + (30 * width)) < thresh) && 
				(*(pointer + idx + (40 * width)) < thresh) && 
				(*(pointer + idx + (50 * width)) < thresh) && 
				(*(pointer + idx + (60 * width)) < thresh) && 
				(*(pointer + idx + (70 * width)) < thresh) && 
				(*(pointer + idx + (80 * width)) < thresh) && 
				(*(pointer + idx + (90 * width)) < thresh) && 
				(*(pointer + idx + (100 * width)) < thresh) )
			{
				find = FALSE;
				break;
			}
		}

		if(find)
		{
			//찾았음
			pos = i;
			break;
		}
	}	
	if(find)
	{
		if(win)
		{
			//찾았음
			set_color(win, HTuple("green"));
			for(i = 0 ; i < 21; i++)
			{
				disp_line(win, (500 + (i * 100)), 0, (500 + (i * 100)), pos);
			}

		}
	}
	else
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			for(i = 0 ; i < 21; i++)
			{
				disp_line(win, (500 + (i * 100)), 0, (500 + (i * 100)), width);
			}

		}
	}

	*chip_offset = pos;

	return find;
*/
}

BOOL C_VisionModule::ChipOut(Hlong win, Hobject *image, Hobject *image2, Hobject *image3, Hobject *image4, Hobject *image5, 
							 Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int thresh, int tolerance,
							  int mthresh, int mtolerance, Hlong moutcount, int edgesize,
							  Hlong *rslt_num, Hobject *rslt_region)
{
	BOOL rslt = TRUE;
	BOOL mrslt = TRUE;

	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	int high_thresh = 0;
	high_thresh = thresh + 20;
	if(high_thresh > 255)
	{
		thresh = 235;
		high_thresh = 255;
	}
	if(high_thresh < 0)
	{
		thresh = 10;
		high_thresh = 30;
	}

	int mhigh_thresh = 0;
	mhigh_thresh = mthresh + 20;
	if(mhigh_thresh > 255)
	{
		mthresh = 235;
		mhigh_thresh = 255;
	}
	if(mhigh_thresh < 0)
	{
		mthresh = 10;
		mhigh_thresh = 30;
	}
	
	double medge_size = 0.;
	medge_size = (double)edgesize / 100;

	rslt = _Hal_ChipOut(win, image, chip_ins_region, chip_ins_region1,
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							 thresh, high_thresh, tolerance, medge_size,
							 rslt_num, rslt_region,
							 min_area, min_width, min_height);
							

	mrslt = _Hal_ChipOut_m(win, image, chip_ins_region, chip_ins_region1,
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							 mthresh, mhigh_thresh, mtolerance, moutcount,
							 rslt_num, rslt_region,
							 min_area, min_width, min_height);

	if(rslt && mrslt)
	{
  		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL C_VisionModule::_Hal_ChipOut(Hlong win, Hobject *image, Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int high_thresh, int tolerance, double edgesize, Hlong *rslt_num, Hobject *rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1, SelectRgn;
	HTuple Num;
	HTuple DiffAng, HomMat2D;
	Hobject CenterRgn0, CenterRgn1, LeftRightRgn, RsltRgn2, RsltRgn3, InsRgn2, CenterRgnTot;
	
	HTuple Row0, Row1, Col0, Col1;
	Hlong width;
	
	double offset_x, offset_y;
	int tol;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	//필름이 잘 안펴 졌을때 얼라인 오차를 줄여 보려고 대입한 식(현재 일단 제외 후 추후 검토예정)
	/*
	float OffsetLengthX;

	OffsetLengthX = fabs(offset_ox-offset_ox2) - fabs(offset_rx-offset_rx2); //20110528 lee

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx+OffsetLengthX, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));
	*/

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));

	//20150309 lby 칩 외곽 따기
	smallest_rectangle1(ChipRgn1, &Row0, &Col0, &Row1, &Col1);
	width = (Hlong)Col1[0] - (Hlong)Col0[0];
	if( (width * edgesize) > 1 )
		erosion_rectangle1(ChipRgn1, &CenterRgnTot, (width * edgesize), (width * edgesize));
	else
		gen_empty_obj(&CenterRgnTot);

	//런타임 칩영역//////////////////////////////

	erosion_rectangle1(ChipRgn1, &InsRgn, 2, 2); 
	/////////////////////////////////////////////
	//Center/////////////////////////////////////

	if(1 > tolerance)
	{
		tol = 1;
	}
	else if(511 < tolerance)
	{
		tol = 511;
	}
	else
	{
		tol = tolerance;
	}
	
	erosion_rectangle1(InsRgn, &CenterRgn0, tol, 1);
	difference(CenterRgn0, CenterRgnTot, &CenterRgn0); //20150309 lby
	reduce_domain(*image, CenterRgn0, &ImageReduced);

	//CHOKI20110603
	if(thresh+10 > high_thresh)
	{
		thresh = high_thresh;
	}
	if((high_thresh + 20) >255)
	{
		high_thresh = 234;
	}

	Herror herr;
	CString status;


	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_01!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100518 lss
	select_shape(RegionClosing, &RsltRgn0, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple("max"));
	//20150121 lss
	select_shape(RsltRgn0, &RsltRgn0, HTuple("width"), HTuple("and"), HTuple(3), HTuple("max"));

	//티칭시 등록된 칩영역///////////////////////
	erosion_rectangle1(ChipRgn1, &InsRgn2, 10, 10);


	if(1 > tolerance)
	{
		tol = 1;
	}
	else if(511 < tolerance)
	{
		tol = 511;
	}
	else
	{
		tol = tolerance;
	}

	erosion_rectangle1(InsRgn2, &CenterRgn1, tol, 1);
	difference(CenterRgn1, CenterRgnTot, &CenterRgn1); //20150309 lby
	reduce_domain(*image, CenterRgn1, &ImageReduced);

	if(thresh > high_thresh+20) thresh = high_thresh; //20110616 lee

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_02!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100518 lss	
	select_shape(RegionClosing, &RsltRgn1, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple("max"));
	//20150121 lss
	select_shape(RsltRgn1, &RsltRgn1, HTuple("width"), HTuple("and"), HTuple(3), HTuple("max"));

	//20100518
	/////////////////////////////////////////////
	//LeftRight//////////////////////////////////
	//

	//20100618 lss 
	erosion_rectangle1(InsRgn, &CenterRgn0, 100, 1);
	dilation_rectangle1(CenterRgn0, &CenterRgn0, 1, 10);

	//가운데 검사영역 제거
	difference(InsRgn, CenterRgn0, &LeftRightRgn);

	//20100618 lss
	tol = tolerance;
	if(1 > tol)
	{
		tol = 1;
	}
	else if(511 < tol)
	{
		tol = 511;
	}
	

	erosion_rectangle1(LeftRightRgn, &LeftRightRgn, 1, tol);
	difference(LeftRightRgn, CenterRgnTot, &LeftRightRgn); //20150309 lby
	reduce_domain(*image, LeftRightRgn, &ImageReduced);

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_03!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn2, HTuple("width"), HTuple("and"), HTuple(tolerance), HTuple("max"));



	//티칭시 등록된 칩영역///////////////////////

	//20100618 lss
	erosion_rectangle1(InsRgn2, &CenterRgn1, 100, 1);
	dilation_rectangle1(CenterRgn1, &CenterRgn1, 1, 10);

	difference(InsRgn2, CenterRgn1, &LeftRightRgn);

	//20100618 lss
	erosion_rectangle1(LeftRightRgn, &LeftRightRgn, 1, tol);
	difference(LeftRightRgn, CenterRgnTot, &LeftRightRgn); //20150309 lby
	reduce_domain(*image, LeftRightRgn, &ImageReduced);

	if(thresh > high_thresh+20) thresh = high_thresh; //20110616 lee

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_04!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn3, HTuple("width"), HTuple("and"), HTuple(tolerance), HTuple("max"));

	union2(RsltRgn0, RsltRgn1, &RsltRgn0);
	union2(RsltRgn0, RsltRgn2, &RsltRgn0); //CHOKI20110509
	union2(RsltRgn0, RsltRgn3, &RsltRgn0);

	connection(RsltRgn0, &RsltRgn0);

	count_obj(RsltRgn0, &Num);

	if(win)
	{
		disp_obj(RsltRgn0, win);
	}

	BOOL result_out = TRUE;
	Hlong cnt = (Hlong)Num[0];
	if(0 < cnt)
	{
		*rslt_num = cnt;
		union2(RsltRgn0, *rslt_region, rslt_region);

		result_out = FALSE;
	}
	
	//imsi
	//union2(RsltRgn0, *rslt_region, rslt_region);
	//*rslt_num = 1;
	//result_out = FALSE;
	
	return result_out;
}

/*
BOOL C_VisionModule::_Hal_ChipOut(Hlong win, Hobject *image, Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int high_thresh, int tolerance, Hlong *rslt_num, Hobject *rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1, SelectRgn;
	HTuple Num;
	HTuple DiffAng, HomMat2D;
	Hobject CenterRgn0, CenterRgn1, LeftRightRgn, RsltRgn2, RsltRgn3, InsRgn2;
	
	double offset_x, offset_y;
	int tol;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));

	//런타임 칩영역//////////////////////////////

	erosion_rectangle1(ChipRgn1, &InsRgn, 2, 2); 
	/////////////////////////////////////////////
	//Center/////////////////////////////////////

	if(1 > tolerance)
	{
		tol = 1;
	}
	else if(511 < tolerance)
	{
		tol = 511;
	}
	else
	{
		tol = tolerance;
	}
	
	erosion_rectangle1(InsRgn, &CenterRgn0, tol, 1);
	reduce_domain(*image, CenterRgn0, &ImageReduced);



	//CHOKI20110603
	if(thresh+10 > high_thresh)
	{
		thresh = high_thresh;
	}
	if((high_thresh + 20) >255)
	{
		high_thresh = 234;
	}

	Herror herr;
	CString status;


	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_01!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100518 lss
	select_shape(RegionClosing, &RsltRgn0, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple("max"));
	//20150121 lss
	select_shape(RsltRgn0, &RsltRgn0, HTuple("width"), HTuple("and"), HTuple(6), HTuple("max"));



	//티칭시 등록된 칩영역///////////////////////
	erosion_rectangle1(ChipRgn1, &InsRgn2, 10, 10);


	if(1 > tolerance)
	{
		tol = 1;
	}
	else if(511 < tolerance)
	{
		tol = 511;
	}
	else
	{
		tol = tolerance;
	}

	erosion_rectangle1(InsRgn2, &CenterRgn1, tol, 1);
	reduce_domain(*image, CenterRgn1, &ImageReduced);

	if(thresh > high_thresh+20) thresh = high_thresh; //20110616 lee

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_02!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100518 lss	
	select_shape(RegionClosing, &RsltRgn1, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple("max"));
	select_shape(RsltRgn1, &RsltRgn1, HTuple("width"), HTuple("and"), HTuple(6), HTuple("max"));

	//20100518
	/////////////////////////////////////////////
	//LeftRight//////////////////////////////////
	//

	//20100618 lss 
	erosion_rectangle1(InsRgn, &CenterRgn0, 100, 1);
	dilation_rectangle1(CenterRgn0, &CenterRgn0, 1, 10);

	//가운데 검사영역 제거
	difference(InsRgn, CenterRgn0, &LeftRightRgn);

	//20100618 lss
	tol = tolerance;
	if(1 > tol)
	{
		tol = 1;
	}
	else if(511 < tol)
	{
		tol = 511;
	}
	

	erosion_rectangle1(LeftRightRgn, &LeftRightRgn, 1, tol);
	  
	reduce_domain(*image, LeftRightRgn, &ImageReduced);

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_03!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn2, HTuple("ra"), HTuple("and"), HTuple(tolerance), HTuple("max"));



	//티칭시 등록된 칩영역///////////////////////

	//20100618 lss
	erosion_rectangle1(InsRgn2, &CenterRgn1, 100, 1);
	dilation_rectangle1(CenterRgn1, &CenterRgn1, 1, 10);

	difference(InsRgn2, CenterRgn1, &LeftRightRgn);

	//20100618 lss
	erosion_rectangle1(LeftRightRgn, &LeftRightRgn, 1, tol);

	reduce_domain(*image, LeftRightRgn, &ImageReduced);

	if(thresh > high_thresh+20) thresh = high_thresh; //20110616 lee

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_04!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn3, HTuple("ra"), HTuple("and"), HTuple(tolerance), HTuple("max"));

	union2(RsltRgn0, RsltRgn1, &RsltRgn0);
	union2(RsltRgn0, RsltRgn2, &RsltRgn0); //CHOKI20110509
	union2(RsltRgn0, RsltRgn3, &RsltRgn0);

	connection(RsltRgn0, &RsltRgn0);

	count_obj(RsltRgn0, &Num);

	if(win)
	{
		disp_obj(RsltRgn0, win);
	}

	BOOL result_out = TRUE;
	Hlong cnt = (Hlong)Num[0];
	if(0 < cnt)
	{
		*rslt_num = cnt;
		union2(RsltRgn0, *rslt_region, rslt_region);

		result_out = FALSE;
	}
	
	//imsi
	//union2(RsltRgn0, *rslt_region, rslt_region);
	//*rslt_num = 1;
	//result_out = FALSE;
	
	return result_out;
}
*/
	
BOOL C_VisionModule::_Hal_ChipOut_m(Hlong win, Hobject *image, Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int high_thresh, int tolerance, int count, Hlong *rslt_num, Hobject *rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1, SelectRgn;
	HTuple Num;
	HTuple DiffAng, HomMat2D;
	Hobject CenterRgn0, CenterRgn1, LeftRightRgn, RsltRgn2, RsltRgn3, InsRgn2;
	
	double offset_x, offset_y;
	int tol;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	float OffsetLengthX;

	OffsetLengthX = fabs(offset_ox-offset_ox2) - fabs(offset_rx-offset_rx2); //20110528 lee


	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx+OffsetLengthX, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));

	//런타임 칩영역//////////////////////////////

	erosion_rectangle1(ChipRgn1, &InsRgn, 3, 3); 
	/////////////////////////////////////////////
	//Center/////////////////////////////////////

	if(1 > tolerance)
	{
		tol = 1;
	}
	else if(511 < tolerance)
	{
		tol = 511;
	}
	else
	{
		tol = tolerance;
	}
	
	erosion_rectangle1(InsRgn, &CenterRgn0, tol, 1);
	reduce_domain(*image, CenterRgn0, &ImageReduced);

	//CHOKI20110603
	if(thresh+10 > high_thresh)
	{
		thresh = high_thresh;
	}
	if((high_thresh + 20) >255)
	{
		high_thresh = 234;
	}

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_01!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100518 lss
	select_shape(RegionClosing, &RsltRgn0, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple("max"));

	//티칭시 등록된 칩영역///////////////////////
	erosion_rectangle1(ChipRgn1, &InsRgn2, 10, 10);


	if(1 > tolerance)
	{
		tol = 1;
	}
	else if(511 < tolerance)
	{
		tol = 511;
	}
	else
	{
		tol = tolerance;
	}

	erosion_rectangle1(InsRgn2, &CenterRgn1, tol, 1);
	reduce_domain(*image, CenterRgn1, &ImageReduced);

	if(thresh > high_thresh+20) thresh = high_thresh; //20110616 lee

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_02!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100518 lss	
	select_shape(RegionClosing, &RsltRgn1, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple("max"));

	//20100518
	/////////////////////////////////////////////
	//LeftRight//////////////////////////////////
	//

	//20100618 lss 
	erosion_rectangle1(InsRgn, &CenterRgn0, 100, 1);
	dilation_rectangle1(CenterRgn0, &CenterRgn0, 1, 10);

	//가운데 검사영역 제거
	difference(InsRgn, CenterRgn0, &LeftRightRgn);

	//20100618 lss
	tol = tolerance;
	if(1 > tol)
	{
		tol = 1;
	}
	else if(511 < tol)
	{
		tol = 511;
	}
	
	erosion_rectangle1(LeftRightRgn, &LeftRightRgn, 1, tol);
	  
	reduce_domain(*image, LeftRightRgn, &ImageReduced);

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_03!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn2, HTuple("ra"), HTuple("and"), HTuple(tolerance), HTuple("max"));

	//티칭시 등록된 칩영역///////////////////////

	//20100618 lss
	erosion_rectangle1(InsRgn2, &CenterRgn1, 100, 1);
	dilation_rectangle1(CenterRgn1, &CenterRgn1, 1, 10);

	difference(InsRgn2, CenterRgn1, &LeftRightRgn);

	//20100618 lss
	erosion_rectangle1(LeftRightRgn, &LeftRightRgn, 1, tol);

	reduce_domain(*image, LeftRightRgn, &ImageReduced);

	if(thresh > high_thresh+20) thresh = high_thresh; //20110616 lee

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh+20),HTuple(10));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_04!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn3, HTuple("ra"), HTuple("and"), HTuple(tolerance), HTuple("max"));

	//concat_obj(RsltRgn0, RsltRgn1, &RsltRgn0);
	//concat_obj(RsltRgn0, RsltRgn2, &RsltRgn0);
	//concat_obj(RsltRgn0, RsltRgn3, &RsltRgn0);
	union2(RsltRgn0, RsltRgn1, &RsltRgn0);
	union2(RsltRgn0, RsltRgn2, &RsltRgn0); //CHOKI20110509
	union2(RsltRgn0, RsltRgn3, &RsltRgn0);

	union1(RsltRgn0, &RsltRgn0);

	connection(RsltRgn0, &RsltRgn0);
	
	select_shape(RsltRgn0, &RsltRgn0, HTuple("width").Concat("height"), HTuple("and"), HTuple(tolerance+2).Concat(tolerance+2), HTuple("max").Concat("max"));

	count_obj(RsltRgn0, &Num);

	if(win)
	{
		disp_obj(RsltRgn0, win);
	}

	BOOL result_out = TRUE;
	Hlong cnt = (Hlong)Num[0];
	if(count < cnt)
	{
		for(int i = 0; i < cnt; i++)
		{
			select_obj(RsltRgn0, &SelectRgn, i + 1);
			
			union2(SelectRgn, *rslt_region, rslt_region);
			//connection(*rslt_region, rslt_region);
			//count_obj(*rslt_region, &Num);
			(*rslt_num)++;
			if(i > 100)//10개까지만 잡는다
			{
				return FALSE;
			}
			result_out = FALSE;
		}
	}	

	return result_out;
}

//CHOKI20100714
BOOL C_VisionModule::_Hal_ChipOutTop(Hlong win, Hobject *image, Hobject *image2, Hobject *image3, 
							Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int high_thresh, int tolerance, Hlong outcount, Hlong *rslt_num, Hobject *rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1, RegionErosion, RegionDilation;
	HTuple Num;
	HTuple DiffAng, HomMat2D;
	Hobject CenterRgn0, CenterRgn1, LeftRightRgn, RsltRgn2, RsltRgn3, InsRgn2;
	CRect ChipAreaRect; 
	Hlong ChipAreaTop, ChipAreaCenter;

	//CHOKI20100804
	BOOL ReturnRslt=TRUE;
	Hobject SelectRgn, ResultRgn;
	HTuple Mean;
	
	double offset_x, offset_y;
	int tol;
	HTuple CRow0, CCol0, CRow1, CCol1;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);
//	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	float OffsetLengthX;

	OffsetLengthX = fabs(offset_ox-offset_ox2) - fabs(offset_rx-offset_rx2); //20110528 lee

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx+OffsetLengthX, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));

	erosion_rectangle1(ChipRgn1, &InsRgn2, 1, 1);

	tol = tolerance;

	erosion_rectangle1(InsRgn2, &CenterRgn1, 1, 1);
	dilation_rectangle1(CenterRgn1, &CenterRgn1, 1, 1);

	reduce_domain(*image, CenterRgn1, &ImageReduced);

//	emphasize(ImageReduced, &ImageReduced, 4, 4, 3);

	//CHOKI20110603
	if(thresh >= high_thresh)
	{
		thresh = high_thresh -10;
	}
	if(thresh < 0 ) 
	{
		thresh = 0;
	}
	else if(thresh > 244)
	{
		thresh = 244;
	}
	if(high_thresh < 10)
	{
		high_thresh = 10;
	}
	else if(high_thresh > 255)
	{
		high_thresh = 255;
	}


	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_23!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}
	
	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	smallest_rectangle1(ImageReduced,&CRow0,&CCol0,&CRow1,&CCol1);

	ChipAreaRect.top = (Hlong)CRow0[0];
	ChipAreaRect.bottom = (Hlong)CRow1[0];
	ChipAreaRect.left = (Hlong)CCol0[0];
	ChipAreaRect.right = (Hlong)CCol1[0];

	ChipAreaTop = (Hlong)ChipAreaRect.top;


	ChipAreaCenter = (Hlong)(ChipAreaTop+15);//CHOKI20100802

	select_shape(RegionClosing, &RsltRgn0, HTuple("width").Concat("height"), HTuple("and"), HTuple(tolerance).Concat(tolerance), HTuple(100).Concat(20));//20110519 lee 미검 발생함. 
	select_shape(RsltRgn0, &RsltRgn0, HTuple("row"), HTuple("and"), HTuple(ChipAreaTop), HTuple(ChipAreaCenter));

	connection(RsltRgn0, &RsltRgn0);
	count_obj(RsltRgn0, &Num);

	gen_empty_obj(rslt_region);

	//CHOKI20110523 //조건 제외시 검출만 되고 이력이 남지않음
    Hlong ResultCount;
	ResultCount = (Hlong)Num[0];
	//CHOKI20110523

	if(outcount <= (Hlong)Num[0])
	{
		for(int i = 0; i < (Hlong)Num[0]; i++)//CHOKI20100804 *rslt_num->(Hlong)Num[0]
		{
			select_obj(RsltRgn0, &SelectRgn, i + 1);
			select_shape(SelectRgn, &SelectRgn, HTuple("width").Concat("height"), HTuple("and"), HTuple(tolerance).Concat(tolerance), HTuple(100).Concat(20));
			intensity(SelectRgn, *image2, &Mean, _);
			//20110530 lee 
			if((Hlong)Mean[0]<180)//CHOKI20110516 칩표면 수지 밝기 필터링 기준을 200->170 -> 150 //CHOKI20110523  제거 되었던 조건을 수치조정하여  재설정 140->180
			{
				union2(SelectRgn, *rslt_region, rslt_region);
				ReturnRslt = FALSE;//CHOKI20100805 if문 안쪽으로 이동
			}
		}
		connection(*rslt_region, rslt_region);
		count_obj(*rslt_region, &Num);
		*rslt_num = (Hlong)Num[0];
		//CHOKI20110523
	}

	//CHOKI20110531
	Hobject ImageReduced530, Thresh530, RsltRgn530, RegionClosing530, RegionErosion530, RegionDilation530;
	HTuple Num530;
	Hlong TempCount = 0;
	HTuple ChipRow, ChipCol, ChipPhi, ChipWidth, ChipHeight;
	HTuple ChipOutArea;

	erosion_rectangle1(CenterRgn1, &RegionErosion530, 4, 2); //20110601 lee

	reduce_domain(*image2, RegionErosion530, &ImageReduced530);

	set_check("~give_error");
	herr = threshold(ImageReduced530,&Thresh530,HTuple(5),HTuple(100));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_24!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	
	
	connection(Thresh530, &Thresh530);
	closing_circle(Thresh530,&RegionClosing530,HTuple(1.5));

	select_shape(RegionClosing530, &RsltRgn530, HTuple("area"), HTuple("and"), HTuple(tolerance), HTuple("max"));
	select_shape(RsltRgn530, &RsltRgn530, HTuple("height"), HTuple("and"), HTuple(2), HTuple(20)); //20110601 lee

	connection(RsltRgn530, &RsltRgn530);
	count_obj(RsltRgn530, &Num530);

	if( 0 < (Hlong)Num530[0])
	{
		for(int i = 0; i < (Hlong)Num530[0]; i++)
		{
			select_obj(RsltRgn530, &SelectRgn, i + 1);
			intensity(SelectRgn, *image, &Mean, _);
			smallest_rectangle2(SelectRgn, &ChipRow, &ChipCol, &ChipPhi, &ChipWidth, &ChipHeight);
			area_center(SelectRgn, &ChipOutArea, _, _);

			if(((Hlong)Mean[0]>40  || (Hlong)Mean[0] < 16) && (double)ChipWidth[0]*2 < 40 &&((double)ChipWidth[0]*2 >= tolerance || (double)ChipHeight[0]*2 >= tolerance ))
			{
				union2(SelectRgn, *rslt_region, rslt_region);
				ReturnRslt = FALSE;
				TempCount++;
			}
		}
		connection(*rslt_region, rslt_region);
		count_obj(*rslt_region, &Num);
		*rslt_num += TempCount;
	}
	//CHOKI20110531

	return ReturnRslt;
}

//CHOKI20100714
BOOL C_VisionModule::_Hal_ChipOutBtm(Hlong win, Hobject *image, Hobject *image2, Hobject *image3, 
							Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int high_thresh, int tolerance, Hlong outcount, Hlong *rslt_num, Hobject *rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1, RegionErosion, RegionDilation;
	HTuple Num;
	HTuple DiffAng, HomMat2D;
	Hobject CenterRgn0, CenterRgn1, LeftRightRgn, RsltRgn2, RsltRgn3, InsRgn2;
	CRect ChipAreaRect; 
	Hlong ChipAreaBtm, ChipAreaCenter;

	//CHOKI20100802
	BOOL ReturnRslt=TRUE;
	Hobject SelectRgn, ResultRgn;
	HTuple Mean;
	
	double offset_x, offset_y;
	int tol;
	HTuple CRow0, CCol0, CRow1, CCol1;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	float OffsetLengthX;

	OffsetLengthX = fabs(offset_ox-offset_ox2) - fabs(offset_rx-offset_rx2); //20110528 lee

	//CHOKI20110527 필름의 왜곡발생으로 안하여 정상적인 영역티칭이 되지않을 경우에 Offset값을 강제로 맞춰줌
	HTuple CRowC0,CColC0,CRowC1,CColC1;
	smallest_rectangle1(chip_ins_region1,&CRowC0,&CColC0,&CRowC1,&CColC1);
    gen_rectangle1(&chip_ins_region1,(Hlong)CRowC0[0],(Hlong)CColC0[0],(Hlong)CRowC1[0],(Hlong)CColC1[0]); //CHOKI20110715
//	gen_rectangle1(&chip_ins_region1,(Hlong)CRowC0[0],(Hlong)CColC0[0]-OffsetLengthX,(Hlong)CRowC1[0],(Hlong)CColC1[0]-OffsetLengthX);
	//CHOKI20110527

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx+OffsetLengthX, HTuple(0), &HomMat2D);
//	vector_angle_to_rigid(offset_ry, offset_rx, DiffAng, offset_oy, offset_ox, HTuple(0), &HomMat2D); //CHOKI20110524 affine_trans_region의 참조값이 바꿔있어 영역을 정상적으로 찾지 못함
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));

	//	erosion_rectangle1(chip_ins_region, &InsRgn, 1, 1); 
	erosion_rectangle1(ChipRgn1, &InsRgn2, 2, 1);

	tol = tolerance;

//	erosion_rectangle1(InsRgn2, &CenterRgn1, tol, 1);//CHOKI20100810
	erosion_rectangle1(InsRgn2, &CenterRgn1, 1, 1);

	reduce_domain(*image, CenterRgn1, &ImageReduced);

//	hysteresis_threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh),HTuple(10));

	//CHOKI20110603
	if(thresh >= high_thresh)
	{
		thresh = high_thresh -10;
	}
	if(thresh < 0 ) 
	{
		thresh = 0;
	}
	else if(thresh > 244)
	{
		thresh = 244;
	}
	if(high_thresh < 10)
	{
		high_thresh = 10;
	}
	else if(high_thresh > 255)
	{
		high_thresh = 255;
	}


	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_25!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(1.5));

	connection(RegionClosing, &RegionClosing);

//	erosion_rectangle1(RegionClosing, &RegionErosion, 1, 1);
	dilation_rectangle1(RegionErosion, &RegionDilation, 2, 6);

	smallest_rectangle1(ImageReduced,&CRow0,&CCol0,&CRow1,&CCol1);

//	ChipAreaBtm = (Hlong)CRow1[0];
//	ChipAreaCenter = (Hlong)(((Hlong)CRow0[0]+(Hlong)CRow1[0])/2);
	ChipAreaRect.top = (Hlong)CRow0[0];
	ChipAreaRect.bottom = (Hlong)CRow1[0];
	ChipAreaRect.left = (Hlong)CCol0[0];
	ChipAreaRect.right = (Hlong)CCol1[0];

	ChipAreaBtm = (Hlong)ChipAreaRect.bottom;


	ChipAreaCenter = (Hlong)(ChipAreaBtm-15);//CHOKI20100802

//	select_shape(RegionClosing, &RsltRgn0, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple(10));
//	select_shape(RegionClosing, &RsltRgn0, HTuple("width").Concat("height"), HTuple("and"), HTuple(2).Concat(tolerance), HTuple(10).Concat(10));//CHOKI20100903 Check OVK발생.
	select_shape(RegionClosing, &RsltRgn0, HTuple("width").Concat("height"), HTuple("and"), HTuple(2).Concat(tolerance), HTuple(100).Concat(20));//20110519 lee 미검 발생 Size횡으로 클겨우 있음. 
	select_shape(RsltRgn0, &RsltRgn0, HTuple("row"), HTuple("and"), HTuple(ChipAreaCenter), HTuple(ChipAreaBtm));

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

//	union2(RsltRgn0, RsltRgn1, &RsltRgn0);

	connection(RsltRgn0, &RsltRgn0);

	count_obj(RsltRgn0, &Num);

	//CHOKI20110523 //조건 제외시 검출만 되고 이력이 남지않음
//	*rslt_num = (Hlong)Num[0]; 
    Hlong ResultCount;
	ResultCount = (Hlong)Num[0];
	//CHOKI20110523

	if(outcount <= (Hlong)Num[0])
	{
		for(int i = 0; i < (Hlong)Num[0]; i++)//CHOKI20100804 *rslt_num->(Hlong)Num[0]
		{
			select_obj(RsltRgn0, &SelectRgn, i + 1);
			intensity(SelectRgn, *image2, &Mean, _);
			//20110530 lee
			if((Hlong)Mean[0]<180) //CHOKI20110516 칩표면 수지 밝기 필터링 기준을 200->170 -> 150 //CHOKI20110523 //CHOKI20110811 제거 되었던 조건을 수치조정하여  재설정 140->180
			{
			//	union1(SelectRgn, rslt_region);
				union2(SelectRgn, *rslt_region, rslt_region);	//CHOKI20100804//CHOKI20100805

				if(win)
				{
					set_color(win, HTuple("red"));
					disp_obj(*rslt_region, win);
				}
				ReturnRslt = FALSE;//CHOKI20100805 if문 안쪽으로 이동
			}
		}
		//CHOKI20110523
		connection(*rslt_region, rslt_region); 
		count_obj(*rslt_region, &Num);
		*rslt_num = (Hlong)Num[0]; 
		//CHOKI20110523
	}

	//CHOKI20110531
	Hobject ImageReduced530, Thresh530, RsltRgn530, RegionClosing530, RegionErosion530, RegionDilation530;
	HTuple Num530;
	Hlong TempCount = 0;
	HTuple ChipRow, ChipCol, ChipPhi, ChipWidth, ChipHeight; //CHOKI20110616
	HTuple ChipOutArea; //CHOKI20110617

	erosion_rectangle1(CenterRgn1, &RegionErosion530, 4, 2); //20110601 lee

	reduce_domain(*image2, RegionErosion530, &ImageReduced530);
	
	set_check("~give_error");
	herr = threshold(ImageReduced530,&Thresh530,HTuple(5),HTuple(100));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_26!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	
	
	connection(Thresh530, &Thresh530);
	closing_circle(Thresh530,&RegionClosing530,HTuple(1.5));

	select_shape(RegionClosing530, &RsltRgn530, HTuple("area"), HTuple("and"), HTuple(tolerance), HTuple("max"));
	select_shape(RsltRgn530, &RsltRgn530, HTuple("height"), HTuple("and"), HTuple(2), HTuple(20)); //20110601 lee

	connection(RsltRgn530, &RsltRgn530);
	count_obj(RsltRgn530, &Num530);

	if( 0 < (Hlong)Num530[0])
	{
		for(int i = 0; i < (Hlong)Num530[0]; i++)
		{
			select_obj(RsltRgn530, &SelectRgn, i + 1);
			intensity(SelectRgn, *image, &Mean, _);
			smallest_rectangle2(SelectRgn, &ChipRow, &ChipCol, &ChipPhi, &ChipWidth, &ChipHeight); //CHOKI20110616
			area_center(SelectRgn, &ChipOutArea, _, _);

			if(((Hlong)Mean[0]>40  || (Hlong)Mean[0] < 16) && (double)ChipWidth[0]*2 < 40 &&((double)ChipWidth[0]*2 >= tolerance || (double)ChipHeight[0]*2 >= tolerance ))
			{
				union2(SelectRgn, *rslt_region, rslt_region);
				ReturnRslt = FALSE;
				TempCount++;
			}
		}
		connection(*rslt_region, rslt_region);
		count_obj(*rslt_region, &Num);
		*rslt_num += TempCount;
	}
	//CHOKI20110531

	return ReturnRslt;
	//CHOKI20100802 
}

//CHOKI20100810
BOOL C_VisionModule::_Hal_ChipOutTopBtm(Hlong win, Hobject *image, Hobject *image2, Hobject chip_ins_region, Hobject chip_ins_region1,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int high_thresh, int tolerance, Hlong outcount, Hlong *rslt_num, Hobject *rslt_region,
							 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1, RegionErosion, RegionDilation;
	HTuple Num;
	HTuple DiffAng, HomMat2D;
	Hobject CenterRgn0, CenterRgn1, LeftRightRgn, RsltRgn2, RsltRgn3, InsRgn2;
	CRect ChipAreaRect; 
	Hlong ChipAreaTop, ChipAreaBtm, ChipAreaTopCenter, ChipAreaBtmCenter;

	BOOL ReturnRslt=TRUE;
	Hobject SelectRgn, ResultRgn;
	HTuple Mean;
	
	double offset_x, offset_y;
	int tol;
	HTuple CRow0, CCol0, CRow1, CCol1;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));

	erosion_rectangle1(ChipRgn1, &InsRgn2, 1, 1);

	tol = tolerance;

	//erosion_rectangle1(InsRgn2, &CenterRgn1, tol, 1);//CHOKI20100810
	erosion_rectangle1(InsRgn2, &CenterRgn1, 3, 3);

	//CHOKI20110603
	if(thresh >= high_thresh)
	{
		thresh = high_thresh -10;
	}
	if(thresh < 0 ) 
	{
		thresh = 0;
	}
	else if(thresh > 244)
	{
		thresh = 244;
	}
	if(high_thresh < 10)
	{
		high_thresh = 10;
	}
	else if(high_thresh > 255)
	{
		high_thresh = 255;
	}

	reduce_domain(*image, CenterRgn1, &ImageReduced);

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple(thresh),HTuple(high_thresh));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_27!... %ld, %ld", (Hlong)thresh, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

//	erosion_rectangle1(RegionClosing, &RegionErosion, 1, 1);
	dilation_rectangle1(RegionErosion, &RegionDilation, 2, 6);

	smallest_rectangle1(ImageReduced,&CRow0,&CCol0,&CRow1,&CCol1);

	ChipAreaRect.top = (Hlong)CRow0[0];
	ChipAreaRect.bottom = (Hlong)CRow1[0];
	ChipAreaRect.left = (Hlong)CCol0[0];
	ChipAreaRect.right = (Hlong)CCol1[0];

	ChipAreaBtm = (Hlong)ChipAreaRect.bottom;

	ChipAreaTop = (Hlong)ChipAreaRect.top;

	ChipAreaTopCenter = (Hlong)(ChipAreaTop+10);
	ChipAreaBtmCenter = (Hlong)(ChipAreaBtm-10);

	select_shape(RegionClosing, &RsltRgn0, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple(10));
	select_shape(RsltRgn0, &RsltRgn0, HTuple("row"), HTuple("and"), HTuple(ChipAreaTop), HTuple(ChipAreaTopCenter));

	select_shape(RegionClosing, &RsltRgn1, HTuple("height"), HTuple("and"), HTuple(tolerance), HTuple(10));
	select_shape(RsltRgn1, &RsltRgn1, HTuple("row"), HTuple("and"), HTuple(ChipAreaBtmCenter), HTuple(ChipAreaBtm));

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

//	union2(RsltRgn0, RsltRgn1, &RsltRgn0);

	connection(RsltRgn0, &RsltRgn0);

	count_obj(RsltRgn0, &Num);
	
	//CHOKI20110523 //조건 제외시 검출만 되고 이력이 남지않음
//	*rslt_num = (Hlong)Num[0]; 
    Hlong ResultCount;
	ResultCount = (Hlong)Num[0];
	//CHOKI20110523

	if(outcount <= (Hlong)Num[0])
	{
		for(int i = 0; i < (Hlong)Num[0]; i++)
		{
			select_obj(RsltRgn0, &SelectRgn, i + 1);
			intensity(SelectRgn, *image2, &Mean, _);
			if((Hlong)Mean[0]<200)
			{
				union2(SelectRgn, *rslt_region, rslt_region);	

				if(win)
				{
					set_color(win, HTuple("red"));
					disp_obj(*rslt_region, win);
				}
				ReturnRslt = FALSE;
			}
		}
		//CHOKI20110523
		connection(*rslt_region, rslt_region); 
		count_obj(*rslt_region, &Num);
		*rslt_num = (Hlong)Num[0]; 
		//CHOKI20110523
	}

	return ReturnRslt;
}

//CHOKI20100722 -> 20100725
BOOL C_VisionModule::_Hal_ChipOut2(Hlong win, Hobject *image, Hobject *image2, Hobject *image3, Hobject chip_ins_region, Hobject chip_ins_region1,
						  double offset_ox, double offset_oy, 
						  double offset_ox2, double offset_oy2, 
						  double offset_rx, double offset_ry,
						  double offset_rx2, double offset_ry2,
						 int thresh, int high_thresh, int tolerance, Hlong *rslt_num, Hobject *rslt_region,
						 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1;
	HTuple Num;
	HTuple DiffAng, HomMat2D;
	Hobject CenterRgn0, CenterRgn1, LeftRightRgn, RsltRgn2, RsltRgn3, InsRgn2;
	HTuple MeanLT, MeanLB, MeanRT, MeanRB;
	HTuple MeanLT2, MeanLB2, MeanRT2, MeanRB2, MeanChip;
	Hobject LTRgnintensity, LBRgnintensity, RTRgnintensity, RBRgnintensity;
	Hobject InLTRgn0, InLBRgn0, InRTRgn0, InRBRgn0;//CHOKI20100810
	Hobject LTRgn0, LBRgn0, RTRgn0, RBRgn0;
	Hobject LTRgn1, LBRgn1, RTRgn1, RBRgn1;
	Hobject LTReduced, LBReduced, RTReduced, RBReduced;
	Hobject LTRsltRgn, LBRsltRgn, RTRsltRgn, RBRsltRgn, RegionTot;
//	CRect ChipAreaRect; 
	HTuple CRow0, CCol0, CRow1, CCol1;
	HTuple CRow3, CCol3, Phi3, ChipHeight3, ChipWidth3;//CHOKI20100810

	//CHOKI20110511
	Hobject SelectRgn;
	HTuple Mean;
	BOOL ReturnRslt;

	smallest_rectangle1(chip_ins_region1, &CRow0, &CCol0, &CRow1, &CCol1);


	//CHOKI20100725
	CRect ChipLTRect; 
	CRect ChipLBRect; 
	CRect ChipRTRect; 
	CRect ChipRBRect; 

	//CHOKI20100810
	CRect InChipLTRect; 
	CRect InChipLBRect; 
	CRect InChipRTRect; 
	CRect InChipRBRect; 
	
	double offset_x, offset_y;
//	int tol;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	float OffsetLengthX;

	OffsetLengthX = fabs(offset_ox-offset_ox2) - fabs(offset_rx-offset_rx2); //CHOKI20110715

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx+OffsetLengthX, HTuple(0), &HomMat2D);
//	vector_angle_to_rigid(offset_ry, offset_rx, DiffAng, offset_oy, offset_ox, HTuple(0), &HomMat2D); //CHOKI20110524 affine_trans_region의 참조값이 바꿔있어 영역을 정상적으로 찾지 못함


	smallest_rectangle1(chip_ins_region1,&CRow0,&CCol0,&CRow1,&CCol1);

	InChipLTRect.top = (Hlong)CRow0[0]+1;
	InChipLTRect.bottom = (Hlong)CRow0[0]+6;
	InChipLTRect.left = (Hlong)CCol0[0]+1;//-OffsetLengthX; //CHOKI20110711
	InChipLTRect.right = (Hlong)CCol0[0]+6;//-OffsetLengthX; //CHOKI20110711
	
	InChipLBRect.top = (Hlong)CRow1[0]-6;
	InChipLBRect.bottom = (Hlong)CRow1[0]-1;
	InChipLBRect.left = (Hlong)CCol0[0]+1;//-OffsetLengthX; //CHOKI20110711
	InChipLBRect.right = (Hlong)CCol0[0]+6;//-OffsetLengthX; //CHOKI20110711
	
	InChipRTRect.top = (Hlong)CRow0[0]+1;
	InChipRTRect.bottom = (Hlong)CRow0[0]+6;
	InChipRTRect.left = (Hlong)CCol1[0]-6-OffsetLengthX;
	InChipRTRect.right = (Hlong)CCol1[0]-1-OffsetLengthX;
	
	InChipRBRect.top = (Hlong)CRow1[0]-6;
	InChipRBRect.bottom = (Hlong)CRow1[0]-1;
	InChipRBRect.left = (Hlong)CCol1[0]-6-OffsetLengthX;
	InChipRBRect.right = (Hlong)CCol1[0]-1-OffsetLengthX;


	gen_rectangle1(&InLTRgn0, InChipLTRect.top, InChipLTRect.left, InChipLTRect.bottom, InChipLTRect.right);
//	dilation_rectangle1(InLTRgn0, &InLTRgn0, 2, 2);
	
	gen_rectangle1(&InLBRgn0, InChipLBRect.top, InChipLBRect.left, InChipLBRect.bottom, InChipLBRect.right);
//	dilation_rectangle1(InLBRgn0, &InLBRgn0, 2, 2);
	
	gen_rectangle1(&InRTRgn0, InChipRTRect.top, InChipRTRect.left, InChipRTRect.bottom, InChipRTRect.right);
//	dilation_rectangle1(InRTRgn0, &InRTRgn0, 2, 2);

	gen_rectangle1(&InRBRgn0, InChipRBRect.top, InChipRBRect.left, InChipRBRect.bottom, InChipRBRect.right);
//	dilation_rectangle1(InRBRgn0, &InRBRgn0, 2, 2);

	affine_trans_region(InLTRgn0, &LTRgn0, HomMat2D, HTuple("false"));
	affine_trans_region(InLBRgn0, &LBRgn0, HomMat2D, HTuple("false"));
	affine_trans_region(InRTRgn0, &RTRgn0, HomMat2D, HTuple("false"));
	affine_trans_region(InRBRgn0, &RBRgn0, HomMat2D, HTuple("false"));
	//CHOKI20100810

	reduce_domain(*image, LTRgn0, &LTReduced);
	reduce_domain(*image, LBRgn0, &LBReduced);
	reduce_domain(*image, RTRgn0, &RTReduced);
	reduce_domain(*image, RBRgn0, &RBReduced);
	
	//CHOKI20110510
	gen_empty_obj(&LTRsltRgn);
	gen_empty_obj(&LBRsltRgn);
	gen_empty_obj(&RTRsltRgn);
	gen_empty_obj(&RBRsltRgn);

	intensity(LTRgn0, *image, &MeanLT, _);//CHOKI20100727
	if((Hlong)MeanLT[0] < 200)
	{
		Herror herr;
		CString status;

		set_check("~give_error");
		herr = threshold(LTReduced,&LTRgn1,HTuple(5),HTuple(140)); //CHOKI20110725
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_28!");
			WriteErrorList((LPCTSTR)status);
			return FALSE;
		}

		connection(LTRgn1, &LTRgn1);
		select_shape(LTRgn1, &LTRsltRgn, HTuple("area"), HTuple("and"), HTuple(15), HTuple("max"));
		select_shape(LTRsltRgn, &LTRsltRgn, HTuple("width"), HTuple("and"), HTuple(3), HTuple("max"));//CHOKI20100903
	}


	intensity(LBRgn0, *image, &MeanLB, _);//CHOKI20100727
	if((Hlong)MeanLB[0] < 200)
	{
		Herror herr;
		CString status;

		set_check("~give_error");
		herr = threshold(LBReduced,&LBRgn1,HTuple(5),HTuple(140)); //CHOKI20110725
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_29!");
			WriteErrorList((LPCTSTR)status);
			return FALSE;
		}	
	
		connection(LBRgn1, &LBRgn1);
		select_shape(LBRgn1, &LBRsltRgn, HTuple("area"), HTuple("and"), HTuple(15), HTuple("max"));
		select_shape(LBRsltRgn, &LBRsltRgn, HTuple("width"), HTuple("and"), HTuple(3), HTuple("max"));//CHOKI20100903


	}

	intensity(RTRgn0, *image, &MeanRT, _);//CHOKI20100727
	if((Hlong)MeanRT[0] < 200)
	{
		Herror herr;
		CString status;

		set_check("~give_error");
		herr = threshold(RTReduced,&RTRgn1,HTuple(5),HTuple(140)); //CHOKI20110725
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_30!");
			WriteErrorList((LPCTSTR)status);
			return FALSE;
		}
		
		connection(RTRgn1, &RTRgn1);
		select_shape(RTRgn1, &RTRsltRgn, HTuple("area"), HTuple("and"), HTuple(15), HTuple("max"));
		select_shape(RTRsltRgn, &RTRsltRgn, HTuple("width"), HTuple("and"), HTuple(3), HTuple("max"));//CHOKI20100903
	}


	intensity(RBRgn0, *image, &MeanRB, _);//CHOKI20100727
	if((Hlong)MeanRB[0] < 200)
	{
		Herror herr;
		CString status;

		set_check("~give_error");
		herr = threshold(RBReduced,&RBRgn1,HTuple(5),HTuple(140)); //CHOKI20110725
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_31!");
			WriteErrorList((LPCTSTR)status);
			return FALSE;
		}
		
		connection(RBRgn1, &RBRgn1);
		select_shape(RBRgn1, &RBRsltRgn, HTuple("area"), HTuple("and"), HTuple(15), HTuple("max"));
		select_shape(RBRsltRgn, &RBRsltRgn, HTuple("width"), HTuple("and"), HTuple(3), HTuple("max"));//CHOKI20100903
	}

	gen_empty_obj(&RegionTot);
	union2(LTRsltRgn,RegionTot,&RegionTot);//CHOKI20100805 분리
	union2(LBRsltRgn,RegionTot,&RegionTot);
	union2(RTRsltRgn,RegionTot,&RegionTot);
	union2(RBRsltRgn,RegionTot,&RegionTot);

	ReturnRslt = TRUE;

	connection(RegionTot, &RegionTot);
	count_obj(RegionTot, &Num);
	
	//CHOKI20110523 //조건 제외시 검출만 되고 이력이 남지않음
//	*rslt_num = (Hlong)Num[0]; 
    Hlong ResultCount;
	ResultCount = (Hlong)Num[0];
	//CHOKI20110523

	if(0 < (Hlong)Num[0])
	{
		for(int i = 0; i < (Hlong)Num[0]; i++)
		{
			select_obj(RegionTot, &SelectRgn, i + 1);   //CHOKI20110512 Down
			intensity(SelectRgn, *image2, &Mean, _);
			if((Hlong)Mean[0]>50)  //CHOKI20110618 //CHOKI20110811 칩연마흔적으로 인한 과검으로 수치조정 40->50
			{
				union2(*rslt_region,SelectRgn, rslt_region);	
				
				if(win)
				{
					set_color(win, HTuple("red"));
					disp_obj(*rslt_region, win);
				}
				ReturnRslt = FALSE;
			}
		}
		//CHOKI20110523
		connection(*rslt_region, rslt_region); 
		count_obj(*rslt_region, &Num);
		*rslt_num = (Hlong)Num[0]; 
		//CHOKI20110523
	}
	return ReturnRslt;
}

BOOL C_VisionModule::ChipMold(Hlong win, Hobject *image, Hobject *broken_image, Hobject chip_ins_region,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int high_thresh, int tolerance, int broken_thresh, 
							  Hlong *rslt_num, Hobject *rslt_region, Hlong *rslt_broken_num, Hobject *rslt_broken_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;


	rslt = _Hal_ChipMold(win, image, broken_image, chip_ins_region, 
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							  high_thresh, tolerance, broken_thresh, 
							  rslt_num, rslt_region, rslt_broken_num, rslt_broken_region,
							  min_area, min_width, min_height);

	return rslt;	



}

BOOL  C_VisionModule::_Hal_ChipMold(Hlong win, Hobject *image, Hobject *broken_image, Hobject chip_ins_region, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int high_thresh, int tolerance, int broken_thresh, 
							  Hlong *rslt_num, Hobject *rslt_region, Hlong *rslt_broken_num, Hobject *rslt_broken_region,
							  Hlong min_area, Hlong min_width, Hlong min_height)
{
	BOOL rslt = TRUE;
	Hobject ImageReduced, Region, RegionClosing, InsRegion, Line, Image;
	HTuple Num, Area;
	Hlong chip_area, tol_area;
	HTuple RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond, ColumnEdgeSecond, AmplitudeSecond, MeasureHandle, IntraDistance, InterDistance;

	Hlong row, col, leng1, height;
	HTuple Row0, Column0;
	RECT ins_region;
	HTuple Width, Height;

	Hobject RsltRgn, RsltBrokenRgn, ChipRgn1;

	HTuple DiffAng, HomMat2D;
	double offset_x, offset_y;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, _, _, &Width, &Height);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_24!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	gen_empty_obj(&RsltRgn);
	gen_empty_obj(&RsltBrokenRgn);

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);
	
	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;
	
	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(chip_ins_region, &ChipRgn1, HomMat2D, HTuple("false"));

	//Mold칩 이면 결함. //////////////////////////////////////
	erosion_rectangle1(ChipRgn1, &InsRegion, 5, 5);
	reduce_domain(*image, InsRegion, &ImageReduced);

	set_check("~give_error");

	herr = threshold(ImageReduced,&Region,HTuple(0),HTuple(high_thresh));

	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_32!... %ld, %ld", (Hlong)0, (Hlong)high_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	area_center(chip_ins_region, &Area, _, _);
	chip_area = (Hlong)Area[0];

	if(0 == tolerance)
	{
		tol_area = min_area;
	}
	else
	{
		tol_area = (Hlong)( ((double)chip_area) * (tolerance * 0.01) );
	}

	select_shape(RegionClosing, &RsltRgn, HTuple("area"), HTuple("and"), HTuple(1),  HTuple("max"));
	select_shape(RsltRgn, &RsltRgn, HTuple("compactness"), HTuple("and"), HTuple(1),  HTuple(20));
	
	connection(RsltRgn, &RsltRgn);

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

	count_obj(RsltRgn, &Num);

	if(0 < (Hlong)Num[0])
	{
		union1(RsltRgn, &RsltRgn);
		area_center(RsltRgn, &Area, _, _);
		if(tol_area < (Hlong)Area[0])
		{
			if(win)
			{
				set_color(win, HTuple("red"));
				disp_obj(RsltRgn, win);
			}

			
			*rslt_num = 1;

			rslt = FALSE;
		}
		else
		{
			*rslt_num = 0;

		}
	}

	copy_obj(RsltRgn, rslt_region, 1, -1);

	//Broken/////////////////////////////////////
	mean_image(*broken_image, &Image, 20, 1);
	emphasize(Image, &Image, 5, 3, 5);

	if(win)
	{
		disp_obj(Image, win);
	}

	smallest_rectangle1(InsRegion, &Row0, &Column0, &Row1, &Column1);

	ins_region.left = (Hlong)Column0[0];
	ins_region.top = (Hlong)Row0[0];
	ins_region.right = (Hlong)Column1[0];
	ins_region.bottom = (Hlong)Row1[0];

	row = (ins_region.top + ins_region.bottom) * 0.5;
	col = (ins_region.left + ins_region.right) * 0.5;
	leng1 = (ins_region.bottom - ins_region.top) * 0.4;
	height = (ins_region.right - ins_region.left) * 0.4;

	*rslt_broken_num = 0;
	gen_measure_rectangle2(row,col,HTuple(1.57079632679),leng1,height,Width,Height,HTuple("nearest_neighbor"),&MeasureHandle);

	measure_pairs(Image, MeasureHandle, HTuple(1), HTuple(broken_thresh), HTuple("negative"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,  
						&RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeSecond,
						&IntraDistance, &InterDistance);


	//20100416 lee
	Hlong edgeCnt;

	edgeCnt = RowEdgeFirst.Num();

	if( (0 < RowEdgeFirst.Num()) && (3 > RowEdgeFirst.Num()) ) //너무 많으면 broken이 아니라 chip grind가 잘못된것->양품처리
	{
		//gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);
		gen_region_line(&Line, RowEdgeFirst, ColumnEdgeFirst, RowEdgeSecond,ColumnEdgeSecond);

		connection(Line, &Line);
		select_shape(Line, &Line, HTuple("area"), HTuple("and"), HTuple(1),  HTuple(20));
		count_obj(Line, &Num);
		
		if(0 < (Hlong)Num[0])
		{
			union2(RsltBrokenRgn, Line, &RsltBrokenRgn);

			*rslt_broken_num = 1;
			if(win)
			{
				set_color(win, HTuple("red"));
				disp_obj(RsltBrokenRgn, win);
			}
			rslt = FALSE;
		}
	}

	copy_obj(RsltBrokenRgn, rslt_broken_region, 1, -1);

	close_measure(MeasureHandle);

	return rslt;
}

BOOL C_VisionModule::PotPart(Hlong win, Hobject *image, Hobject *image2, Hobject *crack_image, Hobject chip_ins_region, Hobject pot_ins_region, Hobject sr_region,
							 double offset_ox, double offset_oy, 
							 double offset_ox2, double offset_oy2, 
							 double offset_rx, double offset_ry,
							 double offset_rx2, double offset_ry2,
							 int part_thresh, int part2_thresh, int high_thresh, int tolerance, int range_tolerance, int black_pot_tol, int black_thresh,
							 Hlong *rslt_num, Hobject *rslt_region, Hlong *range_rslt_num, Hobject *range_rslt_region, Hobject *recog_ins_rslt_region)
{
	//BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height, recog_dist, min_area_sr, min_width_sr, min_height_sr;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;
	recog_dist = RECOG_DIST;
	min_area_sr = VISION_FACT_MIN_AREA_SR;
	min_width_sr = VISION_FACT_MIN_WIDTH_SR;
	min_height_sr = VISION_FACT_MIN_HEIGHT_SR;

	/*
	rslt = Hal_PotPart(win, image, crack_image, chip_ins_region, pot_ins_region, sr_region,
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							 part_thresh, high_thresh, tolerance, range_tolerance, black_pot_tol, black_thresh,
							 rslt_num, rslt_region, range_rslt_num, range_rslt_region, recog_ins_rslt_region,
							 min_area, min_width, min_height, recog_dist,
							 min_area_sr, min_width_sr, min_height_sr);

	return rslt;
	*/

	BOOL rslt = TRUE;
	Hobject ImageReduced, Region, RegionClosing, DilationRegion, ImageReduced2, Region2;
	HTuple Num;
	Hobject FillRegion, RangeRegion, ConvRegion;
	HTuple Row0, Col0, Row1, Col1;
	Hobject RecogRgn;
	Hobject PotBlackPart, RlstPotBlackPart;

	Hobject OutRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean;
	int diff_var;
	Hobject ComplRgn;
	Hobject PosChkRgnTop, PosChkRgnRight, PosChkRgnBottom, PosChkRgnLeft;
	HTuple AreaTop, AreaBottom, AreaLeft, AreaRight;

	Hobject SRRegion, RecogRegion, RsltRegion, RangeRsltRegion, RecogRsltRegion;

	Hobject Pot2InsRgn, Pot2RsltRgn, Pot2ImageReduced, Pot2FillRegion, Pot2DilationRegion;

	Hlong num, i;

	HTuple HomMat2D, DiffAng, Area;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(sr_region, &SRRegion, HomMat2D, HTuple("false"));

	//Particle///////////////////////////////////
	fill_up(pot_ins_region, &FillRegion);

	//튀어나온 부분을 없에기
	closing_circle(FillRegion, &FillRegion, 3.5);
	erosion_circle(FillRegion, &FillRegion, 6.5);
	//작게 분리된 영역 없에기
	connection(FillRegion, &FillRegion);
	select_shape(FillRegion, &FillRegion, HTuple("area"), HTuple("and"), HTuple(100), HTuple("max"));
	union1(FillRegion, &FillRegion);
	fill_up(FillRegion, &FillRegion);

	dilation_circle(chip_ins_region, &DilationRegion, 8.5);
	
	difference(FillRegion, DilationRegion, &Region);//Region = Potting 영역 (섬세한 부분은 제거) 

	//Pot2검사용
	dilation_rectangle1(chip_ins_region, &Pot2DilationRegion, 20.5, 20.5);
	fill_up(pot_ins_region, &Pot2FillRegion);
	closing_circle(Pot2FillRegion, &Pot2FillRegion, 3.5);
	difference(Pot2FillRegion, Pot2DilationRegion, &Pot2InsRgn); //Pot2InsRgn = Potting 영역
	

	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&Pot2RsltRgn);
	gen_empty_obj(&RangeRsltRegion);
	gen_empty_obj(&RecogRsltRegion);

	//Don't care region 좌상, 좌하, 우상, 우하(인식기 부분)
	gen_empty_obj(&RecogRgn);
	smallest_rectangle1(chip_ins_region,&Row0,&Col0,&Row1,&Col1);

	gen_rectangle1(&RecogRgn,Row0 - recog_dist, Col0 - recog_dist, Row0, Col0);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);

	gen_rectangle1(&RecogRgn,Row1, Col0 - recog_dist, Row1 + recog_dist, Col0);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);

	gen_rectangle1(&RecogRgn,Row0 - recog_dist, Col1, Row0, Col1 + recog_dist);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);

	gen_rectangle1(&RecogRgn,Row1, Col1, Row1 + recog_dist, Col1 + recog_dist);
	concat_obj(RecogRsltRegion, RecogRgn, &RecogRsltRegion);
	difference(Region, RecogRsltRegion, &Region);

	difference(Pot2InsRgn, RecogRsltRegion, &Pot2InsRgn);


	copy_obj(RecogRsltRegion, recog_ins_rslt_region, 1, -1);

	//Check position region
	//Top
	gen_rectangle1(&PosChkRgnTop, Row0 - 150, Col0, Row0 - 5, Col1);
	//Bottom
	gen_rectangle1(&PosChkRgnBottom, Row1 + 5, Col0, Row1 + 150, Col1);
	//Left
	gen_rectangle1(&PosChkRgnLeft, Row0, Col0 - 150, Row1, Col0 - 5);
	//Right
	gen_rectangle1(&PosChkRgnRight, Row0, Col1 + 5, Row1, Col1 + 150);

	select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(100), HTuple("max"));
	count_obj(Region, &Num);
	Hlong test;
	test = ((Hlong)Num[0]);
	if(1 <= ((Hlong)Num[0]) )
	{
		union1(Region, &Region);
	}
	else
	{
		return TRUE;
	}

	reduce_domain(*image, Region, &ImageReduced);			//Potting 영역 #1
	reduce_domain(*image2, Pot2InsRgn, &Pot2ImageReduced); // Pottin 영역 #2

	dilation_circle(chip_ins_region, &Region2, 13);
	difference(Region, Region2, &Region2);//원래 첫번째 검사영역에서...
	erosion_circle(SRRegion, &SRRegion, 3.5);
	intersection(Region2, SRRegion, &Region2);//SR region영역에서 찾는다.(두번째 이미지에서는->pot부분에 흰색 띠 형성때문(조명반사))

	if(win)
	{
		set_color(win, HTuple("cyan"));
		disp_obj(SRRegion, win);
	}	
		
	reduce_domain(*crack_image, Region2, &ImageReduced2); // Potting 영역 #3
	
	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}	
		
	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(Region2, win);
	}

	if( (part_thresh > 255) || (high_thresh > 255) )
	{
		part_thresh = 230;
		high_thresh = 255;
	}
	if( (part_thresh < 0) || (high_thresh < 0) )
	{
		part_thresh = 0;
		high_thresh = 20;
	}
	high_thresh = part_thresh + 20;
	if(high_thresh > 255)
	{
		part_thresh = 235;
		high_thresh = 255;
	}
	hysteresis_threshold(ImageReduced,&Region,HTuple(part_thresh),HTuple(high_thresh),HTuple(10));
	hysteresis_threshold(ImageReduced2,&Region2,HTuple(part_thresh),HTuple(high_thresh),HTuple(10));
	union2(Region, Region2, &Region);
	

	//Pot중에서 작은것은 dark 수지묻음으로 인정//
	connection(pot_ins_region, &PotBlackPart);
	select_shape(PotBlackPart, &PotBlackPart, HTuple("area"), HTuple("and"), HTuple(0), HTuple(5000));
	//Port particle에서 제외하고 그것들을 경계값 비교한다. 즉 pot영역외 작은것은 경계값 비교하여 불량은 선별
	difference(Region, PotBlackPart, &RsltRegion);
	select_shape(PotBlackPart, &PotBlackPart, HTuple("width").Concat("height"), HTuple("and"), HTuple(black_pot_tol).Concat(black_pot_tol), HTuple("max").Concat("max"));
	//경계값과 비교
	count_obj(PotBlackPart, &Num);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(PotBlackPart, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 2);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, *image, &Mean, _);
		intensity(OutRgn, *image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		if(0 < diff_var)
		{
			if((black_thresh + 10) < diff_var)
			{
				concat_obj(RlstPotBlackPart, SelectRgn, &RlstPotBlackPart);
			}
		}
	}
	union2(RsltRegion, RlstPotBlackPart, &RsltRegion);
	/////////////////////////////////////////////

	
	connection(RsltRegion, &RsltRegion);
	closing_circle(RsltRegion,&RsltRegion,HTuple(3.5));
	
	select_shape(RsltRegion, &RsltRegion, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(RsltRegion, &RsltRegion, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(RsltRegion, &RsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	//영상이 약간 밀려 오버킬 발생 됨->폭이 칩정도 되면 양품처리(300 : 일반적인 칩에 폭에 2/3수준)
	select_shape(RsltRegion, &RsltRegion, HTuple("width"), HTuple("and"), HTuple("min"), HTuple(80));
	//길쭉한것은 무시(장축단축비)
	select_shape(RsltRegion, &RsltRegion, HTuple("anisometry"), HTuple("and"), HTuple(0), HTuple(10));


	//LSS 201040406
	//Pot#2//////////////////////////////////////
	threshold(Pot2ImageReduced,&Pot2RsltRgn,HTuple(0),HTuple(part2_thresh));
	
	connection(Pot2RsltRgn, &Pot2RsltRgn);
	//closing_circle(Pot2RsltRgn,&Pot2RsltRgn,HTuple(3.5));
	
	select_shape(Pot2RsltRgn, &Pot2RsltRgn, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(Pot2RsltRgn, &Pot2RsltRgn, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(Pot2RsltRgn, &Pot2RsltRgn, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	union2(RsltRegion, Pot2RsltRgn, &RsltRegion);
	connection(RsltRegion, &RsltRegion);

	Hobject select_object;
	HTuple Row2, Column2;
	
	count_obj(RsltRegion, &Num);
	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
		}
		
		rslt = FALSE;
	}

	copy_obj(RsltRegion, rslt_region, 1, -1);
	

	//도포불량///////////////////////////////////
	connection(pot_ins_region, &RangeRegion);
	select_shape(RangeRegion, &RangeRegion, HTuple("area"), HTuple("and"), HTuple(100),  HTuple("max"));
	select_shape(RangeRegion, &RangeRegion, HTuple("width"), HTuple("and"), HTuple(100),  HTuple("max"));
	select_shape(RangeRegion, &RangeRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(range_tolerance).Concat(range_tolerance), HTuple("max").Concat("max"));

	union1(RangeRegion, &RangeRegion);

	//20100310 lss
	opening_circle(RangeRegion, &RangeRegion, 3.5);

	shape_trans(RangeRegion, &ConvRegion, "convex");
	fill_up(pot_ins_region, &FillRegion);
	difference(ConvRegion, FillRegion, &Region);
	//Don't care region 좌상, 좌하, 우상, 우하(인식기 부분)
	dilation_rectangle1(*recog_ins_rslt_region, &RecogRegion, 20, 20);
	difference(Region, RecogRegion, &Region);



	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	//20100315 lss
	select_shape(RegionClosing, &RegionClosing, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple(20000));
	select_shape(RegionClosing, &RegionClosing, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
	select_shape(RegionClosing, &RangeRsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(range_tolerance).Concat(range_tolerance), HTuple("max").Concat("max"));

	count_obj(RangeRsltRegion, &Num);

	Hlong top, bot, left, right;

	if(0 < (Hlong)Num[0])
	{
		//불량발생 위치별 조건
		//Top
		intersection(PosChkRgnTop, RangeRsltRegion, &PosChkRgnTop);
		area_center(PosChkRgnTop, &AreaTop, _, _);

		//Bottom
		intersection(PosChkRgnBottom, RangeRsltRegion, &PosChkRgnBottom);
		area_center(PosChkRgnBottom, &AreaBottom, _, _);

		//Left
		intersection(PosChkRgnLeft, RangeRsltRegion, &PosChkRgnLeft);
		area_center(PosChkRgnLeft, &AreaLeft, _, _);

		//Right
		intersection(PosChkRgnRight, RangeRsltRegion, &PosChkRgnRight);
		area_center(PosChkRgnRight, &AreaRight, _, _);

		top = 0;
		bot = 0;
		left = 0;
		right = 0;
		top = (Hlong)AreaTop[0];
		bot = (Hlong)AreaBottom[0];
		left = (Hlong)AreaLeft[0];
		right = (Hlong)AreaRight[0];

		if(0 < (Hlong)AreaTop[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("height"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else if(0 < (Hlong)AreaBottom[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("height"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else if(0 < (Hlong)AreaLeft[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("width"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else if(0 < (Hlong)AreaRight[0])
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("width"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
		}
		else
		{
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("height"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));
			select_shape(RangeRsltRegion, &RangeRsltRegion, HTuple("width"), HTuple("and"), HTuple(range_tolerance), HTuple("max"));

		}

		count_obj(RangeRsltRegion, &Num);
		*range_rslt_num = (Hlong)Num[0];
		if(0 < *range_rslt_num)
		{

			if(win)
			{
				set_color(win, HTuple("orange"));
				disp_obj(RangeRsltRegion, win);
			}
			
			rslt = FALSE;
		}
	}

	copy_obj(RangeRsltRegion, range_rslt_region, 1, -1);

	return rslt;

}

/////////// Masterimage: GetHalRefRedImage(cam_idx, 1),  image2: GetHalRedImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[6])
//CHOKI20100811
BOOL C_VisionModule::PotPart2(Hlong win, Hobject *Masterimage, Hobject *image, Hobject *image2, 
							  Hobject *crack_image, Hobject *image5, //CHOKI20110619
							  Hobject *image6, //CHOKI201107010
							  Hobject *RefGreenImage, //CHOKI20110824
							  Hobject chip_ins_region, RECT ins_region, Hobject pot_ins_region,
							 double offset_ox, double offset_oy, 
							 double offset_ox2, double offset_oy2, 
							 double offset_rx, double offset_ry,
							 double offset_rx2, double offset_ry2,
							 int part_thresh, int part2_thresh, int high_thresh, int tolerance, int tolerance_hide_part, int range_tolerance, int black_pot_tol, int black_thresh,
							 Hlong *rslt_num, Hobject *rslt_region, Hlong *range_rslt_num, Hobject *range_rslt_region, Hobject *recog_ins_rslt_region)
{
	Hlong min_area, min_width, min_height, recog_dist, min_area_sr, min_width_sr, min_height_sr;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;
	recog_dist = RECOG_DIST;
	min_area_sr = VISION_FACT_MIN_AREA_SR;
	min_width_sr = VISION_FACT_MIN_WIDTH_SR;
	min_height_sr = VISION_FACT_MIN_HEIGHT_SR;


	BOOL rslt = TRUE;
	Hobject ImagePotReduced, ImageMastertReduced, SubReduced, Region, RegionClosing, DilationRegion, ImageReduced2, Region2;
	HTuple Num;
	Hobject FillRegion, RangeRegion, ConvRegion;
	HTuple Row0, Col0, Row1, Col1;
	Hobject RecogRgn;
	Hobject Test1, 	test2;
	Hobject InverPot, InverMaster;
	Hobject erosionRegion;
	Hobject AlignRegion;
	Hobject MasterImageRegion, PotImageRegion;

	Hobject OutRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean;

	Hobject ChipRegion, PotRegion, RecogRegion, RsltRegion;
	Hobject PotRegion2; //CHOKI20110619

	Hobject Pot2InsRgn, Pot2RsltRgn, Pot2ImageReduced, Pot2FillRegion, Pot2DilationRegion;
	Hobject threshImageMaster, threshImagePot;

	HTuple HomMat2D, DiffAng, Area;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	//Chip 영역 회전 
	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);

	//Pot 검사 영역 생성 
	gen_rectangle1(&PotRegion, ins_region.top, ins_region.left, ins_region.bottom, ins_region.right);
	affine_trans_region(PotRegion, &PotRegion2, HomMat2D, HTuple("false"));//CHOKI20110619
	
	//CHOKI20110215
	Hobject PotRgn, ThreshPotRgn, SelectPotRgn; 
	reduce_domain(*crack_image, PotRegion, &PotRgn);

	Herror herr;
	CString status;

	set_check("~give_error");
#ifdef AVI_TYPE_1
	herr = threshold(PotRgn,&ThreshPotRgn,HTuple(190),HTuple(255)); //CHOKI20110725		//AVI_TYPE_1//
#else
	herr = threshold(PotRgn,&ThreshPotRgn,HTuple(180),HTuple(255)); //CHOKI20110725 //CHOKI20110926 170->180	
#endif	
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_34!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	select_shape(ThreshPotRgn, &SelectPotRgn, HTuple("area"), HTuple("and"), HTuple(tolerance), HTuple(50000));
	count_obj(SelectPotRgn, &Num);
	
	//CHOKI20100812
	//Pot Search 영역에서 Chip 영역 제거  
	erosion_rectangle1(pot_ins_region, &SubReduced, 4, 4);
	difference(PotRegion, SubReduced, &PotRegion);
	affine_trans_region(PotRegion, &PotRegion, HomMat2D, HTuple("false"));//CHOKI20110118
 
	copy_obj(*image2, &PotImageRegion, 1, -1);
	copy_obj(*Masterimage, &MasterImageRegion, 1, -1);
 
	//Pattern 추출     
	emphasize(PotImageRegion, &PotImageRegion, 5, 5, 1);
	emphasize(MasterImageRegion, &MasterImageRegion, 5, 5, 3);  //20110521 lee

	set_check("~give_error");
	herr = threshold(PotImageRegion,&threshImagePot,HTuple(0),HTuple(100));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_35!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	set_check("~give_error");
	herr = threshold(MasterImageRegion,&threshImageMaster,HTuple(0),HTuple(110));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_36!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	dilation_rectangle1(threshImageMaster, &threshImageMaster, 8, 5);

	affine_trans_region(threshImageMaster, &AlignRegion, HomMat2D, HTuple("false"));

	//CHOKI20110619
	Hobject RefImageTrans; 
	affine_trans_image(*Masterimage, &RefImageTrans, HomMat2D, HTuple("constant"), HTuple("false"));
	//CHOKI20110824
	Hobject RefGreenImageTrans; 
	affine_trans_image(*RefGreenImage, &RefGreenImageTrans, HomMat2D, HTuple("constant"), HTuple("false"));

	//원본 이미지에서 검사 영역 만큼 영역 추출  
	reduce_domain(threshImagePot, PotRegion, &ImagePotReduced);
	reduce_domain(AlignRegion, PotRegion, &ImageMastertReduced);

	dilation_rectangle1(ImageMastertReduced, &DilationRegion, 8, 5);
	erosion_rectangle1(DilationRegion, &DilationRegion, 2, 1);

	difference(ImagePotReduced, DilationRegion, &RecogRegion);

	connection(RecogRegion, &RecogRegion);

	select_shape(RecogRegion, &Pot2RsltRgn, HTuple("area"), HTuple("and"), HTuple(tolerance), HTuple(500)); //20110519 lee 10->tolerance
	//20100818 외부에서 치수 넣을 수 있게...
	select_shape(Pot2RsltRgn, &Pot2RsltRgn, HTuple("width").Concat("height"), HTuple("and"), HTuple(tolerance).Concat(tolerance), HTuple(50).Concat(50));
	connection(Pot2RsltRgn, &Pot2RsltRgn);

	count_obj(Pot2RsltRgn, &Num);

	//CHOKI20110607
	Hlong CountNum = 0;
	HTuple lCenterX, lCenterY, lPi, lWidth, lHeight;
	Hobject TempResult;
	HTuple MeanGreen5, MeanGreen5Side; //CHOKI20110808
	Hobject DilationSide; //CHOKI20110808
	Hlong absDiffGreen5; //CHOKI20110808

	gen_empty_obj(&TempResult);

	CountNum = (Hlong)Num[0];
	if(0 < CountNum)
	{
		for(int i = 0; i < CountNum; i++)
		{
			select_obj(Pot2RsltRgn, &SelectRgn, i + 1);
			smallest_rectangle2(SelectRgn,&lCenterX, &lCenterY, &lPi, &lWidth, &lHeight);  
			intensity(SelectRgn, *image2, &Mean, _);

			//CHOKI20110808
			intensity(SelectRgn, *image5, &MeanGreen5, _);
			dilation_rectangle1(SelectRgn, &DilationSide, 10, 1);
			intensity(DilationSide, *image5, &MeanGreen5Side, _);
			absDiffGreen5 = abs( (long)((Hlong)MeanGreen5[0] - (Hlong)MeanGreen5Side[0]) );
			//CHOKI20110808
#ifdef AVI_TYPE_1
			if((double)lWidth[0] < (double)lHeight[0]*2.5 && (Hlong)Mean[0] < 65 )//&& 5 < absDiffGreen5) //CHOKI20110711 100->90 //CHOKI20110720 90->80 //CHOKI20110808 검출영역의 좌우 확장영역 밝기값이 영역의 밝기값과 차이가 없을 경우 과검으로 제거함.		//AVI_TYPE_1//
			{//CHOKI20110725 80->70 //CHOKI20110921  (5 < absDiffGreen5) 제거 //CHOKI20111102 80->65
#else
			if((double)lWidth[0] < (double)lHeight[0]*2.5 && (Hlong)Mean[0] < 70 && 5 < absDiffGreen5) //CHOKI20110711 100->90 //CHOKI20110720 90->80 //CHOKI20110808 검출영역의 좌우 확장영역 밝기값이 영역의 밝기값과 차이가 없을 경우 과검으로 제거함.
			{//CHOKI20110725 80->70
#endif
				union2(TempResult, SelectRgn, &TempResult);
			}
		}
	}

	count_obj(TempResult, &Num);
	//CHOKI20110607

	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
		}
		connection(TempResult, &TempResult);		
		rslt = FALSE;
	}

	//CHOKI20110619
	Hobject ImagePotReduced5, threshImagePot5, ErosionRgn5, DilationRgn5, ResultTemp1, ResultTemp2;
	Hlong CountNum2 = 0; 
	Hlong CountNum3 = 0;
	HTuple MeanRef5, Mean5, MeanRef;
	HTuple MeanBase; //CHOKI20110622
	HTuple RowS5,ColumS5,RowE5,ColumE5, RowC5,ColumnC5, AreaC5;
	HTuple lCenterX2, lCenterY2, lPi2, lWidth2, lHeight2;
	HTuple MeanRefGreen; //CHOKI20110824
	Hlong dist_intensity; //CHOKI20110622
	reduce_domain(*image5, PotRegion2, &ImagePotReduced5);

	set_check("~give_error");
#ifdef AVI_TYPE_1	
	herr = threshold(ImagePotReduced5,&threshImagePot5,HTuple(170),HTuple(255)); //CHOKI20110720 140->150 //CHOKI20110727 150->160 //CHOKI20110808 160->165 //CHOKI20110824 165->170		//AVI_TYPE_1//
#else	
	herr = threshold(ImagePotReduced5,&threshImagePot5,HTuple(180),HTuple(255)); //CHOKI20110720 140->150 //CHOKI20110727 150->160 //CHOKI20110808 160->165 //CHOKI20110824 165->170set_check("give_error");
#endif	
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_37!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	erosion_rectangle1(threshImagePot5, &ErosionRgn5, 2, 2);
	dilation_rectangle1(ErosionRgn5, &DilationRgn5, 1, 1); //CHOKI20110720 2,2->1,1
	connection(DilationRgn5, &DilationRgn5);
//	select_shape(DilationRgn5, &ResultTemp1, HTuple("width").Concat("height"), HTuple("and"), HTuple(tolerance).Concat(tolerance), HTuple(50).Concat(50));
	select_shape(DilationRgn5, &ResultTemp1, HTuple("area"), HTuple("and"), HTuple(tolerance ), HTuple(200));
	connection(ResultTemp1, &ResultTemp1);
	count_obj(ResultTemp1, &Num);

	CountNum2 = (Hlong)Num[0];

	gen_empty_obj(&ResultTemp2);
	smallest_rectangle1(pot_ins_region,&RowS5,&ColumS5,&RowE5,&ColumE5);

	if(0 < CountNum2)
	{
		for(int i = 0; i < CountNum2; i++)
		{
			select_obj(ResultTemp1, &SelectRgn, i + 1);
			
			intensity(SelectRgn, *image2, &Mean5, _);
			intensity(SelectRgn, RefImageTrans, &MeanRef5, _);
			intensity(SelectRgn, RefGreenImageTrans, &MeanRefGreen, _);			
			intensity(PotRegion, RefImageTrans, &MeanRef, _);
			intensity(PotRegion, *image2, &MeanBase, _);
			area_center(SelectRgn, &AreaC5, &RowC5, &ColumnC5);
			smallest_rectangle2(SelectRgn,&lCenterX2, &lCenterY2, &lPi2, &lWidth2, &lHeight2);  
			if((Hlong)MeanRef[0] > 150)
			{
				//CHOKI20110622
				dist_intensity = (Hlong)MeanBase[0] - (Hlong)MeanRef[0];
				if(dist_intensity >= 10)
					dist_intensity = -5; //CHOKI20110808 부호변경 //CHOKI20110810 가중치 변경 10->5
				else if(dist_intensity <= -10)
					dist_intensity = 5; //CHOKI20110808 부호변경 //CHOKI20110810 가중치 변경 10->5
				else
					dist_intensity = 0;
				//CHOKI20110622

				if((Hlong)Mean5[0] < (130+dist_intensity) && (Hlong)MeanRef5[0] > 185 && (Hlong)ColumnC5[0] > (Hlong)ColumS5[0] &&  (Hlong)ColumnC5[0] < (Hlong)ColumE5[0] && (double)lWidth2[0]*2 >= 3 && (double)lHeight2[0]*2 >= 3) //CHOKI20110725 180->185
				{
					union2(ResultTemp2, SelectRgn, &ResultTemp2);
				}
			}
			else
			{
				//CHOKI20110622
				dist_intensity = (Hlong)MeanBase[0] - (Hlong)MeanRef[0];
				if(dist_intensity >= 10)
					dist_intensity = -5; //CHOKI20110808 부호변경 //CHOKI20110810 가중치 변경 10->5
				else if(dist_intensity <= -10)
					dist_intensity = 5; //CHOKI20110808 부호변경 //CHOKI20110810 가중치 변경 10->5
				else
					dist_intensity = 0;
				//CHOKI20110622
				//CHIOKI20110824 MeanRefGreen 145
				if((Hlong)MeanRefGreen[0] < 145 &&(Hlong)Mean5[0] < (120+dist_intensity) && (Hlong)MeanRef5[0] > 175 && (Hlong)ColumnC5[0] > (Hlong)ColumS5[0] &&  (Hlong)ColumnC5[0] < (Hlong)ColumE5[0] && (double)lWidth2[0]*2 >= 3 && (double)lHeight2[0]*2 >= 3) //CHOKI20110720 130->120, 160->170 //CHOKI20110725 170->175
				{
					union2(ResultTemp2, SelectRgn, &ResultTemp2);
				}
			}
		}
	}

	connection(ResultTemp2, &ResultTemp2);
	count_obj(ResultTemp2, &Num);

	CountNum3 = (Hlong)Num[0];

	if(CountNum3 > 0)
	{
		*rslt_num += (Hlong)Num[0];
		if(win)
		{
			set_color(win, HTuple("red"));
		}
		union2(ResultTemp2, TempResult, &TempResult);
		connection(TempResult, &TempResult);		
		rslt = FALSE;
	}
	//CHOKI20110619

/*	//CHOKI20110710
	Hobject PotRegion6, ImagePotReducedT, threshImagePotT, ErosionRgnT, DilationRgnT, ResultTempT;
	HTuple RowS6,ColumS6,RowE6,ColumE6;
	//smallest_rectangle1(pot_ins_region,&RowS5,&ColumS5,&RowE5,&ColumE5);
	smallest_rectangle1(PotRegion2,&RowS6,&ColumS6,&RowE6,&ColumE6);
	gen_rectangle1(&PotRegion6, RowS6, ColumS5, RowE6, ColumE5);
	erosion_rectangle1(PotRegion6, &PotRegion6, 20, 1);
	reduce_domain(*image6, PotRegion6, &ImagePotReducedT);
	threshold(ImagePotReducedT,&threshImagePotT,HTuple(0),HTuple(45)); //45~50 가변필요
	connection(threshImagePotT, &threshImagePotT);
	select_shape(threshImagePotT, &ResultTempT, HTuple("ra"), HTuple("and"), HTuple(1 ), HTuple(5));
	select_shape(ResultTempT, &ResultTempT, HTuple("area"), HTuple("and"), HTuple(10 ), HTuple(1000));
	select_shape(ResultTempT, &ResultTempT, HTuple("height"), HTuple("and"), HTuple(2 ), HTuple(100));
	count_obj(ResultTempT, &Num);
	*rslt_num += (Hlong)Num[0];
	if(win)
	{
		set_color(win, HTuple("red"));
	}
	union2(ResultTempT, TempResult, &TempResult);
	connection(TempResult, &TempResult);		
	rslt = FALSE;
	//CHOKI20110710*/

	copy_obj(TempResult, rslt_region, 1, -1); 


	return rslt;
}

BOOL C_VisionModule::ChipPot(Hlong win, Hobject *image, RECT ins_region, Hlong pot_area, int thresh, int under_tol, int over_tol, Hlong *rslt_num, Hlong *rslt_area, Hobject *rslt_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	rslt = _Hal_ChipPot(win, image, ins_region, pot_area, thresh, under_tol, over_tol, rslt_num, rslt_area, rslt_region,
					   min_area, min_width, min_height);


	return rslt;

}

//rslt_num : 불량개수
//rslt_region : 불량영역(SR + user area불량)
//rslt_sr_region : sr 검사영역(위치보정된 후)
//rslt_notsr_region : lead부분의 불량 잡은것(첫번째와 두번째 영상 뺀것)
//rslt_user_region : user area 검사영역(위치보정된 후)
BOOL C_VisionModule::FilmMold(Hlong win, 
							  Hobject *mold_image,		//map 1
							  Hobject *ink_image,		//map 3
							  Hobject *user_image,		//map 1
							  Hobject *scratch_image,	//map 4
							  Hobject *taught_image,     //2번 taught
							  RECT ins_region, RECT chip_region, RECT ocv_region, 
							  Hobject ocv_chars_region, Hobject sr_region, Hobject user_region, Hobject sr_out_region,
							  Hlong var_id, 
							  double first_offset_ox, double first_offset_oy,			//첫 device의 패턴등록점		
							  double first_offset_ox2, double first_offset_oy2, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int dark_thresh, int dark_bright_tol, int white_thresh, int white_bright_tol, int mold_fact, int tolerance, int tolerance_light, int user_thresh, int user_tol,
							  int sr_thresh, int sr_hi_thresh, int sr_tol,

							  int sub_sr_rgn_num, int sub_sr_rgn_x[][SR_SUBREGION_POINT_MAX_NUM], int sub_sr_rgn_y[][SR_SUBREGION_POINT_MAX_NUM],	//20150123 NEW by
							  int sub_sr_rgn_type[SR_SUBREGION_MAX_NUM], //0 : 세밀한 패턴의 SR, 1 : 굵은 패턴의 SR
							  Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region, Hobject *rslt_notsr_region, Hobject *rslt_user_region, Hobject *rslt_sr_out_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height, recog_dist, min_area_sr, min_width_sr, min_height_sr;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;
	recog_dist = RECOG_DIST;
	min_area_sr = VISION_FACT_MIN_AREA_SR;
	min_width_sr = VISION_FACT_MIN_WIDTH_SR;
	min_height_sr = VISION_FACT_MIN_HEIGHT_SR;

	rslt = _Hal_FilmMold(win, mold_image, 
							  ink_image, user_image, scratch_image, taught_image,
							  ins_region, chip_region, ocv_region, 
							  ocv_chars_region, sr_region, user_region, sr_out_region,
							  var_id, 
							  first_offset_ox, first_offset_oy,			//첫 device의 패턴등록점		
							  first_offset_ox2, first_offset_oy2, 
							  offset_ox, offset_oy, 
							  offset_ox2, offset_oy2, 
							  offset_rx, offset_ry,
							  offset_rx2, offset_ry2,
							  dark_thresh, dark_bright_tol, white_thresh, white_bright_tol, mold_fact, tolerance, tolerance_light, user_thresh, user_tol,
							  sr_thresh, sr_hi_thresh, sr_tol,
							  sub_sr_rgn_num, sub_sr_rgn_x, sub_sr_rgn_y,	//20150123 NEW by
							  sub_sr_rgn_type,
							  rslt_num, rslt_region, rslt_sr_region, rslt_notsr_region, rslt_user_region, rslt_sr_out_region,
							  min_area, min_width, min_height,
							  min_area_sr, min_width_sr, min_height_sr);
	return rslt;
}

//20100718 lee
//BOOL C_VisionModule::FilmMetal(Hlong win, Hobject *image, Hobject sr_region, Hobject chip_ins_region1, Hobject char_region,
BOOL C_VisionModule::FilmMetal(Hlong win, Hobject *image, Hobject *Ref_image, //CHOKI20110728 Ref_image추가
							   Hobject sr_region, RECT chip_region, RECT char_region,
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							 int thresh, int tolerance, Hlong *rslt_num, Hobject *rslt_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn0, RsltRgn1, ChipRgn1, SRRgn, charRgn;
	HTuple Num;
	HTuple DiffAng, HomMat2D;


	double offset_x, offset_y;

	//CHOKI20110728
	Hobject SelectRgn, ResultTemp, RefImageTrans;
	HTuple MeanRef;
	Hlong SelectCount, TempCount;

	gen_empty_obj(&ResultTemp);
	//CHOKI20110728

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(sr_region, &SRRgn, HomMat2D, HTuple("false"));
	affine_trans_image(*Ref_image, &RefImageTrans, HomMat2D, HTuple("constant"), HTuple("false")); //CHOKI20110728

	//affine_trans_region(chip_ins_region1, &ChipRgn1, HomMat2D, HTuple("false"));

	//런타임 칩영역//////////////////////////////

	erosion_rectangle1(SRRgn, &InsRgn, 3, 3);
	//dilation_rectangle1(ChipRgn1, &ChipRgn1, 5.5, 5.5);
	//difference(InsRgn, ChipRgn1, &InsRgn);
	reduce_domain(*image, InsRgn, &ImageReduced);

	//threshold(ImageReduced,&Region,HTuple(thresh),HTuple(255));
	//20100718 lee  210->200으로 조정 
	if(thresh > 230) thresh = 220; //20110616 lee

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = hysteresis_threshold(ImageReduced, &Region, HTuple(thresh), HTuple(230), HTuple(5));

	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_05!... %ld", (Hlong)thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	
	//chip과 ocv 영역 전체를 제거한다. 

	gen_rectangle1(&charRgn, (Hlong)(char_region.top), 
								(Hlong)(char_region.left), 
								(Hlong)(char_region.bottom), 
								(Hlong)(char_region.right));

	gen_rectangle1(&ChipRgn1, (Hlong)(chip_region.top), 
								(Hlong)(chip_region.left), 
								(Hlong)(chip_region.bottom), 
								(Hlong)(chip_region.right));	

	//LSS20100406
	//문자영역제외
	//difference(Region, char_region, &Region);
	difference(Region, charRgn, &Region);
	difference(Region, ChipRgn1, &Region);

	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RsltRgn0, HTuple("width").Concat("height"), HTuple("and"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

	connection(RsltRgn0, &RsltRgn0);

	count_obj(RsltRgn0, &Num);
	//CHOKI20110728
	//*rslt_num = (Hlong)Num[0];
	SelectCount = (Hlong)Num[0];
	if(0 < SelectCount)//if(0 < *rslt_num)
	{
		TempCount = 0;
		//copy_obj(RsltRgn0, rslt_region, 1, -1);
		for(int i = 0; i < (Hlong)Num[0]; i++) 
		{
			select_obj(RsltRgn0, &SelectRgn, i + 1);
			intensity(SelectRgn, RefImageTrans, &MeanRef, _);
#ifdef AVI_TYPE_1
			if((Hlong)MeanRef[0] < 180) //CHOKI20111004 230->215 111005 ytlee	215 -> 210  //CHOKI20111005 ->190 // 111007 ytlee 190 -> 180	//AVI_TYPE_1//
#else
			if((Hlong)MeanRef[0] < 230)
#endif
			{
				union2(ResultTemp, SelectRgn, &ResultTemp);
				TempCount++;
			}
		}
		if(TempCount > 0)
		{
			connection(ResultTemp, &ResultTemp);
			count_obj(ResultTemp, &Num);
			*rslt_num = (Hlong)Num[0];
			copy_obj(ResultTemp, rslt_region, 1, -1);

			if(win)
			{
				set_color(win, HTuple("red"));
				disp_obj(*rslt_region, win);
			}
			return FALSE;
		}
		//return FALSE;
	}
	//CHOKI20110728

	return TRUE;
}


BOOL C_VisionModule::FilmPF(Hlong win, Hobject *image, Hobject *image2, 
							RECT ins_region, RECT chip_region, RECT dontcare_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int tolerance, int black_thresh, int black_tolarance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region) 
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height, recog_dist, min_area_sr, min_width_sr, min_height_sr;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;
	recog_dist = RECOG_DIST;
	min_area_sr = VISION_FACT_MIN_AREA_SR;
	min_width_sr = VISION_FACT_MIN_WIDTH_SR;
	min_height_sr = VISION_FACT_MIN_HEIGHT_SR;

	rslt = _Hal_FilmPF(win, image, image2,
							ins_region, chip_region, dontcare_region, pf_region, vacc_region,
						   offset_ox, offset_oy, 
						   offset_ox2, offset_oy2, 
						   offset_rx, offset_ry,
						   offset_rx2, offset_ry2,
						   thresh, tolerance, black_thresh, black_tolarance,
						   rslt_num, rslt_region, rslt_pf_region,
						   min_area, min_width, min_height,
						   min_area_sr, min_width_sr, min_height_sr); 
	return rslt;




}

//하부결함(out lead)
BOOL C_VisionModule::InLead(Hlong win, Hobject *image, Hobject *image2, RECT ins_region, Hobject not_sr_region,	
							Hobject dn_region,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int threshold, double factor, int tolerance, 
						   double* line_width_avg, 
						   Hlong *rslt_num, Hobject *rslt_region,  
						   Hlong *rslt_num2, Hobject *rslt_region2,
						   Hobject *rslt_dn_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	rslt = _Hal_InLead(win, image, image2, ins_region, not_sr_region,	
							dn_region,
							offset_ox, offset_oy, 
							offset_ox2, offset_oy2, 
							offset_rx, offset_ry,
							offset_rx2, offset_ry2,
						   threshold, factor, tolerance, 
						   line_width_avg, 
						   rslt_num, rslt_region,  
						   rslt_num2, rslt_region2,
						   rslt_dn_region,
						   min_area, min_width, min_height);

	return rslt;


}

//하부긁힘(out lead)
BOOL C_VisionModule::InLeadScratch(Hlong win, Hobject *image, Hobject rslt_dn_region, 
							 int thresh, int tolerance, Hlong *rslt_num, Hobject *rslt_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn, EmpImage;
	HTuple Num;

	int min_thresh, max_thresh;

	//런타임 dn lead 영역////////////////////////
	reduce_domain(*image, rslt_dn_region, &ImageReduced);

	emphasize(ImageReduced,&EmpImage,HTuple(5),HTuple(5),HTuple(2)); //20101008 lee

	max_thresh = thresh;
	min_thresh = max_thresh - 30;
	if(100 > min_thresh)
	{
		min_thresh = 100;
		max_thresh = 150;
	}

	if(min_thresh > max_thresh) min_thresh = max_thresh-10; //20110616 lee
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = hysteresis_threshold(EmpImage,&Region,HTuple(min_thresh),HTuple(max_thresh),HTuple(20));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_06!... %ld, %ld", (Hlong)min_thresh, (Hlong)max_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	
	
	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RegionClosing, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(RegionClosing, &RegionClosing, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(RegionClosing, &RsltRgn, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

	connection(RsltRgn, &RsltRgn);

	count_obj(RsltRgn, &Num);
	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{
		copy_obj(RsltRgn, rslt_region, 1, -1);
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}

		return FALSE;
	}

	return TRUE;
}

//A/M scratch
BOOL C_VisionModule::AMLead(Hlong win, Hobject *image,
							BOOL use_0,
							BOOL use_1,
							Hobject region_0_0,
							Hobject region_0_1,
							Hobject region_1_0,
							Hobject region_1_1,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int thres, int tolerance, 
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hobject *rslt_ins_region_0_0, Hobject *rslt_ins_region_0_1, Hobject *rslt_ins_region_1_0, Hobject *rslt_ins_region_1_1)
{
	BOOL rslt = TRUE;

	HTuple HomMat2D;
	double offset_x, offset_y;
	HTuple DiffAng;

	HTuple   Num;

	Hlong max;
	HTuple Min, Max, Area;

	Hobject ImageReduced, ResultRegion, TotRegion, TotRegion0_0, TotRegion0_1, TotRegion1_0, TotRegion1_1;

	gen_empty_obj(&ResultRegion);
	gen_empty_obj(&TotRegion);
	gen_empty_obj(&TotRegion0_0);
	gen_empty_obj(&TotRegion0_1);
	gen_empty_obj(&TotRegion1_0);
	gen_empty_obj(&TotRegion1_1);
	gen_empty_obj(rslt_ins_region_0_0);
	gen_empty_obj(rslt_ins_region_0_1);
	gen_empty_obj(rslt_ins_region_1_0);
	gen_empty_obj(rslt_ins_region_1_1);

	*rslt_num = 0;

	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	//Offset
	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(region_0_0, rslt_ins_region_0_0, HomMat2D, HTuple("false"));
	affine_trans_region(region_0_1, rslt_ins_region_0_1, HomMat2D, HTuple("false"));
	affine_trans_region(region_1_0, rslt_ins_region_1_0, HomMat2D, HTuple("false"));
	affine_trans_region(region_1_1, rslt_ins_region_1_1, HomMat2D, HTuple("false"));

	if(use_0)
	{
		//0_0
		reduce_domain(*image, *rslt_ins_region_0_0, &ImageReduced);

		min_max_gray(*rslt_ins_region_0_0, *image, HTuple(20), &Min, &Max, _);

		max = (Hlong)Max[0];
		max = max - thres;
		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		threshold(ImageReduced, &ResultRegion, 0, max);

		union2(TotRegion0_0, ResultRegion, &TotRegion0_0);

		//0_1
		reduce_domain(*image, *rslt_ins_region_0_1, &ImageReduced);

		min_max_gray(*rslt_ins_region_0_1, *image, HTuple(20), &Min, &Max, _);

		max = (Hlong)Max[0];
		max = max - thres;
		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		threshold(ImageReduced, &ResultRegion, 0, max);

		union2(TotRegion0_1, ResultRegion, &TotRegion0_1);
	}

	if(use_1)
	{
		//1_0
		reduce_domain(*image, *rslt_ins_region_1_0, &ImageReduced);

		min_max_gray(*rslt_ins_region_1_0, *image, HTuple(20), &Min, &Max, _);

		max = (Hlong)Max[0];
		max = max - thres;
		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		threshold(ImageReduced, &ResultRegion, 0, max);

		union2(TotRegion1_0, ResultRegion, &TotRegion1_0);

		//1_1
		reduce_domain(*image, *rslt_ins_region_1_1, &ImageReduced);

		min_max_gray(*rslt_ins_region_1_1, *image, HTuple(20), &Min, &Max, _);

		max = (Hlong)Max[0];
		max = max - thres;
		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		threshold(ImageReduced, &ResultRegion, 0, max);

		union2(TotRegion1_1, ResultRegion, &TotRegion1_1);
	}

	//0-0
	closing_circle(TotRegion0_0, &TotRegion0_0, 2.5);
	connection(TotRegion0_0, &TotRegion0_0);
	select_shape(TotRegion0_0, &TotRegion0_0, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));

	concat_obj(*rslt_region, TotRegion0_0, rslt_region);

	//0-1
	closing_circle(TotRegion0_1, &TotRegion0_1, 2.5);
	connection(TotRegion0_1, &TotRegion0_1);
	select_shape(TotRegion0_1, &TotRegion0_1, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));

	concat_obj(*rslt_region, TotRegion0_1, rslt_region);

	//1-0
	closing_circle(TotRegion1_0, &TotRegion1_0, 2.5);
	connection(TotRegion1_0, &TotRegion1_0);
	select_shape(TotRegion1_0, &TotRegion1_0, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));

	concat_obj(*rslt_region, TotRegion1_0, rslt_region);

	//1-1
	closing_circle(TotRegion1_1, &TotRegion1_1, 2.5);
	connection(TotRegion1_1, &TotRegion1_1);
	select_shape(TotRegion1_1, &TotRegion1_1, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));

	concat_obj(*rslt_region, TotRegion1_1, rslt_region);

	/////////////////////////////////////////////

	//Image1
	count_obj(*rslt_region, &Num);

	*rslt_num = (Hlong)Num[0];

	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}

		rslt = FALSE;
	}

	return rslt;
}

//상부결함(in lead)
BOOL C_VisionModule::OutLead(Hlong win, Hobject *image, Hobject *image2,Hobject *image3,
							RECT ins_region, RECT ins_region2, RECT ins_region3,
							Hobject not_sr_region, 
							Hobject up_region,
							Hobject dn_region,
							Hobject up_region2, 
							Hobject up_region3,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int thresh, double factor, int tolerance, 
						   int thresh_dirt, int tolerance_dirt,
						   BOOL out_lead_use, BOOL out_lead_dirt_use, BOOL use_out_hori,
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hlong *rslt_num2, Hobject *rslt_region2, 						   
						   Hobject *rslt_up_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	//CHOKI20100911
	unsigned int CaseInOutLeadPosition=0;
	HTuple UpLeadRow, UpLeadCaseCol, DnLeadRow, DnLeadCaseCol;
	area_center(up_region, _, &UpLeadRow, &UpLeadCaseCol);
	area_center(dn_region, _, &DnLeadRow, &DnLeadCaseCol);
	if(UpLeadRow < DnLeadRow)//Out 리드가 위쪽에 위치
	{
		CaseInOutLeadPosition = 2;
		m_VDat[0]->m_InsRslt.m_iResultInOutPutDir[0] = 2;
	}
	else if(UpLeadRow > DnLeadRow)//In 리드가 위쪽에 위치
	{
		CaseInOutLeadPosition = 1;
		m_VDat[0]->m_InsRslt.m_iResultInOutPutDir[0] = 1;
	}

	rslt = _Hal_OutLead(win, image, image2,image3,
							ins_region,	ins_region2, ins_region3, 
							not_sr_region, 
							up_region,
							up_region2,
							up_region3,
							offset_ox, offset_oy, 
							offset_ox2, offset_oy2, 
							offset_rx, offset_ry,
							offset_rx2, offset_ry2,
						   thresh, factor, tolerance, 
						   thresh_dirt, tolerance_dirt,
						   out_lead_use, out_lead_dirt_use,use_out_hori,
						   rslt_num, rslt_region, 
						   rslt_num2, rslt_region2, 						   
						   rslt_up_region,
						   min_area, min_width, min_height);

	return rslt;
}

//20100618 lss
//image 검정, image 흰색
BOOL  C_VisionModule::_Hal_OutLead(Hlong win, Hobject *image, Hobject *image2, Hobject *image3,
							RECT ins_region, RECT ins_region2, RECT ins_region3,
							Hobject not_sr_region, 
							Hobject up_region,
							Hobject up_region2,
							Hobject up_region3,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int thresh, double factor, int tolerance, 
						   int thresh_dirt, int tolerance_dirt,
						   BOOL out_lead_use, BOOL out_lead_dirt_use, BOOL use_out_hori,
						   Hlong *rslt_num, Hobject *rslt_region, 
						   Hlong *rslt_num2, Hobject *rslt_region2, 						   
						   Hobject *rslt_up_region,
						   Hlong min_area, Hlong min_width, Hlong min_height)
{
	BOOL rslt = TRUE;
	HTuple   Pointer, Type, Width, Height, WindowHandle, Row1;
	HTuple   Column1, Row2, Column2, Row, Sum, Column, Diff;
	HTuple   Length1, UntilHeight, Start, MeasureHandle0, MeasureHandle1;
	HTuple   RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond;
	HTuple   ColumnEdgeSecond, AmplitudeSecond, IntraDistance;
	HTuple   InterDistance, End, Time;
	Hobject  I, InsRegion, DontCare;
	Hobject  Line, TotLine, select_object;
	HTuple   Num;
	Hlong idx;
	HTuple HomMat2D;

	Hobject RegionDiff;
	Hobject OutRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean;
	Hobject ComplRgn, EmpImage;

	Hobject Region, SubImage;
	HTuple DiffAng;

	Hobject MeanImage;
	Hlong center;
	Hobject TotDirtyRgn, DirtyRgn, LineRgn, ImageReduced;
	Hlong ins_area, tol_area, min, max;
	HTuple Min, Max, Area;

	Hobject NotSRRgn, EmpImage2, InterRgn;

	Hlong i, num;

	get_image_pointer1(*image, &Pointer, &Type, &Width, &Height);

	Hlong row;
	Hlong col;
	Hlong leng1;
	Hlong height;

	//LSS new
	if(use_out_hori)
	{
		row = (ins_region.top + ins_region.bottom) * 0.5;
		col = ins_region.left + 1;
		leng1 = (ins_region.bottom - ins_region.top) * 0.5;
		height = (ins_region.right - ins_region.left) - 1;
	}
	else
	{
		row = ins_region.top + 1;
		col = (ins_region.left + ins_region.right) * 0.5;
		leng1 = (ins_region.right - ins_region.left) * 0.5;
		height = (ins_region.bottom - ins_region.top) - 1;
	}


	//20100621 lss
	Hlong row_l = (ins_region2.top + ins_region2.bottom) * 0.5;
	Hlong col_l = ins_region2.left + 1;
	Hlong leng1_l = (ins_region2.bottom - ins_region2.top) * 0.5;
	Hlong height_l = (ins_region2.right - ins_region2.left) - 1;

	//20100621 lss
	Hlong row_r = (ins_region3.top + ins_region3.bottom) * 0.5;
	Hlong col_r = ins_region3.left + 1;
	Hlong leng1_r = (ins_region3.bottom - ins_region3.top) * 0.5;
	Hlong height_r = (ins_region3.right - ins_region3.left) - 1;

	Hobject RsltRegion, RsltRegion2, RsltUPRegion;

	Hobject TotLine_l, TotLine_r;

	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltRegion2);
	gen_empty_obj(&RsltUPRegion);

	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	if(0.4 > factor)
	{
		factor = 0.4;
	}

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(up_region, &RsltUPRegion, HomMat2D, HTuple("false"));

	//sub_image(*image, *crack_image, &SubImage, 1, 0);
	//emphasize(*image, &EmpImage, 20, 20, 2);
	emphasize(*image, &EmpImage, 10, 10, 2);
	emphasize(*image2, &EmpImage2, 10, 10, 2);

	gen_rectangle1(&InsRegion, ins_region.top, ins_region.left, ins_region.bottom, ins_region.right);
	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(InsRegion, win);
	}

	//Image1/////////////////////////////////////
	gen_empty_obj(&Line);
	gen_empty_obj(&TotLine);

	//20100621 lss
	gen_empty_obj(&TotLine_l);
	gen_empty_obj(&TotLine_r);	

	if(use_out_hori)
	{
		gen_measure_rectangle2(row,col,HTuple(1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(-1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}
	else
	{
		gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}

	for(idx=1; idx<=height; idx+=1)
	{
		if(use_out_hori)
		{
			translate_measure(MeasureHandle0, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle0, row + idx, col);
		}

		measure_pos(EmpImage, MeasureHandle0, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		if(use_out_hori)
		{
			translate_measure(MeasureHandle1, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle1, row + idx, col);
		}

		measure_pos(EmpImage, MeasureHandle1, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);

			union2(TotLine, Line, &TotLine);
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			union2(TotLine, Line, &TotLine);
		}

	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);


	/////////////////////////////////////////////

	connection(TotLine, &TotLine);

	erosion_rectangle1(RsltUPRegion, &RsltUPRegion, 2.5, 2.5);
	intersection(TotLine, RsltUPRegion, &TotLine);

	//20100621 lss
	union2(TotLine, TotLine_l, &TotLine);
	union2(TotLine, TotLine_r, &TotLine);
	connection(TotLine, &TotLine);

	/*
	//이진화 방식으로 얻은 영역과 에지측정으로 얻은 영역의 교집합이 존재하는 조건
	count_obj(TotLine, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(TotLine, &TotLine);
		intersection(not_sr_region, RsltUPRegion, &NotSRRgn);
		connection(NotSRRgn, &NotSRRgn);
		count_obj(NotSRRgn, &Num);
		gen_empty_obj(&InterRgn);
		num = (Hlong)Num[0];
		for(i = 0; i < num; i++)
		{
			select_obj(NotSRRgn, &SelectRgn, i + 1);
			intersection(TotLine, SelectRgn, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			if(1 < (Hlong)Area[0])
			{
				union2(TotLine, NotSRRgn, &TotLine);
			}
		}
	}
	*/

	//union2(TotLine, DynRegion, &TotLine);

	closing_circle(TotLine, &TotLine, HTuple(50.5));

	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));

	if(out_lead_use)
	{
		select_shape(TotLine, &RsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));
	}



	//Image2/////////////////////////////////////
	gen_empty_obj(&Line);
	gen_empty_obj(&TotLine);

	//20100621 lss
	gen_empty_obj(&TotLine_l);
	gen_empty_obj(&TotLine_r);	


	if(use_out_hori)
	{
		gen_measure_rectangle2(row,col,HTuple(1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(-1.57079632679),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}
	else
	{
		gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
	}

	for(idx=1; idx<=height; idx+=1)
	{
		if(use_out_hori)
		{
			translate_measure(MeasureHandle0, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle0, row + idx, col);
		}

		measure_pos(EmpImage2, MeasureHandle0, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		if(use_out_hori)
		{
			translate_measure(MeasureHandle1, row, col + idx);
		}
		else
		{
			translate_measure(MeasureHandle1, row + idx, col);
		}

		measure_pos(EmpImage2, MeasureHandle1, HTuple(factor), HTuple(thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);

			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);
		}

	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);


	/////////////////////////////////////////////

	connection(TotLine, &TotLine);

	erosion_rectangle1(RsltUPRegion, &RsltUPRegion, 5.5, 2.5);
	intersection(TotLine, RsltUPRegion, &TotLine);

	//20100621 lss
	union2(TotLine, TotLine_l, &TotLine);
	union2(TotLine, TotLine_r, &TotLine);
	connection(TotLine, &TotLine);

	/*
	//이진화 방식으로 얻은 영역과 에지측정으로 얻은 영역의 교집합이 존재하는 조건
	count_obj(TotLine, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(TotLine, &TotLine);
		intersection(not_sr_region, RsltUPRegion, &NotSRRgn);
		connection(NotSRRgn, &NotSRRgn);
		count_obj(NotSRRgn, &Num);
		gen_empty_obj(&InterRgn);
		num = (Hlong)Num[0];
		for(i = 0; i < num; i++)
		{
			select_obj(NotSRRgn, &SelectRgn, i + 1);
			intersection(TotLine, SelectRgn, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			if(1 < (Hlong)Area[0])
			{
				union2(TotLine, NotSRRgn, &TotLine);
			}
		}
	}
	*/

	//union2(TotLine, DynRegion, &TotLine);

	closing_circle(TotLine, &TotLine, HTuple(50.5));

	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));

	if(out_lead_use)
	{
		select_shape(TotLine, &RsltRegion2, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));
	}
	//Image1/////////////////////////////////////
	//오염///////////////////////////////////////
	mean_image(*image3, &MeanImage, 10, 2);

	center = (ins_region.top + ins_region.bottom) * 0.5;
	
	gen_empty_obj(&TotDirtyRgn);

	for(i = ins_region.left; i < ins_region.right; i++)
	{
		//All
		gen_region_line(&LineRgn, ins_region.top, i, ins_region.bottom, i);

		reduce_domain(MeanImage, LineRgn, &ImageReduced);
		
		//Find suitable threshold
		min_max_gray(LineRgn, MeanImage, HTuple(20), &Min, &Max, _);
		

		max = (Hlong)Max[0];
		max = max - thresh_dirt;
		if( (0 > max) || (255 < max) )
		{
			max = 0;
		}
	
		/*
		intensity(LineRgn, MeanImage, &Mean, _);
		mean = (Hlong)Mean[0];
		mean = mean - thresh_dirt;
		if( (0 > mean) || (255 < mean) )
		{
			mean = 0;
		}
		max = mean;
		*/
		
		threshold(ImageReduced, &DirtyRgn, 0, max);

		//connection(DirtyRgn, &DirtyRgn);
		//select_shape(DirtyRgn, &DirtyRgn, HTuple("area"), HTuple("and"), HTuple(10000),  HTuple("max"));
		//concat_obj(TotDirtyRgn, DirtyRgn, &TotDirtyRgn);

		union2(TotDirtyRgn, DirtyRgn, &TotDirtyRgn);
	}

	//union1(TotDirtyRgn, &TotDirtyRgn);
	closing_circle(TotDirtyRgn, &TotDirtyRgn, 10.5);
	connection(TotDirtyRgn, &TotDirtyRgn);

	area_center(Region, &Area, _, _);
	ins_area = (Hlong)Area[0];
	tol_area = (Hlong)( ((double)ins_area) * (tolerance_dirt * 0.01) );

	select_shape(TotDirtyRgn, &TotDirtyRgn, HTuple("area"), HTuple("and"), HTuple(tol_area),  HTuple("max"));

	if(out_lead_dirt_use)
	{
		concat_obj(RsltRegion, TotDirtyRgn, &RsltRegion);
	}

	/////////////////////////////////////////////

	//Image1
	count_obj(RsltRegion, &Num);

	*rslt_num = (Hlong)Num[0];

	if( (0 < *rslt_num) && out_lead_use)//LSS new
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion, win);
		}

		rslt = FALSE;
	}

	copy_obj(RsltRegion, rslt_region, 1, -1);

	//Image2
	count_obj(RsltRegion2, &Num);

	*rslt_num2 = (Hlong)Num[0];

	if( (0 < *rslt_num2) && out_lead_use)//LSS new
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion2, win);
		}

		rslt = FALSE;
	}

	copy_obj(RsltRegion2, rslt_region2, 1, -1);

	copy_obj(RsltUPRegion, rslt_up_region, 1, -1);

	close_all_measures();

	return rslt;
}




//상부긁힘(in lead)
BOOL C_VisionModule::OutLeadScratch(Hlong win, Hobject *image, Hobject rslt_up_region, 
							 int thresh, int tolerance, Hlong *rslt_num, Hobject *rslt_region)
{
	BOOL rslt = TRUE;
	Hlong min_area, min_width, min_height;
	min_area = VISION_FACT_MIN_AREA;
	min_width = VISION_FACT_MIN_WIDTH;
	min_height = VISION_FACT_MIN_HEIGHT;

	Hobject ImageReduced, Region, RegionClosing, InsRgn, RsltRgn, EmpImage;
	HTuple Num;

	int min_thresh, max_thresh;

	//런타임 up lead 영역////////////////////////
	reduce_domain(*image, rslt_up_region, &ImageReduced);

	emphasize(ImageReduced,&EmpImage,HTuple(27),HTuple(27),HTuple(2));

	max_thresh = thresh;
	min_thresh = max_thresh - 100;
	if(100 > min_thresh)
	{
		min_thresh = 100;
		max_thresh = 150;
	}

	if(min_thresh > max_thresh) min_thresh = max_thresh -10; //20110616 lee

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = hysteresis_threshold(EmpImage,&Region,HTuple(min_thresh),HTuple(max_thresh),HTuple(20));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_08!... %ld, %ld", (Hlong)min_thresh, (Hlong)max_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	
	connection(Region, &Region);
	closing_circle(Region,&RegionClosing,HTuple(3.5));

	select_shape(RegionClosing, &RegionClosing, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(RegionClosing, &RegionClosing, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(RegionClosing, &RsltRgn, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

	connection(RsltRgn, &RsltRgn);

	count_obj(RsltRgn, &Num);
	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{
		copy_obj(RsltRgn, rslt_region, 1, -1);
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}

		return FALSE;
	}

	return TRUE;
}

//SR 기포검사 & Hall 검사
BOOL C_VisionModule::SRBub(Hlong win, Hobject *image/*blue*/, Hobject *hall_image/*blue*/, Hobject *Green_image/*green*/, 
						   Hobject *RefGreen_image,  Hobject *RefBlue_image, 
						   Hobject *ink_chip_image/*red*/,
						   RECT ins_sr_region, RECT ins_film_region, RECT chip_search_region,
						   double offset_ox, double offset_oy, double offset_rx, double offset_ry,
						   int abs_thresh, int var_thresh, int mold_fact, int tolerance, 
						   int tol_max, int tol_cnt,
						   int hall_thresh, int hall_tol,
						   int fold_thresh, int fold_tol, int ink_chip_size,
			              Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region,
						  Hlong *rslt_hall_num, Hobject *rslt_hall_region, Hobject *rslt_hallins_region)
{
	BOOL rslt = TRUE;
	Hobject Image, ImageTrans, RegionDiff, ImageReduced, RefImage, RefImageReduced, ImageSub, RegionHysteresis2, RegionErosion2, RegionDilation2, RegionDilation3;//CHOKI20100830
	Hobject	RegionRect, RegionRectChip, ChipRect, OCVRect;
	HTuple HomMat2D, Num;
	Hobject select_object;
	Hobject RegionErosion, RegionDilation;
	Hobject ImageTrans2, TempRgn1, PunchReduce, PunchThresh, PunchRgn; 

	Hobject OutRgn, SelectRgn, ResultRgn, ChipRegionRect, ChipSearchRect; 
	HTuple Mean, OutMin, OutMax;
	Hobject ComplRgn; 

	Hobject MeanImage, MeanImageReduced, HallImageReduced, HallRegion, DilRegion;
	long offset_x, offset_y;

	Hobject RedImageEmpha, RefImageEmpha, ImageBubReduced, ThreshBub, SelectRgn2; 

	int i;

	int BubCount, BubCount2; 

	int thresh, high_threshold;

	char temp[100];
	HTuple start_t, end_t;

	count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////

	gen_rectangle1(&RegionRect, (Hlong)(ins_sr_region.top), 
								(Hlong)(ins_sr_region.left), 
								(Hlong)(ins_sr_region.bottom), 
								(Hlong)(ins_sr_region.right));


	gen_rectangle1(&RegionRectChip, (Hlong)(ins_film_region.top), 
								(Hlong)(ins_film_region.left), 
								(Hlong)(ins_film_region.bottom), 
								(Hlong)(ins_film_region.right));

	vector_angle_to_rigid(offset_oy, offset_ox, 0, offset_ry, offset_rx, HTuple(0), &HomMat2D);

	reduce_domain(*RefGreen_image, RegionRectChip, &ImageTrans);
	reduce_domain(*RefBlue_image, RegionRectChip, &ImageTrans2);

	affine_trans_image(ImageTrans, &ImageTrans, HomMat2D, HTuple("constant"), HTuple("false"));
	affine_trans_image(ImageTrans2, &ImageTrans2, HomMat2D, HTuple("constant"), HTuple("false")); 
	affine_trans_region(RegionRect, &RegionRect, HomMat2D, HTuple("false")); 

	count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
	sprintf(temp,"[1:%f]", (double)(HTuple(end_t - start_t)[0]) );
	TRACE(temp);
	count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////

	reduce_domain(*Green_image, RegionRect, &ImageReduced);
	reduce_domain(ImageTrans, RegionRect, &RefImageReduced);

	Hobject RegionThresh, RegionThreshRef, DiffRgn2;

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced, &RegionThresh, HTuple(abs_thresh-5), HTuple(255));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_16!... at %ld", (Hlong)abs_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	set_check("~give_error");

	abs_thresh = 245;

	herr = threshold(RefImageReduced, &RegionThreshRef, HTuple(abs_thresh-15), HTuple(255)); 
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_17!... at %ld", (Hlong)abs_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	difference(RegionThresh, RegionThreshRef, &DiffRgn2);
	connection(DiffRgn2, &DiffRgn2);
	
	erosion_rectangle1(DiffRgn2, &DiffRgn2, HTuple(1), HTuple(3)); 

	select_shape(DiffRgn2, &RegionDiff, HTuple("height"), HTuple("and"), HTuple(5), HTuple("max"));
	select_shape(RegionDiff, &RegionDiff, HTuple("width"), HTuple("and"), HTuple(2), HTuple("max"));  //CHOKI20110720

	dilation_rectangle1(RegionDiff, &RegionDiff, HTuple(1), HTuple(1));
	connection(RegionDiff, &RegionDiff);

	//경계값 및 max tol 비교
	BOOL max_chk = FALSE;
	count_obj(RegionDiff, &Num);
	BubCount = (Hlong)Num[0]; 

	//기포 검사 
	Hobject BubRgnTemp, BubRgnTemp2; 
	long CheckBigBub, CheckSmallBub, CheckTotBub;

	gen_empty_obj(&BubRgnTemp);
	gen_empty_obj(&BubRgnTemp2);
	CheckBigBub = 0;
	CheckSmallBub = 0;
	CheckTotBub = 0;

	long CheckSize;
	HTuple CRow0, CCol0, CRow1, CCol1;//, CArea;
	
	HTuple deviInt, RefInt, AreaBub;
	if(0 < (Hlong)Num[0])
	{	
		for(i = 0; i < BubCount; i++) 
		{
			select_obj(RegionDiff, &SelectRgn, i + 1);
			smallest_rectangle1(SelectRgn, &CRow0, &CCol0, &CRow1, &CCol1);
			CheckSize = (Hlong)CRow1[0] - (Hlong)CRow0[0];
			
			if( CheckSize >= tolerance && CheckSize < tol_max )
			{
				union2(BubRgnTemp2, SelectRgn, &BubRgnTemp2);
				CheckSmallBub++;
			}
			else if(CheckSize >= tol_max)
			{
				union2(BubRgnTemp, SelectRgn, &BubRgnTemp);
				CheckBigBub++;
			}	
		}
		if(CheckSmallBub < tol_cnt)
		{
			gen_empty_obj(&BubRgnTemp2);
		}
		
		if( CheckSmallBub >= tol_cnt || CheckBigBub > 0 )
		{
			union2(BubRgnTemp, BubRgnTemp2, &BubRgnTemp);
			connection(BubRgnTemp, &BubRgnTemp);
			count_obj(BubRgnTemp, &Num);
			BubCount2 = (Hlong)Num[0]; 
			
			for(i = 0; i < BubCount2; i++) 
			{
				select_obj(BubRgnTemp, &SelectRgn2, i + 1);
				intensity(SelectRgn, ImageTrans, &RefInt, _);
				intensity(SelectRgn, *Green_image, &AreaBub, _);
				
				if((Hlong)RefInt[0] < 170)//(Hlong)deviInt[0] < 15 && (Hlong)RefInt[0] < 210 && (Hlong)AreaBub[0] >10 ) 
				{
					union2(SelectRgn2, *rslt_region, rslt_region );
					rslt = FALSE;
					
					if(win)
					{
						set_color(win, HTuple("red"));
						disp_obj(*rslt_region, win);
					}
				}
				else if((Hlong)AreaBub[0] > 250 && (Hlong)RefInt[0] < 210) //CHOKI20110725 기준이미지가 기준이상 밝을 경우 검사제외시킴(패턴은 거의 검출할 수 없음) 
				{
					union2(SelectRgn2, *rslt_region, rslt_region );
					rslt = FALSE;
					
					if(win)
					{
						set_color(win, HTuple("red"));
						disp_obj(*rslt_region, win);
					}
				}
			}
			if(!rslt)
			{
				connection(*rslt_region, rslt_region);
				count_obj(*rslt_region, &Num);
				*rslt_num = (Hlong)Num[0];
			}	
		}	
	}

	//InkChip검사 추가됨 //CHOKI20111121 검사결과는 기포불량으로 나옴
	Hobject InkChipReduced, InkChipThresh, InkChipErosionRgn, InkChipSelectRgn, InkChipRgnTemp, InkChipInsRgn;		// 111222 ytlee 
	long InkChipCount;
	HTuple InkChipMean, Row, Col;		// 111222 ytlee
	HTuple Row0, Col0, Row1, Col1;
	long XSize, YSize;

	area_center(RegionRect, _, &Row, &Col);		// 111222 ytlee
	gen_rectangle1(&InkChipInsRgn, Row - 40, Col - 25, Row + 40, Col + 25);
	
	reduce_domain(*ink_chip_image, InkChipInsRgn, &InkChipReduced);

	if(win)
	{
		set_color(win, HTuple("yellow"));		// 111222 ytlee
		disp_obj(InkChipInsRgn, win);
	}

	set_check("~give_error");
	herr = threshold(InkChipReduced, &InkChipThresh, HTuple(5), HTuple(110));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_InkChip!... at %ld", (Hlong)abs_thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	connection(InkChipThresh, &InkChipThresh);

	erosion_rectangle1(InkChipThresh, &InkChipErosionRgn, HTuple(1), HTuple(1));

	select_shape(InkChipErosionRgn, &InkChipSelectRgn, HTuple("area"), HTuple("and"), HTuple(ink_chip_size), HTuple("max"));
	
	connection(InkChipSelectRgn, &InkChipSelectRgn);

	count_obj(InkChipSelectRgn, &Num);
	InkChipCount = (Hlong)Num[0]; 

	if(0 < InkChipCount)
	{	
		for(i = 0; i < InkChipCount; i++) 
		{
			select_obj(InkChipSelectRgn, &SelectRgn, i + 1);
			intensity(SelectRgn, *image, &InkChipMean, _);
			smallest_rectangle1(SelectRgn, &Row0, &Col0, &Row1, &Col1);
			XSize = (Hlong)Col1[0] - (Hlong)Col0[0];
			YSize = (Hlong)Row1[0] - (Hlong)Row0[0];
			if(YSize < 1) //CHOKI20111229
			{
				YSize = 1;
			}
			if(XSize < 1) //CHOKI20111229
			{
				XSize = 1;
			}
			if((Hlong)InkChipMean[0] > 180 && (Hlong)(YSize/XSize) < 3)
			{
				union2(SelectRgn, *rslt_region, rslt_region );
				rslt = FALSE;
						
				if(win)
				{
					set_color(win, HTuple("red"));
					disp_obj(*rslt_region, win);
				}
			}
		}

		if(!rslt)
		{
			connection(*rslt_region, rslt_region);
			count_obj(*rslt_region, &Num);
			*rslt_num = (Hlong)Num[0];
		}	
	}
	//InkChip검사 추가됨 //CHOKI20111121 검사결과는 기포불량으로 나옴

	/////////////////////////////////////////////////
	//찍힘///////////////////////////////////////////
	/////////////////////////////////////////////////
	long CheckDentCnt(0);

	Hobject FoldInsRgnEmpha, FoldImageReduced,FoldImageEmpha,FoldImageInvert, FoldRegion;

	affine_trans_region(RegionRect,&RegionRect,HomMat2D,HTuple("false"));

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	// chip 검색 영역  //CHOKI20110506
	gen_rectangle1(&ChipSearchRect, (Hlong)(chip_search_region.top + offset_y), 
								(Hlong)(chip_search_region.left + offset_x), 
								(Hlong)(chip_search_region.bottom + offset_y), 
								(Hlong)(chip_search_region.right + offset_x));
			
	gen_rectangle1(&ChipRegionRect, (Hlong)(ins_film_region.top + offset_y), 
								(Hlong)(ins_film_region.left + offset_x), 
								(Hlong)(ins_film_region.bottom + offset_y), 
								(Hlong)(ins_film_region.right + offset_x));


	difference(ChipRegionRect, ChipSearchRect, &TempRgn1);

	reduce_domain(ImageTrans2, ChipRegionRect, &PunchReduce);

	difference(TempRgn1, PunchRgn, rslt_hallins_region);

	///////////////////////////////////////////////////
	//찍힘 / 구겨짐 ///////////////////////////////////
	///////////////////////////////////////////////////
	Hobject HallImage, InsRgn, OutRgn2;

	Hobject RefImageReduced2, RegionThreshRef2, RegionEmpRef2;
	Hobject TempDilationRef1, TempDilationRef2, TempErosionRef1, TempErosionRef2, TempFillUp1, TempFillUp2;
	Hobject InterRefRgn;
	HTuple IntenInterRgn;

	//어두운 것
	fold_thresh = 255 - fold_thresh;//by 

 	high_threshold = fold_thresh + 20;
	thresh = fold_thresh;       

	//조명 180 / Blue : 1.92157
 	if(0 >= thresh) 
	{
		thresh = 10;  
		high_threshold = 30;
	}
	else if(255 <= high_threshold)
	{
		thresh = 230;  
		high_threshold = 255;
	}

	//어두운 것

	erosion_rectangle1(*rslt_hallins_region, &InsRgn, 10, 10);
	
	emphasize(*hall_image, &FoldInsRgnEmpha,HTuple(25),HTuple(25),HTuple(2));
	reduce_domain(FoldInsRgnEmpha, InsRgn, &HallImage);
	invert_image(HallImage, &FoldImageInvert); //이미지 역상 
	mean_image(FoldImageInvert, &FoldImageInvert,10,10);

	set_check("~give_error");
	herr = hysteresis_threshold(FoldImageInvert,&FoldRegion,HTuple(thresh),HTuple(high_threshold),HTuple(20));
	//hysteresis는 밝은색을 찾는 함수(현재는 검은색을 찾기위해 역상 이미지를 사용)
	//herr = hysteresis_threshold(FoldImageInvert,&FoldRegion,HTuple(60),HTuple(high_threshold),HTuple(20));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_00!... at %ld, %ld",(Hlong)thresh, (Hlong)high_threshold);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(FoldRegion, &FoldRegion);

    if(win)
	{
		
		disp_obj(FoldImageInvert, win);
		set_color(win, HTuple("green"));
		disp_obj(*rslt_hallins_region, win);
		set_color(win, HTuple("blue"));
		disp_obj(FoldRegion, win);
	}

	select_shape(FoldRegion, &FoldRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(fold_tol).Concat(fold_tol), HTuple("max").Concat("max"));
	select_shape(FoldRegion, &FoldRegion, HTuple("compactness"), HTuple("and"), HTuple(0), HTuple(20));
	count_obj(FoldRegion, &Num);

	count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
	sprintf(temp,"[2:%f]", (double)(HTuple(end_t - start_t)[0]) );
	TRACE(temp);
	count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////

	long LimitTopSide, LimitBtmSide;
	smallest_rectangle1(*rslt_hallins_region, &Row0, &Col0, &Row1, &Col1);

	LimitTopSide = (Hlong)Row0[0] + 200;
	LimitBtmSide = (Hlong)Row1[0] - 200;

	HTuple Area, SelectRow;
	HTuple Ra, Rb, nCompactness; 
	CheckDentCnt = (Hlong)Num[0];
	HTuple InterArea; 

	BOOL m_bCheckResult; 

	//LSS new
	m_bCheckResult = FALSE; 
	if(0 < CheckDentCnt)
	{
		concat_obj(*rslt_hall_region, FoldRegion, rslt_hall_region);
		*rslt_hall_num = CheckDentCnt;
		m_bCheckResult = TRUE; 
	}

	count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
	sprintf(temp,"[3:%f]", (double)(HTuple(end_t - start_t)[0]) );
	TRACE(temp);

	if(0 < *rslt_hall_num)
	{
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_hall_region, win);
		}
		
		rslt = FALSE;
	}

	return rslt;
}

//구겨짐, 찍힘
BOOL C_VisionModule::_Hal_FilmFold(Hlong win, Hobject *fold_image, Hobject *hall_image,
							  Hobject mask_region,
							  RECT film_region, RECT chip_region,
							  double first_offset_ox, double first_offset_oy,			//첫 device의 패턴등록점		
							  double first_offset_ox2, double first_offset_oy2, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,

							  int hall_thresh, int hall_tol,
							  int fold_thresh, int fold_tol,

							  int sub_sr_rgn_num, int sub_sr_rgn_x[][SR_SUBREGION_POINT_MAX_NUM], int sub_sr_rgn_y[][SR_SUBREGION_POINT_MAX_NUM],	//20150123 NEW by
							  int sub_sr_rgn_type[SR_SUBREGION_MAX_NUM], //0 : 세밀한 패턴의 SR, 1 : 굵은 패턴의 SR

							   Hlong *rslt_fold_num, Hobject *rslt_fold_region, Hobject *rslt_foldins_region,
							   Hlong *rslt_hall_num, Hobject *rslt_hall_region, Hobject *rslt_hallins_region)
{
	BOOL rslt;
	rslt = TRUE;

	HTuple DiffAng;
	double offset_x, offset_y;
	Hobject FilmRegion, ChipRegion;
	HTuple HomMat2D, Num;
	HTuple FirstHomMat2D;
	HTuple FirstDiffAng;

	Hobject TotSRSubRegion_Small, SRSubRegion, InsRegion, TotSRSubRegion_Large;
	HTuple Sub_Rgn_Num_X, Sub_Rgn_Num_Y;
	int i, j;

	Hlong row, col, leng, height, idx;
	HTuple MeasureHandle0, MeasureHandle1, Width, Height;
	Hobject  Line, TotLine;
	HTuple   RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond;
	HTuple   ColumnEdgeSecond, AmplitudeSecond, IntraDistance, InterDistance;
	Hobject EmpImage, MeanImage;
	Hobject RsltRegion, RsltRegion_Small, RsltRegion_Large;

	Hobject MaskRegion;

	gen_empty_obj(&Line);
	gen_empty_obj(&TotLine);
	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltRegion_Small);
	gen_empty_obj(&RsltRegion_Large);

	double factor_vert = 5;
	double factor_hori = 2;

	get_image_pointer1(*fold_image, _, _, &Width, &Height);
	
	//emphasize(*fold_image, &EmpImage, 20, 20, 3);
	mean_image(*fold_image, &MeanImage, 10, 30);//width, height

	if(win)
	{
		//disp_obj(MeanImage, win);
	}

	/*
	//Vert-scan//////////////////////////////////////////////////////////////////////////////////////////////
	row = (film_region.top + film_region.bottom) * 0.5;
	col = film_region.left + 1;
	leng = (film_region.bottom - film_region.top) * 0.5;
	height = (film_region.right - film_region.left) - 1;

	gen_measure_rectangle2(row,col,HTuple(1.57079632679),leng,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
	gen_measure_rectangle2(row,col,HTuple(-1.57079632679),leng,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);

	for(idx=1; idx<=height; idx+=4)
	{
		translate_measure(MeasureHandle0, row, col + idx);

		measure_pos(MeanImage, MeasureHandle0, HTuple(factor_vert), HTuple(fold_thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst, ColumnEdgeFirst);
			union2(TotLine, Line, &TotLine);
		}

	}
	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Hori-scan//////////////////////////////////////////////////////////////////////////////////////////////
	row = film_region.top + 1;
	col = (film_region.left + film_region.right) * 0.5;
	leng = (film_region.right - film_region.left) * 0.5;
	height = (film_region.bottom - film_region.top) - 1;

	gen_measure_rectangle2(row,col,HTuple(0),leng,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
	gen_measure_rectangle2(row,col,HTuple(PI),leng,HTuple(2),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);

	for(idx=1; idx<=height; idx+=4)
	{
		translate_measure(MeasureHandle0, row + idx, col);

		measure_pos(MeanImage, MeasureHandle0, HTuple(factor_hori), HTuple(fold_thresh), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);
			union2(TotLine, Line, &TotLine);
		}

	}
	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	*/
	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	film_region.top = film_region.top + offset_y;
	film_region.left = film_region.left + offset_x;	
	film_region.bottom = film_region.bottom + offset_y;
	film_region.right = film_region.right + offset_x;

	chip_region.top = chip_region.top + offset_y;
	chip_region.left = chip_region.left + offset_x;	
	chip_region.bottom = chip_region.bottom + offset_y;
	chip_region.right = chip_region.right + offset_x;

	gen_rectangle1(&FilmRegion, (Hlong)(film_region.top), 
								(Hlong)(film_region.left), 
								(Hlong)(film_region.bottom), 
								(Hlong)(film_region.right));

	gen_rectangle1(&ChipRegion, (Hlong)(chip_region.top + offset_y), 
							  (Hlong)(chip_region.left + offset_x), 
							  (Hlong)(chip_region.bottom + offset_y), 
							  (Hlong)(chip_region.right + offset_x));

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, first_offset_oy, first_offset_ox, first_offset_oy2, first_offset_ox2, &FirstDiffAng);
	vector_angle_to_rigid(first_offset_oy, first_offset_ox, FirstDiffAng, offset_ry, offset_rx, HTuple(0), &FirstHomMat2D);
	
	affine_trans_region(mask_region, &MaskRegion, HomMat2D, HTuple("false"));

	//결과 초기화
	gen_empty_obj(&TotSRSubRegion_Small);
	gen_empty_obj(&TotSRSubRegion_Large);
	
	for(i = 0; i < sub_sr_rgn_num; i++)
	{
		//Sub region 만들기///////////////////////////////////////////////////////////////////////////////////////
		Sub_Rgn_Num_X.Reset();
		Sub_Rgn_Num_Y.Reset();

		for(j = 0; j < SR_SUBREGION_POINT_MAX_NUM; j++)
		{
			if(0 < sub_sr_rgn_x[i][j])
			{
				tuple_concat(Sub_Rgn_Num_X, sub_sr_rgn_x[i][j], &Sub_Rgn_Num_X);
				tuple_concat(Sub_Rgn_Num_Y, sub_sr_rgn_y[i][j], &Sub_Rgn_Num_Y);
			}
			else
			{
				break;
			}
		}

		gen_region_polygon_filled(&SRSubRegion, Sub_Rgn_Num_Y, Sub_Rgn_Num_X);

		affine_trans_region(SRSubRegion, &SRSubRegion, FirstHomMat2D, HTuple("false"));	

		if(0 == sub_sr_rgn_type[i])
		{
			//세부패턴영역
			concat_obj(TotSRSubRegion_Small, SRSubRegion, &TotSRSubRegion_Small);
		}
		else
		{
			//굵은패턴영역
			difference(SRSubRegion, MaskRegion, &SRSubRegion);
			concat_obj(TotSRSubRegion_Large, SRSubRegion, &TotSRSubRegion_Large);
		}
	}

	difference(TotLine, ChipRegion, &TotLine);
	intersection(TotLine, FilmRegion, &TotLine);

	if(win)
	{
		set_color(win, HTuple("cyan"));
		disp_obj(TotLine, win);
	}

	erosion_circle(FilmRegion, &InsRegion, 1.5);
	intersection(InsRegion, TotLine, &RsltRegion);

	intersection(TotSRSubRegion_Large, RsltRegion, &RsltRegion_Large);

	dilation_circle(RsltRegion_Large, &RsltRegion_Large, 10);
	erosion_circle(RsltRegion_Large, &RsltRegion_Large, 9);
	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(RsltRegion_Large, win);
	}

	connection(RsltRegion_Large, &RsltRegion_Large);
	select_shape(RsltRegion_Large, &RsltRegion_Large, HTuple("area"), HTuple("and"), HTuple(50), HTuple("max"));
	union1(RsltRegion_Large, &RsltRegion_Large);
	closing_circle(RsltRegion_Large, &RsltRegion_Large, 50);

	//union2(RsltRegion_Small, RsltRegion_Large, &RsltRegion);
	connection(RsltRegion_Large, &RsltRegion);
	select_shape(RsltRegion, &RsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(fold_tol).Concat(fold_tol), HTuple("max").Concat("max"));
	
	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(RsltRegion, win);
	}

	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(InsRegion, win);
		disp_obj(TotSRSubRegion_Small, win);
		disp_obj(TotSRSubRegion_Large, win);
	}
	count_obj(RsltRegion, &Num);
	*rslt_fold_num = (Hlong)Num[0];
	if(0 < *rslt_fold_num)
	{
		rslt = FALSE;

		concat_obj(*rslt_fold_region, RsltRegion, rslt_fold_region);
	}

	concat_obj(*rslt_foldins_region, TotSRSubRegion_Small, rslt_foldins_region);
	concat_obj(*rslt_foldins_region, TotSRSubRegion_Large, rslt_foldins_region);

	

	///////////////////////////////////////////////////
	//찍힘
	///////////////////////////////////////////////////
	CString status;
	Hobject HallImage, InsRgn, OutRgn2;

	Hobject RefImageReduced2, RegionThreshRef2, RegionEmpRef2;
	Hobject TempDilationRef1, TempDilationRef2, TempErosionRef1, TempErosionRef2, TempFillUp1, TempFillUp2;
	Hobject InterRefRgn;
	HTuple IntenInterRgn;
	HTuple Row0, Col0, Row1, Col1;

	int thresh, high_threshold;

	Herror herr;
	Hobject FoldRegion, FoldInsRgnEmpha, FoldImageInvert;
	long CheckDentCnt(0);

	//int hall_thresh, int hall_tol,
	Hobject FilmSearchRect;
	gen_rectangle1(&FilmSearchRect, (Hlong)(film_region.top + offset_y), 
								(Hlong)(film_region.left + offset_x), 
								(Hlong)(film_region.bottom + offset_y), 
								(Hlong)(film_region.right + offset_x));

	//어두운 것
	hall_thresh = 255 - hall_thresh;//by 

 	high_threshold = hall_thresh + 20;
	thresh = hall_thresh;       

	//조명 180 / Blue : 1.92157
 	if(0 >= thresh) 
	{
		thresh = 10;  
		high_threshold = 30;
	}
	else if(255 <= high_threshold)
	{
		thresh = 230;  
		high_threshold = 255;
	}

	//어두운 것

	emphasize(*hall_image, &FoldInsRgnEmpha,HTuple(25),HTuple(25),HTuple(2));
	reduce_domain(FoldInsRgnEmpha, FilmSearchRect, &HallImage);
	invert_image(HallImage, &FoldImageInvert); //이미지 역상 
	mean_image(FoldImageInvert, &FoldImageInvert,10,10);

	set_check("~give_error");
	herr = hysteresis_threshold(FoldImageInvert,&FoldRegion,HTuple(thresh),HTuple(high_threshold),HTuple(20));
	//hysteresis는 밝은색을 찾는 함수(현재는 검은색을 찾기위해 역상 이미지를 사용)
	//herr = hysteresis_threshold(FoldImageInvert,&FoldRegion,HTuple(60),HTuple(high_threshold),HTuple(20));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_00!... at %ld, %ld",(Hlong)thresh, (Hlong)high_threshold);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	intersection(TotSRSubRegion_Small, FoldRegion, &FoldRegion);
	connection(FoldRegion, &FoldRegion);

    if(win)
	{
		
		disp_obj(FoldImageInvert, win);
		set_color(win, HTuple("green"));
		disp_obj(*rslt_hallins_region, win);
		set_color(win, HTuple("blue"));
		disp_obj(FoldRegion, win);
	}


	select_shape(FoldRegion, &FoldRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(hall_tol).Concat(hall_tol), HTuple("max").Concat("max"));
	select_shape(FoldRegion, &FoldRegion, HTuple("compactness"), HTuple("and"), HTuple(0), HTuple(20));
	count_obj(FoldRegion, &Num);

	long LimitTopSide, LimitBtmSide;
	//smallest_rectangle1(*rslt_hallins_region, &Row0, &Col0, &Row1, &Col1);

	//LimitTopSide = (Hlong)Row0[0] + 200;
	//LimitBtmSide = (Hlong)Row1[0] - 200;

	HTuple Area, SelectRow;
	HTuple Ra, Rb, nCompactness; 
	CheckDentCnt = (Hlong)Num[0];
	HTuple InterArea; 

	BOOL m_bCheckResult; 

	//LSS new
	m_bCheckResult = FALSE; 
	if(0 < CheckDentCnt)
	{
		concat_obj(*rslt_hall_region, FoldRegion, rslt_hall_region);
		*rslt_hall_num = CheckDentCnt;
		m_bCheckResult = TRUE; 
	}

	if(0 < *rslt_hall_num)
	{
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_hall_region, win);
		}
		
		rslt = FALSE;
	}

	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(*rslt_hall_region, win);
	}

	return rslt;
}

//////////////////////////////////////////////////////////////////////
// Vision module
//////////////////////////////////////////////////////////////////////

C_VisionModule::C_VisionModule()
{
	int i;

	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		m_VDat[i] = new C_VisionData;
	}

	ResetInspectPara();

	ResetSystemPara();
	ResetSystemRslt();
	ResetMainSysPara();
	ResetMainSysRslt();



	ResetLotInfo();

	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		ResetInspectRslt(i);
		ResetInspectRsltRgn(i);
		ResetInspectRgn(i);
		ResetShiftRslt(i);
		ResetInspectNGRslt(i);
		ResetInspectNGRsltRgn(i);

		m_lWindow[i] = 0;
	}

	m_iNGViewMode = -1;

	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		m_bSRBubPrepare[i] = FALSE;
	}
}

C_VisionModule::~C_VisionModule()
{
	clear_all_shape_models();
	//clear_all_variation_models();

	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		delete m_VDat[i];
	}
}


//CHOKI20101101
void C_VisionModule::CopyRGBtoGrayImage(int cam_idx, int buf_idx, int img_idx)
{
/*	rgb1_to_gray(&m_VDat->m_HalColorImage[m_iBufIdx][i], m_VDat->m_HalImage[m_iBufIdx][i]);
	decompose3(&m_VDat->m_HalColorImage[m_iBufIdx][i], &m_VDat->HalRedImage[m_iBufIdx][i], 
	&m_VDat->&HalGreenImage[m_iBufIdx][i], &m_VDat->HalBlueImage[m_iBufIdx][i]);*/
			
}


//Get/Set data
Hobject *C_VisionModule::GetHalImage(int cam_idx, int buf_idx, int img_idx)
{
	
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGImage[m_iNGViewMode][img_idx];
	}

}

Hobject *C_VisionModule::GetHalRotateImage(int cam_idx, int buf_idx, int img_idx)
{
	return &m_VDat[cam_idx]->m_HalRotateImage[buf_idx][img_idx];
}

Hobject *C_VisionModule::GetHalColorImage(int cam_idx, int buf_idx, int img_idx)
{
	return &m_VDat[cam_idx]->m_HalColorImage[buf_idx][img_idx];
}

Hobject *C_VisionModule::GetHalRedImage(int cam_idx, int buf_idx, int img_idx)
{
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalRedImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGRedImage[buf_idx][img_idx];
	}
}

Hobject *C_VisionModule::GetHalGreenImage(int cam_idx, int buf_idx, int img_idx)
{
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalGreenImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGGreenImage[buf_idx][img_idx];
	}
}

Hobject *C_VisionModule::GetHalBlueImage(int cam_idx, int buf_idx, int img_idx)
{
	if(-1 == m_iNGViewMode)
	{
		return &m_VDat[cam_idx]->m_HalBlueImage[buf_idx][img_idx];
	}
	else
	{
		return &m_VDat[cam_idx]->m_HalNGBlueImage[buf_idx][img_idx];
	}
}

Hobject *C_VisionModule::GetHalRefRedImage(int cam_idx, int buf_idx, int img_idx)
{
	return &m_VDat[cam_idx]->m_HalRefRedImage[buf_idx][img_idx];
}

Hobject *C_VisionModule::GetHalRefGreenImage(int cam_idx, int buf_idx, int img_idx)
{
	return &m_VDat[cam_idx]->m_HalRefGreenImage[buf_idx][img_idx];
}

Hobject *C_VisionModule::GetHalRefBlueImage(int cam_idx, int buf_idx, int img_idx)
{
	return &m_VDat[cam_idx]->m_HalRefBlueImage[buf_idx][img_idx];
}

Hobject *C_VisionModule::GetHalNGImage(int cam_idx, int ng_view_idx, int img_idx)
{
	return &m_VDat[cam_idx]->m_HalNGImage[ng_view_idx][img_idx];
}	


void C_VisionModule::SetHalNGImage(int cam_idx, int buf_idx, int ng_view_idx)
{
	int i;

	if(-1 == m_iNGViewMode)
	{
		for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[1]) && (i < IMAGE_MAP_MAX_NUM); i++)
		{
			copy_image(m_VDat[cam_idx]->m_HalImage[buf_idx][i], &m_VDat[cam_idx]->m_HalNGImage[ng_view_idx][i]);
		}
	}
}

Hobject *C_VisionModule::GetHalRsltRgn(int cam_idx, int rgn_idx, int ins_idx)
{
	return &m_VDat[cam_idx]->m_HalRsltRgn[rgn_idx][ins_idx];
}

Hobject *C_VisionModule::GetHalInspectRgn(int cam_idx, int rgn_idx, int ins_idx)
{
	return &m_VDat[cam_idx]->m_HalInsRgn[rgn_idx][ins_idx];
}

Hobject *C_VisionModule::GetHalNGRsltRgn(int cam_idx, int ng_view_idx, int rgn_idx, int ins_idx)
{
	return &m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][rgn_idx][ins_idx];
}

Hobject *C_VisionModule::GetHalMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalMasterImage;
}

Hobject *C_VisionModule::GetHalSRMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalSRMasterImage;
}

Hobject *C_VisionModule::GetHalLeadMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalLeadMasterImage;
}

Hobject *C_VisionModule::GetHalSROutMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalSROutMasterImage;
}

Hobject *C_VisionModule::GetHalChipMasterImage(int cam_idx)
{
	return &m_VDat[cam_idx]->m_HalChipMasterImage;
}

Hobject *C_VisionModule::GetHalTaughtImage(int cam_idx, int map_idx)
{
	return &m_VDat[cam_idx]->m_HalTaughtImage[map_idx];
}

typeInspectPara C_VisionModule::GetInspectPara(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsPara;
}

void C_VisionModule::SetInspectPara(int cam_idx, typeInspectPara para)
{
	m_VDat[cam_idx]->m_InsPara = para;
}

void C_VisionModule::ResetInspectPara()
{
	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		memset(&m_VDat[i]->m_InsPara, NULL, sizeof(m_VDat[i]->m_InsPara));
	}
}

typeInspectRslt C_VisionModule::GetInspectRslt(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsRslt;
}

void C_VisionModule::SetInspectRslt(int cam_idx, typeInspectRslt rslt)
{
	m_VDat[cam_idx]->m_InsRslt = rslt;
}

void C_VisionModule::ResetInspectRslt(int cam_idx)
{
	
	memset(&m_VDat[cam_idx]->m_InsRslt, NULL, sizeof(m_VDat[cam_idx]->m_InsRslt));
}

void C_VisionModule::ResetInspectRsltRgn(int cam_idx)
{
	int i, j, k;
	
	i = cam_idx;
	//Variable clear
	for(j = 0; j < SUB_RGN_MAX_NUM; j++)
	{
		for(k = 0; k < RESULT_INS_MAX_NUM; k++)
		{
			//clear_obj(m_VDat[i]->m_HalRsltRgn[j][k]); 
			gen_empty_obj(&m_VDat[i]->m_HalRsltRgn[j][k]);
		}
	}
}

void C_VisionModule::ResetInspectRgn(int cam_idx)
{
	int i, j, k;

	i = cam_idx;
	
	//Variable clear
	for(j = 0; j < SUB_RGN_MAX_NUM; j++)
	{
		for(k = 0; k < INS_RGN_MAX_NUM; k++)
		{
			//clear_obj(m_VDat[i]->m_HalRsltRgn[j][k]); 
			gen_empty_obj(&m_VDat[i]->m_HalInsRgn[j][k]);
		}
	}
}

typeInspectNGRslt C_VisionModule::GetInspectNGRslt(int cam_idx, int ng_view_idx)
{
	return m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx];
}

void C_VisionModule::SetInspectNGRslt(int cam_idx, int ng_view_idx, typeShiftRslt shift_rslt, typeInspectRslt ins_rslt)
{
	m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_ShiftRslt = shift_rslt;
	m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_InsRslt = ins_rslt;
}

BOOL C_VisionModule::SaveInspectNGRslt(int cam_idx, int ng_view_idx, int rgn_idx, CString path)
{
	BOOL rslt = TRUE;

	Hobject SubRgn, TotRgn, UnionSubRgn;
	HTuple Num;
	int num;

	CString temp;
	
	gen_empty_obj(&SubRgn);
	gen_empty_obj(&TotRgn);
	gen_empty_obj(&UnionSubRgn);
	
	//Region/////////////////////////////////////
	for(int i = 0; i < RESULT_INS_MAX_NUM; i++)
	{
		count_obj(*GetHalNGRsltRgn(cam_idx, ng_view_idx, rgn_idx, i), &Num);
		num = (Hlong)Num[0];
		if( 0 == num )
		{
			gen_empty_region(&UnionSubRgn);
		}
		else
		{
			//if XLD
			if( (i == 2) || (i = 43) )//2 : align pattern, 43 : OCV pattern
			{
				SubRgn = *GetHalNGRsltRgn(cam_idx, ng_view_idx, rgn_idx, i);
				//gen_region_contour_xld(SubRgn, &SubRgn, "fill");
				union1(SubRgn, &UnionSubRgn);
			}
			else
			{
				SubRgn = *GetHalNGRsltRgn(cam_idx, ng_view_idx, rgn_idx, i);
				union1(SubRgn, &UnionSubRgn);
			}
		}
		
		concat_obj(TotRgn, UnionSubRgn, &TotRgn);
	}
	
	//Write region
	temp = path + ".rgn";
	write_region(TotRgn, temp);

	//Data///////////////////////////////////////
	m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx = rgn_idx;

	temp = path + ".dat";
	//NG inspection result
	//typeInspectNGRslt m_InsNGRslt[NG_VIEW_MAX_NUM];
	HANDLE fd=CreateFile(   temp,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		//AfxMessageBox("Error : Can't save NG result file!" + path_sys);
		rslt = FALSE;
	}
	else
	{
		DWORD len;

		WriteFile(fd, &m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx], sizeof(m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx]), &len, NULL);

		CloseHandle(fd);
	}

	fd=0;
		
	return rslt;	
}

BOOL C_VisionModule::LoadInspectNGRslt(int cam_idx, int ng_view_idx, CString path)
{
	BOOL rslt = TRUE;

	Hobject SubRgn, TotRgn;
	HTuple Num;
	long num;
	CString temp;
	int rgn_idx, i;

	temp = path + ".dat";
	HANDLE fd = CreateFile( temp,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		//AfxMessageBox("Error : Can't load NG result file!" + path_sys);
		rslt = FALSE;
	}
	else
	{
		DWORD len;

		//Setup
		ReadFile(fd, &m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx], sizeof(m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx]), &len, NULL);

		CloseHandle(fd);
	}

	fd=0;

	if( (0 > m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx) || (NG_VIEW_MAX_NUM <= m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx) )
	{
		m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx = -1;
	}

	rgn_idx = m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx;

	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		if(i == rgn_idx)
		{
			m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_InsRslt.m_bRegionSkip[i] = TRUE;
		}
		else
		{
			m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_InsRslt.m_bRegionSkip[i] = FALSE;			
		}
	}
	
	//Region/////////////////////////////////////
	//Read region
	temp = path + ".rgn";
	read_region(&TotRgn, temp);
	count_obj(TotRgn, &Num);
	num = (Hlong)Num[0];
	//Region union
	for(i = 0; (i < RESULT_INS_MAX_NUM) && (i < num); i++)
	{
		select_obj(TotRgn, GetHalNGRsltRgn(cam_idx, ng_view_idx, m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx, i), i + 1);
		connection(*GetHalNGRsltRgn(cam_idx, ng_view_idx, m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx, i), 
					GetHalNGRsltRgn(cam_idx, ng_view_idx, m_VDat[cam_idx]->m_InsNGRslt[ng_view_idx].m_iRgnIdx, i));
	}
	
	return rslt;
}

void C_VisionModule::ResetInspectNGRslt(int cam_idx)
{
	int i, j;
	i = cam_idx;
	for(j = 0; j < NG_VIEW_MAX_NUM; j++)
	{
		memset(&m_VDat[i]->m_InsNGRslt[j], NULL, sizeof(m_VDat[i]->m_InsNGRslt[j]));
	}
}

void C_VisionModule::ResetInspectNGRsltRgn(int cam_idx)
{
	int i, j, k, l;
	
	i = cam_idx;
	//Variable clear
	
	for(j = 0; j < NG_VIEW_MAX_NUM; j++)
	{
		for(k = 0; k < SUB_RGN_MAX_NUM; k++)
		{
			for(l = 0; l < RESULT_INS_MAX_NUM; l++)
			{
				//clear_obj(m_VDat[i]->m_HalNGRsltRgn[j][k][l]); 
				gen_empty_obj(&m_VDat[i]->m_HalNGRsltRgn[j][k][l]);
				//gen_empty_region(&m_VDat[i]->m_HalNGRsltRgn[j][k][l]);
			}
		}
	}
	
}

void C_VisionModule::SetInspectNGRsltRgn(int cam_idx, int ng_view_idx)
{
	int i, j, k;
	long num;
	HTuple Num;
	Hobject Region;
	
	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		for(j = 0; j < RESULT_INS_MAX_NUM; j++)
		{
			//clear_obj(m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j]); 
			gen_empty_obj(&m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j]);

			count_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][j], &Num);
			num = (Hlong)Num[0];
			for(k = 0; k < num; k++)
			{
				//copy_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][j], &m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j], k + 1, 1); 
				copy_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][j], &Region, k + 1, 1); 
				concat_obj(m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j], 
						   Region, 
						   &m_VDat[cam_idx]->m_HalNGRsltRgn[ng_view_idx][i][j]);
			}
		}
	}

}

typeSystemPara C_VisionModule::GetSystemPara(int cam_idx)
{
	return m_VDat[cam_idx]->m_SysPara;
}

void C_VisionModule::SetSystemPara(int cam_idx, typeSystemPara rslt)
{
	m_VDat[cam_idx]->m_SysPara = rslt;
}

void C_VisionModule::ResetSystemPara()
{
	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		memset(&m_VDat[i]->m_SysPara, NULL, sizeof(m_VDat[i]->m_SysPara));
	}
}

typeSystemRslt C_VisionModule::GetSystemRslt(int cam_idx)
{
	return m_VDat[cam_idx]->m_SysRslt;
}

typeShiftRslt C_VisionModule::GetShiftRslt(int cam_idx, int buf_idx)
{
	return m_VDat[cam_idx]->m_ShiftRslt[buf_idx];
}

int C_VisionModule::GetCOFNum(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsPara.m_iParaBase[0];
}

int C_VisionModule::GetMapNum(int cam_idx)
{
	return m_VDat[cam_idx]->m_InsPara.m_iParaBase[1];
}

int C_VisionModule::GetPCNum(int cam_idx)
{
	return m_VDat[cam_idx]->m_SysPara.m_iPCIdx;
}

void C_VisionModule::SetSystemRslt(int cam_idx, typeSystemRslt rslt)
{
	m_VDat[cam_idx]->m_SysRslt = rslt;
}

void C_VisionModule::SetShiftRslt(int cam_idx, int buf_idx, typeShiftRslt rslt)
{
	m_VDat[cam_idx]->m_ShiftRslt[buf_idx] = rslt;
}

void C_VisionModule::ResetSystemRslt()
{
	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		memset(&m_VDat[i]->m_SysRslt, NULL, sizeof(m_VDat[i]->m_SysRslt));
	}
}

void C_VisionModule::ResetShiftRslt(int cam_idx)
{
	int i, j;
	
	i = cam_idx;
	
	for(j = 0; j < IMAGE_BUF_NUM; j++)
	{
		memset(&m_VDat[i]->m_ShiftRslt[j], NULL, sizeof(m_VDat[i]->m_ShiftRslt[j]));
	}
}

typeMainSystemPara C_VisionModule::GetMainSysPara()
{
	return m_MainSysPara;
}

void C_VisionModule::SetMainSysPara(typeMainSystemPara rslt)
{
	m_MainSysPara = rslt;
}

void C_VisionModule::ResetMainSysPara()
{
	memset(&m_MainSysPara, NULL, sizeof(m_MainSysPara));
}

typeMainSystemRslt C_VisionModule::GetMainSysRslt()
{
	return m_MainSysRslt;
}

void C_VisionModule::SetMainSysRslt(typeMainSystemRslt rslt)
{
	m_MainSysRslt = rslt;
}

void C_VisionModule::ResetMainSysRslt()
{
	memset(&m_MainSysRslt, NULL, sizeof(m_MainSysRslt));
}

typeLotInfo C_VisionModule::GetLotInfo()
{
	return m_Lot;
}

void C_VisionModule::SetLotInfo(typeLotInfo lot)
{
	m_Lot = lot;
}

void C_VisionModule::ResetLotInfo()
{
	memset(&m_Lot, NULL, sizeof(m_Lot));
}

//Set handle(for test)
void C_VisionModule::SetWindow(int cam_idx, Hlong win)
{
	m_lWindow[cam_idx] = win;
}

BOOL C_VisionModule::LoadInspectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	char temp[DAT_STR_LENG];
	int i, j, k;
	CString file_path, pat_path, extention, master_path;
	Herror Herr;

	file_path = path + "\\Inspection.par";

	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			switch(i)
			{
			case 0:
			case 1:
				//Base///////////////////////////////
				for(j = 0; j < BOOL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_BASE%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaBase[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_BASE%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaBase[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_BASE%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaBase[j] = atof(temp);
				}
				
				//Align//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_ALIGN; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Align%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaAlign[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Align%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaAlign[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Align%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaAlign[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Align%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapAlign[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				//fixed
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_shType = VISIONHALFUNC_MOD_RECT1;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_AngleStart = 0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_AngleExtend = 0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_AngleStep = 0;
				sprintf(m_VDat[i]->m_InsPara.m_PatParaAlign.m_Optimize, "none");
				sprintf(m_VDat[i]->m_InsPara.m_PatParaAlign.m_Matric, "ignore_global_polarity");
				sprintf(m_VDat[i]->m_InsPara.m_PatParaAlign.m_SupPix, "interpolation");
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_Overlap = 0;
				
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_MatchNum = 1;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_Greed = 0.5;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_ScaleMin = 1.0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_ScaleMax = 1.0;
				m_VDat[i]->m_InsPara.m_PatParaAlign.m_ScaleStep = 0;
				
				//read shape region
				pat_path = path + "\\PatternAlign."; 
				extention.Format("m%d", i);
				pat_path = pat_path + extention;
				set_check("~give_error");
				Herr = read_shape_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_PatIDAlign);
				set_check("give_error");
				if(H_MSG_TRUE == Herr)
				{
					m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;

					// 20110531 Dongwhe* BEGIN 2nd Align Mark Shape Load
					// 두개의 마크 정보가 모두 읽히면 TRUE가 되도록 하기 위해
					// 첫번째 마크 정보 로딩에 성공한 경우에 두번째 마크 정보 로딩 시도를 하도록 작성
					// read shape region2
					pat_path = path + "\\PatternAlign2."; 
					extention.Format("m%d", i);
					pat_path = pat_path + extention;
					set_check("~give_error");
					Herr = read_shape_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_PatIDAlign2);
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;	
					}
					else
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
					}
					// 20110531 Dongwhe* END
				}
				else
				{
					m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
				}

				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < PATTERN_INS_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPosX_Align%d_Pos%d", i, j, k);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						m_VDat[i]->m_InsPara.m_dPatternPosXAlign[j][k] = atof(temp);
						
						key.Empty();
						key.Format("CAM%d_iPosY_Align%d_Pos%d", i, j, k);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						m_VDat[i]->m_InsPara.m_dPatternPosYAlign[j][k] = atof(temp);
					}
				}

				//20150119 by
				key.Empty();
				key.Format("CAM%d_iSmallSubRgnIdx", i);
				m_VDat[i]->m_InsPara.m_iSmallSubRgnIdx = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				
				for(j = 0; j < SR_SUBREGION_MAX_NUM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iSmallSubRgnType%d", i, j);
					m_VDat[i]->m_InsPara.m_iSmallSubRgnType[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					key.Empty();
					key.Format("CAM%d_iSmallSubRgnPointIdx%d", i, j);
					m_VDat[i]->m_InsPara.m_iSmallSubRgnPointIdx[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					for(k = 0; k < SR_SUBREGION_POINT_MAX_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iSmallSubRgnX%d_Points%d", i, j, k);
						m_VDat[i]->m_InsPara.m_iSmallSubRgnX[j][k] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
						
						key.Empty();
						key.Format("CAM%d_iSmallSubRgnY%d_Points%d", i, j, k);
						m_VDat[i]->m_InsPara.m_iSmallSubRgnY[j][k] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
					}
				}

				//Chip///////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_CHIP; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Chip%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaChip[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Chip%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaChip[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Chip%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaChip[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Chip%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapChip[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				/////////////////////////////////////
				
				//Lead///////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_LEAD; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Lead%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaLead[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Lead%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaLead[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Lead%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaLead[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Lead%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapLead[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				/////////////////////////////////////
				
				//Film///////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_FILM; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Film%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaFilm[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Film%d", i, j);
					if(j==20) //CHOKI20110824
					{
						m_VDat[i]->m_InsPara.m_iParaFilm[j] = 100;
					}
					else //CHOKI20110824
					{
						m_VDat[i]->m_InsPara.m_iParaFilm[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
					}
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Film%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaFilm[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Film%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapFilm[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
				/*
				//read variation region
				pat_path = path + "\\VarFilm."; 
				extention.Format("v%d_%d", i, j);
				pat_path = pat_path + extention;
				set_check("~give_error");
				Herr = read_variation_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_FilmVarID[j]);
				set_check("give_error");
				if(H_MSG_TRUE == Herr)
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = TRUE;	
				}
				else
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = FALSE;
				}
					*/
					m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = FALSE;
				}
				
				/////////////////////////////////////
				
				//OCV////////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_OCV; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_OCV%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaOCV[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_OCV%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaOCV[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_OCV%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaOCV[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_OCV%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapOCV[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < OCV_CHAR_MAX_NUM; j++)
				{
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Left", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Top", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Right", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Bottom", i, j);
					m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					
					if( (0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom) )
					{
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left = 100;
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top = 100;
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right = 200;
						m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom = 200;
					}
					
					key.Empty();
					key.Format("CAM%d_Char_CenterX_OCV%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dCharCenterX[j] = atof(temp);
					
					key.Empty();
					key.Format("CAM%d_Char_CenterY_OCV%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dCharCenterY[j] = atof(temp);
					
					//fixed
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_shType = VISIONHALFUNC_MOD_RECT1;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_AngleStart = 0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_AngleExtend = 0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_AngleStep = 0;
					sprintf(m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Optimize, "none");
					sprintf(m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Matric, "use_polarity");
					sprintf(m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_SupPix, "none");
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Overlap = 0;
					
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_MatchNum = 1;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_Greed = 0.5;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_ScaleMin = 1.0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_ScaleMax = 1.0;
					m_VDat[i]->m_InsPara.m_PatParaOCV[j].m_ScaleStep = 0;
					
					//read shape region
					pat_path = path + "\\PatternOCV."; 
					extention.Format("m%d_%d", i, j);
					pat_path = pat_path + extention;
					set_check("~give_error");
					Herr = read_shape_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_PatIDOCV[j]);
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = TRUE;	
					}
					else
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = FALSE;
					}
					
					//read variation region
// 					pat_path = path + "\\VarOCV."; 
// 					extention.Format("v%d_%d", i, j);
// 					pat_path = pat_path + extention;
// 					set_check("~give_error");
// 					Herr = read_variation_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_CharVarID[j]);
// 					set_check("give_error");
// 					if(H_MSG_TRUE == Herr)
// 					{
// 						m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = TRUE;	
// 					}
// 					else
// 					{
// 						m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = FALSE;
// 					}
				}
				
				
				/////////////////////////////////////
				
				//SR/////////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_SR; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Left", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Top", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Right", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Bottom", i, j, k);
						m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
						
						if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right) || 
							(0 >= m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom) )
						{
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left = 100;
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top = 100;
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right = 200;
							m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom = 200;
						}
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_SR%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaSR[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_SR%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaSR[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_SR%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaSR[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_SR%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapSR[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					//read variation region
					//pat_path = path + "\\VarSR."; 
					//extention.Format("v%d_%d", i, j);
					//pat_path = pat_path + extention;
					//set_check("~give_error");
					//Herr = read_variation_model(HTuple(((LPCTSTR)pat_path)), &m_VDat[i]->m_InsPara.m_SRVarID[j]);
					//set_check("give_error");
					//if(H_MSG_TRUE == Herr)
					//{
					//	m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = TRUE;	
					//}
					//else
					//{
					//	m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = FALSE;
					//}
				}
				
				/////////////////////////////////////
				switch(i)
				{
				case 0:
					//Read master image
					master_path = path + "\\MasterImage.bmp"; 
					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalMasterImage,HTuple("byte"),2048,2048);
					}
					
					//Read SR master image
					master_path = path + "\\SRMasterImage.bmp"; 
					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalSRMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalSRMasterImage,HTuple("byte"),2048,2048);
					}
					
					//Read Lead master image
					master_path = path + "\\LeadMasterImage.bmp"; 
					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalLeadMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalLeadMasterImage,HTuple("byte"),2048,2048);
					}
					
					//Read SROut master image
					master_path = path + "\\SROutMasterImage.bmp"; 
					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalSROutMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalSROutMasterImage,HTuple("byte"),2048,2048);
					}

					//Read Chip master image
					master_path = path + "\\ChipMasterImage.bmp"; 
					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalChipMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalChipMasterImage,HTuple("byte"),2048,2048);
					}
					
					//Read Taught image
					for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
					{
						master_path = path + "\\TaughtImage"; 
						extention.Format("%d.bmp", j);
						master_path = master_path + extention;
						set_check("~give_error");
						Herr= read_image(&m_VDat[i]->m_HalTaughtImage[j], HTuple((LPCTSTR)master_path));
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							
						}
						else
						{
							gen_image_const(&m_VDat[i]->m_HalTaughtImage[j],HTuple("byte"),2048,2048);
						}
					}
					
					//Read inspection region
					for(j = 0; j < SUB_RGN_MAX_NUM; j++)
					{
						for(k = 0; k < INS_RGN_MAX_NUM; k++)
						{
							pat_path = path + "\\InsRegion_"; 
							extention.Format("%d_%d_%d", i, j, k);
							pat_path = pat_path + extention;
							set_check("~give_error");
							Herr = read_region(&m_VDat[i]->m_HalInsRgn[j][k], pat_path);
							set_check("give_error");
							if(H_MSG_TRUE == Herr)
							{
								
							}
							else
							{
								gen_empty_obj(&m_VDat[i]->m_HalInsRgn[j][k]);
							}
						}
					}

					break;
				case 1:
					//Read master image
					master_path = path + "\\MasterImage_bottom.bmp"; 
					set_check("~give_error");
					Herr= read_image(&m_VDat[i]->m_HalMasterImage, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						
					}
					else
					{
						gen_image_const(&m_VDat[i]->m_HalMasterImage,HTuple("byte"),2048,2048);
					}
					break;
				}
				
				//Light
				for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
				{
					for(k = 0; k < LIGHT_CH_MAX_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPara_Map%d_Light%d", i, j, k);
						m_VDat[i]->m_InsPara.m_iLightVar[j][k] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
					}
				}

				//20100702 lss
				//PPunch(펀칭부)/////////////////////
				for(j = 0; j < RECT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Left", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Top", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 100, file_path);
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Right", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Bottom", i, j);
					m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 200, file_path);
					
					if( (0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right) || 
						(0 >= m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom) )
					{
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left = 100;
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top = 100;
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right = 200;
						m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom = 200;
					}
				}
				
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_PPUNCH%d", i, j);
					m_VDat[i]->m_InsPara.m_bParaPPunch[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_PPUNCH%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaPPunch[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_PPUNCH%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					m_VDat[i]->m_InsPara.m_dParaPPunch[j] = atof(temp);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_PPUNCH%d", i, j);
					m_VDat[i]->m_InsPara.m_iParaMapPPunch[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);
				}				
				break;
			}
		}
		
		
	}
	fd=0;
	
	UpdateSRBubPrepare();
	
	return rslt;
}

 //20110513 lee
void C_VisionModule::MakeDefaultInspPara()
{
	int i;
	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		//for(j = 0; j < BOOL_PARA_MAX_NUM_BASE; j++)
		//{
		m_VDat[i]->m_InsPara.m_iParaBase[0] = 2;
		m_VDat[i]->m_InsPara.m_iParaBase[2] = 8;
		m_VDat[i]->m_InsPara.m_dParaBase[1] = COF_PF_PITCH;
		m_VDat[i]->m_InsPara.m_dParaBase[2] = COF_WIDTH;
		//20150223 by
		switch(GetVisIndex())
		{
		case VISION_CAM_TOP1:
			m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT_VIS1;
			break;								
		case VISION_CAM_TOP2:
			m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT_VIS2;
			break;
		case VISION_CAM_BOTTOM:
			m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT_VIS3;
			break;
		default:
			AfxMessageBox("Invalid Index ! MakeDefaultInspPara()");
			break;
		}
		/*
		switch(GetVisIndex())
		{
		case VISION_CAM_TOP1:
			m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT_VIS1;
			break;								
		case VISION_CAM_BOTTOM1:
			m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT_VIS2;
			break;
		case VISION_CAM_TOP2:
			m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT_VIS3;
			break;
		case VISION_CAM_BOTTOM2:
			m_VDat[i]->m_InsPara.m_iParaBase[1] = USE_MAP_CNT_VIS4;
			break;
		default:
			AfxMessageBox("Invalid Index ! MakeDefaultInspPara()");
			break;
		}
		*/
		//}
	}
}
BOOL C_VisionModule::SaveInspectPara(CString path)
{         
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j, k;
	CString file_path, pat_path, extention, master_path;
	Herror Herr;

	//Make director
	MakeDirectories(path);

	file_path = path + "\\Inspection.par";

	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		//write INI

		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			
			switch(i)
			{
			case 0:
			case 1:
				//Base///////////////////////////////
				for(j = 0; j < BOOL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_BASE%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaBase[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_BASE%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaBase[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_BASE; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_BASE%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaBase[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				//Align//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_ALIGN; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Align%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionAlign[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Align%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Align%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Align%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Align%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapAlign[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				
				//write shape region
				if(m_VDat[i]->m_InsPara.m_bCreatMdIDAlign)
				{
					pat_path = path + "\\PatternAlign."; 
					extention.Format("m%d", i);
					pat_path = pat_path + extention;
					set_check("~give_error");
					Herr = write_shape_model(m_VDat[i]->m_InsPara.m_PatIDAlign, HTuple(((LPCTSTR)pat_path)));
					set_check("give_error");
					if(H_MSG_TRUE == Herr)
					{
						// 20110531 Dongwhe* BEGIN 2nd Mark shape Save
						// 두번째 Align Mark도 저장 하도록 작성
						// m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;
						pat_path = path + "\\PatternAlign2."; 
						extention.Format("m%d", i);
						pat_path = pat_path + extention;
						set_check("~give_error");
						Herr = write_shape_model(m_VDat[i]->m_InsPara.m_PatIDAlign2, HTuple(((LPCTSTR)pat_path)));
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = TRUE;	
						}
						else
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
						}
						// 20110531 Dongwhe* END
					}
					else
					{
						m_VDat[i]->m_InsPara.m_bCreatMdIDAlign = FALSE;
					}
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < PATTERN_INS_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPosX_Align%d_Pos%d", i, j, k);
						dat.Empty();
						dat.Format("%f", m_VDat[i]->m_InsPara.m_dPatternPosXAlign[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_iPosY_Align%d_Pos%d", i, j, k);
						dat.Empty();
						dat.Format("%f", m_VDat[i]->m_InsPara.m_dPatternPosYAlign[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}


				key.Empty();
				key.Format("CAM%d_iSmallSubRgnIdx", i);
				dat.Empty();
				dat.Format("%d", m_VDat[i]->m_InsPara.m_iSmallSubRgnIdx);
				WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				
				for(j = 0; j < SR_SUBREGION_MAX_NUM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iSmallSubRgnType%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iSmallSubRgnType[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);

					key.Empty();
					key.Format("CAM%d_iSmallSubRgnPointIdx%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iSmallSubRgnPointIdx[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);

					for(k = 0; k < SR_SUBREGION_POINT_MAX_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iSmallSubRgnX%d_Points%d", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_iSmallSubRgnX[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_iSmallSubRgnY%d_Points%d", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_iSmallSubRgnY[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}

				
				//Chip//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_CHIP; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Chip%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionChip[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Chip%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Chip%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Chip%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Chip%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapChip[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				/////////////////////////////////////
				
				//Lead//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_LEAD; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Lead%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionLead[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Lead%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Lead%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Lead%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Lead%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapLead[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				/////////////////////////////////////
				
				//Film//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_FILM; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_Film%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionFilm[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Film%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Film%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Film%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_Film%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapFilm[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
				/*
				//write variation region
				if(m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j])
				{
				pat_path = path + "\\VarFilm."; 
				extention.Format("v%d_%d", i, j);
				pat_path = pat_path + extention;
				set_check("~give_error");
				Herr = write_variation_model(m_VDat[i]->m_InsPara.m_FilmVarID[j], HTuple(((LPCTSTR)pat_path)));
				set_check("give_error");
				if(H_MSG_TRUE == Herr)
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = TRUE;	
				}
				else
				{
				m_VDat[i]->m_InsPara.m_bCreatFilmVarID[j] = FALSE;
				}
				}
					*/
				}
				/////////////////////////////////////
				
				//OCV////////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_OCV; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_OCV%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionOCV[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_OCV%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_OCV%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_OCV%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_OCV%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapOCV[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < OCV_CHAR_MAX_NUM; j++)
				{
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Left", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].left);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Top", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].top);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Right", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].right);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_OCV%d_InspectRect1_Bottom", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsCharRegionOCV[j].bottom);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_CenterX_OCV%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dCharCenterX[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Char_CenterY_OCV%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dCharCenterY[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					//write shape region
					if(m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j])
					{
						pat_path = path + "\\PatternOCV."; 
						extention.Format("m%d_%d", i, j);
						pat_path = pat_path + extention;
						set_check("~give_error");
						Herr = write_shape_model(m_VDat[i]->m_InsPara.m_PatIDOCV[j], HTuple(((LPCTSTR)pat_path)));
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = TRUE;	
						}
						else
						{
							m_VDat[i]->m_InsPara.m_bCreatMdIDOCV[j] = FALSE;
						}
					}
					
					//write variation region
					//if(m_VDat[i]->m_InsPara.m_bCreatCharVarID[j])
					//{
					//	pat_path = path + "\\VarOCV."; 
					//	extention.Format("v%d_%d", i, j);
					//	pat_path = pat_path + extention;
					//	set_check("~give_error");
					//	Herr = write_variation_model(m_VDat[i]->m_InsPara.m_CharVarID[j], HTuple(((LPCTSTR)pat_path)));
					//	set_check("give_error");
					//	if(H_MSG_TRUE == Herr)
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = TRUE;	
					//	}
					//	else
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatCharVarID[j] = FALSE;
					//	}
					//}
				}
				
				/////////////////////////////////////
				
				//SR//////////////////////////////
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < RECT_PARA_MAX_NUM_SR; k++)
					{
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Left", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].left);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Top", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].top);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Right", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].right);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
						
						key.Empty();
						key.Format("CAM%d_Ins_SR%d_%d_InspectRect1_Bottom", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionSR[j][k].bottom);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_SR%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_SR%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_SR%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_SR; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_SR%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapSR[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					//write variation region
					//if(m_VDat[i]->m_InsPara.m_bCreatSRVarID[j])
					//{
					//	pat_path = path + "\\VarSR."; 
					//	extention.Format("v%d_%d", i, j);
					//	pat_path = pat_path + extention;
					//	set_check("~give_error");
					//	Herr = write_variation_model(m_VDat[i]->m_InsPara.m_SRVarID[j], HTuple(((LPCTSTR)pat_path)));
					//	set_check("give_error");
					//	if(H_MSG_TRUE == Herr)
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = TRUE;	
					//	}
					//	else
					//	{
					//		m_VDat[i]->m_InsPara.m_bCreatSRVarID[j] = FALSE;
					//	}
					//}
				}
				/////////////////////////////////////
				
				switch(i)
				{
				case 0:
					//Write master image
					master_path = path + "\\MasterImage.bmp"; 
					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					
					//Write SR master image
					master_path = path + "\\SRMasterImage.bmp"; 
					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalSRMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					
					//Write Lead master image
					master_path = path + "\\LeadMasterImage.bmp"; 
					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalLeadMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					
					//Write SROut master image
					master_path = path + "\\SROutMasterImage.bmp"; 
					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalSROutMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");

					//Write Chip master image
					master_path = path + "\\ChipMasterImage.bmp"; 
					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalChipMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					
					//Write Taught image			
					for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
					{
						master_path = path + "\\TaughtImage"; 
						extention.Format("%d.bmp", j);
						master_path = master_path + extention;
						set_check("~give_error");
						Herr= write_image(m_VDat[i]->m_HalTaughtImage[j], "bmp", 0, HTuple((LPCTSTR)master_path));
						set_check("give_error");
					}
					break;
				case 1:
					//Write master image
					master_path = path + "\\MasterImage_bottom.bmp"; 
					set_check("~give_error");
					Herr= write_image(m_VDat[i]->m_HalMasterImage, "bmp", 0, HTuple((LPCTSTR)master_path));
					set_check("give_error");
					break;
				}
				
				//Write inspection region
				for(j = 0; j < SUB_RGN_MAX_NUM; j++)
				{
					for(k = 0; k < INS_RGN_MAX_NUM; k++)
					{
						pat_path = path + "\\InsRegion_"; 
						extention.Format("%d_%d_%d", i, j, k);
						pat_path = pat_path + extention;
						set_check("~give_error");//IMSI LSS
						Herr = write_region(m_VDat[i]->m_HalInsRgn[j][k], HTuple((LPCTSTR)pat_path));
						set_check("give_error");//IMSI LSS
					}
				}
				
				//Light
				for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
				{
					for(k = 0; k < LIGHT_CH_MAX_NUM; k++)
					{
						key.Empty();
						key.Format("CAM%d_iPara_Map%d_Light%d", i, j, k);
						dat.Empty();
						dat.Format("%d", m_VDat[i]->m_InsPara.m_iLightVar[j][k]);
						WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					}
				}
				//20100702
				//PPunch/////////////////////////////
				for(j = 0; j < RECT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Left", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].left);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Top", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].top);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Right", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].right);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
					
					key.Empty();
					key.Format("CAM%d_Ins_PPUNCH%d_InspectRect1_Bottom", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_InsRegionPPunch[j].bottom);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				
				for(j = 0; j < BOOL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_bParaPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < DBL_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%f", m_VDat[i]->m_InsPara.m_dParaPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				
				for(j = 0; j < INT_PARA_MAP_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iParaMap_PPUNCH%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_InsPara.m_iParaMapPPunch[j]);
					WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, file_path);
				}
				break;				
			}
		}
		
	}
	fd=0;
	
	return rslt;
}

//Load/save system parameter
BOOL C_VisionModule::LoadSystemPara(CString path)
{
	char temp[DAT_STR_LENG];
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;

	path = path + "\\System.par";
	//file 존재 확인 
	HANDLE fd = CreateFile( path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		//Read INI
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			
			key.Empty();
			key.Format("PC_INDEX_CH%d", i);
			m_VDat[i]->m_SysPara.m_iPCIdx = GetPrivateProfileInt("SYSTEM_PARAMETER", key, 0, path);

			for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_bPara_Sys%d", i, j);
				m_VDat[i]->m_SysPara.m_bPara[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, path);
			}

			for(j = 0; j < INT_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_iPara_Sys%d", i, j);
				m_VDat[i]->m_SysPara.m_iPara[j] = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, path);
			}

			for(j = 0; j < DBL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_dPara_Sys%d", i, j);
				GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), path);
				m_VDat[i]->m_SysPara.m_dPara[j] = atof(temp);
			}
		}
		m_dPixelSizeData = m_VDat[0]->m_SysPara.m_dPara[0]; //CHOKI20111117
	}
	fd=0;

	return rslt;
}

BOOL  C_VisionModule::SaveSystemPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;

	//Make director
	MakeDirectories(path);
	path = path + "\\System.par";
	//file 존재 확인 
	HANDLE fd=CreateFile(   path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		//Write INI
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			key.Empty();
			key.Format("PC_INDEX_CH%d", i);
			dat.Empty();
			dat.Format("%d", m_VDat[i]->m_SysPara.m_iPCIdx);
			WritePrivateProfileString("SYSTEM_PARAMETER", key, dat, path);

			for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_bPara_Sys%d", i, j);
				dat.Empty();
				dat.Format("%d", m_VDat[i]->m_SysPara.m_bPara[j]);
				WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, path);
			}

			for(j = 0; j < INT_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_iPara_Sys%d", i, j);
				dat.Empty();
				dat.Format("%d", m_VDat[i]->m_SysPara.m_iPara[j]);
				WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, path);
			}

			for(j = 0; j < DBL_PARA_MAX_NUM_SYS; j++)
			{
				key.Empty();
				key.Format("CAM%d_dPara_Sys%d", i, j);
				dat.Empty();
				dat.Format("%f", m_VDat[i]->m_SysPara.m_dPara[j]);
				WritePrivateProfileString("INSPECTION_PARAMETER", key, dat, path);
			}
		}

	}
	fd=0;

	return rslt;
}

//Load/save system result
BOOL C_VisionModule::LoadSystemRslt(CString path)
{
	BOOL rslt = TRUE;
	CString key;

	path = path + "\\MainSystem.rsl";
	//file 존재 확인 
	HANDLE fd = CreateFile( path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(int i = 0; i < CNT_MAX_NUM; i++)
		{
			key.Format("CNT%d", i);
			m_MainSysRslt.m_lCnt[i] = GetPrivateProfileInt("MAINSYSTEM_RESULT", key, 0, path);
		}
	}
	fd=0;

	return rslt;
}

BOOL C_VisionModule::SaveSystemRslt(CString path)
{
	BOOL rslt = TRUE;
	CString dat, key;

	//Make director
	MakeDirectories(path);
	path = path + "\\MainSystem.rsl";
	//file 존재 확인 
	HANDLE fd=CreateFile(   path,
							GENERIC_READ|GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);

		for(int i = 0; i < CNT_MAX_NUM; i++)
		{
			dat.Empty();
			dat.Format("%d", m_MainSysRslt.m_lCnt[i]);
			key.Format("CNT%d", i);
			WritePrivateProfileString("MAINSYSTEM_RESULT", key, dat, path);
		}

	}
	fd=0;

	return rslt;
}

//Inspection
int C_VisionModule::Inspection(int cam_idx, int buf_idx)
{
	int tot_rslt = 1;
	int rslt = 1;
	int i;
	CRect rect,rect_chip;
	BOOL punch_err;
	Hlong area;

	//Variable clear
	ResetInspectRslt(cam_idx);
	ResetInspectRsltRgn(cam_idx);
	
	int pf_pitch;
	
	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;		// 최종결과. 검사 진행 중 NG가 한 개이상 발생하면 0. 초기화

	BOOL lead_scat_hori, find_chip_use;
	lead_scat_hori = FALSE;
	find_chip_use = FALSE;

	//디버그 필요
	switch(GetVisIndex())
	{
	case VISION_CAM_TOP1:
		lead_scat_hori = TRUE;//수평방향 스캔(IC가로 긴방향기준)
		find_chip_use = TRUE;
		break;
	case VISION_CAM_TOP2:
	case VISION_CAM_BOTTOM:
		break;
	default:
		AfxMessageBox("Error : invalid index(C_VisionModule::Inspection)!)");
		break;
	}

	//Check punching chip
	//Punching chip
	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if( VISION_CAM_TOP1 == GetVisIndex() )	// 상부 1번 카메라 경우만 펀칭칩 수행
		{
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;
			
			//chip 중심 기준으로 편차를 찾기 위해 검사 위치 넘겨줌. 
			rect_chip.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
			rect_chip.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
			rect_chip.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
			rect_chip.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;
			
			pf_pitch = (int)((m_VDat[cam_idx]->m_InsPara.m_dParaBase[1] * 1000.) / m_VDat[cam_idx]->m_SysPara.m_dPara[0]);
			
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 0;

			//Punching chip
			if( FindPuchingChip(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[4]),           //펀칭 막힘  
									m_VDat[cam_idx]->m_HalInsRgn[i][0],																//SR region
									rect,
									rect_chip,
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[19],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[9],
									pf_pitch,
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[13],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[14],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[12],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[15],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[20],    
									m_VDat[cam_idx]->m_InsPara.m_iParaBase[2], 
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[25],
									m_VDat[cam_idx]->m_InsPara.m_iParaFilm[24],
									&punch_err, &m_VDat[cam_idx]->m_HalRsltRgn[i][19], &area))
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 2;             //Punching Chip 이면 ...
			}

			//Punching chip이라고 할지라도 PF막히면 불량(편집불량)
			if(m_VDat[cam_idx]->m_SysPara.m_bPara[12])
			{
				if(punch_err)
				{
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					//편집 불량 ...Punch가 있을 수도 있고 없을 수도 있다. 
					//단 Punch가 안보이더라도 PF가 막히므로 여기서 처리 가능함. 예외 가능성 있음.
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 3;  
					//gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][19], rect.top, rect.left, rect.bottom, rect.right);//20100413 lee
				}
			}
		}
		else
		{
			//20141120 lby
			if(TRUE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i]) //검사대상이 펀칭칩이라면 이하 모든 검사 스킵
			{	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 2;             //Punching Chip 이면 ...
			}
		}
	}

	//Align 펀치칩 유무 상관없이 수행
	if( m_VDat[cam_idx]->m_SysPara.m_bPara[0] ||	// Chip out
	  m_VDat[cam_idx]->m_SysPara.m_bPara[2] ||	// Chip이면수지/BROKEN
	  m_VDat[cam_idx]->m_SysPara.m_bPara[3] ||	// CHIP 이중 POTTING/미충진/POT PARTICLE/도포범위불량
	  m_VDat[cam_idx]->m_SysPara.m_bPara[10] ||	// FILM 수지/잉크 묻음/긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[11] || // FILM PF 찢김
	  m_VDat[cam_idx]->m_SysPara.m_bPara[12] ||	// PF 막힘/BURR(편집불량)
	  m_VDat[cam_idx]->m_SysPara.m_bPara[13] ||	// SR면 금속성 이물
	  m_VDat[cam_idx]->m_SysPara.m_bPara[20] || // Our lead 긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[21] ||	// Out lead 변색
	  m_VDat[cam_idx]->m_SysPara.m_bPara[22] ||	// In 긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[23] ||	// In 변색
	  m_VDat[cam_idx]->m_SysPara.m_bPara[24] ||	// OUT LEAD2
	  m_VDat[cam_idx]->m_SysPara.m_bPara[25] ||	// 인쇄불량
	  m_VDat[cam_idx]->m_SysPara.m_bPara[27] ||	// SR 찍힘/구겨짐 (백라이트 사용)
	  m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||	// ALIGN / 기포 / 잉크
	  m_VDat[cam_idx]->m_SysPara.m_bPara[29] )	// SR 찍힘/구겨짐
	{
		rslt = InspectionAlign(cam_idx, buf_idx);
	}
	else
	{
			rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}
	
	//Chip
	if((m_VDat[cam_idx]->m_SysPara.m_bPara[0] ||	// Chip out
	  m_VDat[cam_idx]->m_SysPara.m_bPara[2] ||	// Chip이면수지/BROKEN
	  m_VDat[cam_idx]->m_SysPara.m_bPara[3] ))	// CHIP 이중 POTTING/미충진/POT PARTICLE/도포범위불량)
	{
		rslt = InspectionChip(cam_idx, buf_idx, find_chip_use, m_VDat[cam_idx]->m_SysPara.m_bPara[0], m_VDat[cam_idx]->m_SysPara.m_bPara[2], m_VDat[cam_idx]->m_SysPara.m_bPara[3]);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//OCV
	if(m_VDat[cam_idx]->m_SysPara.m_bPara[25])
	{
		rslt = InspectionOCV(cam_idx, buf_idx, m_VDat[cam_idx]->m_SysPara.m_bPara[25]);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//Film
	if( m_VDat[cam_idx]->m_SysPara.m_bPara[10] ||	// FILM 수지/잉크 묻음/긁힘
	  m_VDat[cam_idx]->m_SysPara.m_bPara[11] ||	// FILM PF 막힘/FILM 찢김
	  m_VDat[cam_idx]->m_SysPara.m_bPara[13] )	// FILM SR면 금속성 이물
	{
		rslt = InspectionFilm(cam_idx, buf_idx, m_VDat[cam_idx]->m_SysPara.m_bPara[10], m_VDat[cam_idx]->m_SysPara.m_bPara[11], m_VDat[cam_idx]->m_SysPara.m_bPara[13]);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//20150129 by 하부를 별도로 나눌 필요가 없어짐.
	if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] ||	// SR 찍힘/구겨짐 (백라이트 사용)
	  m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||	// ALIGN / 기포 / 잉크
	  m_VDat[cam_idx]->m_SysPara.m_bPara[29])	// SR 찍힘/구겨짐
	{
		//count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////
		rslt = InspectionSR_Bottom(cam_idx, buf_idx);
		//count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
		//sprintf(temp,"[SRBub:%f]", (double)(HTuple(end_t - start_t)[0]) );
		//TRACE(temp);
	}
	else
	{
		rslt = 1;
	}
	///////////////////////////////////////////////////////////////

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//Lead
	if(m_VDat[cam_idx]->m_SysPara.m_bPara[15] ||   //AM0
	   m_VDat[cam_idx]->m_SysPara.m_bPara[16] ||   //AM1
	   m_VDat[cam_idx]->m_SysPara.m_bPara[20] ||   //Out긁힘
	   m_VDat[cam_idx]->m_SysPara.m_bPara[21] ||   //Out변색
	   m_VDat[cam_idx]->m_SysPara.m_bPara[22] ||   //In긁힘
	   m_VDat[cam_idx]->m_SysPara.m_bPara[23] ||   //Out변색
	   m_VDat[cam_idx]->m_SysPara.m_bPara[24])     //Out2
	{
		rslt = InspectionLead(cam_idx, buf_idx, 
			                  m_VDat[cam_idx]->m_SysPara.m_bPara[20], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[21], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[22], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[23], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[24],
							  m_VDat[cam_idx]->m_SysPara.m_bPara[15],
							  m_VDat[cam_idx]->m_SysPara.m_bPara[16], lead_scat_hori);
	}
	else
	{
		rslt = 1;
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	return tot_rslt;
}

//20141116 lby BOTTOM 추가
int C_VisionModule::Inspection_Bottom(int cam_idx, int buf_idx)
{
	int tot_rslt = 1;
	int rslt = 1;
	int i;
	CRect rect,rect_chip;
	HTuple start_t, end_t;

	//Variable clear
	ResetInspectRslt(cam_idx);
	ResetInspectRsltRgn(cam_idx);

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;		// 최종결과. 검사 진행 중 NG가 한 개이상 발생하면 0. 초기화

	
	//Align 펀치칩 유무 상관없이 수행
	if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] ||	// SR 찍힘/구겨짐 (백라이트 사용)
	  m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||	// ALIGN / 기포 / 잉크
	  m_VDat[cam_idx]->m_SysPara.m_bPara[29])	// SR 찍힘/구겨짐
	{
		rslt = InspectionAlign(cam_idx, buf_idx);
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}
	
	//20150129 이전. by
	/*
	//Align
		if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] ||	// SR 찍힘/구겨짐 (백라이트 사용)
		  m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||	// ALIGN / 기포 / 잉크
		  m_VDat[cam_idx]->m_SysPara.m_bPara[29])	// SR 찍힘/구겨짐
	{
		//count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////
		rslt = InspectionAlign_Bottom(cam_idx, buf_idx);
		//count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
		//sprintf(temp,"[Align:%f]", (double)(HTuple(end_t - start_t)[0]) );
		//TRACE(temp);
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}
	*/

	if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] ||	// SR 찍힘/구겨짐 (백라이트 사용)
		  m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||	// ALIGN / 기포 / 잉크
		  m_VDat[cam_idx]->m_SysPara.m_bPara[29])	// SR 찍힘/구겨짐
	{
		//count_seconds(&start_t);///////////////////////////////////////////////////////////////////////////////////////////
		rslt = InspectionSR_Bottom(cam_idx, buf_idx);
		//count_seconds(&end_t);///////////////////////////////////////////////////////////////////////////////////////////	
		//sprintf(temp,"[SRBub:%f]", (double)(HTuple(end_t - start_t)[0]) );
		//TRACE(temp);
	}

	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	//20150128 lby IMSI
	//Lead
	if(m_VDat[cam_idx]->m_SysPara.m_bPara[15] ||   //AM0
	   m_VDat[cam_idx]->m_SysPara.m_bPara[16] ||   //AM1
	   m_VDat[cam_idx]->m_SysPara.m_bPara[20] ||   //Out긁힘
	   m_VDat[cam_idx]->m_SysPara.m_bPara[21] ||   //Out변색
	   m_VDat[cam_idx]->m_SysPara.m_bPara[22] ||   //In긁힘
	   m_VDat[cam_idx]->m_SysPara.m_bPara[23] ||   //Out변색
	   m_VDat[cam_idx]->m_SysPara.m_bPara[24])     //Out2
	{
		rslt = InspectionLead(cam_idx, buf_idx, 
			                  m_VDat[cam_idx]->m_SysPara.m_bPara[20], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[21], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[22], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[23], 
							  m_VDat[cam_idx]->m_SysPara.m_bPara[24],
							  m_VDat[cam_idx]->m_SysPara.m_bPara[15],
							  m_VDat[cam_idx]->m_SysPara.m_bPara[16], FALSE);
	}
	
	if(1 != rslt)
	{
		tot_rslt = 0;
	}

	return tot_rslt;
}

//20141116 lby BOTTOM 추가
int C_VisionModule::InspectionAlign_Bottom(int cam_idx, int buf_idx)
{
	int rslt = 1;
	CRect rect, ins_rect, ins_rect2;
	BOOL bAlignCheck = TRUE;
		
	Hobject modimage, shape,Region, ImageReduced;;
	Hobject reudce_image;
	Hobject line;

	Hobject InsRgn;
	Hobject EmphaImage;
	Hobject InvertHallImage;//CHOKI20100713

	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double offset_x, offset_y;
	double pat_score;
	
	rn = 0;
	
	get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]), NULL, NULL, &wd, &ht);


	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;		// 최종결과. 검사 진행 중 NG가 한 개이상 발생하면 0. 초기화
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		bAlignCheck = TRUE;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 0;
	
		if(FALSE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i])
		{
			pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);

			m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;
			
			for(j = 0; j < PATTERN_INS_NUM; j++)
			{/*
				if(1 == j)
				{
					continue;
				}
				*/

				ins_rect = m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0];
				gen_rectangle1(&InsRgn, ins_rect.top, ins_rect.left, ins_rect.bottom, ins_rect.right);

				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][0], InsRgn, &m_VDat[cam_idx]->m_HalRsltRgn[i][0]);

				if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
				{
					//Inspection fail
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -11;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

					continue;
				}
				 
				//imsi 20141116
				reduce_domain(*GetHalRedImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&ImageReduced);

				emphasize(ImageReduced, &EmphaImage, 5, 5, 3); //20100715 lee
			
				Hal_ShapePatternFind(0, 
					&EmphaImage,  //20100715 lee
					m_VDat[cam_idx]->m_InsPara.m_PatIDAlign, 
					m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
					wd, ht,
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);
				if(rn)
				{
					gen_region_line(&line, (long)ry, (long)rx - 10, (long)ry, (long)rx + 10);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					gen_region_line(&line, (long)ry - 10, (long)rx, (long)ry + 10, (long)rx);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][2], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][2]);
					
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][j] = rx;
					m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][j] = ry;
					m_VDat[cam_idx]->m_InsRslt.m_dScoreAlign[i][j] = rscore;
					
					//Pos. limit check
					offset_x = rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][j];
					offset_x = fabs(offset_x);
					if( (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_x) )
					{
						//Limit error
						m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -12;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;	
						rslt = 0;
						bAlignCheck = FALSE;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
				}
				else
				{
					//Can't find
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -10;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					bAlignCheck = FALSE;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			//둘다 성공했으면
			if(TRUE == bAlignCheck)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;
				//첫번째 가지고 x, y offset을 결정한다.
				offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
				offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetX[i] = offset_x;
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetY[i] = offset_y;
			}
		}
		else
		{
			if(TRUE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i]) //검사대상이 펀칭칩이라면 이하 모든 검사 스킵
			{	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 2;             //Punching Chip 이면 ...
			}
		}
	}

	return rslt;	
}


//20141116 lby BOTTOM 추가
int C_VisionModule::InspectionSR_Bottom(int cam_idx, int buf_idx)
{
	int rslt = 1;
	
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hobject RefRedImage, RefGreenImage, RefBlueImage;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect rect, film_rect, chip_search_rect;

	Hlong under_area;
	
	rn = 0;
	
	get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), NULL, NULL, &wd, &ht);

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if(1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] )
		{
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 0;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltPIFold[i] = 0;

			//Find chip//////////////////////////////
			
			//Offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + offset_y;

			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][10], rect.top, rect.left, rect.bottom, rect.right);

			if( m_VDat[cam_idx]->m_SysPara.m_bPara[27] || //기포/잉크
				m_VDat[cam_idx]->m_SysPara.m_bPara[28] ||//SR검사/찍힘 (빽라이트)
				m_VDat[cam_idx]->m_SysPara.m_bPara[29] )//SR검사/찍힘
			{
					m_VDat[cam_idx]->m_InsRslt.m_iRsltPIFold[i] = -61;


					rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].left;
					rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].top;
					rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].right;
					rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].bottom;

					film_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
					film_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
					film_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
					film_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;

					//20100716 lee
					chip_search_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
					chip_search_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
					chip_search_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
					chip_search_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;


					decompose3(*GetHalTaughtImage(0,0), &RefRedImage, &RefGreenImage, &RefBlueImage);

					if(_Hal_FilmFold(0, GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[1]),
						                GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[2]),
														m_VDat[cam_idx]->m_HalInsRgn[i][0],
						      						    film_rect, chip_search_rect,
							  
														m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[0][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[0][0], 
														m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[0][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[0][1], 

														m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
														m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
														m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
														m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],

														m_VDat[cam_idx]->m_InsPara.m_iParaSR[6], m_VDat[cam_idx]->m_InsPara.m_iParaSR[5],
														m_VDat[cam_idx]->m_InsPara.m_iParaSR[8], m_VDat[cam_idx]->m_InsPara.m_iParaSR[7], 
							  
													    m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnIdx,
														m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnX,
														m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnY,
														m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnType,

														&m_VDat[cam_idx]->m_InsRslt.m_iPIFoldNum[i], 
														&m_VDat[cam_idx]->m_HalRsltRgn[i][51],
														&m_VDat[cam_idx]->m_HalRsltRgn[i][50],
														&m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i], 
														&m_VDat[cam_idx]->m_HalRsltRgn[i][53],
														&m_VDat[cam_idx]->m_HalRsltRgn[i][52]) )
					//20150317 by
					/*
					if( SRBub(0, GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[0]), 
								 GetHalBlueImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[1]), 
								 GetHalGreenImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[0]),
								 &RefGreenImage, &RefBlueImage, //CHOKI20110717 기준이미지와 비교하기 위해 RefBlueImage추가
								 GetHalRedImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapSR[0]),
						rect, 
						film_rect,
						chip_search_rect,
						m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
						m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[2], m_VDat[cam_idx]->m_InsPara.m_iParaSR[2] * 0.4, 
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[0], m_VDat[cam_idx]->m_InsPara.m_iParaSR[1], 
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[4], 
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[3], 
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[6], 
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[5], 
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[8], 
						m_VDat[cam_idx]->m_InsPara.m_iParaSR[7], m_VDat[cam_idx]->m_InsPara.m_iParaSR[9],  
						&m_VDat[cam_idx]->m_InsRslt.m_iPIFoldNum[i], 
						&m_VDat[cam_idx]->m_HalRsltRgn[i][51],
						&m_VDat[cam_idx]->m_HalRsltRgn[i][50],
						&m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i], 
						&m_VDat[cam_idx]->m_HalRsltRgn[i][53],
						&m_VDat[cam_idx]->m_HalRsltRgn[i][52]) )
						*/
					{
						//OK
						m_VDat[cam_idx]->m_InsRslt.m_iRsltPIFold[i] = 1;
					}
					else
					{
						//SR bubble
						if( (m_VDat[cam_idx]->m_SysPara.m_bPara[28]) && (0 < m_VDat[cam_idx]->m_InsRslt.m_iPIFoldNum[i]) )
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltPIFold[i] = -61;
						}
						else
						{
							m_VDat[cam_idx]->m_InsRslt.m_iPIFoldNum[i] = 0;
							//gen_empty_obj(&m_VDat[i]->m_HalRsltRgn[i][51]);
						}

						//PI 찍힘
						if( (m_VDat[cam_idx]->m_SysPara.m_bPara[29]) && (0 < m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i]) )
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltPIFold[i] = -71;
						}
						else
						{
							m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i] = 0;
							//gen_empty_obj(&m_VDat[i]->m_HalRsltRgn[i][53]);
						}

						if( (0 < m_VDat[cam_idx]->m_InsRslt.m_iPIFoldNum[i]) || (0 < m_VDat[cam_idx]->m_InsRslt.m_iPIHallNum[i]) )
						{
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
						}
						else
						{
							//OK
							m_VDat[cam_idx]->m_InsRslt.m_iRsltPIFold[i] = 1;
						}
					}

			}
			else
			{
				//OK
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 1;
			}
		}
	}
	
	return rslt;
}


//20100702 lss
//펀칭부 검사
BOOL C_VisionModule::InspectionPPunch(long win , Hobject* rslt_rgn)
{
	BOOL rslt = TRUE;
	CRect rect;
	int dat_cam_idx, cam_idx;

	//주의 : 두번째 카메라 지만 첫번째 데이타를 가지고 검사합니다.(두번째 데이타는 사용하지 않습니다)
	dat_cam_idx = 0;

	//주의 : 이미지는 두번째 카메라를 사용합니다.
	cam_idx = 1;

	rect.left = 10;
	rect.top = 10;
	rect.right = 630;
	rect.bottom = 470;
	

	if( FindPPuchingChip(win, GetHalRedImage(cam_idx, 2, 0), rect, 
		m_VDat[dat_cam_idx]->m_InsPara.m_iParaPPunch[0],
		m_VDat[dat_cam_idx]->m_InsPara.m_iParaPPunch[1],
		m_VDat[dat_cam_idx]->m_InsPara.m_iParaPPunch[2],
		rslt_rgn) )
	{
		rslt = TRUE;
	}
	else
	{
		rslt = FALSE;
	}
	
	return rslt;
}

//펀칭부 칩 위치 검사
BOOL C_VisionModule::InspectionPPunchPos(long win , Hobject* rslt_rgn)
{
	BOOL rslt = TRUE;
	CRect rect;
	int dat_cam_idx, cam_idx;

	//주의 : 두번째 카메라 지만 첫번째 데이타를 가지고 검사합니다.(두번째 데이타는 사용하지 않습니다)
	dat_cam_idx = 0;

	//주의 : 이미지는 두번째 카메라를 사용합니다.
	cam_idx = 1;

	rect.left = 240;
	rect.top = 40;
	rect.right = 380;
	rect.bottom = 440;
	
	
	if( FindPPuchingChipPos(win, GetHalRedImage(cam_idx, 2, 0), rect, 
		rslt_rgn) )
	{
		rslt = TRUE;
	}
	else
	{
		rslt = FALSE;
	}
	
	return rslt;
}

//CHOKI20100712
BOOL C_VisionModule::InspectionPunchCheck(int buf_idx)
{
	int tot_rslt = 1;
	int rslt = 1;
	int i=0;
	CRect rect;

	//	//Check error
	//	if(m_VDat[cam_idx]->m_SysPara.m_bPara[32])
	//	{
	//		write_image(*GetHalImage(0, 0, 0), "bmp", 0, HTuple("d:\\error0.bmp"));
	//		write_image(*GetHalImage(0, 0, 1), "bmp", 0, HTuple("d:\\error1.bmp"));
	//		write_image(*GetHalImage(0, 0, 2), "bmp", 0, HTuple("d:\\error2.bmp"));
	//		write_image(*GetHalImage(0, 0, 3), "bmp", 0, HTuple("d:\\error3.bmp"));
	//		write_image(*GetHalImage(0, 0, 4), "bmp", 0, HTuple("d:\\error4.bmp"));
	//		write_image(*GetHalImage(0, 0, 5), "bmp", 0, HTuple("d:\\error5.bmp"));
	//		write_image(*GetHalImage(0, 0, 6), "bmp", 0, HTuple("d:\\error6.bmp"));
	//		//write_image(*GetHalImage(cam_idx, buf_idx, 7), "bmp", 0, HTuple("error7.bmp"));
	//	}
	
	//Check punching chip
	//20100111LSS
	//Punching chip
    /*	 
    //20110731 lee
	rect.left = m_VDat[0]->m_InsPara.m_InsRegionFilm[buf_idx][0].left;
	rect.top = m_VDat[0]->m_InsPara.m_InsRegionFilm[buf_idx][0].top;
	rect.right = m_VDat[0]->m_InsPara.m_InsRegionFilm[buf_idx][0].right;
	rect.bottom = m_VDat[0]->m_InsPara.m_InsRegionFilm[buf_idx][0].bottom;
    */
	rect.left = m_VDat[0]->m_InsPara.m_InsRegionFilm[0][0].left;
	rect.top = m_VDat[0]->m_InsPara.m_InsRegionFilm[0][0].top;
	rect.right = m_VDat[0]->m_InsPara.m_InsRegionFilm[0][0].right;
	rect.bottom = m_VDat[0]->m_InsPara.m_InsRegionFilm[0][0].bottom;
	//Punching chip

	if( CheckPuchingChipAuto(0, GetHalImage(0, buf_idx, m_VDat[0]->m_InsPara.m_iParaMapFilm[4]), rect, 
						m_VDat[0]->m_InsPara.m_iParaFilm[13],
						m_VDat[0]->m_InsPara.m_iParaFilm[14],
						m_VDat[0]->m_InsPara.m_iParaFilm[12],
						&m_VDat[0]->m_HalRsltRgn[i][19]))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int C_VisionModule::InspectionFirstAlign(int cam_idx, int buf_idx)
{
	int rslt = 1;
	CRect rect, ins_rect, ins_rect2;
		
	Hobject modimage, shape,Region, ImageReduced;;
	Hobject reudce_image;
	Hobject line;

	Hobject InsRgn;
	Hobject EmphaImage;
	Hobject InvertHallImage;//CHOKI20100713

	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double offset_x, offset_y;
	double pat_score;

	offset_x = 0.0;
	offset_y = 0.0;
	pat_score = 0.0;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_27!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}


	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
		rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
		rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
		rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;
		
		
		pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);
		
		m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 1;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;
		
		for(j = 0; j < PATTERN_INS_NUM; j++)
		{
		
			//대칭으로 만들어 2번째 align 검사
			// 20110530 Dongwhe* BEGIN
			// 반대쪽 Align Mark도 따로 지정해주도록 변경
			// -> 대칭으로 만들어내지 않음
			
			ins_rect = m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][(j==0?0:2)];
			gen_rectangle1(&InsRgn, ins_rect.top, ins_rect.left, ins_rect.bottom, ins_rect.right);
			
			concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][0], InsRgn, &m_VDat[cam_idx]->m_HalRsltRgn[i][0]);
			
			if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
			{
				//Inspection fail
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -11;
				m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
				rslt = 0;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				
				continue;
			}
			//20100715 lee
			reduce_domain(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&ImageReduced);
			emphasize(ImageReduced, &EmphaImage, 5, 5, 3); //20100715 lee
			
			//reduce_domain(EmphaImage,InsRgn,&reudce_image);
			
			Hal_ShapePatternFind(0, 
				&EmphaImage,  //20100715 lee
				(j==0?m_VDat[cam_idx]->m_InsPara.m_PatIDAlign:m_VDat[cam_idx]->m_InsPara.m_PatIDAlign2), 
				m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
				//					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
				m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][(j==0?1:3)],//model 영역(고정)
				wd, ht,
				//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
				//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
				m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?0:2)],//pattern cx 
				m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?1:3)],//pattern cy
				pat_score,//Score
				&modimage, 
				&shape, 
				&rx, 
				&ry, 
				&ra, 
				&rs, 
				&rscore, 
				&rn);
			// 20110530 Dongwhe* END
			if(rn)
			{
				gen_region_line(&line, (Hlong)ry, (Hlong)rx - 10, (Hlong)ry, (Hlong)rx + 10);
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
				gen_region_line(&line, (Hlong)ry - 10, (Hlong)rx, (Hlong)ry + 10, (Hlong)rx);
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
				
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][2], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][2]);
				
				m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][j] = rx;
				m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][j] = ry;
				m_VDat[cam_idx]->m_InsRslt.m_dScoreAlign[i][j] = rscore;
				
				//Pos. limit check
				offset_x = rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][j];
				offset_x = fabs(offset_x);
				if( (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_x) )
				{
					//Limit error
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -12;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;	
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
			else
			{
				//Can't find
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -10;
				m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
				rslt = 0;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
			}
		}
		
		//둘다 성공했으면
		if(1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i])
		{
			//첫번째 가지고 x, y offset을 결정한다.
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			m_VDat[cam_idx]->m_InsRslt.m_dOffsetX[i] = offset_x;
			m_VDat[cam_idx]->m_InsRslt.m_dOffsetY[i] = offset_y;
		}

	}
	

	return rslt;
	
}


int C_VisionModule::InspectionAlign(int cam_idx, int buf_idx)
{
	int rslt = 1;
	CRect rect, ins_rect, ins_rect2;
	BOOL bAlignCheck = TRUE;
		
	Hobject modimage, shape,Region, ImageReduced;;
	Hobject reudce_image;
	Hobject line;

	Hobject InsRgn;
	Hobject EmphaImage;
	Hobject InvertHallImage;//CHOKI20100713

	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double offset_x, offset_y;
	double pat_score;

	offset_x = 0.0;
	offset_y = 0.0;
	pat_score = 0.0;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_27!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}


	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		bAlignCheck = TRUE;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 0;
		
		if(FALSE == m_VDat[cam_idx]->m_InsPara.m_bIsPunchSkip[i])
		{
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;
			
			pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);
			
			for(j = 0; j < PATTERN_INS_NUM; j++)
			{
				ins_rect = m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][(j==0?0:2)];
				gen_rectangle1(&InsRgn, ins_rect.top, ins_rect.left, ins_rect.bottom, ins_rect.right);
				
				concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][0], InsRgn, &m_VDat[cam_idx]->m_HalRsltRgn[i][0]);
				
				if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
				{
					//Inspection fail
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -11;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					
					continue;
				}
				//20100715 lee
				reduce_domain(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&ImageReduced);
				emphasize(ImageReduced, &EmphaImage, 5, 5, 3); //20100715 lee
				
				//reduce_domain(EmphaImage,InsRgn,&reudce_image);
				
				Hal_ShapePatternFind(0, 
					&EmphaImage,  //20100715 lee
					(j==0?m_VDat[cam_idx]->m_InsPara.m_PatIDAlign:m_VDat[cam_idx]->m_InsPara.m_PatIDAlign2), 
					m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
					//					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
					m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][(j==0?1:3)],//model 영역(고정)
					wd, ht,
					//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
					//					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?0:2)],//pattern cx 
					m_VDat[cam_idx]->m_InsPara.m_dParaAlign[(j==0?1:3)],//pattern cy
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);
				// 20110530 Dongwhe* END
				if(rn)
				{
					gen_region_line(&line, (Hlong)ry, (Hlong)rx - 10, (Hlong)ry, (Hlong)rx + 10);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					gen_region_line(&line, (Hlong)ry - 10, (Hlong)rx, (Hlong)ry + 10, (Hlong)rx);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], line, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][1], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][1]);
					
					concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][2], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][2]);
					
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][j] = rx;
					m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][j] = ry;
					m_VDat[cam_idx]->m_InsRslt.m_dScoreAlign[i][j] = rscore;
					
					//Pos. limit check
					offset_x = rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][j];
					offset_x = fabs(offset_x);
					offset_x *= m_VDat[cam_idx]->m_SysPara.m_dPara[0];

					offset_y = ry - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][j];
					offset_y = fabs(offset_y);
					offset_y *= m_VDat[cam_idx]->m_SysPara.m_dPara[0];

					if( (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_x) || (m_VDat[cam_idx]->m_InsPara.m_iParaAlign[2] < offset_y) )
					{
						//Limit error
						m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -12;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;	
						rslt = 0;
						bAlignCheck = FALSE;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
				}
				else
				{
					//Can't find
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = -10;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					rslt = 0;
					bAlignCheck = FALSE;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
		
			//둘다 성공했으면
			if(1 == bAlignCheck)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i] = 1;

				//첫번째 가지고 x, y offset을 결정한다.
				offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
				offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetX[i] = offset_x;
				m_VDat[cam_idx]->m_InsRslt.m_dOffsetY[i] = offset_y;
			}
		}
	}

	return rslt;	
}

int C_VisionModule::InspectionAlignForAutoOCVTeach(int cam_idx, int buf_idx, Hobject *mod_image, Hobject *shape, 
												   double *rx, double *ry, double *rscore, Hlong *rn,
												   double *offset_x, double *offset_y)
{
	int rslt = 1;

	CRect rect,rect_chip;
		
	Hobject reudce_image, InsRgn;  //20100715 lee
	Hobject line;

	Hlong wd, ht;
	double ra, rs;   
	int i;
	double pat_score;

	//BOOL punch_rslt;

	Hobject EmphaImage;
	
	*rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_28!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}
	
	for(i = 0; i < 1; i++)//첫번째것만
	{
			gen_rectangle1(&InsRgn, m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].top, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].left, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].bottom, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[i][0].right);
		
		if(!m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDAlign)
		{
			//Inspection fail
			rslt = 0;
		}

		reduce_domain(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapAlign[0]),InsRgn,&reudce_image); //20100715 lee
		emphasize(reudce_image, &EmphaImage, 5, 5, 3); //20100715 lee

		pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaAlign[1]) * 0.01);
		
		Hal_ShapePatternFind(0, 
			&EmphaImage,  //20100715 lee
			m_VDat[cam_idx]->m_InsPara.m_PatIDAlign, 
			m_VDat[cam_idx]->m_InsPara.m_PatParaAlign, 
			m_VDat[cam_idx]->m_InsPara.m_InsRegionAlign[0][1],//model 영역(고정)
			wd, ht,
			m_VDat[cam_idx]->m_InsPara.m_dParaAlign[0],//pattern cx 
			m_VDat[cam_idx]->m_InsPara.m_dParaAlign[1],//pattern cy
			pat_score,//Score
			mod_image, 
			shape, 
			rx, 
			ry, 
			&ra, 
			&rs, 
			rscore, 
			rn);
		if(*rn)
		{
			rslt = 1;
			
			*offset_x = *rx - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			*offset_y = *ry - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];

			//Check punching chip
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left + *offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top + *offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right + *offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right + *offset_y; //20100715 lee

			//chip 중심에서 편차를 구하기 위해 ..
			rect_chip.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + *offset_x;
			rect_chip.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + *offset_y;
			rect_chip.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + *offset_x;
			rect_chip.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + *offset_y;
		}
		else
		{
			rslt = 0;
		}
	}
	
	return rslt;
}

int C_VisionModule::InspectionChip(int cam_idx, int buf_idx, BOOL find_chip_use, BOOL chip_out_use, BOOL chip_mold_use, BOOL chip_pot_use)
{
	int rslt = 1;
	
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hobject RefRedImage, RefGreenImage, RefBlueImage;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect rect, film_rect, chip_search_rect;

	Hlong over_area, under_area;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_29!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		if( 1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]&&		/*얼라인 성공 시*/ 
		  (2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i])&&	/*펀칭 칩이 아닐 때*/ 
		  (3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]))		/*편집불량이 아닐 때*/
		{
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 0;

			//Offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + offset_y;

			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][10], rect.top, rect.left, rect.bottom, rect.right);

			//20100111LSS
			//Find chip
			if(find_chip_use)
			{
				if(FindChip(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]),  
												 rect, 
												 &m_VDat[cam_idx]->m_HalRsltRgn[i][11]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = 1;
				}
				else
				{
					//No chip
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] = -20;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}
		
			/*
			m_map0 = 1;//Chip인식/포팅
			m_map1 = 2;//Chipout(IC표면 밝게)
			m_map2 = 3;//Broken/(IC표면의 어두운것이 잘보이게)
			m_map3 = 3;//칩이면결함(톱니 모양 불량, 밝은 IC면에서 어두운 불량) = broken 조건일수도
			m_map4 = 1;//Potting
			m_map5 = 1;//Potting위 밝은이물/도포불량
			m_map6 = 0;//Potting위 어두운이물
			*/
		
			//Chipout////////////////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = 0;

			if(/*1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] &&	*///칩을 찾았으면..	//20141211 상부#2에서는 수행하지 않음
				chip_out_use)											//검사 유무 설정
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = -21;

				//0 : out tol., 1 : out threshold, 2 : out high threshold
				//20110530 lee green에서 Chip Out이 가장 밝음. 
				if(ChipOut(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[1]), 
							  GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[1]), 
							  GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), 
							  GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[2]), 
							  GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[0]), 
								m_VDat[cam_idx]->m_HalRsltRgn[i][11], 
								m_VDat[cam_idx]->m_HalInsRgn[i][7],//chip region(셋업시 등록된 영역)
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
								m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
								m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[1], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[0], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[18], 
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[17],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[20],
								m_VDat[cam_idx]->m_InsPara.m_iParaChip[19],
								&m_VDat[cam_idx]->m_InsRslt.m_iChipOutNum[i], 
								&m_VDat[cam_idx]->m_HalRsltRgn[i][12]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = 1;
				}
				else
				{
					//Chipout
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipOut[i] = -21;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			//Chip 이면 결함/////////////////////
			//Broken & Mold//////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 0;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 0;

			if(/*1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] &&*/ //칩을 찾았으면..
							chip_mold_use)											//검사 유무 설정
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = -22;	
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = -23;

				//3 : mold tol., 4 : mold threshold, 5 : mold high threshold, 14 : broken threshold

				/*
				m_map0 = 1;//Chip인식/포팅
				m_map1 = 2;//Chipout(IC표면 밝게)
				m_map2 = 3;//Broken/(IC표면의 어두운것이 잘보이게)
				m_map3 = 1;//칩이면결함(톱니 모양 불량, 밝은 IC면에서 어두운 불량) = broken 조건일수도
				m_map4 = 1;//Potting
				m_map5 = 1;//Potting위 밝은이물/도포불량
				m_map6 = 0;//Potting위 어두운이물
				*/

				if( ChipMold(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[3]), 
								GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[2]),
															m_VDat[cam_idx]->m_HalInsRgn[i][7],//m_HalRsltRgn[i][11], //CHOKI20110427
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[5], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[3], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[14],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipMoldNum[i], 
															&m_VDat[cam_idx]->m_HalRsltRgn[i][14],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipBrokenNum[i], 
															&m_VDat[cam_idx]->m_HalRsltRgn[i][13])
 															)
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 1;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 1;
				}
				else
				{
					//Broken
					if(0 < m_VDat[cam_idx]->m_InsRslt.m_iChipBrokenNum[i])
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = -22;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipBroken[i] = 1;
					}

					//Mold
					if(0 < m_VDat[cam_idx]->m_InsRslt.m_iChipMoldNum[i])
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = -23;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipMold[i] = 1;
					}
				}
			}

			//Pot////////////////////////////////////	
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 0;//
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 0;
			
			if(	1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] && //칩을 찾았으면..
							chip_pot_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = -24;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = -25;

				over_area = m_VDat[cam_idx]->m_InsPara.m_iParaChip[12] * (m_VDat[cam_idx]->m_InsPara.m_iParaChip[6] * 0.01);
				under_area = m_VDat[cam_idx]->m_InsPara.m_iParaChip[12] * (m_VDat[cam_idx]->m_InsPara.m_iParaChip[8] * 0.01);

				//6 : over pot tol., 7 : pot threshold, 8 : no pot tol. 12 : pot area
				if(ChipPot(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[4]), 
															rect, //PotRect//CHOKI20100906
															(int)m_VDat[cam_idx]->m_InsPara.m_iParaChip[12],
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[7], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[8], 
															m_VDat[cam_idx]->m_InsPara.m_iParaChip[6], 
															&m_VDat[cam_idx]->m_InsRslt.m_iChipPotNum[i],
															&m_VDat[cam_idx]->m_InsRslt.m_iChipPotArea[i],
															&m_VDat[cam_idx]->m_HalRsltRgn[i][15]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 1;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 1;
				}
				else
				{
					if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
					{
						//NO POTTING
						if(under_area > m_VDat[cam_idx]->m_InsRslt.m_iChipPotArea[i])
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = -24;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
						}
						else
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 1;
						}
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipNPot[i] = 0;
					}
					
					if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
					{
						//DOUBLE POTTING
						if(over_area < m_VDat[cam_idx]->m_InsRslt.m_iChipPotArea[i])
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = -25;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
						}
						else
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 1;
						}
					}
					else
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipDPot[i] = 0;
					}
				}
			}

			//Chip 이면 결함/////////////////////
			//Pot particle///////////////////////
			
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 0;
			m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 0;
			
			if(TRUE == m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i] && // 칩을 찾았으면..
							chip_pot_use)								
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = -26;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = -27;

				//9 : pot part tol., 10 : pot part threshold, 11 : pot part high threshold
				//13 : pot 도포범위 tol.
				
				if(PotPart(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[7]), 
					GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[6]),
					GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapChip[7]),
					m_VDat[cam_idx]->m_HalRsltRgn[i][11],//chip region 
					m_VDat[cam_idx]->m_HalRsltRgn[i][15],//pot region
					m_VDat[cam_idx]->m_HalInsRgn[i][0],//SR region
					
					m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
					m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
					m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
					
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[10], 
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[15],
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[11], 
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[9], 
					m_VDat[cam_idx]->m_InsPara.m_iParaChip[13], 
					m_VDat[cam_idx]->m_InsPara.m_iParaFilm[1],//Film 수지 tol.
					m_VDat[cam_idx]->m_InsPara.m_iParaFilm[2],//Film 수지 thresh.
					&m_VDat[cam_idx]->m_InsRslt.m_iChipPotPNum[i], 
					&m_VDat[cam_idx]->m_HalRsltRgn[i][16],
					&m_VDat[cam_idx]->m_InsRslt.m_iChipPotRNum[i], 
					&m_VDat[cam_idx]->m_HalRsltRgn[i][17],	
					&m_VDat[cam_idx]->m_HalRsltRgn[i][18]//인식기 영역
					))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 1;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 1;
				}
				else
				{
					//Particle
					if( m_VDat[cam_idx]->m_SysPara.m_bPara[3] && (0 < m_VDat[cam_idx]->m_InsRslt.m_iChipPotPNum[i]) )
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = -26;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						 if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 1;
						 }
						 else
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotP[i] = 0;
						 }
						
					}

					//Pot range
					if( m_VDat[cam_idx]->m_SysPara.m_bPara[3] && (0 < m_VDat[cam_idx]->m_InsRslt.m_iChipPotRNum[i]) )
					{
						m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = -27;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
					else
					{
						if(m_VDat[cam_idx]->m_SysPara.m_bPara[3])
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 1;
						 }
						 else
						 {
							m_VDat[cam_idx]->m_InsRslt.m_iRsltChipPotR[i] = 0;
						 }
					}
				}
			}			
		}
	}
	
	return rslt;
}

int C_VisionModule::InspectionFilm(int cam_idx, int buf_idx, BOOL film_mold_use, BOOL film_pf_use, BOOL film_metal_use)
{
	int rslt = 1;
		
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect rect, chip_rect, ocv_rect, chip_rect_insp, ocv_rect_insp, dontcare_rect;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_30!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}

	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = 0;

		if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&			//얼라인이 성공 했을 때
		  (2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&		/*펀칭 칩이 아닐 때*/ 
		  (3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&		/*편집불량이 아닐 때*/
		  film_mold_use )//if not no chip and puching chip
		{
			m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = -31;

			//Y offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			//For display
			//Film-mold inspection region
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom + offset_y;
			
			//Chip inspection region
			chip_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left + offset_x;
			chip_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top + offset_y;
			chip_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right + offset_x;
			chip_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom + offset_y;

			//20100720 lee
			chip_rect_insp = chip_rect;

			//OCV inspection region
			ocv_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].left + offset_x;
			ocv_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].top + offset_y;
			ocv_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].right + offset_x;
			ocv_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].bottom + offset_y;
 
			//20150302 by
			dontcare_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].left + offset_x;
			dontcare_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].top + offset_y;
			dontcare_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].right + offset_x;
			dontcare_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionSR[i][0].bottom + offset_y;

			//20100720 lee
			ocv_rect_insp = ocv_rect;

			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][20], rect.top, rect.left, rect.bottom, rect.right);
			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][21], chip_rect.top, chip_rect.left, chip_rect.bottom, chip_rect.right);
			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][22], ocv_rect.top, ocv_rect.left, ocv_rect.bottom, ocv_rect.right);		

			//For inspection
			//Film-mold inspection region
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;
			
			//Chip inspection region
			chip_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].left;
			chip_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].top;
			chip_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].right;
			chip_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionChip[i][0].bottom;	

			 //20100720 lee  //영상 보정된 영역을 사용하기 위해 
			//OCV inspection region
			ocv_rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].left;
			ocv_rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].top;
			ocv_rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].right;
			ocv_rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].bottom;

			//0 : mold factor(mean_image), 1 : mold tol., 2 : mold thrsh.
			if(FilmMold(0,
					   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[0]),	//Mold image
					   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[2]),//Ink image
					   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[1]),//User image
					   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[3]),//SR-OUT Scratch image
					   GetHalTaughtImage(cam_idx, 2),   //Taught image(SR, 세번째 영상)
					   rect, chip_rect, ocv_rect, m_VDat[cam_idx]->m_HalRsltRgn[i][41],//OCV chars region
					   m_VDat[cam_idx]->m_HalInsRgn[i][0],//SR region
					   m_VDat[cam_idx]->m_HalInsRgn[i][4],//user region
					   m_VDat[cam_idx]->m_HalInsRgn[i][5],//sr out region
							m_VDat[cam_idx]->m_InsPara.m_FilmVarID[i], 

							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[0][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[0][0], 
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[0][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[0][1], 

							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],

							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[22], m_VDat[cam_idx]->m_InsPara.m_iParaFilm[2], 
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[23], m_VDat[cam_idx]->m_InsPara.m_iParaFilm[21], 
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[0], m_VDat[cam_idx]->m_InsPara.m_iParaFilm[1], 
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[6],
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[8],
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[7],
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[16],
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[17],
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[18],
							m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnIdx,
							m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnX,
							m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnY,
							m_VDat[cam_idx]->m_InsPara.m_iSmallSubRgnType,
							&m_VDat[cam_idx]->m_InsRslt.m_iFilmMoldNum[i],
							&m_VDat[cam_idx]->m_HalRsltRgn[i][23],
							&m_VDat[cam_idx]->m_HalRsltRgn[i][24],
							&m_VDat[cam_idx]->m_HalRsltRgn[i][27],
							&m_VDat[cam_idx]->m_HalRsltRgn[i][28],
							&m_VDat[cam_idx]->m_HalRsltRgn[i][29]) )
			{
				//OK
				m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = 1;
			}
			else
			{
				//Film mold
				m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMold[i] = -31;
				rslt = 0;
				m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
				m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
			}

			//PF hall////////////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = 0;

			if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&
				(2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&	/*펀칭 칩이 아닐 때*/ 
				(3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&		/*편집불량이 아닐 때*/
				film_pf_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = -32;

				//3 : pf tol., 4 : pf thresh., 5 : pf high thresh.
				if( FilmPF(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[4]), 
						    GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[5]),
							rect, chip_rect, dontcare_rect,
							m_VDat[cam_idx]->m_HalInsRgn[i][1],//PF region
							m_VDat[cam_idx]->m_HalInsRgn[i][6],//vaccum region

							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0],
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1],
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],							
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],

							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[4], 
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[3], 
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[19],  
							m_VDat[cam_idx]->m_InsPara.m_iParaFilm[9],  

							&m_VDat[cam_idx]->m_InsRslt.m_iFilmPFNum[i], 
							&m_VDat[cam_idx]->m_HalRsltRgn[i][25],   //막힘/찢어짐
							&m_VDat[cam_idx]->m_HalRsltRgn[i][26])  )//PF영역 
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = 1;
				}
				else
				{
					//Film mold
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmPF[i] = -32;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
			}

			//Metal particle/////////////////////
			m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMetal[i] = 0;

			if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&
			  (2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&	/*펀칭 칩이 아닐 때*/ 
			  (3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&		/*편집불량이 아닐 때*/
			  film_metal_use)
			{
				//m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMetal[i] = -33;
				//LSS
				/*
				m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMetal[i] = 1; //CHOKI20110427 GetHalImage -> GetHalRedImage
				if(FilmMetal(0, GetHalRedImage(cam_idx, buf_idx, 2), //CHOKKI20110427 m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[6]), 
					GetHalRefRedImage(cam_idx, 0), //CHOKI20110728
															m_VDat[cam_idx]->m_HalInsRgn[i][0],//SR region
				//											m_VDat[cam_idx]->m_HalInsRgn[i][7],//chip region(셋업시 등록된 영역)
				//											m_VDat[cam_idx]->m_HalRsltRgn[i][42],//OCV chars region
															chip_rect_insp, //20100720 lee
															ocv_rect_insp, //20100720 lee
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
															m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
															m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
															m_VDat[cam_idx]->m_InsPara.m_iParaFilm[11], 
															m_VDat[cam_idx]->m_InsPara.m_iParaFilm[10], 
															&m_VDat[cam_idx]->m_InsRslt.m_iFilmMetalNum[i], 
															&m_VDat[cam_idx]->m_HalRsltRgn[i][50]))
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMetal[i] = 1;
				}
				else
				{
					//Chipout
					m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMetal[i] = -33;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}
				*/
				//m_VDat[cam_idx]->m_InsRslt.m_iRsltFilmMetal[i] = 1;
			}
		}		
	}
	
	return rslt;
}

int C_VisionModule::InspectionLead(int cam_idx, int buf_idx, 
								   BOOL out_lead_use, 
								   BOOL out_lead_dirt_use, 
								   BOOL in_lead_use,
								   BOOL in_lead_dirt_use, 
								   BOOL out_lead2_use,
								   BOOL am_use0, BOOL am_use1, BOOL lead_scat_hori)
{
	int rslt = 1;
		
	Hobject modimage, shape;
	Hobject reudce_image;
	Hobject line;

	Hlong wd, ht;
	Hlong rn;
	int i;
	double offset_x, offset_y;
	CRect trect, brect, lrect, rrect, SrRect;
	
	rn = 0;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapFilm[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_31!");
		WriteErrorList((LPCTSTR)status);
		return 0; 
	}



	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = 0;
		m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = 0;

		if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&//if not no chip and puching chip
			(2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) && 
			(3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]))
		{
			//Y offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			//Top Lead inspection region
			trect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].left + offset_x;
			trect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].top + offset_y;
			trect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].right + offset_x;
			trect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][0].bottom + offset_y;

			//Bottom Lead inspection region
			brect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].left + offset_x;
			brect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].top + offset_y;
			brect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].right + offset_x;
			brect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][1].bottom + offset_y;

			if(out_lead2_use)
			{
				//Top Left Lead inspection region
				lrect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].left + offset_x;
				lrect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].top + offset_y;
				lrect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].right + offset_x;
				lrect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][2].bottom + offset_y;

				//Top Right Lead inspection region
				rrect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].left + offset_x;
				rrect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].top + offset_y;
				rrect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].right + offset_x;
				rrect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionLead[i][3].bottom + offset_y;
			}
			else
			{
				//Top Left Lead inspection region
				lrect.left = 0;
				lrect.top = 0;
				lrect.right = 0;
				lrect.bottom = 0;

				//Top Right Lead inspection region
				rrect.left = 0;
				rrect.top = 0;
				rrect.right = 0;
				rrect.bottom = 0;
			}
			
			//Top Lead///////////////////////////
			//double 0 : top factor, int 1 : top tol, 2 : top trsh, 6 : top-dirty tol, 7 : top-dirty trsh
			if(out_lead_use || out_lead_dirt_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = -40;

				if(10 < trect.top)
				{
					if(OutLead(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[0]),
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[1]), 
						   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[5]), 
						   trect, lrect, rrect,
						   m_VDat[cam_idx]->m_HalRsltRgn[i][27],//not sr black mold result
						   m_VDat[cam_idx]->m_HalInsRgn[i][2],//up lead region
						   m_VDat[cam_idx]->m_HalInsRgn[i][3],//dn lead region
						   m_VDat[cam_idx]->m_HalInsRgn[i][8],//up left lead region
						   m_VDat[cam_idx]->m_HalInsRgn[i][9],//up right lead region
						   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
						   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
						   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
						   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[2],
						   m_VDat[cam_idx]->m_InsPara.m_dParaLead[0], 
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[1],
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[7], 
						   m_VDat[cam_idx]->m_InsPara.m_iParaLead[6],
						   out_lead_use, out_lead_dirt_use, lead_scat_hori,
						   &m_VDat[cam_idx]->m_InsRslt.m_iLeadTopNum[i], 
						   &m_VDat[cam_idx]->m_HalRsltRgn[i][32],
						   &m_VDat[cam_idx]->m_InsRslt.m_iLeadTopNum2[i], 
						   &m_VDat[cam_idx]->m_HalRsltRgn[i][34],
						   &m_VDat[cam_idx]->m_HalRsltRgn[i][30]) )
					{
						//OK
						m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = 1;
					}
					else
					{
						//Top error
						m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadTop[i] = -40;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
					}
				}
			}

			//Bot Lead///////////////////////////
			if(in_lead_use || in_lead_dirt_use)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = -41;

				//20110626 lee
				//검사 영역이 SR 검사 영역 내부에 있지 않으면 검사 PASS한다.
				SrRect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].left;
				SrRect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].top;
				SrRect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].right;
				SrRect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionFilm[i][0].bottom;

				if(10 < brect.top)
				{
					//20110626 lee
					//검사 영역이 SR 검사 영역 내부에 있지 않으면 검사 PASS한다.
					if(brect.left > SrRect.left && brect.right < SrRect.right)
					{	
						//double 1 : bot factor, int 4 : bot tol, 5 : bot trsh
						if(InLead(0, GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[2]),
							GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[3]),
							brect,	
							m_VDat[cam_idx]->m_HalRsltRgn[i][27],
							m_VDat[cam_idx]->m_HalInsRgn[i][3],//dn lead region
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
							m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
							m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
							m_VDat[cam_idx]->m_InsPara.m_iParaLead[5],
							m_VDat[cam_idx]->m_InsPara.m_dParaLead[1], 
							m_VDat[cam_idx]->m_InsPara.m_iParaLead[4],
							&m_VDat[cam_idx]->m_InsRslt.m_dLeadBotWAVG[i], 
							&m_VDat[cam_idx]->m_InsRslt.m_iLeadBotNum[i], 
							&m_VDat[cam_idx]->m_HalRsltRgn[i][33],
							&m_VDat[cam_idx]->m_InsRslt.m_iLeadBotNum2[i], 
							&m_VDat[cam_idx]->m_HalRsltRgn[i][35],
							&m_VDat[cam_idx]->m_HalRsltRgn[i][31]))
						{
							//OK
							m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = 1;
						}
						else
						{
							//Bottom error
							m_VDat[cam_idx]->m_InsRslt.m_iRsltLeadBot[i] = -41;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
						}
					}
				}
			}

			if(am_use0 || am_use1)
			{
				m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -42;

				if(AMLead(0, 
								   GetHalImage(cam_idx, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapLead[4]),
								   am_use0,
								   am_use1,
								   m_VDat[cam_idx]->m_HalInsRgn[i][8],
							       m_VDat[cam_idx]->m_HalInsRgn[i][9],
								   m_VDat[cam_idx]->m_HalInsRgn[i][10],
								   m_VDat[cam_idx]->m_HalInsRgn[i][11],
								   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0], 
								   m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][1], m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][1], 
								   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0],
								   m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][1], m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][1],
								   m_VDat[cam_idx]->m_InsPara.m_iParaLead[9], m_VDat[cam_idx]->m_InsPara.m_iParaLead[8], 
								   &m_VDat[cam_idx]->m_InsRslt.m_iLeadAMNum[i], 
								   &m_VDat[cam_idx]->m_HalRsltRgn[i][60], 
								   &m_VDat[cam_idx]->m_HalRsltRgn[i][61], 
								   &m_VDat[cam_idx]->m_HalRsltRgn[i][62], 
								   &m_VDat[cam_idx]->m_HalRsltRgn[i][63],
								   &m_VDat[cam_idx]->m_HalRsltRgn[i][64]) )
				{
					//OK
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = 1;
				}
				else
				{
					//Bottom error
					m_VDat[cam_idx]->m_InsRslt.m_iRsltAM[i] = -42;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;
				}

			}
		}
	}
	
	return rslt;
}

int C_VisionModule::InspectionOCV(int cam_idx, int buf_idx, BOOL ocv_use)
{
	int rslt = 1;

	Hobject modimage, shape, Region, ReducedImage, RegionDiff, TransImage, CropImage;
	HTuple HomMat2D;
	Hlong wd, ht;
	double rx, ry, ra, rs, rscore, dx, dy, char_dx[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM], char_dy[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM];   
	Hlong rn;
	int i, j;
	int idx;
	double offset_x, offset_y;
	CRect rect;
	HTuple Num;
	Hobject PatternRegion[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM];

	double pat_score;

	BOOL good_chk, sub_good_chk;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*GetHalImage(0, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_32!");
		WriteErrorList((LPCTSTR)status);
		return 0;
	}
	
	rn = 0;

	good_chk = TRUE;
	for(i = 0; (i < m_VDat[cam_idx]->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = 0;

		if((1 == m_VDat[cam_idx]->m_InsRslt.m_iRsltAlign[i]) &&
		  (2 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&	/*펀칭 칩이 아닐 때*/ 
		  (3 != m_VDat[cam_idx]->m_InsRslt.m_iRsltChipFind[i]) &&		/*편집불량이 아닐 때*/
		  ocv_use)//if not no chip and puching chip
		{			
			m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -50;

			pat_score = (((double)m_VDat[cam_idx]->m_InsPara.m_iParaOCV[1]) * 0.01);

			//Y offset
			offset_x = m_VDat[cam_idx]->m_InsRslt.m_dCXAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosXAlign[i][0];
			offset_y = m_VDat[cam_idx]->m_InsRslt.m_dCYAlign[i][0] - m_VDat[cam_idx]->m_InsPara.m_dPatternPosYAlign[i][0];
			
			//For display
			rect.left = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].left + offset_x;
			rect.top = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].top + offset_y;
			rect.right = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].right + offset_x;
			rect.bottom = m_VDat[cam_idx]->m_InsPara.m_InsRegionOCV[i][0].bottom + offset_y;
			
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right); 
			
			gen_rectangle1(&m_VDat[cam_idx]->m_HalRsltRgn[i][40], rect.top, rect.left, rect.bottom, rect.right);
			
			reduce_domain(*GetHalImage(0, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), Region, &ReducedImage);
						
			dx = 0;
			dy = 0;
			if(0 < m_VDat[cam_idx]->m_InsPara.m_iParaOCV[2])
			{
				//Total//////////////////////////////
				idx = m_VDat[cam_idx]->m_InsPara.m_iParaOCV[2];
				
				modimage.Reset();
				shape.Reset();
				good_chk = TRUE;
				
				//0 : pat threshold, 1 : pat score, 2 : Taught character num, 3 : var threshold, 4 : var tol
				if(m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDOCV[idx])
				{
					Hal_ShapePatternFind(0, 
						&ReducedImage, 
						m_VDat[cam_idx]->m_InsPara.m_PatIDOCV[idx], 
						m_VDat[cam_idx]->m_InsPara.m_PatParaOCV[idx], 
						m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[idx], 
						wd, ht,
						m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[idx],
						m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[idx],
						pat_score,//Score
						&m_VDat[cam_idx]->m_HalRsltRgn[i][41], 
						&shape, 
						&rx, 
						&ry, 
						&ra, 
						&rs, 
						&rscore, 
						&rn);
					if(rn)
					{
						//OK
						dx = rx - m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[idx];
						dy = ry - m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[idx];
					}
					else
					{
						rslt = 0;
						
						//Can't find total pattern
						m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -50;
						rslt = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
						m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

						good_chk = FALSE;
					}
				}
				else
				{
					rslt = 0;
					
					//No pattern
					m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -51;
					rslt = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
					m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

					good_chk = FALSE;
				}

				m_VDat[cam_idx]->m_InsRslt.m_iOCVPatNum[i] = 0;

				sub_good_chk = TRUE;
				for(j = 0; j < m_VDat[cam_idx]->m_InsPara.m_iParaOCV[2]; j++)
				{
					modimage.Reset();
					shape.Reset();
				
					if(good_chk)
					{
						//Sub////////////////////////////////
						//세부영역조정
						gen_rectangle1(&Region, m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].top + dy, 
							m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].left + dx, 
							m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].bottom + dy, 
							m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j].right + dx);
						reduce_domain(*GetHalImage(0, buf_idx, m_VDat[cam_idx]->m_InsPara.m_iParaMapOCV[0]), Region, &ReducedImage);
						

						if(m_VDat[cam_idx]->m_InsPara.m_bCreatMdIDOCV[j])
						{
							modimage.Reset();
							shape.Reset();
							//0 : pat threshold, 1 : pat score, 2 : Taught character num, 3 : var threshold, 4 : var tol
							Hal_ShapePatternFind(0, 
								&ReducedImage, 
								m_VDat[cam_idx]->m_InsPara.m_PatIDOCV[j], 
								m_VDat[cam_idx]->m_InsPara.m_PatParaOCV[j], 
								m_VDat[cam_idx]->m_InsPara.m_InsCharRegionOCV[j], 
								wd, ht,
								m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[j],
								m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[j],
								pat_score,//Score
								&modimage, 
								&shape, 
								&rx, 
								&ry, 
								&ra, 
								&rs, 
								&rscore, 
								&rn);
							if(rn)
							{
								char_dx[i][j] = rx - m_VDat[cam_idx]->m_InsPara.m_dCharCenterX[j];
								char_dy[i][j] = ry - m_VDat[cam_idx]->m_InsPara.m_dCharCenterY[j];
								
								concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][42], modimage, &m_VDat[cam_idx]->m_HalRsltRgn[i][42]);
								concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][43], shape, &m_VDat[cam_idx]->m_HalRsltRgn[i][43]);

								copy_obj(shape, &PatternRegion[i][j], 1, -1);
							}
							else
							{
								char_dx[i][j] = 0;
								char_dy[i][j] = 0;
								
								concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][44], Region, &m_VDat[cam_idx]->m_HalRsltRgn[i][44]);
								m_VDat[cam_idx]->m_InsRslt.m_iOCVPatNum[i]++;
								rslt = 0;
						
								//No can't find char pattern
								m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -52;
								rslt = 0;
								m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
								m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

								sub_good_chk = FALSE;
							}
						}
						else
						{
							char_dx[i][j] = 0;
							char_dy[i][j] = 0;
							
							concat_obj(m_VDat[cam_idx]->m_HalRsltRgn[i][44], Region, &m_VDat[cam_idx]->m_HalRsltRgn[i][44]);
							m_VDat[cam_idx]->m_InsRslt.m_iOCVPatNum[i]++;

							rslt = 0;
							
							//No char pattern
							m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = -53;
							rslt = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRslt[i] = 0;
							m_VDat[cam_idx]->m_InsRslt.m_iRsltErrNum[i]++;

							sub_good_chk = FALSE;
						}

						if(sub_good_chk)
						{
							m_VDat[cam_idx]->m_InsRslt.m_iRsltOCV[i] = 1;
						}
					}
				}
			}
		}
	}
	
	return rslt;
}

void C_VisionModule::SetNGViewMode(BOOL mode, int ng_view_idx)
{
	if(mode)
	{
		m_iNGViewMode = ng_view_idx;
		if( (0 > m_iNGViewMode) && (NG_VIEW_MAX_NUM <= m_iNGViewMode) )
		{
			m_iNGViewMode = -1;
		}
	}
	else
	{
		m_iNGViewMode = -1;
	}
}

void C_VisionModule::UpdateSRBubPrepare()
{
	int i;

	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		m_bSRBubPrepare[i] = TRUE;
	}
}

//rslt_num : 불량개수
//rslt_region : 불량영역(SR + user area불량)
//rslt_sr_region : sr 검사영역(위치보정된 후)
//rslt_notsr_region : lead부분의 불량 잡은것(첫번째와 두번째 영상 뺀것)
//rslt_user_region : user area 검사영역(위치보정된 후)
BOOL C_VisionModule::_Hal_FilmMold(Hlong win, Hobject *mold_image, Hobject *ink_image, Hobject *user_image, Hobject *scratch_image, Hobject *taught_image,
							  RECT ins_region, RECT chip_region, RECT ocv_region, 
							  Hobject ocv_chars_region, Hobject sr_region, Hobject user_region, Hobject sr_out_region, 
							  Hlong var_id, 
							  double first_offset_ox, double first_offset_oy,			//첫 device의 패턴등록점		
							  double first_offset_ox2, double first_offset_oy2, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,

							  int dark_thresh, int dark_bright_tol,					//인식밝기차(검정), 허용밝기차(검정)
							  int white_thresh, int white_bright_tol,				//인식밝기차(흰색), 허용밝기차(흰색)

							  int mold_fact, int tolerance, int tolerance_light,	//FACTOR, 허용직경(검정), 허용직경(흰색)
							  
							  int user_thresh, int user_tol,						//사용안함, 사용안함

							  int sr_thresh, int sr_hi_thresh, int sr_tol,			//SR-OUT 편짐영역긇히밝기하한, 상한, 허용직경

							  int sub_sr_rgn_num, int sub_sr_rgn_x[][SR_SUBREGION_POINT_MAX_NUM], int sub_sr_rgn_y[][SR_SUBREGION_POINT_MAX_NUM],	//20150123 NEW by
							  int sub_sr_rgn_type[SR_SUBREGION_MAX_NUM], //0 : 세밀한 패턴의 SR, 1 : 굵은 패턴의 SR

							  Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region, Hobject *rslt_notsr_region, Hobject *rslt_user_region, Hobject *rslt_sr_out_region,
							  Hlong min_area, Hlong min_width, Hlong min_height,
							  Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr)
{
	Hobject Image, ImageLight, ImageTrans, RegionDiff, ImageReduced, ImageReducedFine, ImageReducedLight, ImageReducedLight2;
	Hobject	RegionRect, ChipRect, OCVRect;
	HTuple HomMat2D, Num;
	Hobject select_object;
	Hobject tot_sr_sub_rgn, sr_sub_rgn, ins_rgn, ins_rgn_fine, OCVRegion, ins_rgn_srout;
	HTuple DiffAng;

	Hobject RsltSRRegion, RsltNotSRRegion, RsltUserRegion, RsltSROutRegion;

	HTuple StartSecond, EndSecond;
	long m_lGrabTime;

	gen_empty_obj(&RsltSRRegion);
	gen_empty_obj(&RsltNotSRRegion);
	gen_empty_obj(&RsltUserRegion);
	gen_empty_obj(&RsltSROutRegion);

	int i, j;

	Hobject OutRgn, IntRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean, IntMean;
	int diff_var;
	Hobject ComplRgn;
	
	Hobject Dark, DarkFine, DarkSROut, UserDark, Light, MaskImage, MaskRegion, MaskRegion1, MaskRegion2;
	Hobject ImageReducedNotSR, ImageNotSR, ImageReducedUser, ImageReducedSROut;

	double offset_x, offset_y;

	Hlong max, num;
	HTuple Max;

	Hobject MeanImage, ImageLight2, SROutImage;

	HTuple Area;

	HTuple CircleVar;

	HTuple LightMean;

	Hlong lightvar;

	HTuple Sub_Rgn_Num_X, Sub_Rgn_Num_Y;

	HTuple FirstHomMat2D;
	HTuple FirstDiffAng;

	Hobject ImageMean, TaughtImage, TaughtImageReduced;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;
	ins_region.top = ins_region.top + offset_y;
	ins_region.left = ins_region.left + offset_x;	
	ins_region.bottom = ins_region.bottom + offset_y;
	ins_region.right = ins_region.right + offset_x;

	gen_rectangle1(&RegionRect, (Hlong)(ins_region.top), 
								(Hlong)(ins_region.left), 
								(Hlong)(ins_region.bottom), 
								(Hlong)(ins_region.right));

	gen_rectangle1(&ChipRect, (Hlong)(chip_region.top + offset_y), 
							  (Hlong)(chip_region.left + offset_x), 
							  (Hlong)(chip_region.bottom + offset_y), 
							  (Hlong)(chip_region.right + offset_x));

	gen_rectangle1(&OCVRect, (Hlong)(ocv_region.top + offset_y), 
							  (Hlong)(ocv_region.left + offset_x), 
							  (Hlong)(ocv_region.bottom + offset_y), 
							  (Hlong)(ocv_region.right + offset_x));
	

	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);

	affine_trans_region(sr_region, &RsltSRRegion, HomMat2D, HTuple("false"));
	affine_trans_region(user_region, &RsltUserRegion, HomMat2D, HTuple("false"));
	affine_trans_region(sr_out_region, &RsltSROutRegion, HomMat2D, HTuple("false"));	

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, first_offset_oy, first_offset_ox, first_offset_oy2, first_offset_ox2, &FirstDiffAng);
	vector_angle_to_rigid(first_offset_oy, first_offset_ox, FirstDiffAng, offset_ry, offset_rx, HTuple(0), &FirstHomMat2D);

	//Dark image//////////////////////////////////
	//Mask image(테두리 둔감하게)
	gen_image_proto(*mold_image, &MaskImage, HTuple(0));

	//세부패턴영역
	//SR/////////////////////////////////////////
	reduce_domain(*mold_image, RsltSRRegion, &Image);
	mean_image(Image, &Image, HTuple(mold_fact), HTuple(mold_fact));

	//Light image////////////////////////////////
	reduce_domain(*ink_image, RsltSRRegion, &ImageLight);
	mean_image(ImageLight, &ImageLight, HTuple(mold_fact), HTuple(mold_fact));

	dilation_circle(ocv_chars_region, &OCVRegion, 7.5);

	//굵은패턴영역
	reduce_domain(*mold_image, RsltSRRegion, &MeanImage);
	mean_image(MeanImage, &MeanImage, HTuple(15), HTuple(15));

	reduce_domain(*taught_image, RsltSRRegion, &TaughtImage);
	affine_trans_image(TaughtImage, &TaughtImage, HomMat2D, HTuple("constant"), HTuple("false"));
	mean_image(TaughtImage, &TaughtImage, HTuple(15), HTuple(15));
	invert_image(TaughtImage, &TaughtImage);

	//결과 초기화
	gen_empty_obj(&ResultRgn);
	gen_empty_obj(&tot_sr_sub_rgn);

	for(i = 0; i < sub_sr_rgn_num; i++)
	{
		//Sub region 만들기///////////////////////////////////////////////////////////////////////////////////////
		Sub_Rgn_Num_X.Reset();
		Sub_Rgn_Num_Y.Reset();

		for(j = 0; j < SR_SUBREGION_POINT_MAX_NUM; j++)
		{
			if(0 < sub_sr_rgn_x[i][j])
			{
				tuple_concat(Sub_Rgn_Num_X, sub_sr_rgn_x[i][j], &Sub_Rgn_Num_X);
				tuple_concat(Sub_Rgn_Num_Y, sub_sr_rgn_y[i][j], &Sub_Rgn_Num_Y);
			}
			else
			{
				break;
			}
		}

		gen_region_polygon_filled(&sr_sub_rgn, Sub_Rgn_Num_Y, Sub_Rgn_Num_X);

		if(win)
		{
			set_color(win, HTuple("green"));
			disp_obj(sr_sub_rgn, win);
		}

		affine_trans_region(sr_sub_rgn, &sr_sub_rgn, FirstHomMat2D, HTuple("false"));	
		intersection(RsltSRRegion, sr_sub_rgn, &ins_rgn);
		concat_obj(tot_sr_sub_rgn, ins_rgn, &tot_sr_sub_rgn);

		//SR 전영역 검사영역, 어두운 불량////////////////////////////////////////////////////////////////////////1
		erosion_circle(ins_rgn, &ins_rgn, 1.5);
		difference(ins_rgn, ChipRect, &ins_rgn);
		difference(ins_rgn, OCVRegion, &ins_rgn);


		if(0 == sub_sr_rgn_type[i])
		{
			//세부패턴영역
			reduce_domain(Image, ins_rgn, &ImageReduced);
		}
		else
		{
			//굵은패턴영역
			reduce_domain(MeanImage, ins_rgn, &ImageReduced);
			reduce_domain(TaughtImage, ins_rgn, &TaughtImageReduced);
			//affine_trans_image(TaughtImageReduced, &TaughtImageReduced, HomMat2D, HTuple("constant"), HTuple("false"));
			add_image(ImageReduced, TaughtImageReduced, &ImageReduced, 1, -126);
		}

		if(win)
		{
			disp_obj(ImageReduced, win);
		}
		
		//SR 정밀영역////////////////////////////////////////////////////////////////////////////////////////////2
		//SR 영역에서 테두리 제외 하고 검사(더 세밀히게)영역, 어두운 불량
		if(0 == sub_sr_rgn_type[i])
		{
			fill_up(ins_rgn, &ins_rgn_fine);
			erosion_circle(ins_rgn_fine, &ins_rgn_fine, 10.5);
			intersection(ins_rgn, ins_rgn_fine, &ins_rgn_fine);
			reduce_domain(Image, ins_rgn_fine, &ImageReducedFine);
		}

		//밝은 불량//////////////////////////////////////////////////////////////////////////////////////////////3
		//ink image : map 3
		reduce_domain(ImageLight, ins_rgn, &ImageReducedLight);//긁힘같은 밝은불량  검사영역

		//밝은 불량2/////////////////////////////////////////////////////////////////////////////////////////////4
		//mold image : map 1
		reduce_domain(*mold_image, ins_rgn, &ImageReducedLight2);//밝은불량 검사영역

		//SR out/////////////////////////////////////////////////////////////////////////////////////////////////5
		difference(RsltSROutRegion, ChipRect, &RsltSROutRegion);
		difference(RsltSROutRegion, OCVRegion, &ins_rgn_srout);

		intersection(ins_rgn_srout, sr_sub_rgn, &ins_rgn_srout);

		reduce_domain(*scratch_image, RegionRect, &ImageReducedSROut);//SR out검사영역
		emphasize(ImageReducedSROut, &ImageReducedSROut, 7, 7, 6);
		gray_erosion_rect(ImageReducedSROut, &ImageReducedSROut, 3.5, 3.5);
		reduce_domain(ImageReducedSROut, ins_rgn_srout, &ImageReducedSROut);//SR out검사영역

		/////////////////////////////////////////////
		//Dark///////////////////////////////////////
		//
		//For SR/////////////////////////////////////
		min_max_gray(ins_rgn, Image, HTuple(5), _, &Max, _);
		max = (Hlong)Max[0];
		max = max - dark_thresh;
		if(0 > max)
		{
			max = 230;
		}
		if(255 < max)
		{
			max = 255;
		}

		//주변값을 중간값으로 채운다.
		paint_region(ChipRect, Image, &Image, 255, HTuple("fill"));


		threshold(ImageReduced, &Dark, 0, max);
		closing_circle(Dark, &Dark, 10.5);
		connection(Dark, &Dark);
		fill_up(Dark, &Dark);
		closing_circle(Dark, &Dark, 10.5);

		select_shape(Dark, &Dark, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
		select_shape(Dark, &Dark, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
		select_shape(Dark, &Dark, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

		//경계값과 비교
		count_obj(Dark, &Num);

		if(win)
		{
			set_color(win, HTuple("yellow"));
			disp_obj(Dark, win);
		}

		concat_obj(RsltSRRegion, Dark, &RsltSRRegion);
		num = (Hlong)Num[0];
		for(j = 0; j < num; j++)
		{
			select_obj(Dark, &SelectRgn, j + 1);
			dilation_circle(SelectRgn, &OutRgn, 10.5);
			difference(OutRgn, SelectRgn, &OutRgn);
			intensity(SelectRgn, Image, &Mean, _);
			intensity(OutRgn, Image, &OutMean, _);
			diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

			//크기가 크면 섬세하게 검사
			area_center(SelectRgn, &Area, _, _);

			if(700 < (Hlong)Area[0])//20150122 lss 기존 300
			{
				//밝기
				if(0 < diff_var)
				{
					//Dark(+)
					diff_var = abs(diff_var);
					if(dark_bright_tol < (diff_var + 10))
					{
						concat_obj(ResultRgn, SelectRgn, &ResultRgn);
					}
				}
			}
			else
			{
				//밝기
				if(0 < diff_var)
				{
					//Dark(+)

					diff_var = abs(diff_var);
					if(dark_bright_tol < diff_var)
					{
						concat_obj(ResultRgn, SelectRgn, &ResultRgn);
					}
				}
			}
		}

		if(0 == sub_sr_rgn_type[i])
		{
			//고정(전체 줄임 fine)
			threshold(ImageReducedFine, &DarkFine, 0, max);
			closing_circle(DarkFine, &DarkFine, 5.5);
			closing_rectangle1(DarkFine, &DarkFine, 1, 30.5);
			connection(DarkFine, &DarkFine);
			fill_up(DarkFine, &DarkFine);
			closing_circle(DarkFine, &DarkFine, 5.5);
			closing_rectangle1(DarkFine, &DarkFine, 1, 30.5);
			select_shape(DarkFine, &DarkFine, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
			select_shape(DarkFine, &DarkFine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
			select_shape(DarkFine, &DarkFine, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

			//경계값과 비교
			count_obj(DarkFine, &Num);

			if(win)
			{
				set_color(win, HTuple("blue"));
				disp_obj(DarkFine, win);
			}

			concat_obj(RsltSRRegion, DarkFine, &RsltSRRegion);
			num = (Hlong)Num[0];
			for(j = 0; j < num; j++)
			{
				select_obj(DarkFine, &SelectRgn, j + 1);
				dilation_circle(SelectRgn, &OutRgn, 10.5);
				difference(OutRgn, SelectRgn, &OutRgn);
				intensity(SelectRgn, *mold_image, &Mean, _);
				intensity(OutRgn, *mold_image, &OutMean, _);
				diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

				//크기가 크면 그냥 불량으로 뺀다.
				area_center(SelectRgn, &Area, _, _);


				if(700 < (Hlong)Area[0])//20150122 lss 기존 300
				{
					//밝기
					if(0 < diff_var)
					{
						//Dark(+)
						diff_var = abs(diff_var);
						if(dark_bright_tol < (diff_var + 40))
						{
							concat_obj(ResultRgn, SelectRgn, &ResultRgn);
						}
					}
				}
				else
				{
					//밝기
					if(0 < diff_var)
					{
						//Dark(+)
						diff_var = abs(diff_var);
						if(dark_bright_tol < (diff_var + 10) )
						{
							concat_obj(ResultRgn, SelectRgn, &ResultRgn);
						}
					}
				}
			}
		}

		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(ResultRgn, win);
		}

		/////////////////////////////////////////////
		//Light//////////////////////////////////////
		//Ink(mold image==SR image), 밝은 이물
		max = 255 - white_thresh;
		if((0 > max) || (255 < max))
		{
			max = 230;
		}

		threshold(ImageReducedLight, &Light, max, 255);

		closing_circle(Light, &Light, 5.5);
		connection(Light, &Light);
		fill_up(Light, &Light);

		select_shape(Light, &Light, HTuple("area"), HTuple("and"), HTuple(min_area_sr * 2), HTuple("max"));
		select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr * 2).Concat(min_height_sr * 2), HTuple("max").Concat("max"));
		select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance_light).Concat(tolerance_light), HTuple("max").Concat("max"));

		//경계값과 비교
		count_obj(Light, &Num);

		if(win)
		{
			set_color(win, HTuple("black"));
			disp_obj(Light, win);
		}

		concat_obj(RsltSRRegion, Light, &RsltSRRegion);
		num = (Hlong)Num[0];
		for(j = 0; j < num; j++)
		{
			select_obj(Light, &SelectRgn, j + 1);
			dilation_circle(SelectRgn, &OutRgn, 10.5);
			difference(OutRgn, SelectRgn, &OutRgn);
			intensity(SelectRgn, ImageLight, &Mean, _);
			intensity(OutRgn, ImageLight, &OutMean, _);
			diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

			if(0 >= diff_var)
			{
				//Light(-)
				diff_var = abs(diff_var);
				if( (white_bright_tol) < diff_var)
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
		//
		//
		/////////////////////////////////////////////

		/////////////////////////////////////////////
		//Light2/////////////////////////////////////
		//긁힘(ink image==crack image)<--crack image에서 밝은 긁힘, 밝은 이물
		hysteresis_threshold(ImageReducedLight2,&Light,HTuple(210),HTuple(230),HTuple(10));

		closing_circle(Light, &Light, 45.5);
		connection(Light, &Light);
		fill_up(Light, &Light);

		select_shape(Light, &Light, HTuple("area"), HTuple("and"), HTuple(min_area_sr * 2), HTuple("max"));
		select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr * 2).Concat(min_height_sr * 2), HTuple("max").Concat("max"));
		select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance_light).Concat(tolerance_light), HTuple("max").Concat("max"));

		//경계값과 비교
		count_obj(Light, &Num);

		concat_obj(RsltSRRegion, Light, &RsltSRRegion);
		num = (Hlong)Num[0];
		for(j = 0; j < num; j++)
		{
			select_obj(Light, &SelectRgn, j + 1);
			dilation_circle(SelectRgn, &OutRgn, 10.5);
			difference(OutRgn, SelectRgn, &OutRgn);
			intensity(SelectRgn, *mold_image, &Mean, _);
			intensity(OutRgn, *mold_image, &OutMean, _);
			diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

			if(0 >= diff_var)
			{
				//Light(-)
				diff_var = abs(diff_var);
				if( (white_bright_tol) < diff_var)
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}

		}
		//
		//
		/////////////////////////////////////////////

		/////////////////////////////////////////////
		//Dark-SR out////////////////////////////////
		//긁힘(밝은 back light image에서 검게 나온다. 검은 영역만 잡는다. SR out pattern에서만 검사, 안쪽 가는SR 구간 정밀검사)	

		if(1)
		{
			threshold(ImageReducedSROut,&DarkSROut,HTuple(sr_thresh),HTuple(sr_hi_thresh));

			closing_circle(DarkSROut, &DarkSROut, 10.5);
			connection(DarkSROut, &DarkSROut);

			select_shape(DarkSROut, &DarkSROut, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
			select_shape(DarkSROut, &DarkSROut, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
			select_shape(DarkSROut, &DarkSROut, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(sr_tol), HTuple("max").Concat("max"));

			//경계값과 비교
			count_obj(DarkSROut, &Num);

			concat_obj(RsltSRRegion, DarkSROut, &RsltSRRegion);
			num = (Hlong)Num[0];

			for(j = 0; j < num; j++)
			{
				select_obj(DarkSROut, &SelectRgn, j + 1);
				dilation_circle(SelectRgn, &OutRgn, 7.5);
				dilation_circle(SelectRgn, &IntRgn, 2.5);
				difference(OutRgn, IntRgn, &OutRgn);
				intensity(SelectRgn, ImageReducedSROut, &Mean, _);
				intensity(OutRgn, ImageReducedSROut, &OutMean, _);
				intensity(SelectRgn, ImageLight, &LightMean, _);
				diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);
				circularity(SelectRgn, &CircleVar);		
				//크기가 크면 섬세하게 검사
				area_center(SelectRgn, &Area, _, _);

				if(700 < (Hlong)Area[0])//20150122 lss 기존 300
				{
					//밝기
					if(0 < diff_var)
					{
						//Dark(+)
						diff_var = abs(diff_var);
						if(dark_bright_tol < (diff_var + 10))
						{
							concat_obj(ResultRgn, SelectRgn, &ResultRgn);
						}
					}
				}
				else
				{
					//여기서만 백색 이물 제거 필요(강조를 하다 보니 흰색은 흐리게 나오는데 잡힌다.->오버킬)
					//밝기
					if(0 < diff_var)
					{
						//Dark(+)

						diff_var = abs(diff_var);
						if(dark_bright_tol < diff_var)
						{
							//진원도가 0.5보다 크면서 백색 이물성이면 오버킬로 간주
							if(0.5 < (double)CircleVar[0] )
							{
								//잉크이미지(크랙이미지)에서 밝값이 밝으면 
								lightvar = (Hlong)LightMean[0];
								if(150 > lightvar)
								{
									concat_obj(ResultRgn, SelectRgn, &ResultRgn);
								}
							}
							else
							{
								concat_obj(ResultRgn, SelectRgn, &ResultRgn);
							}
						}
					}
				}
			}
		}

	}
	//
	//
	/////////////////////////////////////////////
	copy_obj(RsltSRRegion, rslt_sr_region, 1, -1);
	copy_obj(RsltNotSRRegion, rslt_notsr_region, 1, -1);
	copy_obj(RsltUserRegion, rslt_user_region, 1, -1);
	
	//20150126 lss
	//copy_obj(RsltSROutRegion, rslt_sr_out_region, 1, -1);
	copy_obj(tot_sr_sub_rgn, rslt_sr_out_region, 1, -1);
	
	union1(ResultRgn, &ResultRgn);
	closing_circle(ResultRgn, &ResultRgn, 50);
	connection(ResultRgn, &ResultRgn);
	count_obj(ResultRgn, &Num);
	*rslt_num = (Hlong)Num[0];

	if(0 < *rslt_num)
	{
		copy_obj(ResultRgn, rslt_region, 1, -1);
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}

		return FALSE;
	}

	return TRUE;
}

//20150123 이전 by
/*
//rslt_num : 불량개수
//rslt_region : 불량영역(SR + user area불량)
//rslt_sr_region : sr 검사영역(위치보정된 후)
//rslt_notsr_region : lead부분의 불량 잡은것(첫번째와 두번째 영상 뺀것)
//rslt_user_region : user area 검사영역(위치보정된 후)
BOOL C_VisionModule::_Hal_FilmMold(Hlong win, Hobject *mold_image, Hobject *ink_image, Hobject *user_image, Hobject *scratch_image,
							  RECT ins_region, RECT chip_region, RECT ocv_region, 
							  Hobject ocv_chars_region, Hobject sr_region, Hobject user_region, Hobject sr_out_region,
							  Hlong var_id, 
							  double offset_ox, double offset_oy, 
							  double offset_ox2, double offset_oy2, 
							  double offset_rx, double offset_ry,
							  double offset_rx2, double offset_ry2,
							  int dark_thresh, int dark_bright_tol, int white_thresh, int white_bright_tol, int mold_fact, int tolerance, int tolerance_light, int user_thresh, int user_tol,
							  int sr_thresh, int sr_hi_thresh, int sr_tol,
							  Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_sr_region, Hobject *rslt_notsr_region, Hobject *rslt_user_region, Hobject *rslt_sr_out_region,
							  Hlong min_area, Hlong min_width, Hlong min_height,
							  Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr)
{
	Hobject Image, ImageLight, ImageTrans, RegionDiff, ImageReduced, ImageReducedFine, ImageReducedLight, ImageReducedLight2;
	Hobject	RegionRect, ChipRect, OCVRect;
	HTuple HomMat2D, Num;
	Hobject select_object;
	Hobject ins_rgn, ins_rgn_fine, OCVRegion, ins_rgn_srout;
	HTuple DiffAng;

	Hobject RsltSRRegion, RsltNotSRRegion, RsltUserRegion, RsltSROutRegion;

	gen_empty_obj(&RsltSRRegion);
	gen_empty_obj(&RsltNotSRRegion);
	gen_empty_obj(&RsltUserRegion);
	gen_empty_obj(&RsltSROutRegion);

	int i;

	Hobject OutRgn, IntRgn, SelectRgn, ResultRgn;
	HTuple Mean, OutMean, IntMean;
	int diff_var;
	Hobject ComplRgn;
	
	Hobject Dark, DarkFine, DarkSROut, UserDark, Light, MaskImage, MaskRegion, MaskRegion1, MaskRegion2;
	Hobject ImageReducedNotSR, ImageNotSR, ImageReducedUser, ImageReducedSROut;

	double offset_x, offset_y;

	Hlong max, num;
	HTuple Max;

	Hobject MeanImage, ImageLight2, SROutImage;

	HTuple Area;

	HTuple CircleVar;

	HTuple LightMean;

	Hlong lightvar;

	angle_ll(offset_ry, offset_rx, offset_ry2, offset_rx2, offset_oy, offset_ox, offset_oy2, offset_ox2, &DiffAng);

	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;
	ins_region.top = ins_region.top + offset_y;
	ins_region.left = ins_region.left + offset_x;	
	ins_region.bottom = ins_region.bottom + offset_y;
	ins_region.right = ins_region.right + offset_x;

	gen_rectangle1(&RegionRect, (Hlong)(ins_region.top), 
								(Hlong)(ins_region.left), 
								(Hlong)(ins_region.bottom), 
								(Hlong)(ins_region.right));

	gen_rectangle1(&ChipRect, (Hlong)(chip_region.top + offset_y), 
							  (Hlong)(chip_region.left + offset_x), 
							  (Hlong)(chip_region.bottom + offset_y), 
							  (Hlong)(chip_region.right + offset_x));

	gen_rectangle1(&OCVRect, (Hlong)(ocv_region.top + offset_y), 
							  (Hlong)(ocv_region.left + offset_x), 
							  (Hlong)(ocv_region.bottom + offset_y), 
							  (Hlong)(ocv_region.right + offset_x));
	
	vector_angle_to_rigid(offset_oy, offset_ox, DiffAng, offset_ry, offset_rx, HTuple(0), &HomMat2D);
	affine_trans_region(sr_region, &RsltSRRegion, HomMat2D, HTuple("false"));
	affine_trans_region(user_region, &RsltUserRegion, HomMat2D, HTuple("false"));
	affine_trans_region(sr_out_region, &RsltSROutRegion, HomMat2D, HTuple("false"));	

	//Dark image/////////////////////////////////
	//Mask image(테두리 둔감하게)
	gen_image_proto(*mold_image, &MaskImage, HTuple(0));

	//End Time #2
	count_seconds(&EndSecond);
	EndSecond = EndSecond - StartSecond;
	m_lGrabTime = (long)(((double)EndSecond[0]) * 1000);
	TRACE1("Time #2 : %d\n", m_lGrabTime);
	
	//SR/////////////////////////////////////////
	//add_image(*mold_image, *mold_image, &Image, HTuple(0.6), HTuple(0));
	//add_image(Image, MaskImage, &Image, HTuple(1.0), HTuple(0));
	//mean_image(Image, &Image, HTuple(mold_fact), HTuple(mold_fact));
	mean_image(*mold_image, &Image, HTuple(mold_fact), HTuple(mold_fact));


	//Not SR(for lead////////////////////////////
	//add_image(*mold_image, *mold_image, &ImageNotSR, HTuple(0.7), HTuple(0));

	//Light image////////////////////////////////
	mean_image(*ink_image, &ImageLight, HTuple(mold_fact), HTuple(mold_fact));

	if(win)
	{
		//disp_obj(Image, win);
	}

	//SR 전영역 검사영역, 어두운 불량////////////
	erosion_circle(RsltSRRegion, &ins_rgn, 1.5);
	difference(ins_rgn, ChipRect, &ins_rgn);
	dilation_circle(ocv_chars_region, &OCVRegion, 7.5);
	difference(ins_rgn, OCVRegion, &ins_rgn);
	reduce_domain(Image, ins_rgn, &ImageReduced);

	//SR 정밀영역////////////////////////////////
	//SR 영역에서 테두리 제외 하고 검사(더 세밀히게)영역, 어두운 불량
	fill_up(ins_rgn, &ins_rgn_fine);
	erosion_circle(ins_rgn_fine, &ins_rgn_fine, 55.5);
	intersection(ins_rgn, ins_rgn_fine, &ins_rgn_fine);
	reduce_domain(Image, ins_rgn_fine, &ImageReducedFine);
	
	//Lead검사용 영역(여기서는 사용않는다 나중에 lead검사부에서 이영역을 사용한다.
	//reduce_domain(ImageNotSR, RegionRect, &ImageReducedNotSR);//Lead 검사영역

	//밝은 불량//////////////////////////////////
	//ink image : map 3
	reduce_domain(ImageLight, ins_rgn, &ImageReducedLight);//긁힘같은 밝은불량  검사영역

	//밝은 불량2/////////////////////////////////
	//mold image : map 1
	reduce_domain(*mold_image, ins_rgn, &ImageReducedLight2);//밝은불량 검사영역

	//User area검사//////////////////////////////
	reduce_domain(*user_image, RsltUserRegion, &ImageReducedUser);//user area검사영역

	difference(RsltSROutRegion, ChipRect, &RsltSROutRegion);
	difference(RsltSROutRegion, OCVRegion, &ins_rgn_srout);

	reduce_domain(*scratch_image, RegionRect, &ImageReducedSROut);//SR out검사영역
	//reduce_domain(*mold_image, RegionRect, &ImageReducedSROut);//SR out검사영역

	emphasize(ImageReducedSROut, &ImageReducedSROut, 7, 7, 6);
	gray_erosion_rect(ImageReducedSROut, &ImageReducedSROut, 3.5, 3.5);
	reduce_domain(ImageReducedSROut, ins_rgn_srout, &ImageReducedSROut);//SR out검사영역


	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(ins_rgn, win);
		set_color(win, HTuple("blue"));
		disp_obj(RsltSROutRegion, win);
	}

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(RsltSRRegion, win);
	}

	gen_empty_obj(&ResultRgn);

	/////////////////////////////////////////////
	//Dark///////////////////////////////////////
	//
	//For SR/////////////////////////////////////
	min_max_gray(ins_rgn, Image, HTuple(5), _, &Max, _);
	max = (Hlong)Max[0];
	max = max - dark_thresh;
	if(0 > max)
	{
		max = 230;
	}
	if(255 < max)
	{
		max = 255;
	}

	//max = 240;
	//max = 230;
	threshold(ImageReduced, &Dark, 0, max);

	//For Lead(여기 필름 검사하고는 상관없음 나중에 lead검사에 사용하려고 시행 한것임)
	//threshold(ImageReducedNotSR, &RsltNotSRRegion, 0, max);

	//고정(전체 줄임 fine)
	//max = 254;
	//max = 230;
	threshold(ImageReducedFine, &DarkFine, 0, max);

	closing_circle(Dark, &Dark, 10.5);
	connection(Dark, &Dark);
	fill_up(Dark, &Dark);
	closing_circle(Dark, &Dark, 10.5);

	closing_circle(DarkFine, &DarkFine, 5.5);
	closing_rectangle1(DarkFine, &DarkFine, 1, 30.5);
	connection(DarkFine, &DarkFine);
	fill_up(DarkFine, &DarkFine);
	closing_circle(DarkFine, &DarkFine, 5.5);
	closing_rectangle1(DarkFine, &DarkFine, 1, 30.5);
	//shape_trans(Dark, &Dark,HTuple("convex"));

	select_shape(Dark, &Dark, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
	select_shape(Dark, &Dark, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
	select_shape(Dark, &Dark, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));

	select_shape(DarkFine, &DarkFine, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
	select_shape(DarkFine, &DarkFine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
	select_shape(DarkFine, &DarkFine, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));


	//주변값을 중간값으로 채운다.
	paint_region(ChipRect, Image, &Image, 255, HTuple("fill"));

	//경계값과 비교
	count_obj(Dark, &Num);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Dark, win);
	}

	concat_obj(RsltSRRegion, Dark, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(Dark, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, Image, &Mean, _);
		intensity(OutRgn, Image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		//크기가 크면 섬세하게 검사
		area_center(SelectRgn, &Area, _, _);

		if(300 < (Hlong)Area[0])
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)
				diff_var = abs(diff_var);
				if(dark_bright_tol < (diff_var + 40))//LSS new 10->40
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
		else
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)

				diff_var = abs(diff_var);
				if(dark_bright_tol < diff_var)
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
	}

	//경계값과 비교
	count_obj(DarkFine, &Num);

	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(DarkFine, win);
	}

	concat_obj(RsltSRRegion, DarkFine, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(DarkFine, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, *mold_image, &Mean, _);
		intensity(OutRgn, *mold_image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		//크기가 크면 그냥 불량으로 뺀다.
		area_center(SelectRgn, &Area, _, _);


		if(300 < (Hlong)Area[0])
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)
				diff_var = abs(diff_var);
				if(dark_bright_tol < (diff_var + 10))
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
		else
		{
			//밝기
			if(0 < diff_var)
			{
				//Dark(+)
				diff_var = abs(diff_var);
				if(dark_bright_tol < diff_var)
				{
					concat_obj(ResultRgn, SelectRgn, &ResultRgn);
				}
			}
		}
	}
	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(ResultRgn, win);
	}

	max = 255 - white_thresh;
	if((0 > max) || (255 < max))
	{
		max = 230;
	}

	threshold(ImageReducedLight, &Light, max, 255);

	closing_circle(Light, &Light, 5.5);
	connection(Light, &Light);
	fill_up(Light, &Light);

	select_shape(Light, &Light, HTuple("area"), HTuple("and"), HTuple(min_area_sr * 2), HTuple("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr * 2).Concat(min_height_sr * 2), HTuple("max").Concat("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance_light).Concat(tolerance_light), HTuple("max").Concat("max"));

	//경계값과 비교
	count_obj(Light, &Num);

	if(win)
	{
		set_color(win, HTuple("black"));
		disp_obj(Light, win);
	}

	concat_obj(RsltSRRegion, Light, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(Light, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, ImageLight, &Mean, _);
		intensity(OutRgn, ImageLight, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		if(0 >= diff_var)
		{
			//Light(-)
			diff_var = abs(diff_var);
			if( (white_bright_tol) < diff_var)
			{
				concat_obj(ResultRgn, SelectRgn, &ResultRgn);
			}
		}
	
	}
	//
	//
	/////////////////////////////////////////////

	/////////////////////////////////////////////
	//Light2/////////////////////////////////////
	//긁힘(ink image==crack image)<--crack image에서 밝은 긁힘, 밝은 이물
	hysteresis_threshold(ImageReducedLight2,&Light,HTuple(210),HTuple(230),HTuple(10));

	closing_circle(Light, &Light, 45.5);
	connection(Light, &Light);
	fill_up(Light, &Light);

	select_shape(Light, &Light, HTuple("area"), HTuple("and"), HTuple(min_area_sr * 2), HTuple("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr * 2).Concat(min_height_sr * 2), HTuple("max").Concat("max"));
	select_shape(Light, &Light, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance_light).Concat(tolerance_light), HTuple("max").Concat("max"));

	//경계값과 비교
	count_obj(Light, &Num);

	concat_obj(RsltSRRegion, Light, &RsltSRRegion);
	num = (Hlong)Num[0];
	for(i = 0; i < num; i++)
	{
		select_obj(Light, &SelectRgn, i + 1);
		dilation_circle(SelectRgn, &OutRgn, 10.5);
		difference(OutRgn, SelectRgn, &OutRgn);
		intensity(SelectRgn, *mold_image, &Mean, _);
		intensity(OutRgn, *mold_image, &OutMean, _);
		diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);

		if(0 >= diff_var)
		{
			//Light(-)
			diff_var = abs(diff_var);
			if( (white_bright_tol) < diff_var)
			{
				concat_obj(ResultRgn, SelectRgn, &ResultRgn);
			}
		}
	
	}
	//
	//
	/////////////////////////////////////////////

	/////////////////////////////////////////////
	//Dark-SR out////////////////////////////////
	//긁힘(밝은 back light image에서 검게 나온다. 검은 영역만 잡는다. SR out pattern에서만 검사, 안쪽 가는SR 구간 정밀검사)	

	intensity(ins_rgn_srout, ImageReducedSROut, &Mean, _);
	int chk_cnt = Mean.Num();
	
	if(0 < chk_cnt)
	{
		max = (Hlong)Mean[0];
		max = 255 - dark_thresh;
		if(0 > max)
		{
			max = 30;
		}
		if(255 < max)
		{
			max = 255;
		}
		threshold(ImageReducedSROut,&DarkSROut,HTuple(0),HTuple(max));
		
		closing_circle(DarkSROut, &DarkSROut, 10.5);
		connection(DarkSROut, &DarkSROut);
		//fill_up(DarkSROut, &DarkSROut);
		
		select_shape(DarkSROut, &DarkSROut, HTuple("area"), HTuple("and"), HTuple(min_area_sr), HTuple("max"));
		select_shape(DarkSROut, &DarkSROut, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width_sr).Concat(min_height_sr), HTuple("max").Concat("max"));
		select_shape(DarkSROut, &DarkSROut, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));
		
		//경계값과 비교
		count_obj(DarkSROut, &Num);
		
		concat_obj(RsltSRRegion, DarkSROut, &RsltSRRegion);
		num = (Hlong)Num[0];
		
		for(i = 0; i < num; i++)
		{
			select_obj(DarkSROut, &SelectRgn, i + 1);
			dilation_circle(SelectRgn, &OutRgn, 7.5);
			dilation_circle(SelectRgn, &IntRgn, 2.5);
			difference(OutRgn, IntRgn, &OutRgn);
			intensity(SelectRgn, ImageReducedSROut, &Mean, _);
			intensity(OutRgn, ImageReducedSROut, &OutMean, _);
			intensity(SelectRgn, ImageLight, &LightMean, _);
			diff_var = ((Hlong)OutMean[0]) - ((Hlong)Mean[0]);
			circularity(SelectRgn, &CircleVar);		
			//크기가 크면 섬세하게 검사
			area_center(SelectRgn, &Area, _, _);
			
			if(300 < (Hlong)Area[0])
			{
				//밝기
				if(0 < diff_var)
				{
					//Dark(+)
					diff_var = abs(diff_var);
					if(dark_bright_tol < (diff_var + 10))
					{
						concat_obj(ResultRgn, SelectRgn, &ResultRgn);
					}
				}
			}
			else
			{
				//여기서만 백색 이물 제거 필요(강조를 하다 보니 흰색은 흐리게 나오는데 잡힌다.->오버킬)
				//밝기
				if(0 < diff_var)
				{
					//Dark(+)
					
					diff_var = abs(diff_var);
					if(dark_bright_tol < diff_var)
					{
						//진원도가 0.5보다 크면서 백색 이물성이면 오버킬로 간주
						if(0.5 < (double)CircleVar[0] )
						{
							//잉크이미지(크랙이미지)에서 밝값이 밝으면 
							lightvar = (Hlong)LightMean[0];
							if(150 > lightvar)
							{
								concat_obj(ResultRgn, SelectRgn, &ResultRgn);
							}
						}
						else
						{
							concat_obj(ResultRgn, SelectRgn, &ResultRgn);
						}
					}
				}
			}
			
			
			
		}
	}
	//
	//
	/////////////////////////////////////////////
	copy_obj(RsltSRRegion, rslt_sr_region, 1, -1);
	copy_obj(RsltNotSRRegion, rslt_notsr_region, 1, -1);
	copy_obj(RsltUserRegion, rslt_user_region, 1, -1);
	copy_obj(RsltSROutRegion, rslt_sr_out_region, 1, -1);

	union1(ResultRgn, &ResultRgn);
	connection(ResultRgn, &ResultRgn);
	count_obj(ResultRgn, &Num);
	*rslt_num = (Hlong)Num[0];

	if(0 < *rslt_num)
	{

		copy_obj(ResultRgn, rslt_region, 1, -1);
		
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(*rslt_region, win);
		}

		return FALSE;
	}

	return TRUE;
}

*/

//CHOKI20100708
BOOL C_VisionModule::_Hal_FindChip(Hlong win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region,
											 Hlong min_area, Hlong min_width, Hlong min_height)
{
	HTuple Width, Height, Row1, Column1, Row2, Column2, Num, Row11, Column11, Row21, Column21;
	HTuple RowBeginT, ColBeginT, RowEndT, ColEndT, Nr, Nc, Dist, RowBeginB, ColBeginB;
	HTuple RowEndB, ColEndB, RowBeginL, ColBeginL, RowEndL, ColEndL, RowBeginR, ColBeginR, RowEndR, ColEndR;
	HTuple RowTL, ColumnTL, IsParallel, RowTR, ColumnTR, RowBL, ColumnBL, RowBR, ColumnBR;

	Hobject Region, ImageReduced, RegionClosing, RegionFillUp, ConnectedRegions, SelectedRegions,SelectedRegions1, RegionUnion;
	Hobject RegionClosing1, RegionGrid, Rectangle1, RegionIntersection, RegionIntersectionVert, Contour;
	Hobject RegionIntersectionHorz, RegionT, RegionB, RegionL, RegionR, RegionTot, ChipRegion;

	long width_check;
	long height_check;

	//Hlong min;
	HTuple Max, ChipMean;
	CRect rect;

	Hobject RsltChipRgn;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &Width, &Height);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_33!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	//Find suitable threshold
	Hlong min;
	Hobject RegionCenter;
	CRect cent_region;
	CPoint cent_p;
	cent_region = ins_region;
	cent_p = cent_region.CenterPoint();
	gen_rectangle1(&RegionCenter, cent_p.y - 100, cent_p.x - 10, cent_p.y + 100, cent_p.x + 10);

	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(RegionCenter, win);
	}
	min_max_gray(RegionCenter, *image, HTuple(0), _, &Max, _);
	min = (Hlong)Max[0];
	min = min - 55;
	if( (0 > min) || (255 < min) )
	{
	min = 180;
	}

	reduce_domain(*image,Region,&ImageReduced);

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple((Hlong)min),HTuple(255)); //20110530 lee
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_55!... %ld", (Hlong)ChipMean[0]);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	connection(Region,&ConnectedRegions);

	//20110528 lee
	erosion_rectangle1(ConnectedRegions, &ConnectedRegions,5,5); //칩 외각으로 함께 찾아진 SR영역을 분리 

	//20100713 lee 10000->5000
	select_shape(ConnectedRegions,&SelectedRegions1,HTuple("area"),HTuple("and"),HTuple(1000),HTuple("max"));

	//Halcon version error

	//20110528 lee
	dilation_rectangle1(SelectedRegions1, &SelectedRegions1,5,5);//칩 외각으로 함께 찾아진 SR영역을 분리한 만큼 다시 복원 

	union1(SelectedRegions1,&RegionUnion);

	closing_circle(RegionUnion,&RegionClosing1,HTuple(17.5));

	//20100107
	shape_trans(RegionClosing1,&RegionClosing1,HTuple("convex"));

	//Vertical///////////////////////////////////
	gen_grid_region(&RegionGrid,Height,HTuple(2),HTuple("lines"),Width,Height);  

	smallest_rectangle1(RegionClosing1,&Row1,&Column1,&Row2,&Column2);

	count_obj(RegionClosing1, &Num);
	if(1 > (Hlong)Num[0])
	{
		return FALSE;
	}

	rect.top = (Hlong)Row1[0];
	rect.left = (Hlong)Column1[0];
	rect.bottom = (Hlong)Row2[0];
	rect.right = (Hlong)Column2[0];
	rect.top = rect.top;// - 50; //CHOKI20110531
	rect.left = rect.left;// + 50;
	rect.bottom = rect.bottom;// + 50;
	rect.right = rect.right;// - 50;

	if( (0 > rect.top) || ((Hlong)Height[0] < rect.top) || (0 > rect.left) || ((Hlong)Width[0] < rect.left) ||
		(0 > rect.bottom) || ((Hlong)Height[0] < rect.bottom) || (0 > rect.right) || ((Hlong)Width[0] < rect.right) )
	{
		return FALSE;
	}

	rect.NormalizeRect();

	gen_rectangle1(&Rectangle1,rect.top, rect.left + 8, rect.bottom, rect.right - 8);
	intersection(RegionGrid,Rectangle1,&RegionIntersection);
	intersection(RegionIntersection,RegionClosing1,&RegionIntersectionVert);
	connection(RegionIntersectionVert,&RegionIntersectionVert);
	select_shape(RegionIntersectionVert,&RegionIntersectionVert,HTuple("area"),HTuple("and"),HTuple(2),HTuple("max"));

	count_obj(RegionIntersectionVert, &Num);
	if(1 > (Hlong)Num[0])
	{
		return FALSE;
	}

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(RegionClosing1, win);
	}

	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(RegionIntersectionVert, win);
	}

	smallest_rectangle1(RegionIntersectionVert,&Row11,&Column11,&Row21,&Column21);

	width_check = (Hlong)Row11.Num();
	if(3 > width_check)
	{
		return FALSE;
	}
	height_check = (Hlong)Column11.Num();
	if(3 > height_check)
	{
		return FALSE;
	}

	gen_contour_polygon_xld(&Contour,Row11,Column11);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginT,&ColBeginT,&RowEndT,&ColEndT,&Nr,&Nc,&Dist);

	gen_contour_polygon_xld(&Contour,Row21,Column21);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginB,&ColBeginB,&RowEndB,&ColEndB,&Nr,&Nc,&Dist);

	//Horizental/////////////////////////////////
	gen_grid_region(&RegionGrid,HTuple(2),Width,HTuple("lines"),Width,Height); 
	smallest_rectangle1(RegionClosing1,&Row1,&Column1,&Row2,&Column2);

	count_obj(RegionClosing1, &Num);
	if(1 > (Hlong)Num[0])
	{
		return FALSE;
	}

	gen_rectangle1(&Rectangle1,Row1 + 10,Column1,Row2 - 10,Column2); 

	intersection(RegionGrid,Rectangle1,&RegionIntersection);
	intersection(RegionIntersection,RegionClosing1,&RegionIntersectionHorz);
	connection(RegionIntersection,&RegionIntersectionHorz);
	select_shape(RegionIntersectionHorz,&RegionIntersectionHorz,HTuple("area"),HTuple("and"),HTuple(5),HTuple("max"));

	if(win)
	{
		set_color(win, HTuple("cyan"));
		disp_obj(RegionClosing1, win);
	}

	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(RegionIntersectionHorz, win);
	}

	smallest_rectangle1(RegionIntersectionHorz,&Row11,&Column11,&Row21,&Column21);

	width_check = (Hlong)Row11.Num();
	if(3 > width_check)
	{
		return FALSE;
	}
	height_check = (Hlong)Column11.Num();
	if(3 > height_check)
	{
		return FALSE;
	}

	gen_contour_polygon_xld(&Contour,Row11,Column11);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginL,&ColBeginL,&RowEndL,&ColEndL,&Nr,&Nc,&Dist);

	gen_contour_polygon_xld(&Contour,Row21,Column21);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginR,&ColBeginR,&RowEndR,&ColEndR,&Nr,&Nc,&Dist);

	intersection_ll(RowBeginT,ColBeginT,RowEndT,ColEndT,RowBeginL,ColBeginL,RowEndL,ColEndL,&RowTL,&ColumnTL,&IsParallel);
	intersection_ll(RowBeginT,ColBeginT,RowEndT,ColEndT,RowBeginR,ColBeginR,RowEndR,ColEndR,&RowTR,&ColumnTR,&IsParallel);
	intersection_ll(RowBeginB,ColBeginB,RowEndB,ColEndB,RowBeginL,ColBeginL,RowEndL,ColEndL,&RowBL,&ColumnBL,&IsParallel);
	intersection_ll(RowBeginB,ColBeginB,RowEndB,ColEndB,RowBeginR,ColBeginR,RowEndR,ColEndR,&RowBR,&ColumnBR,&IsParallel);

	if( (0 >= (Hlong)RowTL[0]) || (0 >= (Hlong)ColumnTL[0]) || (0 >= (Hlong)RowTR[0]) || (0 >= (Hlong)ColumnTR[0]) )
	{
		return FALSE;
	}

	if( (0 >= (Hlong)RowBL[0]) || (0 >= (Hlong)ColumnBL[0]) || (0 >= (Hlong)RowBR[0]) || (0 >= (Hlong)ColumnBR[0]) )
	{
		return FALSE;
	}

	if( (((Hlong)Height[0]) <= (Hlong)RowTL[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnTL[0]) || (((Hlong)Height[0]) <= (Hlong)RowTR[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnTR[0]) )
	{
		return FALSE;
	}

	if( (((Hlong)Height[0]) <= (Hlong)RowBL[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnBL[0]) || (((Hlong)Height[0]) <= (Hlong)RowBR[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnBR[0]) )
	{
		return FALSE;
	}

	gen_region_line(&RegionT,RowTL,ColumnTL,RowTR,ColumnTR);
	gen_region_line(&RegionB,RowBL,ColumnBL,RowBR,ColumnBR);
	gen_region_line(&RegionL,RowTL,ColumnTL,RowBL,ColumnBL);
	gen_region_line(&RegionR,RowTR,ColumnTR,RowBR,ColumnBR);
	
	if(win)
	{
		set_color(win, HTuple("orange"));
		disp_obj(RegionT, win);
		disp_obj(RegionB, win);
		disp_obj(RegionL, win);
		disp_obj(RegionR, win);
	}

	union2(RegionT,RegionB,&RegionTot);
	union2(RegionL,RegionTot,&RegionTot);
	union2(RegionR,RegionTot,&RegionTot);
	shape_trans(RegionTot,&RsltChipRgn,HTuple("convex"));//CHOKI20110509
	HTuple AreaChip;
	area_center(RsltChipRgn, &AreaChip, _, _);

	//erosion_circle(RegionTot, &RsltChipRgn, HTuple(1.5));

	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(RsltChipRgn, win);
	}


	//lSS
	//shape_trans(RsltChipRgn, &RsltChipRgn, "rectangle2");

	copy_obj(RsltChipRgn, rslt_chip_region, 1, -1);
	
	return TRUE;
}

//90변형전 
/*
//CHOKI20100708
BOOL C_VisionModule::_Hal_FindChip(long win, Hobject *image, RECT ins_region, Hobject *rslt_chip_region,
			  long min_area, long min_width, long min_height)
{
	HTuple Width, Height, Row1, Column1, Row2, Column2, Num, Row11, Column11, Row21, Column21;
	HTuple RowBeginT, ColBeginT, RowEndT, ColEndT, Nr, Nc, Dist, RowBeginB, ColBeginB;
	HTuple RowEndB, ColEndB, RowBeginL, ColBeginL, RowEndL, ColEndL, RowBeginR, ColBeginR, RowEndR, ColEndR;
	HTuple RowTL, ColumnTL, IsParallel, RowTR, ColumnTR, RowBL, ColumnBL, RowBR, ColumnBR;

	Hobject Region, ImageReduced, RegionClosing, RegionFillUp, ConnectedRegions, SelectedRegions,SelectedRegions1, RegionUnion;
	Hobject RegionClosing1, RegionGrid, Rectangle1, RegionIntersection, RegionIntersectionVert, Contour;
	Hobject RegionIntersectionHorz, RegionT, RegionB, RegionL, RegionR, RegionTot, ChipRegion;

	//long min;
	HTuple Max, ChipMean;
	CRect rect;

	Hobject RsltChipRgn;
	
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*image, NULL, NULL, &Width, &Height);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_33!");
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}

	gen_rectangle1(&Region, ins_region.top, 
		                    ins_region.left, 
							ins_region.bottom, 
							ins_region.right);

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(Region, win);
	}

	//Find suitable threshold
	long min;
	Hobject RegionCenter;
	CRect cent_region;
	CPoint cent_p;
	cent_region = ins_region;
	cent_p = cent_region.CenterPoint();
	gen_rectangle1(&RegionCenter, cent_p.y - 10, cent_p.x - 100, cent_p.y + 10, cent_p.x + 100);

	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(RegionCenter, win);
	}
	min_max_gray(RegionCenter, *image, HTuple(0), _, &Max, _);
	min = (Hlong)Max[0];
	min = min - 40;
	if( (0 > min) || (255 < min) )
	{
	min = 180;
	}

	reduce_domain(*image,Region,&ImageReduced);

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,HTuple((Hlong)min),HTuple(255)); //20110530 lee
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_55!... %ld", (Hlong)ChipMean[0]);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}
	
	connection(Region,&ConnectedRegions);

	//20110528 lee
	erosion_rectangle1(ConnectedRegions, &ConnectedRegions,5,5); //칩 외각으로 함께 찾아진 SR영역을 분리 

	//20100713 lee 10000->5000
	select_shape(ConnectedRegions,&SelectedRegions1,HTuple("area"),HTuple("and"),HTuple(1000),HTuple("max"));

	//Halcon version error
	//select_shape(SelectedRegions,&SelectedRegions1,HTuple("rectangularity"),HTuple("and"),HTuple(0.3),HTuple("max")); //20110605 lee

	//20110528 lee
	dilation_rectangle1(SelectedRegions1, &SelectedRegions1,5,5);//칩 외각으로 함께 찾아진 SR영역을 분리한 만큼 다시 복원 

	union1(SelectedRegions1,&RegionUnion);

	closing_circle(RegionUnion,&RegionClosing1,HTuple(17.5));


	//20100107
	shape_trans(RegionClosing1,&RegionClosing1,HTuple("convex"));

	//Vertical///////////////////////////////////
	gen_grid_region(&RegionGrid,Height,HTuple(2),HTuple("lines"),Width,Height); //CHOKI20110531 5->2

	smallest_rectangle1(RegionClosing1,&Row1,&Column1,&Row2,&Column2);

	count_obj(RegionClosing1, &Num);
	if(1 > (Hlong)Num[0])
	{
		return FALSE;
	}
	
	rect.top = (Hlong)Row1[0];
	rect.left = (Hlong)Column1[0];
	rect.bottom = (Hlong)Row2[0];
	rect.right = (Hlong)Column2[0];
	rect.top = rect.top;// - 50; //CHOKI20110531
	rect.left = rect.left;// + 50;
	rect.bottom = rect.bottom;// + 50;
	rect.right = rect.right;// - 50;

	if( (0 > rect.top) || ((Hlong)Height[0] < rect.top) || (0 > rect.left) || ((Hlong)Width[0] < rect.left) ||
		(0 > rect.bottom) || ((Hlong)Height[0] < rect.bottom) || (0 > rect.right) || ((Hlong)Width[0] < rect.right) )
	{
		return FALSE;
	}

	rect.NormalizeRect();

	gen_rectangle1(&Rectangle1,rect.top, rect.left, rect.bottom, rect.right);
	intersection(RegionGrid,Rectangle1,&RegionIntersection);
	intersection(RegionIntersection,RegionClosing1,&RegionIntersectionVert);
	connection(RegionIntersectionVert,&RegionIntersectionVert);
	select_shape(RegionIntersectionVert,&RegionIntersectionVert,HTuple("area"),HTuple("and"),HTuple(2),HTuple("max"));

	count_obj(RegionIntersectionVert, &Num);
	if(1 > (Hlong)Num[0])
	{
		return FALSE;
	}

	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(RegionClosing1, win);
	}

	if(win)
	{
		set_color(win, HTuple("red"));
		disp_obj(RegionIntersectionVert, win);
	}

	smallest_rectangle1(RegionIntersectionVert,&Row11,&Column11,&Row21,&Column21);

	gen_contour_polygon_xld(&Contour,Row11,Column11);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginT,&ColBeginT,&RowEndT,&ColEndT,&Nr,&Nc,&Dist);

	gen_contour_polygon_xld(&Contour,Row21,Column21);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginB,&ColBeginB,&RowEndB,&ColEndB,&Nr,&Nc,&Dist);
	
	//Horizental/////////////////////////////////
	gen_grid_region(&RegionGrid,HTuple(2),Width,HTuple("lines"),Width,Height); //20100713 lee 3->2
	smallest_rectangle1(RegionClosing1,&Row1,&Column1,&Row2,&Column2);

	count_obj(RegionClosing1, &Num);
	if(1 > (Hlong)Num[0])
	{
		return FALSE;
	}

	gen_rectangle1(&Rectangle1,Row1+1,Column1,Row2-1,Column2);  //20100713 lee 10->3->5 //CHOKI20110531 Offset 0

	intersection(RegionGrid,Rectangle1,&RegionIntersection);
	intersection(RegionIntersection,RegionClosing1,&RegionIntersectionHorz);
	connection(RegionIntersection,&RegionIntersectionHorz);
	select_shape(RegionIntersectionHorz,&RegionIntersectionHorz,HTuple("area"),HTuple("and"),HTuple(5),HTuple("max"));


	if(win)
	{
		set_color(win, HTuple("cyan"));
		disp_obj(RegionClosing1, win);
	}

	if(win)
	{
		set_color(win, HTuple("blue"));
		disp_obj(RegionIntersectionHorz, win);
	}

	smallest_rectangle1(RegionIntersectionHorz,&Row11,&Column11,&Row21,&Column21);

	gen_contour_polygon_xld(&Contour,Row11,Column11);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginL,&ColBeginL,&RowEndL,&ColEndL,&Nr,&Nc,&Dist);

	gen_contour_polygon_xld(&Contour,Row21,Column21);
	fit_line_contour_xld(Contour,HTuple("drop"),HTuple(-1),HTuple(0),HTuple(5),HTuple(1.0),&RowBeginR,&ColBeginR,&RowEndR,&ColEndR,&Nr,&Nc,&Dist);

	intersection_ll(RowBeginT,ColBeginT,RowEndT,ColEndT,RowBeginL,ColBeginL,RowEndL,ColEndL,&RowTL,&ColumnTL,&IsParallel);
	intersection_ll(RowBeginT,ColBeginT,RowEndT,ColEndT,RowBeginR,ColBeginR,RowEndR,ColEndR,&RowTR,&ColumnTR,&IsParallel);
	intersection_ll(RowBeginB,ColBeginB,RowEndB,ColEndB,RowBeginL,ColBeginL,RowEndL,ColEndL,&RowBL,&ColumnBL,&IsParallel);
	intersection_ll(RowBeginB,ColBeginB,RowEndB,ColEndB,RowBeginR,ColBeginR,RowEndR,ColEndR,&RowBR,&ColumnBR,&IsParallel);

	if( (0 >= (Hlong)RowTL[0]) || (0 >= (Hlong)ColumnTL[0]) || (0 >= (Hlong)RowTR[0]) || (0 >= (Hlong)ColumnTR[0]) )
	{
		return FALSE;
	}

	if( (0 >= (Hlong)RowBL[0]) || (0 >= (Hlong)ColumnBL[0]) || (0 >= (Hlong)RowBR[0]) || (0 >= (Hlong)ColumnBR[0]) )
	{
		return FALSE;
	}

	if( (((Hlong)Height[0]) <= (Hlong)RowTL[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnTL[0]) || (((Hlong)Height[0]) <= (Hlong)RowTR[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnTR[0]) )
	{
		return FALSE;
	}

	if( (((Hlong)Height[0]) <= (Hlong)RowBL[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnBL[0]) || (((Hlong)Height[0]) <= (Hlong)RowBR[0]) || (((Hlong)Width[0]) <= (Hlong)ColumnBR[0]) )
	{
		return FALSE;
	}

	gen_region_line(&RegionT,RowTL,ColumnTL,RowTR,ColumnTR);
	gen_region_line(&RegionB,RowBL,ColumnBL,RowBR,ColumnBR);
	gen_region_line(&RegionL,RowTL,ColumnTL,RowBL,ColumnBL);
	gen_region_line(&RegionR,RowTR,ColumnTR,RowBR,ColumnBR);
	
	if(win)
	{
		set_color(win, HTuple("orange"));
		disp_obj(RegionT, win);
		disp_obj(RegionB, win);
		disp_obj(RegionL, win);
		disp_obj(RegionR, win);
	}

	union2(RegionT,RegionB,&RegionTot);
	union2(RegionL,RegionTot,&RegionTot);
	union2(RegionR,RegionTot,&RegionTot);
	shape_trans(RegionTot,&RsltChipRgn,HTuple("convex"));//CHOKI20110509
	HTuple AreaChip;
	area_center(RsltChipRgn, &AreaChip, _, _);

	//erosion_circle(RegionTot, &RsltChipRgn, HTuple(1.5));

	if(win)
	{
		set_color(win, HTuple("green"));
		disp_obj(RsltChipRgn, win);
	}

	//lSS
	//shape_trans(RsltChipRgn, &RsltChipRgn, "rectangle2");

	copy_obj(RsltChipRgn, rslt_chip_region, 1, -1);
	
	return TRUE;
}
*/

//CHOKI20100708
BOOL C_VisionModule::_Hal_ChipPot(Hlong win, Hobject *image, RECT ins_region, Hlong pot_area, int thresh, int under_tol, int over_tol, Hlong *rslt_num, Hlong *rslt_area, Hobject *rslt_region,
			 Hlong min_area, Hlong min_width, Hlong min_height)
{
	Hobject ImageReduced, Region, RegionClosing, InsRegion, RsltRegion;
	HTuple Num;
	Hlong over_area, under_area;

	over_area = pot_area * (over_tol * 0.01);
	under_area = pot_area * (under_tol * 0.01);

	gen_empty_obj(&RsltRegion);

	gen_rectangle1(&InsRegion, ins_region.top, ins_region.left, ins_region.bottom, ins_region.right);

	reduce_domain(*image, InsRegion, &ImageReduced);

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = threshold(ImageReduced,&Region,0,HTuple(thresh));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at threshold_57!... %ld", (Hlong)thresh);
		WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	connection(Region, &Region);
	union1(Region, &Region);

	select_shape(Region, &RsltRegion, HTuple("area"), HTuple("and"), HTuple(1), HTuple("max"));

	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2, Area;
	
	*rslt_area = 0;
	count_obj(RsltRegion, &Num);

	*rslt_num = (Hlong)Num[0];
	if(0 < *rslt_num)
	{
		area_center(RsltRegion, &Area, _, _);
		*rslt_area = (Hlong)Area[0];
		if( (over_area > *rslt_area) && (under_area < *rslt_area))
		{
			if(win)
			{
				set_color(win, HTuple("green"));
				disp_obj(RsltRegion, win);
			}
			*rslt_num = 0;

			copy_obj(RsltRegion, rslt_region, 1, -1);
		}
		else
		{
			if(win)
			{
				set_color(win, HTuple("red"));
				disp_obj(RsltRegion, win);
			}

			copy_obj(RsltRegion, rslt_region, 1, -1);
			return FALSE;
		}
	}
	else
	{
		//Chip region
		copy_obj(InsRegion, rslt_region, 1, -1);

		*rslt_num = 1;
		return FALSE;  
	}

	return TRUE;
}

BOOL C_VisionModule::_Hal_FilmPF(Hlong win, Hobject *image, Hobject *image2, //image : back light이미지->전체찢김검사 , image2 : 좀더 밝은 back light이미지->PF영역에서만 검사
						   RECT ins_region, RECT chip_region, RECT dontcare_region, Hobject pf_region, Hobject vacc_region,
						   double offset_ox, double offset_oy, 
						   double offset_ox2, double offset_oy2, 
						   double offset_rx, double offset_ry,
						   double offset_rx2, double offset_ry2,
						   int thresh, int tolerance, int black_thresh, int black_tolerance,
						   Hlong *rslt_num, Hobject *rslt_region, Hobject *rslt_pf_region,
						   Hlong min_area, Hlong min_width, Hlong min_height,
						   Hlong min_area_sr, Hlong min_width_sr, Hlong min_height_sr) 
{
	BOOL rslt = TRUE;
	Hobject Region, SubImage, ImageReduced, RsltRegion, DontCare, ChipRgn, AffineRegion, InsRgn;
	Hobject ImageReduced2, Region2, InsRgn2, RsltRegion2;
	HTuple Num, HomMat2D;
	double offset_x, offset_y;
	HTuple DiffAng;
	int low_threshold, high_threshold;
	HTuple Mean;

	Hobject RsltPFRegion, RsltRegionTot;
	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltPFRegion);
	gen_empty_obj(&RsltRegionTot);

	*rslt_num = 0;
	Hlong rslt_num_hall, rslt_num_tear;
	rslt_num_hall = 0;
	rslt_num_tear = 0;

	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);

	high_threshold = 255 - thresh;
	low_threshold = high_threshold - 10;
	if(0 > low_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < low_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	else if(0 > high_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < high_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}

	//Offset
	offset_x = offset_rx - offset_ox;
	offset_y = offset_ry - offset_oy;

	gen_rectangle1(&Region, (Hlong)(ins_region.top + offset_y), 
		                    (Hlong)(ins_region.left + offset_x), 
							(Hlong)(ins_region.bottom + offset_y), 
							(Hlong)(ins_region.right + offset_x));

	gen_rectangle1(&ChipRgn, (Hlong)(chip_region.top + offset_y), 
		                    (Hlong)(chip_region.left + offset_x), 
							(Hlong)(chip_region.bottom + offset_y), 
							(Hlong)(chip_region.right + offset_x));

	/*
	gen_rectangle1(&DontCare, (Hlong)(dontcare_region.top + offset_y), 
		                    (Hlong)(dontcare_region.left + offset_x), 
							(Hlong)(dontcare_region.bottom + offset_y), 
							(Hlong)(dontcare_region.right + offset_x));
	*/
	

	difference(Region, ChipRgn, &Region);
	//difference(Region, DontCare, &Region);

	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(pf_region, &AffineRegion, HomMat2D, HTuple("false"));

	//PF for image///////////////////////////////
	//찢어짐 검사
	difference(Region, AffineRegion, &RsltPFRegion);
	erosion_circle(RsltPFRegion, &InsRgn, 7.5);
	erosion_rectangle1(InsRgn, &InsRgn, 1, 3.5);

	difference(InsRgn, vacc_region, &InsRgn);

	reduce_domain(*image, InsRgn, &ImageReduced);

	hysteresis_threshold(ImageReduced, &RsltRegion, HTuple(low_threshold), HTuple(high_threshold), HTuple(30));
	closing_circle(RsltRegion,&RsltRegion,HTuple(3.5));

	/////////////////////////////////////////////////////////
	
	connection(RsltRegion, &RsltRegion);
	select_shape(RsltRegion, &RsltRegion, HTuple("area"), HTuple("and"), HTuple(tolerance),  HTuple("max"));


	Hobject select_object;
	HTuple Row1, Column1, Row2, Column2;

	count_obj(RsltRegion, &Num);

	rslt_num_tear = (Hlong)Num[0];
	if(0 < rslt_num_tear)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			set_draw(win, HTuple("margin"));
			disp_obj(RsltRegion, win);
		}

		rslt = FALSE;
	}

	if(win)
	{
		set_color(win, HTuple("blue"));
		set_draw(win, HTuple("margin"));
		disp_obj(RsltPFRegion, win);
	}

	//PF for image2//////////////////////////////////////
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	//막힘검사

	erosion_circle(AffineRegion, &Region2, 10.5);

	intersection(Region2, Region, &RsltPFRegion);

	reduce_domain(*image2, RsltPFRegion, &ImageReduced2);

	intensity(InsRgn2, *image2, &Mean, _);
	high_threshold = (Hlong)Mean[0] + black_thresh + 20;
	low_threshold = high_threshold - 10;
	if(0 > low_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < low_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	else if(0 > high_threshold)
	{
		low_threshold = 0;
		high_threshold = 10;
	}
	else if(255 < high_threshold)
	{
		low_threshold = 245;
		high_threshold = 255;
	}
	
	threshold(ImageReduced2, &RsltRegion2, 0, low_threshold);
	closing_circle(RsltRegion2, &RsltRegion2, HTuple(3.5));

	/////////////////////////////////////////////////////////
	
	connection(RsltRegion2, &RsltRegion2);
	select_shape(RsltRegion2, &RsltRegion2, HTuple("height"), HTuple("and"), HTuple(black_tolerance),  HTuple("max"));

	if(win)
	{
		set_color(win, HTuple("blue"));
		set_draw(win, HTuple("margin"));
		disp_obj(RsltPFRegion, win);
	}

	count_obj(RsltRegion2, &Num);
	rslt_num_hall = (Hlong)Num[0];
	if(0 < rslt_num_hall)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			set_draw(win, HTuple("margin"));
			disp_obj(RsltRegion2, win);
		}

		rslt = FALSE;
	}
	
	*rslt_num = rslt_num_tear + rslt_num_hall;
	
	concat_obj(RsltRegion, RsltRegion2, &RsltRegionTot);
	copy_obj(RsltRegionTot, rslt_region, 1, -1);
	copy_obj(RsltPFRegion, rslt_pf_region, 1, -1);
	
	return rslt;
}

BOOL C_VisionModule::_Hal_InLead(Hlong win, Hobject *image, Hobject *image2, RECT ins_region, Hobject not_sr_region,	
							Hobject dn_region,
							double offset_ox, double offset_oy, 
							double offset_ox2, double offset_oy2, 
							double offset_rx, double offset_ry,
							double offset_rx2, double offset_ry2,
						   int threshold, double factor, int tolerance, 
						   double* line_width_avg, 
						   Hlong *rslt_num, Hobject *rslt_region,  
						   Hlong *rslt_num2, Hobject *rslt_region2,
						   Hobject *rslt_dn_region,
						   Hlong min_area, Hlong min_width, Hlong min_height)
{

	HTuple   Pointer, Type, Width, Height, WindowHandle, Row1;
	HTuple   Column1, Row2, Column2, Row, Sum, Column, Diff;
	HTuple   Length1, UntilHeight, Start, MeasureHandle0, MeasureHandle1;
	HTuple   RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond;
	HTuple   ColumnEdgeSecond, AmplitudeSecond, IntraDistance;
	HTuple   InterDistance, End, Time;
	Hobject  I, InsRegion, Line, TotLine;
	HTuple	 Distance;
	Hlong idx;
	BOOL rslt = TRUE;
	HTuple HomMat2D;

	HTuple Num;
	Hobject select_object, NotSRRgn;

	get_image_pointer1(*image,&Pointer,&Type,&Width,&Height);
	
	Hlong row = ins_region.top + 1;
	Hlong col = (ins_region.left + ins_region.right) * 0.5;
	Hlong leng1 = (ins_region.right - ins_region.left) * 0.5;
	Hlong height = (ins_region.bottom - ins_region.top) - 1;

	HTuple DiffAng, Area;

	Hobject InterRgn, SelectRgn;

	Hlong i, num;

	Hobject RsltRegion, RsltRegion2, RsltDNRegion;

	gen_empty_obj(&RsltRegion);
	gen_empty_obj(&RsltRegion2);
	gen_empty_obj(&RsltDNRegion);
	
	angle_ll(offset_oy, offset_ox, offset_oy2, offset_ox2, offset_ry, offset_rx, offset_ry2, offset_rx2, &DiffAng);
	
	vector_angle_to_rigid(offset_oy, offset_ox, HTuple(0), offset_ry, offset_rx, DiffAng, &HomMat2D);
	affine_trans_region(dn_region, &RsltDNRegion, HomMat2D, HTuple("false"));
	
	if(0.4 > factor)
	{
		factor = 0.4;
	}
	
	gen_rectangle1(&InsRegion, ins_region.top, ins_region.left, ins_region.bottom, ins_region.right);
	
	if(win)
	{
		set_color(win, HTuple("yellow"));
		disp_obj(InsRegion, win);
	}

	gen_empty_obj(&TotLine);
	Hlong line_width = 0;
	Hlong line_idx = 0;
	double dist = 0;

	//Image//////////////////////////////////////
	gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
	gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);

	for(idx=1; idx<=height; idx+=1)
	{
		translate_measure(MeasureHandle0, row + idx, col);
		//gen_measure_rectangle2(row,col + idx,HTuple(1.57079632679),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		measure_pos(*image, MeasureHandle0, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		translate_measure(MeasureHandle1, row + idx, col);
		//gen_measure_rectangle2(row,col + idx,HTuple(-1.57079632679),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
		measure_pos(*image, MeasureHandle1, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);
			
			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("green"));
				disp_obj(Line, win);
			}
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("blue"));
				disp_obj(Line, win);
			}
		}
	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);

	connection(TotLine, &TotLine);

	dilation_rectangle1(RsltDNRegion, &RsltDNRegion, 1.5, 1.5);
	intersection(TotLine, RsltDNRegion, &TotLine);

	//이진화 방식으로 얻은 영역과 에지측정으로 얻은 영역의 교집합이 존재하는 조건

	/*
	count_obj(TotLine, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(TotLine, &TotLine);
		intersection(not_sr_region, RsltDNRegion, &NotSRRgn);
		connection(NotSRRgn, &NotSRRgn);
		count_obj(NotSRRgn, &Num);
		gen_empty_obj(&InterRgn);
		num = (Hlong)Num[0];
		for(i = 0; i < num; i++)
		{
			select_obj(NotSRRgn, &SelectRgn, i + 1);
			intersection(TotLine, SelectRgn, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			if(1 < (Hlong)Area[0])
			{
				union2(TotLine, NotSRRgn, &TotLine);
			}
		}
	}
	*/

	union1(TotLine, &TotLine);
	//union2(TotLine, DynRegion, &TotLine);

	closing_circle(TotLine, &TotLine, HTuple(15.5));
	//closing_rectangle1(TotLine, &TotLine, 1, 25.5);

	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(TotLine, &RsltRegion, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));


	//Image2/////////////////////////////////////
	gen_empty_obj(&Line);
	gen_empty_obj(&TotLine);

	gen_measure_rectangle2(row,col,HTuple(0),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
	gen_measure_rectangle2(row,col,HTuple(PI),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);

	for(idx=1; idx<=height; idx+=1)
	{
		translate_measure(MeasureHandle0, row + idx, col);
		//gen_measure_rectangle2(row,col + idx,HTuple(1.57079632679),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle0);
		measure_pos(*image2, MeasureHandle0, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeFirst,&ColumnEdgeFirst,&AmplitudeFirst,&InterDistance);
		
		translate_measure(MeasureHandle1, row + idx, col);
		//gen_measure_rectangle2(row,col + idx,HTuple(-1.57079632679),leng1,HTuple(1),Width,Height,HTuple("nearest_neighbor"),&MeasureHandle1);
		measure_pos(*image2, MeasureHandle1, HTuple(factor), HTuple(threshold), HTuple("all"), HTuple("all"),
			            &RowEdgeSecond,&ColumnEdgeSecond,&AmplitudeFirst,&InterDistance);

		if(0 < RowEdgeFirst.Num())
		{
			gen_region_points(&Line, RowEdgeFirst,ColumnEdgeFirst);
			
			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("green"));
				disp_obj(Line, win);
			}
		}

		if(0 < RowEdgeSecond.Num())
		{
			gen_region_points(&Line, RowEdgeSecond,ColumnEdgeSecond);

			//concat_obj(TotLine, Line, &TotLine);
			union2(TotLine, Line, &TotLine);

			if(win)
			{
				set_color(win, HTuple("blue"));
				disp_obj(Line, win);
			}
		}
	}

	close_measure(MeasureHandle0);
	close_measure(MeasureHandle1);

	connection(TotLine, &TotLine);

	intersection(TotLine, RsltDNRegion, &TotLine);

	/*
	//이진화 방식으로 얻은 영역과 에지측정으로 얻은 영역의 교집합이 존재하는 조건
	count_obj(TotLine, &Num);
	num = (Hlong)Num[0];
	if(0 < num)
	{
		union1(TotLine, &TotLine);
		intersection(not_sr_region, RsltDNRegion, &NotSRRgn);
		connection(NotSRRgn, &NotSRRgn);
		count_obj(NotSRRgn, &Num);
		gen_empty_obj(&InterRgn);
		num = (Hlong)Num[0];
		for(i = 0; i < num; i++)
		{
			select_obj(NotSRRgn, &SelectRgn, i + 1);
			intersection(TotLine, SelectRgn, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			if(1 < (Hlong)Area[0])
			{
				union2(TotLine, NotSRRgn, &TotLine);
			}
		}
	}
	*/


	union1(TotLine, &TotLine);
	//union2(TotLine, DynRegion, &TotLine);

	closing_circle(TotLine, &TotLine, HTuple(15.5));
	//closing_rectangle1(TotLine, &TotLine, 1, 25.5);

	connection(TotLine, &TotLine);
	select_shape(TotLine, &TotLine, HTuple("area"), HTuple("and"), HTuple(min_area), HTuple("max"));
	select_shape(TotLine, &TotLine, HTuple("width").Concat("height"), HTuple("and"), HTuple(min_width).Concat(min_height), HTuple("max").Concat("max"));
	select_shape(TotLine, &RsltRegion2, HTuple("width").Concat("height"), HTuple("or"), HTuple(tolerance).Concat(tolerance), HTuple("max").Concat("max"));


	//Image//////////////////////////////////////
	count_obj(RsltRegion, &Num);

	*rslt_num = (Hlong)Num[0];

	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion, win);
		}

		rslt = FALSE;
	}
	copy_obj(RsltRegion, rslt_region, 1, -1);

	//Image2/////////////////////////////////////
	count_obj(RsltRegion2, &Num);

	*rslt_num2 = (Hlong)Num[0];

	if(0 < *rslt_num)
	{
		if(win)
		{
			set_color(win, HTuple("red"));
			disp_obj(RsltRegion2, win);
		}

		rslt = FALSE;
	}
	copy_obj(RsltRegion2, rslt_region2, 1, -1);

	copy_obj(RsltDNRegion, rslt_dn_region, 1, -1);

	close_all_measures();

	return rslt;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 110720 ytlee
void C_VisionModule::WriteErrorList(LPCTSTR lpszItem)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);
	sprintf(temp, "[%02d:%02d:%02d]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpszItem);

	memset(name, NULL, sizeof(temp));
	sprintf(name, "Error_Log_%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	memset(path, NULL, MAX_PATH);
	sprintf(path, "E:\\Insp_Result_%d\\%04d%02d%02d\\SYSTEM_LOG\\", GetVisIndex(), lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteErrorStatus(path, name, "Error_Log.bak", temp, 0);
}

void C_VisionModule::WriteNotAgreeInspParam(CString ChangeParamName, int notagreepara) //CHOKI20120326
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	CString Dir, strFile;

	memset(temp, NULL, DAT_STR_LENG);
	sprintf(temp, "[%02d:%02d:%02d] : %s -> %d", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, ChangeParamName, notagreepara);

	memset(path, NULL, MAX_PATH);
	sprintf(path, "D:\\DATABASE\\LOG\\%04d%02d%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);
	
	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteErrorStatus(path, "InspParamChageLog.dat", "InspParamChageLog.bak", temp, 1000);
}

BOOL C_VisionModule::WriteErrorStatus(CString path, CString file, CString bak_file, CString log, long limit_line)
{
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	sprintf(strFile, "%s\\%s", path, file); 

	if((stream = fopen( strFile, "a+" )) == NULL)
	{
		HANDLE fd = CreateFile( strFile,
							GENERIC_READ|GENERIC_WRITE,
							//GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
		stream = fopen( strFile, "a+" );
		if(stream == NULL)
		{
			//20150206 by IMSI
			//AfxMessageBox("Error : log file open fail!(WriteErrorStatus)" );
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while(fgets(buffer, DAT_STR_LENG, stream)) 
	{
		line++;
		if(1 < line)
		{
			break;
		}
	}

    result = fseek(stream, 0L, SEEK_END);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%s", log);
    fputs(szTime, stream);
	fputs("\n", stream);

    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	//Check limit line
	line = 0;
	if(limit_line)
	{
		while(fgets(buffer, DAT_STR_LENG, stream)) line++;
		if(line > limit_line)
		{
			sprintf(strToFile, "%s\\%s", path, bak_file); 
			CopyFile(strFile, strToFile, FALSE);
			fclose(stream);
			DeleteFile(strFile);
		}
		else
		{
			fclose(stream);
		}
	}
	else
	{
		fclose(stream);
	}

	return TRUE;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	110723 ytlee 체크 항목 모델별로 로드 위치 이동		
BOOL C_VisionModule::LoadInspectSelectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;
	CString file_path, pat_path, extention, master_path;
	
	file_path = path + "\\InspectionSel.par";
	
	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			switch(i)
			{
			case 0:
				for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Sys%d", i, j);
					m_VDat[i]->m_SysPara.m_bPara[j] = GetPrivateProfileInt("INSPECTION_SELECT_PARAMETER", key, 0, file_path);
					

				}
			}
		}
	}
	fd=0;
	
	return rslt;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 110723 ytlee 체크 항목 모델별로 저장
BOOL C_VisionModule::SaveInspectSelectPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat;
	int i, j;
	CString file_path, pat_path, extention, master_path;
	
	//Make director
	MakeDirectories(path);
	
	file_path = path + "\\InspectionSel.par";
	
	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		
		//write INI
		
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			
			switch(i)
			{
			case 0:
				for(j = 0; j < BOOL_PARA_MAX_NUM_SYS; j++)
				{
					key.Empty();
					key.Format("CAM%d_bPara_Sys%d", i, j);
					dat.Empty();
					dat.Format("%d", m_VDat[i]->m_SysPara.m_bPara[j]);
					WritePrivateProfileString("INSPECTION_SELECT_PARAMETER", key, dat, file_path);
				}
			}
		}
		
	}
	fd=0;
	
	return rslt;
}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	110801 ytlee
BOOL C_VisionModule::CheckInspectInitPara(CString path)
{
	BOOL rslt = TRUE;
	CString key, dat, add_str, file_path, initname;
	char temp[256];
	int i, j, initval;
	double initval2;

	file_path = path + "\\InspectionInit.par";
	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		//write INI
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			switch(i)
			{
			case 0:
				//Align//////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Align%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(!(m_VDat[i]->m_InsPara.m_iParaAlign[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Align%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

						add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
						if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
						{
							m_VDat[i]->m_InsPara.m_iParaAlign[j] = initval;
						}
						else
						{
							WriteNotAgreeInspParam(key, m_VDat[i]->m_InsPara.m_iParaAlign[j]); //CHOKI20120326
						}
					}
				}
				//Chip///////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Chip%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j == 4 || j == 11 || j == 12)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaChip[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Chip%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

						add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
						if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
						{
							m_VDat[i]->m_InsPara.m_iParaChip[j] = initval;
						}
						else
						{
							WriteNotAgreeInspParam(key, m_VDat[i]->m_InsPara.m_iParaChip[j]); //CHOKI20120326
						}
					}
				}
				//Lead///////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Lead%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j ==3)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaLead[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Lead%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

						add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
						if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
						{
							m_VDat[i]->m_InsPara.m_iParaLead[j] = initval;
						}
						else
						{
							WriteNotAgreeInspParam(key, m_VDat[i]->m_InsPara.m_iParaLead[j]); //CHOKI20120326
						}
					}
				}
				for(j = 0; j < DBL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Lead%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					initval2 = atof(temp);

					if(!(m_VDat[i]->m_InsPara.m_dParaLead[j] == initval2))
					{
						key.Empty();
						key.Format("CAM%d_cdPara_Lead%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

						add_str.Format("[%s] 값이 초기값(%f)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval2);
						if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
						{
							m_VDat[i]->m_InsPara.m_dParaLead[j] = initval2;
						}
						else
						{
							WriteNotAgreeInspParam(key, m_VDat[i]->m_InsPara.m_dParaLead[j]); //CHOKI20120326
						}
					}
				}
				//Film///////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Film%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j == 4)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaFilm[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Film%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

						add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
						if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
						{
							m_VDat[i]->m_InsPara.m_iParaFilm[j] = initval;
						}
						else
						{
							WriteNotAgreeInspParam(key, m_VDat[i]->m_InsPara.m_iParaFilm[j]); //CHOKI20120326
						}
					}
				}
				//OCV////////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_OCV%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j > 1)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaOCV[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_OCV%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

						add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
						if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
						{
							m_VDat[i]->m_InsPara.m_iParaOCV[j] = initval;
						}
						else
						{
							WriteNotAgreeInspParam(key, m_VDat[i]->m_InsPara.m_iParaOCV[j]); //CHOKI20120326
						}
					}
				}
				//PPunch(펀칭부)/////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_PPUNCH%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(!(m_VDat[i]->m_InsPara.m_iParaPPunch[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_PPUNCH%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

						add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
						if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
						{
							m_VDat[i]->m_InsPara.m_iParaPPunch[j] = initval;
						}
						else
						{
							WriteNotAgreeInspParam(key, m_VDat[i]->m_InsPara.m_iParaPPunch[j]); //CHOKI20120326
						}
					}
				}
			}
		}
	}
	fd=0;

	return rslt;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>	
BOOL C_VisionModule::CheckInspectInitPara2(CString path) //CHOKI20120326
{
	BOOL rslt = TRUE;
	CString key, dat, add_str, file_path, initname;
	char temp[256];
	int i, j, initval;
	double initval2;

	file_path = path + "\\InspectionInit.par";
	//file 존재 확인 
	HANDLE fd=CreateFile(   file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		rslt = FALSE;
	}
	else
	{
		CloseHandle(fd);
		//write INI
		for(i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			switch(i)
			{
			case 0:
				//Align//////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_ALIGN; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Align%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(!(m_VDat[i]->m_InsPara.m_iParaAlign[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Align%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

					//	add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
					//	if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
					//	{
							m_VDat[i]->m_InsPara.m_iParaAlign[j] = initval;
					//	}
					//	else
					//	{
					//	}
					}
				}
				//Chip///////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_CHIP; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Chip%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j == 4 || j == 11 || j == 12)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaChip[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Chip%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

					//	add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
					//	if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
					//	{
							m_VDat[i]->m_InsPara.m_iParaChip[j] = initval;
					//	}
					//	else
					//	{
					//	}
					}
				}
				//Lead///////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Lead%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j ==3)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaLead[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Lead%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

					//	add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
					//	if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
					//	{
							m_VDat[i]->m_InsPara.m_iParaLead[j] = initval;
					//	}
					//	else
					//	{
					//	}
					}
				}
				for(j = 0; j < DBL_PARA_MAX_NUM_LEAD; j++)
				{
					key.Empty();
					key.Format("CAM%d_dPara_Lead%d", i, j);
					GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
					initval2 = atof(temp);

					if(!(m_VDat[i]->m_InsPara.m_dParaLead[j] == initval2))
					{
						key.Empty();
						key.Format("CAM%d_cdPara_Lead%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

					//	add_str.Format("[%s] 값이 초기값(%f)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval2);
					//	if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
					//	{
							m_VDat[i]->m_InsPara.m_dParaLead[j] = initval2;
					//	}
					//	else
					//	{
					//	}
					}
				}
				//Film///////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_FILM; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_Film%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j == 4)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaFilm[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_Film%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

					//	add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
					//	if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
					//	{
							m_VDat[i]->m_InsPara.m_iParaFilm[j] = initval;
					//	}
					//	else
					//	{
					//	}
					}
				}
				//OCV////////////////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_OCV; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_OCV%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(j > 1)
					{
						continue;
					}
					if(!(m_VDat[i]->m_InsPara.m_iParaOCV[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_OCV%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

					//	add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
					//	if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
					//	{
							m_VDat[i]->m_InsPara.m_iParaOCV[j] = initval;
					//	}
					//	else
					//	{
					//	}
					}
				}
				//PPunch(펀칭부)/////////////////////
				for(j = 0; j < INT_PARA_MAX_NUM_PPUNCH; j++)
				{
					key.Empty();
					key.Format("CAM%d_iPara_PPUNCH%d", i, j);
					initval = GetPrivateProfileInt("INSPECTION_PARAMETER", key, 0, file_path);

					if(!(m_VDat[i]->m_InsPara.m_iParaPPunch[j] == initval))
					{
						key.Empty();
						key.Format("CAM%d_cPara_PPUNCH%d", i, j);
						GetPrivateProfileString("INSPECTION_PARAMETER", key, "0.0", temp, sizeof(temp), file_path);
						initname.Format("%s", temp);

					//	add_str.Format("[%s] 값이 초기값(%d)과 일치 하지 않습니다. 변경하시겠습니까?", initname, initval);
					//	if(IDYES == MessageBox(NULL,add_str, "변경", MB_YESNO) )
					//	{
							m_VDat[i]->m_InsPara.m_iParaPPunch[j] = initval;
					//	}
					//	else
					//	{
					//	}
					}
				}
			}
		}
	}
	fd=0;

	return rslt;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<