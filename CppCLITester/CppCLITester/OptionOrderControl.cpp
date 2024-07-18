#include "OptionOrderControl.h"

Void CppCLITester::OptionOrderControl::btnSendOptionOrder_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (m_UserAccount == nullptr)
	{
		MessageBox::Show("請選擇期貨帳號");
		return;
	}

	System::String^ strFutureNo;
	int nBidAsk;
	int nPeriod;
	int nFlag;
	System::String^ strPrice;
	int nQty;
	int nReserved;


	if (txtStockNo->Text->Trim() == "")
	{
		MessageBox::Show("請輸入商品代碼");
		return;
	}
	strFutureNo = txtStockNo->Text->Trim();

	if (boxBidAsk->SelectedIndex < 0)
	{
		MessageBox::Show("請選擇買賣別");
		return;
	}
	nBidAsk = boxBidAsk->SelectedIndex;

	if (boxPeriod->SelectedIndex < 0)
	{
		MessageBox::Show("請選擇委託條件");
		return;
	}
	nPeriod = boxPeriod->SelectedIndex;

	if (boxFlag->SelectedIndex < 0)
	{
		MessageBox::Show("請選擇當沖與否");
		return;
	}
	nFlag = boxFlag->SelectedIndex;

	double dPrice = 0.0;
	if (double::TryParse(txtPrice->Text->Trim(), dPrice) == false && txtPrice->Text->Trim() != "M" && txtPrice->Text->Trim() != "P")
	{
		MessageBox::Show("委託價請輸入數字");
		return;
	}
	strPrice = txtPrice->Text->Trim();

	if (int::TryParse(txtQty->Text->Trim(), nQty) == false)
	{
		MessageBox::Show("委託量請輸入數字");
		return;
	}

	if (boxReserved->SelectedIndex < 0)
	{
		MessageBox::Show("請選擇盤別");
		return;
	}
	nReserved = boxReserved->SelectedIndex;

	SKCOMLib::FUTUREORDER pFutureOrder;

	pFutureOrder.bstrFullAccount = m_UserAccount;
	pFutureOrder.bstrPrice = strPrice;
	pFutureOrder.bstrStockNo = strFutureNo;
	pFutureOrder.nQty = nQty;
	pFutureOrder.sBuySell = (short)nBidAsk;
	pFutureOrder.sNewClose = (short)nFlag;
	pFutureOrder.sTradeType = (short)nPeriod;
	pFutureOrder.sReserved = (short)nReserved;

	pFutureOrder.bstrTrigger = "";
	pFutureOrder.bstrDealPrice = "";
	pFutureOrder.bstrMovingPoint = "";

	OnOptionOrderSignal(m_UserID, false, pFutureOrder);
}

Void CppCLITester::OptionOrderControl::btnSendOptionOrderAsync_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇期貨帳號");
        return;
    }

    System::String^ strFutureNo;
    int nBidAsk;
    int nPeriod;
    int nFlag;
    System::String^ strPrice;
    int nQty;
    int nReserved;


    if (txtStockNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入商品代碼");
        return;
    }
    strFutureNo = txtStockNo->Text->Trim();

    if (boxBidAsk->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇買賣別");
        return;
    }
    nBidAsk = boxBidAsk->SelectedIndex;

    if (boxPeriod->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇委託條件");
        return;
    }
    nPeriod = boxPeriod->SelectedIndex;

    if (boxFlag->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇倉別");
        return;
    }
    nFlag = boxFlag->SelectedIndex;

    double dPrice = 0.0;
    if (double::TryParse(txtPrice->Text->Trim(),dPrice) == false && txtPrice->Text->Trim() != "M" && txtPrice->Text->Trim() != "P")
    {
        MessageBox::Show("委託價請輸入數字");
        return;
    }
    strPrice = txtPrice->Text->Trim();

    if (int::TryParse(txtQty->Text->Trim(),nQty) == false)
    {
        MessageBox::Show("委託量請輸入數字");
        return;
    }

    if (boxReserved->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇盤別");
        return;
    }
    nReserved = boxReserved->SelectedIndex;

    SKCOMLib::FUTUREORDER pFutureOrder;

    pFutureOrder.bstrFullAccount = m_UserAccount;
    pFutureOrder.bstrPrice = strPrice;
    pFutureOrder.bstrStockNo = strFutureNo;
    pFutureOrder.nQty = nQty;
    pFutureOrder.sBuySell = (short)nBidAsk;
    pFutureOrder.sNewClose = (short)nFlag;
    pFutureOrder.sTradeType = (short)nPeriod;
    pFutureOrder.sReserved = (short)nReserved;

    pFutureOrder.bstrTrigger = "";
    pFutureOrder.bstrDealPrice = "";
    pFutureOrder.bstrMovingPoint = "";

    OnOptionOrderSignal(m_UserID, true, pFutureOrder);
}
