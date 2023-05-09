//new

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

	// 输出学号和系统调用名称
	printk("[Syscall] ps_info\n");
	printk("[StuID] PB21081601\n");

	// 遍历进程列表
	for_each_process(task) {
		pid_a[i] = task -> pid;

		// 从用户空间拷贝上一次的进程执行时间
		err = copy_from_user(&(old_time), user_old_time + i, sizeof(unsigned long long));

		// 计算当前进程执行时间和 CPU 时间
		cpu_time = task -> se.sum_exec_runtime - old_time;

    	// 将 CPU 时间拷贝到用户空间
		err = copy_to_user(user_time + i, &(cpu_time), sizeof(unsigned long long));

		// 将进程状态拷贝到用户空间
		err = copy_to_user(user_state + i, &(task -> state), sizeof(long));

    	// 使用空格作为分隔符，将进程名称存储到字符数组中
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
	// 将进程名称和 PID 拷贝到用户空间
	err = copy_to_user(user_name, name_a, sizeof(name_a));
	err = copy_to_user(user_pid, pid_a, sizeof(pid_a));

	return 0;
}