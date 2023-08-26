//////////////////////////////////////////////////////////////////////////////
//	CSBIWebClient�N���X
//		SBI�،��̌����Ƀ��O�C�����ă��A���^�C�������擾�┄���������o���N���X

#include "CHttpClient.h"
#include "CBuffer.h"

class CSBIWebClient 
{
public:
     /********************************************************************//**
		�eAPI���ʃ��^�[���R�[�h��\���񋓒l
	  ***********************************************************************/
	enum enRETCODE {
		RET_SUCCESS = 0,						//!< ����
		RET_UNKNOWN_ERROR = -100,				//!< �s����(���m��)�G���[
		RET_INTERNET_ERROR = -101,				//!< �C���^�[�l�b�g�ʐM�̃G���[
		RET_UNMATCH_USERNAME_OR_PASSWORD = -102,//!< ���[�U�[�����p�X���[�h���������Ȃ�
		RET_OUT_OF_STRENGTH = - 103,			//!< ���t�]��(�܂��͐M�p���]��)���s�����Ă���
		RET_BRAND_NOT_FOUND = -104,				//!< �w��̖����R�[�h�͂Ȃ�
		RET_PRICE_RANGE_OVER = -105,			//!< �l���͈͂𒴂��Ă���
		RET_TRADE_RESTRICTED = -106,			//!< ���̎���͋K������Ă���
		RET_NOT_HAVE_POSITION = -107,			//!< �a����c��(�܂��͌���)������Ȃ�
		RET_INSTRUCTION_ERROR = -108,			//!< �s���Ȏw���ł���
		RET_ORDER_NOT_FOUND = -109,				//!< �w�肳�ꂽ�����ԍ��̒����͂Ȃ�
		RET_CANCEL_NOT_ACCEPTED = -110,			//!< ����͎󂯕t�����Ȃ�
		RET_CANCEL_FAILED = -111,				//!< ����͎��s����
		RET_PARSE_ERROR = -112,					//!< Web�y�[�W�̉�͂Ɏ��s����
		RET_POSITION_NOT_FOUND = -113,			//!< ���ʂ��Ȃ�
		RET_MODIFY_NOT_ACCEPTED = -114,			//!< �����͎󂯕t�����Ȃ�
		RET_NOT_LOGGEDIN = -115,				//!< ���O�C�����Ă��Ȃ�
		RET_ORDER_PASSWORD_ERROR = -116,		//!< ����p�X���[�h���������Ȃ�
		RET_QUANTITY_UNIT_ERROR = -117,			//!< �����P�ʂ��������Ȃ�
		RET_TEMP_FILE_ERROR = -118,				//!< �ꎞ�t�@�C���̏����Ɏ��s����
		RET_MULTIPLE_REQUEST_ERROR = -119,		//!< �������N�G�X�g��������s��ꂽ
		RET_PRICE_UNIT_ERROR = -120,			//!< ���i�P�ʂ��������Ȃ�
		RET_IOC_ORDER_OVERTIME = -121,			//!< IOC�����͎��ԊO�̂��߂ł��Ȃ�
		RET_INSUFFICIENT_GUARANTEE = -122		//!< �Œ�ϑ��ۏ؋��ɑ���Ȃ�
	};

     /********************************************************************//**
		�����̏�Ԃ̕\���񋓒l
	  ***********************************************************************/
	enum enORDERSTATUS {	
		UNKNOWN_STATUS = 0,		//!< ���m(���T�|�[�g)�̏��
		WAITING,				//!< �ҋ@��
		ORDERING,				//!< ������
		ORDER_COMPLETE,			//!< ����
		CANCELING,				//!< �����
		CANCEL_COMPLETE,		//!< �������
		MODIFYING,				//!< ������
		EXPIRED					//!< ����
	};

     /********************************************************************//**
		���A���^�C��������\���\����
	  ***********************************************************************/
#pragma	pack(1)
	typedef struct stSTOCKPRICES {
		double	open;		//!< �n�l
		double	high;		//!< ���l
		double	low;		//!< ���l
		double	current;	//!< ���ݒl
	} STOCKPRICES;

     /********************************************************************//**
		��������\���\����
	  ***********************************************************************/
	typedef	struct stACCOUNTINFO {
		long	buy_strength;		//!< ���t�]��
		long	margin_strength;	//!< �M�p���]��
		long	asset_valuation;	//!< ���Y�]���z
	} ACCOUNTINFO;
#pragma	pack()

