// DockXmlView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DuiEditor.h"
#include "DockXmlView.h"
#include "SciXmlWriter.h"
#include "DuiEditorTabView.h"

#include "TinyXml2/tinyxml2.h"
// CDockXmlView

IMPLEMENT_DYNAMIC(CDockXmlView, CWnd)

CDockXmlView::CDockXmlView()
{
	m_nTargetLine = -1;
}

CDockXmlView::~CDockXmlView()
{
}


BEGIN_MESSAGE_MAP(CDockXmlView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_SCIWND_CLICK, OnSciClick)
	ON_COMMAND(ID_SCI_UNDO, &CDockXmlView::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_SCI_UNDO, &CDockXmlView::OnUpdateEditUndo)
	ON_COMMAND(ID_SCI_REDO, &CDockXmlView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_SCI_REDO, &CDockXmlView::OnUpdateEditRedo)
	ON_COMMAND(ID_SCI_CUT, &CDockXmlView::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_SCI_CUT, &CDockXmlView::OnUpdateEditCut)
	ON_COMMAND(ID_SCI_COPY, &CDockXmlView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_SCI_COPY, &CDockXmlView::OnUpdateEditCopy)
	ON_COMMAND(ID_SCI_PASTE, &CDockXmlView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_SCI_PASTE, &CDockXmlView::OnUpdateEditPaste)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SCI_UPDATE_DESIGN, &CDockXmlView::OnSciUpdateDesign)
	ON_UPDATE_COMMAND_UI(ID_SCI_UPDATE_DESIGN, &CDockXmlView::OnUpdateSciUpdateDesign)
	ON_COMMAND(ID_SCI_DELETE, &CDockXmlView::OnSciDelete)
	ON_UPDATE_COMMAND_UI(ID_SCI_DELETE, &CDockXmlView::OnUpdateSciDelete)
	ON_COMMAND(ID_SCI_SELECT_ALL, &CDockXmlView::OnSciSelectAll)
	ON_UPDATE_COMMAND_UI(ID_SCI_SELECT_ALL, &CDockXmlView::OnUpdateSciSelectAll)
	ON_COMMAND(ID_SCI_REFRESH, &CDockXmlView::OnSciRefresh)
	ON_UPDATE_COMMAND_UI(ID_SCI_REFRESH, &CDockXmlView::OnUpdateSciRefresh)
END_MESSAGE_MAP()



// CDockXmlView ��Ϣ��������




int CDockXmlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRect(0,0,0,0);

	// TODO:  �ڴ�������ר�õĴ�������
	if (!sci.Create(0, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN, rectDummy, this, ID_SCI_WND))
	{
		AfxMessageBox(_T("create SciLexer window error."));
		return NULL;      // δ�ܴ���
	}
	sci.InitXML(g_xmlEditorOptions);
	sci.sci_UsePopup(FALSE);
	sci.sci_ClearTextAll();

	return 0;
}


void CDockXmlView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: �ڴ˴�������Ϣ�����������
	if(sci.GetSafeHwnd())	
	{
		CRect rcClient;
		rcClient.DeflateRect(1,1,1,1);
		GetClientRect(rcClient);
		sci.MoveWindow(rcClient);
	}
}


void CDockXmlView::Init()
{
	CDuiEditorDoc *pDoc  = (CDuiEditorDoc *)m_pDoc;

	CSciXmlWriter writer(&sci);
	writer.print(pDoc->m_doc.child(_T("Window")));

	if(m_nTargetLine >= 0)
	{
		int line = m_nTargetLine;
		int lineposbegin = sci.sci_PositionFromLine(line);
		int lineposend = sci.sci_GetLineEndPosition(line);

		//��취����
		int firstline = sci.sci_GetFirstVisibleLine();
		int lastline = firstline + sci.sci_LineSonScreen();
		int centerline = firstline + (lastline - firstline)/2;
		if(line < centerline)
			sci.sci_LineScroll(0, line - centerline);
		else
			sci.sci_LineScroll(0, line - centerline);

		sci.sci_SetSel(lineposend, lineposbegin);

		m_nTargetLine = -1;
	}
}

