
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "App11.h"
#include "ChildView.h"

#include "NewTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView() : _nextGenerationHandler(this, &CChildView::HandleNextGeneration),
	_stateChangeHandler(this, &CChildView::HandleStateChange),
	_algorithm(&_nextGenerationHandler, &_stateChangeHandler)
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_NEW_TEST, &CChildView::OnFileNewTest)
	ON_COMMAND(ID_FILE_START, &CChildView::OnFileStart)
	ON_COMMAND(ID_FILE_STOP, &CChildView::OnFileStop)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::HandleNextGeneration(int id, Common::Observing::GaEventData& data)
{
}

void CChildView::HandleStateChange(int id, Common::Observing::GaEventData& data)
{
	Common::Workflows::GaWorkflowStateEventData& d = (Common::Workflows::GaWorkflowStateEventData&)data;

	CMenu* menu = AfxGetMainWnd()->GetMenu();

	if(d.GetNewState() == Common::Workflows::GAWS_RUNNING)
	{
		menu->EnableMenuItem(ID_FILE_NEW_TEST, FALSE);
		menu->EnableMenuItem(ID_FILE_START, FALSE);
		menu->EnableMenuItem(ID_FILE_STOP, TRUE);
	}
	else if(d.GetNewState() == Common::Workflows::GAWS_STOPPED)
	{
		menu->EnableMenuItem(ID_FILE_NEW_TEST, TRUE);
		menu->EnableMenuItem(ID_FILE_START, FALSE);
		menu->EnableMenuItem(ID_FILE_STOP, FALSE);
	}
}

void CChildView::OnFileNewTest()
{
	CNewTestDlg dlg;
	if( dlg.DoModal() == IDOK )
	{
		_algorithm.SetParameters(dlg.GetSheetWidth(), dlg.GetSheetHeight(), 
			dlg.GetItemMinWidth(), dlg.GetItemMaxWidth(), dlg.GetItemMinHeight(), dlg.GetItemMaxHeight(), dlg.GetItemCount());

		AfxGetMainWnd()->GetMenu()->EnableMenuItem(ID_FILE_NEW_TEST, TRUE);
	}
}


void CChildView::OnFileStart()
{
	_algorithm.Start();
}


void CChildView::OnFileStop()
{
	_algorithm.Stop();
}
