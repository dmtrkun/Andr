#define SETUP_PUB

#include	"include.h"
#define SETUP_TOUT_MS	30000

typedef uchar (*func)(uchar);
typedef struct 
{
	const char* title;
	func menu_fnc;
	int tmr_num;
}SET_MENU_SPECS_t;

const SET_MENU_SPECS_t set_menu[] = 
{
	{"TIME ",	setup_p1,		0},
	{"DATE ",	setup_p1,		0},
	{"TMR0 ",	timer_menu,		0},
	{"TMR1 ",	timer_menu,		1},
	{"TMR2 ",	timer_menu,		2},
	{"TMR3 ",	timer_menu,		3},
	{"TMR4 ",	timer_menu,		4},
	{"TMR5 ",	timer_menu,		5},
	{"TMR6 ",	timer_menu,		6},
	{"TMR7 ",	timer_menu,		7},
	{"TMR8 ",	timer_menu,		8},
	{"TMR9 ",	timer_menu,		9},
	{"TEST ",	setup_test,		3},

};

const SET_MENU_SPECS_t tmr_menu[] =
{
	{"START ",	setup_start,	1},
	{"END   ",	setup_end,		1},
	{"CHNL  ",	setup_dstout,	1},
};



float adj_para(float *param_ptr, uchar holdtstat, uchar dir, float max, float min)		 
{
float step;
float param = *param_ptr;
		
	
	if (holdtstat == TRUE)
	{
		if(param < 100.0)
			step = 1.0;
		else 
			step = 10.0;
	}
	else
	{
		if(param < 100.0)
			step = 0.1;
		else 
			step = 1.0;
	}

	if(dir)
		param += step;
	else
		param -= step;
	if ( param > max)
		param = max;
	if ( param < min)
		param = min;
		
	*param_ptr = param;
//	else if(para < 200)
//		step = 1.0;
//	else 
//		step = 10.0;
	return step;
}

int adj_parai(int *param_ptr, uchar holdtstat, uchar dir, int max, int min)		 
{
int step;
int param = *param_ptr;
	
	if (holdtstat == TRUE)
			step = 10;
	else
			step = 1;

	if(dir)
		param += step;
	else
		param -= step;
	if ( param > max)
		param = max;
	if ( param < min)
		param = min;
		
	*param_ptr = param;
	return step;
}

void setup_menu(void);

uchar setup(void)
{
	lcd_clear();
	
	setup_menu();
	return 0;
}

void setup_menu(void)
{
char menu = 0,status =0,disp_stat =1, val;
RTCTime timeval;

	RTclock.timer2 = SETUP_TOUT_MS; //4sec
	RTclock.timer = 1000; //4sec
	while((RTclock.timer2 > 0)&&(status == 0))
	{
		if(disp_stat == 1)
		{
			disp_stat = 0;

			set_cursor (0, 0);
			sprintf(str,"SETUP           ",menu);
			lcd_print ((const unsigned char*)str);

			set_cursor (0, 1);
			if(menu<2)
			{
				sprintf(str,"%02d.%s        ",menu,set_menu[menu].title);
			}
			else
			{
				val = set_menu[menu].tmr_num;
				if(SetupImg.Timer[val].dst_out<6)
					sprintf(str,"%02d.%s >%s",menu,set_menu[menu].title,out_names[SetupImg.Timer[val].dst_out]);
				else
					sprintf(str,"%02d.%s >NONE  ",menu,set_menu[menu].title);
			}

			lcd_print ((const unsigned char*)str);
//			else
//			{
//				val = set_menu[menu].tmr_num;
//				timeval.RTC_Hour = SetupImg.Timer[val].start/3600;
//				timeval.RTC_Min = (SetupImg.Timer[val].start%3600)/60;
//
//				set_cursor (0, 0);
//				sprintf(str,"TMR%d  from %02d:%02d",val, timeval.RTC_Hour,timeval.RTC_Min);
//				lcd_print ((const unsigned char*)str);
//
//				timeval.RTC_Hour = SetupImg.Timer[val].end/3600;
//				timeval.RTC_Min = (SetupImg.Timer[val].start%3600)/60;
//
//				set_cursor (0, 1);
//				if(SetupImg.Timer[val].dst_out<6)
//					sprintf(str,"%s  to %02d:%02d",out_names[SetupImg.Timer[val].dst_out],timeval.RTC_Hour,timeval.RTC_Min);
//				else
//					sprintf(str,"NONE   to %02d:%02d",timeval.RTC_Hour,timeval.RTC_Min);
//				lcd_print ((const unsigned char*)str);
//
//			}

		}

		switch(get_evnt_queue())
		{
			case WE_ENTER:
				set_menu[menu].menu_fnc(set_menu[menu].tmr_num);
				disp_stat = 1;
				break;
//				return 1;
//			case WE_MODE:
//				status = 1;
//				disp_stat = 1;
//				break;
			case WE_UPHLD:
			case WE_UPKEY:
				menu++;
				if(menu >= sizeof(set_menu)/sizeof(SET_MENU_SPECS_t))
					menu = 0;
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
			case WE_DWNHLD:
			case WE_DWNKEY:
				if(menu == 0)
					menu = sizeof(set_menu)/sizeof(SET_MENU_SPECS_t)-1;
				else
					menu--;
//				get_incr_rate(&vol_para.rate, incr_step, 0, max_rate, 0.1);
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
//			case WE_UPDWNREL:
//// 				incr_step = 0;
//				RTclock.timer2 = SETUP_TOUT_MS; //4sec
//				break;
			default:
				break;
		}
	}
}

