/*****************************************************************************
	SBI Client 株式発注ライブラリ
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
	エラーコード一覧
*****************************************************************************/
#define	SBI_SUCCESS							0		/* 成功 */
#define	SBI_NOT_AVAILABLE					(-1)	/* APIは利用できない(試用期限切れ等) */
#define	SBI_NOT_INITIALIZED					(-2)	/* APIは初期化されていない */
#define	SBI_UNKNOWN_ERROR					(-100)	/* 不明な(未知の)エラー */
#define	SBI_INTERNET_ERROR					(-101)	/* インターネット通信のエラー */
#define	SBI_UNMATCH_USERNAME_OR_PASSWORD	(-102)	/* ユーザー名かパスワードが正しくない */
#define	SBI_OUT_OF_STRENGTH					(-103)	/* 買付余力(または信用建余力)が不足している */
#define	SBI_BRAND_NOT_FOUND					(-104)	/* 指定の銘柄コードはない */
#define	SBI_PRICE_RANGE_OVER				(-105)	/* 値幅範囲を超えている */
#define	SBI_TRADE_RESTRICTED				(-106)	/* この取引は規制されている */
#define	SBI_NOT_HAVE_POSITION				(-107)	/* 預かり残高(または建玉)が足りない */
#define	SBI_INSTRUCTION_ERROR				(-108)	/* 不正な指示である */
#define	SBI_ORDER_NOT_FOUND					(-109)	/* 指定された注文番号の注文はない */
#define	SBI_CANCEL_NOT_ACCEPTED				(-110)	/* 取消は受け付けられない */
#define	SBI_CANCEL_FAILED					(-111)	/* 取消は失敗した */
#define	SBI_PARSE_ERROR						(-112)	/* Webページの解析に失敗した */
#define	SBI_POSITION_NOT_FOUND				(-113)	/* 建玉がない */
#define	SBI_MODIFY_NOT_ACCEPTED				(-114)	/* 訂正は受け付けられない */
#define	SBI_NOT_LOGGEDIN					(-115)	/* ログインしていない */
#define	SBI_ORDER_PASSWORD_ERROR			(-116)	/* 取引パスワードが正しくない */
#define	SBI_QUANTITY_UNIT_ERROR				(-117)	/* 株数単位が正しくない */
#define	SBI_TEMP_FILE_ERROR					(-118)	/* 一時ファイルの処理に失敗した */
#define	SBI_MULTIPLE_REQUEST_ERROR			(-119)	/* 同じリクエストが複数回行われた */
#define	SBI_PRICE_UNIT_ERROR				(-120)	/* 価格単位が正しくない */
#define	SBI_IOC_ORDER_OVERTIME				(-121)	/* IOC注文は時間外のためできない */
#define	SBI_INSUFFICIENT_GUARANTEE			(-122)	/* 最低委託保証金に足りない */

/*****************************************************************************
	取引種別コード
*****************************************************************************/
#define	OD_BUY				0	/* 現物買い */
#define OD_SELL				1	/* 現物売り */
#define OD_MARGIN_BUY		2	/* 信用買い */
#define	OD_MARGIN_SELL		3	/* 信用売り */
#define	OD_REPAY_BUY		4	/* 信用返済買い */
#define	OD_REPAY_SELL		5	/* 信用返済売り */

/*****************************************************************************
	取引市場コード
*****************************************************************************/
#define	OD_SOR				0	/* 優先市場/SOR */
#define OD_TOKYO			1	/* 東証 */
#define OD_OSAKA			2	/* 大証 */
#define	OD_JASDAQ			3	/* JASDAQ */
#define	OD_NAGOYA			4	/* 名証 */
#define	OD_FUKUOKA			5	/* 福証 */
#define	OD_SAPPORO			6	/* 札証 */

/*****************************************************************************
	信用取引区分コード
*****************************************************************************/
#define	OD_SEIDO			0	/* 制度 */
#define OD_IPPAN			1	/* 一般 */
#define OD_DAY				2	/* 日計り */

/*****************************************************************************
	執行条件コード
*****************************************************************************/
#define	OD_NOCOND			0	/* 条件なし */
#define	OD_YORINARI			1	/* 寄り成り */
#define	OD_HIKENARI			2	/* 引け成り */
#define	OD_YORISASHI		3	/* 寄り指し */
#define	OD_HIKESASHI		4	/* 引け指し */
#define	OD_FUNARI			5	/* 不成り */
#define	OD_IOCNARI			6	/* IOC成り */
#define	OD_IOCSASHI			7	/* IOC指し */

