SYSCALL_DEFINE1(ps_counter, int __user *, num) {
	struct task_struct* task;
	int counter = 0, err;
	printk("[Syscall] ps_counter\n");
	for_each_process(task) {
		counter ++;
	}
	err = copy_to_user(num, &counter, sizeof(int));
	return 0;
}

SYSCALL_DEFINE5(ps_info, pid_t * __user *, user_pid, char* __user * , user_name, unsigned long long * __user *, user_time, unsigned long long * __user *, user_old_time,  long * __user *, user_state) {
	struct task_struct* task;
	int i = 0, j = 0, k = 0, cnt = 0, err = 0;
	char name_a[1024];
	pid_t pid_a[128];
	unsigned long long old_time, cpu_time;
	for(k = 0; k < 1024; k++) name_a[k] = ' ';

	printk("[Syscall] ps_info\n");

	for_each_process(task) {

		// err = copy_to_user(user_pid + i, &(task -> pid) , sizeof(pid_t)); // This line has unknown bug. You may try and find out why.
		pid_a[i] = task -> pid;
		err = copy_from_user(&(old_time), user_old_time + i, sizeof(unsigned long long));
		cpu_time = task -> se.sum_exec_runtime - old_time;
    	//Pass the data one by one to save the stack space.
		err = copy_to_user(user_time + i, &(cpu_time), sizeof(unsigned long long));
		err = copy_to_user(user_state + i, &(task -> state), sizeof(long));
    	//Use space as delimiter to store process names in a char array.
		for(j = 0; j < 16; j++) {
			if(task -> comm[j] != ' ' && task -> comm[j] != '\0') {
				name_a[cnt + j] = task -> comm[j];
			}
			else {
				name_a[cnt + j] = ' ';
				cnt += j + 1;
				break;
			}
		}

		i++;
	}

	err = copy_to_user(user_name, name_a, sizeof(name_a));
	err = copy_to_user(user_pid, pid_a, sizeof(pid_a));

	return 0;
}