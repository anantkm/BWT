/*
name: bwtdecode
Author: Anant Krishna Mahale
zID: 5277610

Note: 
Max Memory consumption for file >= 12MB is 14.46 MB on Heap. 
But there are static arrays which are not allocated on Heap. 

int c_table[90]                     =>   90 * 4 = 360 bytes
int char_size[255]                  =>   90 * 4 = 360 bytes
char output_buffer[1024]            => 1024 * 1 = 1024 bytes
load_char[bucket_size + 1]          =>  152 * 1 = 152 bytes  ( bucket_size for 15MB input is 151)
load_char[difference_position + 1]  =>  152 * 1 = 152 bytes  ( max difference_position for 15MB input is 151)
                                                ------------
                                                 2048 bytes ==> 2KB

So, total memory is still < 15MB aprt from other variables.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_pos = 0;
int bucket_size = 1;
int c_table[90] = {-1};
int *a_array;
int *c_array;
int *g_array;
int *t_array;
int g_fileLength = 0;
char *read_Buffer_cache;
int bytes_read_beg = 0;
int bytes_read_end = 0;
int offset_1 = 0;

//pre-processing funciton to read the file and create occurence array and c-table.
//if the file is less than 9MB it loads entire file else it load first 9MB.
void file_read_function(FILE *encoded_file)
{
    int char_size[90] = {0};
    fseek(encoded_file, 0, SEEK_END);
    g_fileLength = ftell(encoded_file) / sizeof(char);
    fseek(encoded_file, 0, SEEK_SET);
    int temp_bucket_index = 0;
    int current_bucket_index = 0;
    int n = -1;
    int chache_buffer_limit = 0;
    if (g_fileLength < (9000000 - 1))
    {
        //dynamic gap size.
        if ((g_fileLength >= 250000) && (g_fileLength <= 5000000))
        {
            bucket_size = (int)(g_fileLength / 250000) + 1;
        }
        else
        {

            bucket_size = (int)(g_fileLength / 100000) + 1;
        }
        int array_size = g_fileLength / bucket_size;
        a_array = calloc(array_size, sizeof(int));
        c_array = calloc(array_size, sizeof(int));
        g_array = calloc(array_size, sizeof(int));
        t_array = calloc(array_size, sizeof(int));
        read_Buffer_cache = malloc(sizeof(char) * g_fileLength + 1);
        while ((chache_buffer_limit <= 8) && ((n = fread(read_Buffer_cache + bytes_read_beg, sizeof(char), 1000000, encoded_file)) != 0))
        {
            for (int i = bytes_read_beg; i < (n + bytes_read_beg); i++)
            {
                char_size[(int)read_Buffer_cache[i]]++;
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
            bytes_read_beg += n;
            chache_buffer_limit++;
        }
    }
    else
    {
        bucket_size = (int)(g_fileLength / 100000) + 1;
        int array_size = g_fileLength / bucket_size;
        a_array = calloc(array_size, sizeof(int));
        c_array = calloc(array_size, sizeof(int));
        g_array = calloc(array_size, sizeof(int));
        t_array = calloc(array_size, sizeof(int));
        read_Buffer_cache = malloc(sizeof(char) * 9000000);
        while ((chache_buffer_limit <= 8) && ((n = fread(read_Buffer_cache + bytes_read_beg, sizeof(char), 1000000, encoded_file)) != 0))
        {
            for (int i = bytes_read_beg; i < (n + bytes_read_beg); i++)
            {
                char_size[(int)read_Buffer_cache[i]]++;
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
            bytes_read_beg += n;
            chache_buffer_limit++;
        }
    }
    if (g_fileLength > 9000000)
    {
        // reading remaining part of file > 9MB.
        //buffer_size is 512KB
        char *buffer = malloc(524288 * sizeof(char));
        fseek(encoded_file, 9000000, SEEK_SET);
        while ((n = fread(buffer, sizeof(char), 524288, encoded_file)) != 0)
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
        free(buffer);
    }

    c_table[65] = 1;
    c_table[67] = 1 + char_size[65];
    c_table[71] = c_table[67] + char_size[67];
    c_table[84] = c_table[71] + char_size[71];
    c_table[66] = c_table[67];
    c_table[68] = c_table[71];
    c_table[72] = c_table[84];
}

static inline int occuranceOfChar_2_buffer(FILE *input_file, char search_char, int position)
{
    int count = 0;
    int mod_value = (position + 1) % bucket_size;
    int right_postion = (position + 1 - mod_value);
    int right_bucket = (right_postion / bucket_size) - 1;
    //if the the chars occurs after 1gap then it takes count from gap and remaining chars.
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
    if (mod_value != 0)   // counting the remaining chars. 
    {
        int difference_position = position - right_postion;
        if ((right_postion >= offset_1) && ((right_postion + difference_position + 1) <= (offset_1 + bytes_read_beg - 1))) //checking if the required position is in loaded cache. 
        {
            for (int i = right_postion; i <= position; i++)
            {
                if (read_Buffer_cache[i] == search_char)
                {
                    count++;
                }
            }
        }
        else   
        {
            // else loads from the file.
            char load_char[bucket_size + 1];
            fseek(input_file, right_postion, SEEK_SET);
            fread(load_char, sizeof(char), bucket_size + 1, input_file);

            for (int k = 0; k <= difference_position; k++)
            {
                if (load_char[k] == search_char)
                {
                    count++;
                }
            }
        }
    }

    return count;
}

void decoder_function(FILE *input_file, FILE *output_file)
{
    //static output buffer. 
    char output_buffer[1024];
    int bufferSize = 1024;
    int current_position = g_fileLength;
    int count = g_fileLength - 1;
    output_buffer[bufferSize - 1] = '\n';
    current_position--;
    bufferSize--;
    int next_position = 0 + 0;
    while (count > 0)
    {
        if ((next_position >= offset_1) && (next_position <= (offset_1 + bytes_read_beg - 1)))
        {
            output_buffer[bufferSize - 1] = read_Buffer_cache[next_position - offset_1];
            next_position = c_table[(int)read_Buffer_cache[next_position - offset_1]] + occuranceOfChar_2_buffer(input_file, read_Buffer_cache[next_position - offset_1], next_position) - 1;
        }
        else
        {
            int mod_value = (next_position + 1) % bucket_size;
            int right_postion = (next_position + 1 - mod_value);
            int right_bucket = (right_postion / bucket_size) - 1;
            int difference_position = next_position - right_postion;
            char search_char;
            int char_count = 0;
            if (mod_value != 0)
            {
                char load_char[difference_position + 1];
                fseek(input_file, right_postion, SEEK_SET);
                fread(load_char, sizeof(char), difference_position + 1, input_file);
                search_char = load_char[difference_position];
                for (int i = 0; i <= difference_position; i++)
                {
                    if (load_char[i] == search_char)
                    {
                        char_count++;
                    }
                }
            }
            else
            {
                fseek(input_file, next_position, SEEK_SET);
                search_char = fgetc(input_file);
            }
            if (right_bucket >= 0)
            {

                switch (search_char)
                {
                case 'A':
                    char_count += a_array[right_bucket];
                    break;
                case 'C':
                    char_count += c_array[right_bucket];
                    break;
                case 'G':
                    char_count += g_array[right_bucket];
                    break;
                case 'T':
                    char_count += t_array[right_bucket];
                    break;
                }
            }
            output_buffer[bufferSize - 1] = search_char;
            next_position = c_table[(int)search_char] + char_count - 1;
        }
        count--;
        current_position--;
        bufferSize--;
        if (bufferSize == 0)
        {
            fseek(output_file, (current_position), SEEK_SET);
            fwrite(output_buffer, sizeof(char), 1024, output_file);
            bufferSize = 1024;
        }
    }
    if (bufferSize != 1024)
    {
        fseek(output_file, (current_position), SEEK_SET);
        fwrite(output_buffer + bufferSize, sizeof(char), 1024 - bufferSize, output_file);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: ./bwtdecode <BWT file path> <Reversed file path>\n");
        return 0;
    }
    FILE *encoded_file;
    if ((encoded_file = fopen(argv[1], "r")) == NULL)
    {
        printf("Enter a valid bwt encoded file\n");
        return 0;
    }
    FILE *decoded_file = fopen(argv[2], "w");
    file_read_function(encoded_file);
    decoder_function(encoded_file, decoded_file);
    
    //freing all the heaps. 
    fclose(encoded_file);
    fclose(decoded_file);
    free(a_array);
    free(c_array);
    free(g_array);
    free(t_array);
    free(read_Buffer_cache);
}
