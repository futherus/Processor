#include "args.h"
#include "assert.h"
#include <stdlib.h>
#include <string.h>

args_msg process_args(int argc, char* argv[], char infile_name[], char outfile_name[])
{
    assert(argv && infile_name && outfile_name);

    if(argc < 2)
        return ARGS_NO_OPT;

    int src = 0;
    int dst = 0;

    for(int iter = 1; iter < argc; iter++)
    {
        char command[MAX_COMMAND_SIZE] = "";
        if(memccpy(command, argv[iter], '\0', MAX_COMMAND_SIZE) == nullptr)
        {
            return ARGS_CMD_OVRFLW; //LONG_COMMAND
        }

        if(strcmp(command, "-h") == 0 || strcmp(command, "--help") == 0)
        {
            if(argc > 2)
                return ARGS_HLP_NOTE;
            else
                return ARGS_HLP;
        }
        else if(strcmp(command, "--src") == 0)
        {
            if(src == 1)
                return ARGS_OPT_OVRWRT;

            iter++;
            if(iter >= argc)
                return ARGS_NO_IFL_NAME;

            if(memccpy(infile_name, argv[iter], '\0', MAX_FILENAME_SIZE) == nullptr)
            {
                return ARGS_FLNAME_OVRFLW; //LONG_FILENAME
            }

            src = 1;
        }
        else if(strcmp(command, "--dst") == 0)
        {
            if(dst == 1)
                return ARGS_OPT_OVRWRT;

            iter++;
            if(iter >= argc)
                return ARGS_NO_OFL_NAME;

            if(memccpy(outfile_name, argv[iter], '\0', MAX_FILENAME_SIZE) == nullptr)
            {
                return ARGS_FLNAME_OVRFLW; //LONG_FILENAME
            }

            dst = 1;
        }
        else
        {
            return ARGS_BAD_CMD; //BAD_CMD
        }
    }

    if(src == 0)
        return ARGS_NO_IFL_NAME;
    if(dst == 0)
        return ARGS_NO_OFL_NAME;

    return ARGS_NOMSG;
}

void response_args(args_msg param, FILE* const ostream)
{
    assert(ostream);

    switch(param)
    {
        case ARGS_HLP:
        {
            fprintf(ostream, HELP);
            break;
        }
        case ARGS_HLP_NOTE:
        {
            fprintf(ostream, "%s\n%s", HELP, NOTE);
            break;
        }
        case ARGS_NO_OPT:
        {
            fprintf(ostream, NO_OPTIONS);
            break;
        }
        case ARGS_OPT_OVRWRT:
        {
            fprintf(ostream, OPTION_OVERWRITE);
            break;
        }
        case ARGS_NO_IFL_NAME:
        {
            fprintf(ostream, NO_IFILE_NAME);
            break;
        }
        case ARGS_NO_OFL_NAME:
        {
            fprintf(ostream, NO_OFILE_NAME);
            break;
        }
        case ARGS_CMD_OVRFLW:
        {
            fprintf(ostream, COMMAND_OVERFLOW);
            break;
        }
        case ARGS_FLNAME_OVRFLW:
        {
            fprintf(ostream, FILENAME_OVERFLOW);
            break;
        }
        case ARGS_NOMSG:
        {
            break;
        }
        case ARGS_BAD_CMD:
        {
            fprintf(ostream, BAD_COMMAND);
            break;
        }
        case ARGS_UNEXPCTD_ERR:
        {
            fprintf(ostream, UNEXPECTED_ERROR);
            break;
        }
        default:
        {
            assert(0);
        }
    }
}