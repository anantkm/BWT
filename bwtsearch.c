/*

name:bwtsearch.c
Author: Anant Krishna Mahale.
zID: 5277610


Note: 
Max Memory consumption for input file size 100MB is 14.21 MB on Heap. 
But there are static arrays which are not allocated on Heap. 

int c_table[90]                     =>   90 * 4 =  360 bytes
int char_size[90]                   =>   90 * 4 =  360 bytes
char load_char[bucket_size + 1]     =>  170 * 1 =  170 bytes  ( bucket_size for 100MB input is 170)
char input_terminal[101]            =>  101 * 1 =  101 bytes  ( max input size)
                                                ------------
                                                  991 bytes (less than 1KB)

So, total memory is still < 15MB aprt from other variables.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bucket_size = 1;
int c_table[90] = {-1};
int *a_array;
int *c_array;
int *g_array;
int *t_array;
int g_fileLength = 0;

//pre-processing funciton to read the file and create occurence array and c-table.
void file_read_function(FILE *encoded_file)
{

    int char_size[90] = {0};
    fseek(encoded_file, 0, SEEK_END);
    g_fileLength = ftell(encoded_file) / sizeof(char);

    //dynamic gap size. if the filesize is < 620000 bytes the gap size will be zero. if it is more than 620000, interval will be created using below formula.
    //bucketsize refers to gap size.
    if (g_fileLength >= 620000)
    {
        bucket_size = (int)(g_fileLength / 620000) + 1;
    }
    int array_size = g_fileLength / bucket_size;
    a_array = calloc(array_size, sizeof(int));
    c_array = calloc(array_size, sizeof(int));
    g_array = calloc(array_size, sizeof(int));
    t_array = calloc(array_size, sizeof(int));
    fseek(encoded_file, 0, SEEK_SET);
    int temp_bucket_index = 0;
    int current_bucket_index = 0;

    // buffer to read a file.
    char *buffer;
    buffer = malloc(1000000 * sizeof(char));
    int n = -1;
    while ((n = fread(buffer, sizeof(char), 1000000, encoded_file)) != 0)
    {

        for (int i = 0; i < n; i++)
        {
            char_size[(int)buffer[i]]++;

            if (((temp_bucket_index + 1) % bucket_size) == 0)
            {
                a_array[current_bucket_index] = char_size[65];
                c_array[current_bucket_index] = char_size[67];
                g_array[current_bucket_index] = char_size[71];
                t_array[current_bucket_index] = char_size[84];
                current_bucket_index++;
            }
            temp_bucket_index++;
        }
    }
    //creation of ctable.
    c_table[65] = 1;
    c_table[67] = 1 + char_size[65];
    c_table[71] = c_table[67] + char_size[67];
    c_table[84] = c_table[71] + char_size[71];
    c_table[66] = c_table[67];
    c_table[68] = c_table[71];
    c_table[72] = c_table[84];
    free(buffer);
}

static inline int occuranceOfChar(FILE *input_file, char search_char, int position)
{
    // if the gap size is 1, there will be count at each position hence value is read directly else reads the remaining characters afer gap and counts
    int count = 0;
    if (bucket_size == 1)
    {
        switch (search_char)
        {
        case 'A':
            count = a_array[position];
            break;
        case 'C':
            count = c_array[position];
            break;
        case 'G':
            count = g_array[position];
            break;
        case 'T':
            count = t_array[position];
            break;
        }
    }
    else
    {
        int mod_value = (position + 1) % bucket_size;
        int right_postion = (position + 1 - mod_value);
        int right_bucket = (right_postion / bucket_size) - 1;
        if (right_bucket >= 0)
        {

            switch (search_char)
            {
            case 'A':
                count = a_array[right_bucket];
                break;
            case 'C':
                count = c_array[right_bucket];
                break;
            case 'G':
                count = g_array[right_bucket];
                break;
            case 'T':
                count = t_array[right_bucket];
                break;
            }
        }
        if (mod_value != 0)
        {
            char load_char[bucket_size + 1];
            fseek(input_file, right_postion, SEEK_SET);
            fread(load_char, sizeof(char), bucket_size + 1, input_file);
            int difference_position = position - right_postion;
            for (int i = 0; i <= difference_position; i++)
            {
                if (load_char[i] == search_char)
                {
                    count++;
                }
            }
        }
    }

    return count;
}

int bwtSearch(FILE *input_file, char *toBeSearched, int input_size)
{

    // impletementation of bwtalgorithm from the lecture.
    int i = input_size - 1;
    char c = toBeSearched[i];
    int first = c_table[(int)c];
    int last = c_table[(int)(c + 1)] - 1;

    if (last < 0)
    {
        last = g_fileLength - 1;
    }

    while ((first <= last) && (i >= 1))
    {
        c = toBeSearched[i - 1];
        first = c_table[(int)c] + occuranceOfChar(input_file, c, first - 1);
        last = c_table[(int)c] + occuranceOfChar(input_file, c, last) - 1;
        i = i - 1;
    }
    if (last < first)
    {
        return 0;
    }
    else
    {
        return (last - first + 1);
    }
}

int main(int argc, char *argv[])

{
    if (argc < 2)
    {
        printf("Usage: ./bwtsearch <BWT file path>\n");
        return 0;
    }
    FILE *encoded_file;
    if ((encoded_file = fopen(argv[1], "r")) == NULL)
    {
        printf("Enter a valid bwt encoded file\n");
        return 0;
    }
    file_read_function(encoded_file);
    char input_terminal[101];
    while (fgets(input_terminal, sizeof(input_terminal), stdin) != NULL)
    {
        input_terminal[strcspn(input_terminal, "\n")] = 0;
        int suze = strlen(input_terminal);
        int result = bwtSearch(encoded_file, input_terminal, suze);
        printf("%d\n", result);
    }
    fclose(encoded_file);
    free(a_array);
    free(c_array);
    free(g_array);
    free(t_array);
}
