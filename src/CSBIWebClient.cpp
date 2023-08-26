//////////////////////////////////////////////////////////////////////////////
//	CSBIWebClientクラス
//////////////////////////////////////////////////////////////////////////////

#include "CSBIWebClient.h"
#include "CHtmlTokenizer.h"
#include "CBufferString.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//	CStockOrder 構築と破棄

CSBIWebClient::CStockOrder::CStockOrder()
	:	m_trade(BUY),
		m_market(SOR),
		m_brand_code(1000),
		m_quantity(1),
		m_nariyuki_cond(NO_COND),
		m_sashine_cond(NO_COND),
		m_sashine_price(0),
		m_trigger_price(0),
		m_trigger_zone(MORE),
		m_margin(SEIDO),
		m_azukari("0")
{
}

CSBIWebClient::CStockOrder::~CStockOrder()
{
}

//////////////////////////////////////////////////////////////////////////////
//	CStockOrder 操作

void CSBIWebClient::CStockOrder::SetTrade( int trade )
{
	m_trade = trade;

	//	信用売りか返済買いなら制度信用取引に設定する
	if (trade == MARGIN_SELL || trade == REPAY_BUY)
		SetMargin( SEIDO );
}

void CSBIWebClient::CStockOrder::SetGyakusashine( double price )
{
	//	簡易的な逆指値(トリガー価格に達したら成行で執行)に設定する
	SetNariyuki();
	SetTrigger( price, (m_trade == BUY || m_trade == MARGIN_BUY || m_trade == REPAY_BUY)
					? MORE : LESS );
}

//////////////////////////////////////////////////////////////////////////////
//	CSBIWebClient 構築と破棄

CSBIWebClient::CSBIWebClient()
	: m_loggedin(false)
{
}

CSBIWebClient::~CSBIWebClient()
{
}

//////////////////////////////////////////////////////////////////////////////
//	CSBIWebClient 操作