uchar timer_menu(uchar para)
{
char menu = 0,status =0,disp_stat =1;

	RTclock.timer2 = SETUP_TOUT_MS; //4sec
	RTclock.timer = 1000; //4sec
	while((RTclock.timer2 > 0)&&(status == 0))
	{

		if(disp_stat == 1)
		{
			set_cursor (0, 0);
			sprintf(str,"%02d.SET          ",menu);
			lcd_print ((const unsigned char*)str);					 

			set_cursor (0, 1);
			sprintf(str,"%s          ",tmr_menu[menu].title);
			lcd_print ((const unsigned char*)str);					 
			disp_stat = 0;

		}

		switch(get_evnt_queue())
		{
			case WE_ENTER:
				tmr_menu[menu].menu_fnc(para);
				disp_stat = 1;
				break;
//				return 1;
//			case WE_MODE:
//				status = 1;
//				disp_stat = 1;
//				break;
			case WE_UPHLD:
			case WE_UPKEY:
				menu++;
				if(menu >= sizeof(tmr_menu)/sizeof(SET_MENU_SPECS_t))
					menu = 0;
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
			case WE_DWNHLD:
			case WE_DWNKEY:
				if(menu == 0)
					menu = sizeof(tmr_menu)/sizeof(SET_MENU_SPECS_t)-1;
				else
					menu--;
//				get_incr_rate(&vol_para.rate, incr_step, 0, max_rate, 0.1);		   
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
//			case WE_UPDWNREL:
//// 				incr_step = 0;
//				RTclock.timer2 = SETUP_TOUT_MS; //4sec
//				break;
			default:
				break;
		}
	}
}



