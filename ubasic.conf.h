
#ifndef UBAS_CONF
#define UBAS_CONF
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <sys/time.h>
	
	#define CF_BLACK    	"\033[0;30m"
	#define CF_DRED    	"\033[0;31m"
	#define CF_DGREEN   "\033[0;32m"
	#define CF_ORANGE   	"\033[0;33m"
	#define CF_DBLUE    	"\033[0;34m"
	#define CF_PURPLE   	"\033[0;35m"
	#define CF_CYAN	  	"\033[0;36m"
	#define CF_GREY		"\033[0;37m"
	#define CF_DGREY	    "\033[1;30m"
	#define CF_RED		"\033[1;31m"
	#define CF_GREEN	    "\033[1;32m"
	#define CF_YELLOW   	"\033[1;33m"
	#define CF_BLUE		"\033[1;34m"
	#define CF_PINK		"\033[1;35m"
	#define CF_LBLUE   	"\033[1;36m"
	#define CF_WHITE   	"\033[1;37m"
	#define CL_RESET	    "\033[0m"
	
	#define CB_BLACK	    "\033[40m"
	#define CB_RED 	    "\033[41m"
	#define CB_GREEN   "\033[42m"
	#define CB_ORANGE   "\033[43m"
	#define CB_BLUE    "\033[44m"
	#define CB_PURPLE   "\033[45m"
	#define CB_CYAN     "\033[46m"
	#define CB_GREY     "\033[47m"

	#define UBAS_LABEL_LEN		32
	#define UBAS_KEYWORD_LEN		32
	#define UBAS_VARN_LEN		32
#endif
