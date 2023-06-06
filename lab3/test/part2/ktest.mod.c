#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x32e21920, "module_layout" },
	{ 0x4ca7c5e3, "sysfs_remove_group" },
	{ 0x9e2fb826, "filp_close" },
	{ 0x85c71d85, "kthread_stop" },
	{ 0xdda5fd5d, "sysfs_create_group" },
	{ 0xa60c02a1, "mm_kobj" },
	{ 0x45937b6f, "wake_up_process" },
	{ 0x2da00799, "kthread_create_on_node" },
	{ 0x1d19f77b, "physical_mask" },
	{ 0xb3378a7b, "pv_ops" },
	{ 0xdad13544, "ptrs_per_p4d" },
	{ 0xa92ec74, "boot_cpu_data" },
	{ 0xa648e561, "__ubsan_handle_shift_out_of_bounds" },
	{ 0x72d79d83, "pgdir_shift" },
	{ 0x9fb74541, "kernel_write" },
	{ 0x69acdf38, "memcpy" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0x5deac1f6, "mmput" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xa916b694, "strnlen" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0xb8fe9a11, "get_task_mm" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x4482cdb, "__refrigerator" },
	{ 0xb36bac5, "freezing_slow_path" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x54496b4, "schedule_timeout_interruptible" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0x7ab88a45, "system_freezing_cnt" },
	{ 0xa0c4c91b, "filp_open" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xe9080f4c, "set_user_nice" },
	{ 0x9e61bb05, "set_freezable" },
	{ 0xa7bfbf2f, "current_task" },
	{ 0x4d292ca3, "pid_task" },
	{ 0x50b27389, "find_get_pid" },
	{ 0x800473f, "__cond_resched" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x92997ed8, "_printk" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x5c3c7387, "kstrtoull" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "FDD572332164B5BBD2592C2");
