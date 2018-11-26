%{

/* Declarations section */
#include <stdio.h>
#include <stdlib.h> //for strtol
#include <ctype.h> //for isxdigit
#include <string.h> //for strcmp
#include "tokens.h"

typedef enum tokens tokens_t;

/*tokens_t showToken(const char* name);
void showTokenStr(char* name);
void showTokenInt(char* name);
void showTokenComm(char* name);
*/

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
string1      	(\x22)(([\x09\x20-\x21\x23-\x5B\x5D-\x7E])|((\x5C)([\x20-\x7E]))|({line}))*(\x5C)?(\x22)?
string2     	{letter}+(([\x21-\x22\x24-\x2B\x2D-\x3A\x3C-\x7E])|((\x09|\x20)+([\x21-\x22\x24-\x2B\x2D-\x3A\x3C-\x7E])))*
string      	({string1}|{string2})
comment     	({indent})*((\x23)|(\x3B))[^(\x0A|\x0D|(\x0D\x0A))]*

%s KA IN K
%%
^{key}									 { BEGIN(K); return(KEY);}
<K,KA>{assign}	   	 					 {BEGIN(KA); return(ASSIGN); }
{section}	 					 		 {return(SECTION);}
^{indent}       						 {BEGIN(IN); return(INDENT);}
<<EOF>>        						     {return(EOF);}  //todo: fit this . shani
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
.									     {printf("Error %c\n", yytext[yyleng-1]);}

%%
/*
int NameToToken(const char* name){
    const char* convert[]={"KEY","SECTION","INDENT","ASSIGN","TRUE","FALSE","INTEGER","REAL","STRING","PATH","LINK","SEP","EOF"};
    int i=0;
    for(i=(int)KEY; i<((int)EF+1); i++){
        if (strcmp(name,convert[i]))
            return i;
    }
    return -1; //we shouldnt reach here
}

tokens_t showToken(const char*  name)
{
    tokens_t to_return = (tokens_t)(NameToToken(name));
    return to_return;
}


void NonBlockedStrHandle(char* n_yytext){
    //this function copies the string as it is
    int i=0;
    while(i != yyleng){
        n_yytext[i]=yytext[i];
        i++;
    }
    //deleteTrailingSpaces(yytext,n_yytext,yyleng);	//racheli
}

void BlockedStrHandle(int leng, char* yytext_t,char* n_yytext){
    int i=1;
    int j=0;
    while(yytext_t[i] != '"'){
        if (i == leng){
            //the input limit reached but no '"' detected
            printf("Error unclosed string\n");
            //exit(0);
        }
        if (yytext_t[i] == '\\'){
            i++; //skip the "\"
            if (i==leng){
                // \ is at the end of the string
                printf("Error %c\n", '\\');
              //  exit(0);
            }
            if (yytext_t[i] == 'x' || yytext_t[i] == 'X'){
                char *ptr;
                char hexVal[2];
                hexVal[0] = yytext_t[i+1];
                hexVal[1] = yytext_t[i+2];
                //strtol converts string to int. "16" is for hex value
                int decVal = (int)strtol(hexVal, &ptr, 16);
                if (decVal<0 || decVal > 255 || !isxdigit(hexVal[0]) ||!isxdigit(hexVal[1])){
                    // \xdd - but dd is not a valid ascii value
                    printf("Error undifined escape sequence %c\n", yytext_t[i]);
                //    exit(0);
                }
                else{
                    char c = decVal; //converts int ascii value into it's suitiable char
                    n_yytext[j] = c;
                    i+=3; j++;
                }
            }
            else{
                if(((yytext_t[i]=='\r') && yytext_t[i+1]=='\n')){
                    n_yytext[j]='\r';
                    n_yytext[j+1]='\n';
                    j+=2; i++;
                    continue;
                }
                //escape seqence is not x
                switch(yytext_t[i]){
                    case 't':
                    case 'T':
                        n_yytext[j]='\t';
                        j++; i++;
                        break;
                    case '\\':
                        n_yytext[j]='\\';
                        j++;; i++;
                        break;
                    case '"':
                        n_yytext[j]='\"';
                        j++; i++;
                        break;
                    case 'r':
                    case 'R':
                        n_yytext[j]='\r';
                        j++; i++;
                        break;
                    case ';':
                        n_yytext[j]=';'; //todo: test chars: : ; =
                        j++; i++;
                        break;
                    case ':':
                        n_yytext[j]=':';
                        j++; i++;
                        break;
                    case '=':
                        n_yytext[j]='=';
                        j++; i++;
                        break;
                    case 'n':
                    case 'N':
                        n_yytext[j]='\n';
                        j++; i++;
                        break;

                    case 'a':
                    case 'A':
                        n_yytext[j]='\a';
                        j++; i++;
                        break;
                    case '0':
                        n_yytext[j]='\0';
                        j++; i++;
                        break;
                    case 'b':
                    case 'B':
                        n_yytext[j]='\b';
                        j++; i++;
                        break;
                    case '#':
                        n_yytext[j]='#';
                        j++; i++;
                        break;
                    default:
                        printf("Error undefined escape sequence %c\n", yytext_t[i]);
                  //      exit(0);
                }
            }
        }
        else{
            n_yytext[j] = yytext_t[i];
            i++; j++;
        }
    }
}

int deleteNewLines(char* yytext_t){
    int i=1;
    int j=1;
    yytext_t[0]='"';
    while(i!=yyleng){
        if((yytext[i]=='\r')&&(yytext[i+1]=='\n')){
            yytext_t[j]='\x20';
            i++;
        }
        else if ((yytext[i]=='\x0A')||(yytext[i]=='\x0D')){
            yytext_t[j]='\x20';
        }
        else{
            yytext_t[j] = yytext[i];
        }
        i++; j++;
    }
    return j;
}

void showTokenStr(char*  name)
{
    char n_yytext[1024]={0};
    //a blocked string will allwase start with "
    if (yytext[0] == '"'){
        char yytext_t[1024]={0};
        int leng = deleteNewLines(yytext_t);
        BlockedStrHandle(leng, yytext_t,n_yytext);
    }
    else
        NonBlockedStrHandle(n_yytext);

    showToken(n_yytext);
}

void showTokenInt(char*  name)
{
    long decVal =0;
    char* ptr;
    if (yytext[0] == '0'){
        if(yytext[1] == 'x' || yytext[1] == 'X'){
            //the number is hexdecimal
            decVal = strtol(yytext, &ptr, 0);
        }
        else if (yytext[1]=='b' || yytext[1]=='B'){
            //the number is binary
            decVal = strtol((yytext+2), &ptr, 2);
        }
        else{
            //try to convert to octal. convertion will succeed only if there is only digits from the range 0-8
            decVal = strtol(yytext, &ptr, 8);
            if (decVal == 0 && yyleng!=1){
                //convertion failed, the number is not octal, therefor, it is a decimal that starts with "0"
                decVal = strtol(yytext, &ptr, 10);
            }
        }

    }
    else{
        //the number is decimal
        decVal = strtol(yytext, &ptr, 10);
    }
   showToken(name);
}

void showTokenComm(char*  name)
{
    int temp = yylineno;
    char n_yytext[1024]={0};
    int i=0;
    int j=0;
    while((yytext[i]!='#')&&(yytext[i]!=';')){
        i++;
    }
    //delete Trailing Spaces
    int index=0;
    int index_t=yyleng-1;
    while(yytext[index_t]=='\t' || yytext[index_t]=='\x20'){
        index_t--;
    }
    while(i!=index_t+1){
        n_yytext[j]=yytext[i];
        i++; j++;
    }
    printf("%d %s %s\n",temp, name, n_yytext);
}
*/