int CSBIWebClient::Login( const char* username, const char* password )
{
	int rc;
	CBufferString buffer;
	CHttpClient::CCGIParams params, params2;

	params.Add( "JS_FLG", "0" );
	params.Add( "BW_FLG", "0" );
	params.Add( "_ControlID", "WPLETlgR001Control" );
	params.Add( "_DataStoreID", "DSWPLETlgR001Control" );
	params.Add( "_PageID", "WPLETlgR001Rlgn10" );
	params.Add( "_ActionID", "loginHome" );
	params.Add( "getFlg", "on" );
	params.Add( "user_id", username );	
	params.Add( "user_password", password );

	if ((rc = m_httpClient.MethodPost( "https://www.sbisec.co.jp/ETGate", NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	if ((rc = ExtractLoginParams( buffer, &m_LoginURL, &params2 )) < 0)
		return rc;	//	ユーザー名が間違っている場合はここではじかれる

	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params2, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	if ((rc = ExtractLoginResult( buffer, &m_WBSessionID, &m_SeqNo )) == RET_SUCCESS)
		m_loggedin = true;

	return rc;
}

int CSBIWebClient::Logout()
{
	if (!m_loggedin) return RET_NOT_LOGGEDIN;

	int rc;
	CBufferString buffer;
	string url = m_LoginURL +
		"?_ControlID=WPLETlgR001Control&_PageID=WPLETlgR001Rlgn50&_DataStoreID=DSWPLETlgR001Control"
		"&_ActionID=logout&getFlg=on";

	if ((rc = m_httpClient.MethodGet( url.c_str(), &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	if ((rc = ExtractLogoutResult( buffer )) == RET_SUCCESS)
		m_loggedin = false;

	return rc;
}

int CSBIWebClient::GetAccountInfo( CSBIWebClient::ACCOUNTINFO* accountInfo )
{
	if (!m_loggedin) return RET_NOT_LOGGEDIN;

	int rc;
	CBufferString buffer;
	CHttpClient::CCGIParams params;

	params.Add( "_ControlID", "WPLETacR001Control" );
	params.Add( "_PageID", "DefaultPID" );
	params.Add( "_DataStoreID", "DSWPLETacR001Control" );
	params.Add( "_ActionID", "DefaultAID" );
	params.Add( "getFlg", "on" );

	//	GETでするとなぜかキャッシュされてしまうようなのでとりあえずPOSTに変更(2013/6/8)
	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	if (!ExtractAccountInfo( buffer, accountInfo ))
		return RET_PARSE_ERROR;

	return RET_SUCCESS;
}

int CSBIWebClient::GetRealtimeStockPrice( int code, CSBIWebClient::STOCKPRICES* prices )
{
	if (!m_loggedin) return RET_NOT_LOGGEDIN;

	int rc;
	CBufferString buffer;
	CHttpClient::CCGIParams params;

	params.Add( "_ControlID", "WPLETsiR001Control" );
	params.Add( "_PageID", "WPLETsiR001Iser10" );
	params.Add( "_DataStoreID", "DSWPLETsiR001Control" );
	params.Add( "_ActionID", "clickToSearchStockPriceJP" );
	params.Add( "exchange_code", "JPN" );	
	params.Add( "i_output_type", "0" );	
	params.Add( "i_dom_flg", "1" );	
	params.Add( "getFlg", "on" );
	params.Add( "i_stock_sec", IntToStr( code ) );	
	params.Add( "i_stock_sec1", "" );	

	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	if (!ExtractRealtimePrice( buffer, prices ))
		return RET_PARSE_ERROR;

	return RET_SUCCESS;
}


int CSBIWebClient::StockOrder( const CStockOrder& order, int* orderNumber )
{
	if (!m_loggedin) return RET_NOT_LOGGEDIN;

	//	信用返済注文の場合は別のメンバ関数で処理する
	if (order.m_trade == order.REPAY_BUY || order.m_trade == order.REPAY_SELL)
		return StockOrderRepay( order, orderNumber );

	int rc;
	CBufferString buffer;

	//------------------------------------------------------------------------
	//	取引(注文)ページの取得
	//------------------------------------------------------------------------
	string url = m_LoginURL +
		"?_ControlID=WPLETstT001Control&_PageID=DefaultPID&_DataStoreID=DSWPLETstT001Control"
		"&_ActionID=DefaultAID&getFlg=on"
		"&_SeqNo=" + CHttpClient::URLEncode( m_SeqNo );

	if ((rc = m_httpClient.MethodGet( url.c_str(), &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	CHttpClient::CCGIParams params;

	//	新規注文のフォームのHIDDENパラメータを取得
	if ((rc = ExtractOrderFormParams( buffer,  &params )) < 0)
		return rc;

	//	取得したパラメータの一部は使いまわすので保存しておく
	if (!params.Get("_SeqNo").empty())
		m_SeqNo = params.Get("_SeqNo");
	if (!params.Get("_WBSessionID").empty())
		m_WBSessionID = params.Get("_WBSessionID");

	//------------------------------------------------------------------------
	//	注文の発注
	//------------------------------------------------------------------------

	SetOrderInfoToParams( MODE_NEW_ORDER, order, &params );

	params.Add( "skip_estimate", "on" );
	params.Add( "ACT_place", "注文発注" );

	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	string errorDescr;

	if ((rc = ExtractOrderResult( buffer, orderNumber, &errorDescr )) < 0)
		m_LastOrderErrorDescr = errorDescr;

	return rc;
}


int CSBIWebClient::StockOrderCancel( int orderNumber )
{
	if (!m_loggedin) return RET_NOT_LOGGEDIN;

	int rc;
	CBufferString buffer;

	//------------------------------------------------------------------------
	//	注文一覧ページの取得
	//------------------------------------------------------------------------
	string cancelLink;

	if ((rc = RequestOrderListPage( orderNumber, NULL, NULL, &cancelLink, NULL )) < 0)
		return rc;

	//	注文一覧ページにこの注文の取消リンクがなければエラー
	if (cancelLink.empty())
		return RET_CANCEL_NOT_ACCEPTED;

	//------------------------------------------------------------------------
	//	取消注文ページの取得
	//------------------------------------------------------------------------
	string url = MakeURLFromHrefString( m_LoginURL, cancelLink );

	if ((rc = m_httpClient.MethodGet( url.c_str(), &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	CHttpClient::CCGIParams params;

	//	取消注文のフォームのHIDDENパラメータを取得
	if ((rc = ExtractOrderFormParams( buffer, &params )) < 0)
		return rc;

	//	取得したパラメータの一部は使いまわすので保存しておく
	if (!params.Get("_SeqNo").empty())
		m_SeqNo = params.Get("_SeqNo");
	if (!params.Get("_WBSessionID").empty())
		m_WBSessionID = params.Get("_WBSessionID");

	//------------------------------------------------------------------------
	//	取消注文の発注
	//------------------------------------------------------------------------

	params.Add( "trade_pwd", m_OrderPassword );

	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	string errorDescr;

	if ((rc = ExtractOrderResult( buffer, NULL, &errorDescr )) < 0)
		m_LastOrderErrorDescr = errorDescr;

	return rc;
}

int CSBIWebClient::StockOrderModify( int orderNumber, const CStockOrder& order )
{
	if (!m_loggedin) return RET_NOT_LOGGEDIN;

	int rc;
	CBufferString buffer;

	//------------------------------------------------------------------------
	//	注文一覧ページの取得
	//------------------------------------------------------------------------
	string modifyLink;

	if ((rc = RequestOrderListPage( orderNumber, NULL, NULL, NULL, &modifyLink )) < 0)
		return rc;

	//	注文一覧ページにこの注文の訂正リンクがなければエラー
	if (modifyLink.empty())
		return RET_MODIFY_NOT_ACCEPTED;

	//------------------------------------------------------------------------
	//	訂正注文ページの取得
	//------------------------------------------------------------------------
	string url = MakeURLFromHrefString( m_LoginURL, modifyLink );

	if ((rc = m_httpClient.MethodGet( url.c_str(), &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	CHttpClient::CCGIParams params;

	//	訂正注文のフォームのHIDDENパラメータを取得
	if ((rc = ExtractOrderFormParams( buffer, &params )) < 0)
		return rc;

	//	取得したパラメータの一部は使いまわすので保存しておく
	if (!params.Get("_SeqNo").empty())
		m_SeqNo = params.Get("_SeqNo");
	if (!params.Get("_WBSessionID").empty())
		m_WBSessionID = params.Get("_WBSessionID");

	//------------------------------------------------------------------------
	//	訂正注文の発注
	//------------------------------------------------------------------------
	SetOrderInfoToParams( MODE_MODIFY_ORDER, order, &params );

	params.Add( "skip_estimate", "on" );
	params.Add( "ACT_place", "注文発注" );

	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	string errorDescr;

	if ((rc = ExtractOrderResult( buffer, NULL, &errorDescr )) < 0)
		m_LastOrderErrorDescr = errorDescr;

	return rc;
}

int CSBIWebClient::GetStockOrderStatus( int orderNumber, int* orderStatus, double* lastExecPrice )
{
	if (!m_loggedin) return RET_NOT_LOGGEDIN;

	return RequestOrderListPage( orderNumber, orderStatus, lastExecPrice, NULL, NULL );
}

//////////////////////////////////////////////////////////////////////////////
//	CSBIWebClient ヘルパー関数

int CSBIWebClient::StockOrderRepay( const CStockOrder& order, int* orderNumber )
{
	int rc;
	CBufferString buffer;

	/* デバッグ用 2022.5.1
	remove("RepayOrder0.txt");
	remove("RepayOrder1.html");
	remove("RepayOrder2.html");
	remove("RepayOrder3.html");
	FILE* stream;
	stream = fopen("RepayOrder0.txt", "wb");
	if (stream != NULL)
	{
		fprintf(stream, "trade=%d market=%d brand=%d quantity=%d\n", order.m_trade, order.m_market, order.m_brand_code, order.m_quantity);
		fclose(stream);
	}
	*/
	
	//------------------------------------------------------------------------
	//	信用建玉一覧(一括表示)ページの取得
	//------------------------------------------------------------------------
	string url = m_LoginURL +
		"?_ControlID=WPLETstT007Control&_PageID=WPLETstT007Rlst10&_DataStoreID=DSWPLETstT007Control"
		"&getFlg=on&_ActionID=displayAll&gamen_status=A"
		"&_SeqNo=" + CHttpClient::URLEncode( m_SeqNo );

	if ((rc = m_httpClient.MethodGet( url.c_str(), &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	//buffer.Save("RepayOrder1.html");

	string repayLink;

	if ((rc = ExtractRepayLink( buffer, order, &repayLink )) < 0)
		return rc;

	//------------------------------------------------------------------------
	//	個別信用返済建玉指定ページの取得
	//------------------------------------------------------------------------
	url = MakeURLFromHrefString( m_LoginURL, repayLink );

	if ((rc = m_httpClient.MethodGet( url.c_str(), &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	//buffer.Save("RepayOrder2.html");

	CHttpClient::CCGIParams params;

	//	返済注文のフォームのHIDDENパラメータを取得
	if ((rc = ExtractOrderFormParams( buffer, &params )) < 0)
		return rc;

//	以下はなくても大丈夫らしいからコメントアウト
//	params.Set( "selected_limit_in", "this_day" );
//	params.Set( "limit_in", "14/11/12" );
//	params.Set( "mkt_cd", "TKY" );

	//	建玉指定パラメータをセット
	params.Set( "input_payment_kbn", "0" );	//	[0:一括指定 1:個別指定]
	params.Set( "sort_type", "16" );		//	[61:評価益順 51:評価損順 16:建日古順 26:建日新順]
	params.Set( "quantity_unify", LongToStr(order.m_quantity) );	//	[株数]
	params.Set( "request_type", "16" );		//	[61:評価益順 51:評価損順 16:建日古順 26:建日新順]
	params.Set( "ACT_order", "全株指定" );

	//	取得したパラメータの一部は使いまわすので保存しておく
	if (!params.Get("_SeqNo").empty())
		m_SeqNo = params.Get("_SeqNo");
	if (!params.Get("_WBSessionID").empty())
		m_WBSessionID = params.Get("_WBSessionID");

//	CHttpClient::CCGIParams::const_iterator iter;
//	for (iter = params.begin(); iter != params.end(); iter++)
//		printf("[%s]%s\n", iter->first.c_str(), iter->second.c_str());

	//------------------------------------------------------------------------
	//	信用返済注文ページの取得
	//------------------------------------------------------------------------
	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	//buffer.Save("RepayOrder3.html");

	//	返済注文のフォームのHIDDENパラメータを取得
	if ((rc = ExtractOrderFormParams( buffer, &params )) < 0)
		return rc;

	//------------------------------------------------------------------------
	//	信用返済注文の発注
	//------------------------------------------------------------------------
	SetOrderInfoToParams( MODE_REPAY_ORDER, order, &params );

	params.Add( "skip_estimate", "on" );
	params.Add( "ACT_place", "注文発注" );

	if ((rc = m_httpClient.MethodPost( m_LoginURL.c_str(), NULL, params, &buffer )) < 0)
		return ConvertHttpClientErrorCode( rc );

	string errorDescr;

	if ((rc = ExtractOrderResult( buffer, orderNumber, &errorDescr )) < 0)
	{
		m_LastOrderErrorDescr = errorDescr;
	}

	return rc;
}

int CSBIWebClient::RequestOrderListPage( int orderNumber, int* orderStatus, double* lastExecPrice,
														string* cancelLink, string* modifyLink )
{
	int rc;
	CBufferString buffer;
	string nextLink;

	string url = m_LoginURL +
		"?_ControlID=WPLETstT012Control&_PageID=WPLETstT012Rlst10&_DataStoreID=DSWPLETstT012Control"
		"&_ActionID=search&getFlg=on"
		"&_SeqNo=" + CHttpClient::URLEncode( m_SeqNo );

	//	注文照会が1ページに収まらない場合は注文番号が見つかるまでページを走査する
	do {
		if (!nextLink.empty())
			url = MakeURLFromHrefString( m_LoginURL, nextLink );

		if ((rc = m_httpClient.MethodGet( url.c_str(), &buffer )) < 0)
			return ConvertHttpClientErrorCode( rc );

		rc = ExtractOrderStatus( buffer, orderNumber, orderStatus, lastExecPrice,
									cancelLink,	modifyLink, &m_SeqNo, &nextLink );

	} while (rc == RET_ORDER_NOT_FOUND && !nextLink.empty());

	return rc;
}

bool CSBIWebClient::ExtractRealtimePrice( CBuffer& buffer, CSBIWebClient::STOCKPRICES* prices )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;

	if (!tokenizer.OpenBuffer( &buffer ))
		return false;
	
	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "現在値")
					status = 1;
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT)
				status = 2;
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				prices->current = atof2( token.c_str() );
				status = 3;
			}
			break;
		case 3:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "始値")
					status = 4;
			}
			break;
		case 4:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				prices->open = atof2( token.c_str() );
				status = 5;
			}
			break;
		case 5:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "高値")
					status = 6;
			}
			break;
		case 6:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				prices->high = atof2( token.c_str() );
				status = 7;
			}
			break;
		case 7:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "安値")
					status = 8;
			}
			break;
		case 8:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				prices->low = atof2( token.c_str() );
				status = STATUS_SUCCESS;
			}
			break;
		}
	}
	
	return (status == STATUS_SUCCESS);
}

int CSBIWebClient::ExtractLoginParams( CBuffer& buffer, string* loginURL, CHttpClient::CCGIParams* params )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token, param_name;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;
	int result;

	if (!tokenizer.OpenBuffer( &buffer ))
		return RET_TEMP_FILE_ERROR;

	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "action" ) == 0)
					status = 1;
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				*loginURL = token;
				status = 2;
			}
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "form" ) == 0) {
					status = 10;
				}
			}
			else if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (_stricmp( token.c_str(), "hidden" ) == 0)
					status = 3;
			}
			break;
		case 3:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "name" ) == 0)
					status = 4;
			}
			break;
		case 4:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				param_name = token;
				status = 5;
			}
			break;
		case 5:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "value" ) == 0)
					status = 6;
			}
			else if (tokenType != CHtmlTokenizer::ATTR_VALUE) {
				status = 2;
			}
			break;
		case 6:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				params->Add( param_name, token );
				status = 2;
			}
			break;
		case 10:
			if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "html" ) == 0) {
					result = RET_SUCCESS;
					status = STATUS_SUCCESS;
				}
			}
			else if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (strstr( token.c_str(), "WBLE011200" ) != NULL) {
					//	ユーザー名が間違っているときここに到達する
					result = RET_UNMATCH_USERNAME_OR_PASSWORD;
					status = STATUS_SUCCESS;
				}
			}
			break;
		}
	}

	return (status == STATUS_SUCCESS) ? result : RET_PARSE_ERROR;
}

