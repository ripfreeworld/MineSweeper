
// DlgMineSweeper.h : header file
//

#pragma once
#include "CBtnMine.h"

// Game Status
typedef enum {	GS_NOT_STARTED = 0,
 				GS_STARTED = 1,
				GS_WIN = 2,
				GS_LOST = 3,
		     }	GS_TYPE;	

// replace the number position in BiTMap
typedef enum {	BTM_FLAG = 0,
				BTM_MINE = 1,
				BTM_MINE_R = 2,
				BTM_MARK = 3,
				BTM_FLAG_WRONG = 4
			 }	BTM_TYPE;

// MineCorrect. Check if the surrounding Mines are correct
typedef enum {	MC_TRUE = 0,
				MC_TRUE_FLAG = 1,
				MC_SUSPENDED = 2,
				MC_NOMINE_FLAG = 3,
				MC_MINE_NOFLAG = 4
			 }	MC_TYPE;

// CDlgMineSweeper dialog
class CDlgMineSweeper : public CDialog
{
// Construction
public:
	CDlgMineSweeper(CWnd* pParent = nullptr);	// standard constructor
	~CDlgMineSweeper();

	void ChangeMinesRest(int nMine)			{ m_nMinesRest = m_nMinesRest + nMine; }
	BOOL GetMarkStatus()					{ return m_bMarks;}
	void CheckMines(int nRow, int nCol);
	// increase the number of Buttons that are not pressed. (When manually reset IsPressed() for a button )
	void IncreaseBtnRest(int n = 1)			{ m_nBtnRest += n;}

	GS_TYPE GetGameStatus()					{ return m_nGameStatus;}
	BOOL MinesCorrect(int nRow, int nCol, int nNumber);
	void CheckWin(int nRow, int nCol);
	// Zeilennummer aus der ButtonID berechnen
	int IDToRow(int nID) const              { return (nID - IDC_BUTTON_FIRST) / m_nCols; }               
    // spaltennummer aus der ButtonID berechnen
	int IDToCol(int nID) const              { return (int)fmod(nID - IDC_BUTTON_FIRST, m_nCols); } 

// Dialog Data
	enum { IDD = IDD_MINESWEEPER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnRestart();
	afx_msg void OnButtonClicked(UINT nID);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	void StartNewGame();
	void CreateButtons();
	void ClearButtons();
	void AlloMines();
	void AlloNum();
	COLORREF ChooseColor(int nNumber);

	MC_TYPE MineCorrect(int nRow, int nCol);


	GS_TYPE m_nGameStatus;
	int m_nRowsOld;
	int m_nRows;
	int m_nCols;
	int m_nMines;
	int m_nMinesRest;
	int m_nTimer;
	int m_nBtnRest;		// die Zahl der Knöpfe ohne Minen
    int m_nTimerID;
	BOOL m_bMarks;

	CBtnMine** m_parrBtnMines;	// Mit Zeiger jeden Knopf erreichen
	CImageList m_imgLst;

	// Konvertieren von int nach CString
	CString i2CS (int i)	
	{
		CString str;
		str.Format(_T("%d"), i);	
		return str;
	}
	// Debug Mode
	void Checker();

	//// alternative:
	//virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
