//////////////////////////////////////////////////////////////////////////////
//	CSBIWebClientクラス
//		SBI証券の口座にログインしてリアルタイム株価取得や売買注文を出すクラス

#include "CHttpClient.h"
#include "CBuffer.h"

class CSBIWebClient 
{
public:
     /********************************************************************//**
		各API共通リターンコードを表す列挙値
	  ***********************************************************************/
	enum enRETCODE {
		RET_SUCCESS = 0,						//!< 成功
		RET_UNKNOWN_ERROR = -100,				//!< 不明な(未知の)エラー
		RET_INTERNET_ERROR = -101,				//!< インターネット通信のエラー
		RET_UNMATCH_USERNAME_OR_PASSWORD = -102,//!< ユーザー名かパスワードが正しくない
		RET_OUT_OF_STRENGTH = - 103,			//!< 買付余力(または信用建余力)が不足している
		RET_BRAND_NOT_FOUND = -104,				//!< 指定の銘柄コードはない
		RET_PRICE_RANGE_OVER = -105,			//!< 値幅範囲を超えている
		RET_TRADE_RESTRICTED = -106,			//!< この取引は規制されている
		RET_NOT_HAVE_POSITION = -107,			//!< 預かり残高(または建玉)が足りない
		RET_INSTRUCTION_ERROR = -108,			//!< 不正な指示である
		RET_ORDER_NOT_FOUND = -109,				//!< 指定された注文番号の注文はない
		RET_CANCEL_NOT_ACCEPTED = -110,			//!< 取消は受け付けられない
		RET_CANCEL_FAILED = -111,				//!< 取消は失敗した
		RET_PARSE_ERROR = -112,					//!< Webページの解析に失敗した
		RET_POSITION_NOT_FOUND = -113,			//!< 建玉がない
		RET_MODIFY_NOT_ACCEPTED = -114,			//!< 訂正は受け付けられない
		RET_NOT_LOGGEDIN = -115,				//!< ログインしていない
		RET_ORDER_PASSWORD_ERROR = -116,		//!< 取引パスワードが正しくない
		RET_QUANTITY_UNIT_ERROR = -117,			//!< 株数単位が正しくない
		RET_TEMP_FILE_ERROR = -118,				//!< 一時ファイルの処理に失敗した
		RET_MULTIPLE_REQUEST_ERROR = -119,		//!< 同じリクエストが複数回行われた
		RET_PRICE_UNIT_ERROR = -120,			//!< 価格単位が正しくない
		RET_IOC_ORDER_OVERTIME = -121,			//!< IOC注文は時間外のためできない
		RET_INSUFFICIENT_GUARANTEE = -122		//!< 最低委託保証金に足りない
	};

     /********************************************************************//**
		注文の状態の表す列挙値
	  ***********************************************************************/
	enum enORDERSTATUS {	
		UNKNOWN_STATUS = 0,		//!< 未知(未サポート)の状態
		WAITING,				//!< 待機中
		ORDERING,				//!< 注文中
		ORDER_COMPLETE,			//!< 完了
		CANCELING,				//!< 取消中
		CANCEL_COMPLETE,		//!< 取消完了
		MODIFYING,				//!< 訂正中
		EXPIRED					//!< 失効
	};

     /********************************************************************//**
		リアルタイム株価を表す構造体
	  ***********************************************************************/
#pragma	pack(1)
	typedef struct stSTOCKPRICES {
		double	open;		//!< 始値
		double	high;		//!< 高値
		double	low;		//!< 安値
		double	current;	//!< 現在値
	} STOCKPRICES;

     /********************************************************************//**
		口座情報を表す構造体
	  ***********************************************************************/
	typedef	struct stACCOUNTINFO {
		long	buy_strength;		//!< 買付余力
		long	margin_strength;	//!< 信用建余力
		long	asset_valuation;	//!< 資産評価額
	} ACCOUNTINFO;
#pragma	pack()