void CDockXmlView::SelectXmlNode(CControlUI *pControl)
{
	//�л����У���ƽ�����ѡ�еĿؼ�
	xml_node node((xml_node_struct *)pControl->GetTag());
	SelectXmlNode(node);
}

void CDockXmlView::SelectXmlNode(xml_node node)
{
	//�ĵ��б䶯����������һ��
	if(sci.sci_GetModify())
	{
		Init();
	}

	if(node)
	{
		int line = node.get_row();
		int lineposbegin = sci.sci_PositionFromLine(line);
		int lineposend = sci.sci_GetLineEndPosition(line);

		//��취����
		int firstline = sci.sci_GetFirstVisibleLine();
		int lastline = firstline + sci.sci_LineSonScreen();
		int centerline = firstline + (lastline - firstline)/2;
		if(line < centerline)
			sci.sci_LineScroll(0, line - centerline);
		else
			sci.sci_LineScroll(0, line - centerline);

		sci.sci_SetSel(lineposend, lineposbegin);


// 		CString temp;
// 		temp.Format(_T("line=%d, lineposbegin=%d, lineposend=%d"), line, lineposbegin, lineposend);
// 		InsertMsg(temp);
	}
}

BOOL CDockXmlView::SelectControlUI(int line, xml_node node)
{
	if(node.get_row() == line)
	{
		CControlUI *pControl = (CControlUI *)node.get_tag();
		m_pManager->SelectItem(pControl);
		m_pManager->GetTreeView()->SelectXmlNode(node);
		return TRUE;
	}

	for (xml_node nd=node.first_child(); nd; nd=nd.next_sibling())
	{
		if(SelectControlUI(line, nd))
			return TRUE;
	}
	return FALSE;
}

LRESULT CDockXmlView::OnSciClick(WPARAM WParam, LPARAM LParam)
{
	//����ĵ�����ʱ��ͬʱѡ����ƽ����еĿؼ� �Լ� ���ؼ�
	int line = (int)WParam;
// 	CString temp;
// 	temp.Format(_T("line = %d"), line);
// 	InsertMsg(temp);

	CDuiEditorDoc *pDoc = (CDuiEditorDoc *)m_pDoc;

	//����༭���棬������Ӧ��ֱ�ӷ��� 
	if(!pDoc->GetTabView()->GetActiveView()->IsKindOf(RUNTIME_CLASS(CDuiEditorViewDesign)))
		return 0;


	xml_node root = pDoc->m_doc.root().child(_T("Window"));
	SelectControlUI(line, root);
	return 0;
}

BOOL CDockXmlView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LSSTRING_CONVERSION;

	NMHDR *phDR;
	phDR = (NMHDR*)lParam;

	if (phDR == NULL || phDR->hwndFrom != sci.m_hWnd)
	{
		return CWnd::OnNotify(wParam, lParam, pResult);
	}

	//����"<",  �г�Control
	//����"<x",  �г�����'x'��Control
	//����"</", ������, ����Ѱ��δ��յ�������, �Զ��г�δ��յĿؼ���
	//<Control a, �ҵ�Control�� �г�Control�������б�


	SCNotification *pMsg = (SCNotification*)lParam;
	sci.OnParentNotify(pMsg);

