using System;
using System.Runtime.InteropServices;

/*****************************************************************************
	エラーコード一覧
*****************************************************************************/
enum SBICode : int {
	SUCCESS							= 0,		// 成功
	NOT_AVAILABLE					= (-1),		// APIは利用できない(試用期限切れ等)
	NOT_INITIALIZED					= (-2),		// APIは初期化されていない
	UNKNOWN_ERROR					= (-100),	// 不明な(未知の)エラー
	INTERNET_ERROR					= (-101),	// インターネット通信のエラー
	UNMATCH_USERNAME_OR_PASSWORD	= (-102),	// ユーザー名かパスワードが正しくない
	OUT_OF_STRENGTH					= (-103),	// 買付余力(または信用建余力)が不足している
	BRAND_NOT_FOUND					= (-104),	// 指定の銘柄コードはない
	PRICE_RANGE_OVER				= (-105),	// 値幅範囲を超えている
	TRADE_RESTRICTED				= (-106),	// この取引は規制されている
	NOT_HAVE_POSITION				= (-107),	// 預かり残高(または建玉)が足りない
	INSTRUCTION_ERROR				= (-108),	// 不正な指示である
	ORDER_NOT_FOUND					= (-109),	// 指定された注文番号の注文はない
	CANCEL_NOT_ACCEPTED				= (-110),	// 取消は受け付けられない
	CANCEL_FAILED					= (-111),	// 取消は失敗した
	PARSE_ERROR						= (-112),	// Webページの解析に失敗した
	POSITION_NOT_FOUND				= (-113),	// 建玉がない
	MODIFY_NOT_ACCEPTED				= (-114),	// 訂正は受け付けられない
	NOT_LOGGEDIN					= (-115),	// ログインしていない
	ORDER_PASSWORD_ERROR			= (-116),	// 取引パスワードが正しくない
	QUANTITY_UNIT_ERROR				= (-117),	// 株数単位が正しくない
	TEMP_FILE_ERROR					= (-118),	// 一時ファイルの処理に失敗した
	MULTIPLE_REQUEST_ERROR			= (-119),	// 同じリクエストが複数回行われた
	PRICE_UNIT_ERROR				= (-120),	// 価格単位が正しくない
	IOC_ORDER_OVERTIME				= (-121),	// IOC注文は時間外のためできない
	INSUFFICIENT_GUARANTEE			= (-122),	// 最低委託保証金に足りない
}

/*****************************************************************************
	取引種別コード
*****************************************************************************/
enum Trade : int {
	BUY			= 0,	// 現物買い
	SELL		= 1,	// 現物売り
	MARGIN_BUY	= 2,	// 信用買い
	MARGIN_SELL	= 3,	// 信用売り
	REPAY_BUY	= 4,	// 信用返済買い
	REPAY_SELL	= 5,	// 信用返済売り
}

/*****************************************************************************
	取引市場コード
*****************************************************************************/
enum Market : int {
	SOR			= 0,	// 優先市場/SOR
	TOKYO		= 1,	// 東証
	OSAKA		= 2,	// 大証
	JASDAQ		= 3,	// JASDAQ
	NAGOYA		= 4,	// 名証
	FUKUOKA		= 5,	// 福証
	SAPPORO		= 6,	// 札証
}

/*****************************************************************************
	信用取引区分コード
*****************************************************************************/
enum Margin : int
{
    SEIDO   = 0,    // 制度
    IPPAN   = 1,    // 一般
    DAY     = 2,    // 日計り
}

/*****************************************************************************
	執行条件コード
*****************************************************************************/
enum Cond : int {
	NOCOND		= 0,	// 条件なし
	YORINARI	= 1,	// 寄り成り
	HIKENARI	= 2,	// 引け成り
	YORISASHI	= 3,	// 寄り指し
	HIKESASHI	= 4,	// 引け指し
	FUNARI		= 5,	// 不成り
	IOCNARI		= 6,	// IOC成り
	IOCSASHI	= 7,	// IOC指し
}

/*****************************************************************************
	トリガーゾーンコード
*****************************************************************************/
enum TriggerZone : int {
	MORE		= 0,	// 以上
	LESS		= 1,	// 以下
}

/*****************************************************************************
	注文状態コード
*****************************************************************************/
enum Status : int {
	UNKNOWN	 		= 0,	// 未知(未サポート)の状態
	WAITING			= 1,	// 待機中
	ORDERING		= 2,	// 注文中
	ORDER_COMPLETE	= 3,	// 完了
	CANCELING		= 4,	// 取消中
	CANCEL_COMPLETE	= 5,	// 取消完了
	MODIFYING		= 6,	// 訂正中
	EXPIRED			= 7,	// 失効
}

