//////////////////////////////////////////////////////////////////////////////
// Sample.c :
//		SBI Client DLL�̎g�p����������T���v���v���O�����ł��B
//		���������𔭒�����̂Ŏ��s����ۂ͏\�����ӂ��Ă��������B
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <SBIClient.h>

int _tmain(int argc, _TCHAR* argv[])
{
	int rc;
	char username[256];
	char password[256];
	char orderPassword[256];
	ACCOUNTINFO accountInfo;
	STOCKPRICES prices;
	STOCKORDER order;
	int orderNumber;
	int orderStatus;
	double lastExecPrice;

	puts("���O�C�����[�U�[������͂��Ă��������B");
	gets_s( username, 256 );
	puts("���O�C���p�X���[�h����͂��Ă��������B");
	gets_s( password, 256 );
	puts("����p�X���[�h����͂��Ă��������B");
	gets_s( orderPassword, 256 );

	//------------------------------------------------------------------------
	//	API������������
	//------------------------------------------------------------------------
	if ((rc = SBIInitialize()) == SBI_NOT_AVAILABLE) {
		puts("���p���Ԃ��߂��Ă��܂��B");
		return -1;
	}

	if (rc == 0)
		puts("���C�Z���X�o�^�ς݂ł��B");
	else
		printf("���p���Ԃ͎c��%d���ł��B\n", rc);

	//------------------------------------------------------------------------
	//	SBI�،��̌����Ƀ��O�C������
	//------------------------------------------------------------------------
	if ((rc = SBILogin( username, password )) < 0) {
		printf("Login error = %d\n", rc);
		return -1;
	}

	puts("���O�C���ɐ������܂����B");

	//------------------------------------------------------------------------
	//	���������擾����
	//------------------------------------------------------------------------
	if ((rc = SBIGetAccountInfo( &accountInfo )) < 0) {
		printf("GetAccountInfo error = %d\n", rc);
	} else {
		puts("===== ������� =====");
		printf("���Y�]���z: %ld\n", accountInfo.asset_valuation);
		printf("���t�]��(3�c�Ɠ���): %ld\n", accountInfo.buy_strength);
		printf("�M�p���]��: %ld\n", accountInfo.margin_strength);
	}

	//------------------------------------------------------------------------
	//	���A���^�C���������擾����
	//------------------------------------------------------------------------
	if (( rc = SBIGetStockPrice( 7201, &prices )) < 0) {
		printf("GetStockPrice error = %d\n", rc);
	} else {
		printf("���Y�����Ԃ̊��� ���ݒl:%g �n�l:%g ���l:%g ���l:%g\n",
						prices.last, prices.open, prices.high, prices.low);
	}

	//------------------------------------------------------------------------
	//	����p�X���[�h��ݒ肷��
	//------------------------------------------------------------------------
	SBISetOrderPassword( orderPassword );

	//------------------------------------------------------------------------
	//	��������1�𔭒�����
	//		[���] �M�p����
	//		[�s��] �D��s��/SOR
	//		[����] �g���^������(7203)
	//		[�������] 100��
	//		[���s����] �w�l(6000�~)
	//		[�a�������] ����
	//------------------------------------------------------------------------
	order = SBICreateOrder( OD_MARGIN_BUY, OD_SOR, 7203, 100 );
	SBISetOrderSashine( order, 6000, OD_NOCOND );
	SBISetOrderAzukari( order, OA_TOKUTEI );

	if ((rc = SBIStockOrder( order, &orderNumber )) < 0) {
		printf("StockOrder error = %d\n", rc);
		if (rc == SBI_UNKNOWN_ERROR)
			puts( SBIGetLastOrderErrorDescr() );
		SBIReleaseOrder( order );
		return -1;
	}

	printf("��������1�̔����ɐ������܂����B(�����ԍ�:%d)\n", orderNumber);

	//------------------------------------------------------------------------
	//	��������1�̒�����Ԃ��擾����
	//------------------------------------------------------------------------
	if ((rc = SBIGetStockOrderStatus( orderNumber, &orderStatus, &lastExecPrice )) < 0) {
		printf("GetStockOrderStatus error = %d\n", rc);
	} else {
		puts("��������1�̒�����Ԃ̎擾�ɐ������܂����B");
		printf("[�������:%d][�ŏI��艿�i:%g]\n", orderStatus, lastExecPrice);
	}

	//------------------------------------------------------------------------
	//	��������1�̎��s�����𐬍s(�����Ȃ�)�ɒ�������
	//------------------------------------------------------------------------
	SBISetOrderNariyuki( order, OD_NOCOND );

	if ((rc = SBIStockOrderModify( orderNumber, order )) < 0) {
		printf("StockOrderModify error = %d\n", rc);
		if (rc == SBI_UNKNOWN_ERROR)
			puts( SBIGetLastOrderErrorDescr() );
		SBIReleaseOrder( order );
		return -1;
	}

	printf("��������1�̒����ɐ������܂����B\n");
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	��������1�������
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("��������1�̎�����ɐ������܂����B");

	//------------------------------------------------------------------------
	//	��������2�𔭒�����
	//		[���] �M�p����
	//		[�s��] �D��s��/SOR
	//		[����] �\�t�g�o���N(9984)
	//		[�������] 100��
	//		[���s����] �w�l(7000�~�����w��)
	//		[�a�������] ����
	//------------------------------------------------------------------------
	order = SBICreateOrder( OD_MARGIN_SELL, OD_SOR, 9984, 100 );
	SBISetOrderSashine( order, 7000, OD_HIKESASHI );
	SBISetOrderAzukari( order, OA_TOKUTEI );

	if ((rc = SBIStockOrder( order, &orderNumber )) < 0) {
		printf("StockOrder error = %d\n", rc);
		if (rc == SBI_UNKNOWN_ERROR)
			puts( SBIGetLastOrderErrorDescr() );
		SBIReleaseOrder( order );
		return -1;
	}

	printf("��������2�̔����ɐ������܂����B(�����ԍ�:%d)\n", orderNumber);
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	��������2�������
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("��������2�̎�����ɐ������܂����B");

	//------------------------------------------------------------------------
	//	��������3�𔭒�����
	//		[���] ��������
	//		[�s��] ����
	//		[����] ���{�d�C(6701)
	//		[�������] 1000��
	//		[���s����] �t�w�l(5800�~)
	//		[�a�������] NISA
	//------------------------------------------------------------------------
	order = SBICreateOrder( OD_BUY, OD_TOKYO, 6701, 1000 );
	SBISetOrderGSashine( order, 5800 );
	SBISetOrderAzukari( order, OA_NISA );

	if ((rc = SBIStockOrder( order, &orderNumber )) < 0) {
		printf("StockOrder error = %d\n", rc);
		if (rc == SBI_UNKNOWN_ERROR)
			puts( SBIGetLastOrderErrorDescr() );
		SBIReleaseOrder( order );
		return -1;
	}

	printf("��������3�̔����ɐ������܂����B(�����ԍ�:%d)\n", orderNumber);
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	��������3�������
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("��������3�̎�����ɐ������܂����B");

	//------------------------------------------------------------------------
	//	��������4�𔭒�����
	//		[���] ��������
	//		[�s��] �D��s��/SOR
	//		[����] ���Y������(7201)
	//		[�������] 100��
	//		[���s����] �g���K�[���i480.7�~�ȏ�ɂȂ�����w�l(470.2�~)�Ŏ��s
	//		[�a�������] NISA
	//------------------------------------------------------------------------
	order = SBICreateOrder( OD_BUY, OD_SOR, 7201, 100 );
	SBISetOrderSashine( order, 480.7, OD_NOCOND );
	SBISetOrderTrigger( order, 470.2, OD_MORE );
	SBISetOrderAzukari( order, OA_NISA );

	if ((rc = SBIStockOrder( order, &orderNumber )) < 0) {
		printf("StockOrder error = %d\n", rc);
		if (rc == SBI_UNKNOWN_ERROR)
			puts( SBIGetLastOrderErrorDescr() );
		SBIReleaseOrder( order );
		return -1;
	}

	printf("��������4�̔����ɐ������܂����B(�����ԍ�:%d)\n", orderNumber);
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	��������4�������
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("��������4�̎�����ɐ������܂����B");

	//------------------------------------------------------------------------
	//	���O�A�E�g����
	//------------------------------------------------------------------------
	if ((rc = SBILogout()) < 0) {
		printf("Logout error = %d\n", rc);
		return -1;
	}

	puts("���O�A�E�g���܂����B");

	return 0;
}