     /********************************************************************//**
		���̔������������i�[����N���X
	  ***********************************************************************/
	class CStockOrder {
		friend class CSBIWebClient;

	public:
		/*****************************************************************//**
			����敪��\���񋓒l
		 ********************************************************************/
		enum enTRADE {
			BUY,			//!< ��������
			SELL,			//!< ��������
			MARGIN_BUY,		//!< �M�p����
			MARGIN_SELL,	//!< �M�p����
			REPAY_BUY,		//!< �M�p�ԍϔ���
			REPAY_SELL		//!< �M�p�ԍϔ���
		}; 

		/*****************************************************************//**
			����s��̎w��
		 ********************************************************************/
		enum enMARKET {
			SOR,			//!< �D��s��/SOR
			TOKYO,			//!< ����
			OSAKA,			//!< ���
			JASDAQ,			//!< �W���X�_�b�N
			NAGOYA,			//!< ����
			FUKUOKA,		//!< ����
			SAPPORO			//!< �D��
		}; 

		/*****************************************************************//**
			�M�p����敪�̎w��
		 ********************************************************************/
		enum enMARGIN {
			SEIDO,			//!< ���x
			IPPAN,			//!< ���
			DAY				//!< ���v��
		}; 

		/*****************************************************************//**
			���s����(���s/�w�l)�̎w��
		 ********************************************************************/
		enum enNARISASHI {
			NO_COND,		//!< �����Ȃ�
			YORINARI,		//!< ��萬��
			HIKENARI,		//!< ��������
			YORISASHI,		//!< ���w��
			HIKESASHI,		//!< �����w��
			FUNARI,			//!< �s����
			IOCNARI,		//!< IOC����
			IOCSASHI		//!< IOC�w��
		};

		/*****************************************************************//**
			���s����(�g���K�[�]�[���̏㉺)�̎w��
		 ********************************************************************/
		enum enTRIGGERZONE {
			MORE,	//!< �ȏ�
			LESS	//!< �ȉ�
		};
	
	public:
		CStockOrder();
		virtual ~CStockOrder();

		/*****************************************************************//**
			����敪��ݒ肷��

			�����̎���敪(��������/��������/�M�p����/�M�p����/�M�p�ԍϔ���/
			�M�p�ԍϔ���̂����ꂩ)��ݒ肵�܂��B

			@param trade [in] ����敪
			@return �Ȃ��B
			@see enTRADE
		 ********************************************************************/
		void SetTrade( int trade );

		/*****************************************************************//**
			����s���ݒ肷��

			�����̎���s���ݒ肵�܂��B

			@param market [in] �s��R�[�h
			@return �Ȃ��B
		 ********************************************************************/
		void SetMarket( int market ) { m_market = market; }

		/*****************************************************************//**
			��������̖����R�[�h��ݒ肷��

			�����̎�������R�[�h��ݒ肵�܂��B

			@param brand [in] �����R�[�h
			@return �Ȃ��B
		 ********************************************************************/
		void SetBrandCode( int brand ) { m_brand_code = brand; }

		/*****************************************************************//**
			������銔����ݒ肷��

			�����̎��������ݒ肵�܂��B

			@param quantity [in] �������
			@return �Ȃ��B
		 ********************************************************************/
		void SetQuantity( long quantity ) { m_quantity = quantity; }

		/*****************************************************************//**
			���s�����𐬍s�ɐݒ肷��

			�����̎��s�����𐬍s�ɐݒ肵�܂��B2�������Ƃ��Ċ�萬��E��������
			�EIOC����E�����Ȃ��̂������ꂩ��ݒ肵�܂��BCStockOrder�I�u�W�F�N�g
			���쐬��������̓f�t�H���g�Ő��s(�����Ȃ�)�ɐݒ肳��Ă��܂��B

			@param cond [in] 2�������B�ȗ�����Ə����Ȃ��Ƃ݂Ȃ����B
			@return �Ȃ��B
			@see enNARISASHI
		 ********************************************************************/
		void SetNariyuki(int cond = NO_COND) { m_nariyuki_cond = cond; m_sashine_price = 0; m_trigger_price = 0; }