/*****************************************************************************
	構造体
*****************************************************************************/
[StructLayout(LayoutKind.Sequential)]
struct ACCOUNTINFO
{
    public int buy_strength;	// 買付余力
    public int margin_strength;	// 信用建余力
    public int asset_valuation;	// 資産評価額
}

[StructLayout(LayoutKind.Sequential)]
struct STOCKPRICES
{
    public double open;   //  始値
    public double high;   //  高値
    public double low;    //  安値
    public double last;   //  現在値
}


/*****************************************************************************
	サンプルプログラム
*****************************************************************************/

class Sample
{
    /*****************************************************************************
        SBI関数
    *****************************************************************************/
    [DllImport("SBIClient.dll")]
    static extern SBICode SBIInitialize();
    [DllImport("SBIClient.dll")]
    static extern SBICode SBILogin(string username, string password);
    [DllImport("SBIClient.dll")]
    static extern SBICode SBILogout();
    [DllImport("SBIClient.dll")]
    static extern SBICode SBIGetAccountInfo(out ACCOUNTINFO accountInfo);
    [DllImport("SBIClient.dll")]
    static extern SBICode SBIGetStockPrice(int code, out STOCKPRICES prices);
    [DllImport("SBIClient.dll")]
    static extern IntPtr SBICreateOrder(Trade trade, Market market, int code, int quantity);
    [DllImport("SBIClient.dll")]
    static extern SBICode SBIStockOrder(IntPtr order, out int orderNumber);
    [DllImport("SBIClient.dll")]
    static extern SBICode SBIStockOrderCancel(int orderNumber);
    [DllImport("SBIClient.dll")]
    static extern SBICode SBIStockOrderModify(int orderNumber, IntPtr order);
    [DllImport("SBIClient.dll")]
    static extern SBICode SBIGetStockOrderStatus(int orderNumber, out Status status, out double execPrice);
    [DllImport("SBIClient.dll")]
    static extern void SBIReleaseOrder(IntPtr order);
    [DllImport("SBIClient.dll")]
    static extern void SBISetOrderPassword(string password);
    [DllImport("SBIClient.dll")]
    static extern void SBISetOrderNariyuki(IntPtr order, Cond cond);
    [DllImport("SBIClient.dll")]
    static extern void SBISetOrderSashine(IntPtr order, double price, Cond cond);
    [DllImport("SBIClient.dll")]
    static extern void SBISetOrderGSashine(IntPtr order, double price);
    [DllImport("SBIClient.dll")]
    static extern void SBISetOrderTrigger(IntPtr order, double price, TriggerZone zone);
    [DllImport("SBIClient.dll")]
    static extern void SBISetOrderMargin(IntPtr order, int margin);
    [DllImport("SBIClient.dll")]
    static extern void SBISetOrderAzukari(IntPtr order, string azukari);
    [DllImport("SBIClient.dll")]
    [return: MarshalAs(UnmanagedType.LPStr)]
    static extern string SBIGetLastOrderErrorDescr();

    /*****************************************************************************
        預かり口座コード
    *****************************************************************************/
    public const string OA_GENERAL = "0";   //  一般預かり
    //public const string OA_GENERAL = "-";   //  一般預かり(NISAあり)
    public const string OA_TOKUTEI = "0";   //  特定預かり
    public const string OA_NISA = "4";      //  NISA預かり

