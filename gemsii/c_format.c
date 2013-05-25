/************************************************************************
  c_format -- a PostScript typesetting program for C source code (v0.6)

  Released into the public domain, November 1990 by Dale Schumacher
  email: <dal@syntel.mn.org>  mail: 399 Beacon Ave., St. Paul MN 55104
************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static char psPreamble[] = "\
%!PS-Adobe-2.0\n\
/inch { 72 mul } def\n\
/pica { 12 mul } def\n\
/xOrg 114 def\n\
/xSize 32 pica def\n\
/yOrg 684 def\n\
/ySize 44 pica def\n\
/yDelta 9 def\n\
/Page 1 def\n\
/Line 1 def\n\
/FS {	% find and scale a font\n\
	findfont exch scalefont\n\
} bind def\n\
/Fonts [	% create an array of prescaled fonts\n\
	8 /Courier FS\n\
	8 /Courier-Bold FS\n\
	8 /Courier-Oblique FS\n\
] def\n\
/SL {	% set line position\n\
	/Line exch def\n\
	xOrg yOrg Line yDelta mul sub moveto\n\
} bind def\n\
/NL {	% newline\n\
	Line 1 add SL\n\
} bind def\n\
/R {	% use roman font\n\
	Fonts 0 get setfont\n\
} bind def\n\
/B {	% use bold font\n\
	Fonts 1 get setfont\n\
} bind def\n\
/I {	% use italic font\n\
	Fonts 2 get setfont\n\
} bind def\n\
/S {	% show\n\
	show\n\
} bind def\n\
";

char psFinal[] = "\
/BP {	% begin page\n\
	1 SL R\n\
} bind def\n\
/EP {	% end page\n\
	gsave\n\
		newpath\n\
			xOrg xSize add 0 moveto\n\
			0 11 inch rlineto\n\
			8.5 inch 11 inch lineto\n\
			8.5 inch 0 lineto\n\
		closepath 1 setgray fill	% mask right margin\n\
	grestore\n\
	showpage\n\
} bind def\n\
";

char psDraft[] = "\
/DraftFont 72 /Helvetica-Bold FS def\n\
/BP {	% begin page\n\
	gsave\n\
		newpath\n\
		184 452 moveto\n\
		-30 rotate\n\
		DraftFont setfont\n\
		gsave \n\
			0.82 setgray (DRAFT) show\n\
		grestore \n\
		(DRAFT) false charpath 0 setlinewidth stroke\n\
	grestore\n\
	1 SL R\n\
} bind def\n\
/EP {	% end page\n\
	gsave\n\
		newpath\n\
			xOrg xSize add 0 moveto\n\
			0 11 inch rlineto\n\
			8.5 inch 11 inch lineto\n\
			8.5 inch 0 lineto\n\
		closepath 1 setgray fill	% mask right margin\n\
		0 setgray\n\
		newpath\n\
			xOrg yOrg moveto\n\
			xSize 0 rlineto\n\
			0 ySize neg rlineto\n\
			xSize neg 0 rlineto\n\
		closepath 0 setlinewidth stroke	% nominal text box\n\
	grestore\n\
	showpage\n\
} bind def\n\
";

#define	LPP	(60)		/* maximum allowable lines per page */

char		title[256] = "THE TITLE OF YOUR GEM";
char		author[256] = "YOUR NAME (as it appears on your gem)";
int		draft = 0;	/* draft output mode switch */
int		tabsize = 4;	/* horizontal tab spacing */
int		page = 1;	/* current page of listing (one based) */
int		line = 1;	/* current line on page (one based) */
int		firstline = 18;	/* first line of first page (one based) */

char *
ps_string(s)
char *s;
{
	static char ps_buf[512];
	char *p = ps_buf;
	int c;

	while(c = *s++) {
		if((c == '(') || (c == ')') || (c == '\\')) {
			*p++ = '\\';
		}
		if(isprint(c)) {
			*p++ = c;
		}
	}
	*p = '\0';
	return(ps_buf);
}

void
start_listing(f)
FILE *f;
{
	line = firstline;
	fprintf(f, "%s%s\n", psPreamble, (draft ? psDraft : psFinal));
	fprintf(f, "%% start of listing\n");
	fprintf(f, "initgraphics\nBP\n");
	fprintf(f, "(%s) S NL\n", ps_string(title));
	fprintf(f, "(%s) S\n", ps_string(author));
	fprintf(f, "%d SL\n", firstline);
}

void
page_eject(f)
FILE *f;
{
	++page;
	line = 1;
	fprintf(f, "EP\nBP\n");
}

void
new_line(f)
FILE *f;
{
	++line;
	if(line > LPP) {
		page_eject(f);
	} else {
		fprintf(f, "NL\n");
	}
}

void
end_listing(f)
FILE *f;
{
	fprintf(f, "EP\n\n%% end of listing\n");
}

