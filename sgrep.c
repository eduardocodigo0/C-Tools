#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

typedef struct
{
    char *flag;
    char *input;
    short activated;
} Command;

Command *initialize_command_list()
{
    Command help;
    Command max_line_number;
    Command file;
    Command regex;
    Command show_line_number;

    help.activated = 0;
    help.flag = "-h";
    help.input = "";

    max_line_number.activated = 0;
    max_line_number.flag = "-n";
    max_line_number.input = "0";

    file.activated = 0;
    file.flag = "-f";
    file.input = "";

    regex.activated = 0;
    regex.flag = "-r";
    regex.input = "";

    show_line_number.activated = 0;
    show_line_number.flag = "-l";
    show_line_number.input = "";

    Command *command_list = calloc(sizeof(Command), 5);
    command_list[0] = help;
    command_list[1] = max_line_number;
    command_list[2] = file;
    command_list[3] = regex;
    command_list[4] = show_line_number;

    return command_list;
}

void display_commands_error()
{
    printf("Invalid input, use grep -h command to learn how to use sgrep.\n");
    exit(-1);
}

void display_file_error()
{
    printf("The file does not exist or is not readable.\n");
    exit(-1);
}

void display_help()
{
    printf("Use: ./sgrep -f [FILE NAME] -r [REGULAR EXPRESSION]\n");
    printf("Required arguments: \n");
    printf("-f              File to be used by sgrep in the search for matches.\n");
    printf("-r              Regular expression with the pattern to be found in the file.\n");
    printf("Optional arguments: \n");
    printf("-h              Display this help message.\n");
    printf("-l              Display the line number of a match.\n");
    printf("-n              Maximum number of matches to be found.\n");
    exit(0);
}

void configure_commands(Command *commands, int commands_array_size, char *arguments_array[], int arguments_array_size)
{
    for (int i = 1; i < arguments_array_size; ++i)
    {
        for (int j = 0; j < commands_array_size; ++j)
        {
            if (strcmp(commands[j].flag, arguments_array[i]) == 0)
            {
                commands[j].activated = 1;
                if ((strcmp(commands[j].flag, "-f") == 0) || (strcmp(commands[j].flag, "-n") == 0) || (strcmp(commands[j].flag, "-r") == 0))
                {
                    if ((i + 1) < arguments_array_size)
                    {
                        commands[j].input = arguments_array[i + 1];
                    }
                }
            }
        }
    }
}

int verify_match_in_line(char *line, char *pattern)
{
    regex_t regex;
    regcomp(&regex, pattern, 0);
    int result = regexec(&regex, line, 0, NULL, REG_NOTBOL);
    regfree(&regex);
    
    return result;
}

int main(int argc, char *argv[])
{
    Command *commands = initialize_command_list();

    if ((argc < 2) || (argc > 9))
        display_commands_error();

    configure_commands(commands, 5, argv, argc);

    if (commands[0].activated)
        display_help();
    if (!commands[2].activated || !commands[3].activated || commands[3].input == "")
        display_commands_error();


    FILE *input_file = fopen(commands[2].input, "r");

    if (!input_file)
        display_file_error();

    short max_line_length = 2048;
    char line[max_line_length];

    long line_number = 1;
    long match_qtd = 0;
    long max_matches = atoi(commands[1].input);
    char *pattern = commands[3].input;
    pattern[strcspn(pattern, "\n")] = 0;

    while (fgets(line, max_line_length, input_file))
    {
        if (!verify_match_in_line(line, pattern))
        {
            ++match_qtd;
            if (commands[4].activated)
            {
                printf("%d - %s\n", line_number, line);
            }
            else
            {
                printf("%s\n", line);
            }

            if (commands[1].activated && max_matches != 0)
            {
                if (max_matches <= match_qtd)
                    break;
            }
        }

        ++line_number;
    }

    fclose(input_file);
    free(commands);

    return 0;
}