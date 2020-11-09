
// DoralidarGUISampleView.cpp : DoralidarGUISampleView 类的实现
//

#include "stdafx.h"
#include <atltypes.h>
#include <cmath>
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DoralidarGUISample.h"
#endif

#include "DoralidarGUISampleDoc.h"
#include "DoralidarGUISampleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// DoralidarGUISampleView

IMPLEMENT_DYNCREATE(DoralidarGUISampleView, CView)

BEGIN_MESSAGE_MAP(DoralidarGUISampleView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &DoralidarGUISampleView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_BN_CONNECT, &DoralidarGUISampleView::OnBnConnect)
	ON_COMMAND(ID_BN_DISCONNECT, &DoralidarGUISampleView::OnBnDisconnect)
	ON_COMMAND(ID_BN_START, &DoralidarGUISampleView::OnBnStart)
	ON_COMMAND(ID_BN_ZOOMOUT, &DoralidarGUISampleView::OnBnZoomout)
	ON_COMMAND(ID_BN_ZOOMIN, &DoralidarGUISampleView::OnBnZoomin)
	ON_UPDATE_COMMAND_UI(ID_BN_CONNECT, &DoralidarGUISampleView::OnUpdateBnConnect)
	ON_UPDATE_COMMAND_UI(ID_BN_DISCONNECT, &DoralidarGUISampleView::OnUpdateBnDisconnect)
	ON_UPDATE_COMMAND_UI(ID_BN_START, &DoralidarGUISampleView::OnUpdateBnStart)
	ON_COMMAND(ID_BN_STOP, &DoralidarGUISampleView::OnBnStop)
	ON_UPDATE_COMMAND_UI(ID_BN_STOP, &DoralidarGUISampleView::OnUpdateBnStop)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_BN_MOVE, &DoralidarGUISampleView::OnBnMove)
	ON_UPDATE_COMMAND_UI(ID_BN_MOVE, &DoralidarGUISampleView::OnUpdateBnMove)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_BN_SELECT, &DoralidarGUISampleView::OnBnSelect)
	ON_UPDATE_COMMAND_UI(ID_BN_SELECT, &DoralidarGUISampleView::OnUpdateBnSelect)
	ON_COMMAND(ID_BN_SHOWALL, &DoralidarGUISampleView::OnBnShowall)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// DoralidarGUISampleView 构造/析构

DoralidarGUISampleView::DoralidarGUISampleView()
{
	// TODO: 在此处添加构造代码
	iGridWidth = 50;
	iGridWidthReal = 200;
	dScale = 0.0;
	iCommandType = 0;
	isMove=false;
	isLeftButtonDown =false;
	mTimer = 0;
	iDataPointCount = 811;
	ptScan = new POINT[iDataPointCount];
	ptScreen = new POINT[iDataPointCount + 1];
	isHighPrecision = false;
	dAngResolution = 1.0/3;
	iMaxDistance = 10000;
	for (int i = 0; i < iDataPointCount; i++)
	{
		ptScan[i].x = 1000 * cos((i*dAngResolution + startAngle)*PI / 180);
		ptScan[i].y = 1000 * sin((i*dAngResolution + startAngle)*PI / 180);
	}
	m_pdcMemory = new CDC;
	m_pBitMap =new CBitmap;
}

DoralidarGUISampleView::~DoralidarGUISampleView()
{
	delete m_pdcMemory;
	delete m_pBitMap;
}

BOOL DoralidarGUISampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// DoralidarGUISampleView 绘制