     /********************************************************************//**
		株の売買注文情報を格納するクラス
	  ***********************************************************************/
	class CStockOrder {
		friend class CSBIWebClient;

	public:
		/*****************************************************************//**
			取引区分を表す列挙値
		 ********************************************************************/
		enum enTRADE {
			BUY,			//!< 現物買い
			SELL,			//!< 現物売り
			MARGIN_BUY,		//!< 信用買い
			MARGIN_SELL,	//!< 信用売り
			REPAY_BUY,		//!< 信用返済買い
			REPAY_SELL		//!< 信用返済売り
		}; 

		/*****************************************************************//**
			取引市場の指定
		 ********************************************************************/
		enum enMARKET {
			SOR,			//!< 優先市場/SOR
			TOKYO,			//!< 東証
			OSAKA,			//!< 大証
			JASDAQ,			//!< ジャスダック
			NAGOYA,			//!< 名証
			FUKUOKA,		//!< 福証
			SAPPORO			//!< 札証
		}; 

		/*****************************************************************//**
			信用取引区分の指定
		 ********************************************************************/
		enum enMARGIN {
			SEIDO,			//!< 制度
			IPPAN,			//!< 一般
			DAY				//!< 日計り
		}; 

		/*****************************************************************//**
			執行条件(成行/指値)の指定
		 ********************************************************************/
		enum enNARISASHI {
			NO_COND,		//!< 条件なし
			YORINARI,		//!< 寄り成り
			HIKENARI,		//!< 引け成り
			YORISASHI,		//!< 寄り指し
			HIKESASHI,		//!< 引け指し
			FUNARI,			//!< 不成り
			IOCNARI,		//!< IOC成り
			IOCSASHI		//!< IOC指し
		};

		/*****************************************************************//**
			執行条件(トリガーゾーンの上下)の指定
		 ********************************************************************/
		enum enTRIGGERZONE {
			MORE,	//!< 以上
			LESS	//!< 以下
		};
	
	public:
		CStockOrder();
		virtual ~CStockOrder();

		/*****************************************************************//**
			取引区分を設定する

			注文の取引区分(現物買い/現物売り/信用買い/信用売り/信用返済買い/
			信用返済売りのいずれか)を設定します。

			@param trade [in] 取引区分
			@return なし。
			@see enTRADE
		 ********************************************************************/
		void SetTrade( int trade );

		/*****************************************************************//**
			取引市場を設定する

			注文の取引市場を設定します。

			@param market [in] 市場コード
			@return なし。
		 ********************************************************************/
		void SetMarket( int market ) { m_market = market; }

		/*****************************************************************//**
			取引銘柄の銘柄コードを設定する

			注文の取引銘柄コードを設定します。

			@param brand [in] 銘柄コード
			@return なし。
		 ********************************************************************/
		void SetBrandCode( int brand ) { m_brand_code = brand; }

		/*****************************************************************//**
			取引する株数を設定する

			注文の取引株数を設定します。

			@param quantity [in] 取引株数
			@return なし。
		 ********************************************************************/
		void SetQuantity( long quantity ) { m_quantity = quantity; }

		/*****************************************************************//**
			執行条件を成行に設定する

			注文の執行条件を成行に設定します。2次条件として寄り成り・引け成り
			・IOC成り・条件なしのいずかれかを設定します。CStockOrderオブジェクト
			を作成した直後はデフォルトで成行(条件なし)に設定されています。

			@param cond [in] 2次条件。省略すると条件なしとみなされる。
			@return なし。
			@see enNARISASHI
		 ********************************************************************/
		void SetNariyuki(int cond = NO_COND) { m_nariyuki_cond = cond; m_sashine_price = 0; m_trigger_price = 0; }

