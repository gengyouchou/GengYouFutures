#include "StockOrderControl.h"


#pragma region click

System::Void CppCLITester::StockOrderControl::btnSendStockOrder_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (m_UserAccount == nullptr)
	{
		MessageBox::Show("請選擇證券帳號");
        return;
	}

    System::String^ strStockNo;
    int nPrime;
    int nBidAsk;
    int nPeriod;
    int nFlag;
    System::String^ strPrice;
    int nQty;

    if (txtStockNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入商品代碼");
        return;
    }
    strStockNo = txtStockNo->Text->Trim();

    if (boxPrime->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇上市櫃-興櫃");
        return;
    }
    nPrime = boxPrime->SelectedIndex;

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
    if (double::TryParse(txtPrice->Text->Trim(),  dPrice) == false
        && txtPrice->Text->Trim() != "M"
        && txtPrice->Text->Trim() != "H"
        && txtPrice->Text->Trim() != "h"
        && txtPrice->Text->Trim() != "C"
        && txtPrice->Text->Trim() != "c"
        && txtPrice->Text->Trim() != "L"
        && txtPrice->Text->Trim() != "l")
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
    nQty = int::Parse(txtQty->Text->Trim());

    int nCond, nSpecTradeType;

    if (boxCond->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇委託條件(R/I/F)");
        return;
    }
    nCond = boxCond->SelectedIndex;

    if (boxSpecialTradeType->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇委託價格類型(1市價/0_2限價)");
        return;
    }

    //if (boxSpecialTradeType->SelectedIndex == 1)
    nSpecTradeType = boxSpecialTradeType->SelectedIndex + 1;
    //else
    //    nSpecTradeType = boxSpecialTradeType->SelectedIndex+2; 

    bool boolAsyn = ckboxAsyn->Checked;

    // 宣告物件
    SKCOMLib::STOCKORDER pOrder;

    
    pOrder.bstrFullAccount = m_UserAccount;
    pOrder.bstrPrice = strPrice;
    pOrder.bstrStockNo = strStockNo;
    pOrder.nQty = nQty;
    pOrder.sPrime = (short)nPrime;
    pOrder.sBuySell = (short)nBidAsk;
    pOrder.sFlag = (short)nFlag;
    pOrder.sPeriod = (short)nPeriod;
    pOrder.nTradeType = nCond;
    pOrder.nSpecialTradeType = nSpecTradeType;

   //if (OnOrderSignal != nullptr)
    //{
        OnOrderSignal(m_UserID, boolAsyn, pOrder);
    //}
}

System::Void  CppCLITester::StockOrderControl::btnDecreaseQty_Click(System::Object^ sender, System::EventArgs^ e) {

    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    if (txtDecreaseBookNo->Text->Trim() == "" ) {
        MessageBox::Show("請輸入委託序號");
        return;
    }
    System::String^ bstrSeqNo = txtDecreaseBookNo->Text->Trim();

    int DecreaseQty;
    if (int::TryParse(txtDecreaseQty->Text->Trim(), DecreaseQty) == false)
    {
        MessageBox::Show("委託量請輸入數字");
        return;
    }
    DecreaseQty = int::Parse(txtDecreaseQty->Text->Trim());



    bool boolAsyn = ckboxDecreaseAsyn->Checked;
    //([in] BSTR bstrLogInID, [in] VARIANT_BOOL bAsyncOrder, [in] BSTR bstrAccount, [in] BSTR bstrSeqNo, [in] LONG nDecreaseQty,[out] BSTR* bstrMessage);

    OnDecreaseSignal(m_UserID, boolAsyn,m_UserAccount, bstrSeqNo,DecreaseQty);
}

System::Void CppCLITester::StockOrderControl::btnCancelOrder_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    if (txtCancelStockNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入商品代碼");
        return;
    }

    OnCancelByStockNo(m_UserID,true,m_UserAccount, txtCancelStockNo->Text->Trim());

}

System::Void CppCLITester::StockOrderControl::btnCancelOrderBySeqNo_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    if (txtCancelSeqNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入委託續號");
        return;
    }
    OnCancelBySeqNo(m_UserID, true, m_UserAccount, txtCancelSeqNo->Text->Trim());
}

System::Void CppCLITester::StockOrderControl::btnCancelOrderByBookNo_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    if (txtCancelBookNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入委託書號");
        return;
    }
    OnCancelByBookNo(m_UserID, true, m_UserAccount, txtCancelBookNo->Text->Trim());
}

System::Void CppCLITester::StockOrderControl::btnCorrectPriceBySeqNo_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    int nTradeType;
    System::String ^ strSeqNo;
    System::String ^ strPrice;

    if (txtCorrectSeqNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入委託序號");
        return;
    }
    strSeqNo = txtCorrectSeqNo->Text->Trim();

    double dPrice = 0.0;
    if (double::TryParse(txtCorrectPrice->Text->Trim(), dPrice) == false)
    {
        MessageBox::Show("修改價格請輸入數字");
        return;
    }
    strPrice = txtCorrectPrice->Text->Trim();


    if (boxCorrectTradeType->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇委託條件");
        return;
    }
    nTradeType = boxCorrectTradeType->SelectedIndex;

    OnCorrectPriceBySeqNo(m_UserID,true,m_UserAccount,strSeqNo,strPrice,nTradeType);
}