		/*****************************************************************//**
			���s�������w�l�ɐݒ肷��

			�����̎��s�������w�l�ɐݒ肵�܂��B2�������Ƃ��Ċ��w���E�����w��
			�E�s����EIOC�w���E�����Ȃ��̂������ꂩ��ݒ肵�܂��B

			@param price [in] �w�l���i
			@param cond [in] 2�������B�ȗ�����Ə����Ȃ��Ƃ݂Ȃ����B
			@return �Ȃ��B
			@see enNARISASHI
		 ********************************************************************/
		void SetSashine( double price, int cond=NO_COND ) { m_sashine_price = price; m_sashine_cond = cond; }

		/*****************************************************************//**
			���s�������t�w�l�ɐݒ肷��

			�����̎��s�������t�w�l�ɐݒ肵�܂��B����������Őݒ肵���g���K�[
			���i�ɒB����ƁA���s(�����Ȃ�)�Œ����𔭒����܂��B���s(�����Ȃ�)
			�ȊO�̒������������ꍇ��SetTrigger�֐����g���Ă��������B�܂��A
			����API���R�[������O�ɕK��SetTrade�Ŏ���敪��ݒ肵�Ă��������B

			@param price [in] �g���K�[���i
			@return �Ȃ��B
			@see SetTrade()
			@see SetTrigger()
		 ********************************************************************/
		void SetGyakusashine( double price);

		/*****************************************************************//**
			���s�����̃g���K�[����ݒ肷��

			�����̎��s�����Ƀg���K�[��ݒ肵�A���̃g���K�[���i�ɒB�����ꍇ��
			���߂ď���̔�������(���s�܂��͎w�l)�𔭒�����悤�ɐݒ肵�܂��B
			�ʏ�͋t�w�l�������������邽�߂ɂ��̋@�\���g���܂��B���̃g���K�[
			�ݒ���s���ꍇ�́ASetNariyuki�֐��܂���SetSashine�֐��ƃZ�b�g��
			�g�p���Ă��������B
			
			@param price [in] �g���K�[���i
			@param zone [in] �g���K�[�]�[��(�g���K�[���i�ȏ�܂��͈ȉ�)
			@return �Ȃ��B
			@see SetNariyuki()
			@see SetSashine()
			@see enTRIGGERZONE
		 ********************************************************************/
		void SetTrigger( double price, int zone ) { m_trigger_price = price; m_trigger_zone = zone; }

		/*****************************************************************//**
			�M�p����敪��ݒ肷��

			�M�p����敪(���x�M�p����A��ʐM�p����܂��͓��v��)��ݒ肵�܂��B
			����敪���M�p�����܂��͐M�p����̏ꍇ�̂ݐݒ肪�L���ł��B
			CStockOrder�I�u�W�F�N�g���쐬��������́A�f�t�H���g�Ő��x�M�p���
			�ɐݒ肳��Ă��܂��B

			@param margin [in] �M�p����敪
			@return �Ȃ��B
			@see enMARGIN
			********************************************************************/
		void SetMargin( int margin ) { m_margin = margin; }

		/*****************************************************************//**
			�����̗a����敪��ݒ肷��

			�����̗a����敪(����a����E��ʗa����ENISA�a����)��ݒ肵�܂��B
			����敪�����������E��������E�M�p�����܂��͐M�p����̏ꍇ�̂�
			�ݒ肪�L���ł��BCStockOrder�I�u�W�F�N�g���쐬��������́A�f�t�H���g
			��true(����a����)�ɐݒ肳��Ă��܂��B

			@param azukari [in] �a���������\��������
			@return �Ȃ��B
		 ********************************************************************/
		void SetAzukari( const char* azukari ) { m_azukari = azukari; }

	private:
		int		m_trade;
		int		m_market;
		int		m_brand_code;
		long	m_quantity;
		int		m_nariyuki_cond;
		int		m_sashine_cond;
		double	m_sashine_price;
		double	m_trigger_price;
		int		m_trigger_zone;
		int		m_margin;
		std::string	m_azukari;
	};

public:
	CSBIWebClient();
	virtual ~CSBIWebClient();

     /********************************************************************//**
		SBI�،���Web�T�C�g�Ƀ��O�C������B

		���������̔����E�����E����⃊�A���^�C�������̎擾�Ȃǂ�API���g�p����
		�O�ɂ͕K������API�Ń��O�C��������K�v������܂��B

		@param username [in] ���[�U�[��
		@param password [in] �p�X���[�h
		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see Logout()
		@see enRETCODE
	  ***********************************************************************/
	int Login( const char* username, const char* password );

     /********************************************************************//**
		SBI�،���Web�T�C�g���烍�O�A�E�g����B

		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see Login()
		@see enRETCODE
	  ***********************************************************************/
	int Logout();

