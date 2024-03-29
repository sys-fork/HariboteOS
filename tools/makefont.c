/* makefont.c
 * the source code of makefont command
 * command's utilities:
 *   arguments:
 *      -n <FontDataName> - specify the name of A vector of data of font
 *      -o <OutFileName>  - specify the name of outputfile
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_OUT_FILE    "font.c"
#define DEFAULT_FONT_DATA   "_hankaku"

static int32_t hankaku2asm(unsigned char *, unsigned char *, unsigned char *);
static unsigned char *translate(unsigned char *, unsigned char *);

/* noreturn */
inline static void print_error_msg_hankaku2asm()
{
    fprintf(stderr, "hankaku2asm: [error] An error occured in hankaku2asm()\n ");
    exit(EXIT_FAILURE);
}

struct ARGUMENTS {
    int32_t SourceNumber;       /* if it is not 1, program will occur an error */
    unsigned char *SourceFileName;
    unsigned char *OutFileName;
    unsigned char *FontDataName;
};

/* main function
 * arguments:
 *      argc - the number of arguments.
 *             No.0 is used to store this command name,
 *             so not used.
 *      argv - A argument type of (unsigned char **),
 *             the pointer to a pointer table of arguments's strings.
 */
int main(int argc, char **argv)
{
    int32_t count;
    struct  ARGUMENTS arg = { 0, (unsigned char *) NULL, (unsigned char *) NULL, (unsigned char *) NULL };

    if (argc > 6) {
        fprintf(stderr, "hankaku2asm: [error] The arguments must be below 6\n");
        exit(EXIT_FAILURE);
    }

    /* set the number of some options's arguments which is a member of ARGUMENTS */
    for (count = 1; count <= 5; count++) {  /* the maximum of number of arguments if 6 */
        if (*argv[count] == '-') {
            if (*(argv[count] + 1) == 'o') {
                arg.OutFileName  = (unsigned char *) argv[++count];
            } else if (*(argv[count] + 1) == 'n') {
                arg.FontDataName = (unsigned char *) argv[++count];
            } else {
                fprintf(stderr, "hankaku2asm: [error] Unknown option \"-%c\" \n", *(argv[count] + 1));
                exit(EXIT_FAILURE);
            }
        } else {
            arg.SourceFileName = (unsigned char *) argv[count];
            arg.SourceNumber++;
        }
    }
    
    /* call hankaku2asm() */
    if (arg.SourceNumber < 1) {
        fprintf(stderr, "hankaku2asm: [error] No input file\n");
        exit(EXIT_FAILURE);
    } else if (arg.SourceNumber > 1) {
        fprintf(stderr, "hankaku2asm: [error] Input file is must be one\n");
        exit(EXIT_FAILURE);
    } else {        /* arg.SourceNumber == 1 */
        if (arg.OutFileName == (unsigned char *) NULL) {
            if (arg.FontDataName == (unsigned char *) NULL) {
                /* the default */
                if (hankaku2asm(arg.SourceFileName, (unsigned char *) DEFAULT_OUT_FILE, (unsigned char *) DEFAULT_FONT_DATA)) {
                    print_error_msg_hankaku2asm();
                }
            } else {
                if (hankaku2asm(arg.SourceFileName, (unsigned char *) DEFAULT_OUT_FILE, arg.FontDataName)) {
                    print_error_msg_hankaku2asm();
                }
            }
        } else {
            if (arg.FontDataName == (unsigned char *) NULL) {
                if (hankaku2asm(arg.SourceFileName, arg.OutFileName, (unsigned char *) DEFAULT_FONT_DATA)) {
                    print_error_msg_hankaku2asm();
                }
            } else {
                if (hankaku2asm(arg.SourceFileName, arg.OutFileName, arg.FontDataName)) {
                    print_error_msg_hankaku2asm();
                }
            }
        }
    }

    return 0;
}

/* The function read and write files and call translate()
 * if no error occur this function return 0.
 */
