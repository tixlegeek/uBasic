
#ifndef UBAS_STRUCTS
#define UBAS_STRUCTS

	typedef struct __STACK { 
		unsigned int offset;
		unsigned char *ptr;
	} __STACK;
	
	typedef struct __VAR {
		char *name;
		int value;
		struct __VAR *n;
		struct __VAR *p;
	} __VAR;
	
	typedef struct __VARLIST
	{
		size_t len;
		struct __VAR *f;
		struct __VAR *l;
	} __VARLIST;
	
	typedef struct __CONTEXT { 
		__STACK code;
		__VARLIST vars;
		unsigned int bf_watchdog;
		unsigned int bf_abort;
	} __CONTEXT;

	typedef struct __TOKEN {
		char *str;
		int len;
		enum {
			DELIMITER,
			NUMBER,
			STRING,
			KEYWORD,
			EOKW,
			QUOTE,
			BRACKET,
			STOP
		} type ;
		void (*ptr)(__CONTEXT *context, char* returnstr);
	} __TOKEN;
	
	typedef struct __LABEL {
		char name[UBAS_LABEL_LEN];
		__STACK code;
	} __LABEL;
	
	typedef struct encap_error {
		unsigned char simplequote:1;
		unsigned char doublequote:1;
		unsigned char bracket:1;
		unsigned char parenthesis:1;
		unsigned char brace:1;
		unsigned char general:1;	
		unsigned char padding:2;
	} encap_error ;

#endif