     /********************************************************************//**
		������{�����擾����B

		���O�C�����Ă�������̔��t�]�́E�M�p���]�́E���Y�]���z�Ȃǂ��擾���܂��B

		@param accountInfo [out] �������̍\���̂ւ̃|�C���^
		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see enRETCODE
	  ***********************************************************************/
	int GetAccountInfo( ACCOUNTINFO* accountInfo );

     /********************************************************************//**
		���A���^�C���������擾����B

		�w�肳�ꂽ�R�[�h�̖����̃��A���^�C������(4�{�l)���擾���܂��B
		�s�ꂪ�J�����O�Ȃǂ܂��l���t���Ă��Ȃ����ԑт�4�{�l�Ƃ��[�����Ԃ�܂��B

		@param code [in] �����R�[�h
		@param prices [out] ���A���^�C�������̍\���̂ւ̃|�C���^
		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see enRETCODE
	  ***********************************************************************/
	int	GetRealtimeStockPrice( int code, STOCKPRICES* prices );

     /********************************************************************//**
		���̔��������𔭒�����B

		�������܂��͐M�p����ɂ����锄�������𔭒����܂��B
		��������Ǝ󂯕t���������̒����ԍ����Ԃ���܂��B
		�߂�l��RET_UNKNOWN_ERROR�̏ꍇ�AGetLastOrderErrorDescr�֐����R�[��
		����Əڍ׃G���[��񂪎擾�ł���ꍇ������܂��B

		@param order [in] ���������i�[�����I�u�W�F�N�g
		@param orderNumber [out] �����ԍ����i�[����|�C���^
		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see enRETCODE
		@see GetLastOrderErrorDescr()
	  ***********************************************************************/
	int	StockOrder( const CStockOrder& order, int* orderNumber );

     /********************************************************************//**
		���̔����������������B

		����������������܂��B���̒����̓��e�ɂ���Ē����ł�����e�͕ς����
		����̂ł���API�̎g�p�ɂ͒��ӂ��K�v�ł��B�Ⴆ�Ό��̒������ʏ�̎w�l
		�����������ꍇ�́A�w�l���i��������邩�A���s�����ɒ������邩�A����
		�ύX�ł��܂���B�����������ŉ��ɋt�w�l�����ɒ��������ꍇ�ł��AAPI��
		�߂�l�Ƃ��Ă͐�����Ԃ��Ă��܂�(���ۂ͐������Ȃ�)�ꍇ������̂ŏ\��
		���ӂ��Ă��������B
		�߂�l��RET_UNKNOWN_ERROR�̏ꍇ�AGetLastOrderErrorDescr�֐����R�[��
		����Əڍ׃G���[��񂪎擾�ł���ꍇ������܂��B

		@param orderNumber [in] �������̒����̒����ԍ�
		@param order [in] ������̒��������i�[�����I�u�W�F�N�g
		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see enRETCODE
		@see GetLastOrderErrorDescr()
	  ***********************************************************************/
	int StockOrderModify( int orderNumber, const CStockOrder& order );

     /********************************************************************//**
		���̔���������������B

		����������������܂��B
		�߂�l��RET_UNKNOWN_ERROR�̏ꍇ�AGetLastOrderErrorDescr�֐����R�[��
		����Əڍ׃G���[��񂪎擾�ł���ꍇ������܂��B

		@param orderNumber [in] ����������̒����ԍ�
		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see StockOrder()
		@see enRETCODE
		@see GetLastOrderErrorDescr()
	  ***********************************************************************/
	int StockOrderCancel( int orderNumber );

     /********************************************************************//**
		�����̏�Ԃ��擾����B

		���������̒������(�ҋ@��/������/����/��������Ȃ�)���擾���܂��B
		���������ς�(�ꕔ�܂��͑S���)�̏ꍇ�͍Ō�ɖ�肵�����i��������
		�擾�ł��܂��B

		@param orderNumber [in] �擾���钍���̒����ԍ�
		@param orderStatus [out] ������Ԃ��i�[����|�C���^
		@param lastExecPrice [out] �ŏI��艿�i���i�[����|�C���^(�ȗ���)
		@return ��������ƃ[����Ԃ��܂��B�G���[�̏ꍇ�͕��̒l��Ԃ��܂��B
		@see StockOrder()
		@see enRETCODE
		@see enORDERSTATUS
	  ***********************************************************************/
	int GetStockOrderStatus( int orderNumber, int* orderStatus, double* lastExecPrice=NULL );

