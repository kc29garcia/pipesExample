#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLEN 10000
#define d0 "d0/"
#define d1  "d1/"
#define d2 "d2/"

//sendFileContents function opens the given file name and writes that
//data to the pipe for the child to read
void sendFileContents(int write_fd, const char *filename) {
    
	char path[MAXLEN];
	snprintf(path, MAXLEN, "%s%s", d0, filename);
	FILE *file = fopen(path, "r");
	
	if (!file) {
		perror("Error opening file\n");
		exit(1);
	}
	
	char buffer[MAXLEN];
	size_t bytes_read;
	while ((bytes_read = fread(buffer, 1, MAXLEN, file)) > 0) {
		write(write_fd, buffer, bytes_read);
	}
	fclose(file);
	
	// Send an end-of-file marker
	write(write_fd, "\0", 1);
	
	printf("Files sent to child\n");
}

//writeConents function reads from the pipe
//opens the file under the specified directory
//and copties the data from the pipe to the open file
void writeContents(int read_fd, const char *directory, const char *filename) {
    
	printf("COPYIING FILES TO NEW DIR\n");
	
	char path[MAXLEN];
	snprintf(path, MAXLEN, "%s%s", directory, filename);
	FILE *file = fopen(path, "w");
	if (!file) {
	perror("Error creating file\n");
	exit(1);
	}

	char buffer[MAXLEN];
	ssize_t bytes_read;
	while ((bytes_read = read(read_fd, buffer, MAXLEN)) > 0) {
		if (buffer[bytes_read - 1] == '\0') {
			fwrite(buffer, 1,(bytes_read - 1), file); // Write all except the EOF marker
			break;
		}
	fwrite(buffer, 1, bytes_read, file);
	}
	
	fclose(file);

	printf("File transfer complete\n");
}

int main() {
	
	int toChild1[2];
	int toChild_1[2];
	int toChild2[2];
	int toChild_2[2];

	//open pipes
	pipe(toChild1);
	pipe(toChild_1);
	pipe(toChild2);
	pipe(toChild_2);
	
	printf("toChild1: read: %d, write: %d\n", toChild1[0], toChild1[1]);
	printf("toChild2: read: %d, write: %d\n", toChild2[0], toChild2[1]);
	printf("toChild_1: read: %d, write: %d\n", toChild_1[0], toChild_1[1]);
	printf("toChild_2: read:%d, write: %d\n", toChild_2[0], toChild_2[1]);
	

	//
	if(fork()) { //first fork for first child
		
		if(fork()) { //second fork for second child
		
		//PARENT Process	
		
		const char *file1 = "f1.txt";
		const char *file2 = "f2.txt";
		const char *file3 = "f3.txt";
		const char *file4 = "f4.txt";

		//close unused pipes 
		close(toChild1[0]);
		close(toChild_1[0]);
		close(toChild2[0]);
		close(toChild_2[0]);
	
		//send file names to child 1
		write(toChild1[1], file1, strlen(file1) + 1);
		write(toChild_1[1], file2, strlen(file2) + 1);

		//send f1 and f2 and their contents to child 1
		sendFileContents(toChild1[1], file1);
		sendFileContents(toChild_1[1], file2);
		close(toChild1[1]);
		close(toChild_1[1]);
		
		//send file names to child 2
		write(toChild2[1], file3, strlen(file1) + 1);
		write(toChild_2[1], file4, strlen(file1) + 1);

		//send f3 and f4 and their contents to child 2.
		sendFileContents(toChild2[1], file3);
		sendFileContents(toChild_2[1], file4);
		close(toChild2[1]);
		close(toChild_2[1]);
		
		//wait for children to terminate
		wait(NULL);
		wait(NULL);

		} else{ //second child
			
			//read file names from pipe
			char filename3[MAXLEN], filename4[MAXLEN];
			read(toChild2[0], filename3, MAXLEN);
			read(toChild_2[0], filename4, MAXLEN);
			
			//create files f3 and f4
			char path[MAXLEN];
			snprintf(path, MAXLEN, "%s%s", d2, filename3);
			fclose(fopen(path, "w"));
			
			snprintf(path, MAXLEN, "%s%s", d2, filename4);
			fclose(fopen(path, "w"));
			
			//receive and write contents to newly created files f3 and f4 
			writeContents(toChild2[0], d2, filename3);
			writeContents(toChild_2[0], d2, filename4);
			
			close(toChild2[0]);
			close(toChild_2[0]);
		}
	} else { //first child process
		
		//read file names from the pipe
		char filename1[MAXLEN], filename2[MAXLEN];
		read(toChild1[0], filename1, MAXLEN);
		read(toChild_1[0], filename2, MAXLEN);
		
		//create files f1 and f2
		char path[MAXLEN];
		snprintf(path, MAXLEN, "%s%s", d1, filename1);
		fclose(fopen(path, "wb"));
		
		snprintf(path, MAXLEN, "%s%s", d1, filename2);
		fclose(fopen(path, "wb"));
		
		//receive and write contents to newly created files f1 and f2
		writeContents(toChild1[0], d1, filename1);
		writeContents(toChild_1[0], d1, filename2);
		
		close(toChild1[0]);
		close(toChild_1[0]);
	}
	return 0;
}
