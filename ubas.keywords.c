
#include "./ubasic.conf.h"
#include "./ubasic.structs.h"
#include "./ubasic.headers.h"

void KW_PRINT (__CONTEXT *context, char* returnstr)
{
	__TOKEN token;
	token = _ubas_nextToken(context);
	switch(token.type)
	{
		case QUOTE:
			printf("%s\n", token.str);
		break;
		case STRING:
		break;
		case NUMBER:
		break;
		default:
			printf ("ERROR\n");
		break;
	}
}

void KW_INPUT (__CONTEXT *context, char* returnstr)
{

}

void KW_IF (__CONTEXT *context, char* returnstr)
{

}

void KW_THEN (__CONTEXT *context, char* returnstr)
{

}

void KW_GOTO (__CONTEXT *context, char* returnstr)
{

}

void KW_FOR (__CONTEXT *context, char* returnstr)
{

}

void KW_NEXT (__CONTEXT *context, char* returnstr)
{

}

void KW_TO (__CONTEXT *context, char* returnstr)
{

}

void KW_GOSUB (__CONTEXT *context, char* returnstr)
{

}

void KW_RETURN (__CONTEXT *context, char* returnstr)
{

}

void KW_END (__CONTEXT *context, char* returnstr)
{

}

