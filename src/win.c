/*Elkulator v1.0 by Sarah Walker
  Windows main loop*/
#ifdef WIN32

#ifdef HAL_ALLEGRO_5
#include <allegro5/allegro.h>
#else
#include <allegro.h>
#endif

#include <winalleg.h>
#include <process.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
//#define WIN32_LEAN_AND_MEAN
#include "elk.h"

#include "resources.h"


void error(const char *format, ...)
{
   char buf[256];

   va_list ap;
   va_start(ap, format);
   vsprintf(buf, format, ap);
   va_end(ap);
   MessageBox(NULL,buf,"Elkulator error",MB_OK);;
}

char ssname[260];
char scrshotname[260];

int tapespeed;
int fullscreen=0;
int gotofullscreen=0;
int videoresize=0;
int wantloadstate=0,wantsavestate=0;
int winsizex=640,winsizey=512;
HWND      ghwnd;
HINSTANCE ghinstance;
HMENU     ghmenu;

int plus3=0;
int dfsena=0,adfsena=0;
int turbo=0;
int mrb=0,mrbmode=0;
int ulamode=0;
int drawmode=0;

char discname[260];
char discname2[260];

void setmenu(HMENU m)
{
	RECT ClientArea;
	GetClientRect( ghwnd, &ClientArea);

	SetMenu( ghwnd, m);

	RECT NewClientArea;
	GetClientRect( ghwnd, &NewClientArea);

	/* check if we've lost pixels */
	if(NewClientArea.bottom != ClientArea.bottom)
	{
		/* fix up */
		RECT WindowArea;
		GetWindowRect( ghwnd, &WindowArea);
		WindowArea.bottom += ClientArea.bottom - NewClientArea.bottom;

		MoveWindow( ghwnd, WindowArea.left, WindowArea.top, WindowArea.right - WindowArea.left, WindowArea.bottom - WindowArea.top, TRUE);
	}
}

void getwindowsstuff()
{
/*        SDL_SysWMinfo sysinfo;
        SDL_VERSION(&sysinfo.version);
        if (SDL_GetWMInfo(&sysinfo)==1)
        {
                ghwnd=sysinfo.window;
                ghinstance=GetModuleHandle(NULL);
                ghmenu = LoadMenu(ghinstance,TEXT("MainMenu"));
                setmenu(ghmenu);
                SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
        }
        SDL_WM_SetCaption("Elkulator v0.7 - 15th Nov build no 2","Elkulator");*/
}


