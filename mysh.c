///////////////////////////////////////////////////////////////////://
/// Implements a shell
/// @author julie sojkowski jas7845@g.rit.edu
/// @date: 4/12/19
/////////////////////////////////////////////////////////////////////
#define _DEFAULT_SOURCE
#include <sys/wait.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static int VERBOSE = 0;
static int HISTORY = 10;
static int COUNT = 1;
static int SIZE = 10;


/// Function for internal command history
/// @param argc: cast to void
/// @param history_list: the list of commands
/// return: 0
int history(int argc, char * history_list[]){
        (void) argc;
        // if the number of commands is less than the history
        if(COUNT <= HISTORY){
                for (int i = 1; i< COUNT; i++){
                printf("\t%i: %s\n", i, history_list[i-1]);
                }
        }
        else{
                for (int i = COUNT-HISTORY; i< COUNT; i++){
                        printf("\t%i: %s\n", i, history_list[i-1]);
                }
        }
        return 0;
}


/// Funciton for internal command help
/// @param argc: cast to void 
/// @param argv: cast to void
/// return: 0
int help(int argc, char * argv[]){

        (void) argc;
        (void) argv;
        printf("Ineternal Command Options:\n");
        printf("!N:\t\tre-exucute the N-th command on the history list\n");
        printf("help:\t\tprint a list of internal commands\n");
        printf("history:\tprint a list of commands ececuted so far\n");
        printf("quit:\t\tterminate the shell\n");
        printf("verbose on/off:\tturn shell verbose on or off\n");
        fflush(stdin);
        return 0;
}


/// Function to free history list
/// @param argc: cast to void
/// @param history_list: history list to free
/// return: 0
int quit(int argc, char * history_list[]){

        (void)argc;
        for (int i = 0; i < COUNT-1; i++){
                free(history_list[i]);
        }
        free(history_list);
        return 0;
}


/// Function to validate the bang number
/// @param hist: the number to validate
/// return: 0 if invalid, 1 if valid
int checknum(int hist){

        if(hist < COUNT && hist > 0){
                if(COUNT > HISTORY){
                        if(hist < (COUNT-HISTORY)){
                                fprintf(stderr, "Bang number %i is not in range %i to %i \n",
					hist, COUNT-HISTORY, COUNT);
				return 0;
                        }
                        else return 1;
                }
                return 1;
        }
        fprintf(stderr, "Bang number %i is not in range 0 to %i\n", hist, COUNT);
        return 0;
}


/// Funcition to change the verbose status
/// @param argc: cast to void
/// @param argv: history list with the command line input
/// @return: 0
int verbose(int argc, char * argv[]){

        (void)argc;

        char *pch[3];
        char line[strlen(argv[COUNT -2])];
        strcpy(line, argv[COUNT -2]);

        pch[0] = strtok( line, " \n" );
        int i =0;
        while ( pch[i] != NULL ) {
                i++;
                pch[i] = strtok( NULL, " \n" );
        }
        if(strcmp(pch[1], "off") == 0)
                VERBOSE = 0;
        else if(strcmp(pch[1], "on") == 0)
                VERBOSE = 1;
        else printf("usage: verbose  on | off\n");
        return 0;
}

/// Function to preform the bang command
/// @param hist_num: the number the user entered for the bang
/// @param history_list: list of former commands
/// return: 0 if the command correleating to the number is on the 
///     history list or the hist_num if the command must be execvp'd
int bang(int hist_num, char * history_list[]){
        int res = checknum(hist_num);
        if(res == 1){
                char command[strlen(history_list[hist_num -1])];
                strcpy(command, history_list[hist_num -1]);
                history_list[COUNT-2] = strdup( command); // was strcpy
                printf("comm: %s\n", command); // debug
                char fir[30];
                for(unsigned int h = 0; h< strlen(command); h++){
                        if(command[h] == ' ' || command[h] =='\n')
                                break;
                        else fir[h] = command[h];
                }
		if(strcmp(command, "help") == 0){
                        help(0, history_list);
                        return 0;
                }
                else if (strcmp(command, "history") == 0){
                        history(0, history_list);
                        return 0;
                }
                else if (strcmp(fir, "verbose") == 0){
                        verbose(0, history_list);
                        return 0;
                }
                else return hist_num;
        }
        return 0;
}

