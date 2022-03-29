
// sortDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "sort.h"
#include "sortDlg.h"
#include "afxdialogex.h"
#include <Dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
BOOL g_bstop = FALSE;
typedef CListBox* PCListBox;
// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CsortDlg



CsortDlg::CsortDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SORT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CsortDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_DEVICECHANGE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBOSOURCE, &CsortDlg::OnCbnSelchangeComboSource)
	ON_BN_CLICKED(IDC_BUTTONBROWSE, &CsortDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_COPY, &CsortDlg::OnBnClickedCopy)
	ON_BN_CLICKED(IDC_BUTTONDEL, &CsortDlg::OnBnClickedButtondel)
	ON_BN_CLICKED(IDC_BUTTONFORMAT, &CsortDlg::OnBnClickedButtonformat)
	ON_CBN_SELCHANGE(IDC_COMBODEST, &CsortDlg::OnCbnSelchangeCombodest)
	ON_BN_CLICKED(IDC_BUTTONSTOP, &CsortDlg::OnBnClickedButtonstop)
	ON_MESSAGE(UM_REFRESH_LIST, &CsortDlg::OnRefreshList)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &CsortDlg::OnNMCustomdrawProgress1)
END_MESSAGE_MAP()

DWORD WINAPI ChangeThread(LPVOID lpParameter)
{
	CsortDlg* pDlg = reinterpret_cast<CsortDlg*>(lpParameter);
	const HANDLE cHandle = pDlg->m_dwChangeHandles;
	//PCListBox  pLBDest = (PCListBox)pDlg->GetDlgItem(IDC_LISTDEST);
	
	for(;;)
	{
		// Wait for notification.

		DWORD dwWaitStatus = WaitForSingleObject(cHandle, INFINITE);

		switch (dwWaitStatus)
		{
		case WAIT_OBJECT_0:

			// A file was created, renamed, or deleted in the directory.
			// Refresh this directory and restart the notification.

			pDlg->SendMessage(UM_REFRESH_LIST, 0, 0);
			if (FindNextChangeNotification(cHandle) == FALSE)
			{
				AfxMessageBox(L"Ошибка: FindNextChangeNotification function failed.", MB_ICONEXCLAMATION);
				ExitProcess(GetLastError());
			}
			break;

		case WAIT_FAILED:
			AfxMessageBox(L"Ошибка: WaitForMultipleObjects function failed", MB_ICONEXCLAMATION);
			ExitProcess(GetLastError());
			break;
		}
	}
	return 0;
}
void CsortDlg::ReadDrive()
{
	DWORD dr = GetLogicalDrives(); // функция возвращает битовую маску
	DWORD copythreadID;
	WCHAR strPath[4] = L"A:\\";
	CComboBox* pCBDest = (CComboBox*)GetDlgItem(IDC_COMBODEST);
	pCBDest->ResetContent();
	for (int x = 0; x < 26; x++) // проходимся циклом по битам
	{
		if ((dr >> x) & 1) // если единица - диск с номером x есть
		{
			strPath[0] = L'A' + x;
			UINT drive_type = GetDriveTypeW(strPath);
			if (drive_type == DRIVE_REMOVABLE)
				pCBDest->AddString(strPath);
		}
	}
	int n = pCBDest->GetCount();
	if (n > 0)
	{
		pCBDest->SetCurSel(n - 1);
		if (m_dwChangeHandles)
		{
			FindCloseChangeNotification(m_dwChangeHandles);
			CloseHandle(m_dwChangeHandles);
		}

		m_dwChangeHandles = FindFirstChangeNotification(
			strPath,                       // directory to watch 
			FALSE,                         // do not watch subtree 
			FILE_NOTIFY_CHANGE_FILE_NAME);
		if (m_dwChangeHandles != INVALID_HANDLE_VALUE)
			if (!CreateThread(0, 0, ChangeThread, this, 0, &copythreadID))
			{
				AfxMessageBox(L"Не могу создать поток", MB_ICONEXCLAMATION);
				ExitProcess(GetLastError());
			}
		OnCbnSelchangeCombodest();
	}
}
BOOL CsortDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	m_dwChangeHandles = NULL;
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок
	
	ReadDrive();

	//CComboBox* pCBSource = (CComboBox*)GetDlgItem(IDC_COMBODEST);
	//HANDLE hNotify = FindFirstChangeNotificationW();
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CsortDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CsortDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CsortDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CsortDlg::FindFile(CString &strPath, DWORD idList)
{
	WIN32_FIND_DATA fd;
	HANDLE hFindFile = FindFirstFileW(strPath + L"\\*.*", &fd);
	PCListBox pLBSource = (PCListBox)GetDlgItem(idList);
	pLBSource->ResetContent();
	int n = 0, size;
	while (FindNextFileW(hFindFile, &fd))
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		pLBSource->AddString(fd.cFileName);
		size = fd.nFileSizeLow >> 10;
		pLBSource->SetItemData(n++, ++size);
	}
}