int getfn(HWND hwnd, char *f, char *s, int save, char *de)
{
        char fn[512];
        char start[512];
        OPENFILENAME ofn;
        fn[0]=0;
        start[0]=0;
//        strcpy(fn,s);
//        rpclog("getfn - file %s filter %s\n",fn,f);
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = fn;
        ofn.nMaxFile = 260;
        ofn.lpstrFilter = f;//"All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | 0x10000000;//OFN_FORCESHOWHIDDEN;
        ofn.lpstrDefExt=de;
        if (save)
        {
                if (GetSaveFileName(&ofn))
                {
                        strcpy(s,fn);
                        return 0;
                }
        }
        else
        {
                if (GetOpenFileName(&ofn))
                {
                        strcpy(s,fn);
                        return 0;
                }
        }
        return -1;
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void initmenu()
{
        CheckMenuItem(ghmenu,IDM_DISC_PLUS3,(plus3)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_DISC_ADFS,(adfsena)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_DISC_DFS,(dfsena)?MF_CHECKED:MF_UNCHECKED);
        
        CheckMenuItem(ghmenu,IDM_DISC_WPROT_0,(writeprot[0])?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_DISC_WPROT_1,(writeprot[1])?MF_CHECKED:MF_UNCHECKED);
        
        CheckMenuItem(ghmenu,IDM_DISC_WPROT_D,(defaultwriteprot)?MF_CHECKED:MF_UNCHECKED);
        
        CheckMenuItem(ghmenu,IDM_OTHER_TURBO,(turbo)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_OTHER_MRB,(mrb)?MF_CHECKED:MF_UNCHECKED);

        CheckMenuItem(ghmenu,IDM_OTHER_SNDIN,(sndint)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_OTHER_SNDEX,(sndex)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_OTHER_FBJOY,(firstbyte)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_OTHER_PLUS1,(plus1)?MF_CHECKED:MF_UNCHECKED);
        
        CheckMenuItem(ghmenu,IDM_MRB_OFF,(mrbmode==0)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_MRB_TURBO,(mrbmode==1)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_MRB_SHADOW,(mrbmode==2)?MF_CHECKED:MF_UNCHECKED);

        CheckMenuItem(ghmenu,IDM_ULA_STANDARD,(ulamode==ULA_CONVENTIONAL)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_ULA_ENHANCED_8BIT_DUAL,(ulamode==ULA_RAM_8BIT_DUAL_ACCESS)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_ULA_ENHANCED_8BIT_SINGLE,(ulamode==ULA_RAM_8BIT_SINGLE_ACCESS)?MF_CHECKED:MF_UNCHECKED);

        CheckMenuItem(ghmenu,IDM_VIDEO_SCANLINES,(drawmode==0)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_VIDEO_LINEDBL,(drawmode==1)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_VIDEO_2XSAI,(drawmode==2)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_VIDEO_SCALE2X,(drawmode==3)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_VIDEO_EAGLE,(drawmode==4)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_VIDEO_PAL,(drawmode==5)?MF_CHECKED:MF_UNCHECKED);
        
        CheckMenuItem(ghmenu,IDM_VIDEO_RESIZE,(videoresize)?MF_CHECKED:MF_UNCHECKED);

        CheckMenuItem(ghmenu,IDM_SPD_NORMAL,(tapespeed==0)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_SPD_FAST,  (tapespeed==1)?MF_CHECKED:MF_UNCHECKED);
        CheckMenuItem(ghmenu,IDM_SPD_REALLY,(tapespeed==2)?MF_CHECKED:MF_UNCHECKED);
        
        if (sndddnoise)  CheckMenuItem(ghmenu,IDM_SOUND_DDNOISE, MF_CHECKED);
        if (sndtape)     CheckMenuItem(ghmenu,IDM_SOUND_TNOISE,  MF_CHECKED);
        CheckMenuItem(ghmenu,IDM_DDT_525+ddtype,MF_CHECKED);
        CheckMenuItem(ghmenu,(IDM_DDV_33+ddvol)-1,MF_CHECKED);
}

void runelk();

HWND ghwnd;
char szClassName[ ] = "ElkWindow";
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void updatewindowsize(int x, int y)
{
        RECT r;
        winsizex=x;
        winsizey=y;
        GetWindowRect(ghwnd,&r);
        MoveWindow(ghwnd,r.left,r.top,
                     x+(GetSystemMetrics(SM_CXFIXEDFRAME)*2),
                     y+(GetSystemMetrics(SM_CYFIXEDFRAME)*2)+GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYCAPTION)+1,
                     TRUE);
}

int createwindow(HINSTANCE hThisInstance, int nFunsterStil)
{
        WNDCLASSEX wincl;        /* Data structure for the windowclass */
        /* The Window structure */
        wincl.hInstance = hThisInstance;
        wincl.lpszClassName = szClassName;
        wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
        wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
        wincl.cbSize = sizeof (WNDCLASSEX);

        /* Use default icon and mouse-pointer */
        wincl.hIcon = LoadIcon(hThisInstance, "allegro_icon");
        wincl.hIconSm = LoadIcon(hThisInstance, "allegro_icon");
        wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
        wincl.lpszMenuName = NULL;                 /* No menu */
        wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
        wincl.cbWndExtra = 0;                      /* structure or the window instance */
        /* Use Windows's default color as the background of the window */
        wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

        /* Register the window class, and if it fails quit the program */
        if (!RegisterClassEx (&wincl))
           return -1;
        /* The class is registered, let's create the program*/
        ghwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Elkulator v1.0",    /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           640+(GetSystemMetrics(SM_CXFIXEDFRAME)*2),/* The programs width */
           512+(GetSystemMetrics(SM_CYFIXEDFRAME)*2)+GetSystemMetrics(SM_CYMENUSIZE)+GetSystemMetrics(SM_CYCAPTION)+2,/* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           LoadMenu(hThisInstance,TEXT("MainMenu")),                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

        /* Make the window visible on the screen */
        ShowWindow (ghwnd, nFunsterStil);
        win_set_window(ghwnd);

        ghmenu=GetMenu(ghwnd);
        return 0;
}

