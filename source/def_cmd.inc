
DEF_CMD(put,  15612217495837228428ULL, 1,
{
    fprintf(ostream, "PUT\n");

    stack_push(&cpu.stk, dregs[0]);
})
DEF_CMD(in,     590638987843676716ULL, 0,
{
    fprintf(ostream, "IN\n");
})
DEF_CMD(out,  15613325803558276075ULL, 0,
{
    fprintf(ostream, "OUT\n");
})
DEF_CMD(mul,  15615019051465342581ULL, 0,
{
    fprintf(ostream, "MUL\n");
})
DEF_CMD(dump,  1523291442054197683ULL, 0,
{
    fprintf(ostream, "DUMP\n");

    stack_dump(&cpu.stk, "Stack dump\n");
})
