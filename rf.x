/*
 * rf.x
 *	.x file for rpcgen
 *
 * This file rf.x is used by rpcgen to generate the client and server rpc stubs.
 * It defines the rpc program number, rpc version number and list of rpc procedures.
 *
 * For each procedure (function), the input and output structures are also defined.
 *
 * Author Prof. Krishnamoorthy
 *
 */

/*
 * Definition of all input and reply (output) structures used by rpc procedures
 */

struct RF_OpenFileRequest_T
{
	char	filename[81]; /* file pathname should be < 81 characters long */
	char    mode[2];      /* string dictating "r" for read or "w" for write */
};

struct RF_OpenFileReply_T
{
	long	openStatus;	/* 0 success, else failed */
	long	fd;		    /* file descriptor */
};

struct RF_ReadFileRequest_T
{
	long	fd;		        /* file descriptor */
	long	bytesToRead;	/* number of bytes to read should be < 64 */
				            /* since the buffer size is 64, see Reply below */
};

struct RF_ReadFileReply_T
{
	long	readStatus;	/* 0 success, else failed */
	long	bytesRead;	/* actual number of bytes read */
	char	buf[64];	/* array to hold characters read from file. */
				        /* Must read less than 64 bytes in read request */
};

struct RF_WriteFileRequest_T
{
	long    fd;				/* File descriptor */
	long	bytesToWrite;	/* number of bytes to write should be < 64 */
	char	buf[64];	    /* array to hold characters to write to file. */
};

struct RF_WriteFileReply_T
{
	long	writeStatus;	/* 0 success, else failed */
	long	bytesWritten;	/* actual number of bytes read */
};

struct RF_CloseFileRequest_T
{
	long	fd;		/* File descriptor */
};

struct RF_CloseFileReply_T
{
	long	closeStatus;
};

/*
 * RPC program number, version number and list of procedures (functions)
 */

program RFILE
{
   version RFILE_VERS
   {
	RF_OpenFileReply_T   rf_openfile  (RF_OpenFileRequest_T)  = 1;	/* procedure 1 */
	RF_ReadFileReply_T   rf_readfile  (RF_ReadFileRequest_T)  = 2;	/* procedure 2 */
	RF_CloseFileReply_T  rf_closefile (RF_CloseFileRequest_T) = 3;	/* procedure 3 */
	RF_WriteFileReply_T  rf_writefile (RF_WriteFileRequest_T) = 4;  /* procedure 4 */
   } = 1;  /* RCP server version number is 1 */
} = 877;     /* RPC server program number is 877 */
