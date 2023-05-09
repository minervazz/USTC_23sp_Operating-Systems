asmlinkage long sys_ps_counter(int __user * num);

asmlinkage long sys_ps_info(pid_t* __user * user_pid, char* __user * user_name, unsigned long long * __user * user_time, unsigned long long * __user * old_user_time, long * __user * user_state);


