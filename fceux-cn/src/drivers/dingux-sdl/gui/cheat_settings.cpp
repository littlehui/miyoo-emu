#include "../../../cheat.h"
#include "../../../fceu.h"
#include "../config.h"
#include "utils/memory.h"
#include <stdlib.h>




typedef struct _setting_entry_cheat {
	char *name;
	char *info;
	void (*update)(unsigned long,int);
} SettingCheat;


//cjs函数声明
char *stringcat(char *s1, char *s2);
int sdl_cheat_menu();
void chj_update(unsigned long key,int ec_index);
void str_log(char *info,char *log);
char* str_modifi(char *s1, const char *s2);
char* str_cheatname(char *s1, const char *s2);

char *strip(char *fu_in_strs,char *fu_out_strs);
void value_add(char *s);
int check_hex(char *test_chars);
bool check_add_value(char *add, char *value);
char cheat_menu_yes = 0;



static SettingCheat
	st_cheat_menu[MAX_cjs_cheats];
//	= {	{ "Game EC cheats", "Emulate ALL cheats", gg_update_cjs }};


int RunCheatSettings() {
	if(cheat_menu_yes == 0){
		sdl_cheat_menu();
		}
	static int index = 0;
	static int spy = 72;
	int done = 0, y, i;

	int max_entries = 9;
	int menu_size = cheat_file_num+1;


	static int offset_start = 0;
	static int offset_end = menu_size > max_entries ? max_entries : menu_size;

	char tmp[32];
	int itmp;
	int cheat_index = 0;

	g_dirty = 1;
	while (!done) {
		// Parse input
		readkey();
		if (parsekey(DINGOO_B))
			done = 1;
		//
		if(cheat_file_num > -1){
			if (parsekey(DINGOO_UP, 1)) {
				if (index > 0) {
					index--;
					if (index >= offset_start)
						spy -= 15;
					if ((offset_start > 0) && (index < offset_start)) {
						offset_start--;
						offset_end--;
					}
				} else {
					index = menu_size-1;
					offset_end = menu_size;
					offset_start = menu_size <= max_entries ? 0 : offset_end - max_entries;
					spy = 72 + 15*(index - offset_start);
				}
			}

			if (parsekey(DINGOO_DOWN, 1)) {
				if (index < (menu_size - 1)) {
					index++;

					if (index < offset_end)
						spy += 15;

					if ((offset_end < menu_size) && (index >= offset_end)) {
						offset_end++;
						offset_start++;
					}
				} else {
					index = 0;
					offset_start = 0;
					offset_end = menu_size <= max_entries ? menu_size : max_entries;
					spy = 72;
				}
			}

			if (parsekey(DINGOO_LEFT, 1) || parsekey(DINGOO_RIGHT, 1) || parsekey(
					DINGOO_A)){
					st_cheat_menu[index].update(g_key,index);
				}
		}

		// Draw stuff
		if (g_dirty) {
			draw_bg(g_bg);
			
			//Draw Top and Bottom Bars
			DrawChar(gui_screen, SP_SELECTOR, 0, 37);
			DrawChar(gui_screen, SP_SELECTOR, 81, 37);
			DrawChar(gui_screen, SP_SELECTOR, 0, 225);
			DrawChar(gui_screen, SP_SELECTOR, 81, 225);
			DrawText2(gui_screen, "B键 - 返回", 235, 225);//225-220
			DrawChar(gui_screen, SP_LOGO, 12, 9);
			
			// Draw selector
			DrawChar(gui_screen, SP_SELECTOR, 56, spy);
			DrawChar(gui_screen, SP_SELECTOR, 77, spy);

			//DrawText(gui_screen, "Cheat Settings", 8, 37);
			/***
			汉字处理
			***/
			DrawText2(gui_screen, "金手指", 8, 37);//37-32



			// Draw menu
			if(cheat_file_num > -1){
				for (i = offset_start, y = 72; i < offset_end; i++, y += 15) {
					DrawText2(gui_screen, st_cheat_menu[i].name, 60, y);
					cheat_index=chj[i].cheat_default;
					sprintf(tmp, "%s", chj[i].cheat_key[cheat_index].key_name);
					DrawText2(gui_screen, tmp, 210, y);
					}
				// Draw info
				DrawText2(gui_screen, st_cheat_menu[index].info, 8, 225);//225-220
				}

			// Draw offset marks
			if (offset_start > 0)
				DrawChar(gui_screen, SP_UPARROW, 274, 62);
			if (offset_end < menu_size)
				DrawChar(gui_screen, SP_DOWNARROW, 274, 203);

			g_dirty = 0;
		}

		SDL_Delay(16);

		// Update real screen
		FCEUGUI_Flip();
	}

	// Clear screen
	dingoo_clear_video();

	g_dirty = 1;
	return 0;
}




