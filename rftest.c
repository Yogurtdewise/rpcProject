/* rftest.c
// Client main program to test RF server.
//
// Run this program with UNIX server IP address as the argument like
//       rfclient  RedactedIPAddress
//
// See main for program description.
//
*/

#include <stdio.h>
#include <rpc/rpc.h>

#include "rf.h"

#define RF_PROGRAM 877
#define RF_VERSION 1

#define OKAY 0
#define FAILED -1


// *****************************************************
//
// main
//     Entry point for the client program.
//     Completes the following procedures, failure terminates the program with error message.
//
//         1.)  Establish RPC_Client.
//         2.)  Get remote file name to retrieve from server, from user.
//         3.)  Get name of local file to store remote file as, from user.
//         4.)  Open local file.
//         5.)  Open remote file. If failed, offer user to choose a different file name.
//         6.)  Read remote file. Print read buffer if successful. Goto 8 if failed.
//         7.)  Write to local file. (repeat 6 & 7 as necessary)
//         8.)  Close local and remote files.
//         9.)  Get local file name from user to send to server.
//         10.) Get remote file name from user to store local file as.
//         11.) Open local file. If failed, offer user to choose a different file name.
//         12.) Open remote file.
//         13.) Read local file. Print read buffer.
//         14.) Write to remote file. (repeat 13 & 14 as necessary)
//         15.) Close local and remote files.
//         16.) Offer user the opportunity to repeat the rfclient procedure. Goto 2 if 'y'; else goto 17.
//         17.) Close the RPC Connection and exit(0).
//
// input parameters: A valid IP address String must be supplied as an argument.
// return value: Exit status. This integer is 0 if successful, or negative otherwise.
//
// *****************************************************
main (int argc, char *argv[]) {
   CLIENT *rf_clnt;
   char *server;    /* Server IP address */
   long status;     /* Used to store a process' return value. Valid values are OKAY or FAILED. */
   long fd;         /* File descriptor (index) of server's file pointer array */
   char line_stdin[1024];      /* Used to store user input String */
   char remote_filename[1024]; /* Used to store the filename of the remote file. */
   char local_filename[1024];  /* Used to store the filename of the local file. */
   char deleteFileCmd[1027];   /* Used to remove an invalid local file after a failed attempt. */
   char continueFlag = 'n';    /* Used to determine if the user wants to send/receive another file set */

   
   FILE *filePtr;      //Used to store the local file pointer.
   long bytesWritten;  //The number of bytes written to a file.
   long bytesRead;     //The number of bytes read from a file.

   /* rf.x structures */
   RF_OpenFileRequest_T openReq;
   RF_OpenFileReply_T *openReply;
   RF_ReadFileRequest_T readReq;
   RF_ReadFileReply_T *readReply;
   RF_CloseFileRequest_T closeReq;
   RF_CloseFileReply_T *closeReply;
   RF_WriteFileRequest_T writeReq;
   RF_WriteFileReply_T *writeReply;

   if (argc != 2) {
      printf("Usage: %s server-IP Address\n", argv[0]);
      exit(-1);
   }

   server = argv[1]; /* get server name. */

   // Create client handle.
   //

   printf("Calling RF clnt_create()\n");
   /* Get a RPC CLIENT. Terminate if unsuccessful */
   if ((rf_clnt = clnt_create(server, RF_PROGRAM, RF_VERSION, "udp")) == NULL) {
      clnt_pcreateerror(server);
      exit(-2);
   }


//
// For the project modify code below to do two things:
// (1) to copy a file from the server and store it locally on the client (your system)
// (2) to copy a local file from your system and store it on the server
// Read ONLY 64 bytes at a time
// Test your program with two files of size 200 to 300 bytes/character.
//
// To produce output, you must print the content of file sent from client after read
// as well as content of file received through rpc procedure call.
//

/* Main loop. Get one remote file and send one local file at a time. */
do{
	continueFlag = 'n';
	
	/* Get loop. Get one remote file. If failed, offer user to try again. */
	do{
		continueFlag = 'n';
		line_stdin[0] = NULL;
	
		//Get remote filename from user.
		do{
			printf("Please enter a file name to get from server: ");
			gets(line_stdin);
			if(line_stdin[0] == NULL){
				printf("Invalid file name.\n");
			}
		}while(line_stdin[0] == NULL);
	
		strcpy(remote_filename, line_stdin);
	
		line_stdin[0] = NULL;
	
		//Get local filename from user.
		do{
			printf("Enter the file name to store %s as: ", remote_filename);
			gets(line_stdin);
			if(line_stdin[0] == NULL){
				printf("Invalid file name.\n");
			}
		}while(line_stdin[0] == NULL);
	
		strcpy(local_filename, line_stdin);
	
		printf(" Copy remote server file %s ", remote_filename);
		printf("to local client file %s.\n\n", local_filename);

		filePtr = fopen(local_filename, "w"); /* Open the local file to be written to */
		if(filePtr == NULL){
			printf("ERROR opening local file %s.\n", local_filename);
			clnt_perror(rf_clnt, server);
			exit(-1);
		}

		//Get a RF_OpenFileReply_T by sending server a RF_OpenFileRequest_T.
		printf("Trying rf_openfile_1() on remote file: %s\n", remote_filename);
		strcpy(openReq.filename, remote_filename);
		strcpy(openReq.mode, "r");
		openReply = rf_openfile_1(&openReq, rf_clnt);
		/* Ensure something was returned */
		if (openReply == NULL) {
			printf("rf_openfile_1 failed (returned NULL).\n");
			clnt_perror(rf_clnt, server);
			exit(-1);
		} else {
		/* Check openStatus. If fopen was successful print fd. Otherwise terminate. */
			status = openReply->openStatus;
			if (status == OKAY) {
				fd = openReply->fd;
				printf("RF open is sucessful. FD: %d\n", fd);
			} else {
				printf("ERROR RF open status = %d\n", status);
				do{
					line_stdin[0] = NULL;
					printf("Remote file could not be opened. Try again (y/n)? ");
					gets(line_stdin);
					if(line_stdin[0] == NULL){
						printf("\nInvalid response. Enter \'y\' for yes or \'n\' for no.\n");
					}
				}while(strcmp(line_stdin, "y") != 0 && strcmp(line_stdin, "Y") != 0 && strcmp(line_stdin, "n") != 0 && strcmp(line_stdin, "N") != 0);
				continueFlag = line_stdin[0];
				/* Clean up local file. */
				fclose(filePtr);
				strcpy(deleteFileCmd, "rm ");
				strcat(deleteFileCmd, local_filename);
				system(deleteFileCmd);
			}
		}
	}while(continueFlag == 'y' || continueFlag == 'Y');

	continueFlag = 'n';

	/* Read then close remote file if openReply->openStatus was OKAY. Otherwise,
	// remote file open failed, and the user opted not to continue; therefore skip
	// read and close attempts.
	*/
	if(status == OKAY){
		// Read from remote server file 64 bytes at a time until end of file or error.
		printf("Trying RF read file.\n");

		/* Initialize readReq */
		readReq.fd = fd;
		readReq.bytesToRead = 64;

		// There are four ways to exit this loop. 
		// (1) readReply is NULL, which means the rpc procedure call failed due to network error or rpc server not running
		// (2) rpc call is successful, however, read operation failed with bytesread is negative
		// (3) end of file has reached, bytesread is zero
		// (4) fwrite to local file failed
	   
		while ((readReply = rf_readfile_1(&readReq, rf_clnt)) != NULL) 
		{
			printf("rpc call is successful. Checking rpc return status from the server\n");
			status = readReply->readStatus;
			if (status == OKAY) 
			{
				printf("Successful read from the server. Checking for end of file or not\n");
				if (readReply->bytesRead > 0) {
					printf("RF read file successful with bytesRead = %d.\n\n", readReply->bytesRead);

					// Print each character that is read. Finish with a new line.
					for(int i=0;i<readReply->bytesRead;i++)
					{
						printf("%c",readReply->buf[i]);
					}
					printf("\n");
					
					bytesWritten = 0; /* Reset */
					bytesWritten = fwrite(readReply->buf, 1, readReply->bytesRead, filePtr); /* Write to get_local_file */
					/* If fwrite failed, exit the loop.*/
					if(bytesWritten <= 0){
						printf("ERROR: Write to local file failed. bytesWritten = %d.\n", bytesWritten);
						break;
					}
					printf("Successful write to local file. bytesWritten = %d.\n", bytesWritten);
					if(feof(filePtr)){
						printf("Reached EOF\n");
						break;
					}

				}
				/* Reached EOF. Leave the loop. */
				else if(readReply->bytesRead == 0){
					printf("No bytes to read from remote server.\n");
					break;
				}
				else
				{
					printf("ERROR Bytes read is negative, bytesRead = %d\n", readReply->bytesRead);
					break;
				}
			} 
			else 
			{
				printf("ERROR rpc server procedure returned error. RF status = %d\n", status);
				break;
			}
		}

		// This is the error handling for the readReply is NULL. 
		if (readReply == NULL) {
			printf("ERROR RPC: RF procedure call failed due to network error or rpc server not running\n");
			clnt_perror(rf_clnt,server);
		}


		// Close local client file. Reset filePtr for next operation.
		printf("Closing local file.\n");
		fclose(filePtr);
		filePtr = NULL;

		// close the server file
		printf("Calling RF close file.\n");

		closeReq.fd = fd;
		closeReply  = rf_closefile_1(&closeReq, rf_clnt); /* Get RF_CloseFileReply_T to determine closeStatus. */

		if (closeReply == NULL) 
		{
			printf("RF close file failed.\n");
			clnt_perror(rf_clnt, server);
		}
		else 
		{
			status = closeReply->closeStatus;
			if (status == OKAY) 
			{
				printf("RF close file successful.\n");
				fd = NULL;
			}
			else 
			{
				printf("ERROR RF status = %d\n", status);
			}
		}
	}

	printf("\n");
   
	/* send loop. Send one local file to server. If failed, offer user to try again. */
	do{
		continueFlag = 'n';
		line_stdin[0] = NULL;
	
		//Get local filename from user.
		do{
			printf("Please enter a file name to send to server: ");
			gets(line_stdin);
			if(line_stdin[0] == NULL){
				printf("Invalid file name.\n");
			}
		}while(line_stdin[0] == NULL);
	
		strcpy(local_filename, line_stdin);
	
		line_stdin[0] = NULL;
	
		//Get remote filename from user.
		do{
			printf("Enter the file name to store %s as: ", local_filename);
			gets(line_stdin);
			if(line_stdin[0] == NULL){
				printf("Invalid file name.\n");
			}
		}while(line_stdin[0] == NULL);
	
		strcpy(remote_filename, line_stdin);
   
		printf("Copy local client file %s ", local_filename);
		printf("to remote server file %s.\n\n", remote_filename);
	
		printf("Opening local file \"%s\".\n", local_filename);
	
		filePtr = NULL;
		filePtr = fopen(local_filename, "r"); /* Open the local file to be read from */
	
		/* Check if local file is opened. Offer user the opportunity to try again if open failed. */
		if(filePtr == NULL){
			do{
				line_stdin[0] = NULL;
				printf("Could not open local file. Try again (y/n)? ");
				gets(line_stdin);
				if(line_stdin[0] == NULL){
					printf("\nInvalid response. Enter \'y\' for yes or \'n\' for no.\n");
				}
			}while(strcmp(line_stdin, "y") != 0 && strcmp(line_stdin, "Y") != 0 && strcmp(line_stdin, "n") != 0 && strcmp(line_stdin, "N") != 0);
			continueFlag = line_stdin[0];
			if(continueFlag == 'n' || continueFlag == 'N'){
				printf("Can not open local file. User chose not to continue. Exiting program.\n");
				clnt_perror(rf_clnt, server);
				exit(-1);
			}
		}
	}while(continueFlag == 'y' || continueFlag == 'Y');
	
	continueFlag = 'n';
	
	/* Open, write, then close remote file if local file was opened successfully. Otherwise,
	// open local file in read mode failed, and the user opted not to continue; therefore skip
	// open, write, and close attempts on remote file.
	*/
	if(filePtr != NULL){
		// call open the remote server file using rpc open file procedure (function/method)
		printf("Trying rf_openfile_1()\n");
		strcpy(openReq.filename, remote_filename);
		strcpy(openReq.mode, "w");  // set mode for writing to file.
		openReply = rf_openfile_1(&openReq, rf_clnt); /* Get the RF_OpenFileReply_T struct */
		
		/* Ensure remote file was opened. */
		if(openReply == NULL) {
			printf("rf_openfile_1 failed.\n");
			clnt_perror(rf_clnt, server);
			exit(-1);
		}
		
		/* Check openStatus. If fopen was successful print fd. Otherwise terminate. */
		status = openReply->openStatus;
		if (status == OKAY) {
			fd = openReply->fd;
			printf("RF open is successful. FD: %d\n", fd);
		}else{
			printf("ERROR RF open status = %d\n", status);
			exit(-1);
		}

		/* Read from local file and write to remote file until local file reaches EOF. */
		do{
		
			printf("Reading local file.\n");
			bytesRead = fread(writeReq.buf, 1, 64, filePtr); // Write 64 bytes from local file to RF_WriteFileRequest_T struct.
			/* Ensure there were no read errors; if so, quit. */
			if(bytesRead < 0){
				printf("ERROR reading local file. bytesRead: %d\n", bytesRead);
				clnt_perror(rf_clnt, server);
				exit(-1);
			}
			writeReq.fd = fd; //Supply the file descriptor for RF_WriteFileRequest_T.
			writeReq.bytesToWrite = bytesRead; //Specify the number of bytes to be written in RF_WriteFileRequest_T.
			
			printf("The following bytes were read from the local file\n\n");
			
			/* Print the bytes entered int writeReq.buf */
			for(int i=0; i < bytesRead; i++){
				printf("%c",writeReq.buf[i]);
			}
			printf("\n");
			printf("*** End of bytes read. ***\n");
			printf("\n");
			
			/* Write to remote file. */
			printf("Trying rf_writefile_1.\n");
			writeReply = rf_writefile_1(&writeReq, rf_clnt); //Get the RF_WriteFileReply_T struct from rf_writefile_1.
			
			/* Ensure the remote file was written to successfully; if not, quit. */
			status = writeReply->writeStatus;
			if(status != OKAY){
				printf("ERROR RF write status = %d\n", status);
				clnt_perror(rf_clnt, server);
				exit(-1);
			}
			
			bytesWritten = writeReply->bytesWritten;
			printf("RF write is successful. bytesWritten: %d\n", bytesWritten);
		
		}while(!feof(filePtr)); /* End send loop */
		
		printf("Local file reached EOF.\n");
		
		// close the server file
		printf("Calling RF close file.\n");
		closeReq.fd = fd;
		closeReply = rf_closefile_1(&closeReq, rf_clnt); /* Get RF_CloseFileReply_T to determine closeStatus. */

		if(closeReply == NULL) 
		{
			printf("RF close file failed.\n");
			clnt_perror(rf_clnt, server);
		} 
		else 
		{
			status = closeReply->closeStatus;
			if (status == OKAY) 
			{
				printf("RF close file successful.\n");
				fd = NULL;
			} 
			else 
			{
				printf("ERROR RF status = %d\n", status);
			}
		}
		
		printf("Closing local file.\n");
		
		fclose(filePtr);
		filePtr = NULL;
	}

	printf("\n");
	
	/*Offer user the chance to repeat the rfclient procedure. */
	do{
		line_stdin[0] = NULL;
		printf("rfclient procedure complete. get/send another set of files (y/n)? ");
		gets(line_stdin);
		if(line_stdin[0] == NULL){
			printf("\nInvalid response. Enter \'y\' for yes or \'n\' for no.\n");
		}
	}while(strcmp(line_stdin, "y") != 0 && strcmp(line_stdin, "Y") != 0 && strcmp(line_stdin, "n") != 0 && strcmp(line_stdin, "N") != 0);
	
	continueFlag = line_stdin[0];
	
}while(continueFlag == 'y' || continueFlag == 'Y');
	
	// Close rpc transport connectioj with the server

	clnt_destroy(rf_clnt);

	// Exit main program.
	printf("Exiting main test program.\n\n");

	exit(0);
}

