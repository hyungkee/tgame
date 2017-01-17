#include <windows.h>
#include <time.h>
#include <math.h>
#include <mmsystem.h>

#define HEIGHT 13
#define WIDTH 10

#pragma comment(lib,"winmm.lib")


int SHEIGHT;
int SWIDTH;
int MX=0x7fffffff,MY=0x7fffffff;
int  X=0x7fffffff, Y=0x7fffffff;

RECT crt;

int data[HEIGHT][WIDTH]={0};// 0:Back, 1:Image, 2:Start, 3:Replay
double menu=0;// 0:Start, 1:game, 2:Replay
double mdelta=0.01;
int CurrentLine=-1;
int CurrentStep=-1;
int OldStep=-1;
int CurrentNumber=3;
int sw=1;
int Cut=0;
int Stop=0;

HBITMAP  Image=(HBITMAP)LoadImage(NULL,"image\\Node.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP sImage=(HBITMAP)LoadImage(NULL,"image\\sNode.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP lImage=(HBITMAP)LoadImage(NULL,"image\\lNode.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP NImage=(HBITMAP)LoadImage(NULL,"image\\NNode.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP SImage=(HBITMAP)LoadImage(NULL,"image\\Start.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP RImage=(HBITMAP)LoadImage(NULL,"image\\R.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP Image1=(HBITMAP)LoadImage(NULL,"image\\1.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP Image2=(HBITMAP)LoadImage(NULL,"image\\2.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);
HBITMAP Image3=(HBITMAP)LoadImage(NULL,"image\\3.bmp",NULL,NULL,NULL,LR_LOADFROMFILE);

				int Temp=0;
				int queue[100]={0};
				int Line=CurrentLine;

void Start(){
	if(X!=0x7fffffff && Y!=0x7fffffff){
		menu+=mdelta;
	}
	X=0x7fffffff, Y=0x7fffffff;
}

void Replay(){
	Sleep(50);
	int i=0,L=HEIGHT-1,j;
	for(i=HEIGHT-1;i>0;i--)
		if(data[i][0]==2)
			L=i-1;
	for(j=0;j<WIDTH;j++)
		data[L][j]=2;

	if(L==0 && X!=0x7fffffff && Y!=0x7fffffff){
		menu=3;
	}

	X=0x7fffffff, Y=0x7fffffff;

}

void Back(){
	Sleep(50);
	int i=0,L=0,j;
	for(i=0;i<HEIGHT-1;i++)
		if(data[i][0]==2)
			break;
	L=i;
	for(j=0;j<WIDTH;j++)
		data[L][j]=0;

	if(L==HEIGHT-1){
		Cut=0;
		Stop=0;
		CurrentLine=-1;
		CurrentStep=-1;
		OldStep=-1;
		CurrentNumber=3;
		sw=1;

		menu=1;
	}
}

void Process(HWND hWnd){
	int i;

	if(Stop==0){

		if(CurrentNumber==0){
			menu=2;
		}


		if(CurrentLine<4){
			Sleep(60);
		}else if(CurrentLine<7){
			Sleep(50);
		}else if(CurrentLine<10){
			Sleep(40);
		}else{
			Sleep(30);
		}

		if(Cut==1){
			if(CurrentLine==HEIGHT-1){//RMK
				for(int i=0;i<WIDTH;i++)	if(data[CurrentLine][i]==1)	OldStep=CurrentStep-sw,CurrentStep=i;

				Temp=0;
				memset(queue,0,sizeof(queue));
				Line=CurrentLine;
				for(i=0;i<=WIDTH;i++){
					if(data[CurrentLine][i]==1 && data[CurrentLine-1][i]==0){
						data[CurrentLine][i]=0,queue[Temp++]=i;
					}
				}

				if(CurrentNumber-Temp>0){
					if(data[CurrentLine][CurrentStep]==1)	OldStep=CurrentStep, CurrentStep+=sw;
					for(i=0;i<WIDTH;i++)	data[CurrentLine][i]=0;
					data[CurrentLine][CurrentStep]=1;
					Stop=1;
					{
						Temp=0;
						memset(queue,0,sizeof(queue));
						Line=CurrentLine;
						for(i=0;i<=WIDTH;i++){
							if(data[CurrentLine][i]==1 && data[CurrentLine-1][i]==0){
								data[CurrentLine][i]=0,queue[Temp++]=i;
							}
						}
					}
					Cut=0;
					CurrentNumber=0;
				}
				return;
			}


			if(CurrentLine>=1){
				Temp=0;
				memset(queue,0,sizeof(queue));
				Line=CurrentLine;
				for(int i=0;i<=WIDTH;i++){
					if(data[CurrentLine][i]==1 && data[CurrentLine-1][i]==0){
						data[CurrentLine][i]=0,queue[Temp++]=i;
					}
				}
				if(Temp!=0)	Stop=1;
				CurrentNumber-=Temp;
				CurrentStep=-1;
				OldStep=-1;
				Cut=0;
				return;
			}
			CurrentStep=-1;
			OldStep=-1;
			Cut=0;
		}

		if(CurrentStep==-1){
			CurrentLine++;
			if(CurrentLine==4 && CurrentNumber==3)	CurrentNumber=2;
			if(CurrentLine==7 && CurrentNumber==2)	CurrentNumber=1;
			srand((unsigned long)time(NULL));
			CurrentStep=rand()%(WIDTH-CurrentNumber);
			sw=1;
			if(CurrentLine!=0)	Sleep(500);
		}

		int i;

		for(i=0;i<WIDTH;i++)	data[CurrentLine][i]=0;
		for(i=0;i<CurrentNumber;i++)	data[CurrentLine][CurrentStep+i]=1;

		OldStep=CurrentStep;
		CurrentStep+=sw;

		char SoundName[100];
		wsprintf(SoundName,"Sound\\%d.wav",CurrentNumber);
		if(CurrentStep==WIDTH-CurrentNumber){
			sw=-1;
			PlaySound(TEXT(SoundName),NULL,SND_ASYNC);
		}
		if(CurrentStep==0){
			sw=1;
			PlaySound(TEXT(SoundName),NULL,SND_ASYNC);
		}
		
	}else{
		int i;
		for(i=0;i<Temp;i++){
			if(queue[i]!=-1){
				if(data[Line-1][queue[i]]==0){
					data[Line][queue[i]]=0;
					data[Line-1][queue[i]]=1;
				}else{
					queue[i]=-1;
				}
			}
		}
		Line--;
		Sleep(100);
		if(Line<=0)	Stop=0;
	}
}

void MSG_PAINT(HWND hWnd)
{
	char LStr[128];

	PAINTSTRUCT ps;
	HDC hdc, hMemDC;
	HBITMAP hBitmap, OldBitmap;
	HBRUSH BGBrush;
    HFONT Font, OldFont;

	hdc = BeginPaint(hWnd,&ps);
	hMemDC = CreateCompatibleDC(hdc);
	hBitmap = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
	OldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	Font = CreateFont(13,0,0,0,400,0,0,0,HANGEUL_CHARSET,0,0,0,0,TEXT("돋움체"));
	OldFont = (HFONT)SelectObject(hMemDC,Font);
	SetBkMode(hMemDC, 1);
//	SetTextColor(hMemDC, 0x000000);	// 폰트색
//	SetBkColor(hMemDC, 0xffffff);	// 배경색

		BGBrush = CreateSolidBrush(0xffffff);
		FillRect(hMemDC, &crt, BGBrush);
		DeleteObject(BGBrush);

		wsprintf(LStr,"%d, %d",MX,MY);
		TextOutA(hMemDC,10,10,LStr,lstrlenA(LStr));

	int i,j,max=0,mi,mj,Size;
	for(i=0;i<HEIGHT;i++){
		for(j=0;j<WIDTH;j++){
			HDC MemDC;
			MemDC=CreateCompatibleDC(hMemDC);
			HBITMAP OldBitmap2;
			if(fabs(menu)<mdelta/2)		OldBitmap2=(HBITMAP)SelectObject(MemDC,SImage), Size=100;
			else if(fabs(menu-1)<mdelta/2){
				if(data[HEIGHT-i-1][j]==0){
					if(HEIGHT-i-1<4)			OldBitmap2=(HBITMAP)SelectObject(MemDC,Image), Size=50;
					else if(HEIGHT-i-1<7)	OldBitmap2=(HBITMAP)SelectObject(MemDC,sImage), Size=50;
					else			OldBitmap2=(HBITMAP)SelectObject(MemDC,lImage), Size=50;
				}
				if(data[HEIGHT-i-1][j]==1)	OldBitmap2=(HBITMAP)SelectObject(MemDC,NImage), Size=50;
			}else if(fabs(menu-2)<mdelta/2 || fabs(menu-3)<mdelta/2){
				if(data[HEIGHT-i-1][j]==0){
					if(HEIGHT-i-1<4)			OldBitmap2=(HBITMAP)SelectObject(MemDC,Image), Size=50;
					else if(HEIGHT-i-1<7)	OldBitmap2=(HBITMAP)SelectObject(MemDC,sImage), Size=50;
					else			OldBitmap2=(HBITMAP)SelectObject(MemDC,lImage), Size=50;
				}
				if(data[HEIGHT-i-1][j]==1)	OldBitmap2=(HBITMAP)SelectObject(MemDC,NImage), Size=50;
				if(data[HEIGHT-i-1][j]==2)	OldBitmap2=(HBITMAP)SelectObject(MemDC,RImage), Size=100;
			}else{
				if(menu<0.3)		OldBitmap2=(HBITMAP)SelectObject(MemDC,Image3), Size=100;
				else if(menu<0.6)	OldBitmap2=(HBITMAP)SelectObject(MemDC,Image2), Size=100;
				else				OldBitmap2=(HBITMAP)SelectObject(MemDC,Image1), Size=100;
			}
			int BlockDelta=1000/sqrt(1+(j*55+(SWIDTH-545)/2+25-MX)*(j*55+(SWIDTH-545)/2+25-MX)+(i*55+25-MY)*(i*55+25-MY));
			if(max<BlockDelta)	max=BlockDelta,mi=i,mj=j;
			BlockDelta=min(40,BlockDelta);
			if(fabs(menu-1)<mdelta/2)	BlockDelta=0;
			StretchBlt(hMemDC,j*55+(SWIDTH-545)/2-BlockDelta/2,i*55-BlockDelta/2,50+BlockDelta,50+BlockDelta,MemDC,0,0,Size,Size,SRCCOPY);
			DeleteObject(OldBitmap2);
			DeleteDC(MemDC);
		}
	}

	i=mi;j=mj;
	{
			HDC MemDC;
			MemDC=CreateCompatibleDC(hMemDC);
			HBITMAP OldBitmap2;
			if(fabs(menu)<mdelta/2)		OldBitmap2=(HBITMAP)SelectObject(MemDC,SImage), Size=100;
			else if(fabs(menu-1)<mdelta/2){
				if(data[HEIGHT-i-1][j]==0){
					if(HEIGHT-i-1<4)			OldBitmap2=(HBITMAP)SelectObject(MemDC,Image), Size=50;
					else if(HEIGHT-i-1<7)	OldBitmap2=(HBITMAP)SelectObject(MemDC,sImage), Size=50;
					else			OldBitmap2=(HBITMAP)SelectObject(MemDC,lImage), Size=50;
				}
				if(data[HEIGHT-i-1][j]==1)	OldBitmap2=(HBITMAP)SelectObject(MemDC,NImage), Size=50;
			}else if(fabs(menu-2)<mdelta/2 || fabs(menu-3)<mdelta/2){
				if(data[HEIGHT-i-1][j]==0)	OldBitmap2=(HBITMAP)SelectObject(MemDC,Image), Size=50;
				if(data[HEIGHT-i-1][j]==1)	OldBitmap2=(HBITMAP)SelectObject(MemDC,NImage), Size=50;
				if(data[HEIGHT-i-1][j]==2)	OldBitmap2=(HBITMAP)SelectObject(MemDC,RImage), Size=100;
			}else{
				if(menu<0.3)		OldBitmap2=(HBITMAP)SelectObject(MemDC,Image3), Size=100;
				else if(menu<0.6)	OldBitmap2=(HBITMAP)SelectObject(MemDC,Image2), Size=100;
				else				OldBitmap2=(HBITMAP)SelectObject(MemDC,Image1), Size=100;
			}
//			BitBlt(hMemDC,j*55+(SWIDTH-545)/2,i*55,50,50,MemDC,0,0,SRCCOPY);
			int BlockDelta=1000/sqrt(1+(j*55+(SWIDTH-545)/2+25-MX)*(j*55+(SWIDTH-545)/2+25-MX)+(i*55+25-MY)*(i*55+25-MY));
			BlockDelta=min(40,BlockDelta);
			if(fabs(menu-1)<mdelta/2)	BlockDelta=0;
			StretchBlt(hMemDC,j*55+(SWIDTH-545)/2-BlockDelta/2,i*55-BlockDelta/2,50+BlockDelta,50+BlockDelta,MemDC,0,0,Size,Size,SRCCOPY);
			DeleteObject(OldBitmap2);
			DeleteDC(MemDC);
	}



	BitBlt(hdc, 0, 0, crt.right, crt.bottom, hMemDC, 0, 0, SRCCOPY);
	DeleteObject(SelectObject(hMemDC, OldFont));
	DeleteObject(SelectObject(hMemDC, OldBitmap));
	DeleteDC(hMemDC);
	EndPaint(hWnd,&ps);
}


void SmartRuncher(HWND hWnd, int Cut){

	if(fabs(menu)<mdelta/2)	Start();
	else if(fabs(menu-1)<mdelta/2){
		if(Cut==1 && 0<=CurrentLine && CurrentLine<HEIGHT){
			if(CurrentStep!=-1 && OldStep!=-1){
				CurrentStep=OldStep;
				for(int i=0;i<WIDTH;i++)	data[CurrentLine][i]=0;
				for(int i=0;i<CurrentNumber;i++)	data[CurrentLine][CurrentStep+i]=1;
			}
		}
		Process(hWnd);
	}
	else if(fabs(menu-2)<mdelta/2)	Replay();
	else if(fabs(menu-3)<mdelta/2)	Back();
	else menu+=mdelta;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch(iMessage) {
	case WM_PAINT:		MSG_PAINT(hWnd);						break;
	case WM_KEYDOWN:
						switch(wParam){
						case VK_SPACE:
							Cut=1;
							PlaySound(TEXT("Sound\\HO.wav"),NULL,SND_ASYNC);
//							SmartRuncher(hWnd,Cut);
							X=-0x7fffffff;
							Y=-0x7fffffff;
							break;
						case VK_RETURN:
							Cut=1;
							PlaySound(TEXT("Sound\\HO.wav"),NULL,SND_ASYNC);
//							SmartRuncher(hWnd,Cut);
							X=-0x7fffffff;
							Y=-0x7fffffff;
							break;
						case VK_ESCAPE:
							PostQuitMessage(0);
							break;
						}
						break;
	case WM_TIMER:
						GetClientRect(hWnd, &crt);
						SWIDTH=crt.right-crt.left;
						SHEIGHT=crt.bottom-crt.top;
						SmartRuncher(hWnd,Cut);

						InvalidateRect(hWnd, NULL, FALSE);
						break;
	case WM_LBUTTONDOWN:
						X=LOWORD(lParam);
						Y=HIWORD(lParam);
						break;
	case WM_MOUSEMOVE:
						MX=LOWORD(lParam);
						MY=HIWORD(lParam);
						break;
	case WM_CREATE:		GetClientRect(hWnd, &crt);
						SetTimer(hWnd,0,1,NULL);			break;
	case WM_DESTROY:	PostQuitMessage(0);					break;
	}
	return DefWindowProc( hWnd, iMessage, wParam, lParam );
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow )
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = CreateSolidBrush(RGB( 255,255,255 ) );
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = ( WNDPROC )WndProc;
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = TEXT( "TEST" );
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass( &WndClass );

	hWnd = CreateWindow( TEXT( "TEST" ), TEXT( "C++ Exemple" ),
	WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT,
	800, 800, NULL, (HMENU)NULL, hInstance, NULL );

	ShowWindow( hWnd, nCmdShow );
 
	while( GetMessage( &Message, NULL, 0, 0 ) ) {
		TranslateMessage( &Message );
		DispatchMessage( &Message );
	}
	return (int)Message.wParam;
}