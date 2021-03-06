// DlgRender.cpp : 实现文件
//

#include "stdafx.h"
#include "CarEyePlayer.h"
#include "DlgRender.h"
#include "afxdialogex.h"

#include "../libCarEyePlayer/libCarEyePlayerAPI.h"

// CDlgRender 对话框

IMPLEMENT_DYNAMIC(CDlgRender, CDialogEx)

CDlgRender::CDlgRender(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRender::IDD, pParent)
{
	memset(&channelStatus, 0x00, sizeof(CHANNELSTATUS));
	hMenu		=	NULL;

	m_pCarEyeLogo = NULL;
	channelStatus.showOSD = 0;
	mChannelId	=	0;
}

CDlgRender::~CDlgRender()
{
	ClosePopupMenu();
	UIRenderEngine->RemoveImage(m_pCarEyeLogo);

}

void CDlgRender::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRender, CDialogEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgRender 消息处理程序
void CDlgRender::ClosePopupMenu()
{
	if (NULL != hMenu)
	{
		DestroyMenu(hMenu);
		hMenu = NULL;
	}
}

void CDlgRender::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::PostMessage(GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, 0, 0);

	CDialogEx::OnLButtonDblClk(nFlags, point);
}

#define	POP_MENU_RECORDING	10010
#define	POP_MENU_SHOT			10011
#define POP_MENU_SHOWOSD	10012

void CDlgRender::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClosePopupMenu();

	hMenu = CreatePopupMenu();
	if (NULL != hMenu)
	{
		AppendMenu(hMenu, MF_STRING|(channelStatus.recording==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_RECORDING, TEXT("Recording"));
		AppendMenu(hMenu, MF_STRING, POP_MENU_SHOT, TEXT("ScreenShot"));
		AppendMenu(hMenu, MF_STRING|(channelStatus.showOSD==0x01?MF_CHECKED:MF_UNCHECKED), POP_MENU_SHOWOSD, TEXT("ShowOSD"));

		CPoint	pMousePosition;
		GetCursorPos(&pMousePosition);
		SetForegroundWindow();
		TrackPopupMenu(hMenu, TPM_LEFTALIGN, pMousePosition.x, pMousePosition.y, 0, GetSafeHwnd(), NULL);
	}

	CDialogEx::OnRButtonUp(nFlags, point);
}


BOOL CDlgRender::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD	wID = (WORD)wParam;
	switch (wID)
	{
	case POP_MENU_RECORDING:
		{
			//channelStatus.recording = (channelStatus.recording==0x00?0x01:0x00);
			if (mChannelId > 0)
			{
				channelStatus.recording = (channelStatus.recording==0x00?0x01:0x00);

				if (channelStatus.recording == 0x01)			CarEyePlayer_StartManuRecording(mChannelId);
				else											CarEyePlayer_StopManuRecording(mChannelId);
			}
		}
		break;
	case POP_MENU_SHOT:
		{
			if (mChannelId > 0)
			{
				CarEyePlayer_StartManuPicShot(mChannelId);
				// 				channelStatus.shoting = (channelStatus.shoting==0x00?0x01:0x00);
				// 
				// 				if (channelStatus.shoting == 0x01)			CarEyePlayer_StartManuPicShot(mChannelId);
				// 				else											CarEyePlayer_StopManuPicShot(mChannelId);
			}
		}
		break;
	case POP_MENU_SHOWOSD:
		if (mChannelId > 0)
		{
			CAREYE_MEDIA_INFO mediaInfo;
			memset(&mediaInfo, 0, sizeof(CAREYE_MEDIA_INFO));
			CarEyePlayer_GetMediaInfo(mChannelId, mediaInfo);

			channelStatus.showOSD = (channelStatus.showOSD==0x00?0x01:0x00);
#if 1	//OSD Example
			CAREYE_PALYER_OSD osd;
			osd.alpha = 255;
			osd.size = 35;
			osd.color = RGB(255,0,255);
			osd.rect.left = 10;
			osd.rect.right = 5000;
			osd.rect.top = 100;
			osd.rect.bottom = 800;
			osd.shadowcolor = RGB(0,0,0);
			//char* ss =  "这是CarEyePlayer-RTSP-Win播放器 \r\n的字幕叠加接口的效果！！\r\n以\"\\r\\n\"为换行结束符号\r\n注意：每行的长度不能超过128个字节\r\n总的OSD长度不能超过1024个字节";
			char sMediaInfo[512] = {0,};
			sprintf(sMediaInfo, "媒体信息：分辨率：%d*%d  fps: %d  音频采样率：%d 音频通道：%d  音频采样位宽： %d ",
				mediaInfo.width, mediaInfo.height, mediaInfo.fps, mediaInfo.sample_rate, mediaInfo.channels, mediaInfo.bits_per_sample);
			strcpy(osd.stOSD ,sMediaInfo);
			CarEyePlayer_ShowOSD(mChannelId, channelStatus.showOSD,  osd);

#endif

		}
		break;
	default:
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CDlgRender::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	CBrush brushBkgnd; 
	CRect rcClient;
	brushBkgnd.CreateSolidBrush(RGB(0, 0, 0));
	GetClientRect(&rcClient);
	dc.FillRect(&rcClient, &brushBkgnd);
	brushBkgnd.DeleteObject(); //释放画刷 

	if ( m_pCarEyeLogo != NULL && !m_pCarEyeLogo->IsNull() )
	{
		int nStartX = (rcClient.Width()-184)/2;
		int nStartY =  (rcClient.Height()-184)/2;
		m_pCarEyeLogo->DrawImage(CDC::FromHandle(dc.m_hDC),nStartX,nStartY);
	}
}


BOOL CDlgRender::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pCarEyeLogo = UIRenderEngine->GetImage(TEXT("Res\\CarEyeTeam\\CarEyelogo.png"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgRender::SetChannelStatus(int recording,int	showOSD)
{
	channelStatus.recording = recording;
	channelStatus.showOSD = showOSD;

}
