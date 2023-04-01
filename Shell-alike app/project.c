#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

int ls_command(int argc, char *argv[])
{
    //parse the options
    int option_l = 0, option_s = 0, option_a = 0, option_F = 0;
    char *dir_name = (char *)malloc(sizeof(char) * (100));
    //use the current directory if no path was provided
    strcpy(dir_name, ".");
    //check for the flag or for the path
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            for(int j = 1; argv[i][j]; j++)
            {
                if(argv[i][j] == 'a')
                    option_a = 1;
                if(argv[i][j] == 'l')
                    option_l = 1;
                if(argv[i][j] == 's')
                    option_s = 1;
                if(argv[i][j] == 'F')
                    option_F = 1;
            }
            if(option_a == 0 && option_F == 0 && option_l == 0 && option_s == 0)
            {
                printf("Error: invalid command.\n");
                return 0;
            }
        }
        else strcpy(dir_name, argv[i]);
    }
    //open the directory
    DIR *dir = opendir(dir_name);
    if(dir == NULL)
    {
        perror("Error");
        return -1;
    }
    //read the directory entries
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL)
    {
        struct stat sb;
        //skip hidden files if the -a option is not set
        if(entry->d_name[0] == '.' && !option_a)
        {
            continue;
        }
        //print the file name
        printf("%s", entry->d_name);
        //print the file type if the -F option is set
        if(option_F == 1)
        {
            switch(entry->d_type)
            {
            case DT_BLK: //block device
                printf("/");
                break;
            case DT_CHR: //char device
                printf("@");
                break;
            case DT_DIR: //directory
                printf("/");
                break;
            case DT_FIFO://named pipe/fifo file
                printf("|");
                break;
            case DT_LNK://symbolic link
                printf("@");
                break;
            case DT_REG: //regular/executable file
                if(access(entry->d_name, X_OK) == 0) //check if file is executable
                {
                    printf("*");
                }
                break;
            case DT_SOCK://local-domain socket
                printf("=");
                break;
            default:
                printf("?");
                break;
            }
        }
        //get the file stats if the -l or -s option is set
        if(option_l == 1 || option_s == 1)
        {
            char path[1024];
            snprintf(path, 1024, "%s/%s", dir_name, entry->d_name);
            if(stat(path, &sb) == -1)
            {
                perror("Error");
                return -1;
            }
            //print the file size if the -s option is set
            if(option_s == 1)
            {
                printf(" %lld", (long long) sb.st_blocks/2);
            }
            //print the file details if the -l option is set
            if(option_l == 1)
            {
                //get the file permissions
                char permissions[11] = "----------";
                if(sb.st_mode & S_IRUSR) //sees if it has reading permissions, owner
                {
                    permissions[1] = 'r';
                }
                if(sb.st_mode & S_IWUSR) //writing permissions, owner
                {
                    permissions[2] = 'w';
                }
                if(sb.st_mode & S_IXUSR) //execute/search permissions, owner
                {
                    permissions[3] = 'x';
                }
                if(sb.st_mode & S_IRGRP) //read permissions, group
                {
                    permissions[4] = 'r';
                }
                if(sb.st_mode & S_IWGRP) //write permissions, group
                {
                    permissions[5] = 'w';
                }
                if(sb.st_mode & S_IXGRP) //execute/search permissions, group
                {
                    permissions[6] = 'x';
                }
                if(sb.st_mode & S_IROTH) //read permissions, others
                {
                    permissions[7] = 'r';
                }
                if(sb.st_mode & S_IWOTH) //write permissions, others
                {
                    permissions[8] = 'w';
                }
                if(sb.st_mode & S_IXOTH) //execute/search permissions, others
                {
                    permissions[9] = 'x';
                }
                //access the name/group name that did modiffications
                printf(" %s", permissions);
                //get the file owner and group
                struct passwd *pw = getpwuid(sb.st_uid);
                if(pw == NULL)
                {
                    perror("Error");
                    return -1;
                }
                struct group *gr = getgrgid(sb.st_gid);
                if(gr == NULL)
                {
                    perror("Error");
                    return -1;
                }
                printf(" %s %s", pw->pw_name, gr->gr_name);
                //get the file size
                printf(" %lld", (long long) sb.st_size);
                //get the file modification time
                char time_str[1024];
                strftime(time_str, 1024, "%b %e %H:%M", localtime(&sb.st_mtime));
                printf(" %s", time_str);
            }
        }
        printf("\n");
    }
    //close the directory
    if(closedir(dir) == -1)
    {
        perror("Error");
        return -1;
    }
    return 0;
}

