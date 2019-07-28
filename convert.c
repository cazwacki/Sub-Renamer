#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
// First argument is a folder with the videos. Second argument is the folder with the subs.
// They MUST be in order ex. S1E1 S1E2 or it will incorrectly copy the files. It is NONRECURSIVE.

bool isVideoFile(char* fileName);
bool isSubtitleFile(char* filename);
static int myCompare(const void* a, const void* b);
void sort(char* arr[], int n);
void closeProgram(char** vidPaths, int vidPathsSize, char** subPaths, int subPathsSize, DIR* vidFolderPath, DIR* subFolderPath);

int main(int argc, char* argv[]) {
	
	struct dirent* dirFile; // pointer for navigating through directory

	if(argc != 3) {
		printf("Usage: convert <video folder path> <subtitle file path>\n");
	} else {
		// Opening folders here. If any errors occur, the program stops with relevant complaint.		

		DIR* vidFolderPath = opendir(argv[1]); // path of video folder
		if(vidFolderPath == NULL) {
			printf("Failed to open video folder path. Exiting.\n");
			return -1;
		}
		DIR* subFolderPath = opendir(argv[2]); // path to subtitle folder
		if(subFolderPath == NULL) {
			printf("Failed to open subtitle folder path. Exiting.\n");
			return -1;
		}
		printf("Video Folder Path: %s\nSubtitle Folder Path: %s\n", argv[1], argv[2]);
		
		int vidPathsSize = 1;
		char** vidPaths = malloc(sizeof(char*));

		printf("Looking for Videos...\n");
		while((dirFile = readdir(vidFolderPath)) != NULL) { // read the video folder contents
			if(isVideoFile(dirFile->d_name)) {
				// add a new video file path to the video paths
				char absolutePath[4096];
				realpath(argv[1], absolutePath);
/*				if(temp != NULL) {
*					free(temp);
				}*/
				strcat(absolutePath, "/");
				strcat(absolutePath, dirFile->d_name);
				vidPaths[vidPathsSize-1] = malloc(strlen(absolutePath)+1);
				strcpy(vidPaths[vidPathsSize-1], absolutePath);
				printf("vidPath: %s\n", vidPaths[vidPathsSize-1]);
				vidPathsSize++;
				vidPaths = realloc(vidPaths, sizeof(char*)*vidPathsSize);
			}
		}

		int subPathsSize = 1;
		char** subPaths = malloc(sizeof(char*));

		printf("Looking for Subtitles...\n");
		while((dirFile = readdir(subFolderPath)) != NULL) { // read the subtitle folder contents
			if(isSubtitleFile(dirFile->d_name)) {
				// add a new subtitle file path to the subtitle paths
				char absolutePath[4096];
				realpath(argv[2], absolutePath);
				/*if(temp != NULL) {
					free(temp);
				}*/
				strcat(absolutePath, "/");
				strcat(absolutePath, dirFile->d_name);
				subPaths[subPathsSize-1] = malloc(strlen(absolutePath)+1);
				strcpy(subPaths[subPathsSize-1], absolutePath);
				printf("subPath: %s\n", subPaths[subPathsSize-1]);
				subPathsSize++;
				subPaths = realloc(subPaths, sizeof(char*)*subPathsSize);
			}
		}
		
		if(vidPathsSize != subPathsSize) {
			printf("Vid Count: %d\nSub Count: %d\n", vidPathsSize, subPathsSize);
			printf("These numbers should be equal. Make sure there are no copies or missing files. Exiting.");
			closeProgram(vidPaths, vidPathsSize, subPaths, subPathsSize, vidFolderPath, subFolderPath);
			return -1;
		}
		printf("Video and subtitle counts are equal!\n");

		printf("Sorting...\n");
		
		sort(vidPaths, vidPathsSize - 1);
		sort(subPaths, subPathsSize - 1);
		
		printf("Current Assignment:______________\n");
		for(int i = 0; i < subPathsSize - 1; i++) {
			printf("Vid: %s\nSub: %s\n_______________\n", vidPaths[i], subPaths[i]);
		}
		
		char* proceedCheck = malloc(100); // in case somebody really bothers to mash that many characters in before hitting enter... :/
		printf("If this is correct assocation, type 'y' to proceed. Type anything else to exit: ");
		scanf("%s", proceedCheck);
		if(strcmp(proceedCheck, "y") != 0) {
			printf("Exiting.\n");
			free(proceedCheck);
			closeProgram(vidPaths, vidPathsSize, subPaths, subPathsSize, vidFolderPath, subFolderPath);	
			return -1;	
		}
		free(proceedCheck);
		
		//TODO: Open subtitle files in order, create new subtitle file with video file name in video file directory
		for(int i = 0; i < subPathsSize - 1; i++) {
			FILE* originalSub = fopen(subPaths[i], "r");
			if(originalSub == NULL) {
				perror("fopen");				
				printf("Cannot read subtitle file '%s'. Exiting.\n", subPaths[i]);
				closeProgram(vidPaths, vidPathsSize, subPaths, subPathsSize, vidFolderPath, subFolderPath);
				return 0;
			}
			char* newSubPath = malloc(strlen(vidPaths[i])+4);
			strcpy(newSubPath, vidPaths[i]);

			newSubPath[strlen(newSubPath)-4] = '\0';
			strcat(newSubPath, &subPaths[i][strlen(subPaths[i])-4]);
			printf("Writing subtitle to: %s\n", newSubPath);
			FILE* newSub = fopen(newSubPath, "w+");
			if(newSub == NULL) {
				perror("fopen");				
				printf("Cannot open subtitle file '%s'. Exiting.\n", subPaths[i]);
				closeProgram(vidPaths, vidPathsSize, subPaths, subPathsSize, vidFolderPath, subFolderPath);
				return 0;
			}
			char currentChar = 0;
			while((currentChar = fgetc(originalSub)) != EOF) { // creates exact copy of file in new subtitle file
				fputc(currentChar, newSub);
			}
			free(newSubPath);
			fclose(originalSub);
			fclose(newSub);
		}
		closeProgram(vidPaths, vidPathsSize, subPaths, subPathsSize, vidFolderPath, subFolderPath);
	}
}