int CSBIWebClient::ExtractLoginResult( CBuffer& buffer, string* sessionId, string* seqNo )
{
	int result;
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;

	seqNo->clear();
	sessionId->clear();

	if (!tokenizer.OpenBuffer( &buffer ))
		return RET_TEMP_FILE_ERROR;

	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (strstr( token.c_str(), "WBLE015002" ) != NULL) {
					//	ログインパスワードが間違っているときここに到達する
					result = RET_UNMATCH_USERNAME_OR_PASSWORD;
					status = STATUS_SUCCESS;
				}
			}
			else if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (token == "_SeqNo")
					status = 2;
				else if (token == "_WBSessionID")
					status = 1;
			}
			else if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "html" ) == 0) {
					if (!seqNo->empty() && !sessionId->empty()) {
						result = RET_SUCCESS;
						status = STATUS_SUCCESS;
					}
				}
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				*sessionId = token;
				status = 0;
			}
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				*seqNo = token;
				status = 0;
			}
			break;
		}
	}

	return (status == STATUS_SUCCESS) ? result : RET_PARSE_ERROR;
}

int CSBIWebClient::ExtractLogoutResult( CBuffer& buffer )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;

	if (!tokenizer.OpenBuffer( &buffer ))
		return RET_TEMP_FILE_ERROR;

	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
			if (token == "ログアウト") {
				status = STATUS_SUCCESS;
			}
		}
	}

	return (status == STATUS_SUCCESS) ? RET_SUCCESS : RET_PARSE_ERROR;
}

