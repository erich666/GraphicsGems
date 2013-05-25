/* Copyright (c) 1991 Regents of the University of California */

#ifndef lint
static char SCCSid[] = "@(#)header.c 1.4 4/22/91 LBL";
#endif

/*
 *  header.c - routines for reading and writing information headers.
 *
 *	8/19/88
 *
 *  printargs(ac,av,fp)	print an argument list to fp, followed by '\n'
 *  isformat(s)		returns true if s is of the form "FORMAT=*"
 *  formatval(r,s)	copy the format value in s to r
 *  fputformat(s,fp)	write "FORMAT=%s" to fp
 *  getheader(fp,f,p)	read header from fp, calling f(s,p) on each line
 *  checkheader(i,p,o)	check header format from i against p and copy to o
 *
 *  To copy header from input to output, use getheader(fin, fputs, fout)
 */

#include  <stdio.h>
#include  <ctype.h>

#define  MAXLINE	512

char  FMTSTR[] = "FORMAT=";
int  FMTSTRL = 7;


printargs(ac, av, fp)		/* print arguments to a file */
int  ac;
char  **av;
FILE  *fp;
{
	while (ac-- > 0) {
		fputs(*av++, fp);
		putc(' ', fp);
	}
	putc('\n', fp);
}


isformat(s)			/* is line a format line? */
char  *s;
{
	return(!strncmp(s,FMTSTR,FMTSTRL));
}


formatval(r, s)			/* return format value */
register char  *r;
register char  *s;
{
	s += FMTSTRL;
	while (isspace(*s)) s++;
	if (!*s) { *r = '\0'; return; }
	while(*s) *r++ = *s++;
	while (isspace(r[-1])) r--;
	*r = '\0';
}


fputformat(s, fp)		/* put out a format value */
char  *s;
FILE  *fp;
{
	fputs(FMTSTR, fp);
	fputs(s, fp);
	putc('\n', fp);
}


getheader(fp, f, p)		/* get header from file */
FILE  *fp;
int  (*f)();
char  *p;
{
	char  buf[MAXLINE];

	for ( ; ; ) {
		buf[MAXLINE-2] = '\n';
		if (fgets(buf, sizeof(buf), fp) == NULL)
			return(-1);
		if (buf[0] == '\n')
			return(0);
		if (buf[MAXLINE-2] != '\n') {
			ungetc(buf[MAXLINE-2], fp);	/* prevent false end */
			buf[MAXLINE-2] = '\0';
		}
		if (f != NULL)
			(*f)(buf, p);
	}
}


struct check {
	FILE	*fp;
	char	fs[64];
};


static
mycheck(s, cp)			/* check a header line for format info. */
char  *s;
register struct check  *cp;
{
	if (!strncmp(s,FMTSTR,FMTSTRL))
		formatval(cp->fs, s);
	else if (cp->fp != NULL)	/* don't copy format info. */
		fputs(s, cp->fp);
}


/*
 * Copymatch(pat,str) checks pat for wildcards, and
 * copies str into pat if there is a match (returning true).
 */

#ifdef COPYMATCH
copymatch(pat, str)
char	*pat, *str;
{
	int	docopy = 0;
	register char	*p = pat, *s = str;

	do {
		switch (*p) {
		case '?':			/* match any character */
			if (!*s++)
				return(0);
			docopy++;
			break;
		case '*':			/* match any string */
			while (p[1] == '*') p++;
			do
				if ( (p[1]=='?' || p[1]==*s)
						&& copymatch(p+1,s) ) {
					strcpy(pat, str);
					return(1);
				}
			while (*s++);
			return(0);
		case '\\':			/* literal next */
			p++;
		/* fall through */
		default:			/* normal character */
			if (*p != *s)
				return(0);
			s++;
			break;
		}
	} while (*p++);
	if (docopy)
		strcpy(pat, str);
	return(1);
}
#else
#define copymatch(pat, s)	(!strcmp(pat, s))
#endif


/*
 * Checkheader(fin,fmt,fout) returns a value of 1 if the input format
 * matches the specification in fmt, 0 if no input format was found,
 * and -1 if the input format does not match or there is an
 * error reading the header.  If fmt is empty, then -1 is returned
 * if any input format is found (or there is an error), and 0 otherwise.
 * If fmt contains any '*' or '?' characters, then checkheader
 * does wildcard expansion and copies a matching result into fmt.
 * Be sure that fmt is big enough to hold the match in such cases!
 * The input header (minus any format lines) is copied to fout
 * if fout is not NULL.
 */

checkheader(fin, fmt, fout)
FILE  *fin;
char  *fmt;
FILE  *fout;
{
	struct check	cdat;

	cdat.fp = fout;
	cdat.fs[0] = '\0';
	if (getheader(fin, mycheck, &cdat) < 0)
		return(-1);
	if (cdat.fs[0] != '\0')
		return(copymatch(fmt, cdat.fs) ? 1 : -1);
	return(0);
}
