/*
 * Name: Yuchen Liu
 * loginID: yl6448
*/
// #include "lab3.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

int cache(int, int, int, char *, int *, int *, int *, int);
void parseToCache(long, int, int, int, long *, long *, long *);
long readAddress(char *);
int loadAndStore(long **cache, long set, long tag, int E);

void printHelp();

int main(int argc, char **argv)
{
    int c, s, E, b, vflag, hit_count = 0, miss_count = 0, eviction_count = 0;
    extern char *optarg;
    char *trace;

    while ((c = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (c)
        {
        case 'h':
            printHelp();
            return 0;
            break;
        case 'v':
            vflag = 1;
            break;
        case 's':
            sscanf(optarg, "%d", &s);
            break;
        case 'E':
            sscanf(optarg, "%d", &E);
            break;
        case 'b':
            sscanf(optarg, "%d", &b);
            break;
        case 't':
            trace = (char *)malloc(sizeof(char) * strlen(optarg));
            strncpy(trace, optarg, strlen(optarg));
            break;
        }
    }

    cache(s, E, b, trace, &hit_count, &miss_count, &eviction_count, vflag);

    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

int cache(int s, int E, int b, char *trace_name, int *hit_count, int *miss_count, int *eviction_count, int vflag)
{

    char instruction[100];
    FILE *file;
    long address;
    long tag, set, block;
    int t = 64 - (s + b);
    int S = pow(2, s);
    long **cache = (long **)malloc(S * sizeof(long *));
    for (int i = 0; i < S; i++)
    {
        cache[i] = (long *)malloc(E * sizeof(long));
    }
    for (int i = 0; i < S; i++)
    {
        for (int j = 0; j < E; j++)
        {
            cache[i][j] = -1;
        }
    }

    file = fopen(trace_name, "r");
    if (file == NULL)
    {
        printf("file NULL");
        return -1;
    }

    while (fgets(instruction, 100, file) != NULL)
    {

        //SKIP LOAD INSTRUCTION
        if (instruction[0] == 'I')
        {
            continue;
        }
        else
        {
            char current = instruction[1];
            int line_result;

            address = readAddress(instruction);
            parseToCache(address, t, s, b, &tag, &set, &block);

            int counter = 0;
            while (instruction[counter] != '\0')
            {
                if (instruction[counter] == '\n')
                {
                    instruction[counter] = '\0';
                }
                counter++;
            }

            switch (current)
            {
            //LOAD DATA =======================================
            case 'L':
                if (vflag)
                    printf("%s", instruction);
                line_result = loadAndStore(cache, set, tag, E);
                if (line_result == 2)
                {
                    if (vflag)
                        printf(" hit\n");
                    (*hit_count)++;
                }
                else if (line_result == 1)
                {
                    if (vflag)
                        printf(" miss eviction\n");
                    (*miss_count)++;
                    (*eviction_count)++;
                }
                else if (line_result == 0)
                {
                    if (vflag)
                        printf(" miss\n");
                    (*miss_count)++;
                }
                break;
            //MODIFY DATA =====================================
            case 'M':
                if (vflag)
                    printf("%s", instruction);
                line_result = loadAndStore(cache, set, tag, E);
                if (line_result == 2)
                {
                    if (vflag)
                        printf(" hit ");
                    (*hit_count)++;
                }
                else if (line_result == 1)
                {
                    if (vflag)
                        printf(" miss eviction ");
                    (*miss_count)++;
                    (*eviction_count)++;
                }
                else if (line_result == 0)
                {
                    if (vflag)
                        printf(" miss ");
                    (*miss_count)++;
                }
                int write = loadAndStore(cache, set, tag, E);
                if (write == 2)
                {
                    if (vflag)
                        printf(" hit\n");
                    (*hit_count)++;
                }
                else if (write == 1)
                {
                    if (vflag)
                        printf(" miss eviction\n");
                    (*miss_count)++;
                    (*eviction_count)++;
                }
                else if (write == 0)
                {
                    if (vflag)
                        printf(" miss\n");
                    (*miss_count)++;
                }

                break;
            //STORE DATA ======================================
            case 'S':
                if (vflag)
                    printf("%s", instruction);

                line_result = loadAndStore(cache, set, tag, E);
                if (line_result == 2)
                {
                    if (vflag)
                        printf(" hit\n");
                    (*hit_count)++;
                }
                else if (line_result == 1)
                {
                    if (vflag)
                        printf(" miss eviction\n");
                    (*miss_count)++;
                    (*eviction_count)++;
                }
                else if (line_result == 0)
                {
                    if (vflag)
                        printf(" miss\n");
                    (*miss_count)++;
                }

                break;
            }
        }
    }

    fclose(file);
    return 1;
}

void parseToCache(long hex, int t, int s, int b, long *tag, long *set, long *block)
{
    b = pow(0x2, b);
    if (b > hex)
    {
        *tag = 0x0;
        *set = 0x0;
        *block = hex;
    }
    else
    {
        s = pow(0x2, s);
        *block = hex % b;
        hex /= b;

        if (s > hex)
        {
            *tag = 0x0;
            *set = hex;
        }
        else
        {

            *set = hex % s;
            hex /= s;
            *tag = hex;
        }
    }
}

long readAddress(char *instruction)
{
    int iter = 3;
    char address_string[20];
    long address;
    while (instruction[iter] != ',')
    {
        address_string[iter - 3] = instruction[iter];
        iter++;
    }
    address_string[iter - 3] = '\0';
    sscanf(address_string, "%lx", &address);

    return address;
}

int loadAndStore(long **cache, long set, long tag, int E)
{

    //match
    for (int i = 0; i < E; i++)
    {
        if (cache[set][i] == tag)
        {

            for (int j = 0; j < i + 1; j++)
            {
                int temp = cache[set][j];
                cache[set][j] = cache[set][0];
                cache[set][0] = temp;
            }
            cache[set][0] = tag;
            return 2;
        }
    }

    //empty
    for (int i = 0; i < E; i++)
    {
        if (cache[set][i] == -1)
        {

            for (int j = 0; j < i + 1; j++)
            {
                int temp = cache[set][j];
                cache[set][j] = cache[set][0];
                cache[set][0] = temp;
            }

            cache[set][0] = tag;
            return 0;
        }
    }

    //evict

    for (int j = 0; j < E; j++)
    {
        int temp = cache[set][j];
        cache[set][j] = cache[set][0];
        cache[set][0] = temp;
    }

    cache[set][0] = tag;
    return 1;
}

void printHelp()
{
    printf("\
        Options: \n\
        -h         Print this help message.\n\
        -v         Optional verbose flag.\n\
        -s <num>   Number of set index bits.\n\
        -E <num>   Number of lines per set.\n\
        -b <num>   Number of block offset bits.\n\
        -t <file>  Trace file.\n\n\
        Examples:\n\
        linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n\
        linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n"
    );
}
