#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

/*---------------------------------------------Beginning of constant declaration section---------------------------------------------*/
/*
 * The following constants are initialized to default values, the values can be changed at user discretion
 * but any non-default values have not been tested and may cause unexpected behaviour.
 */

#define DIR_MAX 2048        //Max size of a string containing directory.
#define HOME_MAX 256        //Max size of string containing home directory.
#define INPUT_MAX 2048      //Max size of input string.
#define ARGNUM_MAX 11       //Max number of arguments to parse. (includes executable to run)
#define ARGSIZE_MAX 256     //Max size of argument string.
#define VARNAME_MAX 64      //Max size of name of exported variable.
#define VARVAL_MAX 128      //Max size of value assigned to exported variable.

/*---------------------------------------------End of constant declaration section---------------------------------------------*/

char workDir[DIR_MAX] = "";
char home[HOME_MAX] = "/home/";
FILE *shellLog;

/*---------------------------------------------Beginning of error handling section---------------------------------------------*/
/*
 * The following functions handle errors resulting from pre-defined system calls,
 * if a function returns -1 its corresponding error handler is called to write the timestamp, the error message
 * to the shell log, and the error code.
 * Error messages are taken from the man pages.
 */

//Handler for getcwd()
void handleCWDError() {
    printf("ERROR: Could not obtain working directory, see log for details.\n");
    time_t timeNow;
    timeNow = time(NULL);
    fprintf(shellLog, "%sError occurred when calling getcwd():\n", asctime(localtime(&timeNow)));
    switch(errno) {
        case EACCES:
            fprintf(shellLog, "ERROR: Permission denied while setting up working directory (EACCES)\n");
            fflush(shellLog);
            break;
        case EFAULT:
            fprintf(shellLog, "ERROR: Cannot write to buffer specified at bad memory address (EFAULT)\n");
            fflush(shellLog);
            break;
        case EINVAL:
            fprintf(shellLog, "ERROR: Buffer provided is null (EINVAL)\n");
            fflush(shellLog);
            break;
        case ENAMETOOLONG:
            fprintf(shellLog, "ERROR: Current working directory path is too long (ENAMETOOLONG)\n");
            fflush(shellLog);
            break;
        case ENOENT:
            fprintf(shellLog, "ERROR: The current working directory has been unlinked (ENOENT)\n");
            fflush(shellLog);
            break;
        case ENOMEM:
            fprintf(shellLog, "ERROR: Out of memory (ENOMEM)\n");
            fflush(shellLog);
            break;
        case ERANGE:
            fprintf(shellLog, "ERROR: Provided size argument is less than length of path (ERANGE)\n");
            fflush(shellLog);
            break;
    }
}

//Handler for chdir()
void handleChDirError() {
    printf("ERROR: Cannot change directory, see log for more details.\n");
    time_t timeNow;
    timeNow = time(NULL);
    fprintf(shellLog, "%sError occurred when calling chdir():\n", asctime(localtime(&timeNow)));
    switch(errno) {
        case EACCES:
            fprintf(shellLog, "ERROR: Permission denied while accessing directory (EACCES)\n");
            fflush(shellLog);
            break;
        case EFAULT:
            fprintf(shellLog, "ERROR: Provided path is outside specified address space (EFAULT)\n");
            fflush(shellLog);
            break;
        case EIO:
            fprintf(shellLog, "ERROR: An I/O error has occurred (EIO)\n");
            fflush(shellLog);
            break;
        case ELOOP:
            fprintf(shellLog, "ERROR: Too many symbolic links in provided path (ELOOP)\n");
            fflush(shellLog);
            break;
        case ENAMETOOLONG:
            fprintf(shellLog, "ERROR: Path is too long (ENAMETOOLONG)\n");
            fflush(shellLog);
            break;
        case ENOENT:
            fprintf(shellLog, "ERROR: The specified path does not exist (ENOENT)\n");
            fflush(shellLog);
            break;
        case ENOMEM:
            fprintf(shellLog, "ERROR: Out of memory (ENOMEM)\n");
            fflush(shellLog);
            break;
        case ENOTDIR:
            fprintf(shellLog, "ERROR: One of the components of the path provided is invalid (ERANGE)\n");
            fflush(shellLog);
            break;
    }
}

