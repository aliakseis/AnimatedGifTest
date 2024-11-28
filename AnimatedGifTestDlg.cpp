
// AnimatedGifTestDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "AnimatedGifTest.h"
#include "AnimatedGifTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static std::unique_ptr<Gdiplus::Image> LoadFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
    HRSRC hResource = FindResource(hInst, pName, pType);
    if (!hResource)
        return false;

    DWORD imageSize = SizeofResource(hInst, hResource);
    if (!imageSize)
        return false;

    HGLOBAL hResourceData = LoadResource(hInst, hResource);
    if (!hResourceData)
        return false;

    const void* pResourceData = LockResource(hResourceData);
    if (!pResourceData)
        return false;

    HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, imageSize);
    if (!hBuffer)
        return false;

    {
        void* pBuffer = GlobalLock(hBuffer);
        if (!pBuffer)
        {
            GlobalFree(hBuffer);
            return false;
        }

        CopyMemory(pBuffer, pResourceData, imageSize);

        GlobalUnlock(hBuffer);
    }

    IStream* pStream = NULL;
    if (CreateStreamOnHGlobal(hBuffer, TRUE, &pStream) != S_OK)
    {
        GlobalFree(hBuffer);
        return false;
    }

    std::unique_ptr<Gdiplus::Image> result(Gdiplus::Image::FromStream(pStream));
    pStream->Release();

    if (result->GetLastStatus() != Gdiplus::Ok)
    {
        return {};
    }

    return result;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CAnimatedGifTestDlg dialog



CAnimatedGifTestDlg::CAnimatedGifTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ANIMATEDGIFTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnimatedGifTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAnimatedGifTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CAnimatedGifTestDlg message handlers

BOOL CAnimatedGifTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    m_pImage = LoadFromResource(MAKEINTRESOURCE(IDR_SPLASH_GIF), RT_RCDATA, AfxGetInstanceHandle());
    if (!m_pImage)
    {
        return FALSE;
    }

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// extra initialization here
    m_frameCount = m_pImage->GetFrameCount(&Gdiplus::FrameDimensionTime);
    m_pFrameDelays.resize(m_frameCount);
    Gdiplus::PropertyItem* pItem;
    UINT size = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
    pItem = (Gdiplus::PropertyItem*)malloc(size);
    m_pImage->GetPropertyItem(PropertyTagFrameDelay, size, pItem);
    for (UINT i = 0; i < m_frameCount; ++i) {
        m_pFrameDelays[i] = (static_cast<UINT*>(pItem->value))[i] * 10; // Convert to milliseconds
    }
    free(pItem);

    SetTimer(1, m_pFrameDelays[m_currentFrame], NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAnimatedGifTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAnimatedGifTestDlg::OnPaint()
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
        // Get the dimensions of the dialog
        CRect rect;
        GetClientRect(&rect);

        if (m_pImage)
        {

            // Get the dimensions of the image
            int imgHeight = rect.Height() * 3 / 4;
            //int imgWidth = (std::min)(imgHeight * image.GetWidth() / image.GetHeight(), rect.Width());
            int imgWidth = rect.Width() - 10;

            // imgHeight / image.GetHeight() > imgWidth / image.GetWidth()
            if (imgHeight * m_pImage->GetWidth() > imgWidth * m_pImage->GetHeight())
            {
                imgHeight = imgWidth * m_pImage->GetHeight() / m_pImage->GetWidth();
            }
            else
            {
                imgWidth = imgHeight * m_pImage->GetWidth() / m_pImage->GetHeight();
            }

            // Calculate the position to center the image
            int x = (rect.Width() - imgWidth) / 2;
            int y = (rect.Height() - imgHeight) / 3;

            // Draw the image at the calculated position
            //image.Draw(dc.m_hDC, x, y, imgWidth, imgHeight);
            CPaintDC dc(this); // device context for painting
            Gdiplus::Graphics graphics(dc);
            m_pImage->SelectActiveFrame(&Gdiplus::FrameDimensionTime, m_currentFrame);
            graphics.DrawImage(m_pImage.get(), x, y, imgWidth, imgHeight);
        }
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAnimatedGifTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAnimatedGifTestDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        m_currentFrame = (m_currentFrame + 1) % m_frameCount;
        InvalidateRect(NULL, FALSE);
        SetTimer(1, m_pFrameDelays[m_currentFrame], NULL);
    }
    CDialogEx::OnTimer(nIDEvent);
}
