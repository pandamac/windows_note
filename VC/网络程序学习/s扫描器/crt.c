
#include <tchar.h>
#include <windows.h>

int main(int, char **);



int _vsnprintf( char *buffer, size_t count, const char *format, va_list argptr );

int
printf(
	   const char * format, 
	   ...)
{
	int		bytesFormat = 0;
	DWORD	bytesWritten = 0;
	char	buff[1024];
	va_list	arg_list;
	
	static HANDLE		hConsoleOutput = NULL;
	
	if (hConsoleOutput == NULL)
	{
		hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	
	va_start(arg_list, format);
	bytesFormat = _vsnprintf(buff, sizeof(buff), format, arg_list);
	va_end(arg_list);
	
	WriteConsole(hConsoleOutput, buff, bytesFormat, &bytesWritten, NULL);
	
	return bytesFormat;
}


#define NULCHAR    _T('\0')
#define SPACECHAR  _T(' ')
#define TABCHAR    _T('\t')
#define DQUOTECHAR _T('\"')
#define SLASHCHAR  _T('\\')

#    ifndef _ismbblead
#      define _ismbblead(c) (0x80 <= (c) && ((c) < 0xa0 || 0xe0 <= (c)))
#    endif

static void __cdecl parse_cmdline (								   
	char *cmdstart,
    char **argv,
    char *args,
    int *numargs,
    int *numchars
    )
{
        char *p;
        _TUCHAR c;
        int inquote;                    /* 1 = inside quotes */
        int copychar;                   /* 1 = copy char to *args */
        unsigned numslash;              /* num of backslashes seen */

        *numchars = 0;
        *numargs = 1;                   /* the program name at least */

        /* first scan the program name, copy it, and count the bytes */
        p = cmdstart;
        if (argv)
            *argv++ = args;

#ifdef WILDCARD
        /* To handle later wild card expansion, we prefix each entry by
        it's first character before quote handling.  This is done
        so _[w]cwild() knows whether to expand an entry or not. */
        if (args)
            *args++ = *p;
        ++*numchars;

#endif  /* WILDCARD */

        /* A quoted program name is handled here. The handling is much
           simpler than for other arguments. Basically, whatever lies
           between the leading double-quote and next one, or a terminal null
           character is simply accepted. Fancier handling is not required
           because the program name must be a legal NTFS/HPFS file name.
           Note that the double-quote characters are not copied, nor do they
           contribute to numchars. */
        if ( *p == DQUOTECHAR ) {
            /* scan from just past the first double-quote through the next
               double-quote, or up to a null, whichever comes first */
            while ( (*(++p) != DQUOTECHAR) && (*p != NULCHAR) ) {

#ifdef _MBCS
                if (_ismbblead(*p)) {
                    ++*numchars;
                    if ( args )
                        *args++ = *p++;
                }
#endif  /* _MBCS */
                ++*numchars;
                if ( args )
                    *args++ = *p;
            }
            /* append the terminating null */
            ++*numchars;
            if ( args )
                *args++ = NULCHAR;

            /* if we stopped on a double-quote (usual case), skip over it */
            if ( *p == DQUOTECHAR )
                p++;
        }
        else {
            /* Not a quoted program name */
            do {
                ++*numchars;
                if (args)
                    *args++ = *p;

                c = (_TUCHAR) *p++;
#ifdef _MBCS
                if (_ismbblead(c)) {
                    ++*numchars;
                    if (args)
                        *args++ = *p;   /* copy 2nd byte too */
                    p++;  /* skip over trail byte */
                }
#endif  /* _MBCS */

            } while ( c != SPACECHAR && c != NULCHAR && c != TABCHAR );

            if ( c == NULCHAR ) {
                p--;
            } else {
                if (args)
                    *(args-1) = NULCHAR;
            }
        }

        inquote = 0;

        /* loop on each argument */
        for(;;) {

            if ( *p ) {
                while (*p == SPACECHAR || *p == TABCHAR)
                    ++p;
            }
			
            if (*p == NULCHAR)
                break;              /* end of args */
			
            /* scan an argument */
            if (argv)
                *argv++ = args;     /* store ptr to arg */
            ++*numargs;
			
#ifdef WILDCARD
			/* To handle later wild card expansion, we prefix each entry by
			it's first character before quote handling.  This is done
			so _[w]cwild() knows whether to expand an entry or not. */
			if (args)
				*args++ = *p;
			++*numchars;
			
#endif  /* WILDCARD */
			
			/* loop through scanning one argument */
			for (;;) {
				copychar = 1;
				/* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
				2N+1 backslashes + " ==> N backslashes + literal "
				N backslashes ==> N backslashes */
				numslash = 0;
				while (*p == SLASHCHAR) {
					/* count number of backslashes for use below */
					++p;
					++numslash;
				}
				if (*p == DQUOTECHAR) {
                /* if 2N backslashes before, start/end quote, otherwise
                    copy literally */
					if (numslash % 2 == 0) {
						if (inquote) {
							if (p[1] == DQUOTECHAR)
								p++;    /* Double quote inside quoted string */
							else        /* skip first quote char and copy second */
								copychar = 0;
						} else
							copychar = 0;       /* don't copy quote */
						
						inquote = !inquote;
					}
					numslash /= 2;          /* divide numslash by two */
				}
				
				/* copy slashes */
				while (numslash--) {
					if (args)
						*args++ = SLASHCHAR;
					++*numchars;
				}
				
				/* if at end of arg, break loop */
				if (*p == NULCHAR || (!inquote && (*p == SPACECHAR || *p == TABCHAR)))
					break;
				
				/* copy character into argument */
#ifdef _MBCS
				if (copychar) {
					if (args) {
						if (_ismbblead(*p)) {
							*args++ = *p++;
							++*numchars;
						}
						*args++ = *p;
					} else {
						if (_ismbblead(*p)) {
							++p;
							++*numchars;
						}
					}
					++*numchars;
				}
				++p;
#else  /* _MBCS */
				if (copychar) {
					if (args)
						*args++ = *p;
					++*numchars;
				}
				++p;
#endif  /* _MBCS */
            }
			
            /* null-terminate the argument */
			
            if (args)
                *args++ = NULCHAR;          /* terminate string */
            ++*numchars;
        }
		
        /* We put one last argument in -- a null ptr */
        if (argv)
            *argv++ = NULL;
        ++*numargs;
}

void *malloc( size_t size )
{
	return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

void free( void *memblock )
{
	VirtualFree(memblock, 0, MEM_RELEASE);
}
	

void mainCRTStartup()
{
	int	ret = EXIT_SUCCESS;
	int numargs, numchars;
	void *	p = NULL;
	char *	cmdline = GetCommandLine();

	parse_cmdline(cmdline, NULL, NULL, &numargs, &numchars);
	p = malloc(numargs * sizeof(_TSCHAR *) + numchars * sizeof(_TSCHAR));
	if (p)
	{
		parse_cmdline(cmdline, (char **)p, (char *)p + numargs * sizeof(char *), &numargs, &numchars);
		ret = main(numargs - 1, (char **)p);
		free(p);
	}

	ExitProcess(ret);	
}