void DoralidarGUISampleView::OnDraw(CDC* pDC)
{
	DoralidarGUISampleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	dScale = iGridWidthReal*1.0/iGridWidth;
	RECT cvClient;
	this->GetClientRect(&cvClient);
	//iCenterX = (cvClient.left+cvClient.right)/2;
	//iCenterY = (cvClient.bottom-200);
	COLORREF clorPen =RGB(0,0,0);
	CPen pen(PS_DOT,1,clorPen);
	CPen* pOldPen;
	pOldPen = pDC->SelectObject(&pen);
	for(int i =0;i< iMaxDistance /iGridWidth;i++)
	{
		POINT pt;
		pt.x=0;
		pt.y = iCenterY+i*iGridWidth;
		pDC->MoveTo(pt);
		pDC->LineTo(cvClient.right,pt.y);


		pt.x=0;
		pt.y=iCenterY-i*iGridWidth;
		pDC->MoveTo(pt);
		pDC->LineTo(cvClient.right,pt.y);
		CString str;
		int iCur = i*iGridWidthReal;
		str.Format(_T("%d"),iCur);
		CRect rec(iCenterX+1,iCenterY-i*iGridWidth-15,iCenterX+200,iCenterY-i*iGridWidth);
		pDC->DrawText(str,&rec,DT_MODIFYSTRING);


		pt.x=iCenterX+i*iGridWidth;
		pt.y=0;
		pDC->MoveTo(pt);
		pDC->LineTo(pt.x,cvClient.bottom);
		str.Format(_T("%d"),i*iGridWidthReal);
		rec.left=pt.x;
		rec.right=pt.x+200;
		rec.top = iCenterY+2;
		rec.bottom = rec.top+20;
		pDC->DrawText(str,&rec,DT_MODIFYSTRING);


		pt.x=iCenterX-i*iGridWidth;
		pDC->MoveTo(pt);
		pDC->LineTo(pt.x,cvClient.bottom);
		rec.left=pt.x;
		rec.right=pt.x+200;
		rec.top= iCenterY+2;
		rec.bottom=iCenterY+20;
		pDC->DrawText(str,&rec,DT_MODIFYSTRING);
	}
	CBrush RedBrush(RGB(0,0,0));
	CBrush* pOldBrush;
	pOldBrush=pDC->SelectObject(&RedBrush); 
	CRect reDead(iCenterX-5/dScale,iCenterY-5/dScale,iCenterX+5/dScale,iCenterY+5/dScale);
	pDC->Ellipse(reDead);
	//pDC->SelectObject(pOldBrush);
	pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));				//绘制扫描线时画笔、画刷设置

	clorPen =RGB(255,0,0);
	CPen pRed(PS_SOLID,1,clorPen);
	pDC->SelectObject(pRed);
	DrawScanPoint(pDC,ptScan);

	POINT ptStart,ptEnd,ptLeftTop,ptRightBottom;
	ptLeftTop.x = iMaxDistance *cos(135*PI/180);
	ptLeftTop.y = iMaxDistance *sin(135*PI/180);
	ptLeftTop = PhysicsToScreen(ptLeftTop);
	ptRightBottom.x = iMaxDistance *cos(-45*PI/180);
	ptRightBottom.y = iMaxDistance *sin(-45*PI/180);
	ptRightBottom = PhysicsToScreen(ptRightBottom);
	CRect rt(ptLeftTop,ptRightBottom);
	ptStart.x = (int)(iMaxDistance*cos(-45*PI/180));
	ptStart.y =(int)(iMaxDistance*sin(-45*PI/180));
	ptStart = PhysicsToScreen(ptStart);
	ptEnd.x = (int)(iMaxDistance*cos(225*PI/180));
	ptEnd.y = (int)(iMaxDistance*sin(225*PI/180));
	ptEnd = PhysicsToScreen(ptEnd);
	/*pDC->MoveTo(iCenterX,iCenterY);
	pDC->LineTo(ptStart);
	pDC->MoveTo(iCenterX,iCenterY);*/
	//pDC->LineTo(ptEnd);
	pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(DKGRAY_BRUSH)));
	pDC->Pie(rt,ptEnd,ptStart);

	pDC->SelectObject(pOldBrush);						//绘制完毕恢复画刷、画笔
	pDC->SelectObject(pOldPen);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// DoralidarGUISampleView 打印


void DoralidarGUISampleView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL DoralidarGUISampleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void DoralidarGUISampleView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void DoralidarGUISampleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void DoralidarGUISampleView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void DoralidarGUISampleView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// DoralidarGUISampleView 诊断

#ifdef _DEBUG
void DoralidarGUISampleView::AssertValid() const
{
	CView::AssertValid();
}

void DoralidarGUISampleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

DoralidarGUISampleDoc* DoralidarGUISampleView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(DoralidarGUISampleDoc)));
	return (DoralidarGUISampleDoc*)m_pDocument;
}
#endif //_DEBUG


// DoralidarGUISampleView 消息处理程序


void DoralidarGUISampleView::OnBnConnect()
{
	// TODO: 在此添加命令处理程序代码
	isBreak =false;
	iCommandType =1;
	lsDriver.open("192.168.0.10",2112);
	//lsDriver.set_scanning_parameters(0,2160);
	//mTimer = SetTimer(1,50,0);
}


