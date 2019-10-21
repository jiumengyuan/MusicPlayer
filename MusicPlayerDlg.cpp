
// MusicPlayerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MusicPlayer.h"
#include "MusicPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMusicPlayerDlg 对话框



CMusicPlayerDlg::CMusicPlayerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MUSICPLAYER_DIALOG, pParent)
	, m_nVolume(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_DeviceID = 0;
}

void CMusicPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
	DDX_Text(pDX, IDC_EDIT1, m_nVolume);
}

BEGIN_MESSAGE_MAP(CMusicPlayerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUT_OPEN, &CMusicPlayerDlg::OnBnClickedButOpen)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CMusicPlayerDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CMusicPlayerDlg::OnBnClickedBtnPause)
	ON_BN_CLICKED(IDC_BUN_STOP, &CMusicPlayerDlg::OnBnClickedBunStop)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMusicPlayerDlg::OnCustomdrawSlider1)
END_MESSAGE_MAP()


// CMusicPlayerDlg 消息处理程序

BOOL CMusicPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_slider.SetRange(0, 1000);
	m_slider.SetPos(1000);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMusicPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CPaintDC dc(this);
		//CRect rect;
		//GetClientRect(&rect);
		//dc.FillSolidRect(&rect,RGB(192,192,0));
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMusicPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMusicPlayerDlg::OnBnClickedButOpen()
{
	//MessageBox(L"点击了", L"提示", MB_OK);
	CFileDialog dlg(TRUE,NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,L"MP3文件(*.mp3)|*.mp3|WMA文件(*.wma)|*.wma|WAV文件(*.wav)|*.wav|所有文件(*.*)|*.*||");
	if (IDCANCEL == dlg.DoModal())
		return;
	CString strMusicFile = dlg.GetPathName();
	//MessageBox(strMusicFile);
	MCI_OPEN_PARMS mciOpenParms;
	mciOpenParms.lpstrElementName = strMusicFile;
	MCIERROR mciErro=mciSendCommand(NULL,MCI_OPEN,MCI_OPEN_ELEMENT|MCI_WAIT,(DWORD)&mciOpenParms);
	if (mciErro)
	{
		wchar_t szErrorMsg[256];
		mciGetErrorString(mciErro, szErrorMsg, 256);
		MessageBox(szErrorMsg, L"音乐播放器");
		return;
	}
	m_DeviceID = mciOpenParms.wDeviceID;
	OnBnClickedBtnPlay();
}

void CMusicPlayerDlg::OnBnClickedBtnPlay()
{
	MCI_PLAY_PARMS mciplayParms;
	mciplayParms.dwCallback = NULL;
	mciplayParms.dwFrom = 0;
	mciSendCommand(m_DeviceID, MCI_PLAY, MCI_FROM | MCI_NOTIFY, (DWORD)&mciplayParms);
}

void CMusicPlayerDlg::OnBnClickedBtnPause()
{
	CString str;
	GetDlgItemText(IDC_BTN_PAUSE, str);
	if (str == L"暂停")
	{
		mciSendCommand(m_DeviceID, MCI_PAUSE, 0, 0);
		SetDlgItemText(IDC_BTN_PAUSE, L"继续");
	}
	else if (str == "继续")
	{
		mciSendCommand(m_DeviceID, MCI_RESUME, 0, 0);
		SetDlgItemText(IDC_BTN_PAUSE, L"暂停");
	}
}

void CMusicPlayerDlg::OnBnClickedBunStop()
{
	mciSendCommand(m_DeviceID, MCI_STOP, 0, 0);
	mciSendCommand(m_DeviceID, MCI_CLOSE, 0, 0);
}


void CMusicPlayerDlg::OnCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_nVolume = m_slider.GetPos()/10;
	UpdateData(FALSE);
	MCI_DGV_SETAUDIO_PARMS mciSetVolume;
	mciSetVolume.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciSetVolume.dwValue = m_slider.GetPos();
	mciSendCommand(m_DeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD)&mciSetVolume);
	*pResult = 0;
}
