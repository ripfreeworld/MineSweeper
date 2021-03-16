
// DlgMineSweeper.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MineSweeper.h"
#include "DlgMineSweeper.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "DlgOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <vector>
#include <random>

// CDlgMineSweeper dialog

//***************************************************************************************
// Konstruktor
//***************************************************************************************
CDlgMineSweeper::CDlgMineSweeper(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD, pParent), 
	m_parrBtnMines(nullptr), 
	m_nRowsOld(0),
	m_nMinesRest(0),
	m_nTimer(0), 
	m_nTimerID(0),
	m_nGameStatus(GS_NOT_STARTED)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	const TCHAR *pszKey = _T("2");
	m_nRows = AfxGetApp()->GetProfileInt(pszKey, _T("ROWS"), 9); // default 9, falls nicht existiert
	m_nCols = AfxGetApp()->GetProfileInt(pszKey, _T("COLS"), 9);
	m_nMines = AfxGetApp()->GetProfileInt(pszKey, _T("MINES"), 10);
	m_bMarks = AfxGetApp()->GetProfileInt(pszKey, _T("MARKS"), 0);
	m_nBtnRest = m_nRows * m_nCols;
}

//***************************************************************************************
// Destruktor
//***************************************************************************************
CDlgMineSweeper::~CDlgMineSweeper()
{
	const TCHAR *pszKey = _T("2");	// Unterordner ""
	AfxGetApp()->WriteProfileInt(pszKey, _T("ROWS"), m_nRows);
	AfxGetApp()->WriteProfileInt(pszKey, _T("COLS"), m_nCols);
	AfxGetApp()->WriteProfileInt(pszKey, _T("MINES"), m_nMines);
	// Level ist für den Hauptdialog nicht relevant
	AfxGetApp()->WriteProfileInt(pszKey, _T("MARKS"), m_bMarks);
	ClearButtons();
}


//***************************************************************************************
// Dialog wird geschlossen
//***************************************************************************************
void CDlgMineSweeper::OnDestroy()
{
	CDialog::OnDestroy();
	// stop den Timer
	if( m_nTimerID != 0)
		KillTimer(m_nTimerID);
}

void CDlgMineSweeper::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COUNTER, m_nMinesRest);
	DDX_Text(pDX, IDC_TIMER, m_nTimer);

}

BEGIN_MESSAGE_MAP(CDlgMineSweeper, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_RESTART, &CDlgMineSweeper::OnRestart)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_FIRST, IDC_BUTTON_LAST, OnButtonClicked)
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CDlgMineSweeper message handlers

//***************************************************************************************
// Dialog initialisieren
//***************************************************************************************
BOOL CDlgMineSweeper::OnInitDialog()
{
	CDialog::OnInitDialog();
	StartNewGame();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	//Imageliste laden (flags and bombs)
	m_imgLst.Create(IDB_ICONS, 16, 1, RGB(255,255,255));

	CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        CString strSettings;
        if( strSettings.LoadString(IDS_SETTINGS))
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_SETTINGS, strSettings);
        }
    }

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDlgMineSweeper::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgMineSweeper::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//***************************************************************************************
// System Liste öffnen mit dem Klick obenlinks
//***************************************************************************************
void CDlgMineSweeper::OnSysCommand(UINT nID, LPARAM lParam)
{
	// open the settings dlg
	if ((nID & 0xFFF0) == IDM_SETTINGS)
    {
		CDlgOptions dlgOpt;
		INT_PTR nResponse = dlgOpt.DoModal();
		if (nResponse == IDOK)
		{
			m_nRows = dlgOpt.m_nRows;
			m_nCols = dlgOpt.m_nCols;
			m_nMines = dlgOpt.m_nMines;
			m_bMarks = dlgOpt.m_bMarks;
			StartNewGame();
		}
	}
	CDialog::OnSysCommand(nID, lParam);
}