// 	CString temp;
// 	temp.Format(_T("Sci NotifyCode = %d"), pMsg->nmhdr.code);
// 	InsertMsg(temp);

	switch (pMsg->nmhdr.code)
	{
	case SCK_BACK:
		{
			InsertMsg(_T("SCK_BACK"));
		}
		break;
	case SCN_CHARADDED:	//�����ַ�
		{
			if(pMsg->ch == '<' || pMsg->ch == '/')
			{
				AutoCompleteNode(_T(""));
			}
			else if(pMsg->ch == ' ')
			{
				CString strShow = AutoCompleteProperty(GetNodeName(), _T(""));
				if(!strShow.IsEmpty())
					sci.sci_AutocShow(0, LST2UTF8(strShow));
			}
			else if(pMsg->ch == '>')
			{
			}
			else if(pMsg->ch == '=')
			{

			}
			else if(pMsg->ch == '"')
			{

			}
			else
			{
				int pos = sci.sci_GetCurrentPos();
				int startPos = sci.sci_WordStartPosition(pos-1, TRUE);
				int endPos = sci.sci_WordEndPosition(pos-1, TRUE);

				CStringA objectNameA;
				sci.sci_GetTextRange(startPos, endPos, objectNameA);

				CStringA strLeftA;
				sci.sci_GetTextRange(startPos-1, startPos, strLeftA);
				if(strLeftA == '<' || strLeftA == '/')
				{
					AutoCompleteNode(LSUTF82T(objectNameA));
				}
				else
				{
					CString strShow = AutoCompleteProperty(GetNodeName(), LSUTF82T(objectNameA));
					if(!strShow.IsEmpty())
					{
						sci.sci_AutocShow(0, LST2UTF8(strShow));
					}
				}
			}			
		}
		break;
	case SCN_MODIFIED:
		{
			//UpdateFrameStatus();
		}
		break;
	case SCN_SAVEPOINTREACHED:	//�ļ�������
		{

		}
		break;
	case SCN_SAVEPOINTLEFT: //�ļ����޸�
		{

		}
		break;
	case SCN_UPDATEUI:
		break;
	}	
	return __super::OnNotify(wParam, lParam, pResult);
}

CString CDockXmlView::GetNodeName()
{
	CString strRet;
	int pos = sci.sci_GetCurrentPos();

	CStringA strText;
	sci.sci_GetTextRange(0, pos, strText);
	int nSpacePos = -1;
	for (int i=strText.GetLength()-1; i>=0; i--)
	{
		if(strText[i] == ' ')
		{
			nSpacePos = i;
		}
		else if(strText[i] == '<')
		{
			int nodePos = i-1;
			if(nSpacePos >= 0)
			{
				CStringA strRetA;
				sci.sci_GetTextRange(nodePos+2, nSpacePos, strRetA);
				//InsertMsg(CStringToolExt::CStrA2CStrW(strRetA));
				LSSTRING_CONVERSION;
				strRet = LSUTF82T(strRetA);
				break;
			}
		}
	}


	//InsertMsg(CStringToolExt::CStrA2CStrW(strText));

	return strRet;
}

void CDockXmlView::AutoCompleteNode(CString objectName)		//�Զ���ɿؼ���
{
	CString strShow;

	objectName.MakeUpper();

	for (xml_node node=g_duiProp.GetRoot().first_child(); node; node=node.next_sibling())
	{
		CString nodeName = node.name();
		nodeName.MakeUpper();
		if(nodeName.Find(objectName) >= 0)
		{
			strShow += node.name();
			strShow += _T(" ");
		}
	}

	LSSTRING_CONVERSION;
	if(!strShow.IsEmpty())
	{
		sci.sci_AutocShow(0, LST2UTF8(strShow));
	}
}

CString CDockXmlView::AutoCompleteProperty(CString objectName, CString AttrName)	//�Զ����������
{
	CString strShow;

	xml_node node = g_duiProp.FindControl(objectName);
	for (xml_node nodeAttr = node.first_child(); nodeAttr; nodeAttr=nodeAttr.next_sibling())
	{
		LPCTSTR className = nodeAttr.attribute(_T("name")).value();
		if(AttrName.IsEmpty())
		{
			strShow += className;
			strShow += _T(" ");
		}
		else
		{
			CString strClass = className;
			strClass.MakeUpper();
			AttrName.MakeUpper();
			if(strClass.Find(AttrName) >= 0)
			{
				strShow += className;
				strShow += _T(" ");
			}
		}
	}

	CString parentName = node.attribute(_T("parent")).as_string();
	if(!parentName.IsEmpty())
	{
		strShow += AutoCompleteProperty(parentName, AttrName);
	}

	return strShow;
}

//////////////////////////////////////////////////////////////////////////
void CDockXmlView::OnEditUndo()
{
	sci.sci_Undo();
}


void CDockXmlView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(sci.sci_CanUndo());
}


