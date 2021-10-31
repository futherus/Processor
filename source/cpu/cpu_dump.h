#ifndef CPU_DUMP_H
#define CPU_DUMP_H

const char CPU_DUMPFILE[]  = "cpu_logs/cpu_dump.txt";

void cpu_dump_init();

void cpu_dump(CPU* cpu, size_t* ip);

#endif // CPU_DUMP_H
