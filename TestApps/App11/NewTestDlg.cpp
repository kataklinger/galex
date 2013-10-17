// NewTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "App11.h"
#include "NewTestDlg.h"
#include "afxdialogex.h"


// CNewTestDlg dialog

IMPLEMENT_DYNAMIC(CNewTestDlg, CDialogEx)

CNewTestDlg::CNewTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNewTestDlg::IDD, pParent)
	, m_sheetWidth(0)
	, m_sheetHeight(0)
	, m_itemMinWidth(0)
	, m_itemMaxWidth(0)
	, m_itemMinHeight(0)
	, m_itemMaxHeight(0)
	, m_itemCount(0)
{

}

CNewTestDlg::~CNewTestDlg()
{
}

void CNewTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SHEET_WIDTH, m_sheetWidth);
	DDX_Text(pDX, IDC_SHEET_HEIGHT, m_sheetHeight);
	DDX_Text(pDX, IDC_ITEM_MIN_WIDTH, m_itemMinWidth);
	DDX_Text(pDX, IDC_ITEM_MAX_WIDTH, m_itemMaxWidth);
	DDX_Text(pDX, IDC_ITEM_MIN_HEIGHT, m_itemMinHeight);
	DDX_Text(pDX, IDC_ITEM_MAX_HEIGHT, m_itemMaxHeight);
	DDX_Text(pDX, IDC_ITEM_COUNT, m_itemCount);
	DDV_MinMaxInt(pDX, m_sheetWidth, 1, INT_MAX);
	DDV_MinMaxInt(pDX, m_sheetHeight, m_sheetWidth + 1, INT_MAX);
	DDV_MinMaxInt(pDX, m_itemMinWidth, 1, m_sheetWidth);
	DDV_MinMaxInt(pDX, m_itemMaxWidth, m_itemMinWidth, m_sheetWidth);
	DDV_MinMaxInt(pDX, m_itemMinHeight, 1, m_sheetHeight);
	DDV_MinMaxInt(pDX, m_itemMaxHeight, m_itemMinHeight, m_sheetHeight);
	DDV_MinMaxInt(pDX, m_itemCount, 1, INT_MAX);
}


BEGIN_MESSAGE_MAP(CNewTestDlg, CDialogEx)
END_MESSAGE_MAP()


// CNewTestDlg message handlers
