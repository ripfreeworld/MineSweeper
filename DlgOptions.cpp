//***************************************************************************************
// Einstellungsdialog
//***************************************************************************************
#include "pch.h"
#include "MineSweeper.h"
#include "DlgOptions.h"
#include "afxdialogex.h"
#include "Resource.h"


IMPLEMENT_DYNAMIC(CDlgOptions, CDialog)

//***************************************************************************************
// Konstruktor
//***************************************************************************************
CDlgOptions::CDlgOptions(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_OPTIONS, pParent)
	, m_nRows(0)
	, m_nCols(0)
	, m_nMines(0)
	, m_nCustRows(16)
	, m_nCustCols(30)
	, m_nCustMines(99)
	, m_nLevel(0)
	, m_bMarks(FALSE)
{
	const TCHAR *pszKey = _T("2");
	// load numbers from registry
	m_nCustRows = AfxGetApp()->GetProfileInt(pszKey, _T("CUSTROWS"), 9); // 9 falls nicht existiert
	m_nCustCols = AfxGetApp()->GetProfileInt(pszKey, _T("CUSTCOLS"), 9);
	m_nCustMines = AfxGetApp()->GetProfileInt(pszKey, _T("CUSTMINES"), 10);
	m_nLevel = AfxGetApp()->GetProfileInt(pszKey, _T("LEVEL"), 0);		 // load from level for normal modes
	m_bMarks = AfxGetApp()->GetProfileInt(pszKey, _T("MARKS"), 0);
}

//***************************************************************************************
// Destruktor
//***************************************************************************************
CDlgOptions::~CDlgOptions()
{}

//***************************************************************************************
// Datenaustausch
//***************************************************************************************
void CDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CUST_ROWS, m_nCustRows);
	DDV_MinMaxInt(pDX, m_nCustRows, 1, 50);
	DDX_Text(pDX, IDC_CUST_COLS, m_nCustCols);
	DDV_MinMaxInt(pDX, m_nCustCols, 1, 50);
	DDX_Text(pDX, IDC_CUST_MINES, m_nCustMines);
	DDX_Radio(pDX, IDC_BEGINNER, m_nLevel);	// radio button
	DDX_Check(pDX, IDC_MARKS, m_bMarks);
	DDX_Control(pDX, IDC_CUST_ROWS, m_edtCustRows);
	DDX_Control(pDX, IDC_CUST_COLS, m_edtCustCols);
	DDX_Control(pDX, IDC_CUST_MINES, m_edtCustMines);
}


//***************************************************************************************
// Message Map
//***************************************************************************************
BEGIN_MESSAGE_MAP(CDlgOptions, CDialog)
	// ausgrauen wenn man non-Cust wählt
	ON_BN_CLICKED(IDC_BEGINNER, &CDlgOptions::OnNonCustom)
	ON_BN_CLICKED(IDC_INTERMEDIATE, &CDlgOptions::OnNonCustom)
	ON_BN_CLICKED(IDC_EXPERT, &CDlgOptions::OnNonCustom)
	ON_BN_CLICKED(IDC_CUSTOM, &CDlgOptions::OnCustom)
END_MESSAGE_MAP()


//***************************************************************************************
// Dialog initialisieren
//***************************************************************************************
BOOL CDlgOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_nLevel != LV_CUSTOM) 
		OnNonCustom();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

//***************************************************************************************
// "Neues Spiel" gedrückt
//***************************************************************************************
void CDlgOptions::OnOK()
{
	UpdateData(TRUE);
	switch (m_nLevel)	// m_nLevel von RadioBox
	{
		case LV_BEGINNER:
			m_nRows = 9;
			m_nCols = 9;
			m_nMines = 10;
			break;
		case LV_INTERMEDIATE:
			m_nRows = 16;
			m_nCols = 16;
			m_nMines = 40;
			break;
		case LV_EXPERT:
			m_nRows = 16;
			m_nCols = 30;
			m_nMines = 99;
			break;
		case LV_CUSTOM:
			m_nRows = m_nCustRows;
			m_nCols = m_nCustCols;
			m_nMines = m_nCustMines;
			break;
	}
	// Falls Input (die Anzahl von Minen) ungültig
	if (   m_nMines < 0.05* m_nRows * m_nCols
		|| m_nMines > m_nRows * m_nCols)
	{
		CString str, strCaption;
		str.Format(IDS_MB_MAX_MINES, (int)(m_nRows*m_nCols*0.05), m_nRows*m_nCols);
		strCaption.LoadString(IDS_MLD_ERROR);
		// Wnd::MessageBox (Text, Caption, Type)
		MessageBox( str, strCaption, MB_ICONERROR | MB_OK);
		// den Dialog nicht verlassen, d.h., CDialog::OnOK() wird nicht aufgerufen
		return;
	}
	// Speichern nur wenn m_nMines die "max" nicht überschreitet
	const TCHAR *pszKey = _T("2");	// Unterordner ""
	AfxGetApp()->WriteProfileInt(pszKey, _T("CUSTROWS"), m_nCustRows);
	AfxGetApp()->WriteProfileInt(pszKey, _T("CUSTCOLS"), m_nCustCols);
	AfxGetApp()->WriteProfileInt(pszKey, _T("CUSTMINES"), m_nCustMines);
	AfxGetApp()->WriteProfileInt(pszKey, _T("LEVEL"), m_nLevel);
	AfxGetApp()->WriteProfileInt(pszKey, _T("MARKS"), m_bMarks);

	CDialog::OnOK();
}


//***************************************************************************************
// Typ gewählt
//***************************************************************************************
void CDlgOptions::OnNonCustom()
{
	//Custom-TextEditor ausgrauen
	m_edtCustRows.EnableWindow(FALSE);
	m_edtCustCols.EnableWindow(FALSE);
	m_edtCustMines.EnableWindow(FALSE);
}
void CDlgOptions::OnCustom()
{
	// aktivieren in Default
	m_edtCustRows.EnableWindow();
	m_edtCustCols.EnableWindow();
	m_edtCustMines.EnableWindow();
}