int main( int argc, char * argv[]) {


        extern char *optarg;
        extern int optind;
        int c, err = 0;
        char *  end;
        int num = 10;
        while(( c = getopt(argc, argv, "vh:")) != -1){
                switch(c) {
                        case 'h':
                                printf("h\n");
                                if((num = strtol(optarg, &end, 10 )) > 0){
                                        HISTORY = num;
                                }
                                else fprintf(stderr, "usage: mysh [-v] [-h pos_num]\n");
                                break;
                        case 'v':
                                printf("v\n");
                                VERBOSE = 1;
                                break;
			case'?':
                                printf("none");
                                break;


                }
                if(err){
                        fprintf(stderr, "usage: mysh [-v] [-h pos_num]\n");
                        return EXIT_FAILURE;
                }
        }
        char ** history_list;
	history_list = malloc(sizeof(char *)*HISTORY);
        SIZE = HISTORY;
        for(int i = 0; i< HISTORY; i++){
                history_list[i] = NULL;
        }

        char *line = NULL;
        while(1){
                printf("mysh[%i]> ", COUNT);
                size_t implen;

                getline(&line, &implen, stdin);
                char imp[strlen(line)];
                unsigned int h = 0;
                for (h = 0; h< strlen(line); h++){
                        if(line[h] != '\n'){
                                imp[h] = line[h];
                        }
                }
                imp[h-1] = '\0';

                // check for quit command
		if(feof(stdin) || strcmp(imp, "quit") == 0){
                        quit(0, history_list);
                        break;
                }
                //check for blank line
                if(imp[0] != '\0'){
                        //  assign history
                        history_list[COUNT -1] = strdup(imp);
                        COUNT++;
                        //  resize
                        if(COUNT == SIZE){
                                char ** temp = malloc(COUNT *  sizeof(char*));

                                for(int i = 0; i< COUNT-1; i++){
                                        temp[i] = strdup(history_list[i]);
                                        free(history_list[i]);
                                }
                                SIZE = COUNT + HISTORY;
                                history_list = realloc(history_list, sizeof(char*) *(SIZE));
				for(int i = 0; i< COUNT -1; i++){
                                        history_list[i] = strdup(temp[i]);
                                        free(temp[i]);
                                }
                                free(temp);
                        }
                        // check for which command
                        char first[strlen(imp)];
                        for(unsigned int h = 0; h< strlen(imp); h++){
                                if(imp[h] == ' ' || imp[h] =='\n')
                                        break;
                                else first[h] = imp[h];
                        }
                        first[h] = '\0';
                        if(strcmp(imp, "help") == 0){
                                help(0, argv);
                        }
                        else if (strcmp(imp, "history") == 0){
                                history(0, history_list);
                        }
			else if (strstr(imp, "verbose") != NULL){
                                verbose(argc, history_list);
                        }
                        else if(first[0] == '!'){   // bang command
                                char number[] = " ";
                                int l = 0;
                                for(unsigned int i = 0; i< strlen(imp); i++){
                                        if(isdigit(imp[i])){
                                                number[l] = imp[i];
                                                l++;
                                        }
                                }
                                //get the history number
                                int hist_num = strtol(number, NULL, 10);
                                int res = bang(hist_num, history_list);
                                if( res != 0){ // if it is not an internal command
                                        char command[] =" ";
                                        strcpy(command, history_list[res-1]);
                                        char *pch[10];
                                        pch[0] = strtok( command, " \n" );
                                        int i =0;
					while ( pch[i] != NULL ) {
                                                i++;
                                                pch[i] = strtok( NULL, " \n" );
                                        }
                                        if(VERBOSE == 1){
                                                printf("\tcommand: %s\n\n", command);
                                                int err;
                                                printf("\timput command tokens:\n");
                                                for(int j = 0; j < i; j++){
                                                        printf("\t%i: %s\n", j, pch[j]);
                                                }
                                                int pid = fork();
                                                if(pid == 0){
                                                        printf("\twait for pid %i: %s\n",
                                                                        pid, pch[0]);
                                                        printf("\texecvp: %s\n", pch[0]);
                                                        execvp(pch[0], pch);
                                                        perror("exec");
                                                        exit(1);
                                                }
                                                else wait(&err);
						if(WIFEXITED(err))
                                                        printf("status: %d\n", WEXITSTATUS(err));
                                        }
                                        else{
                                                int pid = fork();
                                                if(pid == 0){
                                                        execvp(pch[0], pch);
                                                        perror("exec");
                                                        exit(1);
                                                }
                                                else wait(NULL);
                                                if(WIFEXITED(err)== 0){
                                                        printf("command status: %d\n",
                                                                        WEXITSTATUS(err));
                                                }
                                        }
                                }
                        }
			else{ // fork and run execvp
                                char *pch[10];
                                pch[0] = strtok( imp, " \n" );
                                int i =0;
                                while ( pch[i] != NULL ) {
                                        i++;
                                        pch[i] = strtok( NULL, " \n" );
                                }
                                if(VERBOSE == 1){
                                        printf("\tcommand: %s\n\n", imp);
                                        int err;
                                        printf("\timput command tokens:\n");
                                        for(int j = 0; j < i; j++){
                                                printf("\t%i: %s\n", j, pch[j]);
                                        }
                                        int pid = fork();
                                        if(pid == 0){
                                                printf("\twait for pid %i: %s\n", pid, pch[0]);
                                                printf("\texecvp: %s\n", pch[0]);
                                                execvp(pch[0], pch);
                                                perror("exec");
					exit(1);
                                        }
                                        else wait(&err);
                                        if(WIFEXITED(err))
                                                printf("command status: %d\n", WEXITSTATUS(err));
                                }
                                else{
                                        int pid = fork();
                                        if(pid == 0){
                                                execvp(pch[0], pch);
                                                perror("exec");
                                                exit(1);
                                        }
                                        else wait(NULL);
					if(WIFEXITED(err) == 0)
                                                printf("command status: %d\n", WEXITSTATUS(err));
                                }
                        }
                }
        }
        // free the malloc'd string from getline()
        free(line);
        return 0;
}

