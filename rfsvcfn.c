/* rfsvcfn.c */

/* This program defines functions that a RF_CLIENT may call to perform
// RPC server procedures. These "remote" function calls include open, read,
// write, and close file procedures.
// The input and return value structures are defined by rf.x and are part of
// the RPCGEN specification.
*/

#include <stdio.h>
#include <rpc/rpc.h>

#include "rf.h"

#define OKAY 0
#define FAILED -1

FILE *filedesc[1]; /* can open up to one file at a time. */
long fd = 0; /* index into filedesc (only one file operation permitted at a time) */


/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25,0 };


/*
 * RPC procedure to open server file
 * NOTE: This file is local to the server and hence fopen is used.
 */

// *****************************************************
//
// rf_openfile_1
//     Used to open a file based on parameters set by a given RF_OpenFileRequest_T.
// input parameters: openArg - The RF_OpenFileRequest_T who's members have been populated by a RF_CLIENT.
//	                 rqstp   - The RF_CLIENT that made the request.
// return value: A RF_OpenFileReply_T who's members reflect the outcome of the rf_openfile_1 procedure.
//
// *****************************************************
RF_OpenFileReply_T *rf_openfile_1(RF_OpenFileRequest_T *openArg, struct svc_req *rqstp)
{
   static RF_OpenFileReply_T res;

   printf("RF Server: Filename to open %s\n", openArg->filename);

   /* Open the specified file. Set openStatus to 0 if successful, -1 otherwise. */
   filedesc[fd] = fopen(openArg->filename, openArg->mode);
   if (filedesc[fd] != NULL)
      res.openStatus = OKAY;
   else
      res.openStatus = FAILED;

   res.fd = fd; /* Set fd to 0 */

   return(&res);
}

/*
 * RPC procedure to read from the server file
 * NOTE: This file is local to the server and hence fread is used.
 */

// *****************************************************
//
// rf_readfile_1
//     Used to read a file based on the parameters set by a given RF_ReadFileRequest_T.
//     Prints bytes read or error message depending on success or failure.
// input parameters: readArg - The RF_ReadFileRequest_T who's members have been populated by a RF_CLIENT.
//	                 rqstp   - The RF_CLIENT that made the request.
// return value: A RF_ReadFileReply_T who's members reflect the outcome of the rf_readfile_1 procedure.
//
// *****************************************************
RF_ReadFileReply_T *rf_readfile_1(RF_ReadFileRequest_T *readArg, struct svc_req *rqstp)
{
	static RF_ReadFileReply_T res;

	printf("RF Server: Recieved file read request.\n");

	/* Read the file specified by fd into buf. Set readStatus to 0 if successful, -1 otherwise. */
	res.bytesRead = fread(res.buf, 1, readArg->bytesToRead, filedesc[readArg->fd]);

	if (res.bytesRead >= 0){
		res.readStatus = OKAY;
		printf("Read file OKAY. Printing all %d bytes read...\n\n", res.bytesRead);
		for(int i=0; i < res.bytesRead; i++){
			printf("%c", res.buf[i]);
		}
		printf("\n");
		printf("End of bytes read.\n");
	}
	else{
		res.readStatus = FAILED;
		printf("Failed to read file. FD: %d\n", readArg->fd);
	}
	
	printf("Returning RF_ReadFileReply_T struct to calling client.\n");

	return(&res);
}

/*
 * RPC procedure to write to the server file.
 * NOTE: This file is local to the server, hence fwrite is used.
 */
 
// *****************************************************
//
// rf_writefile_1
//     Used to write to a file based on the parameters set by a given RF_WriteFileRequest_T.
// input parameters: writeArg - The RF_WriteFileRequest_T who's members have been populated by a RF_CLIENT.
//	                 rqstp    - The RF_CLIENT that made the request.
// return value: A RF_WriteFileReply_T who's members reflect the outcome of the rf_writefile_1 procedure.
//
// *****************************************************

 RF_WriteFileReply_T *rf_writefile_1(RF_WriteFileRequest_T *writeArg, struct svc_req *rqstp){
 
	static RF_WriteFileReply_T res; /* The struct to return */
	
	/* Write the bytes from buf to the file specified by fd. */
	res.bytesWritten = fwrite(writeArg->buf, 1, writeArg->bytesToWrite, filedesc[writeArg->fd]);
	
	/* If any bytes were written, set writeStatus to 0. Otherwise set writeStatus to -1. */
	if(res.bytesWritten > 0){
        res.writeStatus = OKAY;
		printf("Write file OKAY. Printing all %d bytes written...\n\n", writeArg->bytesToWrite);
		for(int i=0; i < writeArg->bytesToWrite; i++){
			printf("%c", writeArg->buf[i]);
		}
		printf("\n");
		printf("End of bytes written.\n");
    }else{
        res.writeStatus = FAILED;
		printf("Failed to write to file. FD: %d\n", writeArg->fd);
	}
	
	printf("Returning RF_WriteFileReply_T struct to calling client.\n");
	
	return(&res);
 }


/*
 * RPC procedure to close server file
 * NOTE: This file is local to the server and hence fclose is used.
 */

// *****************************************************
//
// rf_closefile_1
//     Used to close file based on the parameters set by a given RF_WriteFileRequest_T.
// input parameters: closeArg - The RF_CloseFileRequest_T who's members have been populated by a RF_CLIENT.
//	                 rqstp    - The RF_CLIENT that made the request.
// return value: A RF_CloseFileReply_T who's members reflect the outcome of the rf_closefile_1 procedure.
//
// *****************************************************
RF_CloseFileReply_T *rf_closefile_1(RF_CloseFileRequest_T *closeArg, struct svc_req *rqstp)
{
   static RF_CloseFileReply_T res;

   printf("RF Server: Recieved close file request.\n");

   res.closeStatus = fclose(filedesc[closeArg->fd]); /* Close the file specified by fd */

   return(&res);
}