//cjs
char *stringcat(char *s1, char *s2)
{
	int i=0,j=0;
	char temp_str[1024];
	while(s1[i]!='\0')
	{
		 temp_str[i]=s1[i];
		 i++;
	}

	while(s2[j]!='\0')
	{
		 temp_str[i+j]=s2[j];
		 j++;
	}
	temp_str[i+j]='\0';
	char *s3=(char *)malloc(sizeof(temp_str)+1);
	strcpy(s3,temp_str);
	return s3;

}



/*cheats MENU SDL*/
int sdl_cheat_menu(){
	cheat_menu_yes =1;
	cheat_file_num = -1;
	
	//chj文件处理	
	FILE *fp = NULL;
	char *fn = NULL;

    //if (strlen(sdl_fullname) > 0) fn = str_cheatname(sdl_fullname,".chj");
    if (strlen(sdl_fullname) > 0) fn = str_modifi(sdl_fullname,".chj");

    fp=FCEUD_UTF8fopen(fn,"rb");
	if(fp == NULL) {
        fn=strdup(FCEU_MakeFName(FCEUMKF_CHEAT,0,0).c_str());
		fp=FCEUD_UTF8fopen(fn,"rb");

        if(!fp) {
            //fprintf(stderr, "fp == NULL 22 ..\n");
            //fclose(fp);
			return -1;
		}
	}
	free(fn);
    fprintf(stderr, "fp == NUL ..%s\n",fp);
    char linebuf[1024];
	char namebuf[100];
	int key_num = 0;
	const char *delfault_OFF="OFF";
	char line_strip[1024];
	char ec_name[1024];
	int key_nums=0;
	int ec_nums=0;
	while(fgets(linebuf,1024,fp)>(char *)0){
        strip(linebuf,line_strip);
		char line_first = line_strip[0];
		char line_end = line_strip[strlen(line_strip)-1];
		//大类名称,第一个名称默认OFF，第一个add和value默认0
		if(line_first == '[' && line_end == ']')
		{
			key_nums=0;
			ec_nums=0;
			cheat_file_num++;
			if (cheat_file_num > MAX_cjs_cheats -1)
			{
				cheat_file_num--;
				break;
			}
			strncpy(ec_name,&line_strip[1],strlen(line_strip)-1);
			ec_name[strlen(line_strip)-2]='\0';
			chj[cheat_file_num].cheat_name=(char*)malloc(strlen(ec_name)+1);//初始化
			strcpy(chj[cheat_file_num].cheat_name,ec_name);
			chj[cheat_file_num].cheat_default=0;
			chj[cheat_file_num].cheat_key[ec_nums].key_name=(char*)malloc(strlen(delfault_OFF)+1);
			strcpy(chj[cheat_file_num].cheat_key[ec_nums].key_name,delfault_OFF);
			chj[cheat_file_num].cheat_key[ec_nums].key_add[key_nums]=0;
			chj[cheat_file_num].cheat_key[ec_nums].key_value[key_nums]=0;
			//目录设置
			st_cheat_menu[cheat_file_num].name=chj[cheat_file_num].cheat_name;
			st_cheat_menu[cheat_file_num].info=chj[cheat_file_num].cheat_name;
			st_cheat_menu[cheat_file_num].update=chj_update;
		}
		//处理大类的内容
		else if((strstr(line_strip,"=")!=NULL) && ((strstr(line_strip,",")!=NULL) ) && (cheat_file_num > -1))
		{

            /*获取key_name*/
			char *outer_ptr=NULL;
			char *temp_key_name=NULL;
			key_nums=0;
			ec_nums ++;
			temp_key_name=strtok_r(line_strip,"=",&outer_ptr);
			chj[cheat_file_num].cheat_key[ec_nums].key_name=(char*)malloc(strlen(temp_key_name)+1);
			strcpy(chj[cheat_file_num].cheat_key[ec_nums].key_name,temp_key_name);
			/*获取key_add和key_value*/
			char add_char[10];
			char value_char[10];
			char *temp_str=NULL;
			char *outer_ptr2=NULL;
			int next_add = 0;   //下一个值是不是add
			int value_nums = 0; //逗号的第几次分隔
			//首次逗号分隔，为add无需判断,且下个值为value
			temp_str=strtok_r(outer_ptr,",",&outer_ptr2);
			strcpy(add_char,temp_str);
			outer_ptr=NULL;
			//根据下个字符是分号还是逗号，判断是add还是value，分号(;)后为地址，逗号(,)默认为value
			int add_len=0;
			int value_len=0;
			char *add_pos=NULL;
			char *value_pos=NULL;
			//谁长度长，谁在前,第一个add已经读取，开始读第一个value
			while(strlen(outer_ptr2)>0)
			{
				add_pos=strstr(outer_ptr2,";");
				value_pos=strstr(outer_ptr2,",");
				add_len=0;
				value_len=0;
				if (add_pos != NULL) add_len=strlen(add_pos);
				if (value_pos != NULL) value_len=strlen(value_pos);

				if (add_len > value_len)
				{
					temp_str=strtok_r(NULL,";",&outer_ptr2);
					next_add = 1;
					value_nums++;
				}
				else
				{
					temp_str=strtok_r(NULL,",",&outer_ptr2);
					value_nums++;
					if(next_add ==1)
					{
						value_nums = 0;
						next_add = 0;
					}
				}
				/***
				开始判断分隔出的字符
				***/
				//如果是分号分出来的，是add
				if (value_nums==0)
				{
					strcpy(add_char,temp_str);
				}
				//如果是逗号分出来的，是value
				else if(value_nums > 0)
				{
					//第一次逗号分出
					if(value_nums==1)
					{
						strcpy(value_char,temp_str);
					}
					//非第一次逗号分出，连续分出
					else if(value_nums > 1)
					{
						value_add(add_char);
						strcpy(value_char,temp_str);
					}
					//检验add和value符合性
					if(check_add_value(add_char, value_char))
					{
						sscanf(add_char,"%06x",&chj[cheat_file_num].cheat_key[ec_nums].key_add[key_nums]);
						sscanf(value_char,"%02x",&chj[cheat_file_num].cheat_key[ec_nums].key_value[key_nums]);
						key_nums ++;
						if(key_nums ==MAX_cheat_keys) break;
					}
				}
			}
			//没有合格的add和value，中断寻找和中断文件处理
			if( key_nums ==0)
			{
				cheat_file_num--;
				break;
			}
		}
		//没有符合格式的文件行，中断文件处理
		else if(strlen(line_strip) > 0)
		{
            fprintf(stderr, "有符合格式的文件行，中断文件处理\n");

            cheat_file_num--;
			break;
		}
		chj[cheat_file_num].cheat_max=ec_nums;
	}
	fclose (fp);
	return 1;
	
	


}


