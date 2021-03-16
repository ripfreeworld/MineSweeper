// CBtnMine.cpp : implementation file
//

#include "pch.h"
#include "MineSweeper.h"
#include "DlgMinesweeper.h"
#include "CBtnMine.h"


// CBtnMine

IMPLEMENT_DYNAMIC(CBtnMine, CButton)

//***************************************************************************************
// Konstruktor
//***************************************************************************************
CBtnMine::CBtnMine():
	m_nNumber(0),
	m_bPressed(FALSE),
	m_nStatus(MN_NORMAL)
{}

//***************************************************************************************
// Destruktor
//***************************************************************************************
CBtnMine::~CBtnMine()
{}

BEGIN_MESSAGE_MAP(CBtnMine, CButton)
	ON_WM_RBUTTONDOWN()			// Rechtsklick für Flagge oder Fragezeichen
	ON_WM_LBUTTONDBLCLK()		// Doppelklick mit linker Taste für die Überprüfung rundum
END_MESSAGE_MAP()

// CBtnMine message handlers

//***************************************************************************************
// Rechtsklick für Flagge oder Fragezeichen
//***************************************************************************************
void CBtnMine::OnRButtonDown(UINT nFlags, CPoint point)
{
	// (CDlgMineSweeper*) ist ein Cast, weil GetParent() liefert CWdn* als Rückgabewert
	CDlgMineSweeper* pParent = (CDlgMineSweeper*)GetParent();
	if (pParent->GetGameStatus() == GS_STARTED){
		switch (m_nStatus)
		{
		// set Flag if it was empty
		case MN_NORMAL:
			SetFlag();
			pParent->ChangeMinesRest(-1);
			break;
		case MN_FLAG:
			pParent->ChangeMinesRest(+1);
			// if ?-Mark was turned on
			if (pParent->GetMarkStatus() == TRUE)
				SetMark();					// remove the Flag and increase the number of mines rest
			else
				m_nStatus = MN_NORMAL;		// if ?-Mark was off, directly set back to the empty button
			break;
		case MN_MARK:
			m_nStatus = MN_NORMAL;
			break;
		}
		// den Knopf aktualisieren
		Invalidate();
		// die Zahl restlicher Minen aktualisieren
		pParent->UpdateData(FALSE);
	}
	CButton::OnRButtonDown(nFlags, point);
}

//***************************************************************************************
// Doppelklick mit linker Taste für die Überprüfung rundum
//***************************************************************************************
void CBtnMine::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDlgMineSweeper* pParent = (CDlgMineSweeper*)GetParent();
	// get the row and col information via CWnd
	int nID = GetDlgCtrlID();
	int nCurRow = pParent->IDToRow( nID);
	int nCurCol = pParent->IDToCol( nID);

	// only works for the buttons that have number on it
	if (m_nNumber > 0){
		// pass the position to check, including the number on the button
		if (pParent->MinesCorrect(nCurRow, nCurCol, m_nNumber)){
			// store the current m_nNumber in nRealNumber
			int nRealNumber = m_nNumber;
			// pretend that this button is empty so that CheckMines() can check up to 8 butttons around it.
			m_nNumber = 0;
			m_bPressed = FALSE;
			// set this Button not pressed, so the ButtonRest increases
			pParent->IncreaseBtnRest();
			// CheckMines() has to be defined as public in order to be accessible here
			pParent->CheckMines(nCurRow, nCurCol);
			// set the real number of this button back to m_nNumber
			m_nNumber = nRealNumber;
		}
	}
	// refresh the whole dialog for all the new coming buttons
	//pParent->Invalidate();
	pParent->UpdateData(FALSE);
	pParent->CheckWin(nCurRow, nCurCol);

	CButton::OnLButtonDblClk(nFlags, point);
}

