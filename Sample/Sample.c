//////////////////////////////////////////////////////////////////////////////
// Sample.c :
//		SBI Client DLLの使用例を示したサンプルプログラムです。
//		売買注文を発注するので実行する際は十分注意してください。
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

	puts("ログインユーザー名を入力してください。");
	gets_s( username, 256 );
	puts("ログインパスワードを入力してください。");
	gets_s( password, 256 );
	puts("取引パスワードを入力してください。");
	gets_s( orderPassword, 256 );

	//------------------------------------------------------------------------
	//	APIを初期化する
	//------------------------------------------------------------------------
	if ((rc = SBIInitialize()) == SBI_NOT_AVAILABLE) {
		puts("試用期間を過ぎています。");
		return -1;
	}

	if (rc == 0)
		puts("ライセンス登録済みです。");
	else
		printf("試用期間は残り%d日です。\n", rc);

	//------------------------------------------------------------------------
	//	SBI証券の口座にログインする
	//------------------------------------------------------------------------
	if ((rc = SBILogin( username, password )) < 0) {
		printf("Login error = %d\n", rc);
		return -1;
	}

	puts("ログインに成功しました。");

	//------------------------------------------------------------------------
	//	口座情報を取得する
	//------------------------------------------------------------------------
	if ((rc = SBIGetAccountInfo( &accountInfo )) < 0) {
		printf("GetAccountInfo error = %d\n", rc);
	} else {
		puts("===== 口座情報 =====");
		printf("資産評価額: %ld\n", accountInfo.asset_valuation);
		printf("買付余力(3営業日後): %ld\n", accountInfo.buy_strength);
		printf("信用建余力: %ld\n", accountInfo.margin_strength);
	}

	//------------------------------------------------------------------------
	//	リアルタイム株価を取得する
	//------------------------------------------------------------------------
	if (( rc = SBIGetStockPrice( 7201, &prices )) < 0) {
		printf("GetStockPrice error = %d\n", rc);
	} else {
		printf("日産自動車の株価 現在値:%g 始値:%g 高値:%g 安値:%g\n",
						prices.last, prices.open, prices.high, prices.low);
	}

	//------------------------------------------------------------------------
	//	取引パスワードを設定する
	//------------------------------------------------------------------------
	SBISetOrderPassword( orderPassword );

	//------------------------------------------------------------------------
	//	売買注文1を発注する
	//		[取引] 信用買い
	//		[市場] 優先市場/SOR
	//		[銘柄] トヨタ自動車(7203)
	//		[取引株数] 100株
	//		[執行条件] 指値(6000円)
	//		[預かり口座] 特定
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

	printf("売買注文1の発注に成功しました。(注文番号:%d)\n", orderNumber);

	//------------------------------------------------------------------------
	//	売買注文1の注文状態を取得する
	//------------------------------------------------------------------------
	if ((rc = SBIGetStockOrderStatus( orderNumber, &orderStatus, &lastExecPrice )) < 0) {
		printf("GetStockOrderStatus error = %d\n", rc);
	} else {
		puts("売買注文1の注文状態の取得に成功しました。");
		printf("[注文状態:%d][最終約定価格:%g]\n", orderStatus, lastExecPrice);
	}

	//------------------------------------------------------------------------
	//	売買注文1の執行条件を成行(条件なし)に訂正する
	//------------------------------------------------------------------------
	SBISetOrderNariyuki( order, OD_NOCOND );

	if ((rc = SBIStockOrderModify( orderNumber, order )) < 0) {
		printf("StockOrderModify error = %d\n", rc);
		if (rc == SBI_UNKNOWN_ERROR)
			puts( SBIGetLastOrderErrorDescr() );
		SBIReleaseOrder( order );
		return -1;
	}

	printf("売買注文1の訂正に成功しました。\n");
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	売買注文1を取消す
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("売買注文1の取消しに成功しました。");

	//------------------------------------------------------------------------
	//	売買注文2を発注する
	//		[取引] 信用売り
	//		[市場] 優先市場/SOR
	//		[銘柄] ソフトバンク(9984)
	//		[取引株数] 100株
	//		[執行条件] 指値(7000円引け指し)
	//		[預かり口座] 特定
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

	printf("売買注文2の発注に成功しました。(注文番号:%d)\n", orderNumber);
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	売買注文2を取消す
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("売買注文2の取消しに成功しました。");

	//------------------------------------------------------------------------
	//	売買注文3を発注する
	//		[取引] 現物買い
	//		[市場] 東証
	//		[銘柄] 日本電気(6701)
	//		[取引株数] 1000株
	//		[執行条件] 逆指値(5800円)
	//		[預かり口座] NISA
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

	printf("売買注文3の発注に成功しました。(注文番号:%d)\n", orderNumber);
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	売買注文3を取消す
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("売買注文3の取消しに成功しました。");

	//------------------------------------------------------------------------
	//	売買注文4を発注する
	//		[取引] 現物買い
	//		[市場] 優先市場/SOR
	//		[銘柄] 日産自動車(7201)
	//		[取引株数] 100株
	//		[執行条件] トリガー価格480.7円以上になったら指値(470.2円)で執行
	//		[預かり口座] NISA
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

	printf("売買注文4の発注に成功しました。(注文番号:%d)\n", orderNumber);
	SBIReleaseOrder( order );

	//------------------------------------------------------------------------
	//	売買注文4を取消す
	//------------------------------------------------------------------------
	if ((rc = SBIStockOrderCancel( orderNumber )) < 0) {
		printf("StockOrderCancel error = %d\n", rc);
		return -1;
	}

	puts("売買注文4の取消しに成功しました。");

	//------------------------------------------------------------------------
	//	ログアウトする
	//------------------------------------------------------------------------
	if ((rc = SBILogout()) < 0) {
		printf("Logout error = %d\n", rc);
		return -1;
	}

	puts("ログアウトしました。");

	return 0;
}

