#include "gpt.h"
#include "drivers/disk/ahci/ahci.h"
#include "memory/pmm.h"
#include "drivers/tty/tty.h"

void fs::gpt::read_gpt_info(uint disk_no)
{
    parition_table_header* gpt_header = (parition_table_header *)memory::pmm::request_page();
    io::disk::ahci::command_read(disk_no, 1, 1, (void *)gpt_header);
}