//Handler for fgets()
void handleFgetsError() {
    printf("ERROR: Cannot take input, see log for more details\n");
    time_t timeNow;
    timeNow = time(NULL);
    fprintf(shellLog, "%sError occurred when calling fgets():\n", asctime(localtime(&timeNow)));
    fflush(shellLog);
}

//Handler for getlogin()
void handleGetLoginError() {
    printf("ERROR: Cannot get user details, see log for more details\n");
    time_t timeNow;
    timeNow = time(NULL);
    fprintf(shellLog, "%sError occurred when calling getlogin():\n", asctime(localtime(&timeNow)));
    switch(errno) {
        case EMFILE:
            fprintf(shellLog, "ERROR: Max number of file descriptors are currently open in process (EMFILE)\n");
            fflush(shellLog);
            break;
        case ENFILE:
            fprintf(shellLog, "ERROR: Max allowable number of files currently open in system (ENFILE)\n");
            fflush(shellLog);
            break;
    }
}

//Handler for setenv()
void handleSetEnvError() {
    printf("ERROR: Cannot set environment variable, see log for more details\n");
    time_t timeNow;
    timeNow = time(NULL);
    fprintf(shellLog, "%sError occurred when calling setenv():\n", asctime(localtime(&timeNow)));
    switch(errno) {
        case EINVAL:
            fprintf(shellLog, "ERROR: Provided variable name is null, of length 0 or contains '=' (EINVAL)\n");
            fflush(shellLog);
            break;
        case ENOMEM:
            fprintf(shellLog, "ERROR: Not enough memory to add variable to environment (ENOMEM)\n");
            fflush(shellLog);
            break;
    }
}

//Handler for execvp()
void handleExecError() {
    printf("ERROR: Cannot run executable, see log for more details\n");
    time_t timeNow;
    timeNow = time(NULL);
    fprintf(shellLog, "%sError occurred when calling execvp():\n", asctime(localtime(&timeNow)));
    switch(errno) {
        case E2BIG:
            fprintf(shellLog, "ERROR: The total number of bytes in the argument list is too large (E2BIG)\n");
            fflush(shellLog);
            break;
        case EACCES:
            fprintf(shellLog, "ERROR: Permissions denied for file (EACCES)\n");
            fflush(shellLog);
            break;
        case EFAULT:
            fprintf(shellLog, "ERROR: File path is outside of accessible address space (EFAULT)\n");
            fflush(shellLog);
            break;
        case EINVAL:
            fprintf(shellLog, "ERROR: An ELF executable tried to name more than one interpreter (EINVAL)\n");
            fflush(shellLog);
            break;
        case EIO:
            fprintf(shellLog, "ERROR: An I/O Error has occurred. (EIO)\n");
            fflush(shellLog);
            break;
        case EISDIR:
            fprintf(shellLog, "ERROR: An ELF interpreter was a directory (EISDIR)\n");
            fflush(shellLog);
            break;
        case ELIBBAD:
            fprintf(shellLog, "ERROR: An ELF interpreter was not in a recognized format (ELIBBAD)\n");
            fflush(shellLog);
            break;
        case ELOOP:
            fprintf(shellLog, "ERROR: Too many symbolic links were encountered in resolving name of executable (ELOOP)\n");
            fflush(shellLog);
            break;
        case EMFILE:
            fprintf(shellLog, "ERROR: The process has the maximum number of files open (EMFILE)\n");
            fflush(shellLog);
            break;
        case ENAMETOOLONG:
            fprintf(shellLog, "ERROR: Provided file name is too long (ENAMETOOLONG)\n");
            fflush(shellLog);
            break;
        case ENFILE:
            fprintf(shellLog, "ERROR: System limit on total number of open files has been reached (ENFILE)\n");
            fflush(shellLog);
            break;
        case ENOENT:
            fprintf(shellLog, "ERROR: The executable does not exist, or a shared library required cannot be found (ENOENT)\n");
            fflush(shellLog);
            break;
        case ENOEXEC:
            fprintf(shellLog, "ERROR: The executable is not in a recognized format, is for the wrong architecture, or has other format errors (ENOEXEC)\n");
            fflush(shellLog);
            break;
        case ENOMEM:
            fprintf(shellLog, "ERROR: Not enough memory to execute (ENOMEM)\n");
            fflush(shellLog);
            break;
        case ENOTDIR:
            fprintf(shellLog, "ERROR: A component of the provided file path is not a directory (ENOTDIR)\n");
            fflush(shellLog);
            break;
        case EPERM:
            fprintf(shellLog, "ERROR: The process is being traced, the user is not the superuser and the file has the set-user-ID or set-group-ID bit set (EPERM)\n");
            fflush(shellLog);
            break;
        case ETXTBSY:
            fprintf(shellLog, "ERROR: Executable was open for writing by one or more processes (ETXTBSY)\n");
            fflush(shellLog);
            break;
    }
}