POINT DoralidarGUISampleView::PhysicsToScreen(POINT pt)
{
	POINT ptReturn;
	ptReturn.x =(pt.x/dScale+iCenterX);
	ptReturn.y =(-pt.y/dScale+iCenterY);
	return ptReturn;
}


void DoralidarGUISampleView::DrawScanPoint(CDC* pd,POINT pt[])
{
	ptScreen[0].x=iCenterX;
	ptScreen[0].y=iCenterY;
	for(int i=0;i< iDataPointCount;i++)
	{
		ptScreen[i+1]=PhysicsToScreen(pt[i]);
		pd->Rectangle(ptScreen[i+1].x-1,ptScreen[i+1].y-1,ptScreen[i+1].x+1,ptScreen[i+1].y+1);
	}
	//pd->LineTo()
	/*ptScreen[pointNumber+1].x=iCenterX;
	ptScreen[pointNumber+1].y=iCenterY;
	pd->Polygon(ptScreen,pointNumber);*/
}
UINT __cdecl ThreadB1(LPVOID lpParam)
{
	DoralidarGUISampleView* mView = (DoralidarGUISampleView*)lpParam;
	vector<int> vcData;
	bool bFirstRead = true;
	while(!mView->isBreak)
	{
		vcData.clear();
		if (mView->lsDriver.get_distance(vcData))
		{
			if (bFirstRead)
			{
				bFirstRead = false;
				mView->isHighPrecision = mView->lsDriver.is_high_precision();
				mView->dAngResolution = mView->lsDriver.get_angular_resolution();
				mView->iDataPointCount = (270 / mView->dAngResolution + 1);
				mView->ptScan = new POINT[mView->iDataPointCount];
				mView->ptScreen = new POINT[mView->iDataPointCount + 1];
				if (mView->isHighPrecision)
				{
					mView->iMaxDistance = 100000;
				}
				else
				{
					mView->iMaxDistance = 10000;
				}
			}
			for (int i = 0; i < mView->iDataPointCount; i++)
			{
				mView->ptScan[i].x = vcData[i] * cos((i*mView->dAngResolution - 45)*PI / 180);
				mView->ptScan[i].y = vcData[i] * sin((i*mView->dAngResolution - 45)*PI / 180);
			}
			/*if(iLoopNumber%2==0)
				mView->Invalidate();*/
		}
		else
		{
			mView->MessageBox(_T("通信故障！"));
			mView->isBreak = true;
			mView->iCommandType = 0;
			continue;
		}
	}
	return 1;
}

void DoralidarGUISampleView::OnBnDisconnect()
{
	// TODO: 在此添加命令处理程序代码
	isBreak =true;
	iCommandType =0;
	KillTimer(mTimer);
}


void DoralidarGUISampleView::OnBnStart()
{
	// TODO: 在此添加命令处理程序代码
	iCommandType =2;
	isBreak=false;
	lsDriver.start_measurement();
	mThread =AfxBeginThread(ThreadB1,this);
	mTimer = SetTimer(1,50,0);
}


void DoralidarGUISampleView::OnBnZoomout()
{
	// TODO: 在此添加命令处理程序代码
	iGridWidthReal *= 2;
	iGridWidthReal = (iGridWidthReal / 5) * 5;
	if (isHighPrecision)
	{
		if (iGridWidthReal > 10000)
		{
			iGridWidthReal = 10000;
		}
	}
	else
	{
		if (iGridWidthReal > 500)
		{
			iGridWidthReal = 500;
		}
	}
	this->Invalidate();
}


void DoralidarGUISampleView::OnBnZoomin()
{
	// TODO: 在此添加命令处理程序代码
	iGridWidthReal /= 2;
	iGridWidthReal = (iGridWidthReal / 5) * 5;
	if (isHighPrecision)
	{
		if (iGridWidthReal < 50)
		{
			iGridWidthReal = 50;
		}
	}
	else
	{
		if (iGridWidthReal < 5)
		{
			iGridWidthReal = 5;
		}
	}
	this->Invalidate();
}


void DoralidarGUISampleView::OnUpdateBnConnect(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(iCommandType==0);

}


void DoralidarGUISampleView::OnUpdateBnDisconnect(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(iCommandType!=0);
}


void DoralidarGUISampleView::OnUpdateBnStart(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(iCommandType==1);
}