//	注意！約定が複数回にわたった場合、以下のメソッドが正常動作するかどうかは未確認！

int CSBIWebClient::ExtractOrderStatus( CBuffer& buffer, int orderNumber, int* orderStatus,
		double* lastExecPrice, string* cancelLink, string* modifyLink, string* seqNo, string* nextLink )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token, hrefString, prev_attrval;
	enum { STATUS_SUCCESS = 1000 };
	int result, status = 0;
	bool exec_row = false;
	int row_end_counter, td_start_counter = 0;
	struct OrderItem {
		int		orderNumber;
		string	orderStatus;
		string	cancelLink;
		string	modifyLink;
		double	execPrice;
	} orderItem;

	if (!tokenizer.OpenBuffer( &buffer ))
		return RET_TEMP_FILE_ERROR;

	nextLink->clear();

	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (token == "_SeqNo")
					status = 100;
				else
					prev_attrval = token;
			}
			else if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "次へ→")
					*nextLink = prev_attrval;
				else if (token == "約定単価")
					status = 1;
				else if (token == "同じリクエストが複数回行われた可能性がございます。")
					return RET_MULTIPLE_REQUEST_ERROR;
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				//	各注文行の情報を保存する構造体を初期化しておく
				orderItem.orderNumber = atoi( token.c_str() );
				orderItem.orderStatus.clear();
				orderItem.cancelLink.clear();
				orderItem.modifyLink.clear();
				orderItem.execPrice = 0;
				status = 2;
			}
			else if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "table" ) == 0) {
					//	指定の注文が見つからなかったらここへ到達する
					result = RET_ORDER_NOT_FOUND;
					status = STATUS_SUCCESS;
				}
			}
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "rowspan" ) == 0)
					status = 3;
			}
			break;
		case 3:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				row_end_counter = atoi( token.c_str() );
				status = 4;
			}
			break;
		case 4:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				orderItem.orderStatus = token;
				status = 5;
			}
			break;
		case 5:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "href" ) == 0)
					status = 6;
			}
			else if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "tr" ) == 0) {
					if (--row_end_counter == 0) {
						//	1注文行の確定
						if (orderItem.orderNumber == orderNumber) {
							result = RET_SUCCESS;
							status = STATUS_SUCCESS;
						}
						else status = 1;	//	次の注文行へ
					}
					td_start_counter = 0;
					exec_row = false;
				}
			}
			else if (tokenType == CHtmlTokenizer::TAG_START) {
				if (_stricmp( token.c_str(), "td" ) == 0)
					td_start_counter++;
			}
			else if (td_start_counter == 1 && tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "約定")
					exec_row = true;
			}
			else if (exec_row && td_start_counter == 4 && tokenType == CHtmlTokenizer::OTHER_TEXT) {
				//	各注文の行で最後に約定した価格を保存しておく
				orderItem.execPrice = atoi2( token.c_str() );
			}
			break;
		case 6:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				hrefString = token;
				status = 7;
			}
			break;
		case 7:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "取消")
					orderItem.cancelLink = hrefString;
				else if (token == "訂正")
					orderItem.modifyLink = hrefString;
				status = 5;
			}
			break;
		case 100:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (seqNo != NULL)
					*seqNo = token;
				status = 0;
			}
			break;
		}
	}

	if (status == STATUS_SUCCESS) {
		if (result == RET_SUCCESS) {
			if (orderStatus != NULL)
				*orderStatus = ToOrderStatusCode( orderItem.orderStatus.c_str() );
			if (lastExecPrice != NULL)
				*lastExecPrice = orderItem.execPrice;
			if (cancelLink != NULL)
				*cancelLink = orderItem.cancelLink;
			if (modifyLink != NULL)
				*modifyLink = orderItem.modifyLink;
		}
		return result;
	} else {
		return (status == 0) ? RET_ORDER_NOT_FOUND : RET_PARSE_ERROR;
	}
}