void CDockXmlView::OnEditRedo()
{
	sci.sci_Redo();
}


void CDockXmlView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(sci.sci_CanRedo());
}


void CDockXmlView::OnEditCut()
{
	sci.sci_Cut();
}


void CDockXmlView::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(sci.sci_CanCut());
}


void CDockXmlView::OnEditCopy()
{
	sci.sci_Copy();
}


void CDockXmlView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(sci.sci_CanCopy());
}

void CDockXmlView::OnEditPaste()
{
	sci.sci_Paste();
}


void CDockXmlView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(sci.sci_CanPaste());
}

void CDockXmlView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CPoint pt = point;
	ScreenToClient(&pt);

	CMenu popmenu;
	popmenu.LoadMenu(IDR_MENU_XML_PANE);
	theApp.GetContextMenuManager()->ShowPopupMenu(popmenu.GetSubMenu(0)->m_hMenu, point.x, point.y, this, TRUE);
}


void CDockXmlView::OnSciUpdateDesign()
{
	m_nTargetLine = sci.sci_GetCurLine();
	ApplyDocument();
}


void CDockXmlView::OnUpdateSciUpdateDesign(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(sci.sci_GetModify());
}


void CDockXmlView::OnSciDelete()
{
	sci.sci_Clear();
}


void CDockXmlView::OnUpdateSciDelete(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(sci.sci_GetSelectionStart() != sci.sci_GetSelectionEnd());
}


void CDockXmlView::OnSciSelectAll()
{
	sci.sci_SelectAll();
}


void CDockXmlView::OnUpdateSciSelectAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


BOOL CDockXmlView::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(::GetKeyState(VK_CONTROL) < 0)
		{
			if(pMsg->wParam == 'Z' || pMsg->wParam == 'z')
			{
				OnEditUndo(); 
				return TRUE;
			}
			if(pMsg->wParam == 'Y' || pMsg->wParam == 'y')
			{
				OnEditRedo(); 
				return TRUE;
			}
			if(pMsg->wParam == 'X' || pMsg->wParam == 'x')
			{
				OnEditCut(); 
				return TRUE;
			}
			if(pMsg->wParam == 'C' || pMsg->wParam == 'c')
			{
				OnEditCopy(); 
				return TRUE;
			}
			if(pMsg->wParam == 'V' || pMsg->wParam == 'v')
			{
				OnEditPaste(); 
				return TRUE;
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CDockXmlView::ApplyDocument()
{
	if(!sci.sci_GetModify())
		return TRUE;

	LSSTRING_CONVERSION;
	CStringA strTextUtf8;
	sci.sci_GetTextAll(strTextUtf8);
	CString strText = LSUTF82T(strTextUtf8);

	//���Խ����޸ĺ��xml�ĵ�
	tinyxml2::XMLDocument xml;
	tinyxml2::XMLError err = xml.LoadBuffer(strTextUtf8, strTextUtf8.GetLength());
	if(xml.Error())
	{
		CStringA strMsg;
		strMsg.Format("��������: \r\nErrorID:  %d \r\nErrorName:  %s \r\n\r\n����������:  %d", 
			xml.ErrorID(), xml.ErrorIDToName(err), xml.GetErrorLineNum());
		::MessageBoxA(NULL, strMsg, "XML ERROR", MB_OK);
		return FALSE;
	}

	//���µ������
	CDuiEditorDoc *pDoc = (CDuiEditorDoc *)m_pDoc;
	pDoc->m_doc.load_string(strText, XML_PARSER_OPTIONS);
	pDoc->SetModifiedFlag(TRUE);
	//((CDuiEditorViewDesign *)pDoc->GetDesignView())->InitView();
	m_pManager->GetView()->PostMessage(WM_INIT_VIEW_DESIGN, 0, 0);

	return TRUE;
}


void CDockXmlView::OnSciRefresh()
{
	m_nTargetLine = sci.sci_GetCurLine();
	Init();
}


void CDockXmlView::OnUpdateSciRefresh(CCmdUI *pCmdUI)
{
	// TODO: �ڴ�������������û����洦���������
}