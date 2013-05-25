/***************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    HEADER   - BUFFERED FILE INPUT WITH SYNTAX CHECK
**		AND NOT BUFFERED FILE OUTPUT
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/

#ifdef MSWINDOWS
#include "graph.hxx"
#else
#include "graph.hxx"
#endif

#define BUFFERSIZE	2048
#define MAXLINE		80


class FileIO {
	FILE *	file;			// file descriptor
	char	operation[4];		// operation for open
	int	line_count;		// number of inputed lines
	int	error;			// error code
	char	s[MAXLINE + 1];		// last string
	int	buffpt;			// buffer indes
	int	nbytes;			// number of valid bytes in buffer
	char	buffer[BUFFERSIZE];	// io buffer
protected:
	BOOL	Get( pchar );		    // get char and advance
	void	UnGet( void ) { buffpt--; } // unget last char
public:
		FileIO( char * op ) { strcpy( operation, op ); }
	BOOL	OpenFile ( pchar );	    // open file having that name
	BOOL	GetString ( pchar, int );   // get a string from the file
	BOOL	GetKeyWord ( pchar );	    // get a string and compare to a key
	BOOL	GetKeyAgain ( pchar );	    // get last string again and compare
	BOOL	GetVariable ( double *, double, double );	// get a double variable
	BOOL	GetOperator ( char );	    // get a char and compare to the given char
	int	GetLineNum ( void ) { return line_count;	}
	BOOL	PutString ( pchar );	    // write string to the file
	void	CloseFile ( void );	    // close file
};