int CSBIWebClient::ExtractRepayLink( CBuffer& buffer,
			const CSBIWebClient::CStockOrder& order, string* repayLink )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token, hrefString;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;
	bool found = false;
	int margin;
	int brand_code;
	int brand_row_counter;
	int buysell_row_counter;
	int td_start_counter = 0;
	int rowspan = 0;

	repayLink->clear();

	if (!tokenizer.OpenBuffer( &buffer ))
		return RET_TEMP_FILE_ERROR;

	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "取引") {
					status = 1;
				}
				else if (token == "同じリクエストが複数回行われた可能性がございます。") {
					return RET_MULTIPLE_REQUEST_ERROR;
				}
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (_stricmp( token.c_str(), "alL multiline" ) == 0) {
					brand_row_counter = rowspan;
					status = 2;
				} else
					rowspan = atoi2( token.c_str() );
			} else if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "table" ) == 0)
					status = STATUS_SUCCESS;
			}
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT)
				status = 4;
			/*else if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "rowspan" ) == 0)
					status = 3;
			}*/
			break;
		/*case 3:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				brand_row_counter = atoi( token.c_str() );
				status = 2;
			}
			break;*/
		case 4:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				brand_code = atoi2( token.c_str() );
				status = 5;
			}
			break;
		case 5:
			if (tokenType == CHtmlTokenizer::TAG_START) {
				if (_stricmp( token.c_str(), "td" ) == 0) {
					buysell_row_counter = 1;
					status = 6;
				}
			}
			break;
		case 6:
			if (tokenType == CHtmlTokenizer::TAG_START) {
				if (_stricmp( token.c_str(), "td" ) == 0)
					status = 8;
			}
			else if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "rowspan" ) == 0)
					status = 7;
			}
			break;
		case 7:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				buysell_row_counter = atoi( token.c_str() );
				status = 6;
			}
			break;
		case 8:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT)
				status = 9;
			break;
		case 9:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				//puts(token.c_str());
				if (token == "無期限")
					margin = CStockOrder::IPPAN;
				else if (token == "1日")
					margin = CStockOrder::DAY;
				else
					margin = CStockOrder::SEIDO;
				td_start_counter = 0;
				status = 10;
			}
			break;
		case 10:
			if (tokenType == CHtmlTokenizer::TAG_START) {
				if (_stricmp( token.c_str(), "td" ) == 0) {
					if (++td_start_counter == 7)
						status = 11;
				}
			}
			else if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp(token.c_str(), "tr") == 0) {
					brand_row_counter--;
					buysell_row_counter--;
					if (brand_row_counter == 0)
						status = 1;
					else if (buysell_row_counter == 0)
						status = 5;
					else
						status = 6;
				}
			}
			break;
		case 11:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "href" ) == 0)
					status = 12;
			}
			else if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "tr" ) == 0) {
					brand_row_counter--;
					buysell_row_counter--;
					if (brand_row_counter == 0)
						status = 1;
					else if (buysell_row_counter == 0)
						status = 5;
					else
						status = 6;
				}
			}
			break;
		case 12:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				//printf("brand=%d href=%s\n", brand_code, token.c_str());
				hrefString = token;
				status = 13;
			}
			break;
		case 13:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (brand_code == order.m_brand_code 
					&& order.m_margin == margin
					&& ((order.m_trade == order.REPAY_BUY && token == "返買")
						|| (order.m_trade == order.REPAY_SELL && token == "返売")) )
				{
					found = true;
					*repayLink = hrefString;
					status = STATUS_SUCCESS;
				}
				else status = 11;
			}
			break;
		}
	}

	if (found) {
		return RET_SUCCESS;
	} else {
		return (status == STATUS_SUCCESS || status == 0) ? RET_POSITION_NOT_FOUND : RET_PARSE_ERROR;
	}
}

