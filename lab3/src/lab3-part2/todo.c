// func == 1
static void scan_vma(void)
{
    struct mm_struct *mm;
    printk("func == 1, %s\n", __func__);
    struct mm_struct *mm = get_task_mm(my_task_info.task);
    if (mm)
    {
        // TODO :遍历VMA将VMA的个数记录到my_task_info的vma_cnt变量中
        struct vm_area_struct *vm_area = mm->mmap;
        while (vm_area != NULL) { // 遍历vm_area_struct结构体
            my_task_info.vma_cnt++;
            vm_area = vm_area->vm_next; // 指向下一个vm_area_struct结构体
        }
        mmput(mm);
    }
}

// func == 2
static void print_mm_active_info(void)
{
    int first = 1;
    struct mm_struct *mm;
    unsigned long virt_addr;
    printk("func == 2, %s\n", __func__);
    // TODO : 1. 遍历VMA，并根据VMA的虚拟地址得到对应的struct page结构体（使用mfollow_page函数）
    // struct page *page = mfollow_page(vma, virt_addr, FOLL_GET);
    // unsigned int unused_page_mask;
    // struct page *page = mfollow_page_mask(vma, virt_addr, FOLL_GET, &unused_page_mask);
    // TODO : 2. 使用page_referenced活跃页面是否被访问，并将被访问的页面物理地址写到文件中
    // kernel v5.13.0-40及之后可尝试
    // unsigned long vm_flags;
    // int freq = mpage_referenced(page, 0, (struct mem_cgroup *)(page->memcg_data), &vm_flags);
    // kernel v5.9.0
    // unsigned long vm_flags;
    // int freq = mpage_referenced(page, 0, page->mem_cgroup, &vm_flags);
    mm = get_task_mm(my_task_info.task);
    if (mm) {
        struct vm_area_struct *vm_area = mm->mmap; // 获取mm_struct结构体中的vm_area_struct结构体
        while (vm_area) { // 遍历vm_area_struct结构体
            pr_info("%lx", vm_area->vm_flags); // 打印VMA标志信息
            virt_addr = vm_area->vm_start;
            while (virt_addr != vm_area->vm_end) { // 遍历VMA中的虚拟地址
                struct page *page = mfollow_page(vm_area, virt_addr, FOLL_GET | FOLL_TOUCH); 
                // 获取虚拟地址对应的struct page结构体
                if (!IS_ERR_OR_NULL(page)) {
                    // 如果页面被访问，则将其物理地址转换为字符串，并将其写入文件。
                    //使用一个字符串缓冲区`str_buf`来存储物理地址的字符串表示。
                    //通过`sprintf()`函数将每个页面的物理地址追加到`str_buf`中，
                    //然后调用`write_to_file()`函数将`str_buf`写入文件。
                    unsigned long vm_flags; // 用于存储页面的标志
                    int freq = mpage_referenced(page, 0, (struct mem_cgroup *)(page->memcg_data), &vm_flags); 
                    // 判断页面是否被访问
                    if (freq != 0) {
                        if (first) { // 判断是否是第一次写入文件
                            sprintf(str_buf, "%lu", page_to_pfn(page)); 
                            // 将页面的物理页帧号pfn转换为字符串，并将其存储在 str_buf 缓冲区中
                            first = 0; // 将first置为0, 不是第一次了
                        } else {
                            sprintf(str_buf, ",%lu", page_to_pfn(page)); 
                            // 将页面的物理页帧号转换为字符串，并追加到 str_buf 缓冲区中，以逗号分隔多个物理地址
                        }
                        write_to_file(str_buf, strlen(str_buf)); 
                    }
                }
                virt_addr += PAGE_SIZE;
            }
            vm_area = vm_area->vm_next;
        }
        sprintf(str_buf, "\n");
        write_to_file(str_buf, strlen(str_buf));
        mmput(mm);
    }
}

static unsigned long virt2phys(struct mm_struct *mm, unsigned long virt)
{
    struct page *page = NULL;
    // TODO : 多级页表遍历：pgd->pud->pmd->pte，然后从pte到page，最后得到pfn
    pgd_t *pgd = pgd_offset(mm, virt); // 1级页表
    p4d_t *p4d = p4d_offset(pgd, virt); // 4级页表
    pud_t *pud = pud_offset(p4d, virt); // 3级页表
    pmd_t *pmd = pmd_offset(pud, virt); // 2级页表
    pte_t *pte = pte_offset_kernel(pmd, virt); 
    
    if (pte_present(*pte)) // 判断pte是否存在
    {
        page = pte_page(*pte); // 获取pte对应的page
        if (page)
        {
            return page_to_phys(page); // 获取page对应的物理地址
        }
    }
    
    pr_err("func: %s page is NULL\n", __func__);
    return 0;
}