    static void Main(string[] args)
    {
        SBICode rc;
        IntPtr order;
        int orderNumber;
        ACCOUNTINFO accountInfo;
        STOCKPRICES prices;
        string username, password, orderPassword;
        Status orderStatus;
        double lastExecPrice;

        Console.WriteLine("ログインユーザー名を入力してください。");
        username = Console.ReadLine();
        Console.WriteLine("ログインパスワードを入力してください。");
        password = Console.ReadLine();
        Console.WriteLine("取引パスワードを入力してください。");
        orderPassword = Console.ReadLine();

    	//------------------------------------------------------------------------
	    //	APIを初期化する
	    //------------------------------------------------------------------------
	    if ((rc = SBIInitialize()) == SBICode.NOT_AVAILABLE)
        {
		    Console.WriteLine("試用期間を過ぎています。");
		    return;
	    }

	    if (rc == SBICode.SUCCESS)
            Console.WriteLine("ライセンス登録済みです。");
	    else
            Console.WriteLine("試用期間は残り{0:d}日です。", rc);

        //------------------------------------------------------------------------
        //	SBI証券の口座にログインする
        //------------------------------------------------------------------------
        if ((rc = SBILogin(username, password)) < 0)
        {
            Console.WriteLine("Login error = {0:d}", rc);
            return;
        }

        Console.WriteLine("ログインに成功しました。");

        //------------------------------------------------------------------------
        //	口座情報を取得する
        //------------------------------------------------------------------------
        if ((rc = SBIGetAccountInfo(out accountInfo)) < 0)
            Console.WriteLine("GetAccountInfo error = {0:d}", rc);
        else
        {
            Console.WriteLine("資産評価額: {0:d}", accountInfo.asset_valuation);
            Console.WriteLine("買付余力(3営業日後): {0:d}", accountInfo.buy_strength);
            Console.WriteLine("信用建余力: {0:d}", accountInfo.margin_strength);
        }

        //------------------------------------------------------------------------
        //	リアルタイム株価を取得する
        //------------------------------------------------------------------------
        if ((rc = SBIGetStockPrice(7201, out prices)) < 0)
            Console.WriteLine("GetStockPrice error = {0:d}", rc);
        else
            Console.WriteLine("日産自動車の株価 現在値:{0:g} 始値:{1:g} 高値:{2:g} 安値:{3:g}", prices.last, prices.open, prices.high, prices.low);

        //------------------------------------------------------------------------
        //	取引パスワードを設定する
        //------------------------------------------------------------------------
        SBISetOrderPassword(orderPassword);

        //------------------------------------------------------------------------
        //	売買注文1を発注する
        //		[取引] 信用買い
        //		[市場] 優先市場/SOR
        //		[銘柄] トヨタ自動車(7203)
        //		[取引株数] 100株
        //		[執行条件] 指値(6000円)
        //		[預かり口座] 特定
        //------------------------------------------------------------------------
        order = SBICreateOrder(Trade.MARGIN_BUY, Market.SOR, 7203, 100);
        SBISetOrderSashine(order, 6000, Cond.NOCOND);
        SBISetOrderAzukari(order, OA_TOKUTEI);

        if ((rc = SBIStockOrder(order, out orderNumber )) < 0)
        {
            Console.WriteLine("StockOrder error = {0:d}", rc);
            if (rc == SBICode.UNKNOWN_ERROR)
                Console.WriteLine(SBIGetLastOrderErrorDescr());
            SBIReleaseOrder(order);
            return;
        }

        Console.WriteLine("売買注文1の発注に成功しました。(注文番号:{0:d})", orderNumber);

        //------------------------------------------------------------------------
        //	売買注文1の注文状態を取得する
        //------------------------------------------------------------------------
        if ((rc = SBIGetStockOrderStatus(orderNumber, out orderStatus, out lastExecPrice)) < 0)
            Console.WriteLine("GetStockOrderStatus error = {0:d}", rc);
        else
        {
            Console.WriteLine("売買注文1の注文状態の取得に成功しました。");
            Console.WriteLine("[注文状態:{0:d}][最終約定価格:{1:g}]", orderStatus, lastExecPrice);
        }

        //------------------------------------------------------------------------
        //	売買注文1の執行条件を成行(条件なし)に訂正する
        //------------------------------------------------------------------------
        SBISetOrderNariyuki(order, Cond.NOCOND);

        if ((rc = SBIStockOrderModify(orderNumber, order)) < 0)
        {
            Console.WriteLine("StockOrderModify error = {0:d}", rc);
            if (rc == SBICode.UNKNOWN_ERROR)
                Console.WriteLine(SBIGetLastOrderErrorDescr());
            SBIReleaseOrder(order);
            return;
        }

        Console.WriteLine("売買注文1の訂正に成功しました。");
        SBIReleaseOrder(order);

        //------------------------------------------------------------------------
        //	売買注文1を取消す
        //------------------------------------------------------------------------
        if ((rc = SBIStockOrderCancel(orderNumber)) < 0)
        {
            Console.WriteLine("StockOrderCancel error = {0:d}", rc);
            return;
        }

        Console.WriteLine("売買注文1の取消しに成功しました。");

        //------------------------------------------------------------------------
        //	売買注文2を発注する
        //		[取引] 信用売り
        //		[市場] 優先市場/SOR
        //		[銘柄] ソフトバンク(9984)
        //		[取引株数] 100株
        //		[執行条件] 指値(7000円引け指し)
        //		[預かり口座] 特定
        //------------------------------------------------------------------------
        order = SBICreateOrder(Trade.MARGIN_SELL, Market.SOR, 9984, 100);
        SBISetOrderSashine(order, 7000, Cond.HIKESASHI);
        SBISetOrderAzukari(order, OA_TOKUTEI);

        if ((rc = SBIStockOrder(order, out orderNumber)) < 0)
        {
            Console.WriteLine("StockOrder error = {0:d}", rc);
            if (rc == SBICode.UNKNOWN_ERROR)
                Console.WriteLine(SBIGetLastOrderErrorDescr());
            SBIReleaseOrder(order);
            return;
        }

        Console.WriteLine("売買注文2の発注に成功しました。(注文番号:{0:d})", orderNumber);
        SBIReleaseOrder(order);

        //------------------------------------------------------------------------
        //	売買注文2を取消す
        //------------------------------------------------------------------------
        if ((rc = SBIStockOrderCancel(orderNumber)) < 0)
        {
            Console.WriteLine("StockOrderCancel error = {0:d}", rc);
            return;
        }

        Console.WriteLine("売買注文2の取消しに成功しました。");
        
        //------------------------------------------------------------------------
        //	売買注文3を発注する
        //		[取引] 現物買い
        //		[市場] 東証
        //		[銘柄] 日本電気(6701)
        //		[取引株数] 1000株
        //		[執行条件] 逆指値(5800円)
        //		[預かり口座] NISA
        //------------------------------------------------------------------------
        order = SBICreateOrder(Trade.BUY, Market.TOKYO, 6701, 1000);
        SBISetOrderGSashine(order, 5800);
        SBISetOrderAzukari(order, OA_NISA);

        if ((rc = SBIStockOrder(order, out orderNumber)) < 0)
        {
            Console.WriteLine("StockOrder error = {0:d}", rc);
            if (rc == SBICode.UNKNOWN_ERROR)
                Console.WriteLine(SBIGetLastOrderErrorDescr());
            SBIReleaseOrder(order);
            return;
        }

        Console.WriteLine("売買注文3の発注に成功しました。(注文番号:{0:d})", orderNumber);
        SBIReleaseOrder(order);

        //------------------------------------------------------------------------
        //	売買注文3を取消す
        //------------------------------------------------------------------------
        if ((rc = SBIStockOrderCancel(orderNumber)) < 0)
        {
            Console.WriteLine("StockOrderCancel error = {0:d}", rc);
            return;
        }

        Console.WriteLine("売買注文3の取消しに成功しました。");
        
        //------------------------------------------------------------------------
        //	売買注文4を発注する
        //		[取引] 現物買い
        //		[市場] 優先市場/SOR
        //		[銘柄] 日産自動車(7201)
        //		[取引株数] 100株
        //		[執行条件] トリガー価格480.7円以上になったら指値(470.2円)で執行
        //		[預かり口座] NISA
        //------------------------------------------------------------------------
        order = SBICreateOrder(Trade.BUY, Market.SOR, 7201, 100);
        SBISetOrderSashine(order, 480.7, Cond.NOCOND);
        SBISetOrderTrigger(order, 470.2, TriggerZone.MORE);
        SBISetOrderAzukari(order, OA_NISA);

        if ((rc = SBIStockOrder(order, out orderNumber)) < 0)
        {
            Console.WriteLine("StockOrder error = {0:d}", rc);
            if (rc == SBICode.UNKNOWN_ERROR)
                Console.WriteLine(SBIGetLastOrderErrorDescr());
            SBIReleaseOrder(order);
            return;
        }

        Console.WriteLine("売買注文4の発注に成功しました。(注文番号:{0:d})", orderNumber);
        SBIReleaseOrder(order);

        //------------------------------------------------------------------------
        //	売買注文4を取消す
        //------------------------------------------------------------------------
        if ((rc = SBIStockOrderCancel(orderNumber)) < 0)
        {
            Console.WriteLine("StockOrderCancel error = {0:d}", rc);
            return;
        }

        Console.WriteLine("売買注文4の取消しに成功しました。");

        //------------------------------------------------------------------------
        //	ログアウトする
        //------------------------------------------------------------------------
        if ((rc = SBILogout()) < 0)
        {
            Console.WriteLine("Logout error = {0:d}", rc);
            return;
        }

        Console.WriteLine("ログアウトしました。");
    }
}