uchar setup_p1(uchar para)
{
uchar status = 0, disp_stat = 0, i;
uchar incr_step = 0,val;
	status = 0;
	disp_stat = 1;

	RTclock.timer2 = SETUP_TOUT_MS; //4sec
	RTclock.timer = 1000; //4sec
	while((RTclock.timer2 > 0)&&(status < 4))
	{
		if(disp_stat == 1||(status == 0 && RTclock.timer == 0))
		{
			set_cursor (0, 0);
			sprintf(str,"SETUP TIME      ");
			lcd_print ((const unsigned char*)str);					 

	  	set_cursor (0, 1);
			sprintf(str,"%02d:%02d:%02d      %02d",current_time.RTC_Hour,current_time.RTC_Min,current_time.RTC_Sec,status);
			lcd_print ((const unsigned char*)str);					 
			
			if(status>2)
				set_cursor (7, 1);
			else if(status>1)
				set_cursor (4, 1);
			else if(status>0)
				set_cursor (1, 1);

			if(status != 0)
				lcd_write_cmd (0x0F);                 /* Display ctrl:Disp=ON,Curs/Blnk=ON */
			else
			{
				RTCGetTime(&current_time);
				RTclock.timer = 1000;
			}
			disp_stat = 0;
		}
		switch(get_evnt_queue())
		{
//			case WE_MODE:
//				lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
//				status = 4;
//				break;
			case WE_ENTER:
				status++;
				if(status == 3)
					current_time.RTC_Sec = 0;
				if(status > 3)
				{
					status = 0;
 					RTCSetTime(current_time);
					lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
				}
				disp_stat = 1;
				break;
			case WE_UPHLD:
				incr_step = TRUE;
			case WE_UPKEY:
				if(status == 1)
				{
					if((current_time.RTC_Hour) < 23)			 
						current_time.RTC_Hour++;
        }
				else if(status == 2)
				{
					if((current_time.RTC_Min) < 59)			 
						current_time.RTC_Min++;
        }
				else if(status == 3)
				{
					if((current_time.RTC_Sec) < 59)			 
						current_time.RTC_Sec++;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
			case WE_DWNHLD:
				incr_step = TRUE;
			case WE_DWNKEY:
				if(status == 1)
				{
					if((current_time.RTC_Hour) > 0)			 
						current_time.RTC_Hour--;
        }
				else if(status == 2)
				{
					if((current_time.RTC_Min) > 0)			 
						current_time.RTC_Min--;
        }
				else if(status == 3)
				{
					if((current_time.RTC_Sec) > 0)			 
						current_time.RTC_Sec--;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
//			case WE_UPDWNREL:
// 				incr_step = 0;
//				RTclock.timer2 = SETUP_TOUT_MS; //4sec
//				break;
			default:
				break;
		}
	}
  lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
	return 0;
}

uchar setup_start(uchar para)
{
uchar status = 0, disp_stat = 0, i;
uchar incr_step = 0;
unsigned int val;
unsigned long *src,*dst;
RTCTime timeval;
	
	status = 0;
	disp_stat = 1;

	timeval.RTC_Hour = SetupImg.Timer[para].start/3600;
	val = SetupImg.Timer[para].start%3600 ;
	timeval.RTC_Min = val/60;
	timeval.RTC_Sec = val%60;
	
	RTclock.timer2 = SETUP_TOUT_MS; //4sec
	RTclock.timer = 1000; //4sec
	while((RTclock.timer2 > 0)&&(status < 4))
	{
		if(disp_stat == 1)
		{
			if(status != 0)
				lcd_write_cmd (0x0F);                 /* Display ctrl:Disp=ON,Curs/Blnk=ON */
			
			set_cursor (0, 0);
			sprintf(str,"TIMER1 START    ");
			lcd_print ((const unsigned char*)str);					 

			set_cursor (0, 1);
			sprintf(str,"%02d:%02d:%02d        ",timeval.RTC_Hour,timeval.RTC_Min,timeval.RTC_Sec);
			lcd_print ((const unsigned char*)str);					 
			
			if(status>2)
				set_cursor (7, 1);
			else if(status>1)
				set_cursor (4, 1);
			else if(status>0)
				set_cursor (1, 1);

			disp_stat = 0;
		}
		switch(get_evnt_queue())
		{
//			case WE_MODE:
//				lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
//				status = 4;
//				break;
			case WE_ENTER:
				status++;
				if(status == 3)
					timeval.RTC_Sec = 0;
				if(status > 3)
				{
					SetupImg.Timer[para].start = timeval.RTC_Hour*60*60+ timeval.RTC_Min*60 + timeval.RTC_Sec;
					save_params();
//					/* put checksum to the last halfword */
//					((unsigned short*)&SetupImg)[sizeof(SetupImg_t)/2 - 1] = crc16((unsigned char*)&SetupImg, sizeof(SetupImg_t) - 2);
//					src = (unsigned long*)&SetupImg;
//					dst = (unsigned long*)&BSR_setup_img_t;
//					for(i = 0;i<sizeof(SetupImg_t)/4;i++,dst++,src++)
//						*dst = *src;
////					memcpy(BSR_setup_img_t,SetupImg,sizeof(SetupImg_t));
					status = 0;
			  	lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
				}
				disp_stat = 1;
				break;
			case WE_UPHLD:
				incr_step = TRUE;
			case WE_UPKEY:
				if(status == 1)
				{
					if((timeval.RTC_Hour) < 23)
						timeval.RTC_Hour++;
        }
				else if(status == 2)
				{
					if((timeval.RTC_Min) < 59)
						timeval.RTC_Min++;
        }
				else if(status == 3)
				{
					if((timeval.RTC_Sec) < 59)
						timeval.RTC_Sec++;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
			case WE_DWNHLD:
				incr_step = TRUE;
			case WE_DWNKEY:
				if(status == 1)
				{
					if((timeval.RTC_Hour) > 0)
						timeval.RTC_Hour--;
        }
				else if(status == 2)
				{
					if((timeval.RTC_Min) > 0)
						timeval.RTC_Min--;
        }
				else if(status == 3)
				{
					if((current_time.RTC_Sec) > 0)			 
						current_time.RTC_Sec--;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
//			case WE_UPDWNREL:
// 				incr_step = 0;
//				RTclock.timer2 = SETUP_TOUT_MS; //4sec
//				break;
			default:
				break;
		}
	}
	lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
	return 0;
}
uchar setup_end(uchar para)
{
uchar status = 0, disp_stat = 0, i;
uchar incr_step = 0;
unsigned int val;
unsigned long *src,*dst;
RTCTime timeval;
	
	status = 0;
	disp_stat = 1;

	timeval.RTC_Hour = SetupImg.Timer[para].end/3600;
	val = SetupImg.Timer[para].end%3600 ;
	timeval.RTC_Min = val/60;
	timeval.RTC_Sec = val%60;
	RTclock.timer2 = SETUP_TOUT_MS; //4sec
	RTclock.timer = 1000; //4sec
	while((RTclock.timer2 > 0)&&(status < 4))
	{
		if(disp_stat == 1)
		{
			if(status != 0)
				lcd_write_cmd (0x0F);                 /* Display ctrl:Disp=ON,Curs/Blnk=ON */
			
			set_cursor (0, 0);
			sprintf(str,"TIMER1 END    ");
			lcd_print ((const unsigned char*)str);					 

	  	set_cursor (0, 1);
			sprintf(str,"%02d:%02d:%02d        ",timeval.RTC_Hour,timeval.RTC_Min,timeval.RTC_Sec);
			lcd_print ((const unsigned char*)str);					 
			
			if(status>2)
				set_cursor (7, 1);
			else if(status>1)
				set_cursor (4, 1);
			else if(status>0)
				set_cursor (1, 1);

			disp_stat = 0;
		}
		switch(get_evnt_queue())
		{
//			case WE_MODE:
//				lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
//				status = 4;
//				break;
			case WE_ENTER:
				status++;
				if(status == 3)
					timeval.RTC_Sec = 0;
				if(status > 3)
				{
					SetupImg.Timer[para].end = timeval.RTC_Hour*60*60+ timeval.RTC_Min*60 + timeval.RTC_Sec;
					save_params();
//					/* put checksum to the last halfword */
//					((unsigned short*)&SetupImg)[sizeof(SetupImg_t)/2 - 1] = crc16((unsigned char*)&SetupImg, sizeof(SetupImg_t) - 2);
//					src = (unsigned long*)&SetupImg;
//					dst = (unsigned long*)&BSR_setup_img_t;
//					for(i = 0;i<sizeof(SetupImg_t)/4;i++,dst++,src++)
//						*dst = *src;
////					memcpy(BSR_setup_img_t,SetupImg,sizeof(SetupImg_t));
					status = 0;
			  	lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
				}
				disp_stat = 1;
				break;
			case WE_UPHLD:
				incr_step = TRUE;
			case WE_UPKEY:
				if(status == 1)
				{
					if((timeval.RTC_Hour) < 23)
						timeval.RTC_Hour++;
        }
				else if(status == 2)
				{
					if((timeval.RTC_Min) < 59)
						timeval.RTC_Min++;
        }
				else if(status == 3)
				{
					if((timeval.RTC_Sec) < 59)
						timeval.RTC_Sec++;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
			case WE_DWNHLD:
				incr_step = TRUE;
			case WE_DWNKEY:
				if(status == 1)
				{
					if((timeval.RTC_Hour) > 0)
						timeval.RTC_Hour--;
        }
				else if(status == 2)
				{
					if((timeval.RTC_Min) > 0)
						timeval.RTC_Min--;
        }
				else if(status == 3)
				{
					if((timeval.RTC_Sec) > 0)
						timeval.RTC_Sec--;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
//			case WE_UPDWNREL:
// 				incr_step = 0;
//				RTclock.timer2 = SETUP_TOUT_MS; //4sec
//				break;
			default:
				break;
		}
	}
	lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
	return 0;
}

uchar setup_dstout(uchar para)
{
uchar status = 0, disp_stat = 0, i;
uchar incr_step = 0;
unsigned int val;
unsigned long *src,*dst;
	
	status = 0;
	disp_stat = 1;
	val = SetupImg.Timer[para].dst_out;

	RTclock.timer2 = SETUP_TOUT_MS; //4sec
	RTclock.timer = 1000; //4sec
	while((RTclock.timer2 > 0)&&(status < 2))
	{
		if(disp_stat == 1)
		{
			if(status != 0)
				lcd_write_cmd (0x0F);                 /* Display ctrl:Disp=ON,Curs/Blnk=ON */
			
			set_cursor (0, 0);
			sprintf(str,"TIMER%d CHANNEL  ",para);
			lcd_print ((const unsigned char*)str);					 

			set_cursor (0, 1);
			if ( val > 5)
				sprintf(str,"DISABLED        ");
			else
				sprintf(str,"%s          ",out_names[val]);
			lcd_print ((const unsigned char*)str);					 
			
			if(status != 0)
				set_cursor (1, 1);

			disp_stat = 0;
		}
		switch(get_evnt_queue())
		{
//			case WE_MODE:
//				lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
//				status = 2;
//				break;
			case WE_ENTER:
				status++;
				if(status > 1)
				{
					SetupImg.Timer[para].dst_out = val;
					save_params();
//					/* put checksum to the last halfword */
//					((unsigned short*)&SetupImg)[sizeof(SetupImg_t)/2 - 1] = crc16((unsigned char*)&SetupImg, sizeof(SetupImg_t) - 2);
//					src = (unsigned long*)&SetupImg;
//					dst = (unsigned long*)&BSR_setup_img_t;
//					for(i = 0;i<sizeof(SetupImg_t)/4;i++,dst++,src++)
//						*dst = *src;
////					memcpy(BSR_setup_img_t,SetupImg,sizeof(SetupImg_t));
					status = 0;
			  	lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
				}
				disp_stat = 1;
				break;
			case WE_UPHLD:
				incr_step = TRUE;
			case WE_UPKEY:
				if(status == 1)
				{
					if(val < 7)			 
						val++;
					else
						val=0;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
			case WE_DWNHLD:
				incr_step = TRUE;
			case WE_DWNKEY:
				if(status == 1)
				{
					if(val > 0)			 
						val--;
					else
						val=7;
        }
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
//			case WE_UPDWNREL:
// 				incr_step = 0;
//				RTclock.timer2 = SETUP_TOUT_MS; //4sec
//				break;
			default:
				break;
		}
	}
	lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
	return 0;
}



//SetupImg.Timer[i].start



uchar setup_test(uchar para)
{
uchar status = 0, disp_stat = 0, i;
uchar incr_step = 0;
unsigned int val;
unsigned long *src,*dst;

	status = 0;
	disp_stat = 1;
	val = test_flg;

	RTclock.timer2 = SETUP_TOUT_MS; //4sec
	RTclock.timer = 1000; //4sec
	while((RTclock.timer2 > 0)&&(status < 2))
	{
		if(disp_stat == 1)
		{
			if(status != 0)
				lcd_write_cmd (0x0F);                 /* Display ctrl:Disp=ON,Curs/Blnk=ON */

			set_cursor (0, 0);
			sprintf(str,"TEST MODE       ");
			lcd_print ((const unsigned char*)str);

			set_cursor (0, 1);
			if ( val == 0)
				sprintf(str,"DISABLED        ");
			else
				sprintf(str,"ENABLED         ");
			lcd_print ((const unsigned char*)str);

			if(status != 0)
				set_cursor (1, 1);

			disp_stat = 0;
		}
		switch(get_evnt_queue())
		{
//			case WE_MODE:
//				lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
//				status = 2;
//				break;
			case WE_ENTER:
				status++;
				if(status > 1)
				{
					test_flg = val;
					status = 0;
					lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
				}
				disp_stat = 1;
				break;
			case WE_UPHLD:
				incr_step = TRUE;
			case WE_UPKEY:
				if(status == 1)
				{
					if(val < 1)
						val++;
					else
						val=0;
				}
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
			case WE_DWNHLD:
				incr_step = TRUE;
			case WE_DWNKEY:
				if(status == 1)
				{
					if(val > 0)
						val--;
					else
						val=1;
				}
				disp_stat = 1;
				RTclock.timer2 = SETUP_TOUT_MS; //4sec
				break;
//			case WE_UPDWNREL:
// 				incr_step = 0;
//				RTclock.timer2 = SETUP_TOUT_MS; //4sec
//				break;
			default:
				break;
		}
	}
	lcd_write_cmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
	return 0;
}