// func = 3
static void traverse_page_table(struct task_struct *task)
{
    struct mm_struct *mm;
    printk("func == 3, %s\n", __func__);
    struct mm_struct *mm = get_task_mm(my_task_info.task);
    if (mm)
    {
        // TODO :遍历VMA，并以PAGE_SIZE为粒度逐个遍历VMA中的虚拟地址，然后进行页表遍历
        // record_two_data(virt_addr, virt2phys(task->mm, virt_addr));
        struct vm_area_struct *vm_area = mm->mmap;
        while (vm_area) {
            unsigned long virt_addr = vm_area->vm_start;
            while (virt_addr != vm_area->vm_end) { // 遍历VMA中的虚拟地址
                struct page *page = mfollow_page(vm_area, virt_addr, FOLL_GET); // 获取虚拟地址对应的struct page结构体
                if (!IS_ERR_OR_NULL(page)) { // 判断page是否存在
                    sprintf(str_buf, "0x%lx--0x%lx--0x%lx\n", virt_addr, page_to_pfn(page),virt2phys(mm,virt_addr)); 
                    // 将虚拟地址、PFN、物理地址写入文件 格式化为字符串
                    write_to_file(str_buf, strlen(str_buf));
                }
                virt_addr += PAGE_SIZE; // 虚拟地址加上PAGE_SIZE
            }
            vm_area = vm_area->vm_next;
        }
        mmput(mm); // 释放mm_struct结构体 
    }
    else
    {
        pr_err("func: %s mm_struct is NULL\n", __func__);
    }
}

// func == 4 或者 func == 5
static void print_seg_info(void)
{
    struct mm_struct *mm;
    unsigned long addr;
    unsigned long start,end;
    struct vm_area_struct *vm_area;
    printk("func == 4 or func == 5, %s\n", __func__);
    mm = get_task_mm(my_task_info.task);
    if (mm == NULL)
    {
        pr_err("mm_struct is NULL\n");
        return;
    }

    if (ktest_func == 4) {
        start = mm->start_data; // 数据段的起始地址
        end = mm->end_data;
    } else {
        start = mm->start_code; // 代码段的起始地址 
        end = mm->end_code;
    }
    vm_area = mm->mmap;
    // TODO: 根据数据段或者代码段的起始地址和终止地址得到其中的页面，然后打印页面内容到文件中
    // 提示：可以使用 follow_page 函数得到虚拟地址对应的 page，然后使用 addr = kmap_atomic(page) 得到可以直接
    // 访问的虚拟地址，然后可以使用 memcpy 函数将数据段或代码段拷贝到全局变量 buf 中以写入到文件中
    // 注意：使用 kmap_atomic(page) 结束后还需要使用 kunmap_atomic(addr) 进行释放操作
    // 正确结果：如果是运行实验提供的 workload，这一部分数据段应该会打印出 char *trace_data，
    // static char global_data[100] 和 char hamlet_scene1[8276] 的内容。

    while (vm_area) {
        unsigned long virt_addr = vm_area->vm_start;
        while (virt_addr < vm_area->vm_end) {
            if (start <= virt_addr && end >= virt_addr + PAGE_SIZE) { // 当前页完全包含在数据段或代码段内
                struct page *page = mfollow_page(vm_area, virt_addr, FOLL_GET); // 获取虚拟地址对应的struct page结构体
                if (!IS_ERR_OR_NULL(page)) {
                    addr = kmap_atomic(page); // 获取虚拟地址对应的物理地址
                    memcpy(buf, addr, PAGE_SIZE); // 将PAGE_SIZE大小的数据从addr拷贝到buf中
                    kunmap_atomic(addr); // 释放addr
                    write_to_file(buf, PAGE_SIZE); // 将buf中的PAGE_SIZE大小的数据写入文件
                }
            }
            else if (end >= virt_addr && end <= virt_addr + PAGE_SIZE) { // 当前页的部分数据在数据段或代码段内
                struct page *page = mfollow_page(vm_area, virt_addr, FOLL_GET); 
                if (!IS_ERR_OR_NULL(page)) {
                    addr = kmap_atomic(page);
                    memcpy(buf, addr, end - virt_addr); // 将end - virt_addr大小的数据从addr拷贝到buf中
                    kunmap_atomic(addr);
                    write_to_file(buf, end - virt_addr);
                }
            }
            else if (start >= virt_addr && start <= virt_addr + PAGE_SIZE) { // 当前页的部分数据在数据段或代码段内
                struct page *page = mfollow_page(vm_area, virt_addr, FOLL_GET);
                if (!IS_ERR_OR_NULL(page)) {
                    addr = kmap_atomic(page);
                    memcpy(buf, addr + start - virt_addr, virt_addr + PAGE_SIZE - start);
                    kunmap_atomic(addr);
                    write_to_file(buf, virt_addr + PAGE_SIZE - start);
                }
            }
            virt_addr += PAGE_SIZE;
        }
        vm_area = vm_area->vm_next;
    }
    mmput(mm); // 释放mm_struct结构体
}