		/*****************************************************************//**
			執行条件を指値に設定する

			注文の執行条件を指値に設定します。2次条件として寄り指し・引け指し
			・不成り・IOC指し・条件なしのいずかれかを設定します。

			@param price [in] 指値価格
			@param cond [in] 2次条件。省略すると条件なしとみなされる。
			@return なし。
			@see enNARISASHI
		 ********************************************************************/
		void SetSashine( double price, int cond=NO_COND ) { m_sashine_price = price; m_sashine_cond = cond; }

		/*****************************************************************//**
			執行条件を逆指値に設定する

			注文の執行条件を逆指値に設定します。株価がこれで設定したトリガー
			価格に達すると、成行(条件なし)で注文を発注します。成行(条件なし)
			以外の注文をしたい場合はSetTrigger関数を使ってください。また、
			このAPIをコールする前に必ずSetTradeで取引区分を設定してください。

			@param price [in] トリガー価格
			@return なし。
			@see SetTrade()
			@see SetTrigger()
		 ********************************************************************/
		void SetGyakusashine( double price);

		/*****************************************************************//**
			執行条件のトリガー情報を設定する

			注文の執行条件にトリガーを設定し、そのトリガー価格に達した場合に
			初めて所定の売買注文(成行または指値)を発注するように設定します。
			通常は逆指値注文を実現するためにこの機能が使われます。このトリガー
			設定を行う場合は、SetNariyuki関数またはSetSashine関数とセットで
			使用してください。
			
			@param price [in] トリガー価格
			@param zone [in] トリガーゾーン(トリガー価格以上または以下)
			@return なし。
			@see SetNariyuki()
			@see SetSashine()
			@see enTRIGGERZONE
		 ********************************************************************/
		void SetTrigger( double price, int zone ) { m_trigger_price = price; m_trigger_zone = zone; }

		/*****************************************************************//**
			信用取引区分を設定する

			信用取引区分(制度信用取引、一般信用取引または日計り)を設定します。
			取引区分が信用買いまたは信用売りの場合のみ設定が有効です。
			CStockOrderオブジェクトを作成した直後は、デフォルトで制度信用取引
			に設定されています。

			@param margin [in] 信用取引区分
			@return なし。
			@see enMARGIN
			********************************************************************/
		void SetMargin( int margin ) { m_margin = margin; }

		/*****************************************************************//**
			口座の預かり区分を設定する

			口座の預かり区分(特定預かり・一般預かり・NISA預かり)を設定します。
			取引区分が現物買い・現物売り・信用買いまたは信用売りの場合のみ
			設定が有効です。CStockOrderオブジェクトを作成した直後は、デフォルト
			でtrue(特定預かり)に設定されています。

			@param azukari [in] 預かり口座を表す文字列
			@return なし。
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
		SBI証券のWebサイトにログインする。

		売買注文の発注・訂正・取消やリアルタイム株価の取得などのAPIを使用する
		前には必ずこのAPIでログインをする必要があります。

		@param username [in] ユーザー名
		@param password [in] パスワード
		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see Logout()
		@see enRETCODE
	  ***********************************************************************/
	int Login( const char* username, const char* password );

     /********************************************************************//**
		SBI証券のWebサイトからログアウトする。

		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see Login()
		@see enRETCODE
	  ***********************************************************************/
	int Logout();

     /********************************************************************//**
		口座基本情報を取得する。

		ログインしている口座の買付余力・信用建余力・資産評価額などを取得します。

		@param accountInfo [out] 口座情報の構造体へのポインタ
		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see enRETCODE
	  ***********************************************************************/
	int GetAccountInfo( ACCOUNTINFO* accountInfo );

     /********************************************************************//**
		リアルタイム株価を取得する。

		指定されたコードの銘柄のリアルタイム株価(4本値)を取得します。
		市場が開く直前などまだ値が付いていない時間帯は4本値ともゼロが返ります。

		@param code [in] 銘柄コード
		@param prices [out] リアルタイム株価の構造体へのポインタ
		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see enRETCODE
	  ***********************************************************************/
	int	GetRealtimeStockPrice( int code, STOCKPRICES* prices );

