#ifndef CPU_DUMP_H
#define CPU_DUMP_H

void cpu_dump_init();

void cpu_dump(CPU* cpu, size_t* ip);

void cpu_dump_cmd(const char* cmd_txt, val64_t* args, unsigned char args_sz);

#endif // CPU_DUMP_H
