using System;
using SKCOMLib;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class OOQuoteForm : Form
    {
        // 
        bool isClosing = false;
        string m_UserID;
        // 
        SKCenterLib m_pSKCenter = new SKCenterLib(); // &
        SKOOQuoteLib m_pSKOOQuote = new SKOOQuoteLib(); // 
        public OOQuoteForm(string UserID)
        {
            // Init
            {
                InitializeComponent();
                m_UserID = UserID;
                labelUserID.Text = UserID;
                // dataGridView
                {
                    // dataGridViewSKOOQuoteLib_GetStockByNoLONG
                    {
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column1", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column2", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column3", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column4", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column5", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column6", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column7", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column8", "CallPut");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column9", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column10", "");

                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column11", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column12", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column13", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column14", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column15", "()");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column16", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column17", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column18", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column19", "");
                        dataGridViewSKOOQuoteLib_GetStockByNoLONG.Columns.Add("Column20", "");
                    }
                    // dataGridViewStocks
                    {
                        dataGridViewStocks.Columns.Add("Column1", "");
                        dataGridViewStocks.Columns.Add("Column2", "");
                        dataGridViewStocks.Columns.Add("Column3", "");
                        dataGridViewStocks.Columns.Add("Column4", "");
                        dataGridViewStocks.Columns.Add("Column5", "");
                        dataGridViewStocks.Columns.Add("Column6", "");
                        dataGridViewStocks.Columns.Add("Column7", "");
                        dataGridViewStocks.Columns.Add("Column8", "CallPut");
                        dataGridViewStocks.Columns.Add("Column9", "");
                        dataGridViewStocks.Columns.Add("Column10", "");

                        dataGridViewStocks.Columns.Add("Column11", "");
                        dataGridViewStocks.Columns.Add("Column12", "");
                        dataGridViewStocks.Columns.Add("Column13", "");
                        dataGridViewStocks.Columns.Add("Column14", "");
                        dataGridViewStocks.Columns.Add("Column15", "()");
                        dataGridViewStocks.Columns.Add("Column16", "");
                        dataGridViewStocks.Columns.Add("Column17", "");
                        dataGridViewStocks.Columns.Add("Column18", "");
                        dataGridViewStocks.Columns.Add("Column19", "");
                        dataGridViewStocks.Columns.Add("Column20", "");
                    }
                    // OnNotifyTicksLONG
                    {
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column1", "");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column2", "：：");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column3", "");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column4", "");

                        for (int i = 0; i < 1; i++)
                            dataGridViewOnNotifyTicksLONG.Rows.Add();
                    }
                    // Ticks
                    {
                        dataGridViewTicks.Columns.Add("Column1", "");
                        dataGridViewTicks.Columns.Add("Column2", "");
                        dataGridViewTicks.Columns.Add("Column3", "");
                        dataGridViewTicks.Columns.Add("Column4", "");

                        for (int i = 0; i < 5; i++)
                            dataGridViewTicks.Rows.Add();
                    }
                    // dataGridViewOnNotifyBest10NineDigitLONG
                    {
                        dataGridViewOnNotifyBest10NineDigitLONG.Columns.Add("Column1", "");
                        dataGridViewOnNotifyBest10NineDigitLONG.Columns.Add("Column2", "");
                        dataGridViewOnNotifyBest10NineDigitLONG.Columns.Add("Column3", "");
                        dataGridViewOnNotifyBest10NineDigitLONG.Columns.Add("Column4", "");

                        for (int i = 0; i < 10; i++)
                            dataGridViewOnNotifyBest10NineDigitLONG.Rows.Add();
                    }
                }
            }
        }
        private void OOQuoteForm_Load(object sender, EventArgs e)
        {
            //
            m_pSKOOQuote.OnConnect += new _ISKOOQuoteLibEvents_OnConnectEventHandler(OnConnect);
            void OnConnect(int nCode, int nSocketCode)
            {
                if (isClosing != true)
                {
                    // 
                    string msg = "【OnConnect】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMessage.AppendText(msg + "\n");

                    if (nSocketCode != 0)
                    {
                        // 
                        msg = "【OnConnect】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nSocketCode);
                        richTextBoxMessage.AppendText(msg + "\n");
                    }
                }
            }
            //
            m_pSKOOQuote.OnProducts += new _ISKOOQuoteLibEvents_OnProductsEventHandler(OnProducts);
            void OnProducts(string bstrValue)
            {
                if (isClosing != true)
                {
                    //
                    string msg = bstrValue;
                    richTextBoxOnProducts.AppendText(msg + "\n");
                }
            }
            //(LONG index)，
            m_pSKOOQuote.OnNotifyQuoteLONG += new _ISKOOQuoteLibEvents_OnNotifyQuoteLONGEventHandler(OnNotifyQuoteLONG);
            void OnNotifyQuoteLONG(int nIndex)
            {
                if (isClosing != true)
                {
                    bool stockFound = false; // dataGridView，
                    SKFOREIGNLONG pSKStock = new SKFOREIGNLONG();
                    // (LONG index)，
                    int nCode = m_pSKOOQuote.SKOOQuoteLib_GetStockByIndexLONG(nIndex, ref pSKStock);
                    // 
                    string msg = "【SKOOQuoteLib_GetStockByIndexLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMethodMessage.AppendText(msg + "\n");

                    foreach (DataGridViewRow row in dataGridViewStocks.Rows)
                    {
                        if (row.Cells[0].Value.ToString() == pSKStock.bstrStockNo)
                        {
                            row.Cells[2].Value = pSKStock.sDecimal;
                            row.Cells[3].Value = pSKStock.nDenominator;
                            row.Cells[4].Value = pSKStock.bstrMarketNo;
                            row.Cells[5].Value = pSKStock.bstrExchangeNo;
                            row.Cells[6].Value = pSKStock.bstrExchangeName;
                            row.Cells[7].Value = pSKStock.bstrCallPut;
                            row.Cells[8].Value = pSKStock.nOpen;
                            row.Cells[9].Value = pSKStock.nHigh;
                            row.Cells[10].Value = pSKStock.nLow;
                            row.Cells[11].Value = pSKStock.nClose;
                            row.Cells[12].Value = pSKStock.nSettlePrice;
                            row.Cells[13].Value = pSKStock.nTickQty;
                            row.Cells[14].Value = pSKStock.nRef;
                            row.Cells[15].Value = pSKStock.nBid;
                            row.Cells[16].Value = pSKStock.nBc;
                            row.Cells[17].Value = pSKStock.nAsk;
                            row.Cells[18].Value = pSKStock.nAc;
                            row.Cells[19].Value = pSKStock.nTradingDay;
                            stockFound = true;
                            break;
                        }
                    }
                    if (stockFound == false)
                    {
                        dataGridViewStocks.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.sDecimal, pSKStock.nDenominator, pSKStock.bstrMarketNo, pSKStock.bstrExchangeNo, pSKStock.bstrExchangeName, pSKStock.bstrCallPut, pSKStock.nOpen, pSKStock.nHigh, pSKStock.nLow, pSKStock.nClose, pSKStock.nSettlePrice, pSKStock.nTickQty, pSKStock.nRef, pSKStock.nBid, pSKStock.nBc, pSKStock.nAsk, pSKStock.nAc, pSKStock.nTradingDay);
                    }
                }
            }
            //(LONG index)，
            m_pSKOOQuote.OnNotifyTicksLONG += new _ISKOOQuoteLibEvents_OnNotifyTicksLONGEventHandler(OnNotifyTicksLONG);
            void OnNotifyTicksLONG(int nIndex, int nPtr, int nDate, int nTime, int nClose, int nQty)
            {
                if (isClosing != true)
                {
                    SKFOREIGNTICK pSKTick = new SKFOREIGNTICK();
                    int nCode = m_pSKOOQuote.SKOOQuoteLib_GetTickLONG(nIndex, nPtr, ref pSKTick);

                    // 
                    string msg = "【SKOOQuoteLib_GetTickLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMethodMessage.AppendText(msg + "\n");

                    if (dataGridViewOnNotifyTicksLONG.Rows.Count > 0)
                    {
                        DataGridViewRow Row1 = dataGridViewOnNotifyTicksLONG.Rows[0];
                        Row1.Cells[0].Value = pSKTick.nDate; // 。(YYYYMMDD)
                        Row1.Cells[1].Value = pSKTick.nTime; // 1。(：：)
                        Row1.Cells[2].Value = pSKTick.nClose / 100.0; // 
                        Row1.Cells[3].Value = pSKTick.nQty / 100.0; // 
                    }
                }
            }
            //(LONG index)，Tick 【】
            // m_pSKOOQuote.OnNotifyHistoryTicksLONG += new _ISKOOQuoteLibEvents_OnNotifyHistoryTicksLONGEventHandler(OnNotifyHistoryTicksLONG);
            void OnNotifyHistoryTicksLONG(int nIndex, int nPtr, int nDate, int nTime, int nClose, int nQty)
            {
                if (isClosing != true)
                {
                    if (dataGridViewOnNotifyTicksLONG.Rows.Count > 0)
                    {
                        DataGridViewRow Row1 = dataGridViewOnNotifyTicksLONG.Rows[0];
                        Row1.Cells[0].Value = nDate; // 。(YYYYMMDD)
                        Row1.Cells[1].Value = nTime; // 1。(：：)
                        Row1.Cells[5].Value = nClose / 100.0; // 
                        Row1.Cells[6].Value = nQty / 100.0; // 
                    }
                }
            }
            //()
            m_pSKOOQuote.OnNotifyBest5LONG += new _ISKOOQuoteLibEvents_OnNotifyBest5LONGEventHandler(OnNotifyBest5LONG);
            void OnNotifyBest5LONG(int nStockIdx, int nBestBid1, int nBestBidQty1, int nBestBid2, int nBestBidQty2, int nBestBid3, int nBestBidQty3, int nBestBid4, int nBestBidQty4, int nBestBid5, int nBestBidQty5, int nBestAsk1, int nBestAskQty1, int nBestAsk2, int nBestAskQty2, int nBestAsk3, int nBestAskQty3, int nBestAsk4, int nBestAskQty4, int nBestAsk5, int nBestAskQty5)
            {
                if (isClosing != true)
                {
                    SKBEST5 pSKBest5 = new SKBEST5();
                    int nCode = m_pSKOOQuote.SKOOQuoteLib_GetBest5LONG(nStockIdx, ref pSKBest5);

                    // 
                    string msg = "【SKOOQuoteLib_GetBest5LONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMethodMessage.AppendText(msg + "\n");

                    if (dataGridViewTicks.Rows.Count > 0)
                    {
                        DataGridViewRow Row1 = dataGridViewTicks.Rows[0];
                        Row1.Cells[0].Value = pSKBest5.nBidQty1; //
                        Row1.Cells[1].Value = pSKBest5.nBid1 / 100.0; //
                        Row1.Cells[2].Value = pSKBest5.nAsk1 / 100.0; //
                        Row1.Cells[3].Value = pSKBest5.nAskQty1; //

                        DataGridViewRow Row2 = dataGridViewTicks.Rows[1];
                        Row2.Cells[0].Value = pSKBest5.nBidQty2; //
                        Row2.Cells[1].Value = pSKBest5.nBid2 / 100.0; //
                        Row2.Cells[2].Value = pSKBest5.nAsk2 / 100.0; //
                        Row2.Cells[3].Value = pSKBest5.nAskQty2; //

                        DataGridViewRow Row3 = dataGridViewTicks.Rows[2];
                        Row3.Cells[0].Value = pSKBest5.nBidQty3; //
                        Row3.Cells[1].Value = pSKBest5.nBid3 / 100.0; //
                        Row3.Cells[2].Value = pSKBest5.nAsk3 / 100.0; //
                        Row3.Cells[3].Value = pSKBest5.nAskQty3; //

                        DataGridViewRow Row4 = dataGridViewTicks.Rows[3];
                        Row4.Cells[0].Value = pSKBest5.nBidQty4; //
                        Row4.Cells[1].Value = pSKBest5.nBid4 / 100.0; //
                        Row4.Cells[2].Value = pSKBest5.nAsk4 / 100.0; //
                        Row4.Cells[3].Value = pSKBest5.nAskQty4; //

                        DataGridViewRow Row5 = dataGridViewTicks.Rows[4];
                        Row5.Cells[0].Value = pSKBest5.nBidQty5; //
                        Row5.Cells[1].Value = pSKBest5.nBid5 / 100.0; //
                        Row5.Cells[2].Value = pSKBest5.nAsk5 / 100.0; //
                        Row5.Cells[3].Value = pSKBest5.nAskQty5; //
                    }
                }
            }
            //()
            m_pSKOOQuote.OnNotifyBest10LONG += new _ISKOOQuoteLibEvents_OnNotifyBest10LONGEventHandler(OnNotifyBest10LONG);
            void OnNotifyBest10LONG(int nStockIdx, int nBestBid1, int nBestBidQty1, int nBestBid2, int nBestBidQty2, int nBestBid3, int nBestBidQty3, int nBestBid4, int nBestBidQty4, int nBestBid5, int nBestBidQty5, int nBestBid6, int nBestBidQty6, int nBestBid7, int nBestBidQty7, int nBestBid8, int nBestBidQty8, int nBestBid9, int nBestBidQty9, int nBestBid10, int nBestBidQty10, int nBestAsk1, int nBestAskQty1, int nBestAsk2, int nBestAskQty2, int nBestAsk3, int nBestAskQty3, int nBestAsk4, int nBestAskQty4, int nBestAsk5, int nBestAskQty5, int nBestAsk6, int nBestAskQty6, int nBestAsk7, int nBestAskQty7, int nBestAsk8, int nBestAskQty8, int nBestAsk9, int nBestAskQty9, int nBestAsk10, int nBestAskQty10)
            {
                if (isClosing != true)
                {
                    if (dataGridViewOnNotifyBest10NineDigitLONG.Rows.Count > 0)
                    {
                        DataGridViewRow Row1 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[0];
                        Row1.Cells[0].Value = nBestBidQty1; //
                        Row1.Cells[1].Value = nBestBid1 / 100.0; //
                        Row1.Cells[2].Value = nBestAsk1 / 100.0; //
                        Row1.Cells[3].Value = nBestAskQty1; //

                        DataGridViewRow Row2 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[1];
                        Row2.Cells[0].Value = nBestBidQty2; //
                        Row2.Cells[1].Value = nBestBid2 / 100.0; //
                        Row2.Cells[2].Value = nBestAsk2 / 100.0; //
                        Row2.Cells[3].Value = nBestAskQty2; //

                        DataGridViewRow Row3 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[2];
                        Row3.Cells[0].Value = nBestBidQty3; //
                        Row3.Cells[1].Value = nBestBid3 / 100.0; //
                        Row3.Cells[2].Value = nBestAsk3 / 100.0; //
                        Row3.Cells[3].Value = nBestAskQty3; //

                        DataGridViewRow Row4 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[3];
                        Row4.Cells[0].Value = nBestBidQty4; //
                        Row4.Cells[1].Value = nBestBid4 / 100.0; //
                        Row4.Cells[2].Value = nBestAsk4 / 100.0; //
                        Row4.Cells[3].Value = nBestAskQty4; //

                        DataGridViewRow Row5 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[4];
                        Row5.Cells[0].Value = nBestBidQty5; //
                        Row5.Cells[1].Value = nBestBid5 / 100.0; //
                        Row5.Cells[2].Value = nBestAsk5 / 100.0; //
                        Row5.Cells[3].Value = nBestAskQty5; //

                        DataGridViewRow Row6 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[5];
                        Row6.Cells[0].Value = nBestBidQty6; //
                        Row6.Cells[1].Value = nBestBid6 / 100.0; //
                        Row6.Cells[2].Value = nBestAsk6 / 100.0; //
                        Row6.Cells[3].Value = nBestAskQty6; //

                        DataGridViewRow Row7 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[6];
                        Row7.Cells[0].Value = nBestBidQty7; //
                        Row7.Cells[1].Value = nBestBid7 / 100.0; //
                        Row7.Cells[2].Value = nBestAsk7 / 100.0; //
                        Row7.Cells[3].Value = nBestAskQty7; //

                        DataGridViewRow Row8 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[7];
                        Row8.Cells[0].Value = nBestBidQty8; //
                        Row8.Cells[1].Value = nBestBid8 / 100.0; //
                        Row8.Cells[2].Value = nBestAsk8 / 100.0; //
                        Row8.Cells[3].Value = nBestAskQty8; //

                        DataGridViewRow Row9 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[8];
                        Row9.Cells[0].Value = nBestBidQty9; //
                        Row9.Cells[1].Value = nBestBid9 / 100.0; //
                        Row9.Cells[2].Value = nBestAsk9 / 100.0; //
                        Row9.Cells[3].Value = nBestAskQty9; //

                        DataGridViewRow Row10 = dataGridViewOnNotifyBest10NineDigitLONG.Rows[9];
                        Row10.Cells[0].Value = nBestBidQty10; //
                        Row10.Cells[1].Value = nBestBid10 / 100.0; //
                        Row10.Cells[2].Value = nBestAsk10 / 100.0; //
                        Row10.Cells[3].Value = nBestAskQty10; //
                    }
                }
            }
        }
        private void OOQuoteForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isClosing = true;
            // 
            m_pSKOOQuote.SKOOQuoteLib_LeaveMonitor();
        }
        private void buttonSearchKeyWord_Click(object sender, EventArgs e)
        {
            // 
            richTextBoxOnProducts.SelectAll();

            // （）
            richTextBoxOnProducts.SelectionBackColor = richTextBoxOnProducts.BackColor;

            // 
            richTextBoxOnProducts.DeselectAll();

            int index = 0;

            while (index < richTextBoxOnProducts.Text.Length)
            {
                // 
                index = richTextBoxOnProducts.Find(searchText.Text, index, richTextBoxOnProducts.TextLength, RichTextBoxFinds.None);

                if (index != -1)
                {
                    // ，
                    richTextBoxOnProducts.Select(index, searchText.Text.Length);

                    // 
                    richTextBoxOnProducts.SelectionBackColor = Color.Yellow;

                    // 
                    index += searchText.Text.Length;
                }
                else
                {
                    break;
                }
            }
        }
        private void buttonSKOOQuoteLib_EnterMonitorLONG_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOOQuote.SKOOQuoteLib_EnterMonitorLONG();
            // 
            string msg = "【SKOOQuoteLib_EnterMonitorLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKOOQuoteLib_LeaveMonitor_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOOQuote.SKOOQuoteLib_LeaveMonitor();
            // 
            string msg = "【SKOOQuoteLib_LeaveMonitor】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKOOQuoteLib_IsConnected_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOOQuote.SKOOQuoteLib_IsConnected();
            // 
            string msg = "";
            if (nCode == 1) msg = "";
            else msg = "";
            msg = "【SKOOQuoteLib_IsConnected】" + msg;
            richTextBoxMessage.AppendText(msg + "\n");
        }
        private void buttonSKOOQuoteLib_RequestProducts_Click(object sender, EventArgs e)
        {
            richTextBoxOnProducts.Clear();
            // 
            int nCode = m_pSKOOQuote.SKOOQuoteLib_RequestProducts();
            // 
            string msg = "【SKOOQuoteLib_RequestProducts】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");            
        }
        private void buttonSKOOQuoteLib_GetStockByNoLONG_Click(object sender, EventArgs e)
        {
            SKFOREIGNLONG pSKStock = new SKFOREIGNLONG();
            string bstrStockNo = textBoxSKOOQuoteLib_GetStockByNoLONG.Text;
            // (LONG index)，。
            int nCode = m_pSKOOQuote.SKOOQuoteLib_GetStockByNoLONG(bstrStockNo, ref pSKStock);

            // 
            string msg = "【SKOOQuoteLib_GetStockByNoLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");

            // SKFOREIGNLONG
            bool found = false; // 
            string[] values = bstrStockNo.Split(','); //  
            foreach (DataGridViewRow row in dataGridViewSKOOQuoteLib_GetStockByNoLONG.Rows)
            {
                if (row.IsNewRow)
                    continue;
                if (row.Cells[0].Value != null)
                {
                    if (values[1] == row.Cells[0].Value.ToString())// 
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (found == false)
                dataGridViewSKOOQuoteLib_GetStockByNoLONG.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.sDecimal, pSKStock.nDenominator, pSKStock.bstrMarketNo, pSKStock.bstrExchangeNo, pSKStock.bstrExchangeName, pSKStock.bstrCallPut, pSKStock.nOpen, pSKStock.nHigh, pSKStock.nLow, pSKStock.nClose, pSKStock.nSettlePrice, pSKStock.nTickQty, pSKStock.nRef, pSKStock.nBid, pSKStock.nBc, pSKStock.nAsk, pSKStock.nAc, pSKStock.nTradingDay);
        }
        private void buttonSKOOQuoteLib_RequestStocks_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo2.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                dataGridViewStocks.Rows.Clear();

                short psPageNo = short.Parse(textBoxpsPageNo2.Text);
                // ， bstrStockNos 。OnNotifyQuote。
                int nCode = m_pSKOOQuote.SKOOQuoteLib_RequestStocks(psPageNo, textBoxStockNos.Text);

                // 
                string msg = "【SKOOQuoteLib_RequestStocks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSKOOQuoteLib_RequestTicks_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                // 
                short psPageNo = short.Parse(textBoxpsPageNo.Text);
                int nCode = m_pSKOOQuote.SKOOQuoteLib_RequestTicks(psPageNo, textBoxTicks.Text);

                // (、Ticks)
                {
                    //int nCode = m_pSKOOQuote.SKOOQuoteLib_RequestLiveTick(psPageNo, textBoxTicks.Text);
                }
                // 
                string msg = "【SKOOQuoteLib_RequestTicks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSKOOQuoteLib_RequestMarketDepth_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                // (、；)
                short psPageNo = short.Parse(textBoxpsPageNo.Text);
                int nCode = m_pSKOOQuote.SKOOQuoteLib_RequestMarketDepth(psPageNo, textBoxTicks.Text);

                // 
                string msg = "【SKOOQuoteLib_RequestMarketDepth】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
    }
}
