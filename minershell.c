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

		

			int rc = fork();
			if (rc < 0){ 
				// fork failed; exit
				fprintf(stderr, "fork failed\n");
				exit(1);
			}

			// child (new process)
			else if (rc == 0){


				//switching to dupping mode
				//Counting tokens
    			int count = 0;
				char **clone = tokens;
    			while(*clone){
        			clone++;
        			count += 1;
				}

				// Changing outputs by detecting > argument
				if(count >= 2 && tokens[count-2] && strcmp(tokens[count-2], ">") == 0){
					printf("%s", "Duplicating\n");
					int fw = open(tokens[count-1],  O_WRONLY | O_CREAT | O_TRUNC);
					dup2(fw, STDOUT_FILENO);
					dup2(fw, STDERR_FILENO);

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
			

				else if(strcmp(*tokens, "echo")==0){
					char *pointer = line+5;
					execlp(*tokens, *tokens, pointer, (char *)NULL);
				}
				else if(execlp(*tokens, *tokens, tokens[1], (char *)NULL) == -1){
					printf("%s", "Please enter a real command\n");
				}
				exit(0);
			}
			else{
			wait(NULL); /* reaping parent */
			}
		}

		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++){
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}