/*****************************************************************************
	トリガーゾーンコード
*****************************************************************************/
#define OD_MORE		0	/* 以上 */
#define OD_LESS		1	/* 以下 */

/*****************************************************************************
	注文状態コード
*****************************************************************************/
#define	OS_UNKNOWN	 			0	/* 未知(未サポート)の状態 */
#define	OS_WAITING				1	/* 待機中 */
#define	OS_ORDERING				2	/* 注文中 */
#define	OS_ORDER_COMPLETE		3	/* 完了 */
#define	OS_CANCELING			4	/* 取消中 */
#define	OS_CANCEL_COMPLETE		5	/* 取消完了 */
#define	OS_MODIFYING			6	/* 訂正中 */
#define	OS_EXPIRED				7	/* 失効 */

/*****************************************************************************
	預かり口座コード
*****************************************************************************/
#define	OA_TOKUTEI	 		"0" /* 特定預かり */
#define	OA_GENERAL	 		"1" /* 一般預かり */
#define	OA_NEW_NISA 		"H" /* NISA預かり */
#define	OA_NISA		 		"4" /* 旧NISA預かり */

/*****************************************************************************
	各種型定義
*****************************************************************************/
#pragma	pack(1)

typedef	struct stACCOUNTINFO {
	long	buy_strength;		/* 買付余力 */
	long	margin_strength;	/* 信用建余力 */
	long	asset_valuation;	/* 資産評価額 */
} ACCOUNTINFO;

typedef struct stSTOCKPRICES {
	double	open;		/* 始値 */
	double	high;		/* 高値 */
	double	low;		/* 安値 */
	double	last;		/* 現在値 */
} STOCKPRICES;

typedef void* STOCKORDER;

#pragma	pack()


/*****************************************************************************
	SBI Clientのライブラリを初期化する。

	ライブラリを初期化して各APIを使用可能な状態にします。

	戻り値 成功するとゼロ以上の値を返します。ゼロの場合はライセンス登録済み
	       を表し、正の値の場合は試用期間の残り日数を表します。
	       エラーの場合はSBI_NOT_AVAILABLEを返します。
	参照 エラーコード一覧
*****************************************************************************/
SBICLIENT_API int STDCALL SBIInitialize(void);

/*****************************************************************************
	SBI証券のWebサイトにログインする。

	売買注文の発注・訂正・取消やリアルタイム株価の取得などのAPIを使用する
	前には必ずこのAPIでログインをする必要があります。

	引数 username [in] ユーザー名
	引数 password [in] パスワードい
	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 SBILogout()
	参照 エラーコード一覧
*****************************************************************************/
SBICLIENT_API int STDCALL SBILogin( const char* username, const char* password );

/*****************************************************************************
	SBI証券のWebサイトからログアウトする。

	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 SBILogin()
	参照 エラーコード一覧
*****************************************************************************/
SBICLIENT_API int STDCALL SBILogout(void);

/*****************************************************************************
	口座基本情報を取得する。

	ログインしている口座の買付余力・信用建余力・資産評価額などを取得します。

	引数 accountInfo [out] 口座情報の構造体へのポインタ
	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 エラーコード一覧
*****************************************************************************/
SBICLIENT_API int STDCALL SBIGetAccountInfo( ACCOUNTINFO* accountInfo );

/*****************************************************************************
	リアルタイム株価を取得する。

	指定されたコードの銘柄のリアルタイム株価(4本値)を取得します。
	市場が開く直前などまだ値が付いていない時間帯は4本値ともゼロが返ります。

	引数 code [in] 銘柄コード
	引数 prices [out] リアルタイム株価の構造体へのポインタ
	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 エラーコード一覧
*****************************************************************************/
SBICLIENT_API int STDCALL SBIGetStockPrice( int code, STOCKPRICES* prices );