bool CSBIWebClient::ExtractOrderInfo( CBuffer& buffer,
		string* seqNo, string* sessionId, string* wstm4080 )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;

	if (!tokenizer.OpenBuffer( &buffer ))
		return false;

	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (_stricmp( token.c_str(), "_SeqNo" ) == 0)
					status = 1;
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				*seqNo = token;
				status = 2;
			}
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (_stricmp( token.c_str(), "_WBSessionID" ) == 0)
					status = 3;
			}
			break;
		case 3:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				*sessionId = token;
				status = 4;
			}
			break;
		case 4:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (_stricmp( token.c_str(), "BE_WSTM4080" ) == 0)
					status = 5;
			}
			break;
		case 5:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				*wstm4080 = token;
				status = STATUS_SUCCESS;
			}
			break;
		}
	}

	return (status == STATUS_SUCCESS);
}

int CSBIWebClient::ExtractOrderFormParams( CBuffer& buffer, CHttpClient::CCGIParams* params )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token, param_name;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;

	params->clear();

	if (!tokenizer.OpenBuffer( &buffer ))
		return RET_TEMP_FILE_ERROR;

	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::TAG_START) {
				if (_stricmp( token.c_str(), "form" ) == 0)
					status = 1;
			}
			else if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "同じリクエストが複数回行われた可能性がございます。")
					return RET_MULTIPLE_REQUEST_ERROR;
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::TAG_START) {
				if (_stricmp( token.c_str(), "form" ) == 0)
					status = 10;
			}
			break;
		case 10:	//	2012/5/27 サイトデザイン変更に合わせて追加
			if (tokenType == CHtmlTokenizer::TAG_START) {
				if (_stricmp( token.c_str(), "form" ) == 0)
					status = 2;
			}
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::TAG_END) {
				if (_stricmp( token.c_str(), "form" ) == 0)
					status = STATUS_SUCCESS;
			}
			else if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				if (_stricmp( token.c_str(), "hidden" ) == 0)
					status = 3;
			}
			break;
		case 3:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "name" ) == 0)
					status = 4;
			}
			break;
		case 4:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				param_name = token;
				status = 5;
			}
			break;
		case 5:
			if (tokenType == CHtmlTokenizer::ATTR_NAME) {
				if (_stricmp( token.c_str(), "value" ) == 0)
					status = 6;
			}
			else if (tokenType != CHtmlTokenizer::ATTR_VALUE) {
				status = 2;
			}
			break;
		case 6:
			if (tokenType == CHtmlTokenizer::ATTR_VALUE) {
				params->Add( param_name, token );
				status = 2;
			}
			break;
		}
	}

	return (status == STATUS_SUCCESS) ? RET_SUCCESS : RET_PARSE_ERROR;
}

int CSBIWebClient::ExtractOrderResult( CBuffer& buffer, int* orderNumber, string* errorDescr )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token, errorString;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;
	bool orderAccepted = false;

	if (!tokenizer.OpenBuffer( &buffer ))
		return RET_TEMP_FILE_ERROR;
	
	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (strstr( token.c_str(), "注文番号" ) != NULL)
					status = 1;
				else if (strstr( token.c_str(), "(W" ) != NULL) {
					//	エラーコードの文字列を抽出する。しかしエラーでない場合
					//	(翌営業日発注分としてご注文を受付いたします。(WECW050030/WECW050020))もある。
					*errorDescr = token;
					errorString = token.substr( token.find_last_of('(') + 1 );
					errorString = errorString.substr( 0, errorString.find_first_of(')') );
				}
				else if (strstr( token.c_str(), "ご注文を受け付けました" ) != NULL) {
					orderAccepted = true;
				}
				else if (token == "同じリクエストが複数回行われた可能性がございます。") {
					return RET_MULTIPLE_REQUEST_ERROR;
				}
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (orderNumber != NULL) 
					*orderNumber = atoi2( token.c_str() );
				status = STATUS_SUCCESS;
			}
			break;
		}
	}

	return orderAccepted ? RET_SUCCESS : GetOrderErrorCode( errorString.c_str() );
}

int CSBIWebClient::ExtractAccountInfo( CBuffer& buffer, CSBIWebClient::ACCOUNTINFO* accountInfo )
{
	CHtmlTokenizer tokenizer;
	CHtmlTokenizer::TOKENTYPE tokenType;
	string token;
	enum { STATUS_SUCCESS = 1000 };
	int status = 0;

	//	返却値のクリア
	accountInfo->asset_valuation = 0;
	accountInfo->buy_strength = 0;
	accountInfo->margin_strength = 0;
	
	if (!tokenizer.OpenBuffer( &buffer ))
		return false;
	
	while (status != STATUS_SUCCESS && tokenizer.GetNextToken( &tokenType, &token ))
	{
		switch (status) {
		case 0:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				if (token == "信用建余力")
					status = 1;
				else if (token == "(3営業日後)")
					status = 2;
				else if (token == "計")	//	資産評価額
					status = 3;
			}
			break;
		case 1:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				accountInfo->margin_strength = atoi2( token.c_str() );
				status = 0;
			}
			break;
		case 2:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				accountInfo->buy_strength = atoi2( token.c_str() );
				status = 0;
			}
			break;
		case 3:
			if (tokenType == CHtmlTokenizer::OTHER_TEXT) {
				accountInfo->asset_valuation = atoi2( token.c_str() );
				status = STATUS_SUCCESS;
			}
			break;
		}
	}
	
	return (status == STATUS_SUCCESS);
}

