#include "FutureOrderControl.h"

System::Void CppCLITester::FutureOrderControl::btnSendFutureOrder_Click(System::Object ^ sender, System::EventArgs ^ e)
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
    // if (double->TryParse(txtPrice->Text->Trim(), out dPrice) == false)
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

    SKCOMLib::FUTUREORDER pFutureOrder;

    pFutureOrder.bstrFullAccount = m_UserAccount;
    pFutureOrder.bstrPrice = strPrice;
    pFutureOrder.bstrStockNo = strFutureNo;
    pFutureOrder.nQty = nQty;
    pFutureOrder.sBuySell = (short)nBidAsk;
    pFutureOrder.sDayTrade = (short)nFlag;
    pFutureOrder.sTradeType = (short)nPeriod;

    //  if (OnFutureOrderSignal != null)
    // {
    OnFutureOrderSignal(m_UserID, ckboxAsyn->Checked, pFutureOrder);
    // }
}

System::Void CppCLITester::FutureOrderControl::btnSendFutureOrderCLR_Click(System::Object ^ sender, System::EventArgs ^ e)
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
    int nNewClose;
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

    if (boxNewClose->SelectedIndex < 0)
    {
        MessageBox::Show("��O");
        return;
    }
    nNewClose = boxNewClose->SelectedIndex;

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
    pFutureOrder.sDayTrade = (short)nFlag;
    pFutureOrder.sTradeType = (short)nPeriod;
    pFutureOrder.sNewClose = (short)nNewClose;
    pFutureOrder.sReserved = (short)nReserved;

    // if (OnFutureOrderCLRSignal != null)
    //{
    OnFutureOrderCLRSignal(m_UserID, ckboxAsyn->Checked, pFutureOrder);
    //}
}

System::Void CppCLITester::FutureOrderControl::btnDecreaseQty_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }
    int nQty = 0;
    System::String ^ strSeqNo;

    if (txtDecreaseSeqNo->Text->Trim() == "")
    {
        MessageBox::Show("��J�eU�Ǹ�");
        return;
    }
    strSeqNo = txtDecreaseSeqNo->Text->Trim();

    if (int ::TryParse(txtDecreaseQty->Text->Trim(), nQty) == false)
    {
        MessageBox::Show("���пJ�Ʀr");
        return;
    }
    // if (OnDecreaseOrderSignal != null)
    //{
    OnDecreaseOrderSignal(m_UserID, true, m_UserAccount, strSeqNo, nQty);
    //}
}

System::Void CppCLITester::FutureOrderControl::btnCancelOrder_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }
    System::String ^ strStockNo;

    if (txtCancelStockNo->Text->Trim() == "")
    {
        MessageBox::Show("��J�eU�Ǹ�");
        return;
    }
    strStockNo = txtCancelStockNo->Text->Trim();
    // if (OnCancelOrderSignal != null)
    //{
    OnCancelByStockNo(m_UserID, true, m_UserAccount, strStockNo);
    //}
}

System::Void CppCLITester::FutureOrderControl::btnCancelOrderBySeqNo_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    System::String ^ strSeqNo;

    if (txtCancelSeqNo->Text->Trim() == "")
    {
        MessageBox::Show("��J�eU�Ǹ�");
        return;
    }
    strSeqNo = txtCancelSeqNo->Text->Trim();
    // if (OnCancelOrderSignal != null)
    // {
    OnCancelBySeqNo(m_UserID, true, m_UserAccount, strSeqNo);
    //}
}

System::Void CppCLITester::FutureOrderControl::btnCancelOrderByBookNo_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }
    System::String ^ strBookNo;

    if (txtCancelBookNo->Text->Trim() == "")
    {
        MessageBox::Show("��J�eU�Ǹ�");
        return;
    }
    strBookNo = txtCancelBookNo->Text->Trim();
    /// if (OnCancelOrderSignal != null)
    //{
    OnCancelByBookNo(m_UserID, true, m_UserAccount, strBookNo);
    // }
}

