/****************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    MODUL   - BUFFERED FILE INPUT WITH SYNTAX CHECK
**		AND NOT BUFFERED FILE OUTPUT
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/

#ifdef MSWINDOWS
#include "fileio.hxx"
#else
#include "fileio.hxx"
#endif

/*----------------------   Get ---------------------------------*/
/* Buffered character input from the opened file		*/
/* IN  : character address					*/
/* OUT : char is put to the address				*/
/*	 ret = was it succesful					*/
/*--------------------------------------------------------------*/
BOOL FileIO :: Get( char * pc )
{
    if ( buffpt == nbytes ) {
	if ( (nbytes = fread( buffer, 1, BUFFERSIZE, file)) <= 0) return FALSE;
	else							  buffpt = 0;
    }
    *pc = buffer[ buffpt++ ];
    return TRUE;
}

/*------------------	 GetString  ----------------------------*/
/* Gets a string from a opened file. String ends are space, tab */
/* EOL(\n) and operators( =, : ). Parts between # end EOL are	*/
/* ignored ( comments )						*/
/* IN  : string buffer adress and maximal length		*/
/* OUT : was it succesful ?					*/
/*	 string is put to the address s				*/
/*--------------------------------------------------------------*/
BOOL FileIO :: GetString ( pchar s, int maxlength )
{
    int	 ichar = 0;
    char last_char;
    BOOL iscomment = FALSE;

    for ( ; ; ) {
/*
*    GET CHARACTER
*/
	if ( !Get ( &last_char ) )  return FALSE;

	switch ( last_char ) {
/*
*    COMMENT
*/
	case '#':
	    iscomment = TRUE;
	    break;
/*
*    OPERATORS
*/
	case ':':
	case '=':
	    if ( !iscomment ) {
		if (ichar > 0) UnGet();		    // unget operator
		else	       s[ichar++] = last_char;
		s[ichar] = '\0';
		return TRUE;
	    } else break;
/*
*    SEPARATORS
*/
	case '\n':
	    iscomment = FALSE;
	    line_count++;
	case '\t':
	case ' ':
	    if ( !iscomment ) {
		if ( ichar > 0 ) {
		    s[ichar] = '\0';
		    return TRUE;
		}
	    }
	    break;
/*
*    KEYWORDS AND VARIABLES
*/
	default:
	    if ( !iscomment ) {
		if ( ichar == maxlength ) {
		    return FALSE;
		} else {
		    s[ichar++] = last_char;
		    break;
		}
	    } else  break;
	}
    }
}

/*------------------	 OpenFile   ----------------------------*/
/* Opens a TEXT file for the defined operation			*/
/* IN  : file name						*/
/* OUT : was it succesful ?					*/
/* SIDE EFFECT: - internal file descriptor is created end line	*/
/*		  is initialized				*/
/*--------------------------------------------------------------*/
BOOL FileIO :: OpenFile( pchar name )
{
    line_count = 1;
    buffpt = 0;
    nbytes = 0;

    if ( (file = fopen( name, operation )) == NULL ) {
	return FALSE;
    }
    return TRUE;
}

/*------------------	 CloseFile   ---------------------------*/
/* Closes the opened file					*/
/*--------------------------------------------------------------*/
void FileIO :: CloseFile ( )
{
    fclose ( file );
}

/*------------------	 GetKeyWord  ---------------------------*/
/* Gets a string and compares with the pattern keyword		*/
/* IN  : pattern keyword					*/
/* OUT : is the specified pattern received ?			*/
/*--------------------------------------------------------------*/
BOOL FileIO :: GetKeyWord ( pchar key )
{
    if ( !GetString( s, MAXSTRING ) ) return FALSE;
    return GetKeyAgain( key );
}

/*------------------	 GetKeyAgain ---------------------------*/
/* Gets the last inspected string again and compares with the	*/
/* pattern keyword						*/
/* IN  : pattern keyword					*/
/* OUT : is the specified pattern received ?			*/
/*--------------------------------------------------------------*/
BOOL FileIO :: GetKeyAgain ( pchar key )
{
    if ( strcmp( key, s ) == 0 )      return TRUE;
    else {
	return FALSE;
    }
}

/*------------------	 GetVariable ---------------------------*/
/* Gets a double variable and compares to the specified range	*/
/* IN  : address of var and min, max of range			*/
/* OUT : was it succesful, and the received var			*/
/*--------------------------------------------------------------*/
BOOL FileIO :: GetVariable ( double *pv, double minv, double maxv )
{
    if ( !GetString(s,MAXLINE) ) return FALSE;

    if ( sscanf(s,"%lf", pv ) != 1 ) {
	return FALSE;
    } else {
	if ( *pv < minv || *pv > maxv ) {
	    return FALSE;
	}
	return TRUE;
    }
}

/*------------------	 GetOperator ---------------------------*/
/* Gets a non-white character and compares with the specified	*/
/* operator							*/
/* IN  : specified operator					*/
/* OUT : was it the specified operator				*/
/*--------------------------------------------------------------*/
BOOL FileIO :: GetOperator ( char op )
{
    if ( !GetString( s, 1 ) ) return FALSE;
    if ( s[0] != op ) {
	return FALSE;
    }
    return TRUE;
}

/*------------------	 PutString   ---------------------------*/
/* Writes the given string to the file without buffering	*/
/* IN  : specified string					*/
/* OUT : was it succesful					*/
/*--------------------------------------------------------------*/
BOOL FileIO :: PutString ( pchar s )
{
    if ( fwrite( s, 1, strlen(s), file ) == 0) return FALSE;
    else				       return TRUE;
}
