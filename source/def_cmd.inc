DEF_CMD(sin,  15609380755837011737ULL, 1,
{
    //fprintf(ostream, "SIN\n");

    dtemp[0] = sin(dargs[0]);
    stack_push(&cpu->stk, dtemp[0]);
})
DEF_CMD(cos,  15624613389931219346ULL, 1,
{
    //fprintf(ostream, "COS\n");

    dtemp[0] = cos(dargs[0]);
    stack_push(&cpu->stk, dtemp[0]);
})
DEF_CMD(in,     590638987843676716ULL, 0,
{
    //fprintf(ostream, "IN\n");

    fscanf(istream, "%lg", &dtemp[0]);
    stack_push(&cpu->stk, dtemp[0]);
})
DEF_CMD(out,  15613325803558276075ULL, 0,
{
    stack_pop(&cpu->stk, &dtemp[0]);
    fprintf(ostream, "OUT: %lg\n", dtemp[0]);
})
DEF_CMD(dump,  1523291442054197683ULL, 0,
{
    //fprintf(ostream, "DUMP\n");

    stack_dump(&cpu->stk, "Stack dump\n");
})
DEF_CMD(pixr, 12453253958294016862ULL, 3,
{
    //fprintf(ostream, "PIXR\n");

    //fprintf(ostream, "DARGS: %lg\n", dargs[0]);
    //fprintf(ostream, "DARGS: %lg\n", dargs[1]);
    //fprintf(ostream, "DARGS: %lg\n", dargs[2]);
    long cord = (lround(dargs[0] / 100.0 * RESOL_X)) + (lround(dargs[1] / 100.0 * RESOL_Y)) * RESOL_X;
    //fprintf(ostream, "%ld\n", cord);
    vram[cord] = (unsigned char) dargs[2];
})
DEF_CMD(pix,  15612230689976767012ULL, 3,
{
    //fprintf(ostream, "PIX\n");

    //fprintf(ostream, "DARGS: %lg\n", dargs[0]);
    //fprintf(ostream, "DARGS: %lg\n", dargs[1]);
    //fprintf(ostream, "DARGS: %lg\n", dargs[2]);
    long cord = lround(dargs[0]) + lround(dargs[1]) * RESOL_X;
    //fprintf(ostream, "%ld\n", cord);
    vram[cord] = (unsigned char) dargs[2];
})
DEF_CMD(dspl,  1521302425519142494ULL, 0,
{
    //fprintf(ostream, "DSPL\n");

    for(size_t iter = 0; iter < RESOL_X; iter++)
        fprintf(ostream, "-");

    for(size_t iter_y = 0; iter_y < RESOL_Y; iter_y++)
    {
        fprintf(ostream, "\n");
        for(size_t iter_x = 0; iter_x < RESOL_X; iter_x++)
        {
            if(vram[iter_y * RESOL_X + iter_x] == 0)
                fprintf(ostream, " ");
            else
                fprintf(ostream, "*");
        }
    }
    fprintf(ostream, "\n");

    for(size_t iter = 0; iter < RESOL_X; iter++)
        fprintf(ostream, "-");
})