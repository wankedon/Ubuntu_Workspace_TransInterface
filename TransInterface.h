
#ifdef PLATFROM_WINDOWS
#ifdef TRANSINTERFACE_EXPORTS
#define TRANSINTERFACE_API __declspec(dllexport)
#else
#define TRANSINTERFACE_API __declspec(dllimport)
#endif
#else
#define TRANSINTERFACE_API
#endif

#include "TransComDef.h"
//������
enum TR_Error
{
	TR_ERR_NONE = 0,
	TR_ERR_INIT = -1,
	TR_ERR_INVALID_HANDLE = -2,
	TR_ERR_SEND = -3,
	TR_ERR_NO_HANDLE_RESOURCE = -4,
};
//��ʼ����
TRANSINTERFACE_API TR_Error InitLib();
//����ͨ��
TRANSINTERFACE_API TR_Error CreateTransport(int* fd, const char* udpPoint, const ControlCallBacks& ccbs, const BusSendCallBack& bscb);
//����ԭʼ����
TRANSINTERFACE_API TR_Error InputRawData(int fd, const RawData& buffer);
//�����ϱ��ص�����
TRANSINTERFACE_API TR_Error GetReportCallBacks(int fd, ReportCallBacks& rcbs);
//�ر�ͨ��
TRANSINTERFACE_API TR_Error CloseTransport(int fd);
//�رտ�
TRANSINTERFACE_API TR_Error DeInitLib();
