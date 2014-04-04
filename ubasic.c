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
	{"", KW_END} 
};

__STACK __Code = {0,NULL};

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
/*
*
*	BUG: Memory seems to be corrupted, but no segfault. I'll try to fix that soon
*
*/
__TOKEN getNextTok(__CONTEXT *context)
{
	char *codeStr = NULL;
	__TOKEN token = {NULL,0, STOP};
	
	// clear context code string
	_ubas_trimContext(context);
	if(_CODE_SYMBOLE=='\0')
	{
		token.str = NULL;
		printf("\nTOKEN:\n\ttype: STOP 0\n\tstr: %s\n\tlen: %d\n", token.str, token.len);
	}
	else
	{
		_ubas_trimContext(context);
		codeStr = _CODE_STR;
	
	
		if(strchr("+-*^/%=;(),><", _CODE_SYMBOLE))
		{
			token.str = _ubas_string(codeStr, 1);
			token.len = 1;
			token.type = DELIMITER;
			printf("\nTOKEN:\n\ttype: DELIMITER\n\tstr: %s\n\tlen: %d\n", token.str, token.len);
		}
		else if(strchr("1234567890", _CODE_SYMBOLE))
		{
			token.len = 0;
			while(strchr("1234567890x", _CODE_SYMBOLE))
			{
				token.len ++;
				context->code.offset++;
			}
			context->code.offset-=token.len;
	
			token.str = _ubas_string(codeStr, token.len);
			token.type = NUMBER;
			printf("\nTOKEN:\n\ttype: NUMBER\n\tstr: %s\n\tlen: %d\n", token.str, token.len);
		}
		else
		{
			token.len = strcspn(codeStr," \0");
			token.str = _ubas_string(codeStr, token.len);
			token.type = STRING;
			printf("\nTOKEN:\n\ttype: STRING\n\tstr: %s\n\tlen: %d\n", token.str, token.len);
		}
	}
	
	if(token.str == NULL)
	{
		token.len=0;
		token.type = STOP;
//		printf("\nTOKEN:\n\ttype: STOP\n\tstr: %s\n\tlen: %d\n", token.str, token.len);
	}
	context->code.offset += token.len;

	return token;
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
		if(*(context->code.ptr + context->code.offset + 1) !='\0')
		{
			context->code.offset ++;
		}
		else
		{
			_CODE_SYMBOLE = 0;
			return;
		}
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