#define	TK_EMPTY	(-1)
#define	TK_TEXT		(0)
#define	TK_WORD		(1)
#define	TK_KWORD	(2)
#define	TK_CSTART	(3)
#define	TK_CEMPTY	(4)
#define	TK_CTEXT	(5)
#define	TK_CEND		(6)
#define	TK_SEMPTY	(7)
#define	TK_STEXT	(8)
#define	TK_NEWLINE	(9)
#define	TK_FORMFEED	(10)

static char *keywords[] = {
	"auto",		"break",	"case",		"char",
	"const",	"continue",	"default",	"do",
	"double",	"else",		"entry",	"enum",
	"extern",	"float",	"for",		"goto",
	"if",		"int",		"long",		"register",
	"return",	"short",	"signed",	"sizeof",
	"static",	"struct",	"switch",	"typedef",
	"union",	"unsigned",	"void",		"volatile",
	"while",
	NULL
};

int
iskeyword(s)
char *s;
{
	char **p;

	for(p = keywords; *p; ++p) {
		if((**p == *s) && (strcmp((*p)+1, s+1) == 0)) {
			return(1);
		}
	}
	return(0);
}

static int spaces = 0;
static int pbchar = 0;
static int pbflag = 0;

int
push_back(f, c)
FILE *f;
int c;
{
	pbflag = 1;
	return(pbchar = c);
}

int
next_char(f)
FILE *f;
{
	static int col = 0;
	int c;

	if(pbflag) {
		pbflag = 0;
		return(pbchar);
	} else if(spaces > 0) {
		--spaces;
		++col;
		return(' ');
	}
	do {
		c = getc(f);
		if((c == EOF) || (c == '\f') || (c == '\n')) {
			if(c == '\n') {
				col = 0;
			}
			return(c);
		} else if(c == '\t') {
			spaces = (tabsize - (col % tabsize)) - 1;
			c = ' ';
			break;
		}
	} while(iscntrl(c));
	++col;
	return(c);
}

char	token[256];

int
next_token(f)
FILE *f;
{
	static int state = TK_EMPTY;
	static int qchar = '"';
	int c;
	char *tp;

	tp = token;
	while((c = next_char(f)) != EOF) {
		switch(state) {

		case TK_EMPTY:
			switch(c) {
			case '/':
				c = next_char(f);
				if(c == EOF) {
					*tp++ = '/';
					*tp = '\0';
					return(TK_TEXT);
				} else if(c == '*') {
					*tp++ = '/';
					state = TK_CSTART;
				} else {
					*tp++ = '/';
					state = TK_TEXT;
				}
				break;
			case '\n':
				*tp++ = '\n';
				*tp = '\0';
				return(TK_NEWLINE);
			case '\f':
				*tp++ = '\f';
				*tp = '\0';
				return(TK_FORMFEED);
			case '\'':
			case '"':
				qchar = c;
				state = TK_STEXT;
				break;
			default:
				state = (isalpha(c) || (c=='_'))
					? TK_WORD
					: TK_TEXT;
				break;
			}
			break;

		case TK_TEXT:
			if(isalpha(c)
			|| (c=='_') || (c=='/')
			|| (c=='"') || (c=='\'') || iscntrl(c)) {
				push_back(f, c);
				*tp = '\0';
				state = TK_EMPTY;
				return(TK_TEXT);
			}
			break;

		case TK_WORD:
			if(!(isalnum(c) || (c=='_'))
			|| (c=='/') || (c=='"') || (c=='\'')
			|| iscntrl(c)) {
				push_back(f, c);
				*tp = '\0';
				state = TK_EMPTY;
				return(iskeyword(token)
					? TK_KWORD
					: TK_WORD);
			}
			break;

		case TK_CSTART:
			if(c!='*') {
				push_back(f, c);
				*tp = '\0';
				state = (c == '/') ? TK_CEND : TK_CEMPTY;
				return(TK_CSTART);
			}
			break;

		case TK_CEMPTY:
			switch(c) {
			case '\n':
				*tp++ = '\n';
				*tp = '\0';
				return(TK_NEWLINE);
			case '\f':
				*tp++ = '\f';
				*tp = '\0';
				return(TK_FORMFEED);
			case '*':
				state = TK_CEND;
				break;
			default:
				state = TK_CTEXT;
				break;
			}
			break;

		case TK_CTEXT:
			if((c=='*') || iscntrl(c)) {
				push_back(f, c);
				*tp = '\0';
				state = (c == '*') ? TK_CEND : TK_CEMPTY;
				return(TK_CTEXT);
			}
			break;

		case TK_CEND:
			if(c == '/') {
				*tp++ = '/';
				*tp = '\0';
				state = TK_EMPTY;
				return(TK_CEND);
			} else if(c != '*') {
				push_back(f, c);
				*tp = '\0';
				state = TK_CTEXT;
				return(TK_CTEXT);
			}
			break;

		case TK_SEMPTY:
			switch(c) {
			case '\n':
				*tp++ = '\n';
				*tp = '\0';
				return(TK_NEWLINE);
			case '\f':
				*tp++ = '\f';
				*tp = '\0';
				return(TK_FORMFEED);
			default:
				if(c == qchar) {
					*tp++ = c;
					*tp = '\0';
					state = TK_EMPTY;
					return(TK_STEXT);
				}
				state = TK_STEXT;
				break;
			}
			break;

		case TK_STEXT:
			if(c == qchar) {
				*tp++ = c;
				*tp = '\0';
				state = TK_EMPTY;
				return(TK_STEXT);
			} else if(c == '\\') {
				*tp++ = '\\';
				c = next_char(f);
				if(c == EOF) {
					*tp = '\0';
					state = TK_EMPTY;
					return(TK_STEXT);
				}
			}
			if(iscntrl(c)) {
				push_back(f, c);
				*tp = '\0';
				state = TK_SEMPTY;
				return(TK_STEXT);
			}
			break;

		}
		*tp++ = c;
	}
	*tp = '\0';
	return(TK_EMPTY);
}

