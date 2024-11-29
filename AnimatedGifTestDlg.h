
// AnimatedGifTestDlg.h : header file
//

#pragma once

#include <gdiplus.h>
//using namespace Gdiplus;

#include <memory>
#include <vector>

class GdiPlusWrapper {
public:
    GdiPlusWrapper() {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    }
    GdiPlusWrapper(const GdiPlusWrapper&) = delete;
    GdiPlusWrapper& operator=(const GdiPlusWrapper&) = delete;

    ~GdiPlusWrapper() {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }

private:
    ULONG_PTR m_gdiplusToken;
};


// CAnimatedGifTestDlg dialog
class CAnimatedGifTestDlg : public CDialogEx
{
// Construction
public:
	CAnimatedGifTestDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATEDGIFTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
    GdiPlusWrapper m_gdiplusWrapper;
    std::unique_ptr<Gdiplus::Image> m_pImage;
    UINT m_frameCount{};
    UINT m_currentFrame{};
    std::vector<UINT> m_pFrameDelays;
    HWND m_messageHWND{};
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
