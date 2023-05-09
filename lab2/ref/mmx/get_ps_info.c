#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>
#define max_len 500
struct task_exist{
	int PID;
	char command[16];
	int running;
	double time;
	double CPU;
};
int main(void)
{
	int sum1, sum2;								//the number of process
	int PID1[max_len],PID2[max_len];						//the PID of process
	char COMMAND1[max_len][16],COMMAND2[max_len][16];				//the name of process
	long running1[max_len],running2[max_len];					//if the process is running
	unsigned long long time1[max_len],time2[max_len];				//sum exec time
	struct task_exist task[max_len];						//the process to print
	int i,j;
	int k=0;
	syscall(332,&sum1,PID1,COMMAND1,running1,time1);				//the process information of the last time
	while(1){
		sleep(1);
		system("clear");
		syscall(332,&sum2,PID2,COMMAND2,running2,time2);			//the process information of the next time
		k=0;
		//find the process which exist both in the last time and next time,copy it to task struct
		for(i=0;i<sum2;i++){
			for(j=0;j<sum1;j++){
				if(PID2[i]==PID1[j]){					//if the process exist both in last and next,store and print
					task[k].PID=PID2[i];
					task[k].running=(int)running2[i]?0:1;
					task[k].time=time2[i]/1000000000.0;
					task[k].CPU=(time2[i]-time1[j])/10000000.0;
					strcpy(task[k].command, COMMAND2[i]);
					k++;						//the number of the process exist in both
					break;
				}
			}
		}
		//sort through CPU
		int max_num;								//CPU max num
		double max;								//CPU max
		int flag[max_len];							//if the information has been printed
		for(i=0;i<max_len;i++)
			flag[i]=1;
		printf("%-5s%-16s%-10s%-10s%-10s\n", "PID", "COMM", "ISRUNNING", "%CPU","TIME");	//print the directory
		//find the first 20 process,whose CPU is max
		for(i=0;i<20&&i<k;i++){
			for(j=0;j<k;j++){
				if(flag[j]){
					max=task[j].CPU;
					max_num=j;
					break;
				}
			}
			for(;j<k;j++){
				if(task[j].CPU>max&&flag[j]){
					max=task[j].CPU;
					max_num=j;
				}
			}
			printf("%-5d%-16s%-10d%-10.2lf%-10.2lf\n", task[max_num].PID, task[max_num].command, task[max_num].running, task[max_num].CPU,task[max_num].time);		//print the information of the process whose CPU is max
			flag[max_num]=0;
		}
		//update last time<-next time
		sum1=sum2;
		for(i=0;i<sum2;i++){
			PID1[i]=PID2[i];
			strcpy(COMMAND1[i],COMMAND2[i]);
			running1[i]=running2[i];
			time1[i]=time2[i];
		}
	}
	return 0;
}
