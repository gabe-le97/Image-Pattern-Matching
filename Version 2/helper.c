/*
 * -------------------------------------------------------------------------------------
 * File: helper.c
 * @Author: Gabe Le
 * Due Date: 4/8/18
 * .....................................................................................
 * Reads the contents of an image and pattern file and checks if the pattern exists
 *  in the image. If it does, then save the amount of matches and the coordinates. If it
 *  doesn't, then save 0. Finally, display the pattern name and the total amount of
 *  matches it has in all the image files.
 *  
 * This version is called by another program using a member of the exec family
 * 
 * =====================================================================================
 * Note: Please run the script before running the program to avoid segmentation errors
 * Note: This file does not need to be compiled; parent.c does this
 *
 * Here is how I compiled and ran my program
 *  gcc -Wall helper.c -o helper
 *  ./helper /Users/gabe/Desktop/Test/Data/Images /Users/gabe/Desktop/Test/Data/Patterns
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
//----------------------------------------------------------------------------------------
void checkFiles(char *dataRootPath, int *numberOfFiles, char ***filePath, char ***patternFileNames, bool isPatterns);
void textFileDimensions(char *firstLine, char *fileName, int *fileHeight, int *fileWidth);
void fillArray(char *fileName, int height, int width, char myArray[height][width]);
int hasPattern(int imageHeight, int imageWidth, int patternHeight, int patternWidth, 
                char imageArray[imageHeight][imageHeight], char patternArray[patternHeight][patternWidth], int *searchResults);
void writeToFile(int arraySize, int *searchResults, char *filePath);
//----------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int patternFileCount;
    // holds the paths to all files in the directory
    char** patternFilePath;
    // holds the name of all the pattern files for display later
    char** patternFileNames;


    checkFiles(argv[2], &patternFileCount, &patternFilePath, &patternFileNames, true);

    // set up the 2D array for the particular image file
    int imageHeight;
    int imageWidth;
    // holder for the strings of the first line of an array (for dimensions)
    char firstLine[patternFileCount];
    // gets the size of the 'image' & put the contents in a 2D array
    textFileDimensions(firstLine, argv[1], &imageHeight, &imageWidth);
    char imageArray[imageHeight][imageWidth];
    fillArray(argv[1], imageHeight, imageWidth, imageArray);

    // array with the # of pattern matches and the coordinates
    int searchResults[patternFileCount];

	for(int patternFileId = 0; patternFileId < patternFileCount; patternFileId++) {
        int patternHeight;
        int patternWidth;
        // gets the size of the 'pattern' & put the contents in a 2D array
        textFileDimensions(firstLine, patternFilePath[patternFileId], &patternHeight, &patternWidth);
        char patternArray[patternHeight][patternWidth];
        fillArray(patternFilePath[patternFileId], patternHeight, patternWidth, patternArray);
        int patternMatchesArraySize = hasPattern(imageHeight, 
            imageWidth, patternHeight, patternWidth, imageArray, patternArray, searchResults);
        // write to our output file
        char buf[sizeof(getpid())+15];
        snprintf(buf, sizeof buf, "%s%d%s", "P_", getpid(), "_output.txt");
        writeToFile(patternMatchesArraySize, searchResults, buf);
	}

}

/*
 *------------------------------------------------------------------
 * Takes our pattern array and writes it to a file with each number on a newline
 *------------------------------------------------------------------
 */
void writeToFile(int arraySize, int *searchResults, char *filePath) {
    FILE *fptr;
    fptr = fopen(filePath, "a");
    // write each number on a new line
    for(int i = 0; i < arraySize; i++) {
        fprintf(fptr, "%d\t", searchResults[i]);
    }
    fprintf(fptr, "\n");
    fclose(fptr);
}


/*
 *------------------------------------------------------------------
 * Checks if the pattern grid exists in the image.
 *.................................................................. 
 * Constructs an array with all the matches and their coordinates
 *  on the image file and returns its size for iteration.
 *------------------------------------------------------------------
 */
int hasPattern(int imageHeight, int imageWidth, int patternHeight, int patternWidth, 
                char imageArray[imageHeight][imageWidth], char patternArray[patternHeight][patternWidth], int *searchResults) {
    bool exists = false;
    int counter = 0;
    int numMatches = 0;
    // the starting coordinates of the match
    int matchH = 0;
    int matchW = 0;
    // the area of the pattern
    int size = patternHeight * patternWidth;
    int indexPosition = 1;
    // we only look at postions where an entire pattern can fit
    int testingHeight = imageHeight - patternHeight;
    int testingWidth = imageWidth - patternWidth;
    //---------------------------------------------
    // iterate both arrays and check if the pattern is in it
    //---------------------------------------------
    for(int imageH = 0; imageH < testingHeight; imageH++) {
        for(int imageW = 0; imageW < testingWidth; imageW++) {
            for(int patternH = 0; patternH < patternHeight; patternH++) {
                for(int patternW = 0; patternW < patternWidth; patternW++) {
                    // a match is not found so we exit the loop and resume with the next element
                    if(patternArray[patternH][patternW] != imageArray[imageH+patternH][imageW+patternW]) {
                        counter = 0;
                        goto NextTest;
                    // we have found a match so we check if it continues for the duration of the pattern
                    } else {
                        // reached end of the pattern array, we have a full match so we record starting coordinates
                        if(counter == size-1) {
                            numMatches++;
                            searchResults[0] = numMatches;
                            matchH = imageH + patternH - (patternHeight - 1);
                            matchW = imageW + patternW - (patternWidth - 1);
                            searchResults[indexPosition++] = matchH;
                            searchResults[indexPosition++] = matchW;
                        }
                        counter++;
                    }
                }
                exists = true;
                NextTest: continue;
            }
        }
    }
    // we have no matches to report so we save 0
    if(exists == false) {
        searchResults[0] = 0;
    }
    return indexPosition;
}


