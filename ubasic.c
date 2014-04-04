/*

			C Basic interpreter 
	
	By Tixlegeek <tixlegeek@whoarehackers.com>

	still under active developpement, so dont be affraid
	of the ugly code behind. I'll format and split it when
	i'll can.
	
	Status:
	
	UNDER ACTIVE DEVELOPPEMENT, DONT TRY TO RUN IT!	

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

#define UBAS_LABEL_LEN		32
#define UBAS_KEYWORD_LEN		32

typedef struct __STACK { 
	unsigned int offset;
	unsigned char *ptr;
} __STACK;
	
typedef struct __CONTEXT { 
	__STACK code;
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
		QUOTE,
		BRACKET,
		STOP
	} type ;
} __TOKEN;

void KW_PRINT (__CONTEXT *context, char* args);
void KW_INPUT (__CONTEXT *context, char* args);
void KW_IF (__CONTEXT *context, char* args);
void KW_THEN (__CONTEXT *context, char* args);
void KW_GOTO (__CONTEXT *context, char* args);
void KW_FOR (__CONTEXT *context, char* args);
void KW_NEXT (__CONTEXT *context, char* args);
void KW_TO (__CONTEXT *context, char* args);
void KW_GOSUB (__CONTEXT *context, char* args);
void KW_RETURN (__CONTEXT *context, char* args);
void KW_END (__CONTEXT *context, char* args);
void *getContextCodeStr(__CONTEXT *context);
int getContextCodeOffset(__CONTEXT *context);
void *getContextCodePtr(__CONTEXT *context);

__TOKEN getNextTok(__CONTEXT *context);
char *_ubas_string(char* str, int len);
void _ubas_trimContext(__CONTEXT *context);
char *trim(char *s);

void int_usage();
void _ubas_eval(__CONTEXT *context);

void _ubas_debug_token(__TOKEN token);

typedef struct __LABEL {
	char name[UBAS_LABEL_LEN];
	__STACK code;
} __LABEL;

struct keywords {
	// keyword string
	char key[UBAS_KEYWORD_LEN];
	// callback
	void (*ptr)(__CONTEXT *context, char* args);
} table[] = {
	{"print", KW_PRINT},
	{"input", KW_INPUT},
	{"if", KW_IF},
	{"then", KW_THEN},
	{"goto", KW_GOTO},
	{"for", KW_FOR},
	{"next", KW_NEXT},
	{"to", KW_TO},
	{"gosub", KW_GOSUB},
	{"return", KW_RETURN},
	{"end", KW_END},
	{NULL, KW_END} 
};

__STACK __Code = {0,NULL};


// Return the code PTR in context
#define _CODE_STR (char*)getContextCodeStr(context)
#define _CODE_SYMBOLE *(context->code.ptr + context->code.offset)
void *getContextCodeStr(__CONTEXT *context){	return (void*)(context->code.ptr + context->code.offset);}

// Return the code offset in context
#define _CODE_OFFSET getContextCodeOffset(context)
int getContextCodeOffset(__CONTEXT *context){	return context->code.offset;}

// Return the code offset in context
#define _CODE_PTR getContextCodePtr(context)
void *getContextCodePtr(__CONTEXT *context){	return (void*)(context->code.ptr);}

int main(int argc, char *argv[])
{
	if(argc==2)
	{
		__Code.ptr=(unsigned char*)argv[1];
		__CONTEXT context = {__Code, 0, 0};
		_ubas_eval(&context);
		return 0;
	}
	else
	{
		int_usage();
		return 1;
	}
}

__TOKEN getNextTok(__CONTEXT *context)
{
	char *codeStr = NULL;
	int offset = 0;
	char bracket;
	__TOKEN token = {NULL,0, STOP};
	
	// clear context code string
	_ubas_trimContext(context);
	if(_CODE_SYMBOLE=='\0')
	{
		token.str = NULL;
	}
	else
	{
		_ubas_trimContext(context);
		codeStr = _CODE_STR;
	
		// Delimiter
		if(strchr("+-*^/%=;(),><", _CODE_SYMBOLE))
		{
			token.str = _ubas_string(codeStr, 1);
			token.len = 1;
			token.type = DELIMITER;
			_ubas_debug_token(token);
			context->code.offset += token.len;	return token;
		}
		
		// QUOTE
		else if(strchr("\"\'", _CODE_SYMBOLE))
		{
			bracket = _CODE_SYMBOLE;
		 	context->code.offset++;
			codeStr = _CODE_STR;
			
			token.len = strchr(codeStr,bracket)-codeStr;
			token.str = _ubas_string(codeStr, token.len);
			token.type = QUOTE;

			_ubas_debug_token(token);
			context->code.offset += token.len+1;	return token;
		}
		
		// BRACKET
		else if(strchr("[{", _CODE_SYMBOLE))
		{
			bracket = _CODE_SYMBOLE;
		 	context->code.offset++;
			codeStr = _CODE_STR;
			
			token.len = strchr(codeStr,bracket+2)-codeStr;
			token.str = _ubas_string(codeStr, token.len);
			token.type = BRACKET;

			_ubas_debug_token(token);
			context->code.offset += token.len+1;	return token;
		}
		
		// Numbers
		else if(strchr("1234567890", _CODE_SYMBOLE))
		{
			token.len = 0;
			while(strchr("1234567890x", *(context->code.ptr + context->code.offset + offset)))
			{
				offset++;
				if(*(context->code.ptr + context->code.offset + offset)==0)
					break;
			}
			token.len = offset;
			//context->code.offset-=token.len;
	
			token.str = _ubas_string(codeStr, token.len);
			token.type = NUMBER;

			_ubas_debug_token(token);
			context->code.offset += token.len;	return token;

		}
		
		// String
		else
		{
			token.len = strcspn(codeStr," +-*^/%=;(),><");
			token.str = _ubas_string(codeStr, token.len);
			token.type = STRING;

			_ubas_debug_token(token);
			context->code.offset += token.len;	return token;
		}
	}
	
	if(token.str == NULL)
	{
		token.len=0;
		token.type = STOP;
//		printf("\nTOKEN:\n\ttype: STOP\n\tstr: %s\n\tlen: %d\n", token.str, token.len);
	}
	context->code.offset += token.len;	return token;
}

void _ubas_eval(__CONTEXT *context)
{
	__TOKEN token;
	printf("Execution de %s\n", _CODE_STR);
	token = getNextTok(context);
	//printf("Prochain token:\"%s\"\n(chaine complete: \"%s\")\n", token, _CODE_STR);
	while(token.type!=STOP)
	{
		//printf("Prochain token:\"%s\"\n", token);
		token = getNextTok(context);
		printf("Stack:%d\n",context->code.offset); 
	}
}


char *_ubas_string(char* str, int len)
{
	char* string = malloc((size_t)(sizeof(char)*(len+1)));
	if(string!=NULL)
	{
		memcpy (string, str, len);
		*(string+len+1) = 0;
		return string;
	}
	return NULL;
}

void int_usage()
{
	printf("Nombre d'arguments invalides.\n\tubasic \"cmd[;cmd]\"\n\n");
}


void KW_PRINT (__CONTEXT *context, char* args)
{

}
void KW_INPUT (__CONTEXT *context, char* args)
{

}
void KW_IF (__CONTEXT *context, char* args)
{

}
void KW_THEN (__CONTEXT *context, char* args)
{

}
void KW_GOTO (__CONTEXT *context, char* args)
{

}
void KW_FOR (__CONTEXT *context, char* args)
{

}
void KW_NEXT (__CONTEXT *context, char* args)
{

}
void KW_TO (__CONTEXT *context, char* args)
{

}
void KW_GOSUB (__CONTEXT *context, char* args)
{

}
void KW_RETURN (__CONTEXT *context, char* args)
{

}
void KW_END (__CONTEXT *context, char* args)
{

}

// Trim the 
void _ubas_trimContext(__CONTEXT *context)
{
	
	while((_CODE_SYMBOLE==' '))
	{
		context->code.offset ++;
	}
}

char *trim(char *s) 
{
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}
/*		DELIMITER,
		NUMBER,
		STRING,
		KEYWORD,
		QUOTE,
		BRACKET,
		STOP*/
void _ubas_debug_token(__TOKEN token)
{
	char *tokentype;
	switch(token.type)
	{
		case STRING:
			tokentype="STRING\0";
		break;
		case NUMBER:
			tokentype="NUMBER\0";
		break;
		case DELIMITER:
			tokentype="DELIMITER\0";
		break;
		case BRACKET:
			tokentype="BRACKET\0";
		break;
		case KEYWORD:
			tokentype="KEYWORD\0";
		break;
		case QUOTE:
			tokentype="QUOTE\0";
		break;
		case STOP:
			tokentype="STOP\0";
		break;
		default:
			tokentype="UNDEFINED\0";
		break;
	
	}
	
	printf("\nTOKEN:\n\ttype: %s\n\tstr: %s\n\tlen: %d\n", tokentype, token.str, token.len);
}


