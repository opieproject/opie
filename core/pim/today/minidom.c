/*
 * The GGZ Gaming Zone Metaserver Project
 * Copyright (C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
 * Published under GNU GPL conditions.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minidom.h"

/* Remove all line breaks, spaces, tabs, and check/remove XML header */
char *minidom_cleanstream(const char *stream)
{
	static char *cs = NULL;
	unsigned int i, j;
	int inside;
	int spaceprotect;
	int spacesonly;

	if(!stream) return NULL;
	if(cs)
	{
		free(cs);
		cs = NULL;
	}
	cs = (char*)malloc(strlen(stream) + 1);

	j = 0;
	inside = 0;
	spaceprotect = 0;
	spacesonly = 0;
	for(i = 0; i < strlen(stream); i++)
	{
		if(stream[i] == '\t') continue;
		if(stream[i] == '\n')
		{
			if(inside)
			{
				cs[j] = ' ';
				j++;
			}
			continue;
		}
		if((!inside) && (!spaceprotect) && (stream[i] == ' ')) continue;
		if((stream[i] != ' ') && (stream[i] != '<') && (spacesonly)) spacesonly = 0;
		if(stream[i] == '<')
		{
			if(spacesonly) j = spacesonly;
			spacesonly = 0;
			inside = 1;
			if(stream[i + 1] == '?') inside = 2;
			spaceprotect = 0;
			if(stream[i + 1] == '/') spaceprotect = -1;
		}
		if(inside != 2)
		{
			cs[j] = stream[i];
			j++;
		}
		if(stream[i] == '>')
		{
			inside = 0;
			spaceprotect++;			/* 1 on opening tag, 0 on closing tag */
			if(stream[i - 1] == '/') spaceprotect = 0;
			spacesonly = j;
		}
	}
	cs[j] = 0;

	/*printf("DEBUG: cleanstream: return %s\n", cs);*/
	return cs;
}

/* Return position of c or -1 if not found */
int strpos(const char *s, char c)
{
	unsigned int i;

	if(!s) return -1;
	for(i = 0; i < strlen(s); i++)
		if(s[i] == c) return i;
	return -1;
}

/* Add a complete tag to an element, return the new child */
ELE *minidom_makechild(ELE *parent, char *tag)
{
	char *token;
	int i, j, k, l, len, size, count;
	ELE *ele;
	ATT *att;
	int pos;

	if(!tag) return parent;

	ele = (ELE*)malloc(sizeof(ELE));
	ele->parent = parent;
	ele->name = NULL;
	ele->value = NULL;
	ele->at = NULL;
	ele->el = NULL;
	ele->elnum = 0;
	ele->atnum = 0;

	if(parent)
	{
		parent->elnum++;
		parent->el = (ELE**)realloc(parent->el, (parent->elnum + 1) * sizeof(ELE*));
		parent->el[parent->elnum - 1] = ele;
		parent->el[parent->elnum] = NULL;
	}

	/*printf("TAG: %s\n", tag);*/
	i = 0;
	k = 0;
	l = 0;
	count = 0;
	token = strdup(tag);
	len = strlen(tag);
	for(j = 0; j < len; j++)
	{
		if(tag[j] == '\"') k++;
		if(tag[j] == '\'') k++;
		if(tag[j] == '=') l++;
		if(((tag[j] == ' ') && ((k == 2) || (l == 0))) || (j == len - 1))
		{
			size = j - i;
			if(j == len - 1) size++;
			strncpy(token, tag + i, size);
			token[size] = 0;
			/*printf("ATTRIBUTE: %s\n", token);*/
			if(count == 0)
			{
				/* name */
				/*printf(" ** %s\n", token);*/
				ele->name = (char*)malloc(strlen(token) + 1);
				strcpy(ele->name, token);
			}
			else
			{
				att = (ATT*)malloc(sizeof(ATT));
				pos = strpos(token, '=');
				if(pos == -1)
				{
					att->name = strdup(token);
					att->value = NULL;
				}
				else
				{
					att->name = (char*)malloc(pos + 1);
					att->value = (char*)malloc(strlen(token) - pos + 1);
					memcpy(att->name, token, pos);
					memcpy(att->value, token + pos + 1 + 1, strlen(token) - pos - 1 - 2); /* exclude "" marks */
					att->name[pos] = 0;
					att->value[strlen(token) - pos - 1 - 2] = 0;
				}
				ele->atnum++;
				ele->at = (ATT**)realloc(ele->at, (ele->atnum + 1) * sizeof(ATT*));
				ele->at[ele->atnum - 1] = att;
				ele->at[ele->atnum] = NULL;
			}
			i = j + 1;
			k = 0;
			l = 0;
			count++;
		}
	}
	free(token);

	return ele;
}

