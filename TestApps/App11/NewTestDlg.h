#pragma once


// CNewTestDlg dialog

class CNewTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewTestDlg)

public:
	CNewTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewTestDlg();

// Dialog Data
	enum { IDD = IDD_NEW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	int m_sheetWidth;
	int m_sheetHeight;
	int m_itemMinWidth;
	int m_itemMaxWidth;
	int m_itemMinHeight;
	int m_itemMaxHeight;
	int m_itemCount;

public:
	inline int GetSheetWidth() const { return m_sheetWidth; }
	inline int GetSheetHeight() const { return m_sheetHeight; }
	inline int GetItemMinWidth() const { return m_itemMinWidth; }
	inline int GetItemMaxWidth() const { return m_itemMaxWidth; }
	inline int GetItemMinHeight() const { return m_itemMinHeight; }
	inline int GetItemMaxHeight() const { return m_itemMaxHeight; }
	inline int GetItemCount() const { return m_itemCount; }
};