/*****************************************************************************
	取引パスワードを設定する。

	売買注文の発注・訂正・取消の際に共通に使われる取引パスワードを設定します。

	引数 password [in] 取引パスワードを格納するポインタ
	戻り値 なし。
	参照 SBIStockOrder()
	参照 SBIStockOrderModify()
	参照 SBIStockOrderCancel()
	参照 エラーコード一覧
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderPassword( const char* password );

/*****************************************************************************
	発注エラーの場合にエラー文字列を取得する。

	売買注文の発注・訂正・取消でその他のエラー(SBI_UNKNOWN_ERROR)が返された
	場合に、本APIをコールするとエラーの詳細情報(日本語)を表す文字列が
	取得できます。エラー文字列はシフトJISコードで取得できます。

	戻り値 エラー詳細文字列へのポインタ。
	参照 SBIStockOrder()
	参照 SBIStockOrderModify()
	参照 SBIStockOrderCancel()
	参照 エラーコード一覧
*****************************************************************************/
SBICLIENT_API const char* STDCALL SBIGetLastOrderErrorDescr(void);

/*****************************************************************************
	売買注文を作成する。

	売買注文のオブジェクトを作成します。

	引数 trade [in] 取引の種別
	引数 market [in] 取引市場
	引数 code [in] 銘柄コード
	引数 quantity [in] 取引する株数
	戻り値 成功するとSTOCKORDERオブジェクトを返します。エラーの場合は
	       NULLを返します。
	参照 SBIReleaseOrder()
	参照 取引種別コード
	参照 取引市場コード
*****************************************************************************/
SBICLIENT_API STOCKORDER STDCALL SBICreateOrder( int trade, int market, int code, long quantity );

/*****************************************************************************
	売買注文を解放する。

	売買注文のオブジェクトを解放します。
	売買注文を発注した後はこれを呼び出して注文オブジェクトを解放してください。

	引数 order [in] 解放するSTOCKORDERオブジェクト
	戻り値 なし。
	参照 SBICreateOrder()
*****************************************************************************/
SBICLIENT_API void STDCALL SBIReleaseOrder( STOCKORDER order );

/*****************************************************************************
	注文の執行条件を成行に設定する。

	注文の執行条件を成行に設定します。2次条件として寄り成り・引け成り
	・IOC成り・条件なしのいずかれかを設定します。STOCKORDERオブジェクト
	を作成した直後はデフォルトで成行(条件なし)に設定されています。

	引数 order [in] 設定するSTOCKORDERオブジェクト
	引数 cond [in] 執行条件の2次条件
	戻り値 なし
	参照 執行条件コード
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderNariyuki( STOCKORDER order, int cond );

/*****************************************************************************
	執行条件を指値に設定する。

	注文の執行条件を指値に設定します。2次条件として寄り指し・引け指し
	・不成り・IOC指し・条件なしのいずかれかを設定します。

	引数 order [in] 設定するSTOCKORDERオブジェクト
	引数 price [in] 指値価格
	引数 cond [in] 執行条件の2次条件
	戻り値 なし
	参照 執行条件コード
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderSashine( STOCKORDER order, double price, int cond );

/*****************************************************************************
	執行条件を逆指値に設定する。

	注文の執行条件を逆指値に設定します。株価がこれで設定したトリガー
	価格に達すると、成行(条件なし)で注文を発注します。成行(条件なし)
	以外の注文をしたい場合はSBISetOrderTrigger関数を使ってください。

	引数 order [in] 設定するSTOCKORDERオブジェクト
	引数 price [in] トリガー価格
	戻り値 なし
	参照 SBISetOrderTrigger()
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderGSashine( STOCKORDER order, double price );

/*****************************************************************************
	執行条件にトリガー情報を設定する。

	注文の執行条件にトリガーを設定し、そのトリガー価格に達した場合に
	初めて所定の売買注文(成行または指値)を発注するように設定します。
	通常は逆指値注文を実現するためにこの機能が使われます。このトリガー
	設定を行う場合は、SBISetOrderNariyuki関数またはSBISetOrderSashine関数と
	セットで使用してください。

	引数 order [in] 設定するSTOCKORDERオブジェクト
	引数 price [in] トリガー価格
	引数 zone [in] トリガーゾーン(トリガー価格以上または以下)
	戻り値 なし。
	参照 SBISetOrderNariyuki()
	参照 SBISetOrderSashine()
	参照 トリガーゾーンコード
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderTrigger( STOCKORDER order, double price, int zone );

/*****************************************************************************
	信用取引区分を設定する。

	注文の信用取引区分(制度信用取引、一般信用取引または日計り)を設定します。
	取引区分が信用買いまたは信用売りの場合のみ設定が有効です。
	STOCKORDERオブジェクトを作成した直後は、デフォルトで
	制度信用取引に設定されています。

	引数 order [in] 設定するSTOCKORDERオブジェクト
	引数 margin [in] 信用取引区分コード
	戻り値 なし
	参照 信用取引区分コード
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderMargin( STOCKORDER order, int margin );

/*****************************************************************************
	口座の預かり区分を設定する。

	注文の口座の預かり区分(特定預かり・一般預かり・NISA預かり)を設定します。
	取引区分が現物買い・現物売り・信用買いまたは信用売りの場合のみ
	設定が有効です。STOCKORDERオブジェクトを作成した直後は、デフォルト
	でTRUE(特定預かり)に設定されています。

	引数 order [in] 設定するSTOCKORDERオブジェクト
	引数 azukari [in] 預かり口座を表す文字列を指定します
	戻り値 なし
*****************************************************************************/
SBICLIENT_API void STDCALL SBISetOrderAzukari( STOCKORDER order, const char* azukari );

