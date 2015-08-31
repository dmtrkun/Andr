#ifndef setup_h
#define setup_h

#ifdef SETUP_PUB
#define SETUPPUB
#else
#define SETUPPUB extern
#endif

#define UP_DIR		1
#define DOWN_DIR	0
uchar clear_setup(void);
uchar program_sel(void);
uchar setup(void);
uchar timer_menu(uchar);
uchar setup_p1(uchar);
uchar setup_start(uchar);
uchar setup_end(uchar);
uchar setup_dstout(uchar);
uchar setup_test(uchar);

/* globals */
//SETUPPUB VOL_PARA_t vol_para;
SETUPPUB float adj_para(float *param_ptr, uchar holdtstat, uchar dir, float max, float min);		 
SETUPPUB int adj_parai(int *param_ptr, uchar holdtstat, uchar dir, int max, int min);		 

#endif /* setup_h */

