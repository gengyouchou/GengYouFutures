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
    public partial class OSQuoteForm : Form
    {
        // 
        bool isClosing = false;
        string m_UserID;
        // 
        SKCenterLib m_pSKCenter = new SKCenterLib(); // &
        SKOSQuoteLib m_pSKOSQuote = new SKOSQuoteLib(); // 
        public OSQuoteForm(string UserID)
        {
            InitializeComponent();
            m_UserID = UserID;

            //dataGridView
            {
                //dataGridViewTicks
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

                // dataGridViewOnNotifyTicksLONG
                {
                    dataGridViewOnNotifyTicksLONG.Columns.Add("Column1", "");
                    dataGridViewOnNotifyTicksLONG.Columns.Add("Column2", "：：");
                    dataGridViewOnNotifyTicksLONG.Columns.Add("Column3", "");
                    dataGridViewOnNotifyTicksLONG.Columns.Add("Column4", "");

                    for (int i = 0; i < 1; i++)
                        dataGridViewOnNotifyTicksLONG.Rows.Add();
                }
                //dataGridViewKLine
                {
                    dataGridViewKLine.Columns.Add("Column1", "()");
                    dataGridViewKLine.Columns.Add("Column2", "");
                    dataGridViewKLine.Columns.Add("Column3", "");
                    dataGridViewKLine.Columns.Add("Column4", "");
                    dataGridViewKLine.Columns.Add("Column5", "");
                    dataGridViewKLine.Columns.Add("Column6", "()/");
                }
                // SKFOREIGNLONG
                {
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
                    // dataGridViewSKOSQuoteLib_GetStockByNoLONG
                    {
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column1", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column2", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column3", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column4", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column5", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column6", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column7", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column8", "CallPut");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column9", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column10", "");

                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column11", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column12", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column13", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column14", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column15", "()");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column16", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column17", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column18", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column19", "");
                        dataGridViewSKOSQuoteLib_GetStockByNoLONG.Columns.Add("Column20", "");
                    }
                }
            }
            //comboBox
            {
                //comboBoxsKLineType
                {
                    comboBoxsKLineType.Items.Add("");
                    comboBoxsKLineType.Items.Add("");
                    comboBoxsKLineType.Items.Add("");
                    comboBoxsKLineType.Items.Add("");
                }
            }
        }
        private void OSQuoteForm_Load(object sender, EventArgs e)
        {
            labelUserID.Text = m_UserID;
            
            //()
            m_pSKOSQuote.OnNotifyBest5NineDigitLONG += new _ISKOSQuoteLibEvents_OnNotifyBest5NineDigitLONGEventHandler(OnNotifyBest5NineDigitLONG);
            void OnNotifyBest5NineDigitLONG(int nStockidx, long nBestBid1, int nBestBidQty1, long nBestBid2, int nBestBidQty2, long nBestBid3, int nBestBidQty3, long nBestBid4, int nBestBidQty4, long nBestBid5, int nBestBidQty5, long nBestAsk1, int nBestAskQty1, long nBestAsk2, int nBestAskQty2, long nBestAsk3, int nBestAskQty3, long nBestAsk4, int nBestAskQty4, long nBestAsk5, int nBestAskQty5)
            {
                if (isClosing != true)
                {
                    SKBEST5_9 pSKBest5 = new SKBEST5_9();
                    int nCode = m_pSKOSQuote.SKOSQuoteLib_GetBest5NineDigitLONG(nStockidx, ref pSKBest5);

                    // 
                    string msg = "【SKOSQuoteLib_GetBest5NineDigitLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
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
            m_pSKOSQuote.OnNotifyBest10NineDigitLONG += new _ISKOSQuoteLibEvents_OnNotifyBest10NineDigitLONGEventHandler(OnNotifyBest10NineDigitLONG);
            void OnNotifyBest10NineDigitLONG(int nStockIdx, long nBestBid1, int nBestBidQty1, long nBestBid2, int nBestBidQty2, long nBestBid3, int nBestBidQty3, long nBestBid4, int nBestBidQty4, long nBestBid5, int nBestBidQty5, long nBestBid6, int nBestBidQty6, long nBestBid7, int nBestBidQty7, long nBestBid8, int nBestBidQty8, long nBestBid9, int nBestBidQty9, long nBestBid10, int nBestBidQty10, long nBestAsk1, int nBestAskQty1, long nBestAsk2, int nBestAskQty2, long nBestAsk3, int nBestAskQty3, long nBestAsk4, int nBestAskQty4, long nBestAsk5, int nBestAskQty5, long nBestAsk6, int nBestAskQty6, long nBestAsk7, int nBestAskQty7, long nBestAsk8, int nBestAskQty8, long nBestAsk9, int nBestAskQty9, long nBestAsk10, int nBestAskQty10)
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

            //(LONG index)，
            m_pSKOSQuote.OnNotifyTicksNineDigitLONG += new _ISKOSQuoteLibEvents_OnNotifyTicksNineDigitLONGEventHandler(OnNotifyTicksNineDigitLONG);
            void OnNotifyTicksNineDigitLONG(int nIndex, int nPtr, int nDate, int nTime, long nClose, int nQty)
            {
                if (isClosing != true)
                {
                    SKFOREIGNTICK_9 pSKTick = new SKFOREIGNTICK_9();
                    int nCode = m_pSKOSQuote.SKOSQuoteLib_GetTickNineDigitLONG(nIndex, nPtr, ref pSKTick);

                    // 
                    string msg = "【SKOSQuoteLib_GetTickNineDigitLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
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
            //m_pSKOSQuote.OnNotifyHistoryTicksNineDigitLONG += new _ISKOSQuoteLibEvents_OnNotifyHistoryTicksNineDigitLONGEventHandler(OnNotifyHistoryTicksNineDigitLONG);
            void OnNotifyHistoryTicksNineDigitLONG(int nIndex, int nPtr, int nDate, int nTime, long nClose, int nQty)
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
            //K
            m_pSKOSQuote.OnKLineData += new _ISKOSQuoteLibEvents_OnKLineDataEventHandler(OnKLineData);
            void OnKLineData(string bstrStockNo, string bstrData)
            {
                // [()], [],[],[],[],[ ()   ]
                string[] values = new string[6];
                values = bstrData.Split(',');
                dataGridViewKLine.Rows.Add(values[0], values[1], values[2], values[3], values[4], values[5]);
            }
            //
            m_pSKOSQuote.OnNotifyQuoteLONG += new _ISKOSQuoteLibEvents_OnNotifyQuoteLONGEventHandler(OnNotifyQuoteLONG);
            void OnNotifyQuoteLONG(int nIndex)
            {
                if (isClosing != true)
                {
                    bool stockFound = false;

                    // (LONG index)，

                    SKFOREIGNLONG pSKStock = new SKFOREIGNLONG();
                    int nCode = m_pSKOSQuote.SKOSQuoteLib_GetStockByIndexLONG(nIndex, ref pSKStock);

                    // (LONG index)(CME)，。
                    {
                        // SKFOREIGN_9LONG pSKStock = new SKFOREIGN_9LONG();
                        // int nCode = m_pSKOSQuote.SKOSQuoteLib_GetStockByIndexNineDigitLONG(nIndex, ref pSKStock);
                    }

                    // 
                    string msg = "【SKQuoteLib_GetStockByIndexLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
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
            //
            m_pSKOSQuote.OnConnect += new _ISKOSQuoteLibEvents_OnConnectEventHandler(OnConnect);
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
            m_pSKOSQuote.OnOverseaProducts += new _ISKOSQuoteLibEvents_OnOverseaProductsEventHandler(OnOverseaProducts);
            void OnOverseaProducts(string bstrValue)
            {
                //
               string msg = bstrValue;
                richTextBoxOnOverseaProducts.AppendText(msg + "\n");
            }

            //()
            m_pSKOSQuote.OnOverseaProductsDetail += new _ISKOSQuoteLibEvents_OnOverseaProductsDetailEventHandler(OnOverseaProductsDetail);
            void OnOverseaProductsDetail(string bstrValue)
            {
                //
               string msg = bstrValue;
                richTextBoxOnOverseaProductsDetail.AppendText(msg + "\n");
            }
        }

        private void buttonSKOSQuoteLib_EnterMonitorLONG_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOSQuote.SKOSQuoteLib_EnterMonitorLONG();
            // 
            string msg = "【SKOSQuoteLib_EnterMonitorLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_RequestOverseaProducts_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOSQuote.SKOSQuoteLib_RequestOverseaProducts();
            // 
            string msg = "【SKOSQuoteLib_RequestOverseaProducts】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_GetOverseaProductDetail_Click(object sender, EventArgs e)
        {
            short sType = 1;
            // ()
            int nCode = m_pSKOSQuote.SKOSQuoteLib_GetOverseaProductDetail(sType);
            // 
            string msg = "【SKOSQuoteLib_GetOverseaProductDetail】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_LeaveMonitor_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOSQuote.SKOSQuoteLib_LeaveMonitor();
            // 
            string msg = "【SKOSQuoteLib_LeaveMonitor】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }

        short sServer = 0; // 0：  1：
        private void buttonSKOSQuoteLib_SetOSQuoteServer_Click(object sender, EventArgs e)
        {
            if (sServer == 0) sServer = 1;
            else sServer = 0;
            // 、
            int nCode = m_pSKOSQuote.SKOSQuoteLib_SetOSQuoteServer(sServer);
            // 
            string msg = "【SKOSQuoteLib_SetOSQuoteServer】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
            // ...
            if (sServer == 0) msg = "";
            else msg = "";
            msg = "、:" + msg;
            richTextBoxMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_Initialize_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOSQuote.SKOSQuoteLib_Initialize();
            // 
            string msg = "【SKOSQuoteLib_Initialize】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_IsConnected_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKOSQuote.SKOSQuoteLib_IsConnected();
            // 
            string msg;
            if (nCode == 1) msg = "";
            else msg = "";          
            richTextBoxMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_GetQuoteStatus_Click(object sender, EventArgs e)
        {
            int pnConnectionCount = 0; // pbIsOutLimittrue,；pbIsOutLimitfalse, ()

            bool pbIsOutLimit = false; // ，pbIsOutLimit false，，
            // (,)
            int nCode = m_pSKOSQuote.SKOSQuoteLib_GetQuoteStatus(ref pnConnectionCount, ref pbIsOutLimit);
            // 
            string msg = "【SKOSQuoteLib_GetQuoteStatus】" + ":" + pnConnectionCount + ":" + pbIsOutLimit;
            richTextBoxMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_RequestTicks_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                // 
                short psPageNo = short.Parse(textBoxpsPageNo.Text);
                int nCode = m_pSKOSQuote.SKOSQuoteLib_RequestTicks(psPageNo, textBoxTicks.Text);

                // 
                string msg = "【SKOSQuoteLib_RequestTicks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }

        private void buttonSearchKeyword_Click(object sender, EventArgs e)
        {
            // 
            richTextBoxOnOverseaProductsDetail.SelectAll();
            // （）
            richTextBoxOnOverseaProductsDetail.SelectionBackColor = richTextBoxOnOverseaProductsDetail.BackColor;
            // 
            richTextBoxOnOverseaProductsDetail.DeselectAll();
            int index = 0;
            while (index < richTextBoxOnOverseaProductsDetail.Text.Length)
            {
                // 
                index = richTextBoxOnOverseaProductsDetail.Find(TextBoxSearchKeyword.Text, index, richTextBoxOnOverseaProductsDetail.TextLength, RichTextBoxFinds.None);

                if (index != -1)
                {
                    // ，
                    richTextBoxOnOverseaProductsDetail.Select(index, TextBoxSearchKeyword.Text.Length);

                    // 
                    richTextBoxOnOverseaProductsDetail.SelectionBackColor = Color.Yellow;

                    // 
                    index += TextBoxSearchKeyword.Text.Length;
                }
                else
                {
                    break;
                }
            }
        }

        private void comboBoxsKLineType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxsKLineType.SelectedItem != null)
            {
                buttonSKOSQuoteLib_RequestKLineByDate.Enabled = true;
                textBoxsMinuteNumber.Enabled = true;
            }
        }

        private void buttonSKOSQuoteLib_RequestKLineByDate_Click(object sender, EventArgs e)
        {
            dataGridViewKLine.Rows.Clear();

            short sKLineType = 0;
            string bstrStartDate = textBoxbstrStartDate.Text;
            string bstrEndDate = textBoxbstrEndDate.Text;
            short sMinuteNumber = short.Parse(textBoxsMinuteNumber.Text);

            string selectValue = comboBoxsKLineType.SelectedItem.ToString();
            if (selectValue == "") sKLineType = 0;
            else if (selectValue == "") sKLineType = 1;
            else if (selectValue == "") sKLineType = 2;
            else if (selectValue == "") sKLineType = 3;

            // K，，KK

            int nCode = m_pSKOSQuote.SKOSQuoteLib_RequestKLineByDate(textBoxbstrStockNo.Text, sKLineType, bstrStartDate, bstrEndDate, sMinuteNumber);

            // K，
            {
                //int nCode = m_pSKOSQuote.SKOSQuoteLib_RequestKLine(textBoxbstrStockNo.Text, sKLineType);
            }

            // 
            string msg = "【SKOSQuoteLib_RequestKLineByDate】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }

        private void buttonSKOSQuoteLib_RequestStocks_Click(object sender, EventArgs e)
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
                int nCode = m_pSKOSQuote.SKOSQuoteLib_RequestStocks(psPageNo, textBoxStockNos.Text);

                // 
                string msg = "【SKOSQuoteLib_RequestStocks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }

        private void buttonSKOSQuoteLib_GetStockByNoNineDigitLONG_Click(object sender, EventArgs e)
        {
            dataGridViewSKOSQuoteLib_GetStockByNoLONG.Rows.Clear();

            SKFOREIGN_9LONG pSKStock = new SKFOREIGN_9LONG();
            string bstrStockNo = textBoxSKOSQuoteLib_GetStockByNoLONG.Text;
            // (LONG index)(CME)，。
            int nCode = m_pSKOSQuote.SKOSQuoteLib_GetStockByNoNineDigitLONG(bstrStockNo, ref pSKStock);

            // 
            string msg = "【SKOSQuoteLib_GetStockByNoNineDigitLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");

            // SKFOREIGNLONG
            bool found = false; // 
            string[] values = bstrStockNo.Split(','); // 
            foreach (DataGridViewRow row in dataGridViewSKOSQuoteLib_GetStockByNoLONG.Rows)
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
                dataGridViewSKOSQuoteLib_GetStockByNoLONG.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.sDecimal, pSKStock.nDenominator, pSKStock.bstrMarketNo, pSKStock.bstrExchangeNo, pSKStock.bstrExchangeName, pSKStock.bstrCallPut, pSKStock.nOpen, pSKStock.nHigh, pSKStock.nLow, pSKStock.nClose, pSKStock.nSettlePrice, pSKStock.nTickQty, pSKStock.nRef, pSKStock.nBid, pSKStock.nBc, pSKStock.nAsk, pSKStock.nAc, pSKStock.nTradingDay);
        }

        private void buttonSKOSQuoteLib_GetStockByNoLONG_Click(object sender, EventArgs e)
        {
            dataGridViewSKOSQuoteLib_GetStockByNoLONG.Rows.Clear();

            SKFOREIGNLONG pSKStock = new SKFOREIGNLONG();
            string bstrStockNo = textBoxSKOSQuoteLib_GetStockByNoLONG.Text;
            // (LONG index)，。
            int nCode = m_pSKOSQuote.SKOSQuoteLib_GetStockByNoLONG(bstrStockNo, ref pSKStock);

            // 
            string msg = "【SKOSQuoteLib_GetStockByNoLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");

            // SKFOREIGNLONG
            bool found = false; // 
            string[] values = bstrStockNo.Split(','); //  
            foreach (DataGridViewRow row in dataGridViewSKOSQuoteLib_GetStockByNoLONG.Rows)
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
                dataGridViewSKOSQuoteLib_GetStockByNoLONG.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.sDecimal, pSKStock.nDenominator, pSKStock.bstrMarketNo, pSKStock.bstrExchangeNo, pSKStock.bstrExchangeName, pSKStock.bstrCallPut, pSKStock.nOpen, pSKStock.nHigh, pSKStock.nLow, pSKStock.nClose, pSKStock.nSettlePrice, pSKStock.nTickQty, pSKStock.nRef, pSKStock.nBid, pSKStock.nBc, pSKStock.nAsk, pSKStock.nAc, pSKStock.nTradingDay);
        }
        private void buttonSKOSQuoteLib_RequestMarketDepth_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                // (、；)
                short psPageNo = short.Parse(textBoxpsPageNo.Text);
                int nCode = m_pSKOSQuote.SKOSQuoteLib_RequestMarketDepth(psPageNo, textBoxTicks.Text);

                // 
                string msg = "【SKOSQuoteLib_RequestMarketDepth】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }

        private void OSQuoteForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isClosing = true;
            // 
            m_pSKOSQuote.SKOSQuoteLib_LeaveMonitor();
        }
    }
}