System::Void CppCLITester::FutureOrderControl::btnCorrectPriceBySeqNo_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    int nTradeType;
    System::String ^ strSeqNo;
    System::String ^ strPrice;

    if (txtCorrectSeqNo->Text->Trim() == "")
    {
        MessageBox::Show("��J�eU�Ǹ�");
        return;
    }
    strSeqNo = txtCorrectSeqNo->Text->Trim();

    double dPrice = 0.0;
    if (double ::TryParse(txtCorrectPrice->Text->Trim(), dPrice) == false)
    {
        MessageBox::Show("����п��Ʀr");
        return;
    }
    strPrice = txtCorrectPrice->Text->Trim();

    if (boxCorrectTradeType->SelectedIndex < 0)
    {
        MessageBox::Show("��e�U����");
        return;
    }
    nTradeType = boxCorrectTradeType->SelectedIndex;

    OnCorrectPriceBySeqNo(m_UserID, true, m_UserAccount, strSeqNo, strPrice, nTradeType);
}

System::Void CppCLITester::FutureOrderControl::btnCorrectPriceByBookNo_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    int nTradeType;
    System::String ^ strBookNo;
    System::String ^ strPrice;

    if (txtCorrectBookNo->Text->Trim() == "")
    {
        MessageBox::Show("��J�eU�Ѹ�");
        return;
    }
    strBookNo = txtCorrectBookNo->Text->Trim();

    double dPrice = 0.0;
    if (double ::TryParse(txtCorrectPrice->Text->Trim(), dPrice) == false)
    {
        MessageBox::Show("����п��Ʀr");
        return;
    }
    strPrice = txtCorrectPrice->Text->Trim();

    if (boxCorrectSymbol->SelectedIndex < 0)
    {
        MessageBox::Show("����²��");
        return;
    }
    nTradeType = boxCorrectTradeType->SelectedIndex;

    if (boxCorrectTradeType->SelectedIndex < 0)
    {
        MessageBox::Show("��e�U����");
        return;
    }

    OnCorrectPriceByBookNo(m_UserID, true, m_UserAccount, boxCorrectSymbol->Text->Trim(), strBookNo, strPrice, nTradeType);
}

System::Void CppCLITester::FutureOrderControl::GetOpenInterest_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    OnGetOpenInterest(m_UserID, m_UserAccount);
}

System::Void CppCLITester::FutureOrderControl::btnGetOpenInterestFormat_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    int Format;

    if (FormatBox->SelectedIndex < 0)
    {
        MessageBox::Show("��^�");
        return;
    }
    Format = FormatBox->SelectedIndex;

    OnGetOpenInterestWithFormat(m_UserID, m_UserAccount, Format);
}

System::Void CppCLITester::FutureOrderControl::btnGetFutureRights_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    int CoinType;
    if (comBox_CoinType->SelectedIndex < 0)
    {
        MessageBox::Show("����O");
        return;
    }
    CoinType = comBox_CoinType->SelectedIndex;

    GetFutureRights(m_UserID, m_UserAccount, CoinType);
}

System::Void CppCLITester::FutureOrderControl::btnSendTXOffset_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    if (m_UserAccount == nullptr)
    {
        MessageBox::Show("����f�b��");
        return;
    }

    if (txtOffsetYearMonth->Text->Trim() == "")
    {
        MessageBox::Show("��J�~��(YYYYMM)");
        return;
    }
    System::String ^ YearMonth = txtOffsetYearMonth->Text->Trim();

    if (boxOffsetBuySell->SelectedIndex < 0)
    {
        MessageBox::Show("��R��O");
        return;
    }
    int BuySell = boxOffsetBuySell->SelectedIndex;

    int Qty;
    if (int ::TryParse(txtOffsetQty->Text->Trim(), Qty) == false)
    {
        MessageBox::Show("�e��q�пJ�Ʀr");
        return;
    }

    SendTXOffset(m_UserID, false, m_UserAccount, YearMonth, BuySell, Qty);
}
