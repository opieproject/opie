
//This code is need to get the WinMain function called. Current MinGW runtimes tries to call main
//before WinMain. Untill such time as MinGw runtime has been changed this is need.
//Most of this code is taken from the MinGw runtime sources.


#include <signal.h>
#include <process.h>
#include <float.h>
#include <io.h>


/*
 * Setup the default file handles to have the _CRT_fmode mode, as well as
 * any new files created by the user.
 */

extern int _fmode; 
extern "C"  int* __p__fmode(void); /* To access the dll _fmode */
extern int _CRT_fmode;

static void _mingw32_init_fmode ()
{
  /* Don't set the std file mode if the user hasn't set any value for it. */
  if (_CRT_fmode)
    {
      _fmode = _CRT_fmode;

      /*
       * This overrides the default file mode settings for stdin,
       * stdout and stderr. At first I thought you would have to
       * test with isatty, but it seems that the DOS console at
       * least is smart enough to handle _O_BINARY stdout and
       * still display correctly.
       */
      if (stdin)
	{
	  _setmode (_fileno (stdin), _CRT_fmode);
	}
      if (stdout)
	{
	  _setmode (_fileno (stdout), _CRT_fmode);
	}
      if (stderr)
	{
	  _setmode (_fileno (stderr), _CRT_fmode);
	}
    }

    /*  Now sync  the dll _fmode to the  one for this .exe.  */
    *__p__fmode() = _fmode;	

}

/* This function will be called when a trap occurs. Thanks to Jacob
   Navia for his contribution. */
static CALLBACK long _gnu_exception_handler (EXCEPTION_POINTERS * exception_data)
{
  void (*old_handler) (int);
  long action = EXCEPTION_CONTINUE_SEARCH;
  int reset_fpu = 0;

  switch (exception_data->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
      /* test if the user has set SIGSEGV */
      old_handler = signal (SIGSEGV, SIG_DFL);
      if (old_handler == SIG_IGN)
	{
	  /* this is undefined if the signal was raised by anything other
	     than raise ().  */
	  signal (SIGSEGV, SIG_IGN);
	  action = EXCEPTION_CONTINUE_EXECUTION;
	}
      else if (old_handler != SIG_DFL)
	{
	  /* This means 'old' is a user defined function. Call it */
	  (*old_handler) (SIGSEGV);
	  action = EXCEPTION_CONTINUE_EXECUTION;
	}
      break;

    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_OVERFLOW:
    case EXCEPTION_FLT_UNDERFLOW:
    case EXCEPTION_FLT_INEXACT_RESULT:
      reset_fpu = 1;
      /* fall through. */

    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      /* test if the user has set SIGFPE */
      old_handler = signal (SIGFPE, SIG_DFL);
      if (old_handler == SIG_IGN)
	{
	  signal (SIGFPE, SIG_IGN);
	  if (reset_fpu)
	    _fpreset ();
	  action = EXCEPTION_CONTINUE_EXECUTION;
	}
      else if (old_handler != SIG_DFL)
	{
	  /* This means 'old' is a user defined function. Call it */
	  (*old_handler) (SIGFPE);
	  action = EXCEPTION_CONTINUE_EXECUTION;
	}
      break;

    default:
      break;
    }
  return action;
}


extern char __RUNTIME_PSEUDO_RELOC_LIST__;
extern char __RUNTIME_PSEUDO_RELOC_LIST_END__;
extern char _image_base__;

typedef struct {
    DWORD addend;
    DWORD target;
} runtime_pseudo_reloc;

static void do_pseudo_reloc (void* start, void* end, void* base)
{
    DWORD reloc_target;
    runtime_pseudo_reloc* r;
    for (r = (runtime_pseudo_reloc*) start; r < (runtime_pseudo_reloc*) end; r++) {
        reloc_target = (DWORD) base + r->target;
        *((DWORD*) reloc_target) += r->addend;
    }
}

void _pei386_runtime_relocator ()
{
    do_pseudo_reloc (&__RUNTIME_PSEUDO_RELOC_LIST__,
                     &__RUNTIME_PSEUDO_RELOC_LIST_END__,
                     &_image_base__);
}


#ifdef __MSVCRT__
#define __UNKNOWN_APP    0
#define __CONSOLE_APP    1
#define __GUI_APP        2
extern "C" void __set_app_type(int);
#endif /* __MSVCRT__ */

#define ISSPACE(a)	(a == ' ' || a == '\t')

int QtWinMainCRTStartup()
{
#ifdef __MSVCRT__
    __set_app_type (__GUI_APP);
#endif
        
    SetUnhandledExceptionFilter (_gnu_exception_handler);
    
    /*
    * Initialize floating point unit.
    */
    _fpreset ();			/* Supplied by the runtime library. */
    
    /*
    * Sets the default file mode.
    * If _CRT_fmode is set, also set mode for stdin, stdout
    * and stderr, as well
    * NOTE: DLLs don't do this because that would be rude!
    */
    _mingw32_init_fmode ();
    
    /* Adust references to dllimported data that have non-zero offsets.  */
    _pei386_runtime_relocator ();
    
    char *szCmd;
    STARTUPINFO startinfo;
    int nRet;
    
    /* Get the command line passed to the process. */
    szCmd = GetCommandLineA ();
    GetStartupInfo (&startinfo);
    
    /* Strip off the name of the application and any leading
    * whitespace. */
    if (szCmd) {
        
        while (ISSPACE (*szCmd)) {
            szCmd++;
        }
        
        /* On my system I always get the app name enclosed
        * in quotes... */
        if (*szCmd == '\"') {
            do {
                szCmd++;
            } while (*szCmd != '\"' && *szCmd != '\0');
            
            if (*szCmd == '\"') {
                szCmd++;
            }
        } else {
        /* If no quotes then assume first token is program
        * name. */
            while (!ISSPACE (*szCmd) && *szCmd != '\0') {
                szCmd++;
            }
        }
        
        while (ISSPACE (*szCmd)) {
            szCmd++;
        }
    }
    
    nRet = WinMain (GetModuleHandle (NULL), NULL, szCmd,
                    (startinfo.dwFlags & STARTF_USESHOWWINDOW) ?
                     startinfo.wShowWindow : SW_SHOWDEFAULT);
    
    /*
    * Perform exit processing for the C library. This means
    * flushing output and calling 'atexit' registered functions.
    */
    _cexit ();
    
    ExitProcess (nRet);
    
    return 0;
    
}

