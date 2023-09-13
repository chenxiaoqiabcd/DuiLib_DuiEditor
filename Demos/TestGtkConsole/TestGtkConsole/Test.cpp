// DuiLibTemplate.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Test.h"



int main(int argc, char* argv[])
{
	//_CrtSetBreakAlloc();

	//���������ڵ�����
	uiApp.SetAppName(_T("Test"));

	//���ó���Ϊ��һִ��ʵ����������ʵ���Ļ����Ͳ���Ҫ�����ˡ�
//	uiApp.SetSingleApplication(true, _T("CBD4DD1F-3B18-4DA4-A043-0EA9AEC8D570"));

	//����xml��Դ���뷽ʽ��ѡ��һ�־���	
#ifdef _DEBUG
	uiApp.SetResType_File(_T("skin"));								//���ļ���������xml�ļ�
#else
	uiApp.SetResType_ZipResource(_T("skin"), _T(""), IDR_ZIPRES1, _T("ZIPRES"));	//�ӳ�����Դ�ļ�������xml�ļ�
#endif

	HMODULE hModule = GetModuleHandle(NULL);
	CPaintManagerUI::SetInstance(hModule);

	CPaintManagerUI::SetRenderEngineType(DuiLib_Render_Cairo);

	//��ʼ��duilib, ע����������������
	if(!uiApp.InitInstance(argc, argv))
		return 0;

	//������Ϣѭ��
	uiApp.Run();

	//������Դ��
	uiApp.ExitInstance();
	return 0;
}