     /********************************************************************//**
		����p�X���[�h��ݒ肷��B

		���������̔���(StockOrder)�E����(StockOrderModify)�E���(StockOrderCancel)
		�̍ۂɋ��ʂɎg�������p�X���[�h��ݒ肵�܂��B

		@param password [in] ����p�X���[�h���i�[����|�C���^
		@return �Ȃ��B
		@see StockOrder()
		@see StockOrderModify()
		@see StockOrderCancel()
		@see enRETCODE
	  ***********************************************************************/
	void SetOrderPassword( const char* password ) { m_OrderPassword = password; }

     /********************************************************************//**
		�����G���[�̏ꍇ�ɃG���[��������擾����B

		���������̔���(StockOrder)�E����(StockOrderModify)�E���(StockOrderCancel)
		�ł��̑��̃G���[(RET_UNKNOWN_ERROR)���Ԃ��ꂽ�ꍇ�ɁA�{API���R�[�������
		�G���[�̏ڍ׏��(���{��)��\�������񂪎擾�ł��܂��B

		@return �G���[�ڍו�����ւ̃|�C���^�B
		@see StockOrder()
		@see StockOrderModify()
		@see StockOrderCancel()
		@see enRETCODE
	  ***********************************************************************/
	const char* GetLastOrderErrorDescr() const { return m_LastOrderErrorDescr.c_str(); }

private:
	int	StockOrderRepay( const CStockOrder& order, int* orderNumber );
	int RequestOrderListPage( int orderNumber, int* orderStatus, double* lastExecPrice,
										std::string* cancelLink, std::string* modifyLink );
	enum { MODE_NEW_ORDER, MODE_MODIFY_ORDER, MODE_REPAY_ORDER };
	void SetOrderInfoToParams( int mode, const CStockOrder& order, CHttpClient::CCGIParams* params );
	int SetRepayQuantityToParams( long quantity, CHttpClient::CCGIParams* params );

private:
public: // TODO:���ɖ߂�
	int ExtractLoginParams( CBuffer& buffer, std::string* loginURL, CHttpClient::CCGIParams* params );
	int ExtractLoginResult( CBuffer& buffer, std::string* sessionId, std::string* seqNo );
	int ExtractLogoutResult( CBuffer& buffer );
	bool ExtractOrderInfo( CBuffer& buffer,
						std::string* seqNo, std::string* sessionId, std::string* wstm4080 );
	int ExtractOrderStatus( CBuffer& buffer, int orderNumber, int* orderStatus, double* lastExecPrice,
		std::string* cancelLink, std::string* modifyLink, std::string* seqNo, std::string* nextLink );
	int ExtractOrderResult( CBuffer& buffer, int* orderNumber, std::string* errorDescr );
	int ExtractRepayLink( CBuffer& buffer, const CSBIWebClient::CStockOrder& order,
		std::string* repayLink );
	bool ExtractRealtimePrice( CBuffer& buffer, CSBIWebClient::STOCKPRICES* prices );
	int ExtractOrderFormParams( CBuffer& buffer, CHttpClient::CCGIParams* params );
	int ExtractAccountInfo( CBuffer& buffer, CSBIWebClient::ACCOUNTINFO* accountInfo );

	static int GetOrderErrorCode( const char* errorString );
	static std::string ToTradingFormCode( int trading );
	static std::string ToMarketFormCode( int market );
	static std::string ToMarginFormCode( int margin );
	static std::string ToNariyukiFormCode( int nariyuki_cond );
	static std::string ToSashineFormCode( int sashine_cond );
	static int ToOrderStatusCode( const char* statusString );
	static int ConvertHttpClientErrorCode( int code );
	static std::string DecodeHrefString( const std::string& src );
	static std::string MakeURLFromHrefString( const std::string& baseUrl, const std::string& href );
	static std::string IntToStr( int value );
	static std::string LongToStr( long value );
	static std::string DoubleToStr( double value );
	static int atoi2( const char* src );
	static long atol2( const char* src );
	static double atof2( const char* src );

private:
	CHttpClient	m_httpClient;
	std::string	m_LoginURL;
	std::string	m_OrderPassword;
	std::string m_LastOrderErrorDescr;
	std::string m_WBSessionID;
	std::string m_SeqNo;
	bool m_loggedin;
};

