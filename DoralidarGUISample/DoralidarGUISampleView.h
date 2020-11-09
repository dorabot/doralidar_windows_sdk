
// WRRadarGUISampleView.h : DoralidarGUISampleView 类的接口
//

#pragma once
#include "DoralidarDriver.h"
#define pointNumber 811
#define PI 3.1415926
#define angleResolution 1.0/3.0
#define startAngle -45
using namespace dl_ls;
UINT __cdecl ThreadB1(LPVOID lpParam); 
class DoralidarGUISampleView : public CView
{
protected: // 仅从序列化创建
	DoralidarGUISampleView();
	DECLARE_DYNCREATE(DoralidarGUISampleView)

// 特性
public:
	DoralidarGUISampleDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~DoralidarGUISampleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
private:
	int iCenterX;
	int iCenterY;
	int iGridWidth;
	int iGridWidthReal;
	CWinThread* mThread;
	vector<int> data;

public:
	afx_msg void OnBnConnect();
private:
	CDC* m_pdcMemory;
	CBitmap* m_pBitMap;
	double dScale;
	POINT PhysicsToScreen(POINT pt);
	
public:
	void DrawScanPoint(CDC* pd,POINT pt[]);
	POINT* ptScreen;
	bool isBreak;
	bool isHighPrecision;
	double dAngResolution;
	int iMaxDistance;
	int iDataPointCount;
	POINT* ptScan;
	dl_ls_driver lsDriver;
public:
	afx_msg void OnBnDisconnect();
	afx_msg void OnBnStart();
	afx_msg void OnBnZoomout();
	afx_msg void OnBnZoomin();
	afx_msg void OnUpdateBnConnect(CCmdUI *pCmdUI);
	int iCommandType;
public:
	afx_msg void OnUpdateBnDisconnect(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBnStart(CCmdUI *pCmdUI);
	afx_msg void OnBnStop();
	afx_msg void OnUpdateBnStop(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
private:
	CPoint pointLast;
	CPoint pointCur;
	bool isMove;
public:
	afx_msg void OnBnMove();
	afx_msg void OnUpdateBnMove(CCmdUI *pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
private:
	bool isLeftButtonDown;
public:
	virtual void OnInitialUpdate();
	afx_msg void OnBnSelect();
	afx_msg void OnUpdateBnSelect(CCmdUI *pCmdUI);
	afx_msg void OnBnShowall();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	unsigned int mTimer;
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
struct threadInfo
{
      dl_ls_driver* mDriver;
      DoralidarGUISampleView* mView;
};

#ifndef _DEBUG  // WRRadarGUISampleView.cpp 中的调试版本
inline DoralidarGUISampleDoc* DoralidarGUISampleView::GetDocument() const
   { return reinterpret_cast<DoralidarGUISampleDoc*>(m_pDocument); }
#endif

