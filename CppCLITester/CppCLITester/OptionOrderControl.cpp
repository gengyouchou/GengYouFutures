#include "OptionOrderControl.h"

Void CppCLITester::OptionOrderControl::btnSendOptionOrder_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    System::String ^ strFutureNo;
    int nBidAsk;
    int nPeriod;
    int nFlag;
    System::String ^ strPrice;
    int nQty;
    int nReserved;

    if (txtStockNo->Text->Trim() == "")
    {
        MessageBox::Show("��Jӫ~�N�X");
        return;
    }
    strFutureNo = txtStockNo->Text->Trim();

    if (boxBidAsk->SelectedIndex < 0)
    {
        MessageBox::Show("��R��O");
        return;
    }
    nBidAsk = boxBidAsk->SelectedIndex;

    if (boxPeriod->SelectedIndex < 0)
    {
        MessageBox::Show("��e�U����");
        return;
    }
    nPeriod = boxPeriod->SelectedIndex;

    if (boxFlag->SelectedIndex < 0)
    {
        MessageBox::Show("����R�P�_");
        return;
    }
    nFlag = boxFlag->SelectedIndex;

    double dPrice = 0.0;
    if (double ::TryParse(txtPrice->Text->Trim(), dPrice) == false && txtPrice->Text->Trim() != "M" && txtPrice->Text->Trim() != "P")
    {
        MessageBox::Show("�e�����J�Ʀr");
        return;
    }
    strPrice = txtPrice->Text->Trim();

    if (int ::TryParse(txtQty->Text->Trim(), nQty) == false)
    {
        MessageBox::Show("�e��q�пJ�Ʀr");
        return;
    }

    if (boxReserved->SelectedIndex < 0)
    {
        MessageBox::Show("��L�O");
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

Void CppCLITester::OptionOrderControl::btnSendOptionOrderAsync_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    System::String ^ strFutureNo;
    int nBidAsk;
    int nPeriod;
    int nFlag;
    System::String ^ strPrice;
    int nQty;
    int nReserved;

    if (txtStockNo->Text->Trim() == "")
    {
        MessageBox::Show("��Jӫ~�N�X");
        return;
    }
    strFutureNo = txtStockNo->Text->Trim();

    if (boxBidAsk->SelectedIndex < 0)
    {
        MessageBox::Show("��R��O");
        return;
    }
    nBidAsk = boxBidAsk->SelectedIndex;

    if (boxPeriod->SelectedIndex < 0)
    {
        MessageBox::Show("��e�U����");
        return;
    }
    nPeriod = boxPeriod->SelectedIndex;

    if (boxFlag->SelectedIndex < 0)
    {
        MessageBox::Show("��O");
        return;
    }
    nFlag = boxFlag->SelectedIndex;

    double dPrice = 0.0;
    if (double ::TryParse(txtPrice->Text->Trim(), dPrice) == false && txtPrice->Text->Trim() != "M" && txtPrice->Text->Trim() != "P")
    {
        MessageBox::Show("�e�����J�Ʀr");
        return;
    }
    strPrice = txtPrice->Text->Trim();

    if (int ::TryParse(txtQty->Text->Trim(), nQty) == false)
    {
        MessageBox::Show("�e��q�пJ�Ʀr");
        return;
    }

    if (boxReserved->SelectedIndex < 0)
    {
        MessageBox::Show("��L�O");
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