/*---------------------------------------------End of error handling section---------------------------------------------*/

/*
 * Function that checks a string for any environment variables, then replaces it with its value.
 */
int loadEnvironmentVars(char* str) {
    //Initialize character pointers.
    char* substring = malloc(ARGSIZE_MAX * sizeof(char));
    char* temp = NULL;

    //Save reference to original memory location.
    temp = substring;

    //Make a copy of input string.
    strcpy(substring, str);

    //Main loop, keeps running until no more '$' characters are found in copy of input string
    while((substring = strstr(substring, "$")) != NULL) {
        char varName[VARNAME_MAX] = "";
        int i = 1;

        //Get variable name
        while(substring[i] != ' ' && substring[i] != '\0' && substring[i] != '\"') {
            varName[i-1] = substring[i];
            i++;
        }

        //Get variable value.
        char* varVal = getenv(varName);

        //Set value to empty string if not found.
        if(varVal == NULL) varVal = "";

        //Make a copy of substring to restore at end of loop.
        char oldSubstring[ARGSIZE_MAX] = "";
        strcpy(oldSubstring, substring);

        i = 0;
        int j = 0;
        int k = 0;

        //Overwrite variable name with variable value
        while(varVal[k] != '\0') {
            substring[i++] = varVal[k++];
        }

        //Skip variable in copy of substring
        while(oldSubstring[j] != ' ' && oldSubstring[j] != '\0' && oldSubstring[j] != '\"') {
            j++;
        }

        //Restore characters after variable name
        while(oldSubstring[j] != '\0') {
            substring[i++] = oldSubstring[j++];
        }
        substring[i] = '\0';
    }
    //Place result in input string
    strcpy(str, temp);
    free(temp);
}

/*
 * Function to log the termination of a child process.
 */
void writeReapingMsg(pid_t childID) {
    time_t timeNow = time(NULL);
    fprintf(shellLog, "%s child process with PID %i has terminated\n", asctime(localtime(&timeNow)), childID);
    fflush(shellLog);
}

/*
 * Function to handle any incoming SIGCHLD signals and reap child accordingly.
 */
void handleChildSignals() {
    pid_t pid = 0;
    int status = 0;

    // Loop for all terminating children.
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        writeReapingMsg(pid);
    }
}

/*
 * Function to initialize directories on shell launch.
 */
void initEnvironment() {
    char* activeUser = getlogin();
    if(activeUser == NULL) {
        handleGetLoginError();
        exit(EXIT_FAILURE);
    }
    strcat(home, activeUser);
    strcat(home, "/");
    if(getcwd(workDir, DIR_MAX) == NULL) {
        handleCWDError();
        exit(EXIT_FAILURE);
    }
    strcat(workDir, "/");
    if(chdir(workDir) == -1) {
        handleChDirError();
        exit(EXIT_FAILURE);
    }
}