//***************************************************************************************
// Knöpfe für das Spielfeld anlegen
//***************************************************************************************
void CDlgMineSweeper::CreateButtons()
{
	LockWindowUpdate();
	ClearButtons();	//Alte Buttons löschen
	m_parrBtnMines = new CBtnMine*[m_nRows];
	for( int cRow = 0; cRow < m_nRows; cRow++)
		m_parrBtnMines[cRow] = new CBtnMine[m_nCols];
	m_nRowsOld = m_nRows;

	//Retrieves a pointer to the specified control or child window in a dialog box or other window.
	//Warum braucht man Zeiger auf IDC_BTN1? GetDlgItem gibt einen Zeiger zurück.
	//point to IDC_BTN1
	CWnd* pbtnFirst = GetDlgItem(IDC_BTN1);
	CWnd* pbtnRestart = GetDlgItem(IDC_RESTART);
	CWnd* pTimer = GetDlgItem(IDC_TIMER);

	// if the IDC_BTN1 exists
	if (pbtnFirst && pbtnRestart)
	{
		DWORD dwStyleBtn1;
		CRect rcBtn1, rcBtnRes, rcTimer;
		CRect rcWnd, rcClient;

		// get the window region of a window.
		pbtnFirst->GetWindowRect(rcBtn1);		// first Button of play field (initializer)
		pbtnRestart->GetWindowRect(rcBtnRes);	// Buton of restart
		pTimer->GetWindowRect(rcTimer);
		GetWindowRect(rcWnd);
		GetClientRect(rcClient);

		// Converts the screen coordinates of a given point or rectangle on the display to client coordinates.
		// rcClient already in client coordinates
		ScreenToClient(rcBtn1);
		ScreenToClient(rcWnd);
		ScreenToClient(rcBtnRes);
		ScreenToClient(rcTimer);
		// Returns the current window style.
		dwStyleBtn1 = pbtnFirst->GetStyle() | WS_VISIBLE;

		int cxClient, cyClient, cxDlg, cyDlg;
		int cxMargin = rcWnd.Width()- rcClient.Width();
		int cyMargin = rcWnd.Height()- rcClient.Height();
		//int cRand = rcBtn1.left + cxMargin/2;
		cxClient = 2 * rcBtn1.left + rcBtn1.Width() * m_nCols;
		cyClient = rcBtn1.left + rcBtn1.top + rcBtn1.Height() * m_nRows;
		cxDlg = cxClient + cxMargin;
		cyDlg = cyClient + cyMargin;
		SetWindowPos( NULL, 0, 0, cxDlg, cyDlg, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
		// Anpassung der Buttons
		rcBtnRes.MoveToX(cxClient/2 - rcBtnRes.Width()/2);
		rcTimer.MoveToX(cxClient - rcTimer.Width() - rcBtn1.left);

		// nicht erneut zeichnen pbtnRestart ist das Objekt, und rcBtnRes ist eine Position
		pbtnRestart->MoveWindow( rcBtnRes, FALSE);
		pTimer->MoveWindow( rcTimer, FALSE);

		for (int cRow = 0; cRow < m_nRows; cRow++) {
			for (int cCol = 0; cCol < m_nCols; cCol++) {
				m_parrBtnMines[cRow][cCol].Create( _T(""), dwStyleBtn1, rcBtn1, this, IDC_BUTTON_FIRST + cRow * m_nCols + cCol);
				rcBtn1.OffsetRect(rcBtn1.Width(), 0);
			}
			rcBtn1.OffsetRect(m_nCols * (-rcBtn1.Width()), rcBtn1.Height());
		}
	}
	UnlockWindowUpdate();
}

//***************************************************************************************
// alte Knöpfe entfernen
//***************************************************************************************
void CDlgMineSweeper::ClearButtons()
{
	if (m_parrBtnMines)
	{
		for( int cRow = 0; cRow < m_nRowsOld; cRow++)
			delete [] m_parrBtnMines[cRow];
		delete [] m_parrBtnMines;
		m_parrBtnMines = nullptr;
	}
}

//***************************************************************************************
// "Restart" gedrückt
//***************************************************************************************
void CDlgMineSweeper::OnRestart()
{
	StartNewGame();	
}

//***************************************************************************************
// Neues Spiel starten
//***************************************************************************************
void CDlgMineSweeper::StartNewGame()
{
	//Alles zurücksetzen
	m_nTimer = 0;
	// reset the GameStatus to enable interaction
	m_nGameStatus = GS_NOT_STARTED;
	// how many buttons remaining
	m_nBtnRest = m_nRows * m_nCols;
	if (m_nTimerID) 
	{ KillTimer(m_nTimerID); m_nTimerID = 0;}
	CreateButtons();		//Knöpfe anlegen
	
	m_nMinesRest = m_nMines;
	UpdateData(FALSE);
	//Minen verteilen
	AlloMines();
	//Nummern berechnen
	AlloNum();
	//Checker();
	Invalidate();
}


//***************************************************************************************
// auf den Knopf klicken
//***************************************************************************************
void CDlgMineSweeper::OnButtonClicked(UINT nID)
{
	if( m_nGameStatus == GS_NOT_STARTED)
	{
		if (m_nTimerID) { KillTimer(m_nTimerID); }
		m_nTimerID = (int)SetTimer(1, 1000, NULL);
		m_nGameStatus = GS_STARTED;
	}
	if (m_nGameStatus == GS_STARTED)
	{
		int nRow = IDToRow(nID);
		int nCol = IDToCol(nID);
		CheckMines(nRow, nCol);
		CheckWin(nRow, nCol);
		UpdateData(FALSE);
		//Invalidate();
	}
}

//***************************************************************************************
// allocate die Positionen der Minen
//***************************************************************************************
void CDlgMineSweeper::AlloMines() {
	std::vector<int> v_iMine;
	for (int i = 0; i != m_nRows * m_nCols; ++i) 
		v_iMine.push_back(i);
	// https://stackoverflow.com/a/21598352/10173282
	//the shuffle algorithm takes a pseudo random number generator (PRNG) object seeded as third parameter
	std::mt19937 rng(static_cast<uint32_t>(time(0)));
	std::shuffle(std::begin(v_iMine), std::end(v_iMine), rng);
	for (int c = 0; c != m_nMines; ++c) {
		int nRow = v_iMine[c] / m_nCols;
		int nCol = (int)fmod( v_iMine[c], m_nCols);
		m_parrBtnMines[nRow][nCol].SetMine();
	}
}

//***************************************************************************************
// Anzahl der Minen auf den umliegenden Knöpfen berechnen
//***************************************************************************************
void CDlgMineSweeper::AlloNum() {
	for (int i = 0; i != m_nRows; ++i) {
		for (int j = 0; j != m_nCols; ++j) {
			if (m_parrBtnMines[i][j].IsMine()){
				bool bTop = (i==0);
				bool bBot = (i==m_nRows-1);
				bool bRight = (j==m_nCols-1);
				bool bLeft = (j==0);

				// alle 8 Nachbarn überprüfen
				if (!bBot)
					m_parrBtnMines[i+1][j].IncreaseNum();		//unten
				if (!bTop)
					m_parrBtnMines[i-1][j].IncreaseNum();		//oben
				if (!bRight)
					m_parrBtnMines[i][j+1].IncreaseNum();		//rechts
				if (!bLeft)
					m_parrBtnMines[i][j-1].IncreaseNum();		//links
				if (!bBot && !bRight)
					m_parrBtnMines[i+1][j+1].IncreaseNum();		//unten rechts
				if (!bBot && !bLeft)
					m_parrBtnMines[i+1][j-1].IncreaseNum();		//unten links
				if (!bTop && !bRight)
					m_parrBtnMines[i-1][j+1].IncreaseNum();		//oben rechts
				if (!bTop && !bLeft)
					m_parrBtnMines[i-1][j-1].IncreaseNum();		//oben links
			}
		}
	}
}


//***************************************************************************************
// Die jetzige Mine überprüfen, ggf. rekursiv
//***************************************************************************************
void CDlgMineSweeper::CheckMines(int nRow, int nCol) {
	bool bTop = (nRow==0);
	bool bBot = (nRow==m_nRows-1);
	bool bRight = (nCol==m_nCols-1);
	bool bLeft = (nCol==0);
	// für leeres Feld
	if (!m_parrBtnMines[nRow][nCol].IsPressed() && m_parrBtnMines[nRow][nCol].GetStatus()==MN_NORMAL)
	{
		if (m_parrBtnMines[nRow][nCol].GetNumber()==0) 
		{	
			// nicht mehr möglich zu drücken
			m_parrBtnMines[nRow][nCol].EnableWindow(FALSE);
			// set the current button pressed
			m_parrBtnMines[nRow][nCol].SetPressed();
			m_nBtnRest--;
			// rekursiv wenn nicht am Rand
			if (!bBot)
				CheckMines(nRow+1,nCol);		//unten
			if (!bTop)
				CheckMines(nRow-1,nCol);		//oben
			if (!bRight)
				CheckMines(nRow,nCol+1);		//rechts
			if (!bLeft)
				CheckMines(nRow,nCol-1);		//links
			if (!bBot && !bRight)
				CheckMines(nRow+1,nCol+1);		//unten rechts
			if (!bBot && !bLeft)
				CheckMines(nRow+1,nCol-1);		//unten links
			if (!bTop && !bRight)
				CheckMines(nRow-1,nCol+1);		//oben rechts
			if (!bTop && !bLeft)
				CheckMines(nRow-1,nCol-1);		//oben links
		}
		// an Knopf mit Zahl. i.e., m_nNumber > 0
		if( m_parrBtnMines[nRow][nCol].GetNumber() > 0) 
		{
			// zuerst gedrückt, später von OnDrawItem() gezeichnet
			m_parrBtnMines[nRow][nCol].SetPressed();
			m_nBtnRest--;
		}
		m_parrBtnMines[nRow][nCol].Invalidate();
	}
}


//***************************************************************************************
// Bis zu 8 Nachbarn überprüfen vor der Erweiterung
//***************************************************************************************
BOOL CDlgMineSweeper::MinesCorrect(int nRow, int nCol, int nNumber) {
	MC_TYPE res[8] = {};						// {0,0,0,...}  0 ist gleich MC_TRUE
	bool bTop = (nRow==0);
	bool bBot = (nRow==m_nRows-1);
	bool bRight = (nCol==m_nCols-1);
	bool bLeft = (nCol==0);

	if (!bBot && res)
		res[0] = MineCorrect(nRow+1,nCol);		//unten
	if (!bTop && res)
		res[1] = MineCorrect(nRow-1,nCol);		//oben
	if (!bRight && res)
		res[2] = MineCorrect(nRow,nCol+1);		//rechts
	if (!bLeft && res)
		res[3] = MineCorrect(nRow,nCol-1);		//links
	if (!bBot && !bRight && res)
		res[4] = MineCorrect(nRow+1,nCol+1);	//unten rechts
	if (!bBot && !bLeft && res)
		res[5] = MineCorrect(nRow+1,nCol-1);	//unten links
	if (!bTop && !bRight && res)
		res[6] = MineCorrect(nRow-1,nCol+1);	//oben rechts
	if (!bTop && !bLeft && res)
		res[7] = MineCorrect(nRow-1,nCol-1);	//oben links

	// Err1&&Err2: if Mine_not_set_with_Flag and Non_Mine_set_with_Flag --- LOST
	MC_TYPE* pErr1 = std::find(std::begin(res), std::end(res), MC_NOMINE_FLAG);
	MC_TYPE* pErr2 = std::find(std::begin(res), std::end(res), MC_MINE_NOFLAG);
	// find if there is MC_SUSPENDED --- no action
	MC_TYPE* pFalse = std::find(std::begin(res), std::end(res), MC_SUSPENDED);

	// collect how many correct flags there
	int cFlags = 0;
	for (int i = 0; i < 8; i++) {
		if (res[i] == MC_TRUE_FLAG)
			cFlags++;
	}

	// find if both kinds of errors (Err1 and Err2) exist
	if (pErr1 != std::end(res) && pErr2 != std::end(res))
	{	
		m_nGameStatus = GS_LOST;
		return FALSE;
	}
	// if no MC_SUSPENDED found and enough correct Flags, return TRUE
	else if (pFalse == std::end(res) && cFlags == nNumber)
		return TRUE;
	else
		return FALSE;
}

//***************************************************************************************
// Einzelnen Nachbarn prüfen
//***************************************************************************************
MC_TYPE CDlgMineSweeper::MineCorrect(int nRow, int nCol) {
	// if the Mine is not set with Flag
	if (m_parrBtnMines[nRow][nCol].GetStatus() == MN_NORMAL&& m_parrBtnMines[nRow][nCol].IsMine())
	{
		// return Mine not set with flag
		return MC_MINE_NOFLAG;
	}
	else if (m_parrBtnMines[nRow][nCol].GetStatus() == MN_FLAG && !m_parrBtnMines[nRow][nCol].IsMine())
	{
		// return Mine not set with flag
		return MC_NOMINE_FLAG;
	}
	// non-Mine's m_nStatus is MN_NORMAL (also not MN_MARK) --- CORRECT 
	else if (m_parrBtnMines[nRow][nCol].GetStatus() == MN_NORMAL && !m_parrBtnMines[nRow][nCol].IsMine())
		return MC_TRUE;			// remains the default value of array
	// if Mine's m_nStatus is MN_FLAG --- CORRECT
	else if (m_parrBtnMines[nRow][nCol].GetStatus() == MN_FLAG && m_parrBtnMines[nRow][nCol].IsMine())
		return MC_TRUE_FLAG;	// to count how many flags there
	// else, Mine's m_nStatus with marks, or non-Mine button is MN_FLAG --- SUSPENDED
	else
		return MC_SUSPENDED;	// SUSPENDED leads to ignoration of double click
}


//***************************************************************************************
// WIN oder LOST
//***************************************************************************************
void CDlgMineSweeper::CheckWin(int nRow, int nCol) {
	// gewonnen
	if (m_nBtnRest == m_nMines) 
	{
		m_nGameStatus = GS_WIN;
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
		// Weil Flaggen automatisch gezeichnet werden, aktualisiert den Anzeiger auch.
		m_nMinesRest = 0;
		UpdateData(FALSE);
		// Aktualisierung der restlichen Minen mit Flaggen, bevor das Fenster auftaucht
		Invalidate();	
		CString str;
		str.LoadString(IDS_WIN);
		MessageBox( str, 0, MB_ICONWARNING | MB_OK);
	}
	// -1 -> Game Over
	if (m_parrBtnMines[nRow][nCol].IsMine() || m_nGameStatus == GS_LOST){
		for (int i = 0; i != m_nRows; ++i) {
			for (int j = 0; j != m_nCols; ++j) {
				// Nicht überschreiben für den Feld, der schon mit Flag eingesetzt wurde.
				if (m_parrBtnMines[i][j].IsMine() && m_parrBtnMines[i][j].GetStatus() != MN_FLAG)
				{
					m_parrBtnMines[i][j].SetPressed();
					m_nBtnRest--;
					if (i==nRow && j==nCol)
						m_parrBtnMines[i][j].SetCriticalMine();
				}
			}
		}
		m_nGameStatus = GS_LOST;
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
		m_nMinesRest = 0;	// Anzeiger aktualisieren
		m_nMinesRest = 0;
		UpdateData(FALSE);
		Invalidate();		// Alle Minen anzeigen bevor das Fenster auftaucht
		CString str;
		str.LoadString(IDS_GG);
		MessageBox( str, 0, MB_ICONWARNING | MB_OK);
	}
}

//***************************************************************************************
// Knöpfe zeichnen
//***************************************************************************************
void CDlgMineSweeper::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl >= IDC_BUTTON_FIRST && nIDCtl < nIDCtl + m_nRows * m_nCols)
    {
        int cRow = IDToRow( nIDCtl);
        int cCol = IDToCol( nIDCtl);
		//nNumber = -1 für Mine
		int nNumber = m_parrBtnMines[cRow][cCol].GetNumber();
		int nStatus = m_parrBtnMines[cRow][cCol].GetStatus();

        //lpDrawItemStruct->itemAction == ODA_DRAWENTIRE
        HDC hDC = lpDrawItemStruct->hDC;
        //CWnd* pbtnDraw = GetDlgItem(nIDCtl);
        CDC* pDC = CDC::FromHandle(hDC);
        //UINT nItemID = lpDrawItemStruct->CtlID;
        CRect rcWnd(lpDrawItemStruct->rcItem);
        int nWidth = rcWnd.Width();
        int nHeight = rcWnd.Height();
        CFont fntBtn;
		// CRect(POINT topLeft, POINT bottomRight)
        CRect rcFont(CPoint((int)(nWidth*0.35), (int)(nHeight*0.15)), CPoint(nWidth, (int)(nHeight * 0.9)));
        pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
		// create font für Text, nämlich Nummer
        fntBtn.CreateFont((int)(nHeight*0.9), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
		// to store this Font
        CFont* pOldFont = pDC->SelectObject(&fntBtn);
        CBrush brushBack(GetSysColor(COLOR_BTNFACE));
        pDC->FillRect(rcWnd, &brushBack);               

		//Knopf wurde nicht gedrückt --- leer, Flag oder Fragezeichen
        if (!m_parrBtnMines[cRow][cCol].IsPressed())
        {
			CRect rcFont2(CPoint((int)(nWidth*0.2), (int)(nHeight*0.2)), CPoint(nWidth, nHeight));
			// normal situation
			if( nStatus == MN_FLAG)
				// Flag anzeigen
				m_imgLst.Draw(pDC, BTM_FLAG, rcFont2.TopLeft(), ILD_TRANSPARENT);
			if( nStatus == MN_MARK)
				// Fragezeichen anzeigen
				m_imgLst.Draw(pDC, BTM_MARK, rcFont2.TopLeft(), ILD_TRANSPARENT);

			// if win, alle restliche Flaggen werden automatisch ergänzt
			if (m_nGameStatus == GS_WIN) {
				if (m_parrBtnMines[cRow][cCol].IsMine())
					m_imgLst.Draw(pDC, BTM_FLAG, rcFont2.TopLeft(), ILD_TRANSPARENT);
			}
			// if lost, überschreiben für das flasch mit Flagge makierte Feld
			if (m_nGameStatus == GS_LOST) {
				if( nStatus == MN_FLAG && !m_parrBtnMines[cRow][cCol].IsMine())
					m_imgLst.Draw(pDC, BTM_FLAG_WRONG, rcFont2.TopLeft(), ILD_TRANSPARENT);
			}
			// Unpressed Button im "3D-Look" 
			// 3D Effekt hat eine feste Breite, und DeflateRect setzt Offset.
			pDC->Draw3dRect(rcWnd, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
			rcWnd.DeflateRect(1, 1);
			pDC->Draw3dRect(rcWnd, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));            
			rcWnd.DeflateRect(1, 1);
			pDC->Draw3dRect(rcWnd, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
		}

		//Knopf wurde schon gedrückt
		else
		{
			// wenn es rundum Mine(n) gibt, Nummer zu zeigen
			if (nNumber > 0)
			{
				pDC->SetTextColor(ChooseColor(nNumber));
				pDC->DrawText(i2CS(nNumber), rcFont, DT_LEFT);
				// BF_RECT: Entire border rectangle.
				pDC->DrawEdge(rcWnd, BDR_SUNKENINNER, BF_RECT);
			}

			if (nNumber == 0)	//Keine Minen als Nachbarn 
			{
				// auch für leeren Knopf wird Linie gezeichnet
				pDC->DrawEdge(rcWnd, BDR_SUNKENINNER, BF_RECT);
			}

			if (m_parrBtnMines[cRow][cCol].IsMine())	//Feld enthält eine Mine
			{
				pDC->DrawEdge(rcWnd, BDR_SUNKENINNER, BF_RECT);
				// CRect(POINT topLeft, POINT bottomRight) throw();		
				// POINT TOPLEFT anpassen, um die Mine in die Mitte zu stellen
				CRect rcFont2(CPoint((int)(nWidth*0.2), (int)(nHeight*0.2)), CPoint(nWidth, nHeight));

				// die Mine, die tatsächlich gedrückt wurde, wird mit Sonderzeichen (in Rot) angezeigt
				if (nStatus == MN_CRITICAL)
					m_imgLst.Draw(pDC, BTM_MINE_R, rcFont2.TopLeft(), ILD_TRANSPARENT);
				if (nStatus == MN_NORMAL)
					m_imgLst.Draw(pDC, BTM_MINE, rcFont2.TopLeft(), ILD_TRANSPARENT);
			}
		}
		// Danach wird CFont gelöscht, aber pDC nicht. d.h. pDC zeigt auf eine ungültige Adresse.
		pDC->SelectObject( pOldFont);
		return;
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

//***************************************************************************************
// automatische Farbe wählen
//***************************************************************************************
COLORREF CDlgMineSweeper::ChooseColor(int nNumber) {
	int triColor = nNumber - 1;
	int n = 1 + triColor / 3;
	int remainder = (int)fmod(triColor, 3);
	switch (remainder)
	{
	case 0:
		return RGB(100, 255-80*n, 100);
	case 1:
		return RGB(255-80*n, 100, 100);
	default:	// default ist hier 2
		return RGB(100, 100, 255-80*n);
	}
}


//***************************************************************************************
// Timer aktualisieren
//***************************************************************************************
void CDlgMineSweeper::OnTimer(UINT_PTR nIDEvent)
{
	// je 1000 Milisekunde
	m_nTimer++;
	UpdateData(FALSE);

	CDialog::OnTimer(nIDEvent);
}


//***************************************************************************************
// Für Anzeige aller Zahlen in Debug Mode
//***************************************************************************************
void CDlgMineSweeper::Checker() {
	for (int i = 0; i != m_nRows; ++i) {
		for (int j = 0; j != m_nCols; ++j) {
			CString strNum;
			int cMines = m_parrBtnMines[i][j].GetNumber();
			strNum.Format(_T("%d"), cMines);
			m_parrBtnMines[i][j].SetWindowText(strNum);
		}
	}
}


//BOOL CDlgMineSweeper::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	if (wParam >= IDC_BUTTON_FIRST && wParam < IDC_BUTTON_FIRST + m_nRows * m_nCols)
//  {
//	}
//	return CDialog::OnCommand(wParam, lParam);
//}