int dirname_command(int argc, char *argv[])
{
    //check if there are more arguments
    if(argc != 2)
    {
        fprintf(stderr, "Error: invalid number of arguments\n");
        return -1;
    }
    //get the argument
    char *path = argv[optind];
    //find the last '/' character
    char *slash = strrchr(path, '/');
    if(!slash)
    {
        //no '/' found, print "."
        printf(".\n");
    }
    else if(slash == path)
    {
        //'/' found at the beginning of the string, print "/"
        printf("/\n");
    }
    else
    {
        //'/' found somewhere in the middle of the string, print everything up to it
        *slash = '\0';
        printf("%s\n", path);
    }
    return 0;
}

void help()
{
    printf("\nList of supported commands: \n");
    printf(">ls \n");
    printf(">tac \n");
    printf(">dirname \n");
    printf(">system supported commands \n");
    printf(">help \n");
    printf(">exit \n");
    printf("Thank you! \n\n");
}

void execute_command(int argc, char *argv[])
{
    //check if the command is "exit"
    if(strcmp(argv[0], "exit") == 0)
    {
        //exit the program
        exit(0);
    }
    int redirect = 0, append = 0, i, file_position = -1;
    for(i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], ">") == 0) //find if there is '>' to redirect
        {
            redirect = 1;
            file_position = i+1;
            break;
        }
        else if(strcmp(argv[i], ">>") == 0) //find if there is '>>' to append
        {
            append = 1;
            file_position = i+1;
            break;
        }
    }
    char *file = NULL;
    int fd1;
    if(redirect == 1) //if redirect found, we redirect the output to the file
    {
        fd1 = dup(STDOUT_FILENO);
        int fd;
        file = argv[file_position];
        argv[file_position - 1] = NULL;
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0744);
        if(fd == -1)
        {
            perror("open");
            exit(1);
        }
        close(STDOUT_FILENO);
        if(dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(fd);
    }
    if(append == 1) //if append found, we append the output to the file
    {
        fd1 = dup(STDOUT_FILENO);
        int fd;
        file = argv[file_position];
        argv[file_position - 1] = NULL;
        fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0744);
        if(fd == -1)
        {
            perror("open");
            exit(1);
        }
        close(STDOUT_FILENO);
        if(dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(fd);
    }
    int pid = fork();
    if(pid == 0)
    {
        //child process
        if(strcmp(argv[0], "exit") == 0)
        {
            exit(0);
        }
        else
        {
            //check if the command is "exit"
            if(strcmp(argv[0], "exit") == 0)
            {
                // exit the program
                exit(0);
            }
            //check if the command is "help"
            else if(strcmp(argv[0], "help") == 0)
            {
                help();
            }
            //check if the command is "dirname"
            else if(strcmp(argv[0], "dirname") == 0)
            {
                //execute the dirname command
                int command = dirname_command(argc, argv);
                if(command != 0)
                {
                    //command execution failed, print the error code
                    printf("Error: command 'dirname' returned %d\n", command);
                }
            }
            //check if the command is "ls"
            else if(strcmp(argv[0], "ls") == 0)
            {
                //execute the ls command
                int command = ls_command(argc, argv);
                if(command != 0)
                {
                    //command execution failed, print the error code
                    printf("Error: command 'ls' returned %d\n", command);
                }
            }
            //check if the command is "tac"
            else if(strcmp(argv[0], "tac") == 0)
            {
                execv("tac_silvana", argv);
            }
            else
            {
                //check if the command is "exit"
                if(strcmp(argv[0], "exit") == 0)
                {
                    // exit the program
                    exit(0);
                }
                execvp(argv[0], argv);
                printf("Wrong command\n");
                exit(0);
            }
        }
    }
    else if(pid == -1)
    {
        perror("fork");
        if(redirect == 1 || append == 1)
            close(fd1);
    }
    else
    {
        //parent process
        wait(NULL);
        if(redirect == 1 || append == 1)
        {
            if(dup2(fd1, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                exit(1);
            }
            close(fd1);
        }
    }
}

int main()
{
    while(1)
    {
        //show the prompt for starting the command line interpreter
        char *line = readline("> ");
        if(!line)
        {
            //end of input, exit the loop
            break;
        }
        //add the line to history
        add_history(line);
        //split the line into words
        char *word = strtok(line, " ");
        int argc = 0;
        char **argv = (char **)malloc(sizeof(char *)*10);
        while(word != NULL)
        {
            argv[argc] = (char *)malloc(sizeof(char) * (100));
            strcpy(argv[argc++], word);
            word = strtok(NULL, " ");
        }
        argv[argc] = NULL;
        //check if the command is empty
        if(argc == 0)
        {
            //empty command, skip this line
            continue;
        }
        //check if the command is "exit"
        if(strcmp(argv[0], "exit") == 0)
        {
            //exit the program
            exit(0);
        }
        execute_command(argc, argv);
        free(argv);
    }
    return 0;
}