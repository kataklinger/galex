
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
	_algorithm(&_nextGenerationHandler),
	_initialized(false),
	_fitness(0),
	_generation(-1)
{
}

CChildView::~CChildView()
{
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_NEW_TEST, &CChildView::OnFileNewTest)
	ON_COMMAND(ID_FILE_START, &CChildView::OnFileStart)
	ON_COMMAND(ID_FILE_STOP, &CChildView::OnFileStop)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_TEST, &CChildView::OnUpdateFileNewTest)
	ON_UPDATE_COMMAND_UI(ID_FILE_START, &CChildView::OnUpdateFileStart)
	ON_UPDATE_COMMAND_UI(ID_FILE_STOP, &CChildView::OnUpdateFileStop)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
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

inline unsigned int FNV1a(unsigned char value, unsigned int hash)
{
	return ( value ^ hash ) * 0x01000193;
}

unsigned int FNV1a(const void* data,
				   int size,
				   unsigned int hash = 0x811C9DC5)
{
	const unsigned char* ptr = (const unsigned char*)data;
	while( size-- )
		hash = FNV1a( *ptr++, hash );

	return hash;
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

	CString str;
	str.Format(L"Generation: %i, Fitness: %f", _generation, _fitness );
	dc.TextOut( sx + 10, sy + 10, str );

	if( _generation >= 0 )
	{
		dc.Rectangle( sx + 10, sy + 50, sx + 10 + _sheetSize.GetWidth(), sy + 50 + _sheetSize.GetHeight() );
		for( std::vector<Problems::CSP::Placement>::iterator it = placements.begin(); it != placements.end(); ++it )
		{
			int index = it->GetItem().GetIndex();
			char* buf = (char*)&index;

			int color = FNV1a( buf, sizeof( index ) ) & 0xFFFFFF;

			CBrush brush;
			brush.CreateSolidBrush( color );
			dc.SelectObject( brush );

			dc.Rectangle( sx + 10 + it->GetArea().GetPosition().GetX(), sy + 50 + it->GetArea().GetPosition().GetY(),
				sx + 10 + it->GetArea().GetLimit().GetX(), sy + 50 + it->GetArea().GetLimit().GetY() );

			dc.SelectStockObject( NULL_BRUSH );
			brush.DeleteObject();
		}
	}

	wndDC.BitBlt( 0, 0, clientRect.Width(), clientRect.Height(), &dc, 0, 0, SRCCOPY );
	dc.DeleteDC();
	bmp.DeleteObject();
}

void CChildView::HandleNextGeneration(int id, Common::Observing::GaEventData& data)
{
	const Population::GaPopulation& population = ( (Population::GaPopulationEventData&)data ).GetPopulation();

	const Problems::CSP::CspChromosome& chromosome = (const Problems::CSP::CspChromosome&)*population[ 0 ].GetChromosome();
	const Problems::CSP::CspConfigBlock& ccb = (const Problems::CSP::CspConfigBlock&)*chromosome.GetConfigBlock();

	const Statistics::GaStatistics& stats = population.GetStatistics();

	_generation = stats.GetCurrentGeneration();

	Problems::CSP::Sheet sheet( ccb.GetSheetSize() );
	Problems::CSP::PlaceItems( sheet, ccb.GetItems(), chromosome.GetGenes() );

	_placements = sheet.GetPlacements();

	const Problems::CSP::CspFitness f = (const Problems::CSP::CspFitness&)population[ 0 ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW );
	_fitness = f.GetValue();

	Invalidate();
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return false;
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

		_generation = -1;
		_fitness = 0.0;

		_initialized = true;
	}
}

void CChildView::OnUpdateFileNewTest(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( _algorithm.GetState() == Common::Workflows::GAWS_STOPPED );
}

void CChildView::OnFileStart()
{
	_algorithm.Start();
}

void CChildView::OnUpdateFileStart(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( _initialized && _algorithm.GetState() == Common::Workflows::GAWS_STOPPED );
}

void CChildView::OnFileStop()
{
	_algorithm.Stop();
}

void CChildView::OnUpdateFileStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( _algorithm.GetState() == Common::Workflows::GAWS_RUNNING );
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();

	if( _algorithm.GetState() != Common::Workflows::GAWS_STOPPED )
		_algorithm.Stop();
}

void CChildView::ComputeScrollBars()
{
	CRect cr;
	GetClientRect( cr );

	SCROLLINFO hsi;
	hsi.cbSize = sizeof( hsi );
	hsi.fMask = SIF_RANGE | SIF_PAGE;
	hsi.nMin = 0;
	hsi.nMax = _sheetSize.GetWidth() + 20;
	hsi.nPage = cr.Width();

	SetScrollInfo( SB_HORZ, &hsi, TRUE );

	hsi.nMax = _sheetSize.GetHeight() + 100;
	hsi.nPage = cr.Height();

	SetScrollInfo( SB_VERT, &hsi, TRUE );
}

void CChildView::Scroll(int scrollBar, int nSBCode, int nPos)
{
	int minpos, maxpos, curpos;

	GetScrollRange( SB_HORZ, &minpos, &maxpos ); 
	maxpos = GetScrollLimit( scrollBar );
	curpos = GetScrollPos( scrollBar );

	switch( nSBCode )
	{
	case SB_LEFT:
		curpos = minpos;
		break;

	case SB_RIGHT:
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:
		break;

	case SB_LINELEFT:
		if( curpos > minpos )
			curpos--;
		break;

	case SB_LINERIGHT:
		if( curpos < maxpos )
			curpos++;
		break;

	case SB_PAGELEFT:
		{
			SCROLLINFO   info;
			GetScrollInfo( scrollBar, &info, SIF_ALL);

			if( curpos > minpos )
				curpos = max(minpos, curpos - (int) info.nPage);
		}
		break;

	case SB_PAGERIGHT:
		{
			SCROLLINFO info;
			GetScrollInfo( scrollBar, &info, SIF_ALL);

			if( curpos < maxpos )
				curpos = min( maxpos, curpos + (int)info.nPage );
		}
		break;

	case SB_THUMBPOSITION:
		curpos = nPos;
		break;

	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	}

	SetScrollPos( scrollBar, curpos);
}

void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Scroll( SB_HORZ, nSBCode, nPos );
	Invalidate();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Scroll( SB_VERT, nSBCode, nPos );
	Invalidate();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	ComputeScrollBars();
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetScrollPos( SB_VERT, GetScrollPos( SB_VERT ) - zDelta );
	Invalidate();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