System::Void CppCLITester::StockOrderControl::btnCorrectPriceByBookNo_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    int nTradeType;
    System::String^ strBookNo;
    System::String^ strPrice;

    if (txtCorrectBookNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入委託書號");
        return;
    }
    strBookNo = txtCorrectBookNo->Text->Trim();

    double dPrice = 0.0;
    if (double::TryParse(txtCorrectPrice->Text->Trim(), dPrice) == false)   
    {
        MessageBox::Show("修改價格請輸入數字");
        return;
    }
    strPrice = txtCorrectPrice->Text->Trim();

    if (boxCorrectSymbol->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇市場簡稱");
        return;
    }

    if (boxCorrectTradeType->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇委託條件");
        return;
    }
    nTradeType = boxCorrectTradeType->SelectedIndex;
    
    OnCorrectPriceByBookNo(m_UserID, true, m_UserAccount, boxCorrectSymbol->Text->Trim(), strBookNo, strPrice, nTradeType);
}

System:: Void CppCLITester::StockOrderControl::btnGetRealBalanceReport_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    OnGetRealBalanceReport(m_UserID,m_UserAccount);
}

System::Void CppCLITester::StockOrderControl::GetBalanceQueryReport_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    if (txtBalanceQueryStockNo->Text->Trim() == "") {
        MessageBox::Show("請輸入商品代碼");
        return;
    }

    OnGetBalanceQuery(m_UserID,m_UserAccount, txtBalanceQueryStockNo->Text->Trim());
}

System::Void CppCLITester::StockOrderControl::btnGetAmountLimit_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    if (txtAmountLimitStockNo->Text->Trim() == "")
    {
        MessageBox::Show("請輸入商品代碼");
        return;
    }

    OnGetMarginPurchaseAmountLimit(m_UserID, m_UserAccount, txtAmountLimitStockNo->Text->Trim());
}
System::Void CppCLITester::StockOrderControl::btnGetRequestProfitReport_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    OnGetRequestProfitReport(m_UserID, m_UserAccount);
}
System::Void CppCLITester::StockOrderControl::btn_GetProfitLossGW_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    SKCOMLib::TSPROFITLOSSGWQUERY pGWQuery ;


    pGWQuery.bstrFullAccount = m_UserAccount;

    if (box_QueryType->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇查詢損益類別");
        return;
    }
    pGWQuery.nTPQueryType = box_QueryType->SelectedIndex;

    int nFormat;

    if (box_format->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇查詢損益格式(0彙總:1明細)");
        return;
    }
    nFormat = box_format->SelectedIndex;
    pGWQuery.bstrFullAccount = m_UserAccount;
    pGWQuery.nFunc = nFormat;

    switch (box_QueryType->SelectedIndex)
    {

    case 0:
    {
        System::String^ strStockNo;
        System::String^ strTradeType;

        if (nFormat == 1)
        {

            strStockNo = txt_ProfitLossStock->Text;
            if (box_format->SelectedIndex < 0)
            {
                MessageBox::Show("請選擇查詢損益格式(0彙總:1明細)");
                return;
            }

            strTradeType = box_format->SelectedIndex.ToString();
            if (box_TradeType->SelectedIndex == 7 || box_TradeType->SelectedIndex == 8)
                strTradeType = (box_format->SelectedIndex + 1).ToString();


            if (box_TradeType->SelectedIndex == -1 || box_TradeType->SelectedIndex == 9)
                strTradeType = " ";


            pGWQuery.bstrStockNo = strStockNo;
            pGWQuery.bstrTradeType = strTradeType;
        }

        //if (OnProfitGWReportSignal != null)
       // {
            OnProfitGWReportSignal(m_UserID, pGWQuery);
        //}
    }
    break;

    case 1:
    {
        System::String^ strTradeType;
        pGWQuery.bstrStartDate = txt_ProfitLossYMStart->Text;

        pGWQuery.bstrEndDate = txt_ProfitLossYMEnd->Text;

        if (nFormat == 1)
        {

            pGWQuery.bstrBookNo = txt_ProfitLossBookNo->Text;
            pGWQuery.bstrSeqNo = txt_ProfitLossSeqNo->Text;
            if (box_TradeType->SelectedIndex == -1 || box_TradeType->SelectedIndex == 9)
                strTradeType = " ";
            else if (box_TradeType->SelectedIndex == 7 || box_TradeType->SelectedIndex == 8)
                strTradeType = (box_format->SelectedIndex + 1).ToString();
            else
                strTradeType = box_TradeType->SelectedItem->ToString();




            pGWQuery.bstrTradeType = strTradeType;
            pGWQuery.bstrStockNo = txt_ProfitLossStock->Text;
            pGWQuery.bstrEndDate = " ";
        }
        if (nFormat == 3)
        {
            pGWQuery.bstrStockNo = txt_ProfitLossStock->Text;
        }
        //if (OnProfitGWReportSignal != null)
       // {
            OnProfitGWReportSignal(m_UserID, pGWQuery);
        //}

    }
    break;
    case 2:
    {//summary:1 //detail:2
        nFormat = box_format->SelectedIndex + 1;

        pGWQuery.nFunc = nFormat;
        if (nFormat == 2)
        {
            pGWQuery.bstrStockNo = txt_ProfitLossStock->Text;

        }
       ///if (OnProfitGWReportSignal != null)
        //{
            OnProfitGWReportSignal(m_UserID, pGWQuery);
       // }
    }
    break;
    default:
        break;


    }
}