void closeProgram(char** vidPaths, int vidPathsSize, char** subPaths, int subPathsSize, DIR* vidFolderPath, DIR* subFolderPath) {
	for(int i = 0; i < vidPathsSize - 1; i++) { // -1 because our final char* is uninitialized; no need to free
		free(vidPaths[i]);
	}
	free(vidPaths);

	for(int i = 0; i < subPathsSize - 1; i++) { // -1 because our final char* is uninitialized; no need to free
		free(subPaths[i]);
	}
	free(subPaths);
	printf("Complete!\n");
	closedir(vidFolderPath);
	closedir(subFolderPath);
}

/*
* Returns whether the file ends with one of the following:
* .mp4, .mkv, .avi
*/
bool isVideoFile(char* filename) {
	int strLength = strlen(filename);
	char* fileEnd = malloc(5);
	strncpy(fileEnd, filename + strLength - 4, 4);
	fileEnd[4] = '\0'; // this should result in the last four characters being captured
	//printf("temp: %s\n", fileEnd); DEBUG
	bool videoFile = false;
	if(strcmp(fileEnd, ".mp4") == 0 || strcmp(fileEnd, ".mkv") == 0 || strcmp(fileEnd, ".avi") == 0) {
		videoFile = true;
	}
	free(fileEnd);
	return videoFile;
}

/*
* Returns whether the file ends with one of the following:
* .sub, .srt
*/
bool isSubtitleFile(char* filename) {
	int strLength = strlen(filename);
	char* fileEnd = malloc(5);
	strncpy(fileEnd, filename + strLength - 4, 4);
	fileEnd[4] = '\0'; // this should result in the last four characters being captured
	//printf("temp: %s\n", fileEnd); DEBUG
	bool subFile = false;
	if(strcmp(fileEnd, ".srt") == 0 || strcmp(fileEnd, ".sub") == 0) {
		subFile = true;
	}
	free(fileEnd);
	return subFile;
}

// Defining comparator function as per the requirement for sort
static int myCompare(const void* a, const void* b) 
{ 
  
    // setting up rules for comparison 
    return strcmp(*(const char**)a, *(const char**)b); 
} 
  
// Function to sort the array 
void sort(char* arr[], int n) 
{ 
    // calling qsort function to sort the array 
    // with the help of Comparator 
    qsort(arr, n, sizeof(const char*), myCompare); 
} 
