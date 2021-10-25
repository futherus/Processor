/* System commands of processor */

DEF_SYSCMD(hal,  15619821718256421904ULL, 0,
{
    fprintf(ostream, "HAL\n");
})

/*  Can get only one-literal arg.
    Used for decomposing arguments.
    Can be used for immensive reading from memory. */
DEF_SYSCMD(push, 12441716782781469529ULL, 1, 
{
    fprintf(ostream, "PUSH\n");

    stack_push(&cpu.stk, *cpu.reg_ptr);
})

/*  Can get only one-literal arg.
    Used for decomposing arguments.
    The ONLY command able to write to memory. */
DEF_SYSCMD(pop,  15612228490953510534ULL, 1,
{
    fprintf(ostream, "POP\n");

    stack_pop(&cpu.stk, cpu.reg_ptr);
})

DEF_SYSCMD(add,  15626593610373248958ULL, 0,
{
    fprintf(ostream, "ADD\n");

    stack_pop(&cpu.stk, &dregs[1]);
    stack_pop(&cpu.stk, &dregs[0]);

    stack_push(&cpu.stk, dregs[0] + dregs[1]);
})

DEF_SYSCMD(sub,  15609385153883524641ULL, 0,
{
    fprintf(ostream, "SUB\n");

    stack_pop(&cpu.stk, &dregs[1]);
    stack_pop(&cpu.stk, &dregs[0]);

    stack_push(&cpu.stk, dregs[0] - dregs[1]);
})
