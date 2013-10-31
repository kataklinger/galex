
// ChildView.h : interface of the CChildView class
//


#pragma once

#include "CSPAlgorithm.h"

// CChildView window

class CChildView : public CWnd
{
private:
	Common::Observing::GaMemberEventHandler<CChildView> _nextGenerationHandler;
	Common::Observing::GaMemberEventHandler<CChildView> _stateChangeHandler;

	CCriticalSection _sect;

	Problems::CSP::Size _sheetSize;
	std::vector<Problems::CSP::Placement> _placements;
	float _fitness;

	CSPAlgorithm _algorithm;

// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

private:
	void GACALL HandleNextGeneration(int id, Common::Observing::GaEventData& data);
	void GACALL HandleStateChange(int id, Common::Observing::GaEventData& data);

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileNewTest();
	afx_msg void OnFileStart();
	afx_msg void OnFileStop();
};

