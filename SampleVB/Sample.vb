Module Sample

    '*****************************************************************************
    '   エラーコード一覧
    '*****************************************************************************
    Public Enum SBICode
        SUCCESS = 0                             ' 成功
        NOT_AVAILABLE = (-1)                    ' APIは利用できない(試用期限切れ等)
        NOT_INITIALIZED = (-2)                  ' APIは初期化されていない
        UNKNOWN_ERROR = (-100)                  ' 不明な(未知の)エラー
        INTERNET_ERROR = (-101)                 ' インターネット通信のエラー
        UNMATCH_USERNAME_OR_PASSWORD = (-102)   ' ユーザー名かパスワードが正しくない
        OUT_OF_STRENGTH = (-103)                ' 買付余力(または信用建余力)が不足している
        BRAND_NOT_FOUND = (-104)                ' 指定の銘柄コードはない
        PRICE_RANGE_OVER = (-105)               ' 値幅範囲を超えている
        TRADE_RESTRICTED = (-106)               ' この取引は規制されている
        NOT_HAVE_POSITION = (-107)              ' 預かり残高(または建玉)が足りない
        INSTRUCTION_ERROR = (-108)              ' 不正な指示である
        ORDER_NOT_FOUND = (-109)                ' 指定された注文番号の注文はない
        CANCEL_NOT_ACCEPTED = (-110)            ' 取消は受け付けられない
        CANCEL_FAILED = (-111)                  ' 取消は失敗した
        PARSE_ERROR = (-112)                    ' Webページの解析に失敗した
        POSITION_NOT_FOUND = (-113)             ' 建玉がない
        MODIFY_NOT_ACCEPTED = (-114)            ' 訂正は受け付けられない
        NOT_LOGGEDIN = (-115)                   ' ログインしていない
        ORDER_PASSWORD_ERROR = (-116)           ' 取引パスワードが正しくない
        QUANTITY_UNIT_ERROR = (-117)            ' 株数単位が正しくない
        TEMP_FILE_ERROR = (-118)                ' 一時ファイルの処理に失敗した
        MULTIPLE_REQUEST_ERROR = (-119)         ' 同じリクエストが複数回行われた
        PRICE_UNIT_ERROR = (-120)               ' 価格単位が正しくない
        IOC_ORDER_OVERTIME = (-121)             ' IOC注文は時間外のためできない
        INSUFFICIENT_GUARANTEE = (-122)         ' 最低委託保証金に足りない
    End Enum

    '****************************************************************************
    '   取引種別コード
    '****************************************************************************
    Public Enum Trade
        BUY = 0         ' 現物買い
        SELL = 1        ' 現物売り
        MARGIN_BUY = 2  ' 信用買い
        MARGIN_SELL = 3 ' 信用売り
        REPAY_BUY = 4   ' 信用返済買い
        REPAY_SELL = 5  ' 信用返済売り
    End Enum

    '****************************************************************************
    '   取引市場コード
    '****************************************************************************
    Public Enum Market
        SOR = 0     ' 優先市場/SOR
        TOKYO = 1   ' 東証
        OSAKA = 2   ' 大証
        JASDAQ = 3  ' JASDAQ
        NAGOYA = 4  ' 名証
        FUKUOKA = 5 ' 福証
        SAPPORO = 6 ' 札証
    End Enum

    '****************************************************************************
    '   信用取引区分コード
    '****************************************************************************
    Public Enum Margin
        SEIDO = 0  ' 制度
        IPPAN = 1  ' 一般
        DAY = 2    ' 日計り
    End Enum

    '****************************************************************************
    '   執行条件コード
    '****************************************************************************
    Public Enum Cond
        NOCOND = 0      ' 条件なし
        YORINARI = 1    ' 寄り成り
        HIKENARI = 2    ' 引け成り
        YORISASHI = 3   ' 寄り指し
        HIKESASHI = 4   ' 引け指し
        FUNARI = 5      ' 不成り
        IOCNARI = 6     ' IOC成り
        IOCSASHI = 7    ' IOC指し
    End Enum

    '****************************************************************************
    '   トリガーゾーンコード
    '****************************************************************************
    Public Enum TriggerZone
        MORE = 0 ' 以上
        LESS = 1 ' 以下
    End Enum

    '****************************************************************************
    '   注文状態コード
    '****************************************************************************
    Public Enum Status
        UNKNOWN = 0         ' 未知(未サポート)の状態
        WAITING = 1         ' 待機中
        ORDERING = 2        ' 注文中
        ORDER_COMPLETE = 3  ' 完了
        CANCELING = 4       ' 取消中
        CANCEL_COMPLETE = 5 ' 取消完了
        MODIFYING = 6       ' 訂正中
        EXPIRED = 7         ' 失効
    End Enum

    '****************************************************************************
    '   預かり口座コード
    '****************************************************************************
    Public Const OA_TOKUTEI = "0"   ' 特定預かり
    Public Const OA_GENERAL = "1"   ' 一般預かり
    Public Const OA_NEW_NISA = "H"  ' NISA預かり
    Public Const OA_NISA = "4"      ' 旧NISA預かり

    '****************************************************************************
    '   構造体
    '****************************************************************************
    Public Structure ACCOUNTINFO
        Public buy_strength As Integer     ' 買付余力
        Public margin_strength As Integer  ' 信用建余力
        Public asset_valuation As Integer  ' 資産評価額
    End Structure

    Public Structure STOCKPRICES
        Public open As Double    ' 始値
        Public high As Double    ' 高値
        Public low As Double     ' 安値
        Public last As Double    ' 終値
        Public volume As Double  ' 出来高
    End Structure

    '****************************************************************************
    '   SBI関数
    '****************************************************************************
    Public Declare Function SBIInitialize Lib "SBIClient.dll" () As SBICode
    Public Declare Function SBILogin Lib "SBIClient.dll" (ByVal username As String, ByVal password As String) As SBICode
    Public Declare Function SBILogout Lib "SBIClient.dll" () As SBICode
    Public Declare Function SBIGetAccountInfo Lib "SBIClient.dll" (ByRef accountInfo As ACCOUNTINFO) As SBICode
    Public Declare Function SBIGetStockPrice Lib "SBIClient.dll" (ByVal code As String, ByRef prices As STOCKPRICES) As SBICode
    Public Declare Function SBICreateOrder Lib "SBIClient.dll" (ByVal trade As Trade, ByVal market As Market, ByVal code As String, ByVal quantity As Long) As IntPtr
    Public Declare Function SBIStockOrder Lib "SBIClient.dll" (ByVal order As IntPtr, ByRef orderNumber As Integer) As SBICode
    Public Declare Function SBIStockOrderCancel Lib "SBIClient.dll" (ByVal orderNumber As Integer) As SBICode
    Public Declare Function SBIGetStockOrderStatus Lib "SBIClient.dll" (ByVal orderNumber As Integer, ByRef status As Status, ByRef execPrice As Double) As SBICode
    Public Declare Function SBIStockOrderModify Lib "SBIClient.dll" (ByVal orderNumber As Integer, ByRef order As IntPtr) As SBICode
    Public Declare Function SBIGetLastOrderErrorDescr Lib "SBIClient.dll" () As String
    Public Declare Sub SBIReleaseOrder Lib "SBIClient.dll" (ByVal order As IntPtr)
    Public Declare Sub SBISetOrderPassword Lib "SBIClient.dll" (ByVal password As String)
    Public Declare Sub SBISetOrderNariyuki Lib "SBIClient.dll" (ByVal order As IntPtr, ByVal cond As Cond)
    Public Declare Sub SBISetOrderSashine Lib "SBIClient.dll" (ByVal order As IntPtr, ByVal price As Double, ByVal cond As Cond)
    Public Declare Sub SBISetOrderGSashine Lib "SBIClient.dll" (ByVal order As IntPtr, ByVal price As Double)
    Public Declare Sub SBISetOrderTrigger Lib "SBIClient.dll" (ByVal order As IntPtr, ByVal price As Double, ByVal zone As TriggerZone)
    Public Declare Sub SBISetOrderMargin Lib "SBIClient.dll" (ByVal order As IntPtr, ByVal margin As Integer)
    Public Declare Sub SBISetOrderAzukari Lib "SBIClient.dll" (ByVal order As IntPtr, ByVal azukari As String)

    Sub Main()

        Dim rc As SBICode
        Dim order As IntPtr
        Dim orderNumber As Integer
        Dim accountInfo As ACCOUNTINFO
        Dim prices As STOCKPRICES
        Dim orderStatus As Status
        Dim lastExecPrice As Double
        Dim username As String
        Dim password As String
        Dim orderPassword As String

        Console.WriteLine("ログインユーザー名を入力してください。")
        username = Console.ReadLine()
        Console.WriteLine("ログインパスワードを入力してください。")
        password = Console.ReadLine()
        Console.WriteLine("取引パスワードを入力してください。")
        orderPassword = Console.ReadLine()

        '------------------------------------------------------------------------
        '	APIを初期化する
        '------------------------------------------------------------------------
        rc = SBIInitialize()
        If (rc = SBICode.NOT_AVAILABLE) Then
            Console.WriteLine("試用期間を過ぎています。")
            Return
        End If

        If (rc = SBICode.SUCCESS) Then
            Console.WriteLine("ライセンス登録済みです。")
        Else
            Console.WriteLine("試用期間は残り{0:d}日です。", rc)
        End If

        '------------------------------------------------------------------------
        '	SBI証券の口座にログインする
        '------------------------------------------------------------------------
        rc = SBILogin(username, password)
        If (rc < 0) Then
            Console.WriteLine("Login error = {0:d}", rc)
            Return
        End If

        Console.WriteLine("ログインに成功しました。")

        '------------------------------------------------------------------------
        '	口座情報を取得する
        '------------------------------------------------------------------------
        rc = SBIGetAccountInfo(accountInfo)
        If (rc < 0) Then
            Console.WriteLine("GetAccountInfo error = {0:d}", rc)
        Else
            Console.WriteLine("資産評価額: {0:d}", accountInfo.asset_valuation)
            Console.WriteLine("買付余力(3営業日後): {0:d}", accountInfo.buy_strength)
            Console.WriteLine("信用建余力: {0:d}", accountInfo.margin_strength)
        End If

        '------------------------------------------------------------------------
        '	リアルタイム株価を取得する
        '------------------------------------------------------------------------
        rc = SBIGetStockPrice("7201", prices)
        If (rc < 0) Then
            Console.WriteLine("GetStockPrice error = {0:d}", rc)
        Else
            Console.WriteLine("日産自動車の株価 現在値:{0:g} 始値:{1:g} 高値:{2:g} 安値:{3:g} 出来高:{4}", prices.last, prices.open, prices.high, prices.low, prices.volume)
        End If

        '------------------------------------------------------------------------
        '	取引パスワードを設定する
        '------------------------------------------------------------------------
        SBISetOrderPassword(orderPassword)

        '------------------------------------------------------------------------
        '	売買注文1を発注する
        '		[取引] 信用買い
        '		[市場] 優先市場/SOR
        '		[銘柄] トヨタ自動車(7203)
        '		[取引株数] 100株
        '		[執行条件] 指値(5800円)
        '       [預かり口座] 特定
        '------------------------------------------------------------------------
        order = SBICreateOrder(Trade.MARGIN_BUY, Market.SOR, "7203", 100)
        SBISetOrderSashine(order, 5800, Cond.NOCOND)
        SBISetOrderAzukari(order, OA_TOKUTEI)

        rc = SBIStockOrder(order, orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrder error = {0:d}", rc)
            If (rc = SBICode.UNKNOWN_ERROR) Then
                Console.WriteLine(SBIGetLastOrderErrorDescr())
            End If
            SBIReleaseOrder(order)
            Return
        End If

        Console.WriteLine("売買注文1の発注に成功しました。(注文番号:{0:d})", orderNumber)

        '------------------------------------------------------------------------
        '	売買注文1の注文状態を取得する
        '------------------------------------------------------------------------
        rc = SBIGetStockOrderStatus(orderNumber, orderStatus, lastExecPrice)
        If (rc < 0) Then
            Console.WriteLine("GetStockOrderStatus error = {0:d}", rc)
        Else
            Console.WriteLine("売買注文1の注文状態の取得に成功しました。")
            Console.WriteLine("[注文状態:{0:d}][最終約定価格:{1:g}]", orderStatus, lastExecPrice)
        End If

        '------------------------------------------------------------------------
        '	売買注文1の執行条件を成行(条件なし)に訂正する
        '------------------------------------------------------------------------
        SBISetOrderNariyuki(order, Cond.NOCOND)

        rc = SBIStockOrderModify(orderNumber, order)
        If (rc < 0) Then
            Console.WriteLine("StockOrderModify error = {0:d}", rc)
            If (rc = SBICode.UNKNOWN_ERROR) Then
                Console.WriteLine(SBIGetLastOrderErrorDescr())
            End If
            SBIReleaseOrder(order)
            Return
        End If

        Console.WriteLine("売買注文1の訂正に成功しました。")
        SBIReleaseOrder(order)

        '------------------------------------------------------------------------
        '	売買注文1を取消す
        '------------------------------------------------------------------------
        rc = SBIStockOrderCancel(orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrderCancel error = {0:d}", rc)
            Return
        End If

        Console.WriteLine("売買注文1の取消しに成功しました。")

        '------------------------------------------------------------------------
        '	売買注文2を発注する
        '		[取引] 信用売り
        '		[市場] 優先市場/SOR
        '		[銘柄] ソフトバンク(9984)
        '		[取引株数] 100株
        '		[執行条件] 指値(9500円引け指し)
        '       [預かり口座] 特定
        '------------------------------------------------------------------------
        order = SBICreateOrder(Trade.MARGIN_SELL, Market.SOR, "9984", 100)
        SBISetOrderSashine(order, 9500, Cond.HIKESASHI)
        SBISetOrderAzukari(order, OA_TOKUTEI)

        rc = SBIStockOrder(order, orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrder error = {0:d}", rc)
            If (rc = SBICode.UNKNOWN_ERROR) Then
                Console.WriteLine(SBIGetLastOrderErrorDescr())
            End If
            SBIReleaseOrder(order)
            Return
        End If

        Console.WriteLine("売買注文2の発注に成功しました。(注文番号:{0:d})", orderNumber)
        SBIReleaseOrder(order)

        '------------------------------------------------------------------------
        '	売買注文2を取消す
        '------------------------------------------------------------------------
        rc = SBIStockOrderCancel(orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrderCancel error = {0:d}", rc)
            Return
        End If

        Console.WriteLine("売買注文2の取消しに成功しました。")

        '------------------------------------------------------------------------
        '	売買注文3を発注する
        '		[取引] 現物買い
        '		[市場] 東証
        '		[銘柄] 日本電気(6701)
        '		[取引株数] 1000株
        '		[執行条件] 逆指値(320円)
        '       [預かり口座] NISA
        '------------------------------------------------------------------------
        order = SBICreateOrder(Trade.BUY, Market.TOKYO, "6701", 1000)
        SBISetOrderGSashine(order, 320)
        SBISetOrderAzukari(order, OA_NISA)

        rc = SBIStockOrder(order, orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrder error = {0:d}", rc)
            If (rc = SBICode.UNKNOWN_ERROR) Then
                Console.WriteLine(SBIGetLastOrderErrorDescr())
            End If
            SBIReleaseOrder(order)
            Return
        End If

        Console.WriteLine("売買注文3の発注に成功しました。(注文番号:{0:d})", orderNumber)
        SBIReleaseOrder(order)

        '------------------------------------------------------------------------
        '	売買注文3を取消す
        '------------------------------------------------------------------------
        rc = SBIStockOrderCancel(orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrderCancel error = {0:d}", rc)
            Return
        End If

        Console.WriteLine("売買注文3の取消しに成功しました。")

        '------------------------------------------------------------------------
        '	売買注文4を発注する
        '		[取引] 現物買い
        '		[市場] 優先市場/SOR
        '		[銘柄] 日産自動車(7201)
        '		[取引株数] 100株
        '		[執行条件] トリガー価格1000円以上になったら指値(980円)で執行
        '       [預かり口座] NISA
        '------------------------------------------------------------------------
        order = SBICreateOrder(Trade.BUY, Market.SOR, "7201", 100)
        SBISetOrderSashine(order, 980, Cond.NOCOND)
        SBISetOrderTrigger(order, 1000, TriggerZone.MORE)
        SBISetOrderAzukari(order, OA_NISA)

        rc = SBIStockOrder(order, orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrder error = {0:d}", rc)
            If (rc = SBICode.UNKNOWN_ERROR) Then
                Console.WriteLine(SBIGetLastOrderErrorDescr())
            End If
            SBIReleaseOrder(order)
            Return
        End If

        Console.WriteLine("売買注文4の発注に成功しました。(注文番号:{0:d})", orderNumber)
        SBIReleaseOrder(order)

        '------------------------------------------------------------------------
        '	売買注文4を取消す
        '------------------------------------------------------------------------
        rc = SBIStockOrderCancel(orderNumber)
        If (rc < 0) Then
            Console.WriteLine("StockOrderCancel error = {0:d}", rc)
            Return
        End If

        Console.WriteLine("売買注文4の取消しに成功しました。")

        '------------------------------------------------------------------------
        '	ログアウトする
        '------------------------------------------------------------------------
        rc = SBILogout()
        If (rc < 0) Then
            Console.WriteLine("Logout error = {0:d}", rc)
            Return
        End If

        Console.WriteLine("ログアウトしました。")

    End Sub

End Module