HANDLE mainthread;
int bempause=0,bemwaiting=0;
void _mainthread(PVOID pvoid)
{
//        initbbc(argc,argv);
        while (1)
        {
                if (bempause)
                {
                        bemwaiting=1;
                        Sleep(100);
                }
                else
                {
                        bemwaiting=0;
                        runelk();
                }
        }
}

CRITICAL_SECTION cs;

void startblit()
{
        EnterCriticalSection(&cs);
}

void endblit()
{
        LeaveCriticalSection(&cs);
}

void waitforthread()
{
        startblit();
        Sleep(200);
}

void stopwaiting()
{
        endblit();
}

int quited=0;
int infocus=1;

void setquit()
{
        quited=1;
}

void setejecttext(int drive, char *fn)
{
	MENUITEMINFO mi;
	HMENU hmenu;
	char s[128];
	int c;
	for (c=0;c<strlen(fn);c++) if (fn[c]==13 || fn[c]==10) fn[c]=32;
	if (fn[0]) sprintf(s,"Eject drive :%i/%i - %s",drive,drive+2,get_filename(fn));
	else       sprintf(s,"Eject drive :%i/%i",drive,drive+2);
	memset(&mi,0,sizeof(MENUITEMINFO));
	mi.cbSize=sizeof(MENUITEMINFO);
	mi.fMask=MIIM_STRING;
	mi.fType=MFT_STRING;
	mi.dwTypeData=s;
	hmenu=GetMenu(ghwnd);
	SetMenuItemInfo(hmenu,IDM_DISC_EJECT_0+drive,0,&mi);
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{
        MSG messages;            /* Here messages to the application are saved */
        int oldaltenter=0;
        ghinstance=hThisInstance;
        if (createwindow(hThisInstance,nFunsterStil))
           return -1;
        allegro_init();
        getwindowsstuff();
        initelk();
        initmenu();
        if (videoresize) SetWindowLong(ghwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW|WS_VISIBLE);
        else             SetWindowLong(ghwnd, GWL_STYLE, (WS_OVERLAPPEDWINDOW&~WS_SIZEBOX&~WS_THICKFRAME&~WS_MAXIMIZEBOX)|WS_VISIBLE);
        updatewindowsize(640,512);
        InitializeCriticalSection(&cs);
        mainthread=(HANDLE)_beginthread(_mainthread,0,NULL);
//        startsdl(runelk);
        while (!quited)
        {
//                runelk();
                if (gotofullscreen || (!fullscreen && key[KEY_ALT] && key[KEY_ENTER] && !oldaltenter))
                {
                        gotofullscreen=0;
                        startblit();
                        Sleep(200);
                        updatewindowsize(800,600);
                        enterfullscreen();
//                        set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 512, 0, 0);
                        setmenu(NULL);
                        fullscreen=1;
                        key[KEY_ALT]=key[KEY_ENTER]=0;
                        endblit();
                }
                else if (key[KEY_ALT] && key[KEY_ENTER] && fullscreen && !oldaltenter)
                {
                        fullscreen=0;
                        startblit();
                        Sleep(200);
                        leavefullscreen();
//                        set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 512, 0, 0);
                        setmenu(ghmenu);
                        updatewindowsize(640,512);
//                        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
                        key[KEY_ALT]=key[KEY_ENTER]=0;
                        endblit();
                }
                oldaltenter=key[KEY_ALT] && key[KEY_ENTER];
                if (PeekMessage(&messages,NULL,0,0,PM_REMOVE))
                {
                        if (messages.message==WM_QUIT)
                           quited=1;
                        /* Translate virtual-key messages into character messages */
                        TranslateMessage(&messages);
                        /* Send message to WindowProcedure */
                        DispatchMessage(&messages);
                }
                Sleep(20);
        }
        EnterCriticalSection(&cs);
        Sleep(200);
        TerminateThread(mainthread,0);
        closeelk();
        DeleteCriticalSection(&cs);
        return 0;
}

extern unsigned char hw_to_mycode[256];
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        int c;
        char s[512];
        HMENU hmenu;
        RECT rect;
        switch (message)                  /* handle the messages */
        {
                case WM_CREATE:
                for (c=0;c<128;c++) key[c]=0;
                return 0;
                case WM_COMMAND:
                hmenu=GetMenu(hwnd);
                switch (LOWORD(wParam))
                {
                        case IDM_FILE_RESET:
                        startblit();
                        Sleep(200);
                        resetit=1;
                        endblit();
                        break;
                        case IDM_FILE_LSTATE:
                        EnterCriticalSection(&cs);
                        if (!getfn(hwnd,"Save State (*.SNP)\0*.SNP\0All files (*.*)\0*.*\0\0",ssname,0,"SNP"))
                           loadstate(ssname);
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_FILE_SSTATE:
                        EnterCriticalSection(&cs);
                        if (!getfn(hwnd,"Save State (*.SNP)\0*.SNP\0All files (*.*)\0*.*\0\0",ssname,1,"SNP"))
                           savestate(ssname);
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_FILE_EXIT:
                        PostQuitMessage(0);
                        break;
                        case IDM_TAPE_LOAD:
                        EnterCriticalSection(&cs);
                        if (!getfn(hwnd,"All tape files (*.UEF;*.CSW)\0*.UEF;*.CSW\0UEF Tape Image (*.UEF)\0*.UEF\0All\0*.*\0\0",tapename,0,"UEF"))
                        {
                                loadtape(tapename);
                        }
                        //resetsound();
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_TAPE_REWIND:
                        loadtape(tapename);
                        break;
                        case IDM_DISC_PLUS3:
                        waitforthread();
                        plus3^=1;
                        if (firstbyte)
                        {
                                firstbyte=0;
                                CheckMenuItem(ghmenu,IDM_OTHER_FBJOY,MF_UNCHECKED);
                        }
                        CheckMenuItem(ghmenu,IDM_DISC_PLUS3,(plus3)?MF_CHECKED:MF_UNCHECKED);
                        reset6502e();
                        resetula();
                        stopwaiting();
                        break;
                        case IDM_DISC_ADFS:
                        waitforthread();
                        adfsena^=1;
                        CheckMenuItem(ghmenu,IDM_DISC_ADFS,(adfsena)?MF_CHECKED:MF_UNCHECKED);
                        reset6502e();
                        resetula();
                        stopwaiting();
                        break;
                        case IDM_DISC_DFS:
                        waitforthread();
                        dfsena^=1;
                        CheckMenuItem(ghmenu,IDM_DISC_DFS,(dfsena)?MF_CHECKED:MF_UNCHECKED);
                        reset6502e();
                        resetula();
                        stopwaiting();
                        break;
                        case IDM_OTHER_TURBO:
//                        waitforthread();
                        turbo^=1;
                        CheckMenuItem(ghmenu,IDM_OTHER_TURBO,(turbo)?MF_CHECKED:MF_UNCHECKED);
                        if (turbo)
                        {
                                mrb=0;
                                CheckMenuItem(ghmenu,IDM_OTHER_MRB,MF_UNCHECKED);
                        }
                        reset6502e();
                        resetula();
//                        stopwaiting();
                        break;
                        case IDM_OTHER_MRB:
                        waitforthread();
                        mrb^=1;
                        CheckMenuItem(ghmenu,IDM_OTHER_MRB,(mrb)?MF_CHECKED:MF_UNCHECKED);
                        if (mrb)
                        {
                                turbo=0;
                                CheckMenuItem(ghmenu,IDM_OTHER_TURBO,MF_UNCHECKED);
                        }
                        reset6502e();
                        resetula();
                        stopwaiting();
                        break;
                        case IDM_OTHER_SNDIN:
                        sndint^=1;
                        CheckMenuItem(ghmenu,IDM_OTHER_SNDIN,(sndint)?MF_CHECKED:MF_UNCHECKED);
                        break;
                        case IDM_OTHER_SNDEX:
                        waitforthread();
                        sndex^=1;
                        CheckMenuItem(ghmenu,IDM_OTHER_SNDEX,(sndex)?MF_CHECKED:MF_UNCHECKED);
                        reset6502e();
                        resetula();
                        resetsound();
                        stopwaiting();
                        break;
                        case IDM_OTHER_FBJOY:
                        waitforthread();
                        firstbyte^=1;
                        CheckMenuItem(ghmenu,IDM_OTHER_FBJOY,(firstbyte)?MF_CHECKED:MF_UNCHECKED);
                        if (plus3)
                        {
                                plus3=0;
                                CheckMenuItem(ghmenu,IDM_DISC_PLUS3,MF_UNCHECKED);
                                reset6502e();
                                resetula();
                        }
                        stopwaiting();
                        break;
                        case IDM_OTHER_PLUS1:
                        waitforthread();
                        plus1^=1;
                        CheckMenuItem(ghmenu,IDM_OTHER_PLUS1,(plus1)?MF_CHECKED:MF_UNCHECKED);
                        if (firstbyte)
                        {
                                firstbyte=0;
                                CheckMenuItem(ghmenu,IDM_OTHER_FBJOY,MF_UNCHECKED);
                        }
                        reset6502e();
                        resetula();
                        stopwaiting();
                        break;
                        case IDM_MRB_OFF:
                        case IDM_MRB_TURBO:
                        case IDM_MRB_SHADOW:
//                        waitforthread();
                        mrbmode=wParam-IDM_MRB_OFF;
                        CheckMenuItem(ghmenu,IDM_MRB_OFF,(mrbmode==0)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_MRB_TURBO,(mrbmode==1)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_MRB_SHADOW,(mrbmode==2)?MF_CHECKED:MF_UNCHECKED);
                        reset6502e();
                        resetula();
//                        stopwaiting();
                        break;
                        case IDM_ULA_STANDARD:
                        case IDM_ULA_ENHANCED_8BIT_SINGLE:
                        case IDM_ULA_ENHANCED_8BIT_DUAL:
                        ulamode=wParam-IDM_ULA_STANDARD;
                        /* Adjust ulamode to account for a "missing" entry. */
                        if (ulamode) ulamode += 1;
                        CheckMenuItem(ghmenu,IDM_ULA_STANDARD,(ulamode==ULA_CONVENTIONAL)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_ULA_ENHANCED_8BIT_DUAL,(ulamode==ULA_RAM_8BIT_DUAL_ACCESS)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_ULA_ENHANCED_8BIT_SINGLE,(ulamode==ULA_RAM_8BIT_SINGLE_ACCESS)?MF_CHECKED:MF_UNCHECKED);
                        reset6502e();
                        resetula();
                        break;
                        case IDM_DISC_AUTOBOOT:
                        EnterCriticalSection(&cs);
                        if (!getfn(hwnd,"Disc Image (*.ADF;*.ADL;*.SSD;*.DSD)\0*.ADF;*.ADL;*.SSD;*.DSD\0DFS Single Sided Disc Image (*.SSD)\0*.SSD\0DFS Double Sided Disc Image (*.DSD)\0*.DSD\0ADFS Disc Image (*.ADF;*.ADL)\0*.ADF;*.ADL\0All files (*.*)\0*.*\0\0",discname,0,"ADF"))
                           loaddisc(0,discname);
                        reset6502e();
                        resetula();
                        resetsound();
                        autoboot=200;
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_DISC_LOAD_0:
                        EnterCriticalSection(&cs);
//                        if (discchanged[0]) savedisc(discname,0);
                        if (!getfn(hwnd,"Disc Image (*.ADF;*.ADL;*.SSD;*.DSD)\0*.ADF;*.ADL;*.SSD;*.DSD\0DFS Single Sided Disc Image (*.SSD)\0*.SSD\0DFS Double Sided Disc Image (*.DSD)\0*.DSD\0ADFS Disc Image (*.ADF;*.ADL)\0*.ADF;*.ADL\0All files (*.*)\0*.*\0\0",discname,0,"ADF"))
                           loaddisc(0,discname);
                        //resetsound();
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_DISC_LOAD_1:
                        EnterCriticalSection(&cs);
//                        if (discchanged[1]) savedisc(discname,1);
                        if (!getfn(hwnd,"Disc Image (*.ADF;*.ADL;*.SSD;*.DSD)\0*.ADF;*.ADL;*.SSD;*.DSD\0DFS Single Sided Disc Image (*.SSD)\0*.SSD\0DFS Double Sided Disc Image (*.DSD)\0*.DSD\0ADFS Disc Image (*.ADF;*.ADL)\0*.ADF;*.ADL\0All files (*.*)\0*.*\0\0",discname2,0,"ADF"))
                           loaddisc(1,discname2);
                        //resetsound();
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_DISC_EJECT_0:
                        closedisc(0);
                        discname[0]=0;
                        setejecttext(0,"");
                        break;
                        case IDM_DISC_EJECT_1:
                        closedisc(1);
                        discname2[0]=0;
                        setejecttext(1,"");
                        break;
                        case IDM_DISC_NEW_0:
                        if (!getfn(hwnd,"Disc image (*.SSD;*.DSD;*.ADF;*.ADL)\0*.SSD;*.DSD;*.ADF;*.ADL\0All files (*.*)\0*.*\0",discname,1,"ADF"))
                        {
                                closedisc(0);
                                newdisc(0,discname);
                                if (defaultwriteprot) writeprot[0]=1;
                                CheckMenuItem(hmenu,IDM_DISC_WPROT_0,(writeprot[0])?MF_CHECKED:MF_UNCHECKED);
                        }
                        break;
                        case IDM_DISC_NEW_1:
                        if (!getfn(hwnd,"Disc image (*.SSD;*.DSD;*.ADF;*.ADL)\0*.SSD;*.DSD;*.ADF;*.ADL\0All files (*.*)\0*.*\0",discname2,1,"ADF"))
                        {
                                closedisc(1);
                                newdisc(1,discname2);
                                if (defaultwriteprot) writeprot[1]=1;
                                CheckMenuItem(hmenu,IDM_DISC_WPROT_1,(writeprot[1])?MF_CHECKED:MF_UNCHECKED);
                        }
                        break;
                        case IDM_DISC_WPROT_0:
                        writeprot[0]=!writeprot[0];
                        if (fwriteprot[0]) writeprot[0]=1;
                        CheckMenuItem(hmenu,IDM_DISC_WPROT_0,(writeprot[0])?MF_CHECKED:MF_UNCHECKED);
                        break;
                        case IDM_DISC_WPROT_1:
                        writeprot[1]=!writeprot[1];
                        if (fwriteprot[1]) writeprot[1]=1;
                        CheckMenuItem(hmenu,IDM_DISC_WPROT_1,(writeprot[1])?MF_CHECKED:MF_UNCHECKED);
                        break;
                        case IDM_DISC_WPROT_D:
                        defaultwriteprot=!defaultwriteprot;
                        CheckMenuItem(hmenu,IDM_DISC_WPROT_D,(defaultwriteprot)?MF_CHECKED:MF_UNCHECKED);
                        break;
                        case IDM_ROM_LOAD1:
                        EnterCriticalSection(&cs);
                        s[0]=0;
                        if (!getfn(hwnd,"ROM Image (*.ROM)\0*.ROM\0All\0*.*\0\0",s,0,"ROM"))
                        {
                                waitforthread();
                                loadcart(s);
                                reset6502e();
                                resetula();
                                stopwaiting();
                        }
                        //resetsound();
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_ROM_LOAD2:
                        EnterCriticalSection(&cs);
                        s[0]=0;
                        if (!getfn(hwnd,"ROM Image (*.ROM)\0*.ROM\0All\0*.*\0\0",s,0,"ROM"))
                        {
                                waitforthread();
                                loadcart2(s);
                                reset6502e();
                                resetula();
                                stopwaiting();
                        }
                        //resetsound();
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_ROM_UNLOAD:
                        waitforthread();
                        unloadcart();
                        reset6502e();
                        resetula();
                        stopwaiting();
                        break;
                        case IDM_VIDEO_SCANLINES:
                        case IDM_VIDEO_LINEDBL:
                        case IDM_VIDEO_2XSAI:
                        case IDM_VIDEO_SCALE2X:
                        case IDM_VIDEO_EAGLE:
                        case IDM_VIDEO_PAL:
                        drawmode=wParam-IDM_VIDEO_SCANLINES;
//                        clear();
                        CheckMenuItem(ghmenu,IDM_VIDEO_SCANLINES,(drawmode==0)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_VIDEO_LINEDBL,(drawmode==1)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_VIDEO_2XSAI,(drawmode==2)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_VIDEO_SCALE2X,(drawmode==3)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_VIDEO_EAGLE,(drawmode==4)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_VIDEO_PAL,(drawmode==5)?MF_CHECKED:MF_UNCHECKED);
                        clearall();
                        if (drawmode!=1 && videoresize)
                        {
                                videoresize=!videoresize;
                                CheckMenuItem(hmenu,IDM_VIDEO_RESIZE,(videoresize)?MF_CHECKED:MF_UNCHECKED);
                                if (videoresize) SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW|WS_VISIBLE);
                                else             SetWindowLong(hwnd, GWL_STYLE, (WS_OVERLAPPEDWINDOW&~WS_SIZEBOX&~WS_THICKFRAME&~WS_MAXIMIZEBOX)|WS_VISIBLE);
                                GetWindowRect(hwnd,&rect);
                                SetWindowPos(hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
                                updatewindowsize(640,512);
                        }
                        break;
                        case IDM_VIDEO_FULLSCREEN:
                        gotofullscreen=1;
                        break;
                        case IDM_VIDEO_RESIZE:
                        videoresize=!videoresize;
                        CheckMenuItem(hmenu,IDM_VIDEO_RESIZE,(videoresize)?MF_CHECKED:MF_UNCHECKED);
                        if (videoresize) SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW|WS_VISIBLE);
                        else             SetWindowLong(hwnd, GWL_STYLE, (WS_OVERLAPPEDWINDOW&~WS_SIZEBOX&~WS_THICKFRAME&~WS_MAXIMIZEBOX)|WS_VISIBLE);
                        GetWindowRect(hwnd,&rect);
                        SetWindowPos(hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
                        updatewindowsize(640,512);
                        break;
                        case IDM_SPD_NORMAL:
                        case IDM_SPD_FAST:
                        case IDM_SPD_REALLY:
                        tapespeed=wParam-IDM_SPD_NORMAL;
                        CheckMenuItem(ghmenu,IDM_SPD_NORMAL,(tapespeed==0)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_SPD_FAST,  (tapespeed==1)?MF_CHECKED:MF_UNCHECKED);
                        CheckMenuItem(ghmenu,IDM_SPD_REALLY,(tapespeed==2)?MF_CHECKED:MF_UNCHECKED);
                        break;
                        case IDM_OTHER_RDKEY:
                        redefinekeys();
                        break;
                        case IDM_OTHER_KEYDF:
                        for (c=0;c<128;c++) keylookup[c]=c;
                        break;

                        case IDM_SOUND_DDNOISE:
                        sndddnoise=!sndddnoise;
                        CheckMenuItem(hmenu,IDM_SOUND_DDNOISE,(sndddnoise)?MF_CHECKED:MF_UNCHECKED);
                        break;
                        case IDM_SOUND_TNOISE:
                        sndtape=!sndtape;
                        CheckMenuItem(hmenu,IDM_SOUND_TNOISE,(sndtape)?MF_CHECKED:MF_UNCHECKED);
                        break;

                        case IDM_DDV_33: case IDM_DDV_66: case IDM_DDV_100:
                        CheckMenuItem(hmenu,(IDM_DDV_33+ddvol)-1,MF_UNCHECKED);
                        ddvol=(LOWORD(wParam)-IDM_DDV_33)+1;
                        CheckMenuItem(hmenu,(IDM_DDV_33+ddvol)-1,MF_CHECKED);
                        break;

                        case IDM_DDT_525: case IDM_DDT_35:
                        CheckMenuItem(hmenu,IDM_DDT_525+ddtype,MF_UNCHECKED);
                        ddtype=LOWORD(wParam)-IDM_DDT_525;
                        CheckMenuItem(hmenu,IDM_DDT_525+ddtype,MF_CHECKED);
                        closeddnoise();
                        loaddiscsamps();
                        break;

                        case IDM_SCRSHOT:
                        EnterCriticalSection(&cs);
                        if (!getfn(hwnd,"Bitmap (*.BMP)\0*.BMP\0All files (*.*)\0*.*\0\0",scrshotname,1,"BMP"))
                           savescrshot(scrshotname);
                        cleardrawit();
                        infocus=1;
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_DEBUGGER:
                        EnterCriticalSection(&cs);
                        rest(200);
                        if (!debugon)
                        {
                                debug=debugon=1;
                                startdebug();
                                EnableMenuItem(hmenu,IDM_BREAK,MF_ENABLED);
                        }
                        else
                        {
                                debug^=1;
                                enddebug();
                                EnableMenuItem(hmenu,IDM_BREAK,MF_GRAYED);
                        }
                        CheckMenuItem(hmenu,IDM_DEBUGGER,(debug)?MF_CHECKED:MF_UNCHECKED);
                        LeaveCriticalSection(&cs);
                        break;
                        case IDM_BREAK:
                        debug=1;
                        break;
                }
                return 0;
                case WM_SETFOCUS:
                infocus=1;
                for (c=0;c<128;c++) key[c]=0;
                break;
                case WM_KILLFOCUS:
                infocus=0;
                for (c=0;c<128;c++) key[c]=0;
                break;
                case WM_DESTROY:
                PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
                break;

        	case WM_SYSKEYDOWN:
        	case WM_KEYDOWN:
                if (LOWORD(wParam)!=255)
                {
                        rpclog("Key %04X %04X\n",LOWORD(wParam),VK_LEFT);
                        c=MapVirtualKey(LOWORD(wParam),0);
                        c=hw_to_mycode[c];
                        rpclog("MVK %i %i %i\n",c,hw_to_mycode[c],KEY_PGUP);
                        if (LOWORD(wParam)==VK_LEFT)   c=KEY_LEFT;
                        if (LOWORD(wParam)==VK_RIGHT)  c=KEY_RIGHT;
                        if (LOWORD(wParam)==VK_UP)     c=KEY_UP;
                        if (LOWORD(wParam)==VK_DOWN)   c=KEY_DOWN;
                        if (LOWORD(wParam)==VK_HOME)   c=KEY_HOME;
                        if (LOWORD(wParam)==VK_END)    c=KEY_END;
                        if (LOWORD(wParam)==VK_INSERT) c=KEY_INSERT;
                        if (LOWORD(wParam)==VK_DELETE) c=KEY_DEL;
                        if (LOWORD(wParam)==VK_PRIOR)  c=KEY_PGUP;
                        if (LOWORD(wParam)==VK_NEXT)   c=KEY_PGDN;
                        rpclog("MVK2 %i %i %i\n",c,hw_to_mycode[c],KEY_PGUP);
                        key[c]=1;
                }
                break;
        	case WM_SYSKEYUP:
        	case WM_KEYUP:
                if (LOWORD(wParam)!=255)
                {
//                        rpclog("Key %04X %04X\n",LOWORD(wParam),VK_LEFT);
                        c=MapVirtualKey(LOWORD(wParam),0);
                        c=hw_to_mycode[c];
                        if (LOWORD(wParam)==VK_LEFT)   c=KEY_LEFT;
                        if (LOWORD(wParam)==VK_RIGHT)  c=KEY_RIGHT;
                        if (LOWORD(wParam)==VK_UP)     c=KEY_UP;
                        if (LOWORD(wParam)==VK_DOWN)   c=KEY_DOWN;
                        if (LOWORD(wParam)==VK_HOME)   c=KEY_HOME;
                        if (LOWORD(wParam)==VK_END)    c=KEY_END;
                        if (LOWORD(wParam)==VK_INSERT) c=KEY_INSERT;
                        if (LOWORD(wParam)==VK_DELETE) c=KEY_DEL;
                        if (LOWORD(wParam)==VK_PRIOR)  c=KEY_PGUP;
                        if (LOWORD(wParam)==VK_NEXT)   c=KEY_PGDN;
//                        rpclog("MVK %i\n",c);
                        key[c]=0;
                }
                break;

                case WM_ENTERMENULOOP:
                EnterCriticalSection(&cs);
                for (c=0;c<128;c++) key[c]=0;
                break;
                case WM_EXITMENULOOP:
                LeaveCriticalSection(&cs);
                for (c=0;c<128;c++) key[c]=0;
                break;

                case WM_SIZE:
                winsizex=lParam&0xFFFF;
                winsizey=lParam>>16;
                break;

                default:                      /* for messages that we don't deal with */
                return DefWindowProc (hwnd, message, wParam, lParam);
        }
        return 0;
}

#endif
