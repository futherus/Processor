/* System commands of processor */

DEF_SYSCMD(hal,  15619821718256421904ULL, 0,
{
    //fprintf(ostream, "HAL\n");

    return CPU_NOERR;
})

/*  Can get only one-literal arg.
    Used for decomposing arguments.
    Can be used for immensive reading from memory. */
DEF_SYSCMD(push, 12441716782781469529ULL, 1, 
{
    //fprintf(ostream, "PUSH\n");
    cpu->reg_sz++;
    stack_push(&cpu->stk, *cpu->reg_ptr);
})

/*  Can get only one-literal arg.
    Used for decomposing arguments.
    The ONLY command able to write to memory. */
DEF_SYSCMD(pop,  15612228490953510534ULL, 1,
{
    //fprintf(ostream, "POP\n");

    stack_pop(&cpu->stk, cpu->reg_ptr);
})

DEF_SYSCMD(add,  15626593610373248958ULL, 0,
{
    //fprintf(ostream, "ADD\n");

    stack_pop(&cpu->stk, &dtemp[0]);
    stack_pop(&cpu->stk, &dtemp[1]);

    stack_push(&cpu->stk, dtemp[1] + dtemp[0]);
})

DEF_SYSCMD(sub,  15609385153883524641ULL, 0,
{
    //fprintf(ostream, "SUB\n");

    stack_pop(&cpu->stk, &dtemp[0]);
    stack_pop(&cpu->stk, &dtemp[1]);

    stack_push(&cpu->stk, dtemp[1] - dtemp[0]);
})

DEF_SYSCMD(mul,  15615019051465342581ULL, 0,
{
    stack_pop(&cpu->stk, &dtemp[0]);
    stack_pop(&cpu->stk, &dtemp[1]);

    stack_push(&cpu->stk, dtemp[1] * dtemp[0]);
    
    //fprintf(ostream, "MUL\n");
})
DEF_SYSCMD(div,  15623727183559070478ULL, 0,
{
    stack_pop(&cpu->stk, &dtemp[0]);
    stack_pop(&cpu->stk, &dtemp[1]);

    stack_push(&cpu->stk, dtemp[1] / dtemp[0]);

    //fprintf(ostream, "DIV\n");
})

DEF_SYSCMD(jmp,  15618106480116791414ULL, 1,
{
    //fprintf(ostream, "JMP\n");

    memcpy(&bin->ip, &cpu->reg_temp_64b, sizeof(size_t));
})

DEF_SYSCMD(ja,     590637888332048622ULL, 1,
{
    //fprintf(ostream, "JA\n");

    stack_pop(&cpu->stk, &dtemp[0]);
    stack_pop(&cpu->stk, &dtemp[1]);

    if(dtemp[1] > dtemp[0])
        memcpy(&bin->ip, &cpu->reg_temp_64b, sizeof(size_t));
})

DEF_SYSCMD(jae,   15618119674256329743ULL, 1,
{
    //fprintf(ostream, "JAE\n");
    
    stack_pop(&cpu->stk, &dtemp[0]);
    stack_pop(&cpu->stk, &dtemp[1]);

    if(dtemp[1] >= dtemp[0])
        memcpy(&bin->ip, &cpu->reg_temp_64b, sizeof(size_t));
})

DEF_SYSCMD(je,     590637888332048618ULL, 1,
{
    //fprintf(ostream, "JE\n");

    stack_pop(&cpu->stk, &dtemp[0]);
    stack_pop(&cpu->stk, &dtemp[1]);

    if(dtemp[1] == dtemp[0])
        memcpy(&bin->ip, &cpu->reg_temp_64b, sizeof(size_t));
})

DEF_SYSCMD(call,  2069086815125772341ULL, 1,
{
    //fprintf(ostream, "CALL\n");

    memcpy(&dtemp[0], &cpu->reg_sz, sizeof(size_t));
    memcpy(&dtemp[1], &bin->ip,    sizeof(size_t));
    
    stack_push(&cpu->stk, dtemp[0]);    // push reg_sz
    stack_push(&cpu->stk, dtemp[1]);    // push ip

    memcpy(&bin->ip, &cpu->reg_temp_64b, sizeof(size_t)); // jump
})

DEF_SYSCMD(ret,  15610251569046365794ULL, 0,
{
    //fprintf(ostream, "RET\n");
        
    stack_pop(&cpu->stk, &dtemp[0]);     // pop ip
    stack_pop(&cpu->stk, &dtemp[1]);     // pop reg_sz

    memcpy(&cpu->reg_sz, &dtemp[1], sizeof(size_t));
    memcpy(&bin->ip, &dtemp[0], sizeof(size_t));
})