int CSBIWebClient::GetOrderErrorCode( const char* errorString )
{
	static const struct { char* str; int code;} errorTable[] = {
		"WECEK00210",	RET_OUT_OF_STRENGTH,
		"WECET00330",	RET_OUT_OF_STRENGTH,
		"WPLE009011",	RET_BRAND_NOT_FOUND,
		"WECES00300",	RET_PRICE_RANGE_OVER,
		"WECES01310",	RET_PRICE_RANGE_OVER,
		"WECES00510",	RET_TRADE_RESTRICTED,
		"WECEK00360",	RET_TRADE_RESTRICTED,
		"WECEA00030",	RET_NOT_HAVE_POSITION,
		"WPLE002041",	RET_ORDER_PASSWORD_ERROR,
		"WBLE015003",	RET_ORDER_PASSWORD_ERROR,
		"WPLE002024",	RET_INSTRUCTION_ERROR,
		"WECEG00220",	RET_INSTRUCTION_ERROR,
		"WECEH00270",	RET_INSTRUCTION_ERROR,
		"WECEC00100",	RET_QUANTITY_UNIT_ERROR,
		"WECES00050",	RET_PRICE_UNIT_ERROR,
		"WECEI00200",	RET_IOC_ORDER_OVERTIME,
		"WECEH00060",	RET_INSUFFICIENT_GUARANTEE,
		NULL, 0
	};

	for (int i = 0; errorTable[i].str != NULL; i++) {
		if (strcmp( errorString, errorTable[i].str ) == 0)
			return errorTable[i].code;
	}

	return RET_UNKNOWN_ERROR;
}

int CSBIWebClient::ConvertHttpClientErrorCode( int code )
{
	return (code == CHttpClient::RET_CANNOT_OPEN_FILE) ? RET_TEMP_FILE_ERROR : RET_INTERNET_ERROR;
}

string CSBIWebClient::ToTradingFormCode( int trading )
{
	string result;

	switch (trading) {
	case CStockOrder::BUY:
		result = "0"; break;
	case CStockOrder::SELL:
		result = "1"; break;
	case CStockOrder::MARGIN_BUY:
		result = "2"; break;
	case CStockOrder::MARGIN_SELL:
		result = "3"; break;
	}

	return result;
}

string CSBIWebClient::ToMarketFormCode( int market )
{
	string result;

	switch (market) {
	case CStockOrder::SOR:
		result = "   "; break;
	case CStockOrder::TOKYO:
		result = "TKY"; break;
	case CStockOrder::OSAKA:
		result = "OSK"; break;
	case CStockOrder::JASDAQ:
		result = "OTC"; break;
	case CStockOrder::NAGOYA:
		result = "NGY"; break;
	case CStockOrder::FUKUOKA:
		result = "FKO"; break;
	case CStockOrder::SAPPORO:
		result = "SPR"; break;
	}

	return result;
}

string CSBIWebClient::ToMarginFormCode(int margin)
{
	string result;

	switch (margin) {
	case CStockOrder::SEIDO:
		result = "6"; break;
	case CStockOrder::IPPAN:
		result = "9"; break;
	case CStockOrder::DAY:
		result = "D"; break;
	}

	return result;
}

string CSBIWebClient::ToNariyukiFormCode( int nariyuki_cond )
{
	string result;

	switch (nariyuki_cond) {
	case CStockOrder::NO_COND:
		result = "N"; break;
	case CStockOrder::YORINARI:
		result = "Y"; break;
	case CStockOrder::HIKENARI:
		result = "H"; break;
	case CStockOrder::IOCNARI:
		result = "O"; break;
	}

	return result;
}

string CSBIWebClient::ToSashineFormCode( int sashine_cond )
{
	string result;

	switch (sashine_cond) {
	case CStockOrder::NO_COND:
		result = " "; break;
	case CStockOrder::YORISASHI:
		result = "Z"; break;
	case CStockOrder::HIKESASHI:
		result = "I"; break;
	case CStockOrder::FUNARI:
		result = "F"; break;
	case CStockOrder::IOCSASHI:
		result = "P"; break;
	}

	return result;
}

int CSBIWebClient::ToOrderStatusCode( const char* statusString )
{
	if (strstr( statusString, "取消完了" ) != NULL)
		return CANCEL_COMPLETE;
	if (strstr( statusString, "取消中" ) != NULL)
		return CANCELING;
	if (strstr( statusString, "注文中" ) != NULL)
		return ORDERING;
	if (strstr( statusString, "訂正中" ) != NULL)
		return MODIFYING;
	if (strstr( statusString, "待機中" ) != NULL)
		return WAITING;
	if (strstr( statusString, "完了" ) != NULL)
		return ORDER_COMPLETE;
	if (strstr( statusString, "失効" ) != NULL)
		return EXPIRED;

	return UNKNOWN_STATUS;
}

string CSBIWebClient::IntToStr( int value )
{
	char buffer[20];
	return string( _itoa( value, buffer, 10 ) );
}

string CSBIWebClient::LongToStr( long value )
{
	char buffer[20];
	return string( _ltoa( value, buffer, 10 ) );
}

string CSBIWebClient::DoubleToStr( double value )
{
	char buffer[40];
	sprintf(buffer, "%g", value);
	return string(buffer);
}

int CSBIWebClient::atoi2( const char* src )
{
	string str;
	
	//	左端の数字以外の文字はスキップ
	while (*src != '\0' && !isdigit((unsigned char)*src)) {
		src++;
	}
	//	連続する数値文字を抽出(コンマは除去する)
	while (*src != '\0') {
		if (isdigit((unsigned char)*src)) {
			str += *src;
			src++;
		} else if (*src == ',') {
			src++;
		} else
			break;
	}

	return atoi( str.c_str() );
}

