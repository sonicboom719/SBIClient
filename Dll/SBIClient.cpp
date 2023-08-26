#include "stdafx.h"
#include "SBIClient.h"
#include "CSBIWebClient.h"


static CSBIWebClient gSbi;
static bool gInitialized = false;

#define INITIALIZE_CHECK	{if (!gInitialized) return SBI_NOT_INITIALIZED;}


int STDCALL SBIInitialize(void)
{
	int rest_days = 100;

	gInitialized = true;

	return gInitialized ? rest_days : SBI_NOT_AVAILABLE;
}

int STDCALL SBILogin( const char* username, const char* password )
{
	INITIALIZE_CHECK
	return gSbi.Login( username, password );
}

int STDCALL SBILogout(void)
{
	INITIALIZE_CHECK
	return gSbi.Logout();
}

int STDCALL SBIGetAccountInfo( ACCOUNTINFO* accountInfo )
{
	INITIALIZE_CHECK
	return gSbi.GetAccountInfo( (CSBIWebClient::ACCOUNTINFO*)accountInfo );
}

int STDCALL SBIGetStockPrice( int code, STOCKPRICES* prices )
{
	INITIALIZE_CHECK
	return gSbi.GetRealtimeStockPrice( code, (CSBIWebClient::STOCKPRICES*)prices );
}

void STDCALL SBISetOrderPassword( const char* password )
{
	gSbi.SetOrderPassword( password );
}

const char* STDCALL SBIGetLastOrderErrorDescr(void)
{
	//	C#から使うとなぜか落ちるので、以下のように間にバッファをかませた。
	static char errmsg[256];
	strncpy( errmsg, gSbi.GetLastOrderErrorDescr(), sizeof(errmsg) - 1 );
	return errmsg;
}

STOCKORDER STDCALL SBICreateOrder( int trade, int market, int code, long quantity )
{
	CSBIWebClient::CStockOrder* pOrder = new CSBIWebClient::CStockOrder();

	if (pOrder == NULL) return NULL;

	pOrder->SetTrade( trade );
	pOrder->SetMarket( market );
	pOrder->SetBrandCode( code );
	pOrder->SetQuantity( quantity );

	return pOrder;
}

void STDCALL SBIReleaseOrder( STOCKORDER order )
{
	if (order != NULL) delete order;
}

void STDCALL SBISetOrderNariyuki( STOCKORDER order, int cond )
{
	((CSBIWebClient::CStockOrder*)order)->SetNariyuki( cond );
}

void STDCALL SBISetOrderSashine( STOCKORDER order, double price, int cond )
{
	((CSBIWebClient::CStockOrder*)order)->SetSashine( price, cond );
}

void STDCALL SBISetOrderGSashine( STOCKORDER order, double price )
{
	((CSBIWebClient::CStockOrder*)order)->SetGyakusashine( price );
}

void STDCALL SBISetOrderTrigger( STOCKORDER order, double price, int zone )
{
	((CSBIWebClient::CStockOrder*)order)->SetTrigger( price, zone );
}

void STDCALL SBISetOrderMargin( STOCKORDER order, int margin )
{
	((CSBIWebClient::CStockOrder*)order)->SetMargin( margin );
}

void STDCALL SBISetOrderAzukari( STOCKORDER order, const char* azukari )
{
	((CSBIWebClient::CStockOrder*)order)->SetAzukari( azukari );
}

int STDCALL SBIStockOrder( STOCKORDER order, int* orderNumber )
{
	INITIALIZE_CHECK
	return gSbi.StockOrder( *(CSBIWebClient::CStockOrder*)order, orderNumber );
}

int STDCALL SBIStockOrderModify( int orderNumber, STOCKORDER order )
{
	INITIALIZE_CHECK
	return gSbi.StockOrderModify( orderNumber, *(CSBIWebClient::CStockOrder*)order );
}

int STDCALL SBIStockOrderCancel( int orderNumber )
{
	INITIALIZE_CHECK
	return gSbi.StockOrderCancel( orderNumber );
}

int STDCALL SBIGetStockOrderStatus( int orderNumber, int* orderStatus, double* lastExecPrice )
{
	INITIALIZE_CHECK
	return gSbi.GetStockOrderStatus( orderNumber, orderStatus, lastExecPrice );
}
