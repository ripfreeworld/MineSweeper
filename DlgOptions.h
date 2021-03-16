//***************************************************************************************
// Einstellungsdialog
//***************************************************************************************
#pragma once

typedef enum {	LV_BEGINNER  = 0,
 				LV_INTERMEDIATE = 1,
				LV_EXPERT  = 2,
				LV_CUSTOM = 3
		     }	LV_TYPE;

class CDlgOptions : public CDialog
{
	DECLARE_DYNAMIC(CDlgOptions)
	friend class CDlgMineSweeper;

public:
	CDlgOptions(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgOptions();					// destructor
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	afx_msg void OnNonCustom();
	afx_msg void OnCustom();

	DECLARE_MESSAGE_MAP()
private:
	int m_nLevel;
	int m_nRows;
	int m_nCols;
	int m_nMines;
	int m_nCustRows;
	int m_nCustCols;
	int m_nCustMines;
	BOOL m_bMarks;
	CEdit m_edtCustRows;
	CEdit m_edtCustCols;
	CEdit m_edtCustMines;
};