/*
 * Function to print working directory and replace home directory string with '~' character.
 */
void printDir() {
    char* tilde = strstr(workDir, home);
    if(tilde != NULL) {
        printf("~%s", (workDir + strlen(home) - 1));
    } else {
        printf("%s", workDir);
    }
}

/*
 * Function to print prompt to user then take input.
 */
void takeInput(char* inputStore, int size) {
    printf("OShell:");
    printDir();
    printf(">> ");
    if(fgets(inputStore, size, stdin) == NULL) handleFgetsError();
    inputStore[strlen(inputStore) - 1] = '\0';
}

/*
 * Function to split a string according to delimiter provided
 * Ignores delimiters within pairs of '"'.
 * Returns number of strings after splitting.
 */
int splitString(char* input, char delimiter, int maxSize, char stringStore[][maxSize+1]) {
    int i = 0;
    int j = 0;
    int k = 0;
    while (input[i] != '\0') {
        //Detect quotes to ignore delimiters inside
        if(input[i] == '\"') {
            i++;
            while(input[i] != '\"') {
                stringStore[j][k++] = input[i++];
            }
            //On delimiter find, split string.
        } else if(input[i] == delimiter){
            j++;
            k = 0;
        } else {
            if(k == maxSize) {
                j++;
                k = 0;
            }
            stringStore[j][k++] = input[i];
        }
        i++;
    }
    return j;
}

/*
 * Function to parse input into arguments.
 * Returns an integer based on whether a '&' char was entered (0 if entered, 1 else).
 */
int parseInput(char* input, int* argNumStore, char** argumentStore) {
    char strings[ARGNUM_MAX][ARGSIZE_MAX] = {""};
    loadEnvironmentVars(input);
    splitString(input, ' ', ARGSIZE_MAX-1, strings);
    for(int i = 0; i < ARGNUM_MAX; i++) {
        if(strings[i][0] == '&') {
            *argNumStore = i;
            return 0;
        }
        if(strings[i][0] == '\0') {
            *argNumStore = i;
            return 1;
        }
        strcpy(argumentStore[i], strings[i]);
    }
}

/*
 * Function to change working directory (implementation of cd command).
 */
void cd(char* arg) {
    //Back-up working directory
    char workDirCopy[DIR_MAX] = "";
    strcpy(workDirCopy, workDir);

    //Case "cd"
    if(arg == NULL) {
        return;
    }
    //Case "cd ~" and "cd ~/path"
    if(arg[0] == '~') {
        strcpy(workDir, home);
        if(strlen(arg) > 2) strcat(workDir, &arg[2]);
        if(chdir(workDir) == -1) {
            handleChDirError();
            //Restore from back-up
            strcpy(workDir, workDirCopy);
        }
    }
    //Case "cd .."
    else if (strcmp(arg, "..") == 0) {
        int i = 0;
        while(workDir[i+1] != '\0') {
            i++;
        }
        do {
            workDir[i] = '\0';
            i--;
        } while (workDir[i] != '/');
        if(chdir(workDir) == -1) {
            handleChDirError();
            //Restore from back-up
            strcpy(workDir, workDirCopy);
        }
    }
    //Case "cd absolute_path"
    else if (arg[0] == '/') {
        strcpy(workDir, arg);
        //Add slash at end if not found.
        if(workDir[strlen(workDir) - 1] != '/') workDir[strlen(workDir)] = '/';
        if(chdir(workDir) == -1) {
            handleChDirError();
            //Restore from back-up
            strcpy(workDir, workDirCopy);
        }
    }
    //Case "cd relative_path"
    else {
        strcat(workDir, arg);
        //Add slash at end if not found.
        if(workDir[strlen(workDir) - 1] != '/') workDir[strlen(workDir)] = '/';
        if(chdir(workDir) == -1) {
            handleChDirError();
            //Restore from back-up
            strcpy(workDir, workDirCopy);
        }
    }
}

/*
 * Function to print to terminal a string (implementation of echo command).
 */