void CsortDlg::OnCbnSelchangeComboSource()
{
	CComboBox* pCBSource = (CComboBox*)GetDlgItem(IDC_COMBOSOURCE);
	int n = pCBSource->GetCurSel();
	CString strPath;
	pCBSource->GetLBText(n, strPath);
	FindFile(strPath, IDC_LISTSOURCE);
}


void CsortDlg::OnBnClickedButtonBrowse()
{
	CString fileName;
	WCHAR* p = fileName.GetBuffer(MAX_PATH);
	CFolderPickerDialog dlgFile(0,0,this,sizeof(OPENFILENAME));
	OPENFILENAME& ofn = dlgFile.GetOFN();
	ofn.lpstrFile = p;

	if (dlgFile.DoModal() == IDOK)
	{
		fileName.ReleaseBuffer();
		CComboBox* pCBSource = (CComboBox*)GetDlgItem(IDC_COMBOSOURCE);
		pCBSource->AddString(fileName);
		int n = pCBSource->GetCount();
		pCBSource->SetCurSel(n - 1);
		WIN32_FIND_DATA fd;
		HANDLE hFindFile = FindFirstFileW(fileName + L"\\*.*", &fd);
		PCListBox pLBSource = (PCListBox)GetDlgItem(IDC_LISTSOURCE);
		pLBSource->ResetContent();
		n = 0; 
		int size, s = 0;
		while (FindNextFileW(hFindFile, &fd))
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
			pLBSource->AddString(fd.cFileName);
			size = (fd.nFileSizeLow >> 10) + 1;
			s += size;
			pLBSource->SetItemData(n++, (DWORD_PTR)size);
		}
	}
}

