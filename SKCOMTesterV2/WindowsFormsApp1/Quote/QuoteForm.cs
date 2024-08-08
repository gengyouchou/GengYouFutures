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
    public partial class QuoteForm : Form
    {
        // 
        bool isClosing = false;
        string m_UserID;
        // 
        SKCenterLib m_pSKCenter = new SKCenterLib(); // &
        SKQuoteLib m_pSKQuote = new SKQuoteLib(); // 
        public QuoteForm(string UserID)
        {
            //Init
            {
                InitializeComponent();
                m_UserID = UserID;
                //dataGridView
                {
                    // Ticks
                    {
                        dataGridViewTicks.Columns.Add("Column1", "");
                        dataGridViewTicks.Columns.Add("Column2", "");
                        dataGridViewTicks.Columns.Add("Column3", "");
                        dataGridViewTicks.Columns.Add("Column4", "");

                        for (int i = 0; i < 5; i++)
                            dataGridViewTicks.Rows.Add();
                    }
                    // dataGridViewOnNotifyTicksLONG
                    {
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column1", "");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column2", "：：");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column3", "’");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column4", "");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column5", "");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column6", "");
                        dataGridViewOnNotifyTicksLONG.Columns.Add("Column7", "");

                        for (int i = 0; i < 1; i++)
                            dataGridViewOnNotifyTicksLONG.Rows.Add();
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
                        dataGridViewStocks.Columns.Add("Column8", "");
                        dataGridViewStocks.Columns.Add("Column9", "()");
                        dataGridViewStocks.Columns.Add("Column10", "()");
                        dataGridViewStocks.Columns.Add("Column11", "");
                        dataGridViewStocks.Columns.Add("Column12", "()");
                        dataGridViewStocks.Columns.Add("Column13", "");
                        dataGridViewStocks.Columns.Add("Column14", "");
                        dataGridViewStocks.Columns.Add("Column15", "");
                    }
                    // dataGridViewKLine
                    {
                        dataGridViewKLine.Columns.Add("Column1", "// :");
                        dataGridViewKLine.Columns.Add("Column2", "");
                        dataGridViewKLine.Columns.Add("Column3", "");
                        dataGridViewKLine.Columns.Add("Column4", "");
                        dataGridViewKLine.Columns.Add("Column5", "");
                        dataGridViewKLine.Columns.Add("Column6", "");
                    }
                    // dataGridViewOnNotifyCommodityListWithTypeNo
                    {
                        dataGridViewOnNotifyCommodityListWithTypeNo.Columns.Add("Column1", "");
                        dataGridViewOnNotifyCommodityListWithTypeNo.Columns.Add("Column2", "");
                        dataGridViewOnNotifyCommodityListWithTypeNo.Columns.Add("Column3", "()");
                        dataGridViewOnNotifyCommodityListWithTypeNo.Columns.Add("Column4", "()");
                    }
                    // OnNotifyFutureTradeInfoLONG
                    {
                        dataGridViewOnNotifyFutureTradeInfoLONG.Columns.Add("Column1", "");
                        dataGridViewOnNotifyFutureTradeInfoLONG.Columns.Add("Column2", "");
                        dataGridViewOnNotifyFutureTradeInfoLONG.Columns.Add("Column3", "");
                        dataGridViewOnNotifyFutureTradeInfoLONG.Columns.Add("Column4", "");
                        dataGridViewOnNotifyFutureTradeInfoLONG.Columns.Add("Column5", "");
                        dataGridViewOnNotifyFutureTradeInfoLONG.Columns.Add("Column6", "");
                        for (int i = 0; i < 1; i++)
                        {
                            dataGridViewOnNotifyFutureTradeInfoLONG.Rows.Add();
                        }
                    }
                    //dataGridViewOnNotifyStrikePrices
                    {
                        {
                            dataGridViewOnNotifyStrikePrices.Columns.Add("Column1", "");
                            dataGridViewOnNotifyStrikePrices.Columns.Add("Column2", "");
                            dataGridViewOnNotifyStrikePrices.Columns.Add("Column3", "Call ");
                            dataGridViewOnNotifyStrikePrices.Columns.Add("Column4", "Put ");
                            dataGridViewOnNotifyStrikePrices.Columns.Add("Column5", "");
                            dataGridViewOnNotifyStrikePrices.Columns.Add("Column6", "＋");
                            dataGridViewOnNotifyStrikePrices.Columns.Add("Column7", "");
                        }
                    }
                    //dataGridViewOnNotifyOddLotSpreadDeal
                    {
                        {
                            dataGridViewOnNotifyOddLotSpreadDeal.Columns.Add("Column1", "");
                            dataGridViewOnNotifyOddLotSpreadDeal.Columns.Add("Column2", "");
                            dataGridViewOnNotifyOddLotSpreadDeal.Columns.Add("Column3", "(-)");
                        }
                    }
                    // dataGridViewSKQuoteLib_GetStockByNoLONG
                    {
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column1", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column2", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column3", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column4", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column5", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column6", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column7", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column8", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column9", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column10", "、");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column11", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column12", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column13", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column14", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column15", "()");

                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column16", "()");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column17", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column18", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column19", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column20", "");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column21", " 0: 1:()");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column22", "[] 0: 1:2:");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column23", "(YYYYMMDD)");
                        dataGridViewSKQuoteLib_GetStockByNoLONG.Columns.Add("Column24", "[] 、   0: ; 1:");
                    }
                }
                //comboBox
                {
                    // KLine
                    {
                        //comboBoxsKLineType
                        {
                            comboBoxsKLineType.Items.Add("");
                            comboBoxsKLineType.Items.Add("");
                            comboBoxsKLineType.Items.Add("");
                            comboBoxsKLineType.Items.Add("");
                        }

                        //comboBoxsOutType
                        {
                            comboBoxsOutType.Items.Add("");
                            comboBoxsOutType.Items.Add("");
                        }

                        //sTradeSession
                        {
                            comboBoxsTradeSession.Items.Add("");
                            comboBoxsTradeSession.Items.Add("AM");
                        }
                    }
                    
                    //comboBoxSKQuoteLib_RequestStockList
                    {
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("-");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("-");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("");
                        comboBoxSKQuoteLib_RequestStockList.Items.Add("");
                    }
                    //comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo
                    {
                        comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo.Items.Add("-(5)");
                        comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo.Items.Add("-(6)");
                        comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo.Items.Add("-9");
                        comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo.Items.Add("-10");
                    }
                    //comboBoxSKQuoteLib_RequestStocksWithMarketNo
                    {
                        comboBoxSKQuoteLib_RequestStocksWithMarketNo.Items.Add("-(5)");
                        comboBoxSKQuoteLib_RequestStocksWithMarketNo.Items.Add("-(6)");
                        comboBoxSKQuoteLib_RequestStocksWithMarketNo.Items.Add("-9");
                        comboBoxSKQuoteLib_RequestStocksWithMarketNo.Items.Add("-10");
                    }
                    // comboBoxnCallPut
                    {
                        comboBoxnCallPut.Items.Add("Call");
                        comboBoxnCallPut.Items.Add("Put");
                    }
                }
            }
        }
        private void QuoteForm_Load(object sender, EventArgs e)
        {
            labelUserID.Text = m_UserID;

            //
            {
                m_pSKQuote.OnNotifyServerTime += new _ISKQuoteLibEvents_OnNotifyServerTimeEventHandler(OnNotifyServerTime);
                void OnNotifyServerTime(short sHour, short sMinute, short sSecond, int nTotal)
                {
                    if (isClosing != true)
                        labelOnNotifyServerTime2.Text = sHour + ":" + sMinute + ":" + sSecond + " Total:" + nTotal;
                }
            }
            // －
            {
                m_pSKQuote.OnNotifyOddLotSpreadDeal += new _ISKQuoteLibEvents_OnNotifyOddLotSpreadDealEventHandler(OnNotifyOddLotSpreadDeal);
                void OnNotifyOddLotSpreadDeal(short sMarketNo, string bstrStockNo, int nDealPrice, short sDigit)
                {
                    if (isClosing != true)
                    {
                        double dResult = (double)nDealPrice;
                        while (sDigit-- != 0)
                        {
                            dResult /= 10.0;
                        }

                        bool found = false; // 
                        foreach (DataGridViewRow row in dataGridViewOnNotifyOddLotSpreadDeal.Rows)
                        {
                            if (bstrStockNo == row.Cells[1].Value.ToString()) // 
                            {
                                row.Cells[2].Value = nDealPrice;
                                found = true;
                                break;
                            }
                        }
                        if (found == false)
                            dataGridViewOnNotifyOddLotSpreadDeal.Rows.Add(sMarketNo, bstrStockNo, dResult);
                    }
                }
            }
            //，
            {
                m_pSKQuote.OnConnection += new _ISKQuoteLibEvents_OnConnectionEventHandler(OnConnection);
                void OnConnection(int nKind, int nCode)
                {
                    if (isClosing != true)
                    {
                        // 
                        string msg = "【OnConnection】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nKind);
                        richTextBoxMessage.AppendText(msg + "\n");
                    }
                }
            }
            //()
            {
                m_pSKQuote.OnNotifyBest5LONG += new _ISKQuoteLibEvents_OnNotifyBest5LONGEventHandler(OnNotifyBest5LONG);
                void OnNotifyBest5LONG(short sMarketNo, int nStockidx, int nBestBid1, int nBestBidQty1, int nBestBid2, int nBestBidQty2, int nBestBid3, int nBestBidQty3, int nBestBid4, int nBestBidQty4, int nBestBid5, int nBestBidQty5, int nExtendBid, int nExtendBidQty, int nBestAsk1, int nBestAskQty1, int nBestAsk2, int nBestAskQty2, int nBestAsk3, int nBestAskQty3, int nBestAsk4, int nBestAskQty4, int nBestAsk5, int nBestAskQty5, int nExtendAsk, int nExtendAskQty, int nSimulate)
                {
                    if (isClosing != true)
                    {
                        if (nSimulate == 0) labelnSimulate.Text = "";
                        else if (nSimulate == 1) labelnSimulate.Text = "";

                        if (dataGridViewTicks.Rows.Count > 0)
                        {
                            DataGridViewRow Row1 = dataGridViewTicks.Rows[0];
                            Row1.Cells[0].Value = nBestBidQty1; //
                            Row1.Cells[1].Value = nBestBid1 / 100.0; //
                            Row1.Cells[2].Value = nBestAsk1 / 100.0; //
                            Row1.Cells[3].Value = nBestAskQty1; //

                            DataGridViewRow Row2 = dataGridViewTicks.Rows[1];
                            Row2.Cells[0].Value = nBestBidQty2; //
                            Row2.Cells[1].Value = nBestBid2 / 100.0; //
                            Row2.Cells[2].Value = nBestAsk2 / 100.0; //
                            Row2.Cells[3].Value = nBestAskQty2; //

                            DataGridViewRow Row3 = dataGridViewTicks.Rows[2];
                            Row3.Cells[0].Value = nBestBidQty3; //
                            Row3.Cells[1].Value = nBestBid3 / 100.0; //
                            Row3.Cells[2].Value = nBestAsk3 / 100.0; //
                            Row3.Cells[3].Value = nBestAskQty3; //

                            DataGridViewRow Row4 = dataGridViewTicks.Rows[3];
                            Row4.Cells[0].Value = nBestBidQty4; //
                            Row4.Cells[1].Value = nBestBid4 / 100.0; //
                            Row4.Cells[2].Value = nBestAsk4 / 100.0; //
                            Row4.Cells[3].Value = nBestAskQty4; //

                            DataGridViewRow Row5 = dataGridViewTicks.Rows[4];
                            Row5.Cells[0].Value = nBestBidQty5; //
                            Row5.Cells[1].Value = nBestBid5 / 100.0; //
                            Row5.Cells[2].Value = nBestAsk5 / 100.0; //
                            Row5.Cells[3].Value = nBestAskQty5; //
                        }
                    }
                }
            }
            //(LONG index)，
            {
                m_pSKQuote.OnNotifyTicksLONG += new _ISKQuoteLibEvents_OnNotifyTicksLONGEventHandler(OnNotifyTicksLONG);
                void OnNotifyTicksLONG(short sMarketNo, int nIndex, int nPtr, int nDate, int nTimehms, int nTimemillismicros, int nBid, int nAsk, int nClose, int nQty, int nSimulate)
                {
                    if (isClosing != true)
                    {
                        if (dataGridViewOnNotifyTicksLONG.Rows.Count > 0)
                        {
                            DataGridViewRow Row1 = dataGridViewOnNotifyTicksLONG.Rows[0];
                            Row1.Cells[0].Value = nDate; // 。(YYYYMMDD)
                            Row1.Cells[1].Value = nTimehms; // 1。(：：)
                            Row1.Cells[2].Value = nTimemillismicros; // 2。(‘")
                            Row1.Cells[3].Value = nBid / 100.0; // 
                            Row1.Cells[4].Value = nAsk / 100.0; // 
                            Row1.Cells[5].Value = nClose / 100.0; // 
                            Row1.Cells[6].Value = nQty / 100.0; // 
                        }
                    }
                }
            }
            //(LONG index)，Tick 【】
            {
                //m_pSKQuote.OnNotifyHistoryTicksLONG += new _ISKQuoteLibEvents_OnNotifyHistoryTicksLONGEventHandler(OnNotifyHistoryTicksLONG);
                void OnNotifyHistoryTicksLONG(short sMarketNo, int nIndex, int nPtr, int nDate, int nTimehms, int nTimemillismicros, int nBid, int nAsk, int nClose, int nQty, int nSimulate)
                {
                    if (isClosing != true)
                    {
                        if (dataGridViewOnNotifyTicksLONG.Rows.Count > 0)
                        {
                            DataGridViewRow Row1 = dataGridViewOnNotifyTicksLONG.Rows[0];
                            Row1.Cells[0].Value = nDate; // 。(YYYYMMDD)
                            Row1.Cells[1].Value = nTimehms; // 1。(：：)
                            Row1.Cells[2].Value = nTimemillismicros; // 2。(‘")
                            Row1.Cells[3].Value = nBid / 100.0; // 
                            Row1.Cells[4].Value = nAsk / 100.0; // 
                            Row1.Cells[5].Value = nClose / 100.0; // 
                            Row1.Cells[6].Value = nQty / 100.0; // 
                        }
                    }
                }
            }
            //
            {
                m_pSKQuote.OnNotifyQuoteLONG += new _ISKQuoteLibEvents_OnNotifyQuoteLONGEventHandler(OnNotifyQuoteLONG);
                void OnNotifyQuoteLONG(short sMarketNo, int nIndex)
                {
                    if (isClosing != true)
                    {
                        bool stockFound = false;
                        SKSTOCKLONG pSKStock = new SKSTOCKLONG();
                        int nCode = m_pSKQuote.SKQuoteLib_GetStockByIndexLONG(sMarketNo, nIndex, ref pSKStock);
                        
                        foreach (DataGridViewRow row in dataGridViewStocks.Rows)
                        {
                            if (row.Cells[0].Value.ToString() == pSKStock.bstrStockNo)
                            {
                                row.Cells[2].Value = pSKStock.nOpen / 100.0;
                                row.Cells[3].Value = pSKStock.nClose / 100.0;
                                row.Cells[4].Value = pSKStock.nHigh / 100.0;
                                row.Cells[5].Value = pSKStock.nLow / 100.0;
                                row.Cells[6].Value = pSKStock.nUp / 100.0;
                                row.Cells[7].Value = pSKStock.nDown / 100.0;
                                row.Cells[8].Value = pSKStock.nTBc.ToString();
                                row.Cells[9].Value = pSKStock.nTAc;
                                row.Cells[10].Value = pSKStock.nTQty;
                                row.Cells[11].Value = pSKStock.nRef / 100.0;
                                row.Cells[12].Value = pSKStock.nYQty;

                                if (pSKStock.nBid == m_pSKQuote.SKQuoteLib_GetMarketPriceTS()) // () 
                                {
                                    row.Cells[13].Value = "";
                                }
                                else
                                {
                                    row.Cells[13].Value = pSKStock.nBid / 100.0;
                                }

                                if (pSKStock.nBid == m_pSKQuote.SKQuoteLib_GetMarketPriceTS()) // () 
                                {
                                    row.Cells[14].Value = "";
                                }
                                else
                                {
                                    row.Cells[14].Value = pSKStock.nAsk / 100.0;
                                }
                                stockFound = true;
                                break;
                            }
                        }

                        if (stockFound == false)
                        {
                            if (pSKStock.nBid == m_pSKQuote.SKQuoteLib_GetMarketPriceTS() && pSKStock.nAsk == m_pSKQuote.SKQuoteLib_GetMarketPriceTS())
                            {
                                dataGridViewStocks.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.nOpen / 100.0, pSKStock.nClose / 100.0, pSKStock.nHigh / 100.0, pSKStock.nLow / 100.0, pSKStock.nUp / 100.0, pSKStock.nDown / 100.0, pSKStock.nTBc.ToString(), pSKStock.nTAc, pSKStock.nTQty, pSKStock.nRef / 100.0, pSKStock.nYQty, "", "");
                            }
                            else if (pSKStock.nBid == m_pSKQuote.SKQuoteLib_GetMarketPriceTS() && pSKStock.nAsk != m_pSKQuote.SKQuoteLib_GetMarketPriceTS())
                            {
                                dataGridViewStocks.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.nOpen / 100.0, pSKStock.nClose / 100.0, pSKStock.nHigh / 100.0, pSKStock.nLow / 100.0, pSKStock.nUp / 100.0, pSKStock.nDown / 100.0, pSKStock.nTBc.ToString(), pSKStock.nTAc, pSKStock.nTQty, pSKStock.nRef / 100.0, pSKStock.nYQty, "", pSKStock.nAsk / 100.0);

                            }
                            else if (pSKStock.nBid != m_pSKQuote.SKQuoteLib_GetMarketPriceTS() && pSKStock.nAsk == m_pSKQuote.SKQuoteLib_GetMarketPriceTS())
                            {
                                dataGridViewStocks.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.nOpen / 100.0, pSKStock.nClose / 100.0, pSKStock.nHigh / 100.0, pSKStock.nLow / 100.0, pSKStock.nUp / 100.0, pSKStock.nDown / 100.0, pSKStock.nTBc.ToString(), pSKStock.nTAc, pSKStock.nTQty, pSKStock.nRef / 100.0, pSKStock.nYQty, pSKStock.nBid / 100.0, "");
                            }
                            else
                            {
                                dataGridViewStocks.Rows.Add(pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.nOpen / 100.0, pSKStock.nClose / 100.0, pSKStock.nHigh / 100.0, pSKStock.nLow / 100.0, pSKStock.nUp / 100.0, pSKStock.nDown / 100.0, pSKStock.nTBc.ToString(), pSKStock.nTAc, pSKStock.nTQty, pSKStock.nRef / 100.0, pSKStock.nYQty, pSKStock.nBid / 100.0, pSKStock.nAsk / 100.0);

                            }

                        }
                    }
                }
            }
            // SKQuoteLib_GetMarketBuySellUpDown ，
            {
                m_pSKQuote.OnNotifyMarketTot += new _ISKQuoteLibEvents_OnNotifyMarketTotEventHandler(OnNotifyMarketTot);
                void OnNotifyMarketTot(short sMarketNo, short sPtr, int nTime, int nTotv, int nTots, int nTotc)
                {
                    if (isClosing != true)
                    {
                        if (sMarketNo == 0) // 
                        {
                            labelMarketTotPtr.Text = "" + sPtr + "";
                            labelMarketTotTime.Text = ":" + nTime / 10000 + "" + (nTime % 10000) / 100 + "" + nTime % 100 + "";
                            labelnTotv.Text = "():" + (nTotv / 100.00);
                            labelnTots.Text = ":" + nTots;
                            labelnTotc.Text = ":" + nTotc;
                        }
                        else // 2
                        {
                            labelMarketTotPtr2.Text = "" + sPtr + "";
                            labelMarketTotTime2.Text = ":" + nTime / 10000 + "" + (nTime % 10000) / 100 + "" + nTime % 100 + "";
                            labelnTotv2.Text = "():" + (nTotv / 100.00);
                            labelnTots2.Text = ":" + nTots;
                            labelnTotc2.Text = ":" + nTotc;
                        }
                    }
                }
            }
            // SKQuoteLib_GetMarketBuySellUpDown ，
            {
                m_pSKQuote.OnNotifyMarketBuySell += new _ISKQuoteLibEvents_OnNotifyMarketBuySellEventHandler(OnNotifyMarketBuySell);
                void OnNotifyMarketBuySell(short sMarketNo, short sPtr, int nTime, int nBc, int nSc, int nBs, int nSs)
                {
                    if (isClosing != true)
                    {
                        //labelMarketBuySellPtr.Text = "" + sPtr + "";
                        //labelMarketBuySellTime.Text = ":" + nTime / 10000 + "" + (nTime % 10000) / 100 + "" + nTime % 100 + "";
                        if (sMarketNo == 0) // 
                        {
                            labelnBs.Text = ":" + nBs;
                            labelnSs.Text = ":" + nSs;
                            labelnBc.Text = ":" + nBc;
                            labelnSc.Text = ":" + nSc;
                        }
                        else // 2
                        {
                            labelnBs2.Text = ":" + nBs;
                            labelnSs2.Text = ":" + nSs;
                            labelnBc2.Text = ":" + nBc;
                            labelnSc2.Text = ":" + nSc;
                        }
                    }
                }
            }
            // SKQuoteLib_GetMarketBuySellUpDown ，(『』、 『』)
            {
                m_pSKQuote.OnNotifyMarketHighLowNoWarrant += new _ISKQuoteLibEvents_OnNotifyMarketHighLowNoWarrantEventHandler(OnNotifyMarketHighLowNoWarrant);
                void OnNotifyMarketHighLowNoWarrant(short sMarketNo, int nPtr, int nTime, int nUp, int nDown, int nHigh, int nLow, int nNoChange, int nUpNoW, int nDownNoW, int nHighNoW, int nLowNoW, int nNoChangeNoW)
                {
                    if (isClosing != true)
                    {
                        if (sMarketNo == 0) // 
                        {
                            labelnUp.Text = ":" + nUp;
                            labelnDown.Text = ":" + nDown;
                            labelnHigh.Text = ":" + nHigh;
                            labelnLow.Text = ":" + nLow;
                            labelnNoChange.Text = ":" + nNoChange;

                            labelnUpNoW.Text = "():" + nUpNoW;
                            labelnDownNoW.Text = "():" + nDownNoW;
                            labelnHighNoW.Text = "():" + nHighNoW;
                            labelnLowNoW.Text = "():" + nLowNoW;
                            labelnNoChangeNoW.Text = "():" + nNoChangeNoW;
                        }
                        else // 2
                        {
                            labelnUp2.Text = ":" + nUp;
                            labelnDown2.Text = ":" + nDown;
                            labelnHigh2.Text = ":" + nHigh;
                            labelnLow2.Text = ":" + nLow;
                            labelnNoChange2.Text = ":" + nNoChange;

                            labelnUpNoW2.Text = "():" + nUpNoW;
                            labelnDownNoW2.Text = "():" + nDownNoW;
                            labelnHighNoW2.Text = "():" + nHighNoW;
                            labelnLowNoW2.Text = "():" + nLowNoW;
                            labelnNoChangeNoW2.Text = "():" + nNoChangeNoW;
                        }
                    }
                }
            }
            //(LONG index)－MACD。（－）
            {
                m_pSKQuote.OnNotifyMACDLONG += new _ISKQuoteLibEvents_OnNotifyMACDLONGEventHandler(OnNotifyMACDLONG);
                void OnNotifyMACDLONG(short sMarketNo, int nStockidx, string bstrMACD, string bstrDIF, string bstrOSC)
                {
                    // (LONG index)MACD。()
                    SKMACD pSKMACD = new SKMACD();
                    pSKMACD.bstrStockNo = textBoxbstrStockNo.Text;
                    pSKMACD.bstrMACD = bstrMACD;
                    pSKMACD.bstrDIF = bstrDIF;
                    pSKMACD.bstrOSC = bstrOSC;

                    int nCode = m_pSKQuote.SKQuoteLib_GetMACDLONG(sMarketNo, nStockidx, ref pSKMACD);

                    // 
                    string msg = "【SKQuoteLib_GetMACDLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMessage.AppendText(msg + "\n");

                    richTextBoxMACD.AppendText("【OnNotifyMACDLONG】" + ":" + sMarketNo + "" + nStockidx + "MACD:" + pSKMACD.bstrMACD + "DIF:" + pSKMACD.bstrDIF + "OSC:" + pSKMACD.bstrOSC);
                }
            }
            //(LONG index)－。（－）
            {
                m_pSKQuote.OnNotifyBoolTunelLONG += new _ISKQuoteLibEvents_OnNotifyBoolTunelLONGEventHandler(OnNotifyBoolTunelLONG);
                void OnNotifyBoolTunelLONG(short sMarketNo, int nStockidx, string bstrAVG, string bstrUBT, string bstrLBT)
                {
                    // (LONG index)BoolTunel
                    SKBoolTunel pBoolTunel = new SKBoolTunel();
                    pBoolTunel.bstrStockNo = textBoxbstrStockNo.Text;
                    pBoolTunel.bstrAVG = bstrAVG;
                    pBoolTunel.bstrUBT = bstrUBT;
                    pBoolTunel.bstrLBT = bstrLBT;

                    int nCode = m_pSKQuote.SKQuoteLib_GetBoolTunelLONG(sMarketNo, nStockidx, ref pBoolTunel);

                    // 
                    string msg = "【SKQuoteLib_GetBoolTunelLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMessage.AppendText(msg + "\n");

                    richTextBoxBoolTunel.AppendText("【OnNotifyBoolTunelLONG】" + ":" + sMarketNo + "" + nStockidx + ":" + pBoolTunel.bstrAVG + ":" + pBoolTunel.bstrUBT + ":" + pBoolTunel.bstrLBT);
                }
            }
            //
            {
                m_pSKQuote.OnNotifyKLineData += new _ISKQuoteLibEvents_OnNotifyKLineDataEventHandler(OnNotifyKLineData);
                void OnNotifyKLineData(string bstrStockNo, string bstrData)
                {
                    // ，
                    string[] values = new string[6];
                    values = bstrData.Split(',');
                    dataGridViewKLine.Rows.Add(values[0], values[1], values[2], values[3], values[4], values[5]);

                }
            }
            //－
            {
                m_pSKQuote.OnNotifyCommodityListWithTypeNo += new _ISKQuoteLibEvents_OnNotifyCommodityListWithTypeNoEventHandler(OnNotifyCommodityListWithTypeNo);
                void OnNotifyCommodityListWithTypeNo(short sMarketNo, string bstrStockData)
                {
                    // 
                    string[] valueClass = bstrStockData.Split('%');
                    if (bstrStockData != "##,,;")
                    {
                        dataGridViewOnNotifyCommodityListWithTypeNo.Rows.Add(":" + valueClass[1] + valueClass[2]);
                        bstrStockData = valueClass[3];
                    }

                    //  ';'
                    string[] valuesSemicolon = bstrStockData.Split(';');
                    for (int i = 0; i < valuesSemicolon.Length - 1; i++) // ;，-1
                    {
                        string[] valuesComma = valuesSemicolon[i].Split(',');
                        if (bstrStockData != "##,,;") dataGridViewOnNotifyCommodityListWithTypeNo.Rows.Add(valuesComma[0], valuesComma[1], valuesComma[2], valuesComma[3]);
                    }
                }
            }
            //(LONG index)
            {
                m_pSKQuote.OnNotifyFutureTradeInfoLONG += new _ISKQuoteLibEvents_OnNotifyFutureTradeInfoLONGEventHandler(OnNotifyFutureTradeInfoLONG);
                void OnNotifyFutureTradeInfoLONG(string bstrStockNo, short sMarketNo, int nStockidx, int nBuyTotalCount, int nSellTotalCount, int nBuyTotalQty, int nSellTotalQty, int nBuyDealTotalCount, int nSellDealTotalCount)
                {
                    if (dataGridViewOnNotifyFutureTradeInfoLONG.Rows.Count > 0)
                    {
                        DataGridViewRow Row1 = dataGridViewOnNotifyFutureTradeInfoLONG.Rows[0];
                        Row1.Cells[0].Value = nBuyTotalCount; //
                        Row1.Cells[1].Value = nSellTotalCount; //
                        Row1.Cells[2].Value = nBuyTotalQty; //
                        Row1.Cells[3].Value = nSellTotalQty; //
                        Row1.Cells[4].Value = nBuyDealTotalCount; //
                        Row1.Cells[5].Value = nSellDealTotalCount; //
                    }
                }
            }
            //。 GetStrikePrices ，
            {
                m_pSKQuote.OnNotifyStrikePrices += new _ISKQuoteLibEvents_OnNotifyStrikePricesEventHandler(OnNotifyStrikePrices);
                void OnNotifyStrikePrices(string bstrOptionData)
                {
                    string[] values = bstrOptionData.Split(',');
                    dataGridViewOnNotifyStrikePrices.Rows.Add(values[0], values[1], values[2], values[3], values[4], values[5], values[6]);
                }
            }
        }
        private void QuoteForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isClosing = true;
            // Solace
            m_pSKQuote.SKQuoteLib_LeaveMonitor();
        }
        private void checkBoxSKQuoteLib_RequestTicksWithMarketNos_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxSKQuoteLib_RequestTicksWithMarketNos.Checked)
            {
                comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo.Visible = true;
                buttonSKQuoteLib_RequestTicksWithMarketNo.Visible = true;
                MessageBox.Show("(/),,,,");
            }
            else
            {
                comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo.Visible = false;
                buttonSKQuoteLib_RequestTicksWithMarketNo.Visible = false;
            }
        }
        private void checkBoxSKQuoteLib_RequestStocksWithMarketNo_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxSKQuoteLib_RequestStocksWithMarketNo.Checked)
            {
                comboBoxSKQuoteLib_RequestStocksWithMarketNo.Visible = true;
                buttonSKQuoteLib_RequestStocksWithMarketNo.Visible = true;
                MessageBox.Show("(/),,,,");
            }
            else
            {
                comboBoxSKQuoteLib_RequestStocksWithMarketNo.Visible = false;
                buttonSKQuoteLib_RequestStocksWithMarketNo.Visible = false;
            }
        }
        private void comboBoxSKQuoteLib_RequestStockList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxSKQuoteLib_RequestStockList.Text != null) 
                buttonSKQuoteLib_RequestStockList.Enabled = true;
        }
        private void comboBoxsOutType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxsOutType.SelectedItem != null)
                comboBoxsTradeSession.Enabled = true;
        }
        private void comboBoxsTradeSession_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxsTradeSession.SelectedItem != null)
                buttonSKQuoteLib_RequestKLineAMByDate.Enabled = true;
        }
        private void comboBoxsKLineType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxsKLineType.Text == "")
            {
                textBoxsMinuteNumber.Enabled = true;
            }
            else
            {
                textBoxsMinuteNumber.Enabled = false;
            }

            if (comboBoxsKLineType.SelectedItem != null)
                comboBoxsOutType.Enabled = true;
        }
        private void buttonEnterMonitor_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKQuote.SKQuoteLib_EnterMonitorLONG();
            // 
            string msg = "【SKQuoteLib_EnterMonitorLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_LeaveMonitor_Click(object sender, EventArgs e)
        {
            // Solace
            int nCode = m_pSKQuote.SKQuoteLib_LeaveMonitor();
            // 
            string msg = "【SKQuoteLib_LeaveMonitor】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_IsConnected_Click(object sender, EventArgs e)
        {
            string msg;
            // 
            int nCode = m_pSKQuote.SKQuoteLib_IsConnected();
            // 
            switch (nCode)
            {
                case 0:
                    msg = "";
                    break;
                case 1:
                    msg = "";
                    break;
                case 2:
                    msg = "";
                    break;
                default:
                    msg = "";
                    break;
            }
            msg = "【SKQuoteLib_IsConnected】" + msg;
            richTextBoxMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_GetQuoteStatus_Click(object sender, EventArgs e)
        {
            int pnConnectionCount = 0; //pnConnectionCount ，，
            bool pbIsOutLimit = false; //pbIsOutLimit  false，，

            // (,)
            int nCode = m_pSKQuote.SKQuoteLib_GetQuoteStatus(ref pnConnectionCount, ref pbIsOutLimit);
            // 
            // pbIsOutLimitfalse, ()
            string msg = "【SKQuoteLib_GetQuoteStatus】" + ":" + pnConnectionCount + ":" + pbIsOutLimit;
            richTextBoxMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_RequestStocks_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo2.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            } 
            else
            {
                short psPageNo = short.Parse(textBoxpsPageNo2.Text);
                // ()
                int nCode = m_pSKQuote.SKQuoteLib_RequestStocks(psPageNo, textBoxStockNos.Text);

                // 
                string msg = "【SKQuoteLib_RequestStocks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSKQuoteLib_RequestTicks_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                // 。
                short psPageNo = short.Parse(textBoxpsPageNo.Text);
                int nCode = m_pSKQuote.SKQuoteLib_RequestTicks(psPageNo, textBoxTicks.Text);

                // 
                string msg = "【SKQuoteLib_RequestTicks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSKQuoteLib_Gamma_Click(object sender, EventArgs e)
        {
            if (comboBoxnCallPut.SelectedItem == null || textBoxS.Text =="" || textBoxR.Text == "" || textBoxsigma.Text == "" || textBoxK.Text == "" || textBoxT.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                double Gamma;
                // 1~5 Gamma
                int nCode = m_pSKQuote.SKQuoteLib_Gamma(double.Parse(textBoxS.Text), double.Parse(textBoxK.Text), double.Parse(textBoxR.Text), double.Parse(textBoxT.Text), double.Parse(textBoxsigma.Text), out Gamma);
                labelGamma.Text = "Gamma:" + Gamma;
                // 
                string msg = "【SKQuoteLib_Gamma】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");

                double Vega;
                // 1~5 Vega
                nCode = m_pSKQuote.SKQuoteLib_Vega(double.Parse(textBoxS.Text), double.Parse(textBoxK.Text), double.Parse(textBoxR.Text), double.Parse(textBoxT.Text), double.Parse(textBoxsigma.Text), out Vega);
                labelVega.Text = "Vega:" + Vega;
                // 
                msg = "【SKQuoteLib_Vega】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");

                short nCallPut = 0; // ( Call: 0 , Put: 1)
                if (comboBoxnCallPut.Text == "Call") nCallPut = 0;
                else if (comboBoxnCallPut.Text == "Put") nCallPut = 1;

                if (comboBoxnCallPut.SelectedItem != null)
                {
                    double Delta;
                    // 1~6 Delta
                    nCode = m_pSKQuote.SKQuoteLib_Delta(nCallPut, double.Parse(textBoxS.Text), double.Parse(textBoxK.Text), double.Parse(textBoxR.Text), double.Parse(textBoxT.Text), double.Parse(textBoxsigma.Text), out Delta);
                    labelDelta.Text = "Delta:" + Delta;
                    // 
                    msg = "【SKQuoteLib_Delta】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMethodMessage.AppendText(msg + "\n");

                    double Theta;
                    // 1~6 Theta
                    nCode = m_pSKQuote.SKQuoteLib_Theta(nCallPut, double.Parse(textBoxS.Text), double.Parse(textBoxK.Text), double.Parse(textBoxR.Text), double.Parse(textBoxT.Text), double.Parse(textBoxsigma.Text), out Theta);
                    labelTheta.Text = "Theta:" + Theta;
                    // 
                    msg = "【SKQuoteLib_Theta】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMethodMessage.AppendText(msg + "\n");

                    double Rho;
                    // 1~6 Rho
                    nCode = m_pSKQuote.SKQuoteLib_Rho(nCallPut, double.Parse(textBoxS.Text), double.Parse(textBoxK.Text), double.Parse(textBoxR.Text), double.Parse(textBoxT.Text), double.Parse(textBoxsigma.Text), out Rho);
                    labelRho.Text = "Rho:" + Rho;
                    // 
                    msg = "【SKQuoteLib_Rho】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMethodMessage.AppendText(msg + "\n");
                }
            }
        }
        private void buttonSKQuoteLib_RequestKLineAMByDate_Click(object sender, EventArgs e)
        {
            dataGridViewKLine.Rows.Clear();

            short sKLineType = 0;
            short sOutType = 0;
            short sTradeSession = 0;
            string bstrStartDate = textBoxbstrStartDate.Text;
            string bstrEndDate = textBoxbstrEndDate.Text;
            short sMinuteNumber = short.Parse(textBoxsMinuteNumber.Text);

            string selectValue = comboBoxsKLineType.SelectedItem.ToString();
            if (selectValue == "") sKLineType = 0;
            else if (selectValue == "") sKLineType = 4;
            else if (selectValue == "") sKLineType = 5;
            else if (selectValue == "") sKLineType = 6;

            selectValue = comboBoxsOutType.SelectedItem.ToString();
            if (selectValue == "") sOutType = 0;
            else if (selectValue == "") sOutType = 1;

            selectValue = comboBoxsTradeSession.SelectedItem.ToString();
            if (selectValue == "") sTradeSession = 0;
            else if (selectValue == "AM") sTradeSession = 1;

            // （），，AM，，KK
            int nCode = m_pSKQuote.SKQuoteLib_RequestKLineAMByDate(textBoxbstrStockNo.Text, sKLineType, sOutType, sTradeSession, bstrStartDate, bstrEndDate, sMinuteNumber);

            // 
            string msg = "【SKQuoteLib_RequestKLineAMByDate】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_RequestStockList_Click(object sender, EventArgs e)
        {
            dataGridViewOnNotifyCommodityListWithTypeNo.Rows.Clear();

            short sMarketNo = 0;

            string selectValue = comboBoxSKQuoteLib_RequestStockList.Text;
            if (selectValue == "") sMarketNo = 0;
            else if (selectValue == "") sMarketNo = 1;
            else if (selectValue == "") sMarketNo = 2;
            else if (selectValue == "") sMarketNo = 3;
            else if (selectValue == "") sMarketNo = 4;
            else if (selectValue == "-") sMarketNo = 5;
            else if (selectValue == "-") sMarketNo = 6;
            else if (selectValue == "") sMarketNo = 9;
            else if (selectValue == "") sMarketNo = 10;
            // ，
            int nCode = m_pSKQuote.SKQuoteLib_RequestStockList(sMarketNo);

            // 
            string msg = "【SKQuoteLib_RequestStockList】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_CancelRequestTicks_Click(object sender, EventArgs e)
        {
            // RequestTicks
            int nCode = m_pSKQuote.SKQuoteLib_CancelRequestTicks(textBoxTicks.Text);

            // 
            string msg = "【SKQuoteLib_CancelRequestTicks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_RequestTicksWithMarketNo_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                // ，
                short psPageNo = short.Parse(textBoxpsPageNo.Text);
                short sMarketNo = 5; //-(5)、- (6)
                string selectedValue = comboBoxSKQuoteLib_RequestTicksWithMarketNosMarketNo.Text;
                if (selectedValue == "-(5)") sMarketNo = 5;
                else if (selectedValue == "-(6)") sMarketNo = 6;
                else if (selectedValue == "-9") sMarketNo = 9;
                else if (selectedValue == "-10") sMarketNo = 10;
                int nCode = m_pSKQuote.SKQuoteLib_RequestTicksWithMarketNo(psPageNo, sMarketNo, textBoxTicks.Text);
                // 
                string msg = "【SKQuoteLib_RequestTicksWithMarketNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSKQuoteLib_CancelRequestStocks_Click(object sender, EventArgs e)
        {
            // SKQuoteLib_RequestStocks，
            int nCode = m_pSKQuote.SKQuoteLib_CancelRequestStocks(textBoxStockNos.Text);

            string[] values = (textBoxStockNos.Text).Split(',');
            List<string> listValues = values.ToList();

            List<DataGridViewRow> rowsToRemove = new List<DataGridViewRow>();
            foreach (DataGridViewRow row in dataGridViewStocks.Rows)
            {
                foreach(string value in listValues)
                {
                    if (row.Cells[0].Value.ToString() == value)
                    {
                        rowsToRemove.Add(row); // Rows
                    }
                }
            }

            // Rows
            foreach (DataGridViewRow rowToRemove in rowsToRemove)
            {
                dataGridViewStocks.Rows.Remove(rowToRemove);
            }

            // 
            string msg = "【SKQuoteLib_CancelRequestStocks】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_RequestStocksWithMarketNo_Click(object sender, EventArgs e)
        {
            if (textBoxpsPageNo2.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                // 
                // sMarketNo、 bstrStockNos 

                short psPageNo = short.Parse(textBoxpsPageNo2.Text);
                short sMarketNo = 5; //-(5)、- (6)
                string selectedValue = comboBoxSKQuoteLib_RequestStocksWithMarketNo.Text;
                if (selectedValue == "-(5)") sMarketNo = 5;
                else if (selectedValue == "-(6)") sMarketNo = 6;
                else if (selectedValue == "-9") sMarketNo = 9;
                else if (selectedValue == "-10") sMarketNo = 10;
                int nCode = m_pSKQuote.SKQuoteLib_RequestStocksWithMarketNo(psPageNo, sMarketNo, textBoxStockNos.Text);
                // 
                string msg = "【SKQuoteLib_RequestStocksWithMarketNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSKQuoteLib_RequestFutureTradeInfo_Click(object sender, EventArgs e)
        {
            if (textBoxSKQuoteLib_RequestFutureTradeInfopsPageNo.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                short psPageNo = short.Parse(textBoxSKQuoteLib_RequestFutureTradeInfopsPageNo.Text);
                string bstrStockNo = textBoxSKQuoteLib_RequestFutureTradeInfobstrStockNo.Text;
                // 
                int nCode = m_pSKQuote.SKQuoteLib_RequestFutureTradeInfo(psPageNo, bstrStockNo);

                // 
                string msg = "【SKQuoteLib_RequestFutureTradeInfo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSKQuoteLib_GetStrikePrices_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKQuote.SKQuoteLib_GetStrikePrices();

            // 
            string msg = "【SKQuoteLib_GetStrikePrices】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_GetStockByNoLONG_Click(object sender, EventArgs e)
        {
            string bstrStockNo = textBoxSKQuoteLib_GetStockByNoLONG.Text;

            SKSTOCKLONG pSKStock = new SKSTOCKLONG();
            // (LONG index)，
            int nCode = m_pSKQuote.SKQuoteLib_GetStockByNoLONG(bstrStockNo, ref pSKStock);

            // 
            string msg = "【SKQuoteLib_GetStockByNoLONG】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");

            // SKSTOCKLONG
            bool found = false; // 
            {
                foreach (DataGridViewRow row in dataGridViewSKQuoteLib_GetStockByNoLONG.Rows)
                {
                    if (row.IsNewRow)
                        continue;
                    if (row.Cells[2].Value != null)
                    {
                        if (bstrStockNo == row.Cells[2].Value.ToString())// 
                        {
                            found = true;
                            break;
                        }
                    }
                    
                }
                if (found == false)
                    dataGridViewSKQuoteLib_GetStockByNoLONG.Rows.Add(pSKStock.sTypeNo, pSKStock.bstrMarketNo, pSKStock.bstrStockNo, pSKStock.bstrStockName, pSKStock.nHigh / 100.0, pSKStock.nOpen / 100.0, pSKStock.nLow / 100.0, pSKStock.nClose / 100.0, pSKStock.nTickQty, pSKStock.nRef / 100.0, pSKStock.nBid / 100.0, pSKStock.nBc, pSKStock.nAsk / 100.0, pSKStock.nAc, pSKStock.nTBc, pSKStock.nTAc, pSKStock.nTQty, pSKStock.nYQty, pSKStock.nUp / 100.0, pSKStock.nDown / 100.0, pSKStock.nSimulate, pSKStock.nDayTrade, pSKStock.nTradingDay, pSKStock.nTradingLotFlag);
            }
        }
        private void buttonSKQuoteLib_RequestServerTime_Click(object sender, EventArgs e)
        {
            // 。
            int nCode = m_pSKQuote.SKQuoteLib_RequestServerTime();
            // 
            string msg = "【SKQuoteLib_RequestServerTime】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_GetMarketBuySellUpDown_Click(object sender, EventArgs e)
        {
            // (,,)
            int nCode = m_pSKQuote.SKQuoteLib_GetMarketBuySellUpDown();
            // 
            string msg = "【SKQuoteLib_GetMarketBuySellUpDown】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_RequestMACD_Click(object sender, EventArgs e)
        {
            short psPageNo = short.Parse(textBoxSKQuoteLib_RequestMACDpsPageNo.Text);

            string bstrStockNo = textBoxbstrStockNo.Text;

            // MACD。()
            int nCode = m_pSKQuote.SKQuoteLib_RequestMACD(psPageNo, bstrStockNo);

            // 
            string msg = "【SKQuoteLib_RequestMACD】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKQuoteLib_RequestBoolTunel_Click(object sender, EventArgs e)
        {
            short psPageNo = short.Parse(textBoxSKQuoteLib_RequestMACDpsPageNo.Text);

            string bstrStockNo = textBoxbstrStockNo.Text;

            // BoolTunel
            int nCode = m_pSKQuote.SKQuoteLib_RequestBoolTunel(psPageNo, bstrStockNo);

            // 
            string msg = "【SKQuoteLib_RequestBoolTunel】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
    }
}