void DoralidarGUISampleView::OnBnStop()
{
	// TODO: 在此添加命令处理程序代码
	isBreak = true;
	iCommandType = 1;
	KillTimer(mTimer);
}


void DoralidarGUISampleView::OnUpdateBnStop(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(iCommandType==2);
}


void DoralidarGUISampleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(isMove)
	{
		pointLast = point;
		isLeftButtonDown =true;
	}
		
	CView::OnLButtonDown(nFlags, point);
}


void DoralidarGUISampleView::OnBnMove()
{
	// TODO: 在此添加命令处理程序代码
	isMove =true;
	HCURSOR hand = LoadCursor(NULL,IDC_HAND);
	SetClassLongPtr(this->m_hWnd,GCLP_HCURSOR,(LONG)hand);
}


void DoralidarGUISampleView::OnUpdateBnMove(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}


void DoralidarGUISampleView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(isMove&&isLeftButtonDown)
	{
		int iOffetX = point.x-pointLast.x;
		int iOfferY = point.y-pointLast.y;
		iCenterX +=iOffetX;
		iCenterY +=iOfferY;
		pointLast = point;
		this->Invalidate();
	}
	CView::OnMouseMove(nFlags, point);
}


void DoralidarGUISampleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	isLeftButtonDown =false;
	CView::OnLButtonUp(nFlags, point);
}


void DoralidarGUISampleView::OnInitialUpdate()
{
	RECT cvClient;
	this->GetClientRect(&cvClient);
	iCenterX = (cvClient.left + cvClient.right) / 2;
	iCenterY = (cvClient.bottom - 200);
	CView::OnInitialUpdate();
	// TODO: 在此添加专用代码和/或调用基类
	if (m_pdcMemory->GetSafeHdc() == NULL)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc);
		CRect rectMax;
		dc.SetMapMode(MM_LOENGLISH);
		GetClientRect(&rectMax);
		m_pdcMemory->CreateCompatibleDC(&dc);
		m_pBitMap->CreateCompatibleBitmap(&dc, rectMax.Width(), rectMax.Height());
	}
}


void DoralidarGUISampleView::OnBnSelect()
{
	// TODO: 在此添加命令处理程序代码
	isMove =false;
	HCURSOR arrow = LoadCursor(NULL,IDC_ARROW);
	SetClassLongPtr(this->m_hWnd,GCLP_HCURSOR,(LONG)arrow);
}


void DoralidarGUISampleView::OnUpdateBnSelect(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}


void DoralidarGUISampleView::OnBnShowall()
{
	// TODO: 在此添加命令处理程序代码
	RECT cvClient;
	this->GetClientRect(&cvClient);
	iCenterX = (cvClient.left+cvClient.right)/2;
	iCenterY = (cvClient.bottom-200);
	this->Invalidate(false);
}


void DoralidarGUISampleView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnTimer(nIDEvent);
	this->Invalidate(false);
}


void DoralidarGUISampleView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CView::OnPaint()
	OnPrepareDC(&dc);
	CRect rectUpdate;
	GetClientRect(&rectUpdate);
	CBrush backgroundBrush((COLORREF) ::GetSysColor(COLOR_WINDOW));
	if (m_pBitMap->m_hObject != NULL)
	{
		CBitmap* pOldBitmap = m_pdcMemory->SelectObject(m_pBitMap);
		CBrush* pOldBrush = m_pdcMemory->SelectObject(&backgroundBrush);
		dc.DPtoLP(&rectUpdate);
		m_pdcMemory->PatBlt(rectUpdate.left, rectUpdate.top,
			rectUpdate.Width(), rectUpdate.Height(),
			PATCOPY);
		OnDraw(m_pdcMemory);
		dc.BitBlt(rectUpdate.left, rectUpdate.top,
			rectUpdate.Width(), rectUpdate.Height(),
			m_pdcMemory, rectUpdate.left, rectUpdate.top,
			SRCCOPY);
		m_pdcMemory->SelectObject(pOldBitmap);
		m_pdcMemory->SelectObject(pOldBrush);
	}

}


void DoralidarGUISampleView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_pBitMap->m_hObject!=NULL)
	{
		m_pBitMap->Detach();
		CRect rt;
		GetClientRect(&rt);
		CClientDC dc(this);
		OnPrepareDC(&dc);
		m_pBitMap->CreateCompatibleBitmap(&dc,rt.Width(),rt.Height());
	}
}