DWORD WINAPI copyThread(LPVOID lpParameter)
{
	CsortDlg  *pDlg      = reinterpret_cast<CsortDlg*>(lpParameter);
	PCListBox  pLBSource = (PCListBox)pDlg->GetDlgItem(IDC_LISTSOURCE);
	PCListBox  pLBDest   = (PCListBox)pDlg->GetDlgItem(IDC_LISTDEST);
	CComboBox *pCBSource = (CComboBox*)pDlg->GetDlgItem(IDC_COMBOSOURCE);
	CComboBox *pCBDest   = (CComboBox*)pDlg->GetDlgItem(IDC_COMBODEST);
	CProgressCtrl* pProgress = (CProgressCtrl*)pDlg->GetDlgItem(IDC_PROGRESS);
	CButton* pButtonStop = (CButton*)pDlg->GetDlgItem(IDC_BUTTONSTOP);
	
	CString strPath, strName, strDest;
	pCBSource->GetWindowText(strPath);
	pCBDest->GetWindowText(strDest);
	strPath += L'\\';
	strDest += L'\\';
	int n = pLBSource->GetCount();
	int prpos = 0;
	for (int i = 0, j = 0; i < n; i++)
	{
		if (!g_bstop)
		{
			pLBSource->GetText(j, strName);
			if (CopyFileW(strPath + strName, strDest + strName, 0))
			{
				if (pLBDest->FindString(-1, strName) == LB_ERR)				
					pLBDest->AddString(strName);
				pLBSource->DeleteString(j);			
			}
			else
			{
				j++;
				if (AfxMessageBox(L"Не могу скопировать файл. Продолжить копирование?" + strName, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
					return 0;
			}
			prpos += (int)pLBSource->GetItemData(j);
			pProgress->SetPos(prpos);
		}
		else
		{
			g_bstop = FALSE;
			break;
		}
	}
	pProgress->ShowWindow(0);
	pButtonStop->EnableWindow(0);
	return 0;
}

void CsortDlg::OnBnClickedCopy()
{
	CString strDest;
	DWORD copythreadID;
	PCListBox pLBSource = (PCListBox)GetDlgItem(IDC_LISTSOURCE);
	int n = pLBSource->GetCount();
	if (n > 0)
	{
		__int64 s = 0, space;
		for (int i = 0; i < n; i++)
			s = s + pLBSource->GetItemData(i);
		CComboBox* pCBDest = (CComboBox*)GetDlgItem(IDC_COMBODEST);
		int m = pCBDest->GetCurSel();
		if (m >= 0)
		{
			CButton* pButtonStop = (CButton*)GetDlgItem(IDC_BUTTONSTOP);
			pButtonStop->EnableWindow();
			CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
			int size = 0;
			for (int i = 0; i < n; i++)
				size += pLBSource->GetItemData(i);
			pProgress->SetRange(0, size);
			pProgress->SetPos(0);
			pProgress->ShowWindow(1);
			pCBDest->GetLBText(m, strDest);
			GetDiskFreeSpaceExW(strDest, 0, 0, (PULARGE_INTEGER)&space);
			if (space < s)
				if (AfxMessageBox(L"Места не хватит", MB_OKCANCEL) == IDCANCEL)
					return;
			CreateThread(0, 0, copyThread, this, 0, &copythreadID);
		}
		else AfxMessageBox(L"Флешка не выбрана");
	}
	else AfxMessageBox(L"Файлы не выбраны");
}


void CsortDlg::OnBnClickedButtondel()
{
	PCListBox pLBSource = (PCListBox)GetDlgItem(IDC_LISTSOURCE);
	int n = pLBSource->GetCurSel();
	if (n>=0)
		pLBSource->DeleteString(n);
}


void CsortDlg::OnBnClickedButtonformat()
{
	CString strDest;
	CComboBox* pCBDest = (CComboBox*)GetDlgItem(IDC_COMBODEST);
	int n = pCBDest->GetCurSel();
	if (n >= 0)
	{
		pCBDest->GetLBText(n, strDest);
		if (!SHFormatDrive(0, strDest[0] - L'A', SHFMT_OPT_FULL, 1))
		{
			PCListBox pLBDest = (PCListBox)GetDlgItem(IDC_LISTDEST);
			pLBDest->ResetContent();
		}
	}
	else AfxMessageBox(L"Флешка не выбрана");
}
BOOL CsortDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	if (nEventType == DBT_DEVICEARRIVAL || nEventType == DBT_DEVICEREMOVECOMPLETE)
		ReadDrive();
	
	return TRUE;
}


void CsortDlg::OnCbnSelchangeCombodest()
{
	CComboBox* pCBDest = (CComboBox*)GetDlgItem(IDC_COMBODEST);
	int n = pCBDest->GetCurSel();
	if (n >= 0)
	{
		CString strPath;
		pCBDest->GetLBText(n, strPath);
		WIN32_FIND_DATA fd;
		HANDLE hFindFile = FindFirstFileW(strPath + L"\\*.*", &fd);
		PCListBox pLBSource = (PCListBox)GetDlgItem(IDC_LISTDEST);
		pLBSource->ResetContent();
		while (FindNextFileW(hFindFile, &fd))
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
			pLBSource->AddString(fd.cFileName);
		}
	}
}


void CsortDlg::OnBnClickedButtonstop()
{
	g_bstop = TRUE;
	CButton* pButtonStop = (CButton*)GetDlgItem(IDC_BUTTONSTOP);
	pButtonStop->EnableWindow(0);
}

LRESULT CsortDlg::OnRefreshList(WPARAM wp,LPARAM lp)
{
	OnCbnSelchangeCombodest();
	return 0;
}

