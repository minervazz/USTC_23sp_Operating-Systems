#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/syscall.h>
#include<sys/types.h>

typedef unsigned long int       uint64_t; 
typedef uint64_t  u64;

#define TASK_COMM_LEN 16
#define MAX_CMDLINE_LENGTH  1024    /* max cmdline length in a line*/
#define MAX_BUF_SIZE        4096    /* max buffer size */
#define MAX_CMD_ARG_NUM     32      /* max number of single command args */

struct topinfo
{
    pid_t pid;
    char comm[TASK_COMM_LEN];
    volatile long state;
    u64 sum_exec_runtime;
    int die ;//判断该行数据是否有效 0：有效/1：无效
};

/*  
    int split_string(char* string, char *sep, char** string_clips);

    基于分隔符sep对于string做分割，并去掉头尾的空格

    arguments:      char* string, 输入, 待分割的字符串 
                    char* sep, 输入, 分割符
                    char** string_clips, 输出, 分割好的字符串数组

    return:   分割的段数 
*/

int split_string(char* string, char *sep, char** string_clips) {
    
    char string_dup[MAX_BUF_SIZE];
    string_clips[0] = strtok(string, sep);
    int clip_num=0;
    
    do {
        char *head, *tail;
        head = string_clips[clip_num];
        tail = head + strlen(string_clips[clip_num]) - 1;
        while(*head == ' ' && head != tail)
            head ++;
        while(*tail == ' ' && tail != head)
            tail --;
        *(tail + 1) = '\0';
        string_clips[clip_num] = head;
        clip_num ++;
    }while(string_clips[clip_num]=strtok(NULL, sep));
    return clip_num;
}



//排序函数
void srt(int st[],u64 runtime[],int tag[], int num){
    int flag[128] = {0};
    for(int i = 0; i < 20 ; i++ ){
        u64 max = 0;
        int max_num = -1;
        // int l = len(runtime);
        for(int j = 0; j < num ; j++ ){
            if(runtime[j] >= max ){
                if(flag[j] == 0){
                    max = runtime[j];
                    max_num = j;
                }
            }
        }
        flag[max_num] = 1;
        st[i] = max_num;
    }
    // for(int i = 0; i < 20; i++) {
    //     printf("st[%d]=%d\n", i, st[i]);
    // }

    // for(int i = 0; i < 20 ; i++ ){
    //     if((st[i] == 0&&st[i-1] ==0)||(st[i] == 0&&st[i+1] ==0)){
    //         for(int j = 0 ; j<128 ; j++){
    //             if(tag[j] == 0 && flag[j] == 0){
    //                 st[i] = j;
    //             }
    //         }

    //     }
    // }
}

int main(void)
{
    int num;
    float T = 1;//刷新时间
    int flag = 0;

    struct topinfo tinfo[128];
    struct topinfo temp_tinfo[128];
    struct topinfo result[128];
    u64 runtime[128];

    /* 输入的命令行 */
    char cmdline[MAX_CMDLINE_LENGTH];
    /*增加打印当前目录，格式类似"shell:/home/oslab ->" */
    char * buf = (char *)malloc(MAX_BUF_SIZE);
    char * cwd_now = getcwd(buf,MAX_BUF_SIZE);
    printf("shell:%s -> ",cwd_now);
    fflush(stdout);//刷新缓冲区,将缓冲区内的数据输出到设备

    fgets(cmdline, 256, stdin);
    strtok(cmdline, "\n");//分解字符串cmdline为一组字符串，\n为分隔符

    char *argv[MAX_CMD_ARG_NUM];
    int argc = split_string(cmdline, " ", argv);
    if(strcmp(argv[0], "mytop") == 0){
    if(argc ==3 && strcmp(argv[1], "-b") == 0){
        T = (float)atoi(argv[2]);
    }
    while (1)
    {
        int tag[128] = {0};
        int st[20];
        // for(int i = 0; i < 128 ; i++){
        //     tag[i] = 0;
        // }
        system("clear");
        //上一次数据失效
        for(int i = 0; i < 128; i++){
            tinfo[i].die = 1;
            runtime[i] = 0;
        }
        for(int i = 0; i < 20; i++){
            st[i] = -1;
        }

        if(flag == 0){
            syscall(555, &num , (void *)result);
            for(int i = 0; i < num; i++ ){
                runtime[i] = tinfo[i].sum_exec_runtime;
            }
            flag = 1;
        }else{
            syscall(555, &num ,(void *)temp_tinfo);
            int hasFlagged = 0;
            for(int i = 0; i < num; i++ ){
                hasFlagged = 0;
                for(int j = 0; j < 128 ; j++ ){
                    if(tinfo[j].pid == temp_tinfo[i].pid){
                        result[i].state = temp_tinfo[i].state;
                        runtime[i] = temp_tinfo[i].sum_exec_runtime - tinfo[j].sum_exec_runtime;
                        result[i].sum_exec_runtime = temp_tinfo[i].sum_exec_runtime;
                        result[i].die = 0;
                        result[i].pid = tinfo[j].pid;
                        strcpy(result[i].comm, tinfo[j].comm);
                        result[i].state = tinfo[j].state;

                        tag[i] = 1;
                        hasFlagged = 1;
                        break;
                    }
                }
                if(hasFlagged == 0) {
                    result[i].state = temp_tinfo[i].state;
                    runtime[i] = temp_tinfo[i].sum_exec_runtime;
                    result[i].sum_exec_runtime = temp_tinfo[i].sum_exec_runtime;
                    result[i].die = 0;
                    result[i].pid = temp_tinfo[i].pid;
                    strcpy(result[i].comm, temp_tinfo[i].comm);
                    result[i].state = temp_tinfo[i].state;
                    tag[i] = 1;
                }
                // printf("runtime[%d]=%d\n", i, runtime[i]);

            }
            // for (int i = 0; i < num; i++)
            // {
            //     if(tag[i] == 0){
            //         for(int j = 0; j < 128 ; j++ ){
            //             if(tinfo[j].die == 1){
            //                 tinfo[j].pid = temp_tinfo[i].pid;
            //                 tinfo[j].state = temp_tinfo[i].state;
            //                 runtime[j] = temp_tinfo[i].sum_exec_runtime;
            //                 tinfo[j].sum_exec_runtime = temp_tinfo[i].sum_exec_runtime;
            //                 memcpy(tinfo[j].comm,temp_tinfo[i].comm,16);
            //                 tinfo[j].die = 0;
            //                 tag[i] = 1;
            //             }
            //         }
            //     }
            // }     
        }

        
        // for(int i = 0; i < 128 ; i++){
        //     tag[i] = tinfo[i].die;
        // }
        //排序
        srt(st,runtime,tag, num);

        //输出
        printf("PID\tS\tCPU\t\tCOMMAND\n");
        for(int i = 0 ; i < 20 ; i++){
            int m = st[i];
            float r = (float)runtime[m]/(T*1e9);
            printf("%-6d\t%-5ld\t%-3f\t%-10s\n",result[m].pid,result[m].state,r,result[m].comm);
        }
        for(int i = 0; i < 128; i++) {
            tinfo[i] = result[i];
        }
        sleep(T); 
    }
            
    }

}
    
    
    