long CSBIWebClient::atol2( const char* src )
{
	string str;
	
	//	左端の数字以外の文字はスキップ
	while (*src != '\0' && !isdigit((unsigned char)*src)) {
		src++;
	}
	//	連続する数値文字を抽出(コンマは除去する)
	while (*src != '\0') {
		if (isdigit((unsigned char)*src)) {
			str += *src;
			src++;
		} else if (*src == ',') {
			src++;
		} else
			break;
	}

	return atol( str.c_str() );
}

double CSBIWebClient::atof2( const char* src )
{
	string str;

	//	左端の数字以外の文字はスキップ
	while (*src != '\0' && !isdigit((unsigned char)*src)) {
		src++;
	}
	//	連続する数値文字を抽出(コンマは除去する)
	while (*src != '\0') {
		if (isdigit((unsigned char)*src) || *src == '.') {
			str += *src;
			src++;
		}
		else if (*src == ',') {
			src++;
		}
		else
			break;
	}

	return atof(str.c_str());
}

string CSBIWebClient::DecodeHrefString( const string& src )
{
	//	【注意！】
	//		現状ではhref文字列の中にある&amp;を&に置換するだけのルーチンです。
	//		本来ならば、&lt;等の特殊文字も変換されることが望ましいです。
	string result;
	int	status = 0;
	string char_name;
	string::const_iterator iter;

	for (iter = src.begin(); iter != src.end(); iter++) 
	{
		switch (status) {
		case 0:
			if (*iter == '&') {
				char_name.clear();
				status = 1;
			} else
				result += *iter;
			break;
		case 1:
			if (isalnum( *iter ))
				char_name += *iter;
			else {
				if (char_name == "amp")
					result += '&';
				status = 0;
			}
			break;
		}
	}

	return result;
}

string CSBIWebClient::MakeURLFromHrefString( const string& baseUrl, const string& href )
{
	return baseUrl + DecodeHrefString( href.substr( href.find_first_of('?') ) );
}

void CSBIWebClient::SetOrderInfoToParams( int mode, const CStockOrder& order, CHttpClient::CCGIParams* params )
{
	//	基本情報を設定
	switch (mode) {
	case MODE_NEW_ORDER:
		params->Add( "trade_kbn", ToTradingFormCode( order.m_trade ) );
		params->Add( "stock_sec_code", IntToStr( order.m_brand_code ) );
		params->Add( "input_market", ToMarketFormCode( order.m_market ) );
		params->Add( "input_quantity", LongToStr( order.m_quantity ) );
		break;
	case MODE_REPAY_ORDER:
	case MODE_MODIFY_ORDER:
		break;
	}

	//	執行条件を設定
	if (order.m_trigger_price == 0)
	{
		//	通常注文(成行または指値)
		if (order.m_sashine_price == 0) {
			//	成行
			params->Set( "in_sasinari_kbn", "N" );	
			params->Set( "nariyuki_condition", ToNariyukiFormCode( order.m_nariyuki_cond ) );
		} else {
			//	指値
			params->Set( "in_sasinari_kbn", " " );
			params->Set( "sasine_condition", ToSashineFormCode( order.m_sashine_cond ) );
			params->Set( "input_price", DoubleToStr( order.m_sashine_price ) );
		}
	}
	else
	{
		//	逆指値注文
		params->Set( "in_sasinari_kbn", "G" );
		params->Set( "input_trigger_price", DoubleToStr( order.m_trigger_price ) );
		params->Set( "input_trigger_zone", (order.m_trigger_zone == order.MORE) ? "0":"1" );

		if (order.m_sashine_price == 0) {
			//	成行
			params->Set( "gsn_sasinari_kbn", "N" );
			params->Set( "gsn_nariyuki_condition", ToNariyukiFormCode( order.m_nariyuki_cond ) );
		} else {
			//	指値
			params->Set( "gsn_sasinari_kbn", " " );
			params->Set( "gsn_sasine_condition", ToSashineFormCode( order.m_sashine_cond ) );
			params->Set( "gsn_input_price", DoubleToStr( order.m_sashine_price ) );
		}
	}

	//	付加情報を設定
	switch (mode) {
	case MODE_NEW_ORDER:
		params->Add( "selected_limit_in", "this_day" );	
		params->Add( "hitokutei_trade_kbn", order.m_azukari );
		params->Add( "payment_limit", ToMarginFormCode( order.m_margin ) );
		params->Add( "agreement", "on" );
		break;
	case MODE_REPAY_ORDER:
		params->Add( "selected_limit_in", "this_day" );	
		break;
	case MODE_MODIFY_ORDER:
		break;
	}

	//	取引パスワードを設定
	params->Add( "trade_pwd", m_OrderPassword );
}

int CSBIWebClient::SetRepayQuantityToParams( long quantity, CHttpClient::CCGIParams* params )
{
	if (quantity <= 0) return RET_INSTRUCTION_ERROR;

	CHttpClient::CCGIParams::const_iterator iter;
	long tmp_max_quantity = 0;

	for (iter = params->begin(); iter != params->end(); iter++)
	{
		if (iter->first == "tmp_cont_position")				//	(この返済期日の)建株数
			tmp_max_quantity = atol2( iter->second.c_str() );
		else if (iter->first == "tmp_unactual_quantity")	//	(この返済期日の)建株数の注文中分
			tmp_max_quantity -= atol2( iter->second.c_str() );
		else if (iter->first == "tmp_open_contract_no") {
			long tmp_quantity = (quantity <= tmp_max_quantity) ? quantity : tmp_max_quantity;
			params->insert( ++iter, pair<string,string>("tmp_quantity", (tmp_quantity > 0) ? LongToStr(tmp_quantity) : "") );
			quantity -= tmp_quantity;
		} else if (iter->first == "stock_sec_code")
			break;
	}

	return (quantity == 0) ? RET_SUCCESS : RET_NOT_HAVE_POSITION;
}
