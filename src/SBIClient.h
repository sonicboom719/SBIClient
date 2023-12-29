/*****************************************************************************
	SBI Client �����������C�u����
	Copyright (C) 2012 SonicBoom All Rights Reserved.
*****************************************************************************/

#ifndef	SBICLIENT__
#define	SBICLIENT__

#ifdef DLL_EXPORTS
#define SBICLIENT_API __declspec(dllexport)
#else
#define SBICLIENT_API __declspec(dllimport)
#endif

#define STDCALL __stdcall

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************************
	�G���[�R�[�h�ꗗ
*****************************************************************************/
#define	SBI_SUCCESS							0		/* ���� */
#define	SBI_NOT_AVAILABLE					(-1)	/* API�͗��p�ł��Ȃ�(���p�����؂ꓙ) */
#define	SBI_NOT_INITIALIZED					(-2)	/* API�͏���������Ă��Ȃ� */
#define	SBI_UNKNOWN_ERROR					(-100)	/* �s����(���m��)�G���[ */
#define	SBI_INTERNET_ERROR					(-101)	/* �C���^�[�l�b�g�ʐM�̃G���[ */
#define	SBI_UNMATCH_USERNAME_OR_PASSWORD	(-102)	/* ���[�U�[�����p�X���[�h���������Ȃ� */
#define	SBI_OUT_OF_STRENGTH					(-103)	/* ���t�]��(�܂��͐M�p���]��)���s�����Ă��� */
#define	SBI_BRAND_NOT_FOUND					(-104)	/* �w��̖����R�[�h�͂Ȃ� */
#define	SBI_PRICE_RANGE_OVER				(-105)	/* �l���͈͂𒴂��Ă��� */
#define	SBI_TRADE_RESTRICTED				(-106)	/* ���̎���͋K������Ă��� */
#define	SBI_NOT_HAVE_POSITION				(-107)	/* �a����c��(�܂��͌���)������Ȃ� */
#define	SBI_INSTRUCTION_ERROR				(-108)	/* �s���Ȏw���ł��� */
#define	SBI_ORDER_NOT_FOUND					(-109)	/* �w�肳�ꂽ�����ԍ��̒����͂Ȃ� */
#define	SBI_CANCEL_NOT_ACCEPTED				(-110)	/* ����͎󂯕t�����Ȃ� */
#define	SBI_CANCEL_FAILED					(-111)	/* ����͎��s���� */
#define	SBI_PARSE_ERROR						(-112)	/* Web�y�[�W�̉�͂Ɏ��s���� */
#define	SBI_POSITION_NOT_FOUND				(-113)	/* ���ʂ��Ȃ� */
#define	SBI_MODIFY_NOT_ACCEPTED				(-114)	/* �����͎󂯕t�����Ȃ� */
#define	SBI_NOT_LOGGEDIN					(-115)	/* ���O�C�����Ă��Ȃ� */
#define	SBI_ORDER_PASSWORD_ERROR			(-116)	/* ����p�X���[�h���������Ȃ� */
#define	SBI_QUANTITY_UNIT_ERROR				(-117)	/* �����P�ʂ��������Ȃ� */
#define	SBI_TEMP_FILE_ERROR					(-118)	/* �ꎞ�t�@�C���̏����Ɏ��s���� */
#define	SBI_MULTIPLE_REQUEST_ERROR			(-119)	/* �������N�G�X�g��������s��ꂽ */
#define	SBI_PRICE_UNIT_ERROR				(-120)	/* ���i�P�ʂ��������Ȃ� */
#define	SBI_IOC_ORDER_OVERTIME				(-121)	/* IOC�����͎��ԊO�̂��߂ł��Ȃ� */
#define	SBI_INSUFFICIENT_GUARANTEE			(-122)	/* �Œ�ϑ��ۏ؋��ɑ���Ȃ� */

/*****************************************************************************
	�����ʃR�[�h
*****************************************************************************/
#define	OD_BUY				0	/* �������� */
#define OD_SELL				1	/* �������� */
#define OD_MARGIN_BUY		2	/* �M�p���� */
#define	OD_MARGIN_SELL		3	/* �M�p���� */
#define	OD_REPAY_BUY		4	/* �M�p�ԍϔ��� */
#define	OD_REPAY_SELL		5	/* �M�p�ԍϔ��� */

/*****************************************************************************
	����s��R�[�h
*****************************************************************************/
#define	OD_SOR				0	/* �D��s��/SOR */
#define OD_TOKYO			1	/* ���� */
#define OD_OSAKA			2	/* ��� */
#define	OD_JASDAQ			3	/* JASDAQ */
#define	OD_NAGOYA			4	/* ���� */
#define	OD_FUKUOKA			5	/* ���� */
#define	OD_SAPPORO			6	/* �D�� */

/*****************************************************************************
	�M�p����敪�R�[�h
*****************************************************************************/
#define	OD_SEIDO			0	/* ���x */
#define OD_IPPAN			1	/* ��� */
#define OD_DAY				2	/* ���v�� */

/*****************************************************************************
	���s�����R�[�h
*****************************************************************************/
#define	OD_NOCOND			0	/* �����Ȃ� */
#define	OD_YORINARI			1	/* ��萬�� */
#define	OD_HIKENARI			2	/* �������� */
#define	OD_YORISASHI		3	/* ���w�� */
#define	OD_HIKESASHI		4	/* �����w�� */
#define	OD_FUNARI			5	/* �s���� */
#define	OD_IOCNARI			6	/* IOC���� */
#define	OD_IOCSASHI			7	/* IOC�w�� */

/*****************************************************************************
	�g���K�[�]�[���R�[�h
*****************************************************************************/
#define OD_MORE		0	/* �ȏ� */
#define OD_LESS		1	/* �ȉ� */

/*****************************************************************************
	������ԃR�[�h
*****************************************************************************/
#define	OS_UNKNOWN	 			0	/* ���m(���T�|�[�g)�̏�� */
#define	OS_WAITING				1	/* �ҋ@�� */
#define	OS_ORDERING				2	/* ������ */
#define	OS_ORDER_COMPLETE		3	/* ���� */
#define	OS_CANCELING			4	/* ����� */
#define	OS_CANCEL_COMPLETE		5	/* ������� */
#define	OS_MODIFYING			6	/* ������ */
#define	OS_EXPIRED				7	/* ���� */

/*****************************************************************************
	�a��������R�[�h
*****************************************************************************/
#define	OA_TOKUTEI	 		"0" /* ����a���� */
#define	OA_GENERAL	 		"1" /* ��ʗa���� */
#define	OA_NEW_NISA 		"H" /* NISA�a���� */
#define	OA_NISA		 		"4" /* ��NISA�a���� */

/*****************************************************************************
	�e��^��`
*****************************************************************************/
#pragma	pack(1)

typedef	struct stACCOUNTINFO {
	long	buy_strength;		/* ���t�]�� */
	long	margin_strength;	/* �M�p���]�� */
	long	asset_valuation;	/* ���Y�]���z */
} ACCOUNTINFO;

typedef struct stSTOCKPRICES {
	double	open;		/* �n�l */
	double	high;		/* ���l */
	double	low;		/* ���l */
	double	last;		/* ���ݒl */
} STOCKPRICES;

typedef void* STOCKORDER;

#pragma	pack()


/*****************************************************************************
	SBI Client�̃��C�u����������������B

	���C�u���������������ĊeAPI���g�p�\�ȏ�Ԃɂ��܂��B

	�߂�l ��������ƃ[���ȏ�̒l��Ԃ��܂��B�[���̏ꍇ�̓��C�Z���X�o�^�ς�
	       ��\���A���̒l�̏ꍇ�͎��p���Ԃ̎c�������\���܂��B
	       �G���[�̏ꍇ��SBI_NOT_AVAILABLE��Ԃ��܂��B
	�Q�� �G���[�R�[�h�ꗗ
*****************************************************************************/
SBICLIENT_API int STDCALL SBIInitialize(void);

/*****************************************************************************
	SBI�،���Web�T�C�g�Ƀ��O�C������B

	���������̔����E�����E����⃊�A���^�C�������̎擾�Ȃǂ�API���g�p����
	�O�ɂ͕K������API�Ń��O�C��������K�v������܂��B

	���� username [in] ���[�U�[��
	���� password [in] �p�X���[�h��
	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� SBILogout()
	�Q�� �G���[�R�[�h�ꗗ
*****************************************************************************/
SBICLIENT_API int STDCALL SBILogin( const char* username, const char* password );

/*****************************************************************************
	SBI�،���Web�T�C�g���烍�O�A�E�g����B

	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� SBILogin()
	�Q�� �G���[�R�[�h�ꗗ
*****************************************************************************/
SBICLIENT_API int STDCALL SBILogout(void);

/*****************************************************************************
	������{�����擾����B

	���O�C�����Ă�������̔��t�]�́E�M�p���]�́E���Y�]���z�Ȃǂ��擾���܂��B

	���� accountInfo [out] �������̍\���̂ւ̃|�C���^
	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� �G���[�R�[�h�ꗗ
*****************************************************************************/
SBICLIENT_API int STDCALL SBIGetAccountInfo( ACCOUNTINFO* accountInfo );

/*****************************************************************************
	���A���^�C���������擾����B

	�w�肳�ꂽ�R�[�h�̖����̃��A���^�C������(4�{�l)���擾���܂��B
	�s�ꂪ�J�����O�Ȃǂ܂��l���t���Ă��Ȃ����ԑт�4�{�l�Ƃ��[�����Ԃ�܂��B

	���� code [in] �����R�[�h
	���� prices [out] ���A���^�C�������̍\���̂ւ̃|�C���^
	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� �G���[�R�[�h�ꗗ
*****************************************************************************/
SBICLIENT_API int STDCALL SBIGetStockPrice( int code, STOCKPRICES* prices );

/*****************************************************************************
	����p�X���[�h��ݒ肷��B

	���������̔����E�����E����̍ۂɋ��ʂɎg�������p�X���[�h��ݒ肵�܂��B

	���� password [in] ����p�X���[�h���i�[����|�C���^
	�߂�l �Ȃ��B
	�Q�� SBIStockOrder()
	�Q�� SBIStockOrderModify()
	�Q�� SBIStockOrderCancel()
	�Q�� �G���[�R�[�h�ꗗ
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderPassword( const char* password );

/*****************************************************************************
	�����G���[�̏ꍇ�ɃG���[��������擾����B

	���������̔����E�����E����ł��̑��̃G���[(SBI_UNKNOWN_ERROR)���Ԃ��ꂽ
	�ꍇ�ɁA�{API���R�[������ƃG���[�̏ڍ׏��(���{��)��\��������
	�擾�ł��܂��B�G���[������̓V�t�gJIS�R�[�h�Ŏ擾�ł��܂��B

	�߂�l �G���[�ڍו�����ւ̃|�C���^�B
	�Q�� SBIStockOrder()
	�Q�� SBIStockOrderModify()
	�Q�� SBIStockOrderCancel()
	�Q�� �G���[�R�[�h�ꗗ
*****************************************************************************/
SBICLIENT_API const char* STDCALL SBIGetLastOrderErrorDescr(void);

/*****************************************************************************
	�����������쐬����B

	���������̃I�u�W�F�N�g���쐬���܂��B

	���� trade [in] ����̎��
	���� market [in] ����s��
	���� code [in] �����R�[�h
	���� quantity [in] ������銔��
	�߂�l ���������STOCKORDER�I�u�W�F�N�g��Ԃ��܂��B�G���[�̏ꍇ��
	       NULL��Ԃ��܂��B
	�Q�� SBIReleaseOrder()
	�Q�� �����ʃR�[�h
	�Q�� ����s��R�[�h
*****************************************************************************/
SBICLIENT_API STOCKORDER STDCALL SBICreateOrder( int trade, int market, int code, long quantity );

/*****************************************************************************
	�����������������B

	���������̃I�u�W�F�N�g��������܂��B
	���������𔭒�������͂�����Ăяo���Ē����I�u�W�F�N�g��������Ă��������B

	���� order [in] �������STOCKORDER�I�u�W�F�N�g
	�߂�l �Ȃ��B
	�Q�� SBICreateOrder()
*****************************************************************************/
SBICLIENT_API void STDCALL SBIReleaseOrder( STOCKORDER order );

/*****************************************************************************
	�����̎��s�����𐬍s�ɐݒ肷��B

	�����̎��s�����𐬍s�ɐݒ肵�܂��B2�������Ƃ��Ċ�萬��E��������
	�EIOC����E�����Ȃ��̂������ꂩ��ݒ肵�܂��BSTOCKORDER�I�u�W�F�N�g
	���쐬��������̓f�t�H���g�Ő��s(�����Ȃ�)�ɐݒ肳��Ă��܂��B

	���� order [in] �ݒ肷��STOCKORDER�I�u�W�F�N�g
	���� cond [in] ���s������2������
	�߂�l �Ȃ�
	�Q�� ���s�����R�[�h
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderNariyuki( STOCKORDER order, int cond );

/*****************************************************************************
	���s�������w�l�ɐݒ肷��B

	�����̎��s�������w�l�ɐݒ肵�܂��B2�������Ƃ��Ċ��w���E�����w��
	�E�s����EIOC�w���E�����Ȃ��̂������ꂩ��ݒ肵�܂��B

	���� order [in] �ݒ肷��STOCKORDER�I�u�W�F�N�g
	���� price [in] �w�l���i
	���� cond [in] ���s������2������
	�߂�l �Ȃ�
	�Q�� ���s�����R�[�h
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderSashine( STOCKORDER order, double price, int cond );

/*****************************************************************************
	���s�������t�w�l�ɐݒ肷��B

	�����̎��s�������t�w�l�ɐݒ肵�܂��B����������Őݒ肵���g���K�[
	���i�ɒB����ƁA���s(�����Ȃ�)�Œ����𔭒����܂��B���s(�����Ȃ�)
	�ȊO�̒������������ꍇ��SBISetOrderTrigger�֐����g���Ă��������B

	���� order [in] �ݒ肷��STOCKORDER�I�u�W�F�N�g
	���� price [in] �g���K�[���i
	�߂�l �Ȃ�
	�Q�� SBISetOrderTrigger()
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderGSashine( STOCKORDER order, double price );

/*****************************************************************************
	���s�����Ƀg���K�[����ݒ肷��B

	�����̎��s�����Ƀg���K�[��ݒ肵�A���̃g���K�[���i�ɒB�����ꍇ��
	���߂ď���̔�������(���s�܂��͎w�l)�𔭒�����悤�ɐݒ肵�܂��B
	�ʏ�͋t�w�l�������������邽�߂ɂ��̋@�\���g���܂��B���̃g���K�[
	�ݒ���s���ꍇ�́ASBISetOrderNariyuki�֐��܂���SBISetOrderSashine�֐���
	�Z�b�g�Ŏg�p���Ă��������B

	���� order [in] �ݒ肷��STOCKORDER�I�u�W�F�N�g
	���� price [in] �g���K�[���i
	���� zone [in] �g���K�[�]�[��(�g���K�[���i�ȏ�܂��͈ȉ�)
	�߂�l �Ȃ��B
	�Q�� SBISetOrderNariyuki()
	�Q�� SBISetOrderSashine()
	�Q�� �g���K�[�]�[���R�[�h
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderTrigger( STOCKORDER order, double price, int zone );

/*****************************************************************************
	�M�p����敪��ݒ肷��B

	�����̐M�p����敪(���x�M�p����A��ʐM�p����܂��͓��v��)��ݒ肵�܂��B
	����敪���M�p�����܂��͐M�p����̏ꍇ�̂ݐݒ肪�L���ł��B
	STOCKORDER�I�u�W�F�N�g���쐬��������́A�f�t�H���g��
	���x�M�p����ɐݒ肳��Ă��܂��B

	���� order [in] �ݒ肷��STOCKORDER�I�u�W�F�N�g
	���� margin [in] �M�p����敪�R�[�h
	�߂�l �Ȃ�
	�Q�� �M�p����敪�R�[�h
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderMargin( STOCKORDER order, int margin );

/*****************************************************************************
	�����̗a����敪��ݒ肷��B

	�����̌����̗a����敪(����a����E��ʗa����ENISA�a����)��ݒ肵�܂��B
	����敪�����������E��������E�M�p�����܂��͐M�p����̏ꍇ�̂�
	�ݒ肪�L���ł��BSTOCKORDER�I�u�W�F�N�g���쐬��������́A�f�t�H���g
	��TRUE(����a����)�ɐݒ肳��Ă��܂��B

	���� order [in] �ݒ肷��STOCKORDER�I�u�W�F�N�g
	���� azukari [in] �a���������\����������w�肵�܂�
	�߂�l �Ȃ�
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderAzukari( STOCKORDER order, const char* azukari );

/*****************************************************************************
	���������𔭒�����B

	�������܂��͐M�p����ɂ����锄�������𔭒����܂��B
	��������Ǝ󂯕t���������̒����ԍ����Ԃ���܂��B
	�߂�l��SBI_UNKNOWN_ERROR�̏ꍇ�AGetLastOrderErrorDescr�֐����R�[��
	����Əڍ׃G���[��񂪎擾�ł���ꍇ������܂��B

	���� order [in] �����I�u�W�F�N�g
	���� orderNumber [out] �����ԍ����i�[����|�C���^
	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� �G���[�R�[�h�ꗗ
	�Q�� SBIGetLastOrderErrorDescr()
*****************************************************************************/
SBICLIENT_API int STDCALL SBIStockOrder( STOCKORDER order, int* orderNumber );

/*****************************************************************************
	�����������������B

	����������������܂��B���̒����̓��e�ɂ���Ē����ł�����e�͕ς����
	����̂ł���API�̎g�p�ɂ͒��ӂ��K�v�ł��B�Ⴆ�Ό��̒������ʏ�̎w�l
	�����������ꍇ�́A�w�l���i��������邩�A���s�����ɒ������邩�A����
	�ύX�ł��܂���B�����������ŉ��ɋt�w�l�����ɒ��������ꍇ�ł��AAPI��
	�߂�l�Ƃ��Ă͐�����Ԃ��Ă��܂�(���ۂ͐������Ȃ�)�ꍇ������̂ŏ\��
	���ӂ��Ă��������B
	�߂�l��SBI_UNKNOWN_ERROR�̏ꍇ�ASBIGetLastOrderErrorDescr�֐����R�[��
	����Əڍ׃G���[��񂪎擾�ł���ꍇ������܂��B

	���� orderNumber [in] �������̒����̒����ԍ�
	���� order [in] ������̒����I�u�W�F�N�g
	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� �G���[�R�[�h�ꗗ
	�Q�� SBIGetLastOrderErrorDescr()
*****************************************************************************/
SBICLIENT_API int STDCALL SBIStockOrderModify( int orderNumber, STOCKORDER order );

/*****************************************************************************
	����������������B

	����������������܂��B
	�߂�l��SBI_UNKNOWN_ERROR�̏ꍇ�ASBIGetLastOrderErrorDescr�֐����R�[��
	����Əڍ׃G���[��񂪎擾�ł���ꍇ������܂��B

	���� orderNumber [in] ����������̒����ԍ�
	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� �G���[�R�[�h�ꗗ
	�Q�� SBIGetLastOrderErrorDescr()
*****************************************************************************/
SBICLIENT_API int STDCALL SBIStockOrderCancel( int orderNumber );

/*****************************************************************************
	�����̏�Ԃ��擾����B

	���������̒������(�ҋ@��/������/����/��������Ȃ�)���擾���܂��B
	���������ς�(�ꕔ�܂��͑S���)�̏ꍇ�͍Ō�ɖ�肵�����i��������
	�擾�ł��܂��B

	���� orderNumber [in] �擾���钍���̒����ԍ�
	���� orderStatus [out] ������Ԃ��i�[����|�C���^
	���� lastExecPrice [out] �Ō�̖��̖�艿�i���i�[����|�C���^
	                         �s�v�ȏꍇ��NULL���w��ł��܂��B
	�߂�l ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
	�Q�� �G���[�R�[�h�ꗗ
	�Q�� ������ԃR�[�h
*****************************************************************************/
SBICLIENT_API int STDCALL SBIGetStockOrderStatus( int orderNumber, int* orderStatus, double* lastExecPrice );


#ifdef	__cplusplus
}
#endif

#endif
