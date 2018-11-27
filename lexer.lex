%{

/* Declarations section */
#include <stdio.h>
#include "tokens.h"

typedef enum tokens tokens_t;

%}
%option yylineno
%option noyywrap
%option caseless
digit   		[0-9]
decimal      	(\x2B|\x2D)?({digit})+
hex          	(0x)([a-f]|{digit})*
oct          	0([0-7])*
bin          	0b([0-1])*
integer      	{decimal}|{hex}|{oct}|{bin}
realo        	(\x2B|\x2D)?(((\x2E){digit}+)|({digit}+(\x2E))|({digit}+(\x2E){digit}+))
realt        	{realo}e(\x2B|\x2D){digit}+
real        	{realt}|{realo}
true      	 	(true)|(yes)
false        	(false)|(no)
letter  		[a-zA-Z]
allowedsp		(\x7E|\x5F|\x2D|\x3F|\x27|\x2E|\x24)
line			(\x0A|\x0D|(\x0D\x0A))
whitespace		(\x09|\x20)
alowed			{digit}|{letter}|{allowedsp}
key 			{letter}+({alowed}|((\x20)+{alowed}))*
section		 	(\x5B){key}(\x5D)
indent       	({whitespace})+
assign       	(\x3D)|(\x3A)
dirname     	{letter}+({digit}|{letter}|(\x2D|\x2B|\x5F|\x2E|\x7E))*
path        	(\x2F)+(({dirname})*(\x2F)*)*
link        	(\x24)((\x7B{key}\x7D)|(\x7B{key}\x23{key}\x7D))
sep         	(\x2C)
string1      	(\x22)(([\x09\x20-\x21\x23-\x5B\x5D-\x7E])|((\x5C)([\x20-\x7E]))|({line}))*(\x5C)?(\x22)
string2     	{letter}+(([\x21-\x22\x24-\x2B\x2D-\x3A\x3C-\x7E])|((\x09|\x20)+([\x21-\x22\x24-\x2B\x2D-\x3A\x3C-\x7E])))*
string      	({string1}|{string2})
comment     	({indent})*((\x23)|(\x3B))[^(\x0A|\x0D|(\x0D\x0A))]*

%s KA IN K
%%
^{key}									 { BEGIN(K); return(KEY);}
<K,KA>{assign}	   	 					 {BEGIN(KA); return(ASSIGN); }
{comment}                                {return(-1);}
{section}	 					 		 {return(SECTION);}
^{indent}       						 {BEGIN(IN); return(INDENT);}
<<EOF>>        						     {return(EF);}
<IN,KA>{true}/.* 					     {return(TRUE);}
<IN,KA>{false}/.*    					 {return(FALSE);}
<IN,KA>{integer}        		 		 {return(INTEGER);}
<IN,KA>{real}          		 			 {return(REAL);}
<IN,KA>{path}           		 		 {return(PATH);}
<IN,KA>{link}           		 		 {return(LINK);}
<IN,KA>{sep}             				 {return(SEP);}
<IN,KA>{string}          				 {return(STRING);}
{line}								     BEGIN(0);
{whitespace}							 ;
.									     {printf("Error %c\n", yytext[yyleng-1]); exit(0);}
%%