     /********************************************************************//**
		株の売買注文を発注する。

		現物株または信用取引における売買注文を発注します。
		成功すると受け付けた注文の注文番号が返されます。
		戻り値がRET_UNKNOWN_ERRORの場合、GetLastOrderErrorDescr関数をコール
		すると詳細エラー情報が取得できる場合があります。

		@param order [in] 注文情報を格納したオブジェクト
		@param orderNumber [out] 注文番号を格納するポインタ
		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see enRETCODE
		@see GetLastOrderErrorDescr()
	  ***********************************************************************/
	int	StockOrder( const CStockOrder& order, int* orderNumber );

     /********************************************************************//**
		株の売買注文を訂正する。

		売買注文を訂正します。元の注文の内容によって訂正できる内容は変わって
		くるのでこのAPIの使用には注意が必要です。例えば元の注文が通常の指値
		注文だった場合は、指値価格を訂正するか、成行注文に訂正するか、しか
		変更できません。しかしそこで仮に逆指値注文に訂正した場合でも、APIの
		戻り値としては成功を返してしまう(実際は成功しない)場合があるので十分
		注意してください。
		戻り値がRET_UNKNOWN_ERRORの場合、GetLastOrderErrorDescr関数をコール
		すると詳細エラー情報が取得できる場合があります。

		@param orderNumber [in] 訂正元の注文の注文番号
		@param order [in] 訂正後の注文情報を格納したオブジェクト
		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see enRETCODE
		@see GetLastOrderErrorDescr()
	  ***********************************************************************/
	int StockOrderModify( int orderNumber, const CStockOrder& order );

     /********************************************************************//**
		株の売買注文を取消す。

		売買注文を取消します。
		戻り値がRET_UNKNOWN_ERRORの場合、GetLastOrderErrorDescr関数をコール
		すると詳細エラー情報が取得できる場合があります。

		@param orderNumber [in] 取消す注文の注文番号
		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see StockOrder()
		@see enRETCODE
		@see GetLastOrderErrorDescr()
	  ***********************************************************************/
	int StockOrderCancel( int orderNumber );

     /********************************************************************//**
		注文の状態を取得する。

		売買注文の注文状態(待機中/注文中/完了/取消完了など)を取得します。
		注文が約定済み(一部または全約定)の場合は最後に約定した価格も同時に
		取得できます。

		@param orderNumber [in] 取得する注文の注文番号
		@param orderStatus [out] 注文状態を格納するポインタ
		@param lastExecPrice [out] 最終約定価格を格納するポインタ(省略可)
		@return 成功するとゼロを返します。エラーの場合は負の値を返します。
		@see StockOrder()
		@see enRETCODE
		@see enORDERSTATUS
	  ***********************************************************************/
	int GetStockOrderStatus( int orderNumber, int* orderStatus, double* lastExecPrice=NULL );

     /********************************************************************//**
		取引パスワードを設定する。

		売買注文の発注(StockOrder)・訂正(StockOrderModify)・取消(StockOrderCancel)
		の際に共通に使われる取引パスワードを設定します。

		@param password [in] 取引パスワードを格納するポインタ
		@return なし。
		@see StockOrder()
		@see StockOrderModify()
		@see StockOrderCancel()
		@see enRETCODE
	  ***********************************************************************/
	void SetOrderPassword( const char* password ) { m_OrderPassword = password; }

     /********************************************************************//**
		発注エラーの場合にエラー文字列を取得する。

		売買注文の発注(StockOrder)・訂正(StockOrderModify)・取消(StockOrderCancel)
		でその他のエラー(RET_UNKNOWN_ERROR)が返された場合に、本APIをコールすると
		エラーの詳細情報(日本語)を表す文字列が取得できます。

		@return エラー詳細文字列へのポインタ。
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
public: // TODO:元に戻す
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

