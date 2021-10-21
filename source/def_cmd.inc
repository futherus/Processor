/*      txt_code     n_args       */
DEF_CMD(in,          0,
{
    fprintf(ostream, "NOCODEYET");

    break;
})
DEF_CMD(out,         0,
{
    arg_t temp = 0;
    stack_pop(&stk, &temp);

    fprintf(ostream, "%lg ", temp);
                
    break;
})
DEF_CMD(mul,         0,
{
    fprintf(ostream, "NOCODEYET");

    break;
})
DEF_CMD(add,         0,
{
    arg_t fst_term = 0;
    arg_t snd_term = 0;
    stack_pop(&stk, &fst_term);
    stack_pop(&stk, &snd_term);

    stack_push(&stk, fst_term + snd_term);

    break;
})
DEF_CMD(hal,         0,
{
    fprintf(ostream, "NOCODEYET");

    break;
})
DEF_CMD(push,         1, 
{
    arg_t arg = 0;
    get_arg(&arg, bin);

    stack_push(&stk, arg);
                
    break;
})
DEF_CMD(dump,         0,
{
    stack_dump(&stk, "CPU dump");

    break;
})