/* Parses a stream to add its contents to a DOM */
DOM *minidom_parse(const char *stream)
{
	DOM *dom;
	char *cs;
	unsigned int i;
	int mark, lastmark;
	char *token;
	int error = 0;
	ELE *ele, *cp;							/* root node and current pointer */
	int endtag;

	if(!stream) return NULL;
	cs = minidom_cleanstream(stream);

	dom = (DOM*)malloc(sizeof(DOM));
	dom->processed = 0;
	dom->valid = 0;
	dom->el = NULL;

	/*ele = (ELE*)malloc(sizeof(ELE));*/		/* memory loss! */
	/*ele->parent = NULL;
	ele->name = NULL;
	ele->value = NULL;
	ele->el = NULL;
	ele->at = NULL;
	ele->elnum = 0;*/
	ele = NULL;
	cp = NULL;
	token = NULL;
	mark = -1;
	error = 0;
	lastmark = 0;
	endtag = 0;
	for(i = 0; i < strlen(cs); i++)
	{
		if(cs[i] == '<')
		{
			if(mark == -1) mark = i + 1;
			else error = 1;
			if((int)i != lastmark)
			{
				if(token) free(token);
				token = (char*)malloc(i - lastmark + 1);
				memcpy(token, cs + lastmark, i - lastmark);
				token[i - lastmark] = 0;
				/*printf("  --> content: %s\n", token);*/
				cp->value = (char*)malloc(strlen(token) + 1);
				strcpy(cp->value, token);
			}
		}
		if(cs[i] == '>')
		{
			if(mark != -1)
			{
				if(token) free(token);
				token = (char*)malloc(i - mark + 1);
				memcpy(token, cs + mark, i - mark);
				token[i - mark] = 0;
				/*printf("--> token: %s\n", token);*/
				if((token[0] == '/') && (cp)) cp = cp->parent;
				else
				{
					if(token[i - mark - 1] == '/')
					{
						token[i - mark - 1] = 0;
						/*cp = cp->parent;*/
						endtag = 1;
					}
					/*printf("INSERT AT: %i\n", cp);*/
					cp = minidom_makechild(cp, token);
					if((cp) && (!ele)) ele = cp;
					/*if(cp) cp = cp->parent;*/ /* QUICK HACK?! */
					if(endtag)
					{
						if(cp) cp = cp->parent;
						endtag = 0;
					}
				}
				mark = -1;
				lastmark = i + 1;
			}
			else error = 1;
		}
	}

	dom->valid = !error;
	dom->processed = 1;
	dom->el = ele;

	return dom;
}

/* Loads an XML file and return the DOM */
DOM *minidom_load(const char *file)
{
	DOM *dom;
	FILE *f;
	char buf[1024];
	char *buffer;

	f = fopen(file, "r");
	if(!f) return NULL;

	buffer = (char*)malloc(1);
	strcpy(buffer, "");
	while(fgets(buf, sizeof(buf), f))
	{
		if(strlen(buf) > 0)
		{
			/*printf("Got: %s\n", buf);*/
			buffer = (char*)realloc(buffer, strlen(buffer) + strlen(buf) + 1);
			/*buf[strlen(buf) - 1] = 0;*/
			/*printf("Add: %s", buf);*/
			strcat(buffer, buf);
		}
	}
	fclose(f);

	/*printf("DEBUG: ready!\n");
	printf("DEBUG: load: parse %s\n", buffer);*/
	dom = minidom_parse(buffer);
	free(buffer);

	return dom;
}

void minidom_internal_dump(ELE *ele);		/* forward decl */

/* Dump out the DOM in XML format */
/* FIXME: return a char* and print this if dump is needed */
void minidom_dump(DOM *dom)
{
	if(!dom) return;
	if(!dom->processed)
	{
		printf("ERROR: DOM is incomplete!\n");
		return;
	}
	if(!dom->valid)
	{
		printf("ERROR: DOM is invalid!\n");
		return;
	}
	if(!dom->el)
	{
		printf("ERROR: DOM is empty!\n");	/* is this really an error? */
		return;
	}
	minidom_internal_dump(dom->el);
}

void minidom_internal_dump(ELE *ele)
{
	int i;
	static int indent = 0;
	static int start = 0;

	if(!ele) return;
	if(!start)
	{
		start = 1;
		printf("<?xml version=\"1.0\"?>\n");
	}
	indent++;
	for(i = 0; i < (indent - 1) * 2; i++)
		printf("  ");
	printf("<%s", ele->name);

	i = 0;
	while((ele->at) && (ele->at[i]))
	{
		if(ele->at[i]->value)
			printf(" %s=\"%s\"", ele->at[i]->name, ele->at[i]->value);
		else
			printf(" %s", ele->at[i]->name);
		i++;
	}

	if((!ele->value) && (!ele->el)) printf("/");
	printf(">\n");
	if(ele->value)
	{
		for(i = 0; i < (indent - 1) * 2; i++)
			printf("  ");
		printf("  %s\n", ele->value);
	}

	i = 0;
	while((ele->el) && (ele->el[i]))
	{
		minidom_internal_dump(ele->el[i]);
		i++;
	}

	if((ele->value) || (ele->el))
	{
		for(i = 0; i < (indent -1) * 2; i++)
			printf("  ");
		printf("</%s>\n", ele->name);
	}
	indent--;
}

/* Clean up after all operations */
void minidom_free(DOM *dom)
{
	if(!dom) return;
	free(dom);
	dom = NULL;
}

/* Query a list of elements */
ELE **MD_querylist(ELE *parent, const char *name)
{
	static ELE **elelist = NULL;
	int i, j;

	if(!parent) return NULL;
	/*if(elelist)
	{
		i = 0;
		while(elelist[i])
		{
			free(elelist[i]);
			i++;
		}
		free(elelist);
		elelist = NULL;
	}*/						/* MEMORY HOLE !*/

	i = 0;
	j = 1;
	while((parent->el) && (parent->el[i]))
	{
		if(!strcmp(parent->el[i]->name, name))
		{
			elelist = (ELE**)malloc((j + 1) * sizeof(ELE*));
			elelist[j - 1] = parent->el[i];
			elelist[j] = NULL;
			j++;
		}
		i++;
	}

	return elelist;
}

/* Query a single element */
ELE *MD_query(ELE *parent, const char *name)
{
	ELE **elelist;

	elelist = MD_querylist(parent, name);
	if((elelist) && (elelist[0])) return elelist[0];
	return NULL;
}

/* Get an attribute's value */
char *MD_att(ELE *element, const char *name)
{
	int i;

	if((!element) || (!element->at)) return NULL;
	i = 0;
	while((element->at[i]))
	{
		if(!strcmp(element->at[i]->name, name)) return element->at[i]->value;
		i++;
	}
	return NULL;
}