void echo(char* arg) {
    if(arg == NULL) {
        printf("\n");
    }
    printf("%s\n", arg);
}

/*
 * Function to declare an environment variable (implementation of export command).
 */
void export(char* arg) {
    char varName[VARNAME_MAX] = "";
    char varVal[VARVAL_MAX] = "";
    int i = 0;

    //Loop to obtain variable name
    while(arg[i] != '\0') {
        if(arg[i] == '=') {
            i++;
            break;
        }
        varName[i] = arg[i];
        i++;
    }
    int j = 0;

    //Loop to obtain variable value
    while(arg[i] != '\0') {
        varVal[j++] = arg[i++];
    }

    if(setenv(varName, varVal, 1) == -1) {
        handleSetEnvError();
    }
}

/*
 * Function to call appropriate built-in command implementation.
 */
void builtIn(char command, char* arg) {
    switch(command) {
        case 'c':
            cd(arg);
            break;
        case 'e':
            echo(arg);
            break;
        case 'x':
            export(arg);
            break;
        default:
            break;
    }
}

/*
 * Function to execute a non-built-in command.
 * Forks and then child process is overwritten with executable.
 * By default, does not wait until child terminates unless doWait argument is non-zero.
 */
void executeCommand(int argNum, char** args, int doWait) {
    pid_t childID = fork();
    int status = 0;
    //If current process is child.
    if (!childID) {
        //Make a copy of arguments to not set provided argument pointer to NULL
        char** argCpy = malloc(ARGNUM_MAX * sizeof(char *));
        for(int i = 0; i < ARGNUM_MAX; i++) {
            argCpy[i] = malloc(ARGSIZE_MAX * sizeof(char));
            argCpy[i] = memcpy(argCpy[i], args[i], ARGSIZE_MAX * sizeof(char));
        }
        argCpy[argNum] = NULL;
        if (execvp(argCpy[0], argCpy) == -1) {
            handleExecError();
            exit(EXIT_FAILURE);
        }
    } else {
        if(doWait) {
            waitpid(childID, &status, 0);
            writeReapingMsg(childID);
            //100 milliseconds of sleep to wait for any messages printed by exiting child process.
            usleep(100000);
        }
    }
}

int shell() {
    char input[INPUT_MAX] = "";

    //Initialize arguments string array.
    char** arguments = malloc(ARGNUM_MAX * sizeof(char *));
    for(int i = 0; i < ARGNUM_MAX; i++) {
        arguments[i] = malloc(ARGSIZE_MAX * sizeof(char));
    }

    int argNum = 0;
    int running = 1;
    do {
        takeInput(input, INPUT_MAX);
        int doWait = parseInput(input, &argNum, arguments);

        //Check to see which command to execute
        if(strcmp(arguments[0], "exit") == 0) {
            running = 0;
        } else if(strcmp(arguments[0], "cd") == 0) {
            builtIn('c', arguments[1]);
        } else if(strcmp(arguments[0], "echo") == 0) {
            builtIn('e', arguments[1]);
        } else if(strcmp(arguments[0], "export") == 0) {
            builtIn('x', arguments[1]);
        } else {
            executeCommand(argNum, arguments, doWait);
        }

        //Clear arguments after each loop
        for(int i = 0; i < ARGNUM_MAX; i++) {
            arguments[i] = memset(arguments[i], 0, ARGSIZE_MAX * sizeof(char));
        }
    } while (running);

    //Free allocated variables before terminating function
    for(int i = 0; i < ARGNUM_MAX; i++) {
        free(arguments[i]);
    }
    free(arguments);
    return 0;
}

int main()
{
    //Open log file (Creates file if not found, if found clears it)
    shellLog = fopen("shell_log.txt", "w");

    //Declare SIGCHLD handler.
    signal(SIGCHLD, handleChildSignals);

    initEnvironment();
    shell();

    //Close log file
    fclose(shellLog);

    //Exit program successfully
    exit(EXIT_SUCCESS);
}