void chj_update(unsigned long key,int ec_index) {
	int val=chj[ec_index].cheat_default;
	int max_val=chj[ec_index].cheat_max;
	if (key == DINGOO_RIGHT){
		if (val < max_val){
			val ++;
			}
		}
	else if (key == DINGOO_LEFT){
		if (val > 0){
			val --;
			}
		}
	chj[ec_index].cheat_default=val;
	
	if (cheat_file_num > -1){
		 FCEU_Cheat_chj();
		}
	

}



//log
void str_log(char *info,char *log)
{
	//获取时间
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);//获取Unix时间戳。
	timeinfo = localtime (&rawtime);;//转为时间结构。
	char now_time[128];
	strftime (now_time,sizeof(now_time),"%Y-%m-%d,%H:%M:%S - :",timeinfo);

	//log记录
	char *fn=NULL;
	fn=strdup(FCEU_MakeFName(FCEUMKF_CHEAT,0,0).c_str());
	//fn = "./debug.chj";
	char *file_name=stringcat(fn,"_debug.log");
	free(fn);
	FILE *fp_log = fopen (file_name,"w");
	free(file_name);
	//if(!fp_log) return 1;
	fputs(now_time, fp_log);
	fputs(info, fp_log);
	fputs(":", fp_log);
	fputs(log, fp_log);
	fputs("\n", fp_log);
	fflush(fp_log);
	fclose(fp_log);


}


