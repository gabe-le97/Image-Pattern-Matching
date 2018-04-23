/*
 * -------------------------------------------------------------------------------------
 * File: parent.c
 * @Author: Gabe Le
 * Due Date: 4/8/18
 * .....................................................................................
 * Reads the contents of an image and pattern file and checks if the pattern exists
 *  in the image. If it does, then save the amount of matches and the coordinates. If it
 *  doesn't, then save 0. Finally, display the pattern name and the total amount of
 *  matches it has in all the image files.
 * This program just displays the matches and the pattern name
 *
 * This version compiles the helper code & creates children (as many that there are image files) that call exec.
 * The external program writes the matches to a file and this program displays it.
 *
 * =====================================================================================
 * Note: Please run the script before running the program to avoid segmentation errors
 * Note: The helper C file is compiled when this program is run
 * Note: Strange bug that puts garbage in my patternFileName array
 *
 * Here is how I compiled and ran my program
 *  gcc -Wall parent.c -o parent
 * 	./parent /Users/gabe/Desktop/Test/Data/Images /Users/gabe/Desktop/Test/Data/Patterns
 * -------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//-----------------------------------------------------
int main(int argc, char *argv[]) {
	// holds the paths to all files in the directory
	char **imageFilePath;
	char **patternFileNames;
	int imageFileCount = 0;
	int patternFileCount = 0;
	// we want 2 file directories to be checked
	int numDirectories = 1;
    int fileCount = 0;
	DIR *directory;
	struct dirent *entry;

    system("gcc -Wall helper.c -o helper");
	//------------------------------------
	//	Check if the file directories are valid & get
	//   the number of files in each directory
	//------------------------------------
	while(numDirectories < argc) {
		directory = opendir(argv[numDirectories]);
	    if (directory == NULL) {
	        printf("data folder %s not found\n", argv[numDirectories]);
	        exit(0);
	    }
		while ((entry = readdir(directory)) != NULL) {
		    if (entry->d_type == DT_REG) {
		         fileCount++;
		    }
		}
		closedir(directory);
		if(numDirectories == 1) {
			imageFileCount = fileCount;
		} else {
			patternFileCount = fileCount;
		}
		numDirectories++;
		fileCount = 0;
	}

	// we will run script.sh if we need to modify the end of the text files
    printf("\n%s", "Run the script to modify the files? Y/N: ");
    char choice;
    scanf("%c", &choice);
    if(choice == 'Y' || choice == 'y') {
        char cmdLine[256];
        sprintf(cmdLine, ".././script.sh %s %s", argv[1], argv[2]);
        system(cmdLine);
    }

    int k = 0;
    directory = opendir(argv[1]);
    imageFilePath = (char**) malloc(imageFileCount*sizeof(char*));
    //--------------------------------------------------------------
    // Get the names in the image directory and add the name to them
    //--------------------------------------------------------------
    while ((entry = readdir(directory)) != NULL) {
	    char* name = entry->d_name;
	    // Ignores "invisible" files (name starts with . char)
	    if (name[0] != '.') {
	        imageFilePath[k] = malloc((strlen(argv[1]) + strlen(name)+2)*sizeof(char));
	        strcpy(imageFilePath[k], argv[1]);
	        // check if user path had / at the end
	        int endsInSlash = (argv[1] && *argv[1] && argv[1][strlen(argv[1]) - 1] == '/') ? true : false;
	       	if(!endsInSlash) {
	        	strcat(imageFilePath[k], "/");
	    	}
	        strcat(imageFilePath[k], name);
	        k++;
	    	}
	}
    closedir(directory);

    k = 0;
    directory = opendir(argv[2]);
    patternFileNames = (char**) malloc(patternFileCount*sizeof(char*));
    //--------------------------------------------------------------
    // This version is only allowed a main function so this duplication
    //  is necessary for the pattern names
    //--------------------------------------------------------------
    while ((entry = readdir(directory)) != NULL) {
	    char* name = entry->d_name;
	    // Ignores "invisible" files (name starts with . char)
	    if (name[0] != '.') {
	        patternFileNames[k] = malloc((strlen(argv[2]) + strlen(name)+2)*sizeof(char));
	        patternFileNames[k] = name;
	        k++;
	    	}
	}
    closedir(directory);

	// keep track of child pid for reading & naming files later
    pid_t* childPidArray;
    childPidArray = (pid_t*) malloc(imageFileCount*sizeof(pid_t));

    printf("%s\n", imageFilePath[0]);

	//------------------------------------------------------------------------
    // Fork as many times as there are image files to make our child processes
    //------------------------------------------------------------------------
	for(int imageFileId = 0; imageFileId < imageFileCount; imageFileId++) {
		pid_t pid = fork();
		if(pid == 0) {
			printf("%s %d created & running\n", "Child", getpid());
			// call the helper program & pass path to file and pattern directory
			char *args[] = {"./helper", imageFilePath[imageFileId], argv[2], NULL};
			execvp(args[0], args);
		}
		else if (pid > 0){
            childPidArray[imageFileId] = pid;
        }
        else {
        	printf("Fork failed\n");
        	exit(1);
        }
	}
	free(imageFilePath);

	int status = 0;
    // wait until all the children are done to execute parent
    for(int imageFileId = 0; imageFileId < imageFileCount; imageFileId++) {
		waitpid(childPidArray[imageFileId], &status, 0);
		printf("%s %d %s\n", "Child", childPidArray[imageFileId], "terminating");
    }

    printf("\nParent process running");
    // contains a tally of how many matches have been found
    int patternMatchArray[patternFileCount];
    // allocate all spots in the array to 0 to avoid garbage values
    memset(patternMatchArray, 0, sizeof patternMatchArray);

    // look at all the matches of each pattern file
    for(int i = 0; i < imageFileCount; i++) {
        char buf[sizeof(getpid())+15];
        snprintf(buf, sizeof buf, "%s%d%s", "P_", childPidArray[i], "_output.txt");
        char mystring[sizeof(patternMatchArray)*2];
        FILE* fptr;
        char character;
        int index = 0;
        fptr = fopen (buf, "r");
        // read until we hit a space to signal the end of the number
        while(fgets(mystring, sizeof(mystring)*2, fptr) != NULL) {
            int i = -1;
            while(++i < strlen(mystring)) {
                if ((character = mystring[i]) != ' ') {
                    break;
                }
            }
            // converting our string to an int (changing ASCII)
            int count = character - '0';
            patternMatchArray[index] += count;
            index++;
        }
        fclose(fptr);
        
	}
	printf("\n");

    // we don't need the child pids anymore so free them from memory
    free(childPidArray);
    printf("\n");


    // print the file name and then the tally of occurence
    for(int i = 0; i < patternFileCount; i++) {
        printf("%s has %d matches --> ", patternFileNames[i], patternMatchArray[i]);
        for(int j = 0; j < patternMatchArray[i]; j++) {
            printf("|");
        }
        printf("\n");
    }
    free(patternFileNames);

}
