//new

asmlinkage long sys_ps_counter(int __user * num);

asmlinkage long sys_ps_info(int *num, int *pid_array, char (*command_array)[16], long *running_array, unsigned long long *time_array);
