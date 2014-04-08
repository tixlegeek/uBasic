#include "./ubasic.conf.h"
#include "./ubasic.structs.h"
#include "./ubasic.headers.h"

void _ubas_var_add(__CONTEXT *context, char* name, int value);
__VAR *_ubas_var(__CONTEXT *context, char* name);

void _ubas_var_add(__CONTEXT *context, char* name, int value)
{
	struct __VAR *newvar = malloc(sizeof(newvar));
	int namel = strlen(name);
	if(namel<32)
	{
		newvar->name = _ubas_string(name, namel);
		
		if(newvar!=NULL)
		{
			newvar->value = value;
			newvar->n = NULL;
			if(context->vars.l == NULL)
			{
				newvar->p = NULL;
				context->vars.f = newvar;
				context->vars.l = newvar;
			}
			else
			{
				context->vars.l->n = newvar;
				newvar->p = context->vars.l;
				context->vars.l = newvar;
			}
			context->vars.len++;
		}
	}
}
/*
void _ubas_var_del(__CONTEXT *context, char* name)
{
    struct __VAR *temp_var = context->vars.f;
    int index = 1;
	while(strcmp (name, temp_var->name)!=0)
	{
		printf("\n\tElement %d ", index);
		index++;
		
		if(temp_var->n != NULL)
		{
			temp_var = temp_var->n;
		}
		else
		{
			break;
		}
	}
	printf("\nvariable %s = %d", temp_var->name, temp_var->value);
	
}*/

__VAR *_ubas_var(__CONTEXT *context, char* name)
{
    struct __VAR *temp_var = context->vars.f;
    int index = 1;
    int found = strcmp (name, temp_var->name);
	while(found!=0)
	{
		printf("\n\tElement %d ", index);
		index++;
		
		if(temp_var->n != NULL)
		{
			temp_var = temp_var->n;
		}
		else
		{
			break;
		}
		found = strcmp (name, temp_var->name);
	}
	if(found)
	{
		return (__VAR*)temp_var;
	}
	else
	{
		return NULL;
	}
	
}
