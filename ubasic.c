/*

			C Basic interpreter 
	
	By Tixlegeek <tixlegeek@whoarehackers.com>

	still under active developpement, so dont be affraid
	of the ugly code behind. I'll format and split it when
	i'll can.
	
	Status:
	
	UNDER ACTIVE DEVELOPPEMENT, DONT TRY TO RUN IT!	

*/



#include "./ubasic.conf.h"
#include "./ubasic.structs.h"
#include "./ubasic.headers.h"

struct keywords {
	// keyword string
	char key[UBAS_KEYWORD_LEN];
	// callback
	void (*ptr)(__CONTEXT *context, char* returnstr);
	char iskw;
} table[] = {
	{"print", KW_PRINT, 1},
	{"input", KW_INPUT, 1},
	{"if", KW_IF, 1},
	{"then", KW_THEN, 1},
	{"goto", KW_GOTO, 1},
	{"for", KW_FOR, 1},
	{"next", KW_NEXT, 1},
	{"to", KW_TO, 1},
	{"gosub", KW_GOSUB, 1},
	{"return", KW_RETURN, 1},
	{"end", KW_END, 1},
	{"", KW_END, 0} 
};



__STACK __Code = {0,NULL};
__VARLIST __Vars = {0, NULL, NULL};

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
	__VAR *tempv = NULL ;
	encap_error quoteerror = {0};
	if(argc==2)
	{
		__Code.ptr=(unsigned char*)argv[1];
		__CONTEXT context = {__Code, __Vars, 0, 0};
		
		quoteerror = _ubas_encap(&context);
		_ubas_var_add(&context, "test", 1);
		_ubas_var_add(&context, "test2", 1);
		_ubas_var_add(&context, "test3", 1);
		_ubas_var_add(&context, "test4", 1);
		
		tempv = _ubas_var(&context, "test4");
		if(tempv != NULL)
		{
			printf("\nVarable %s existe.",tempv->name);
		}
		else
		{
			printf("\n Non trouvé");
		}

		if(!quoteerror.general)
		{
			_ubas_eval(&context);
		}
		else
		{
			_ubas_debug_encap(quoteerror);
		}
		return 0;
	}
	else
	{
		_ubas_usage();
		return 1;
	}
}

void _ubas_eval(__CONTEXT *context)
{
	__TOKEN token, eokwtok;
	token = _ubas_nextToken(context);
	while(token.type!=STOP)
	{
		if(token.type == KEYWORD)
		{
			(*token.ptr)(context, NULL);
			eokwtok = _ubas_nextToken(context);
			if(eokwtok.type != EOKW)
			{
				printf("\n%sKEYWORD ERROR:%s", CF_RED, CL_RESET);
				printf("%sERROR Bad arguments number for command \"%s\".%s",CF_DRED, token.str, CL_RESET);
				exit(0);
			}
			free(token.str);
		}
		token = _ubas_nextToken(context);
	}
}

void (* _ubas_keywordCallback(__TOKEN *token))(__CONTEXT *context, char* returnstr)
{
	int index=0;
	while(table[index].iskw == 1)
	{
		if(strcmp (token->str,table[index].key)==0)
		{
			return *table[index].ptr;		
		}

		index++;
	}
	return NULL;
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

void _ubas_trimContext(__CONTEXT *context)
{
	while((_CODE_SYMBOLE==' '))
	{
		context->code.offset ++;
	}
}

void _ubas_usage()
{
	printf("%sNombre d'arguments invalides.\n\tubasic \"cmd[;cmd]\"%s\n\n",CF_GREEN,CL_RESET);
}

/*
	Returns a struct which defines encapsulation errors.
*/

encap_error _ubas_encap(__CONTEXT *context)
{
	int offset = 0;
	encap_error encap_check;
	int simplequote_mutex = 0;
	int doublequote_mutex = 0;
	int bracket = 0;
	int parenthesis= 0;
	int brace = 0;
	
	while(*(context->code.ptr + context->code.offset + offset))
	{
		switch(	*(context->code.ptr + context->code.offset + offset))
		{
			case '\'':
				if((!doublequote_mutex))
				{
					//simplequote_cnt ++;
					simplequote_mutex = !simplequote_mutex;
				}
				
			break;
			case '"':
				if((!simplequote_mutex))
				{
					//doublequote_cnt ++;
					doublequote_mutex = !doublequote_mutex;
				}

			break;
			
			case '(':
					parenthesis += !( simplequote_mutex || doublequote_mutex );
			break;
			case ')':
					parenthesis -= !( simplequote_mutex || doublequote_mutex );
			break;
			
			case '[':
					bracket += !( simplequote_mutex || doublequote_mutex );
			break;
			case ']':
					bracket -= !( simplequote_mutex || doublequote_mutex );
			break;
			
			case '{':
					brace += !( simplequote_mutex || doublequote_mutex );
			break;
			case '}':
					brace -= !( simplequote_mutex || doublequote_mutex );
			break;
			
			default:
			break;
		}
		offset++;
	}
	
	encap_check.simplequote = simplequote_mutex;
	encap_check.doublequote = doublequote_mutex;
	encap_check.brace = brace;
	encap_check.bracket = bracket;
	encap_check.parenthesis = parenthesis;
	encap_check.general = ( simplequote_mutex || doublequote_mutex ) || (brace + bracket + parenthesis);
	
	return encap_check; 
}

void _ubas_debug_encap(encap_error encap)
{
	printf("\n%sENCAP ERROR:%s", CB_RED, CL_RESET);
	printf("\n\t simplequote: \t%d ", encap.simplequote);
	printf("\n\t doublequote: \t%d ", encap.doublequote);
	printf("\n\t brace: \t%d ", encap.brace);
	printf("\n\t bracket: \t%d ", encap.bracket);
	printf("\n\t parenthesis: \t%d ", encap.parenthesis);
	printf("\n\t general: \t%d%s\n", encap.general, CL_RESET);
}

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
		case EOKW:
			tokentype="EOKW\0";
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
	
	printf("\n%sTOKEN:\n\ttype: %s\n\tstr: %s\n\tlen: %d%s\n",CB_BLUE CF_YELLOW, tokentype, token.str, token.len, CL_RESET);
}

__TOKEN _ubas_nextToken(__CONTEXT *context)
{
	char *codeStr = NULL;
	int offset = 0;
	char bracket;
	__TOKEN token = {NULL,0, STOP, NULL};
	
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
			if(_CODE_SYMBOLE==';')
			{
				free(token.str);
				token.type = EOKW;
			}
			else
			{
				token.type = DELIMITER;
			}
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

			//_ubas_debug_token(token);
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

			//_ubas_debug_token(token);
			context->code.offset += token.len;	return token;

		}
		
		// String / Keyword
		else
		{
			token.len = strcspn(codeStr," +-*^/%=;(),><");
			token.str = _ubas_string(codeStr, token.len);
			token.ptr= *_ubas_keywordCallback(&token);
			if(token.ptr!=NULL)
			{
				token.type = KEYWORD;
			}
			else
			{
				token.type = STRING;
			}
			_ubas_debug_token(token);
			context->code.offset += token.len;	return token;
		}
	}
	
	if(token.str == NULL)
	{
		token.len=0;
		token.type = STOP;
	}
	context->code.offset += token.len;	return token;
}