/*****************************************************************************
	売買注文を発注する。

	現物株または信用取引における売買注文を発注します。
	成功すると受け付けた注文の注文番号が返されます。
	戻り値がSBI_UNKNOWN_ERRORの場合、GetLastOrderErrorDescr関数をコール
	すると詳細エラー情報が取得できる場合があります。

	引数 order [in] 注文オブジェクト
	引数 orderNumber [out] 注文番号を格納するポインタ
	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 エラーコード一覧
	参照 SBIGetLastOrderErrorDescr()
*****************************************************************************/
SBICLIENT_API int STDCALL SBIStockOrder( STOCKORDER order, int* orderNumber );

/*****************************************************************************
	売買注文を訂正する。

	売買注文を訂正します。元の注文の内容によって訂正できる内容は変わって
	くるのでこのAPIの使用には注意が必要です。例えば元の注文が通常の指値
	注文だった場合は、指値価格を訂正するか、成行注文に訂正するか、しか
	変更できません。しかしそこで仮に逆指値注文に訂正した場合でも、APIの
	戻り値としては成功を返してしまう(実際は成功しない)場合があるので十分
	注意してください。
	戻り値がSBI_UNKNOWN_ERRORの場合、SBIGetLastOrderErrorDescr関数をコール
	すると詳細エラー情報が取得できる場合があります。

	引数 orderNumber [in] 訂正元の注文の注文番号
	引数 order [in] 訂正後の注文オブジェクト
	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 エラーコード一覧
	参照 SBIGetLastOrderErrorDescr()
*****************************************************************************/
SBICLIENT_API int STDCALL SBIStockOrderModify( int orderNumber, STOCKORDER order );

/*****************************************************************************
	売買注文を取消す。

	売買注文を取消します。
	戻り値がSBI_UNKNOWN_ERRORの場合、SBIGetLastOrderErrorDescr関数をコール
	すると詳細エラー情報が取得できる場合があります。

	引数 orderNumber [in] 取消す注文の注文番号
	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 エラーコード一覧
	参照 SBIGetLastOrderErrorDescr()
*****************************************************************************/
SBICLIENT_API int STDCALL SBIStockOrderCancel( int orderNumber );

/*****************************************************************************
	注文の状態を取得する。

	売買注文の注文状態(待機中/注文中/完了/取消完了など)を取得します。
	注文が約定済み(一部または全約定)の場合は最後に約定した価格も同時に
	取得できます。

	引数 orderNumber [in] 取得する注文の注文番号
	引数 orderStatus [out] 注文状態を格納するポインタ
	引数 lastExecPrice [out] 最後の約定の約定価格を格納するポインタ
	                         不要な場合はNULLを指定できます。
	戻り値 成功するとゼロを返します。エラーの場合は負の値を返します。
	参照 エラーコード一覧
	参照 注文状態コード
*****************************************************************************/
SBICLIENT_API int STDCALL SBIGetStockOrderStatus( int orderNumber, int* orderStatus, double* lastExecPrice );


#ifdef	__cplusplus
}
#endif

#endif
