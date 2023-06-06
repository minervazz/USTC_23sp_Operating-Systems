/*
 * memlib.c - a module that simulates the memory system.  Needed because it 
 *            allows us to interleave calls from the student's malloc package 
 *            with the system's malloc package in libc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "memlib.h"
#include "config.h"

/* private variables */
static char *mem_start_brk;  /* points to first byte of heap */
static char *mem_brk;        /* points to last byte of heap */
static char *mem_max_addr;   /* largest legal heap address */ 

/* 
 * mem_init - initialize the memory system model
 */
void mem_init(void)
{
    /*
        TODO : 
        调用 sbrk, 初始化 mem_start_brk、mem_brk、以及 mem_max_addr
        此处增长堆空间大小为 MAX_HEAP
    */
    mem_start_brk = sbrk(MAX_HEAP); // 通过系统调用 sbrk 请求分配 MAX_HEAP 大小的堆空间
    if(mem_start_brk == (void *)-1){  // sbrk()函数执行失败，返回-1
        exit(-1);
    }
    mem_brk = mem_start_brk; // 将堆的当前位置设置为起始位置
    mem_max_addr = mem_start_brk + MAX_HEAP; // 将堆的最大地址设置为起始位置加上 MAX_HEAP 的大小. mem_max_addr指向堆的最大地址
}

/* 
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void)
{
    free(mem_start_brk);
}

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk()
{
    mem_brk = mem_start_brk;
}

/* 
 * mem_sbrk - simple model of the sbrk function. Extends the heap 
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
void *mem_sbrk(int incr) 
{
    char *old_brk = mem_brk; // 保存当前的堆位置
    /*
        TODO :
            模拟堆增长
            incr: 申请 mem_brk 的增长量
            返回值: 旧 mem_brk 值
        HINTS:
        1. 若 mem_brk + incr 没有超过实际的 mem_max_addr 值，直接推进 mem_brk 值即可
        2. 若 mem_brk + incr 超过实际的 mem_max_addr 值，需要调用 sbrk 为内存分配器掌管的内存扩容
        3. 每次调用 sbrk 时， mem_max_addr 增量以 MAXHEAP 对齐
    */
    if ((mem_brk + incr) > mem_max_addr) { // 如果堆的当前位置加上增长量大于堆的最大地址
        int aligned_incr = ((incr + MAXHEAP - 1) / MAXHEAP) * MAXHEAP; // 将增长量对齐到 MAXHEAP
        
        // 调用 sbrk 为内存分配器掌管的内存扩容
        if (sbrk(aligned_incr) == (void *)-1) {
            return (void *)-1; // 分配内存失败时返回错误值
        }
        
        mem_max_addr += aligned_incr; // 每次调用 sbrk 时，mem_max_addr 增加对齐后的增长量
    }
    
    mem_brk += incr; // 推进堆的当前位置增加增长量
    return (void *)old_brk; // 返回旧的 mem_brk 值
}


/*
 * mem_heap_lo - return address of the first heap byte
 */
void *mem_heap_lo()
{
    return (void *)mem_start_brk;
}

/* 
 * mem_heap_hi - return address of last heap byte
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize() 
{
    return (size_t)(mem_brk - mem_start_brk);
}

/*
 * mem_pagesize() - returns the page size of the system
 */
size_t mem_pagesize()
{
   /*return (size_t)getpagesize();*/
   return 4096;
}