#define	ROMAN_FONT	(0)
#define	BOLD_FONT	(1)
#define	ITALIC_FONT	(2)

void
list(fi, fo)
FILE *fi;
FILE *fo;
{
	int tk, ptk;
	int font = ROMAN_FONT;

	start_listing(fo);
	ptk = TK_EMPTY;
	while((tk = next_token(fi)) != TK_EMPTY) {
		switch(tk) {
		case TK_TEXT:
		case TK_WORD:
		case TK_STEXT:
			if(font != ROMAN_FONT) {
				fprintf(fo, "R ");
				font = ROMAN_FONT;
			}
			fprintf(fo, "(%s) S\n", ps_string(token));
			break;
		case TK_KWORD:
			if(font != BOLD_FONT) {
				fprintf(fo, "B ");
				font = BOLD_FONT;
			}
			fprintf(fo, "(%s) S\n", ps_string(token));
			break;
		case TK_CSTART:
			if(font != ROMAN_FONT) {
				fprintf(fo, "R ");
				font = ROMAN_FONT;
			}
			fprintf(fo, "(%s) S\n", ps_string(token));
			break;
		case TK_CTEXT:
			if(font != ITALIC_FONT) {
				fprintf(fo, "I ");
				font = ITALIC_FONT;
			}
			fprintf(fo, "(%s) S\n", ps_string(token));
			break;
		case TK_CEND:
			font = ROMAN_FONT;
			fprintf(fo, "R (%s) S\n", ps_string(token));
			break;
		case TK_NEWLINE:
			if(((ptk == TK_NEWLINE) || (ptk == TK_FORMFEED))
			&& (line < 2)) {
				break;	/* ignore if at top of page */
			}
			if((ptk != TK_NEWLINE) || (line < (LPP - 6))) {
				new_line(fo);
				break;
			}
			/* FALL THRU */
		case TK_FORMFEED:
			if(line < 2) {
				break;	/* ignore if at top of page */
			}
			page_eject(fo);
			break;
		}
		ptk = tk;
	}
	end_listing(fo);
}

void
usage()
{
	fprintf(stderr, "\
usage: c_format [-options] [-] [file.c]\n\
options:\n\
	-d		draft output mode\n\
	-t title	set listing title\n\
	-a author	set listing author\n\
	-x tabsize	set tab stops\n\
	-o output.ps	write to output file (default to stdout)\n\
");
	exit(1);
}

main(argc, argv)
int argc;
char **argv;
{
	FILE *f;
	register int c;
	register char *p;
	extern int optind;
	extern char *optarg;

	while((c = getopt(argc, argv, "dt:a:x:o:f:")) != EOF) {
		switch(c) {
		case 'd':
			draft = !draft;
			break;
		case 't':
			strcpy(title, optarg);
			break;
		case 'a':
			strcpy(author, optarg);
			break;
		case 'x':
			tabsize = atoi(optarg);
			break;
		case 'o':
			if(freopen(optarg, "w", stdout) == NULL) {
				perror(optarg);
				exit(1);
			}
			break;
		case 'f':
			c = atoi(optarg);
			if((c > 0) && (c < LPP)) {
				firstline = c;
			}
			break;
		case '?':
		default:
			usage();
		}
	}
	c = argc - optind;
	if(c < 1) {
		if(isatty(fileno(stdin))) {
			usage();
		} else {
			list(stdin, stdout);
		}
	} else if(c == 1) {
		p = argv[optind];
		if((p[0] == '-') && (p[1] == '\0')) {
			list(stdin, stdout);
		} else {
			if(f = fopen(p, "r")) {
				list(f, stdout);
				fclose(f);
			} else {
				perror(p);
				exit(1);
			}
		}
	} else {
		usage();
	}
	exit(0);
}

