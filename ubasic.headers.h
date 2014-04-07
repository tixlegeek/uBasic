
#ifndef UBAS_HEADERS
#define UBAS_HEADERS

	void KW_PRINT 	(__CONTEXT *context, char* returnstr);
	void KW_INPUT 	(__CONTEXT *context, char* returnstr);
	void KW_IF 		(__CONTEXT *context, char* returnstr);
	void KW_THEN 	(__CONTEXT *context, char* returnstr);
	void KW_GOTO 	(__CONTEXT *context, char* returnstr);
	void KW_FOR 	(__CONTEXT *context, char* returnstr);
	void KW_NEXT 	(__CONTEXT *context, char* returnstr);
	void KW_TO 		(__CONTEXT *context, char* returnstr);
	void KW_GOSUB 	(__CONTEXT *context, char* returnstr);
	void KW_RETURN 	(__CONTEXT *context, char* returnstr);
	void KW_END 	(__CONTEXT *context, char* returnstr);
	
	void *	getContextCodeStr	(__CONTEXT *context);
	int 	getContextCodeOffset(__CONTEXT *context);
	void *	getContextCodePtr	(__CONTEXT *context);

	__TOKEN _ubas_nextToken	(__CONTEXT *context);

	char *	_ubas_string(char* str, int len);
	void 	_ubas_trimContext(__CONTEXT *context);
	void 	_ubas_eval(__CONTEXT *context);
	void (*	_ubas_keywordCallback(__TOKEN *token))(__CONTEXT *context, char* returnstr);
	
	encap_error _ubas_encap(__CONTEXT *context);
		
	void _ubas_usage();
	void _ubas_debug_token(__TOKEN token);
	void _ubas_debug_encap(encap_error encap);


#endif
