//
// Created by littlehui on 2022/1/3.
//

#include "fileLog.h"

void writeLogLittlehui(char* info, char* log) {
    //获取时间
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);//获取Unix时间戳。
    timeinfo = localtime (&rawtime);;//转为时间结构。
    char now_time[128];
    strftime (now_time,sizeof(now_time),"%Y-%m-%d,%H:%M:%S - :",timeinfo);

    //log记录
    //char *fn=NULL;
    //fn=strdup(FCEU_MakeFName(FCEUMKF_CHEAT,0,0).c_str());
    //fn = "./debug.chj";
    //char *file_name=stringcat(log,"_debug.log");
    //free(fn);

    FILE *fp_log = fopen (log,"a+");
    //free(file_name);
    //if(!fp_log) return 1;
    fputs(now_time, fp_log);
    fputs(info, fp_log);
    fputs(":", fp_log);
    fputs(log, fp_log);
    fputs("\n", fp_log);
    fflush(fp_log);
    fclose(fp_log);
}

void str_log(char *info)
{
#ifdef MAKLOG
    writeLogLittlehui(info, "/mnt/fc.log");
#endif

}


