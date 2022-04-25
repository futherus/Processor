/* Commands of processor */

/** Registers for arguments
 *  NOT WRITABLE
 */
////DARGS 

/** Temporary registers
 *  Meant to be used during command execution
 */
////DTMPS

/** Pointer to register with system argument
 *  FOR SYSCMD ONLY
 */
////REG_PTR

/** Register with relative stack size
 *  NOT WRITABLE
 */
////REG_SZ   

/** Register for integer calculation
 *  Meant to be used during command execution
 */
////REG_INT  

/** Instruction pointer of processor
 *  FOR SYSCMD ONLY
 *  Used for jumps and function calls
 */
////IP       

/** \brief Copies src to dst
 *  \param DST [out] Pointer to destination location
 *  \param SRC [in]  Pointer to source location
 */
////COPY(DST, SRC)

/// \brief Prints to output
////PRINT(format, ...) 

/// \brief Scans from input
////SCAN(format, ...)  

/// \brief Behaves like print, but only if #define'd CPU_VERBOSE
////VERBOSE(format, ...)

/// \brief Array of RAM, double
////RAM  

/// \brief Array of VRAM, unsigned char
////VRAM

/** \brief Pushes argument to stack
 *  \param ARG [in]  Arg value
 */
////PUSH(ARG) 

/** \brief Pops to argument from stack
 *  \param ARG [out] Pointer to arg
 */
////POP(ARG) 

DEF_CMD(eq,    590643385890189695ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[0], DTMPS[1]) == 0)
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(neq, 15614157034349014371ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[0], DTMPS[1]) != 0)
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(gr,    590641186866933210ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[1], DTMPS[0]) > 0)
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(le,    590635689308792140ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[1], DTMPS[0]) < 0)
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(leq, 15616140553325928533ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[1], DTMPS[0]) <= 0)
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(geq, 15620925627930956582ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[1], DTMPS[0]) >= 0)
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(and,  15626582615256966840ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[1], 0) && CMP(DTMPS[0], 0))
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(or,     590632390773907554ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);
    if(CMP(DTMPS[1], 0) || CMP(DTMPS[0], 0))
        PUSH(1);
    else
        PUSH(0);
})

DEF_CMD(sin,  15609380755837011737ULL, 1,
{
    DTMPS[0] = sin(DARGS[0]);
    PUSH(DTMPS[0]);
})

DEF_CMD(cos,  15624613389931219346ULL, 1,
{
    DTMPS[0] = cos(DARGS[0]);
    PUSH(DTMPS[0]);
})

DEF_CMD(sqrt, 10605811640109351699ULL, 1,
{
    DTMPS[0] = sqrt(DARGS[0]);
    PUSH(DTMPS[0]);
})

DEF_CMD(in,     590638987843676716ULL, 0,
{
    SCAN("%lg", &DTMPS[0]);
    PUSH(DTMPS[0]);
})

DEF_CMD(out,  15613325803558276075ULL, 0,
{
    POP(&DTMPS[0]);
    PRINT("-> %lg\n", DTMPS[0]);
})

DEF_CMD(pixr, 12453253958294016862ULL, 3,
{
    REG_INT = (lround(DARGS[0] / 100.0 * RESOL_X)) + (lround(DARGS[1] / 100.0 * RESOL_Y)) * RESOL_X;
    VRAM[REG_INT] = (unsigned char) DARGS[2];

    VERBOSE("PIXR (%lg, %lg, %lg)\n",
            DARGS[0], DARGS[1], DARGS[2]);
    VERBOSE("  Coordinate: %lld\n", REG_INT);
})

DEF_CMD(pix,  15612230689976767012ULL, 3,
{
    REG_INT = lround(DARGS[0]) + lround(DARGS[1]) * RESOL_X;
    VRAM[REG_INT] = (unsigned char) DARGS[2];

    VERBOSE("PIX (%lg, %lg, %lg)\n",
            DARGS[0], DARGS[1], DARGS[2]);
    VERBOSE("  Coordinate: %lld\n", REG_INT);
})

DEF_CMD(dspl,  1521302425519142494ULL, 0,
{
    PRINT("\n");

    for(long iter = 0; iter < RESOL_X; iter++)
        PRINT("-");

    for(long iter_y = 0; iter_y < RESOL_Y; iter_y++)
    {
        PRINT("\n");
        for(long iter_x = 0; iter_x < RESOL_X; iter_x++)
        {
            if(VRAM[iter_y * RESOL_X + iter_x] == 0)
                PRINT(" ");
            else
                PRINT("*");
        }
    }
    PRINT("\n");

    for(long iter = 0; iter < RESOL_X; iter++)
        PRINT("-");

    PRINT("\n");
})

DEF_CMD(show, 10597054029992354278ULL, 2,
{
    PRINT("\n");

    if(DARGS[1] + DARGS[0] > RAM_CAP)
        return PROCESSING_RAM_OVERFLOW;
    
    long resol = (long) sqrt(DARGS[1]);
    PRINT("Resolution: %ld\n", resol);
    for(long iter = 0; iter < resol; iter++)
        PRINT("--");

    for(long iter_y = 0; iter_y < resol; iter_y++)
    {
        PRINT("\n");
        for(long iter_x = 0; iter_x < resol; iter_x++)
        {
            if(CMP(RAM[(long) DARGS[0] + iter_y * resol + iter_x], 0) == 0)
                PRINT("  ");
            else
                PRINT("$$");
        }
    }
    PRINT("\n");

    for(long iter = 0; iter < resol; iter++)
        PRINT("--");

    PRINT("\n");
})

DEF_CMD(int, 15618947606512183472ULL, 1,
{
    PUSH((double) lround(DARGS[0]));
})

DEF_CMD(pow, 15612228490953510529ULL, 0,
{
    POP(&DTMPS[0]);
    POP(&DTMPS[1]);

    PUSH(pow(DTMPS[1], DTMPS[0]));
})