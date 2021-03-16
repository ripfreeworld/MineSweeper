//***************************************************************************************
// Klasse für einen Button des Minenfeldes
//***************************************************************************************

#pragma once

#define IDC_BUTTON_FIRST 10000
#define IDC_BUTTON_LAST 12500

// später zeigt nur die Minen bei NORMAL und CRITICAL, wenn game over
typedef enum {	MN_NORMAL = 0,
 				MN_FLAG = 1,
				MN_MARK  = 2,
				MN_CRITICAL  = 5,	// Only one. Set in red
		     }	MN_TYPE;

class CBtnMine : public CButton
{
	DECLARE_DYNAMIC(CBtnMine)

public:
	CBtnMine();
	virtual ~CBtnMine();

	int GetNumber() const					{ return m_nNumber;}
	int GetStatus() const					{ return m_nStatus;}
	BOOL IsMine() const						{ return m_nNumber == -1;}
	BOOL IsPressed() const					{ return m_bPressed;}
	
	void SetMine()							{ m_nNumber = -1;}
	void IncreaseNum()						{ if( !IsMine()) m_nNumber++;}
	void SetPressed( BOOL bPressed = TRUE)	{ m_bPressed = bPressed;}
	void SetCriticalMine()					{ m_nStatus = MN_CRITICAL;}
	void SetFlag()							{ m_nStatus = MN_FLAG;}
	void SetMark()							{ m_nStatus = MN_MARK;}
	
protected:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	int m_nNumber;		// -1 für Mine; Nummer für Zahl der Minen rundum
	int m_nStatus;		// Status von jedem Knopf
	BOOL m_bPressed;

};


