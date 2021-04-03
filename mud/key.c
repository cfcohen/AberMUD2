#include "anachronisms.h"
/*  Key drivers */

#include <stdio.h>
#include <sgtty.h>
#define sgttyb termios /*anachronism*/
#define sg_flags c_lflag /*anachronism*/
#define gtty(fd, st) tcgetattr(fd, st) /*anachronism*/
#define stty(fd, st) tcsetattr(fd, TCSANOW, st) /*anachronism*/

long save_flag= -1;

keysetup()
{
	struct sgttyb x;
	gtty(fileno(stdin),&x);
	save_flag=x.sg_flags;
	x.sg_flags&=~ECHO;
	x.sg_flags&=~ICANON/*|=CBREAK anachronism*/;
	stty(fileno(stdin),&x);
}

void /*anachronism*/
keysetback()
{
	struct sgttyb x;
	if(save_flag== -1) return;
	gtty(fileno(stdin),&x);
	x.sg_flags=save_flag;
	stty(fileno(stdin),&x);
}

char key_buff[256];
char pr_bf[32];
long key_mode= -1;

void /*anachronism*/
key_input(ppt,len_max)
char *ppt;
int len_max;
{
   char x;
   extern long pr_due;
   int len_cur=0;
   key_mode=0;
   strcpy(pr_bf,ppt);
   bprintf("%s",ppt);
   pbfr();
   pr_due=0;
   strcpy(key_buff,"");
   while(len_cur<len_max)
   {
   	x=getchar();
   	if(x=='\n')
   	{
   		printf("\n");
   		key_mode= -1;
    		return;
   	}
   	if(((x==8)||(x==127))&&(len_cur))
	{
		putchar(8);
		putchar(' ');
		putchar(8);
		len_cur--;
		key_buff[len_cur]=0;
		continue;
	}
	if(x<32) continue;
	if(x==127) continue;
	putchar(x);
	key_buff[len_cur++]=x;
	key_buff[len_cur]=0;
     }
}	

key_reprint()
{
extern long pr_due;
extern long pr_qcr;
pr_qcr=1;
pbfr();
if((key_mode==0)&&(pr_due))printf("\n%s%s",pr_bf,key_buff);
pr_due=0;
fflush(stdout);
}
