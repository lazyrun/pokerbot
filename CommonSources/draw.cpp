static HWND hWnd;
static HDC hDc;
static HFONT hFont;

void MainWindow::timerEvent(QTimerEvent *)
{
   RECT rect;
   SetRect(&rect, 100, 100, 700, 500);
   
   SetBkColor(hDc, RGB(0, 0, 0));
   ExtTextOut(hDc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);   

   HPEN hPen = CreatePen(PS_DOT, 5, RGB(255, 0, 0)); //Создаётся объект
   SelectObject(hDc, hPen); //Объект делается текущим   

   MoveToEx(hDc, 100, 100, NULL); //сделать текущими координаты x1, y1
   LineTo(hDc, 700, 100);   
   
   MoveToEx(hDc, 700, 100, NULL); //сделать текущими координаты x1, y1
   LineTo(hDc, 700, 500);   

   MoveToEx(hDc, 700, 500, NULL); //сделать текущими координаты x1, y1
   LineTo(hDc, 100, 500);

   MoveToEx(hDc, 100, 500, NULL); //сделать текущими координаты x1, y1
   LineTo(hDc, 100, 100);

   //HBRUSH hBrush;
   //hBrush=CreateSolidBrush(RGB(255,0,0));
   //SelectObject(hDc, hBrush);
   //FrameRect(hDc, &rect, hBrush);
   //Rectangle(hDc, rect.left, rect.top, 
   //    rect.right, rect.bottom);
   
   SelectObject(hDc, hFont);
   DrawText(hDc, L"Ac Qs", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
   RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
}

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags)
{
   hWnd = GetDesktopWindow();
   hDc = GetWindowDC(hWnd);
   hFont = CreateFont(48,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
      OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, 
      VARIABLE_PITCH,TEXT("Impact"));
   //SelectObject(hDc, hFont);
   SetTextColor(hDc, RGB(255,0,0));
   //SetBkMode(hDc, TRANSPARENT);
   //ReleaseDC(hWnd, hDc);   
   startTimer(500);

   //RECT rcBmp;
   //SetRect(&rcBmp, 200, 200, 700, 500);

   //PAINTSTRUCT ps;  
   //BeginPaint(hWnd, &ps); 
   //Rectangle(ps.hdc, rcBmp.left, rcBmp.top, 
   //    rcBmp.right, rcBmp.bottom); 
   //StretchBlt(ps.hdc, rcBmp.left + 1, rcBmp.top + 1, 
   //    (rcBmp.right - rcBmp.left) - 2, 
   //    (rcBmp.bottom - rcBmp.top) - 2, hdcCompat, 
   //    0, 0, 32, 32, SRCCOPY); 
   //EndPaint(hWnd, &ps); 

   //hFont = CreateFont(48,0,0,0,FW_DONTCARE,FALSE,TRUE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
   //CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Impact"));
   //SelectObject(hDc, hFont);
   //RECT rect;
   //SetRect(&rect, 100,100,700,200);
   //SetTextColor(hDc, RGB(255,0,0));
   //SetBkMode(hDc, TRANSPARENT);

   //DrawText(hDc, L"TEST", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
   //for(int i = 0;i < 100; i++)
   //{
   //   DrawText(hDc, L"TEST", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
   //   Sleep(100);
   //}
   //RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