void value_add(char *s)
{
	char *ptr=NULL;
	int add_ret=0;
	char add_num[20];
	add_ret = strtol(s, &ptr, 16);
	add_ret ++;
	sprintf(add_num,"%x",add_ret);
	strcpy(s,add_num);

}
/***
去除字符串中空字符
***/
char *strip(char *fu_in_strs,char *fu_out_strs)
{
	char *r = fu_out_strs;
	while(*fu_in_strs != '\0')
	{
		if(*fu_in_strs ==' ' || *fu_in_strs == '\t' || *fu_in_strs == '\r' || *fu_in_strs == '\n')
		{
			fu_in_strs++;
		}
		else
		{
			*r++ = *fu_in_strs++;
		}
	}
	*r='\0';
	return fu_out_strs;
}


/***
检测add和value是不是符合要求，add位数大于2，value位数小于3，且2者都要是16进制
***/
bool check_add_value(char *add, char *value)
{
	if ( (strlen(add) >= 2) && (check_hex(add)) && (strlen(value) < 3) && (check_hex(value)) )
	{
		return true;
	}
	else
	{
		return false;
	}

}
/***
连接字符串
***/
char* join3(char *s1, char *s2)
{
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);  //+1 for the zero-terminator
    if (result == NULL) return result;
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* str_join(int str_nums, ...)
{
    int i = 0;
    int str_total = 0;
    va_list args;
    va_start(args, str_nums);
    for (i=0;i<str_nums;i++){
        str_total += strlen(va_arg(args, char*));
    }
    va_start(args, str_nums);
    char *result = (char*)malloc(str_total+1);  //+1 for the zero-terminator
    for (i=0;i<str_nums;i++){
        if (i==0) strcpy(result, va_arg(args, char*));
        else strcat(result, va_arg(args, char*));
    }
    va_end(args);
    return result;
}

char* str_modifi(char *s1, const char *s2)
{
    int str_total = strlen(s1);
    char* str_temp = NULL;
    str_temp = strrchr(s1, '.');
    if (str_temp){
        int str_left_len = str_total-strlen(str_temp)+1; //包含点.
        char *result = (char*)malloc(str_left_len+strlen(s2)+1);  //+1 for the zero-terminator
        strncpy(result, s1, str_left_len);
        result[str_left_len]='\0';
        strcat(result, s2);
        return result;
    }
    else {
        char *result = (char*)malloc(str_total+strlen(s2)+1);  //+1 for the zero-terminator
        strcpy(result, s1);
        strcat(result, s2);
        return result;
    }
}


extern char FileBase[2048];

char* str_cheatname(char *fname, const char *ex)
{
    static char dir [1024];
    //char fname [SAL_MAX_PATH];
    char ext [1024];

    strcpy(dir, "/usr/local/home/.fceux");
    strcat(dir, "/cheats/");
    strcat(dir,FileBase);
    strcat (dir, ex);
    fprintf(stderr,"str_cheatname  %s\n",dir);
    return (dir);
}


char* gnu_basename(char *pathname)
{
	char *base;
	if ((base = strrchr (pathname, '/')) != NULL || (base = strrchr (pathname, '\\')) != NULL)
	{
		return (base + 1);
	}
	else
	{
		return (pathname);
	}
}



/***
字符串是不是都是16进制字符判断
***/
int check_hex(char *test_chars)
{
	while(*test_chars != '\0')
	{
		if (!(isxdigit(*test_chars)))
		{
			return 0;
		}
		test_chars++;
	}
	return 1;

}

