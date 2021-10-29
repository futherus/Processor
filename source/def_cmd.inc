
DEF_CMD(put,  15612217495837228428ULL, 3,
{
    fprintf(ostream, "PUT\n");

    stack_push(&cpu->stk, dargs[0]);
    stack_push(&cpu->stk, dargs[1]);
    stack_push(&cpu->stk, dargs[2]);
})
DEF_CMD(in,     590638987843676716ULL, 0,
{
    fprintf(ostream, "IN\n");
})
DEF_CMD(out,  15613325803558276075ULL, 0,
{
    stack_pop(&cpu->stk, &dtemp[0]);
    fprintf(ostream, "OUT: %lg", dtemp[0]);
})
DEF_CMD(dump,  1523291442054197683ULL, 0,
{
    fprintf(ostream, "DUMP\n");

    stack_dump(&cpu->stk, "Stack dump\n");
})