System::Void CppCLITester::StockOrderControl::StockOddOrder_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    System::String^ strStockNo;

    int nBidAsk;
    int nPeriod;

    System::String^ strPrice;
    int nQty;

    if (StockNoOdd->Text->Trim() == "")
    {
        MessageBox::Show("請輸入商品代碼");
        return;
    }
    strStockNo = StockNoOdd->Text->Trim();


    if (BuySellBoxOdd->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇買賣別");
        return;
    }
    nBidAsk = BuySellBoxOdd->SelectedIndex;

    if (BoxOddPeriod->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇委託條件");
        return;
    }
    nPeriod = BoxOddPeriod->SelectedIndex + 4;



    double dPrice = 0.0;
    if (double::TryParse(PriceOdd->Text->Trim(),  dPrice) == false
        && PriceOdd->Text->Trim() != "M"
        && PriceOdd->Text->Trim() != "H"
        && PriceOdd->Text->Trim() != "h"
        && PriceOdd->Text->Trim() != "C"
        && PriceOdd->Text->Trim() != "c"
        && PriceOdd->Text->Trim() != "L"
        && PriceOdd->Text->Trim() != "l")
    {
        MessageBox::Show("委託價請輸入數字");
        return;
    }
    strPrice = PriceOdd->Text->Trim();

    if (int::TryParse(QtyOdd->Text->Trim(), nQty) == false)
    {
        MessageBox::Show("委託量請輸入數字");
        return;
    }



    SKCOMLib::STOCKORDER pOrder;

    pOrder.bstrFullAccount = m_UserAccount;
    pOrder.bstrPrice = strPrice;
    pOrder.bstrStockNo = strStockNo;
    pOrder.nQty = nQty;
    pOrder.sPrime = 0;
    pOrder.sBuySell = (short)nBidAsk;
    //pOrder.sFlag = (short)nFlag;
    pOrder.sPeriod = (short)nPeriod;



    OnOddOrderSignal(m_UserID, false, pOrder);
}
System::Void CppCLITester::StockOrderControl::StockOddOrderAsync_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("請選擇證券帳號");
        return;
    }

    System::String^ strStockNo;

    int nBidAsk;
    int nPeriod;

    System::String^ strPrice;
    int nQty;

    if (StockNoOdd->Text->Trim() == "")
    {
        MessageBox::Show("請輸入商品代碼");
        return;
    }
    strStockNo = StockNoOdd->Text->Trim();


    if (BuySellBoxOdd->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇買賣別");
        return;
    }
    nBidAsk = BuySellBoxOdd->SelectedIndex;

    if (BoxOddPeriod->SelectedIndex < 0)
    {
        MessageBox::Show("請選擇委託條件");
        return;
    }
    nPeriod = BoxOddPeriod->SelectedIndex + 4;



    double dPrice = 0.0;
    if (double::TryParse(PriceOdd->Text->Trim(), dPrice) == false
        && PriceOdd->Text->Trim() != "M"
        && PriceOdd->Text->Trim() != "H"
        && PriceOdd->Text->Trim() != "h"
        && PriceOdd->Text->Trim() != "C"
        && PriceOdd->Text->Trim() != "c"
        && PriceOdd->Text->Trim() != "L"
        && PriceOdd->Text->Trim() != "l")
    {
        MessageBox::Show("委託價請輸入數字");
        return;
    }
    strPrice = PriceOdd->Text->Trim();

    if (int::TryParse(QtyOdd->Text->Trim(), nQty) == false)
    {
        MessageBox::Show("委託量請輸入數字");
        return;
    }



    SKCOMLib::STOCKORDER pOrder;

    pOrder.bstrFullAccount = m_UserAccount;
    pOrder.bstrPrice = strPrice;
    pOrder.bstrStockNo = strStockNo;
    pOrder.nQty = nQty;
    pOrder.sPrime = 0;
    pOrder.sBuySell = (short)nBidAsk;
    //pOrder.sFlag = (short)nFlag;
    pOrder.sPeriod = (short)nPeriod;



    OnOddOrderSignal(m_UserID, true, pOrder);
}
#pragma endregion