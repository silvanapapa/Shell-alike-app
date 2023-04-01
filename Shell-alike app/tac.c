#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SEPARATOR "\n" // Default separator is newline

void print_usage(char* program_name) {
    printf("Usage: %s [-b] [-s separator] [input_file]\n", program_name);
    printf("Reverses the order of lines in input_file or standard input.\n");
    printf("If input_file is not specified, input is read from standard input.\n");
    printf("Options:\n");
    printf("  -b         Ignore trailing newline at end of file or input.\n");
    printf("  -s sep     Use sep as the separator between lines. (default: \\n)\n");
}

int main(int argc, char* argv[]) {
    int option_b = 0;
    char* separator = DEFAULT_SEPARATOR;
    char* input_file = NULL;
    FILE* file;
    char* line = NULL;
    size_t line_size = 0;
    ssize_t read;

    // Parse command line arguments
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            option_b = 1;
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            separator = argv[i + 1];
            i++;
        } else if (argv[i][0] == '-') {
            printf("Invalid option: %s\n", argv[i]);
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        } else {
            input_file = argv[i];
        }
    }

    // Open input file or use stdin
    if (input_file != NULL) {
        file = fopen(input_file, "r");
        if (file == NULL) {
            printf("Error opening file: %s\n", input_file);
            exit(EXIT_FAILURE);
        }
    } else {
        file = stdin;
    }

    // Read lines from file or stdin into array
    char** lines = malloc(sizeof(char*) * 1024);
    int num_lines = 0;
    while ((read = getline(&line, &line_size, file)) != -1) {
       /* // Remove trailing newline if specified
        if (option_b && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }*/
        // Add line to array
        lines[num_lines] = malloc(sizeof(char) * (read + 1));
        strncpy(lines[num_lines], line, read);
        //lines[num_lines][read] = '\0';
        num_lines++;
        line = NULL;
        line_size = 0;
    }
    if (file != stdin) {
        fclose(file);
    }

    // Print lines in reverse order
    int j;
    if(option_b == 1)
       {
        if(file == stdin)
        {
            printf("\n\n");

        }
       else if(input_file != NULL)
       {
           //print cate linii avem-1
           // for(int k = 1; k <= num_lines - 2; k++)
             //   printf("\n");
       }
       for(size_t i = num_lines; i-- > 2; )
        {
            printf("%s", lines[i]);
            free(lines[i]);
        }
        printf("%.*s", (int)strlen(lines[1])-1, lines[1]);
        printf("%.*s", (int)strlen(lines[0])-1, lines[0]);
        printf("\n");
        free(lines[1]);
        free(lines[0]);
       }
    else for(j = num_lines - 1; j >= 0; j--) 
    {
        printf("%s", lines[j]);
        /*if (j > 0) {
            printf("%s", separator);
        }*/
        free(lines[j]);
    }
    free(lines);

    return 0;
}
//REZOLVA CU AFISARILE