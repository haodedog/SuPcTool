#include<windows.h>
#include<windowsx.h>
#include<strsafe.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("SuPcTool");
    //MessageBox(NULL, TEXT("作者正在努力制作哦，请稍等..."), TEXT("suPcTool"), MB_OK);
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("The exe need Windows NT !"), szAppName, MB_OK);
        return 0;
    }

    hwnd = CreateWindow(szAppName,
        TEXT("su电脑工具箱"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);


    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    //屏幕的宽度和高度
    static size_t xScreen, yScreen;
    
    //原本的字符信息
    UINT TextAlign;

    //显示器分辨率的字符串缓冲区
    static TCHAR DisplayResolutionBuffer[128];
    //固定欢迎语
    PTCHAR Welcome = (PTCHAR)TEXT("su电脑工具箱正在开发中...点击窗口任意位置即可禁用UAC...如果不成功请以管理员身份运行");
    //一个字符的高度，也是一行的高度
    static size_t LineInterval;

    // 这个变量用来保存显示器分辨率提示信息长度,然后用来保存欢迎语句的长度
    size_t iTarget;

    
    switch (message)
    {

    case WM_CREATE:
        //创建窗口的时候获取字符信息
        hdc = GetDC(hwnd);
        TEXTMETRIC tm;
        GetTextMetrics(hdc, &tm);
        LineInterval = tm.tmHeight;
        ReleaseDC(hwnd, hdc);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);

        //获取显示器的宽度和高度
        xScreen = GetSystemMetrics(SM_CXSCREEN);
        yScreen = GetSystemMetrics(SM_CYSCREEN);

        //先获取原本的字符显示模式
        TextAlign = GetTextAlign(hdc);

        //设置TextOut函数显示模式，基准点为中间
        SetTextAlign(hdc, TA_CENTER);

        //显示欢迎信息
        StringCchLength(Welcome, 1024, &iTarget);
        TextOut(hdc, rect.right / 2, rect.bottom / 2 - LineInterval, Welcome, iTarget);

        //把显示器信息写入字符数组缓冲区
        StringCchPrintf(DisplayResolutionBuffer, 128, TEXT("当前显示器分辨率是：%d * %d"), xScreen, yScreen);
        StringCchLength(DisplayResolutionBuffer, 128, &iTarget);

        //打印出显示器信息，打印到客户区正中间
        TextOut(hdc, rect.right / 2, rect.bottom / 2, DisplayResolutionBuffer, iTarget);
        //为了活命，这一句不要了
        //TextOut(hdc, rect.right / 2, rect.bottom / 2 - 2 * LineInterval, TEXT("高文汇，我希望你永远记得，我喜欢你"), 16);

        //把原本的文本显示模式还回去
        SetTextAlign(hdc, TextAlign);
        EndPaint(hwnd, &ps);
        return 0;
    case WM_LBUTTONUP:
        if (MessageBox(hwnd, TEXT("确定禁用UAC？"), TEXT("确认"), MB_YESNO) == IDYES)
        {
            system("reg add HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v EnableLUA /t REG_DWORD /d 0 /f");
        }
        return 0;
    case WM_CLOSE:
        //MessageBox(hwnd,TEXT("确认关闭suPcTool？"),  TEXT("确认") , MB_OK);
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}