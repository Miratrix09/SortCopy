
// sortDlg.h: файл заголовка
//

#pragma once

#define UM_REFRESH_LIST WM_USER+1
// Диалоговое окно CsortDlg
class CsortDlg : public CDialogEx
{
// Создание
public:
	CsortDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SORT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV
	void FindFile(CString&,DWORD);
	void ReadDrive();

// Реализация
public:
	HANDLE m_dwChangeHandles;

protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboSource();
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedButtondel();
	afx_msg void OnBnClickedButtonformat();
	afx_msg void OnCbnSelchangeCombodest();
	afx_msg void OnBnClickedButtonstop();
	afx_msg LRESULT OnRefreshList(WPARAM, LPARAM);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

};
