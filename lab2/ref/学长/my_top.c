#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

typedef struct ps_array
{
    pid_t pid_a[128];
    char name_a[1024];
    unsigned long long cpu_time_a[128];
    long state_a[128];
}ps_array;

int split_string(char* string, char *sep, char** string_clips) {
    
    char string_dup[1024];
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

int main()
{
    ps_array user_array;
    int  i = 0, j = 0, cnt;
    int p_out[128];
    char* ps_name[128];
    char out_name[1024];
    char* n_out[128];
    unsigned long long old_time[128];
    long ps_state[128], s_out[128];
    double ps_time[128], t_out[128];

    for(i = 0; i < 128; i++) old_time[i] = 0;

    while(1){
        syscall(332, &cnt);
        syscall(333, &(user_array.pid_a), &(user_array.name_a), &(user_array.cpu_time_a), &(old_time), &(user_array.state_a));
        strcpy(out_name, user_array.name_a);
        int pieces = split_string(user_array.name_a, " ", ps_name);
        int pieces_out = split_string(out_name," ", n_out);
        for(i = 0; i < cnt; i++) {
            if(!user_array.state_a[i]) ps_state[i] = 1;
            else ps_state[i] = 0;
            old_time[i] = user_array.cpu_time_a[i] + old_time[i];
            ps_time[i] = (((double) (user_array.cpu_time_a[i]))/1000000000);
        
            s_out[i] = ps_state[i]; t_out[i] = ps_time[i]; p_out[i] = user_array.pid_a[i];
        }

      //冒泡排序 这里相当于每次都copy一份user_array之后用这个copy做排序 （如果用原数组排序而忽略内核态函数for_each_process生成process的顺序会导致混乱）
      
        for(i = 0; i < cnt; i++) {
            for(j = 0; j < cnt - i -1; j++){
                if(t_out[j] < t_out[j + 1]){
                    char * n_tmp = n_out[j + 1]; n_out[j + 1] = n_out[j]; n_out[j] = n_tmp;
                    
                    long s_tmp = s_out[j + 1]; double t_tmp = t_out[j + 1]; pid_t p_tmp = p_out[j + 1];
                    s_out[j + 1] = s_out[j]; t_out[j + 1] = t_out[j]; p_out[j + 1] = p_out[j];
                    s_out[j] = s_tmp; t_out[j] = t_tmp; p_out[j] = p_tmp;
                }
            }
        }

        printf("PID               COMM        CPU          ISRUNNING\n");
        //for(i = 0; i < 20; i++) printf("%-5d %*s %10.5f%% %18ld\n", user_array.pid_a[i], 16, ps_name[i], ps_time[i], ps_state[i]);
        for(i = 0; i < 20; i++) printf("%-5d %*s %10.5f%% %18ld\n", p_out[i], 16, n_out[i], t_out[i], s_out[i]);
        
        sleep(1);
        system("clear");
    }
}