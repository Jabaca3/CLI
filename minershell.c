#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line){
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++){

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';

			if (tokenIndex != 0){
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else{
			token[tokenIndex++] = readChar;
		}
	}
	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}



int main(int argc, char *argv[]){
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;

	while (1){
		/* BEGIN: TAKING INPUT */
		//dup2(STDOUT_FILENO, 1);
		bzero(line, sizeof(line));
		printf("minersh$ ");
		scanf("%[^\n]", line);
		getchar();

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);


		// If no tokens found we dont execute anything
		if(*tokens == NULL){
			//pass
		}
		
		
		// Testing for user input "cd"
		// This changes the current directory of this process
		else if(strcmp(*tokens, "cd")==0){
			if(chdir(tokens[1]) == -1){
				printf("%s", "Incorrect command to the display and prompting for the next command.\n");
			}
		}



		// Testing for user input "exit"
		// This exits the main program for this process
		else if (strcmp(*tokens, "exit") == 0 ){
			printf("%s", "Thank you, now exiting program\n");
			exit(0);
		}
		




		// Checking user input for various commands that may exist
		else if(*tokens){

		// Searching for | boolean value piping to determin if we are piping later
			char **index = tokens;
				int piping = 0;	
				while( *index != 0 ){
					if( strcmp(*index, "|") ==0 ){
						printf("%s", "Piping...\n");
						piping = 1;
						break;
					}
					index++;
				}

			int fd[2];
			if(piping && pipe(fd) == -1){
				char error_message[30] = "Anerrorhasoccurred\n";
				printf("%s", "were erroring here");
				write(STDERR_FILENO,error_message,strlen(error_message));
			}



//----------------------------------------------Creating child proces---------------------------------------------------
			int rc = fork();
			// If child process fails exit
			if (rc < 0){ 
				fprintf(stderr, "fork failed\n");
				exit(1);
			}

			// Child (new process) if it exist
			else if (rc == 0){

				//Counting tokens
    			int count = 0;
				char **clone = tokens;
    			while(*clone){
        			clone++;
        			count += 1;
				}

// Handeling piping here ------------------------------------------

				if(piping){
					dup2(fd[1], STDOUT_FILENO);
					close(fd[0]);
					close(fd[1]);
					execlp(tokens[0], tokens[0], (char *)NULL);
				}

// Handeling piping here ------------------------------------------

				// Switching to dupping mode
				// Changing outputs by detecting > argument
				// Taking in amount of flags (Not dynamic)

				if(!piping && count >= 2 && tokens[count-2] && strcmp(tokens[count-2], ">") == 0){
					printf("%s", "Duplicating\n");
					int fw = open(tokens[count-1],  O_WRONLY | O_CREAT | O_TRUNC);
					dup2(fw, STDOUT_FILENO);
					dup2(fw, STDERR_FILENO);

					// Specific for echo because echo has different properties than other commands
					if(strcmp(*tokens, "echo")==0){
						char *pointer = line+5;
						execlp(*tokens, *tokens, tokens[1], (char *)NULL);
					} 
					// Taking in 2 flags
					else if(count == 5 && execlp(*tokens, *tokens, tokens[1], tokens[2], (char *)NULL) == -1){
						printf("%s", "Please enter a real command\n");
					}
					// Taking in 1 flag
					else if(count == 4 && execlp(*tokens, *tokens, tokens[1], (char *)NULL) == -1){
						printf("%s", "Please enter a real command\n");
					}
					// Taking in no flags
					else if(count == 3 && execlp(*tokens, *tokens, (char *)NULL) == -1){
						printf("%s", "Please enter a real command\n");
					}
					close(fw);
				}
			

				// Specific for echo because echo has different properties than other commands
				else if(!piping && strcmp(*tokens, "echo")==0){
					char *pointer = line+5;
					execlp(*tokens, *tokens, pointer, (char *)NULL);
				}

				// The standard execution for a single command
				else if(!piping && execlp(*tokens, *tokens, tokens[1], (char *)NULL) == -1){
					printf("%s", "Please enter a real command\n");
				}
				exit(0);
			}
			
			else{
				if(piping){
					int child2 = fork();
					if(child2 == 0){
						dup2(fd[0], STDIN_FILENO);
						close(fd[0]);
						close(fd[1]);
						if(execlp(tokens[2], tokens[2], (char *)NULL)==-1){
							printf("%s", "Execlp broke or something");
						}
						exit(0);
					}
					else{
						close(fd[0]);
						close(fd[1]);
						wait(NULL);
					}
				}
				wait(NULL);
			}
		}
//----------------------------------------------Ending processes---------------------------------------------------

		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++){
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}