/*
 *------------------------------------------------------------------
 * Fill a 2d array with the contents of the text file
 *------------------------------------------------------------------
 */
void fillArray(char *fileName, int height, int width, char myArray[height][width]) {
    FILE *fptr;
    char firstLine[width];
    // open the file and skip the first line (b/c it has the dimensions)
    fptr = fopen(fileName, "r");
    fgets(firstLine, sizeof(firstLine)*10, fptr);
    // iterate through the file and put the contents in the 2 dimensional array
    for(int i = 0; i < height; i++) {
        for(int j=0; j < width; j++) {
            if (fscanf(fptr,"%c ", &myArray[i][j]) != 1) {
                return;
            }
        }
    }
    fclose(fptr);
}


/*
 *------------------------------------------------------------------
 * Reads the top part of the file (where the sizes are located).
 *..................................................................
 * The numbers may be more than 1 digit long so loop through them
 *  until the first whitespace and store the first as height & second as width.
 *------------------------------------------------------------------
 */
void textFileDimensions(char *firstLine, char *fileName, int *fileHeight, int *fileWidth) {
    FILE *fptr;
    // sizes for the 2 dimensional array
    int width;
    int height;
    int counter = 1;
    bool exit = false;
    int secondNumStart = 0;
    // read the .txt file and store the contents into a 2d array
    fptr = fopen(fileName, "r");
    fgets(firstLine, sizeof(firstLine)*2, fptr);
    // getting the sizes of the arrays & converting them to ints
    char result[sizeof(fileName)*2];
    //------------------------------- 
    // get the width of the pattern
    //------------------------------- 
    while(exit != true) {
        strncpy(result, &firstLine[0], counter);
        sscanf(result, "%d", &width);
        if(firstLine[counter] == ' ') {
            exit = true;
        }
        counter++; 
    }
    exit = false;
    secondNumStart = counter;
    //------------------------------- 
    // get the height of the pattern
    //------------------------------- 
    while(exit != true) {
        strncpy(result, &firstLine[secondNumStart], counter);
        sscanf(result, "%d", &height);
        // we are done when we reach a new line
        if(firstLine[counter+1] == '\n') {
            exit = true;
        }
        counter++; 
    }
    *fileHeight = height;
    *fileWidth = width;
    fclose(fptr);
}


/*
 *------------------------------------------------------------------
 * Herve's modified function for checking the contents of a directory
 * (All credit goes to him)
 *..................................................................
 * Adds the directory path concatenated with the name of the file to an array
 *------------------------------------------------------------------
*/
void checkFiles(char *dataRootPath, int *numberOfFiles, char ***filePath, char ***patternFileNames, bool isPatterns) {
    DIR* directory = opendir(dataRootPath);
    if (directory == NULL) {
        printf("data folder %s not found\n", dataRootPath);
        exit(0);
    }
    
    struct dirent* entry;
    int counter = 0;
    
    //    First pass: c the entries
    while ((entry = readdir(directory)) != NULL) {
        char* name = entry->d_name;
        if (name[0] != '.') {
            counter++;
        }
    }
    closedir(directory);
    // assigns the number of files in the directory using a rerference 
    *numberOfFiles = counter;
    
    // Now allocate the array of file names
    *filePath = (char**) malloc(counter*sizeof(char*));
    if(isPatterns) {
        *patternFileNames = (char**) malloc(counter*sizeof(char*));
    }
    
    // Second pass: read the file names
    int k=0;
    directory = opendir(dataRootPath);
    while ((entry = readdir(directory)) != NULL) {
        char* name = entry->d_name;
        // Ignores "invisible" files (name starts with . char)
        if (name[0] != '.') {
            (*filePath)[k] = malloc((strlen(dataRootPath) + strlen(name)+2)*sizeof(char));
            strcpy((*filePath)[k], dataRootPath);
            // check if user path had / at the end
            int endsInSlash = (dataRootPath && *dataRootPath && dataRootPath[strlen(dataRootPath) - 1] == '/') ? true : false;
            if(!endsInSlash) {
                strcat((*filePath)[k], "/");
            }
            strcat((*filePath)[k], name);
            if(isPatterns) {
                (*patternFileNames)[k] = name;
            }
            k++;
        }
    }
    closedir(directory);
}
