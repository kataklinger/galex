
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
	CPaintDC wndDC(this);

	CRect clientRect;
	GetClientRect( clientRect );

	CDC dc;
	CBitmap bmp;
	dc.CreateCompatibleDC( &wndDC );
	bmp.CreateCompatibleBitmap( &wndDC, clientRect.Width(), clientRect.Height() );
	dc.SelectObject( &bmp );

	int sx = -GetScrollPos( SB_HORZ );
	int sy = -GetScrollPos( SB_VERT );

	CBrush bgBrush( RGB( 255, 255, 255 ) );
	dc.FillRect( clientRect, &bgBrush );

	dc.SetBkColor( RGB( 255, 255, 255 ) );
	dc.SetBkMode( TRANSPARENT );

	CSingleLock lock( &_sect, TRUE );

	std::vector<Problems::CSP::Placement> placements( _placements );

	lock.Unlock();

	dc.Rectangle( 0, 0, _sheetSize.GetWidth(), _sheetSize.GetHeight() );
	for( std::vector<Problems::CSP::Placement>::iterator it = placements.begin(); it != placements.end(); ++it )
	{
		dc.Rectangle(it->GetArea().GetPosition().GetX(), it->GetArea().GetPosition().GetY(), it->GetArea().GetLimit().GetX(), it->GetArea().GetLimit().GetY() );
	}

	wndDC.BitBlt( 0, 0, clientRect.Width(), clientRect.Height(), &dc, 0, 0, SRCCOPY );
}

void CChildView::HandleNextGeneration(int id, Common::Observing::GaEventData& data)
{
	const Population::GaPopulation& population = ( (Population::GaPopulationEventData&)data ).GetPopulation();

	const Problems::CSP::CspChromosome& chromosome = (const Problems::CSP::CspChromosome&)*population[ 0 ].GetChromosome();
	const Problems::CSP::CspConfigBlock& ccb = (const Problems::CSP::CspConfigBlock&)*chromosome.GetConfigBlock();

	const Statistics::GaStatistics& stats = population.GetStatistics();
	if( stats.GetCurrentGeneration() != 1 && !stats.GetValue<Fitness::GaFitness>( Population::GADV_BEST_FITNESS ).IsChanged( 2 ) )
		return;

	_placements = chromosome.GetSheet().GetPlacements();

	const Problems::CSP::CspFitness f = (const Problems::CSP::CspFitness&)population[ 0 ].GetFitness(Population::GaChromosomeStorage::GAFT_RAW);
	_fitness = f.GetValue();

	Invalidate();
}

void CChildView::HandleStateChange(int id, Common::Observing::GaEventData& data)
{
	Common::Workflows::GaWorkflowStateEventData& d = (Common::Workflows::GaWorkflowStateEventData&)data;

	CMenu* menu = AfxGetMainWnd()->GetMenu();

	if(d.GetNewState() == Common::Workflows::GAWS_RUNNING)
	{
		menu->EnableMenuItem( ID_FILE_NEW_TEST, FALSE );
		menu->EnableMenuItem( ID_FILE_START, FALSE );
		menu->EnableMenuItem( ID_FILE_STOP, TRUE );
	}
	else if(d.GetNewState() == Common::Workflows::GAWS_STOPPED)
	{
		menu->EnableMenuItem( ID_FILE_NEW_TEST, TRUE );
		menu->EnableMenuItem( ID_FILE_START, FALSE );
		menu->EnableMenuItem( ID_FILE_STOP, FALSE );
	}
}

void CChildView::OnFileNewTest()
{
	CNewTestDlg dlg;
	if( dlg.DoModal() == IDOK )
	{
		_algorithm.SetParameters( dlg.GetSheetWidth(), dlg.GetSheetHeight(), 
			dlg.GetItemMinWidth(), dlg.GetItemMaxWidth(), dlg.GetItemMinHeight(), dlg.GetItemMaxHeight(), dlg.GetItemCount() );

		_sheetSize.SetWidth(dlg.GetSheetWidth());
		_sheetSize.SetHeight(dlg.GetSheetHeight());
		_placements.clear();

		AfxGetMainWnd()->GetMenu()->EnableMenuItem( ID_FILE_START, TRUE );
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