static int32_t hankaku2asm(unsigned char *in, unsigned char *out, unsigned char *FontDataName)
{
    /* combine file */
    FILE    *infile, *outfile;
    unsigned char readed_by_infile[1024];
    int32_t counter;

    unsigned char *source_code_no_comments, *asm_code;

    if (!(source_code_no_comments = (unsigned char *) malloc(100000)) || !(asm_code = (unsigned char *) malloc(100000))) {
        fprintf(stderr, "hankaku2asm: [error] Mamory arocating error\n");
        exit(EXIT_FAILURE);
    }

    if (!(infile = fopen((char *) in, "r")) || !(outfile = fopen((char *) out, "w"))) {
        fprintf(stderr, "hankaku2asm: [error] File open error\n");
        exit(EXIT_FAILURE);
    } else {
        for (;;) {
            if (!fgets((char *) readed_by_infile, sizeof(readed_by_infile) - 1, infile)) {
                if (feof(infile)) {
                    /* if suceed in read and removing comments */
                    asm_code = translate(source_code_no_comments, FontDataName);
                    if (!fwrite(asm_code, 1, strlen((char *) asm_code), outfile)) {
                        fprintf(stderr, "hankaku2asm: [error] An error occured in fwrite()\n");
                        exit(EXIT_FAILURE);
                    } else {
                        goto exit;
                    }
                } else {
                    fprintf(stderr, "hankaku2asm: [error] An error occured in fgets()\n");
                    exit(EXIT_FAILURE);
                }
            } else {
                /* eliminating comments */
                counter = 0;
                while (counter < 1024 && readed_by_infile[counter] != '\n' && readed_by_infile[counter] != '#' && readed_by_infile[counter] != '\0') counter++;
                strncat((char *) source_code_no_comments, (char *) readed_by_infile, counter);
            }
        }
    }

exit:
    fclose(infile);
    fclose(outfile);
    free(source_code_no_comments);

    return 0;
}

/* The function that translate font data to assembly
 * return A pointer to assembly code.
 */
static unsigned char *translate(unsigned char *source, unsigned char *FontDataName)
{
    unsigned char buf[16];
    unsigned char *hex, *asm_code, *source_eliminated;
    unsigned char horizontal_font_data8;
    int32_t i, j, q;

    if (!(hex = (unsigned char *) malloc(100000)) || !(asm_code = (unsigned char *) malloc(100000)) || !(source_eliminated = (unsigned char *) malloc(100000))) {
        fprintf(stderr, "hankaku2asm: [error] Memory arocating error\n");
        exit(EXIT_FAILURE);
    }
    
    /* eliminating \n(<CR>) and space and \0(NULL)*/
    for (i = 0, j = 0; i < 100000; i++) {
        if (source[i] != '\n' && source[i] != '\r' && source[i] != ' ' && source[i] != '\0') {
            source_eliminated[j] = source[i];
            j++;
        }
    }

    source_eliminated[j] = '\0';

    /* translating */
    for (i = 0; i < 256; i++) {
        strcat((char *) hex, "\n    .byte ");
        for (j = 0; j < 16; j++) {
            horizontal_font_data8 = 0;

            for (q = 0; q < 8; q++) {
                switch (source_eliminated[i * 8 * 16 + j * 8 + q]) {
                    case '*':
                        horizontal_font_data8 |= (0x01 << (7 - q));
                        break;
                    case '.':
                        break;
                    default:
                        fprintf(stderr, "hankaku2asm: [error] %i:%i An invalid word \"%c\"\n", i * 16 + j + 1, q, source_eliminated[i * 16 + j * 8 + q]);
                        exit(EXIT_FAILURE);
                }
            }

            if (j < 15) sprintf((char *) buf, "0x%02x, ", horizontal_font_data8);
            else sprintf((char *) buf, "0x%02x", horizontal_font_data8);
            strncat((char *) hex, (char *) buf, 6);
        }
    }

    /* combine head[] and foot[] and hex[] */
    sprintf((char *) asm_code, "%s%s%c%s%c%s%c", ".data\n.globl ", FontDataName, '\n', FontDataName, ':', hex, '\n');

    return asm_code;
}

