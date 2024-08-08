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
    public partial class TSStrategyOrderForm : Form
    {
        // 
        SKCenterLib m_pSKCenter = new SKCenterLib(); //&
        SKOrderLib m_pSKOrder = new SKOrderLib(); //
        // 
        bool bAsyncOrder = false;
        // [UserID]-
        Dictionary<string, List<string>> m_dictUserID = new Dictionary<string, List<string>>();
        List<string> allkeys;       
        static void AddUserID(Dictionary<string, List<string>> dictUserID, string UserID, string AccountData)
        {
            string[] values = AccountData.Split(',');

            // broker ID (IB)4 + 7
            string Account = values[1] + values[3];

            if (dictUserID.ContainsKey(UserID))
            {
                // ， List<string>
                dictUserID[UserID].Add(Account);
            }
            else
            {
                // ， List<string>，
                dictUserID[UserID] = new List<string> { Account };
            }
        }
        public TSStrategyOrderForm()
        {
            // Init
            {
                InitializeComponent();

                // comboBox
                {
                    // DayTrade
                    {
                        // comboBoxnBuySellDT
                        {
                            comboBoxnBuySellDT.Items.Add("0:");
                            comboBoxnBuySellDT.Items.Add("1:");
                        }
                        // comboBoxnOrderPriceCondDT
                        {
                            comboBoxnOrderPriceCondDT.Items.Add("0:ROD");
                            comboBoxnOrderPriceCondDT.Items.Add("3:IOC");
                            comboBoxnOrderPriceCondDT.Items.Add("4:FOK");
                        }
                        // comboBoxnOrderPriceTypeDT
                        {
                            comboBoxnOrderPriceTypeDT.Items.Add("1:");
                            comboBoxnOrderPriceTypeDT.Items.Add("2:");
                        }
                        // comboBoxnInnerOrderIsMITDT
                        {
                            comboBoxnInnerOrderIsMITDT.Items.Add("0:N");
                            comboBoxnInnerOrderIsMITDT.Items.Add("1:Y");
                        }
                        // comboBoxnMITDirDT
                        {
                            comboBoxnMITDirDT.Items.Add("0:MIT");
                            comboBoxnMITDirDT.Items.Add("1:()");
                            comboBoxnMITDirDT.Items.Add("2:()");
                        }
                        // comboBoxnClearAllFlagDT
                        {
                            comboBoxnClearAllFlagDT.Items.Add("0:");
                            comboBoxnClearAllFlagDT.Items.Add("1:");
                        }
                        // comboBoxnClearAllPriceTypeDT
                        {
                            comboBoxnClearAllPriceTypeDT.Items.Add("1:");
                            comboBoxnClearAllPriceTypeDT.Items.Add("2:");
                        }
                        // comboBoxnFinalClearFlagDT
                        {
                            comboBoxnFinalClearFlagDT.Items.Add("0:");
                            comboBoxnFinalClearFlagDT.Items.Add("1:");
                        }
                        // comboBoxnTakeProfitFlagDT
                        {
                            comboBoxnTakeProfitFlagDT.Items.Add("0:");
                            comboBoxnTakeProfitFlagDT.Items.Add("1:");
                        }
                        // comboBoxnStopLossFlagDT
                        {
                            comboBoxnStopLossFlagDT.Items.Add("0:");
                            comboBoxnStopLossFlagDT.Items.Add("1:");
                        }
                        // comboBoxnClearOrderCondDT
                        {
                            comboBoxnClearOrderCondDT.Items.Add("0:ROD");
                            comboBoxnClearOrderCondDT.Items.Add("3:IOC");
                            comboBoxnClearOrderCondDT.Items.Add("4:FOK");
                        }

                        // comboBoxnRDOTPPercent
                        {
                            comboBoxnRDOTPPercent.Items.Add("0:");
                            comboBoxnRDOTPPercent.Items.Add("1:");
                        }
                        // comboBoxnRDTPMarketPriceType
                        {
                            comboBoxnRDTPMarketPriceType.Items.Add("1:");
                            comboBoxnRDTPMarketPriceType.Items.Add("2:");
                        }
                        // comboBoxnRDOSLPercent
                        {
                            comboBoxnRDOSLPercent.Items.Add("0:");
                            comboBoxnRDOSLPercent.Items.Add("1:");
                        }
                        // comboBoxnRDSLMarketPriceType
                        {
                            comboBoxnRDSLMarketPriceType.Items.Add("1:");
                            comboBoxnRDSLMarketPriceType.Items.Add("2:");
                        }
                        // comboBoxnTakeProfitOrderCond
                        {
                            comboBoxnTakeProfitOrderCond.Items.Add("0:ROD");
                            comboBoxnTakeProfitOrderCond.Items.Add("3:IOC");
                            comboBoxnTakeProfitOrderCond.Items.Add("4:FOK");
                        }
                        // comboBoxnStopLossOrderCond
                        {
                            comboBoxnStopLossOrderCond.Items.Add("0:ROD");
                            comboBoxnStopLossOrderCond.Items.Add("3:IOC");
                            comboBoxnStopLossOrderCond.Items.Add("4:FOK");
                        }
                    }
                    // ClearOut
                    {
                        // comboBoxnBuySellC
                        {
                            comboBoxnBuySellC.Items.Add("0:");
                            comboBoxnBuySellC.Items.Add("1:");
                        }
                        // comboBoxnClearAllFlagC
                        {
                            comboBoxnClearAllFlagC.Items.Add("0:");
                            comboBoxnClearAllFlagC.Items.Add("1:");
                        }
                        // comboBoxnClearAllPriceTypeC
                        {
                            comboBoxnClearAllPriceTypeC.Items.Add("1:");
                            comboBoxnClearAllPriceTypeC.Items.Add("2:");
                        }
                        // comboBoxnFinalClearFlagC
                        {
                            comboBoxnFinalClearFlagC.Items.Add("0:");
                            comboBoxnFinalClearFlagC.Items.Add("1:");
                        }
                        // comboBoxnOrderTypeC
                        {
                            comboBoxnOrderTypeC.Items.Add("0:");
                            comboBoxnOrderTypeC.Items.Add("3:");
                            comboBoxnOrderTypeC.Items.Add("4:");
                        }
                        // comboBoxnClearOrderCondC
                        {
                            comboBoxnClearOrderCondC.Items.Add("0:ROD");
                            comboBoxnClearOrderCondC.Items.Add("3:IOC");
                            comboBoxnClearOrderCondC.Items.Add("4:FOK");
                        }

                        // comboBoxnLTEFlag
                        {
                            comboBoxnLTEFlag.Items.Add("0:");
                            comboBoxnLTEFlag.Items.Add("1:");
                        }
                        // comboBoxnLTEMarketPrice
                        {
                            comboBoxnLTEMarketPrice.Items.Add("1:");
                            comboBoxnLTEMarketPrice.Items.Add("2:");
                        }
                        // comboBoxnGTEFlag
                        {
                            comboBoxnGTEFlag.Items.Add("0:");
                            comboBoxnGTEFlag.Items.Add("1:");
                        }
                        // comboBoxnGTEMarketPrice
                        {
                            comboBoxnGTEMarketPrice.Items.Add("1:");
                            comboBoxnGTEMarketPrice.Items.Add("2:");
                        }
                        // comboBoxnGTEOrderCond
                        {
                            comboBoxnGTEOrderCond.Items.Add("0:ROD");
                            comboBoxnGTEOrderCond.Items.Add("3:IOC");
                            comboBoxnGTEOrderCond.Items.Add("4:FOK");
                        }
                        // comboBoxnLTEOrderCond
                        {
                            comboBoxnLTEOrderCond.Items.Add("0:ROD");
                            comboBoxnLTEOrderCond.Items.Add("3:IOC");
                            comboBoxnLTEOrderCond.Items.Add("4:FOK");
                        }
                    }
                    // MIT
                    {
                        // comboBoxnOrderTypeMIT
                        {
                            comboBoxnOrderTypeMIT.Items.Add("0:");
                            comboBoxnOrderTypeMIT.Items.Add("3:");
                            comboBoxnOrderTypeMIT.Items.Add("4:");
                            comboBoxnOrderTypeMIT.Items.Add("8:");
                        }
                        // comboBoxnBuySellMIT
                        {
                            comboBoxnBuySellMIT.Items.Add("0:");
                            comboBoxnBuySellMIT.Items.Add("1:");
                        }
                        // comboBoxnOrderPriceTypeMIT
                        {
                            comboBoxnOrderPriceTypeMIT.Items.Add("1:");
                            comboBoxnOrderPriceTypeMIT.Items.Add("2:");
                        }
                        // comboBoxnOrderCondMIT
                        {
                            comboBoxnOrderCondMIT.Items.Add("0:ROD");
                            comboBoxnOrderCondMIT.Items.Add("3:IOC");
                            comboBoxnOrderCondMIT.Items.Add("4:FOK");
                        }
                        // comboBoxnTriggerDirMIT
                        {
                            comboBoxnTriggerDirMIT.Items.Add("1:GTE");
                            comboBoxnTriggerDirMIT.Items.Add("2:LTE");
                        }
                        // comboBoxnLongActionFlagMIT
                        {
                            comboBoxnLongActionFlagMIT.Items.Add("0:");
                            comboBoxnLongActionFlagMIT.Items.Add("1:");
                        }
                        // comboBoxnLATypeMIT
                        {
                            comboBoxnLATypeMIT.Items.Add("0:");
                            comboBoxnLATypeMIT.Items.Add("1:");
                            comboBoxnLATypeMIT.Items.Add("3:");
                        }

                        // comboBoxnPreRiskFlag
                        {
                            comboBoxnPreRiskFlag.Items.Add("0:");
                            comboBoxnPreRiskFlag.Items.Add("1:");
                        }
                    }
                    // OCO
                    {
                        // comboBoxnBuySellUp
                        {
                            comboBoxnBuySellUp.Items.Add("0:");
                            comboBoxnBuySellUp.Items.Add("1:");
                        }
                        // comboBoxnBuySellDown
                        {
                            comboBoxnBuySellDown.Items.Add("0:");
                            comboBoxnBuySellDown.Items.Add("1:");
                        }
                        // comboBoxnOrderCondUp
                        {
                            comboBoxnOrderCondUp.Items.Add("0:ROD");
                            comboBoxnOrderCondUp.Items.Add("3:IOC");
                            comboBoxnOrderCondUp.Items.Add("4:FOK");
                        }
                        // comboBoxnOrderCondDown
                        {
                            comboBoxnOrderCondDown.Items.Add("0:ROD");
                            comboBoxnOrderCondDown.Items.Add("3:IOC");
                            comboBoxnOrderCondDown.Items.Add("4:FOK");
                        }
                        // comboBoxnOrderTypeUp
                        {
                            comboBoxnOrderTypeUp.Items.Add("0:");
                            comboBoxnOrderTypeUp.Items.Add("3:");
                            comboBoxnOrderTypeUp.Items.Add("4:");
                            comboBoxnOrderTypeUp.Items.Add("8:");
                        }
                        // comboBoxnOrderTypeDown
                        {
                            comboBoxnOrderTypeDown.Items.Add("0:");
                            comboBoxnOrderTypeDown.Items.Add("3:");
                            comboBoxnOrderTypeDown.Items.Add("4:");
                            comboBoxnOrderTypeDown.Items.Add("8:");
                        }
                        // comboBoxnOrderPriceTypeUp
                        {
                            comboBoxnOrderPriceTypeUp.Items.Add("1:");
                            comboBoxnOrderPriceTypeUp.Items.Add("2:");
                        }
                        // comboBoxnOrderPriceTypeDown
                        {
                            comboBoxnOrderPriceTypeDown.Items.Add("1:");
                            comboBoxnOrderPriceTypeDown.Items.Add("2:");
                        }

                    }
                    // MIOC
                    {
                        // comboBoxnPrimeMIOC
                        {
                            comboBoxnPrimeMIOC.Items.Add("0:");
                            comboBoxnPrimeMIOC.Items.Add("1:");
                        }
                        // comboBoxnOrderTypeMIOC
                        {
                            comboBoxnOrderTypeMIOC.Items.Add("0:");
                            comboBoxnOrderTypeMIOC.Items.Add("3:");
                            comboBoxnOrderTypeMIOC.Items.Add("4:");
                            comboBoxnOrderTypeMIOC.Items.Add("8:");
                        }

                        // comboBoxnBuySellMIOC1
                        {
                            comboBoxnBuySellMIOC1.Items.Add("1:");
                            comboBoxnBuySellMIOC1.Items.Add("2:");
                        }
                     
                        // comboBoxnOrderPriceTypeMIOC
                        {
                            comboBoxnOrderPriceTypeMIOC.Items.Add("0:");
                            comboBoxnOrderPriceTypeMIOC.Items.Add("1:()()");
                        }
                    }
                    // MST
                    {
                        // comboBoxnOrderTypeMST
                        {
                            comboBoxnOrderTypeMST.Items.Add("0:");
                            comboBoxnOrderTypeMST.Items.Add("3:");
                            comboBoxnOrderTypeMST.Items.Add("4:");
                            comboBoxnOrderTypeMST.Items.Add("8:");
                        }
                        // comboBoxnBuySellMST
                        { 
                            comboBoxnBuySellMST.Items.Add("0:");
                            comboBoxnBuySellMST.Items.Add("1:");
                        }
                        // comboBoxnOrderPriceTypeMST
                        {
                            comboBoxnOrderPriceTypeMST.Items.Add("1:");
                            comboBoxnOrderPriceTypeMST.Items.Add("2:");
                        }
                        // comboBoxnOrderCondMST
                        {
                            comboBoxnOrderCondMST.Items.Add("0:ROD");
                            comboBoxnOrderCondMST.Items.Add("3:IOC");
                            comboBoxnOrderCondMST.Items.Add("4:FOK");
                        }
                        // comboBoxnTriggerDirMST
                        {
                            comboBoxnTriggerDirMST.Items.Add("1:GTE");
                            comboBoxnTriggerDirMST.Items.Add("2:LTE");
                        }
                        // comboBoxnPrimeMST
                        {
                            comboBoxnPrimeMST.Items.Add("0:");
                            comboBoxnPrimeMST.Items.Add("1:");
                        }
                        // comboBoxnLongActionFlagMST
                        {
                            comboBoxnLongActionFlagMST.Items.Add("0:");
                            comboBoxnLongActionFlagMST.Items.Add("1:");
                        }
                        // comboBoxnLATypeMST
                        {
                            comboBoxnLATypeMST.Items.Add("0:");
                            comboBoxnLATypeMST.Items.Add("1:");
                        }

                        // comboBoxnTriggerMethod
                        {
                            comboBoxnTriggerMethod.Items.Add("0:,");
                            comboBoxnTriggerMethod.Items.Add("1:,");
                        }

                    }
                    // AB
                    {
                        // comboBoxnOrderTypeAB
                        {
                            comboBoxnOrderTypeAB.Items.Add("0:");
                            comboBoxnOrderTypeAB.Items.Add("3:");
                            comboBoxnOrderTypeAB.Items.Add("4:");
                            comboBoxnOrderTypeAB.Items.Add("8:");
                        }
                        // comboBoxnBuySellAB
                        {
                            comboBoxnBuySellAB.Items.Add("0:");
                            comboBoxnBuySellAB.Items.Add("1:");
                        }
                        // comboBoxnOrderPriceTypeAB
                        {
                            comboBoxnOrderPriceTypeAB.Items.Add("1:");
                            comboBoxnOrderPriceTypeAB.Items.Add("2:");
                        }
                        // comboBoxnOrderCondAB
                        {
                            comboBoxnOrderCondAB.Items.Add("0:ROD");
                            comboBoxnOrderCondAB.Items.Add("3:IOC");
                            comboBoxnOrderCondAB.Items.Add("4:FOK");
                        }
                        // comboBoxnTriggerDirAB
                        {
                            comboBoxnTriggerDirAB.Items.Add("1:GTE");
                            comboBoxnTriggerDirAB.Items.Add("2:LTE");
                        }
                        // comboBoxnPrimeAB
                        {
                            comboBoxnPrimeAB.Items.Add("0:");
                            comboBoxnPrimeAB.Items.Add("1:");
                        }

                        // comboBoxnMarketNo
                        {
                            comboBoxnMarketNo.Items.Add("1:");
                            comboBoxnMarketNo.Items.Add("2:");
                            comboBoxnMarketNo.Items.Add("3:");
                            comboBoxnMarketNo.Items.Add("4:");
                        }
                        // comboBoxnReserved
                        {
                            comboBoxnReserved.Items.Add("0:");
                            comboBoxnReserved.Items.Add("1:");
                        }
                    }
                    // CB
                    {
                        // comboBoxnOrderTypeCB
                        {
                            comboBoxnOrderTypeCB.Items.Add("0:");
                            comboBoxnOrderTypeCB.Items.Add("3:()");
                            comboBoxnOrderTypeCB.Items.Add("4:()");
                            comboBoxnOrderTypeCB.Items.Add("8:");
                        }
                        // comboBoxnOrderPriceCondCB
                        {
                            comboBoxnOrderPriceCondCB.Items.Add("0:ROD");
                            comboBoxnOrderPriceCondCB.Items.Add("3:IOC");
                            comboBoxnOrderPriceCondCB.Items.Add("4:FOK");
                        }
                        // comboBoxnBuySellCB
                        {
                            comboBoxnBuySellCB.Items.Add("0:");
                            comboBoxnBuySellCB.Items.Add("1:");
                        }
                        // comboBoxnOrderPriceTypeCB
                        {
                            comboBoxnOrderPriceTypeCB.Items.Add("1:");
                            comboBoxnOrderPriceTypeCB.Items.Add("2:");
                        }
                        // comboBoxnInnerOrderIsMITCB
                        {
                            comboBoxnInnerOrderIsMITCB.Items.Add("0:N");
                            comboBoxnInnerOrderIsMITCB.Items.Add("1:Y");
                        }
                        // comboBoxnStopLossFlagCB
                        {
                            comboBoxnStopLossFlagCB.Items.Add("0:");
                            comboBoxnStopLossFlagCB.Items.Add("1:");
                        }
                        // comboBoxnSLDirCB
                        {
                            comboBoxnSLDirCB.Items.Add("0:None");
                            comboBoxnSLDirCB.Items.Add("1:GTE");
                            comboBoxnSLDirCB.Items.Add("2:LTE");
                        }
                        // comboBoxnTickFlagCB
                        {
                            comboBoxnTickFlagCB.Items.Add("0:");
                            comboBoxnTickFlagCB.Items.Add("1:");
                        }
                        // comboBoxnPreQtyFlagCB
                        {
                            comboBoxnPreQtyFlagCB.Items.Add("0:");
                            comboBoxnPreQtyFlagCB.Items.Add("1:");
                        }
                        // comboBoxnPreQtyDirCB
                        {
                            comboBoxnPreQtyDirCB.Items.Add("0:None");
                            comboBoxnPreQtyDirCB.Items.Add("1:GTE");
                            comboBoxnPreQtyDirCB.Items.Add("2:LTE");
                        }
                        // comboBoxnSumQtyFlagCB
                        {
                            comboBoxnSumQtyFlagCB.Items.Add("0:");
                            comboBoxnSumQtyFlagCB.Items.Add("1:");
                        }
                        // comboBoxnClearAllFlagCB
                        {
                            comboBoxnClearAllFlagCB.Items.Add("0:");
                            comboBoxnClearAllFlagCB.Items.Add("1:");
                        }
                        // comboBoxnFinalClearFlagCB
                        {
                            comboBoxnFinalClearFlagCB.Items.Add("0:");
                            comboBoxnFinalClearFlagCB.Items.Add("1:");
                        }
                        // comboBoxnMITDirCB
                        {
                            comboBoxnMITDirCB.Items.Add("0:None");
                            comboBoxnMITDirCB.Items.Add("1:GTE");
                            comboBoxnMITDirCB.Items.Add("2:LTE");
                        }
                        // comboBoxnTakeProfitFlagCB
                        {
                            comboBoxnTakeProfitFlagCB.Items.Add("0:");
                            comboBoxnTakeProfitFlagCB.Items.Add("1:");
                        }

                        // comboBoxnTPDir
                        {
                            comboBoxnTPDir.Items.Add("0:None");
                            comboBoxnTPDir.Items.Add("1:GTE");
                            comboBoxnTPDir.Items.Add("2:LTE");
                        }
                        // comboBoxnTickDir
                        {
                            comboBoxnTickDir.Items.Add("0:None");
                            comboBoxnTickDir.Items.Add("1:GTE");
                            comboBoxnTickDir.Items.Add("2:LTE");
                        }
                        // comboBoxnUpDownFlag
                        {
                            comboBoxnUpDownFlag.Items.Add("0:");
                            comboBoxnUpDownFlag.Items.Add("1:");
                        }
                        // comboBoxnUpDownDir
                        {
                            comboBoxnUpDownDir.Items.Add("0:None");
                            comboBoxnUpDownDir.Items.Add("1:GTE");
                            comboBoxnUpDownDir.Items.Add("2:LTE");
                        }
                        // comboBoxnSumQtyDir
                        {
                            comboBoxnSumQtyDir.Items.Add("0:None");
                            comboBoxnSumQtyDir.Items.Add("1:GTE");
                            comboBoxnSumQtyDir.Items.Add("2:LTE");
                        }
                    }

                    // 
                    {
                        // comboBoxnTradeKindVersion
                        {
                            comboBoxnTradeKindVersion.Items.Add("");
                            comboBoxnTradeKindVersion.Items.Add("V1");
                        }
                        // comboBoxnTradeKind
                        {
                            comboBoxnTradeKind.Items.Add("6:MIOC");
                            comboBoxnTradeKind.Items.Add("7:MST");
                            comboBoxnTradeKind.Items.Add("8:MIT");
                        }
                        // comboBoxnTradeKindV1
                        {
                            comboBoxnTradeKindV1.Items.Add("3:OCO");
                            comboBoxnTradeKindV1.Items.Add("8:MIT");
                            comboBoxnTradeKindV1.Items.Add("9:MST");
                            comboBoxnTradeKindV1.Items.Add("10:AB");
                            comboBoxnTradeKindV1.Items.Add("11:");
                            comboBoxnTradeKindV1.Items.Add("17:");
                            comboBoxnTradeKindV1.Items.Add("27：CB");
                        }
                        // comboBoxnDeletType
                        {
                            comboBoxnDeletType.Items.Add("1:");
                            comboBoxnDeletType.Items.Add("2:");
                            comboBoxnDeletType.Items.Add("3:");
                        }
                        // comboBoxnMarket
                        {
                            comboBoxnMarket.Items.Add("1：");
                            comboBoxnMarket.Items.Add("2：");
                            comboBoxnMarket.Items.Add("3：");
                            comboBoxnMarket.Items.Add("4：");
                        }
                    }

                    // 
                    {
                        // comboBoxbstrKind
                        {
                            comboBoxbstrKind.Items.Add("DayTrade:");
                            comboBoxbstrKind.Items.Add("ClearOut:");
                            comboBoxbstrKind.Items.Add("MIT：、MIT");
                            comboBoxbstrKind.Items.Add("OCO：");
                            comboBoxbstrKind.Items.Add("MIOC：IOC");
                            comboBoxbstrKind.Items.Add("MST：、MST");
                            comboBoxbstrKind.Items.Add("AB：AB");
                            comboBoxbstrKind.Items.Add("CB：");
                        }
                    }
                }
            }
        }
        private void checkBoxAsyncOrder_CheckedChanged(object sender, EventArgs e)
        {
            // 
            if (checkBoxAsyncOrder.Checked == true) bAsyncOrder = true;
            else bAsyncOrder = false;
        }
        private void comboBoxUserID_DropDown(object sender, EventArgs e)
        {
            m_dictUserID.Clear(); //

            // 
            int nCode = m_pSKOrder.GetUserAccount();
            // 
            string msg = "【GetUserAccount】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void SendOrderForm_Load(object sender, EventArgs e)
        {
            //
            m_pSKOrder.OnAccount += new _ISKOrderLibEvents_OnAccountEventHandler(OnAccount);
            void OnAccount(string bstrLogInID, string bstrAccountData)
            {
                string[] values = bstrAccountData.Split(',');
                if (values[0] == "TS")
                {
                    AddUserID(m_dictUserID, bstrLogInID, bstrAccountData);

                    //key
                    if (allkeys != null) allkeys.Clear();
                    allkeys = new List<string>(m_dictUserID.Keys);

                    if (comboBoxUserID.DataSource != null) comboBoxUserID.DataSource = null;
                    comboBoxUserID.DataSource = allkeys;

                    if (comboBoxAccount.DataSource != null) comboBoxAccount.DataSource = null;
                    comboBoxAccount.DataSource = m_dictUserID[comboBoxUserID.Text];
                }
            }
            // 
            m_pSKOrder.OnAsyncOrder += new _ISKOrderLibEvents_OnAsyncOrderEventHandler(OnAsyncOrder);
            void OnAsyncOrder(int nThreadID, int nCode, string bstrMessage)
            {
                // 
                string msg = "TID:" + nThreadID + ":" + bstrMessage;
                msg = "【】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode) + msg;
                richTextBoxMessage.AppendText(msg + "\n");
            }
            // 
            {
                int nCode = m_pSKOrder.GetUserAccount();
                // 
                string msg = "【GetUserAccount】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
            //<>
            m_pSKOrder.OnTSSmartStrategyReport += new _ISKOrderLibEvents_OnTSSmartStrategyReportEventHandler(OnTSSmartStrategyReport);
            void OnTSSmartStrategyReport(string bstrData)
            {
                richTextBoxOnTSSmartStrategyReport.AppendText(bstrData + '\n');
            }
        }
        private void buttonGetTSSmartStrategyReport_Click(object sender, EventArgs e)
        {
            string bstrKind = "DayTrade";
            string bstrDate = textBoxbstrDate.Text; // (ex:20220601)

            if (comboBoxbstrKind.Text == "DayTrade:") bstrKind = "DayTrade";
            else if (comboBoxbstrKind.Text == "ClearOut:") bstrKind = "ClearOut";
            else if (comboBoxbstrKind.Text == "MIT：、MIT") bstrKind = "MIT";
            else if (comboBoxbstrKind.Text == "OCO：") bstrKind = "OCO";
            else if (comboBoxbstrKind.Text == "MIOC：IOC") bstrKind = "MIOC";
            else if (comboBoxbstrKind.Text == "MST：、MST") bstrKind = "MST";
            else if (comboBoxbstrKind.Text == "AB：AB") bstrKind = "AB";
            else if (comboBoxbstrKind.Text == "CB：") bstrKind = "CB";

            // 
            int nCode = m_pSKOrder.GetTSSmartStrategyReport(comboBoxUserID.Text, comboBoxAccount.Text, "TS", 0, bstrKind, bstrDate);
            // 
            string msg = "【GetTSSmartStrategyReport】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonCancelTSStrategyOrder_Click(object sender, EventArgs e)
        {
            int nTradeKind = 0; // 6:MIOC;7:MST; 8:MIT。
            string bstrMessage;
            if (comboBoxnTradeKind.Text == "6:MIOC") nTradeKind = 6;
            else if (comboBoxnTradeKind.Text == "7:MST") nTradeKind = 7;
            else if (comboBoxnTradeKind.Text == "8:MIT") nTradeKind = 8;
            // 。GetTSStrategyOrder 。，，。
            int nCode = m_pSKOrder.CancelTSStrategyOrder(comboBoxUserID.Text, bAsyncOrder, comboBoxAccount.Text, textBoxbstrSmartKey.Text, nTradeKind, out bstrMessage);
            // 
            string msg = "【CancelTSStrategyOrder】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonCancelTSStrategyOrderV1_Click(object sender, EventArgs e)
        {
            CANCELSTRATEGYORDER pCancelOrder = new CANCELSTRATEGYORDER(); // 
            pCancelOrder.bstrFullAccount = comboBoxAccount.Text;
            // 1：、2：、3：、4：
            if (comboBoxnMarket.Text == "1：") pCancelOrder.nMarket = 1;
            else if (comboBoxnMarket.Text == "2：") pCancelOrder.nMarket = 2;
            else if (comboBoxnMarket.Text == "3：") pCancelOrder.nMarket = 3;
            else if (comboBoxnMarket.Text == "4：") pCancelOrder.nMarket = 4;

            pCancelOrder.bstrParentSmartKey = textBoxbstrParentSmartKey.Text; // ; 
            pCancelOrder.bstrSmartKey = textBoxbstrSmartKey.Text; // ; 

            if (comboBoxnTradeKindV1.Text == "3:OCO") pCancelOrder.nTradeKind = 3;
            else if (comboBoxnTradeKindV1.Text == "8:MIT") pCancelOrder.nTradeKind = 8;
            else if (comboBoxnTradeKindV1.Text == "9:MST") pCancelOrder.nTradeKind = 9;
            else if (comboBoxnTradeKindV1.Text == "10:AB") pCancelOrder.nTradeKind = 10;
            else if (comboBoxnTradeKindV1.Text == "11:") pCancelOrder.nTradeKind = 11;
            else if (comboBoxnTradeKindV1.Text == "17:") pCancelOrder.nTradeKind = 17;
            else if (comboBoxnTradeKindV1.Text == "27：CB") pCancelOrder.nTradeKind = 27;
            else pCancelOrder.nTradeKind = 0;
            if (comboBoxnDeletType.Text == "1:") pCancelOrder.nDeleteType = 1;
            else if (comboBoxnDeletType.Text == "2:") pCancelOrder.nDeleteType = 2;
            else if (comboBoxnDeletType.Text == "3:") pCancelOrder.nDeleteType = 3;

            pCancelOrder.bstrSeqNo = textBoxbstrSeqNo.Text;//(1~3, )
            pCancelOrder.bstrOrderNo = textBoxbstrOrderNo.Text; //((/))，（，）
            pCancelOrder.bstrSmartKeyOut = textBoxbstrSmartKeyOut.Text; //(，

            string bstrMessage;
            // 。GetTSStrategyOrder 。
            int nCode = m_pSKOrder.CancelTSStrategyOrderV1(comboBoxUserID.Text, pCancelOrder, out bstrMessage);
            // 
            string msg = "【CancelTSStrategyOrderV1】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }

        private void buttonSendStockStrategyDayTrade_Click(object sender, EventArgs e)
        {
            if (textBoxnQtyDT.Text == "") // ，!
            {
                MessageBox.Show("()！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDER pOrder = new STOCKSTRATEGYORDER();
                pOrder.bstrFullAccount = comboBoxAccount.Text;

                pOrder.bstrStockNo = textBoxbstrStockNoDT.Text;
                pOrder.nQty = int.Parse(textBoxnQtyDT.Text);
                pOrder.bstrOrderPrice = textBoxbstrOrderPriceDT.Text;

                if (comboBoxnBuySellDT.Text == "0:") pOrder.nBuySell = 0;
                else if (comboBoxnBuySellDT.Text == "1:") pOrder.nBuySell = 1;

                if (comboBoxnOrderPriceCondDT.Text == "0:ROD") pOrder.nOrderPriceCond = 0;
                else if (comboBoxnOrderPriceCondDT.Text == "3:IOC") pOrder.nOrderPriceCond = 3;
                else if (comboBoxnOrderPriceCondDT.Text == "4:FOK") pOrder.nOrderPriceCond = 4;

                if (comboBoxnOrderPriceTypeDT.Text == "1:") pOrder.nOrderPriceType = 1;
                else if (comboBoxnOrderPriceTypeDT.Text == "2:") pOrder.nOrderPriceType = 2;

                if (comboBoxnInnerOrderIsMITDT.Text == "0:N") pOrder.nInnerOrderIsMIT = 0;
                else if (comboBoxnInnerOrderIsMITDT.Text == "1:Y") pOrder.nInnerOrderIsMIT = 1;

                if (comboBoxnMITDirDT.Text == "0:MIT") pOrder.nMITDir = 0;
                else if (comboBoxnMITDirDT.Text == "1:()") pOrder.nMITDir = 1;
                else if (comboBoxnMITDirDT.Text == "2:()") pOrder.nMITDir = 2;

                pOrder.bstrMITTriggerPrice = textBoxbstrMITTriggerPriceDT.Text;
                pOrder.bstrMITDealPrice = textBoxbstrMITDealPriceDT.Text;

                if (comboBoxnClearAllFlagDT.Text == "0:") pOrder.nClearAllFlag = 0;
                else if (comboBoxnClearAllFlagDT.Text == "1:") pOrder.nClearAllFlag = 1;

                pOrder.bstrClearCancelTime = textBoxbstrClearCancelTimeDT.Text;

                if (comboBoxnClearAllPriceTypeDT.Text == "1:") pOrder.nClearAllPriceType = 1;
                else if (comboBoxnClearAllPriceTypeDT.Text == "2:") pOrder.nClearAllPriceType = 2;

                pOrder.bstrClearAllOrderPrice = textBoxbstrClearAllOrderPriceDT.Text;

                if (comboBoxnFinalClearFlagDT.Text == "0:") pOrder.nFinalClearFlag = 0;
                else if (comboBoxnFinalClearFlagDT.Text == "1:") pOrder.nFinalClearFlag = 1;

                if (comboBoxnTakeProfitFlagDT.Text == "0:") pOrder.nTakeProfitFlag = 0;
                else if (comboBoxnTakeProfitFlagDT.Text == "1:") pOrder.nTakeProfitFlag = 1;

                if (comboBoxnRDOTPPercent.Text == "0:") pOrder.nRDOTPPercent = 0;
                else if (comboBoxnRDOTPPercent.Text == "1:") pOrder.nRDOTPPercent = 1;

                pOrder.bstrTPPercent = textBoxbstrTPPercent.Text;
                pOrder.bstrTPTrigger = textBoxbstrTPTrigger.Text;

                if (comboBoxnRDTPMarketPriceType.Text == "1:") pOrder.nRDTPMarketPriceType = 1;
                else if (comboBoxnRDTPMarketPriceType.Text == "2:") pOrder.nRDTPMarketPriceType = 2;

                pOrder.bstrTPOrderPrice = textBoxbstrTPOrderPriceDT.Text;

                if (comboBoxnStopLossFlagDT.Text == "0:") pOrder.nStopLossFlag = 0;
                else if (comboBoxnStopLossFlagDT.Text == "1:") pOrder.nStopLossFlag = 1;

                if (comboBoxnRDOSLPercent.Text == "0:") pOrder.nRDOSLPercent = 0;
                else if (comboBoxnRDOSLPercent.Text == "1:") pOrder.nRDOSLPercent = 1;

                pOrder.bstrSLPercent = textBoxbstrSLPercentDT.Text;
                pOrder.bstrSLTrigger = textBoxbstrSLTrigger.Text;

                if (comboBoxnRDSLMarketPriceType.Text == "1:") pOrder.nRDSLMarketPriceType = 1;
                else if (comboBoxnRDSLMarketPriceType.Text == "2:") pOrder.nRDSLMarketPriceType = 2;

                pOrder.bstrSLOrderPrice = textBoxbstrSLOrderPriceDT.Text;

                if (comboBoxnTakeProfitOrderCond.Text == "0:ROD") pOrder.nTakeProfitOrderCond = 0;
                else if (comboBoxnTakeProfitOrderCond.Text == "3:IOC") pOrder.nTakeProfitOrderCond = 3;
                else if (comboBoxnTakeProfitOrderCond.Text == "4:FOK") pOrder.nTakeProfitOrderCond = 4;

                if (comboBoxnStopLossOrderCond.Text == "0:ROD") pOrder.nStopLossOrderCond = 0;
                else if (comboBoxnStopLossOrderCond.Text == "3:IOC") pOrder.nStopLossOrderCond = 3;
                else if (comboBoxnStopLossOrderCond.Text == "4:FOK") pOrder.nStopLossOrderCond = 4;

                if (comboBoxnClearOrderCondDT.Text == "0:ROD") pOrder.nClearOrderCond = 0;
                else if (comboBoxnClearOrderCondDT.Text == "3:IOC") pOrder.nClearOrderCond = 3;
                else if (comboBoxnClearOrderCondDT.Text == "4:FOK") pOrder.nClearOrderCond = 4;

                string bstrMessage;
                // 。
                int nCode = m_pSKOrder.SendStockStrategyDayTrade(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyDayTrade】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSendStockStrategyClear_Click(object sender, EventArgs e)
        {
            if (textBoxnQtyC.Text == "") // ，!
            {
                MessageBox.Show("()！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDEROUT pOrder = new STOCKSTRATEGYORDEROUT();
                pOrder.bstrFullAccount = comboBoxAccount.Text;
                pOrder.bstrStockNo = textBoxbstrStockNoC.Text;
                pOrder.bstrClearCancelTime = textBoxbstrClearCancelTimeC.Text;
                pOrder.bstrClearAllOrderPrice = textBoxbstrClearAllOrderPriceC.Text;

                pOrder.bstrLTETriggerPrice = textBoxbstrLTETriggerPrice.Text;
                pOrder.bstrLTEOrderPrice = textBoxbstrLTEOrderPrice.Text;
                pOrder.bstrGTETriggerPrice = textBoxbstrGTETriggerPrice.Text;
                pOrder.bstrGTEOrderPrice = textBoxbstrGTEOrderPrice.Text;

                pOrder.nQty = int.Parse(textBoxnQtyC.Text);

                if (comboBoxnBuySellC.Text == "0:") pOrder.nBuySell = 0;
                else if (comboBoxnBuySellC.Text == "1:") pOrder.nBuySell = 1;

                if (comboBoxnClearAllFlagC.Text == "0:") pOrder.nClearAllFlag = 0;
                else if (comboBoxnClearAllFlagC.Text == "1:") pOrder.nClearAllFlag = 1;

                if (comboBoxnClearAllPriceTypeC.Text == "1:") pOrder.nClearAllPriceType = 1;
                else if (comboBoxnClearAllPriceTypeC.Text == "2:") pOrder.nClearAllPriceType = 2;

                if (comboBoxnFinalClearFlagC.Text == "0:") pOrder.nFinalClearFlag = 0;
                else if (comboBoxnFinalClearFlagC.Text == "1:") pOrder.nFinalClearFlag = 1;

                if (comboBoxnOrderTypeC.Text == "0:") pOrder.nOrderType = 0;
                else if (comboBoxnOrderTypeC.Text == "3:") pOrder.nOrderType = 3;
                else if (comboBoxnOrderTypeC.Text == "4:") pOrder.nOrderType = 4;

                if (comboBoxnLTEFlag.Text == "0:") pOrder.nLTEFlag = 0;
                else if (comboBoxnLTEFlag.Text == "1:") pOrder.nLTEFlag = 1;

                if (comboBoxnLTEMarketPrice.Text == "1:") pOrder.nLTEMarketPrice = 1;
                else if (comboBoxnLTEMarketPrice.Text == "2:") pOrder.nLTEMarketPrice = 2;

                if (comboBoxnGTEFlag.Text == "0:") pOrder.nGTEFlag = 0;
                else if (comboBoxnGTEFlag.Text == "1:") pOrder.nGTEFlag = 1;

                if (comboBoxnGTEMarketPrice.Text == "1:") pOrder.nGTEMarketPrice = 1;
                else if (comboBoxnGTEMarketPrice.Text == "2:") pOrder.nGTEMarketPrice = 2;

                if (comboBoxnGTEOrderCond.Text == "0:ROD") pOrder.nGTEOrderCond = 0;
                else if (comboBoxnGTEOrderCond.Text == "3:IOC") pOrder.nGTEOrderCond = 3;
                else if (comboBoxnGTEOrderCond.Text == "4:FOK") pOrder.nGTEOrderCond = 4;

                if (comboBoxnLTEOrderCond.Text == "0:ROD") pOrder.nLTEOrderCond = 0;
                else if (comboBoxnLTEOrderCond.Text == "3:IOC") pOrder.nLTEOrderCond = 3;
                else if (comboBoxnLTEOrderCond.Text == "4:FOK") pOrder.nLTEOrderCond = 4;

                if (comboBoxnClearOrderCondC.Text == "0:ROD") pOrder.nClearOrderCond = 0;
                else if (comboBoxnClearOrderCondC.Text == "3:IOC") pOrder.nClearOrderCond = 3;
                else if (comboBoxnClearOrderCondC.Text == "4:FOK") pOrder.nClearOrderCond = 4;

                string bstrMessage;
                // 。
                int nCode = m_pSKOrder.SendStockStrategyClear(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyClear】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSendStockStrategyMIT_Click(object sender, EventArgs e)
        {
            if (textBoxnQtyMIT.Text == "") // ，!
            {
                MessageBox.Show("()！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDERMIT pOrder = new STOCKSTRATEGYORDERMIT();
                pOrder.bstrFullAccount = comboBoxAccount.Text;
                pOrder.bstrStockNo = textBoxbstrStockNoMIT.Text;
                pOrder.bstrOrderPrice = textBoxbstrOrderPriceMIT.Text;
                pOrder.bstrTriggerPrice = textBoxbstrTriggerPriceMIT.Text;
                pOrder.bstrDealPrice = textBoxbstrDealPriceMIT.Text;
                pOrder.bstrLongEndDate = textBoxbstrLongEndDateMIT.Text;
                pOrder.nQty = int.Parse(textBoxnQtyMIT.Text);

                if (comboBoxnOrderTypeMIT.Text == "0:") pOrder.nOrderType = 0;
                else if (comboBoxnOrderTypeMIT.Text == "3:") pOrder.nOrderType = 3;
                else if (comboBoxnOrderTypeMIT.Text == "4:") pOrder.nOrderType = 4;
                else if (comboBoxnOrderTypeMIT.Text == "8:") pOrder.nOrderType = 8;

                if (comboBoxnBuySellMIT.Text == "0:") pOrder.nBuySell = 0;
                else if (comboBoxnBuySellMIT.Text == "1:") pOrder.nBuySell = 1;

                if (comboBoxnOrderPriceTypeMIT.Text == "1:") pOrder.nOrderPriceType = 1;
                else if (comboBoxnOrderPriceTypeMIT.Text == "2:") pOrder.nOrderPriceType = 2;

                if (comboBoxnOrderCondMIT.Text == "0:ROD") pOrder.nOrderCond = 0;
                else if (comboBoxnOrderCondMIT.Text == "3:IOC") pOrder.nOrderCond = 3;
                else if (comboBoxnOrderCondMIT.Text == "4:FOK") pOrder.nOrderCond = 4;

                if (comboBoxnTriggerDirMIT.Text == "1:GTE") pOrder.nTriggerDir = 1;
                else if (comboBoxnTriggerDirMIT.Text == "2:LTE") pOrder.nTriggerDir = 2;

                if (comboBoxnPreRiskFlag.Text == "0:") pOrder.nPreRiskFlag = 0;
                else if (comboBoxnPreRiskFlag.Text == "1:") pOrder.nPreRiskFlag = 1;

                if (comboBoxnLongActionFlagMIT.Text == "0:") pOrder.nLongActionFlag = 0;
                else if (comboBoxnLongActionFlagMIT.Text == "1:") pOrder.nLongActionFlag = 1;

                if (comboBoxnLATypeMIT.Text == "1:") pOrder.nLAType = 1;
                else if (comboBoxnLATypeMIT.Text == "3:") pOrder.nLAType = 3;

                string bstrMessage;
                // MIT。
                int nCode = m_pSKOrder.SendStockStrategyMIT(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyMIT】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSendStockStrategyOCO_Click(object sender, EventArgs e)
        {
            if (textBoxnQtyOCO.Text == "") // ，!
            {
                MessageBox.Show("()！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDEROCO pOrder = new STOCKSTRATEGYORDEROCO();
                pOrder.bstrFullAccount = comboBoxAccount.Text;
                pOrder.bstrStockNo = textBoxbstrStockNoOCO.Text;
                pOrder.nQty = int.Parse(textBoxnQtyOCO.Text);
                pOrder.bstrOrderPrice = textBoxbstrOrderPriceOCO.Text;

                pOrder.bstrTriggerUp = textBoxbstrTriggerUp.Text;
                pOrder.bstrOrderPrice2 = textBoxbstrOrderPrice2.Text;
                pOrder.bstrTriggerDown = textBoxbstrTriggerDown.Text;

                if (comboBoxnBuySellUp.Text == "0:") pOrder.nBuySellUp = 0;
                else if (comboBoxnBuySellUp.Text == "1:") pOrder.nBuySellUp = 1;

                if (comboBoxnBuySellDown.Text == "0:") pOrder.nBuySellDown = 0;
                else if (comboBoxnBuySellDown.Text == "1:") pOrder.nBuySellDown = 1;

                if (comboBoxnOrderCondUp.Text == "0:ROD") pOrder.nOrderCondUp = 0;
                else if (comboBoxnOrderCondUp.Text == "3:IOC") pOrder.nOrderCondUp = 3;
                else if (comboBoxnOrderCondUp.Text == "4:FOK") pOrder.nOrderCondUp = 4;

                if (comboBoxnOrderCondDown.Text == "0:ROD") pOrder.nOrderCondDown = 0;
                else if (comboBoxnOrderCondDown.Text == "3:IOC") pOrder.nOrderCondDown = 3;
                else if (comboBoxnOrderCondDown.Text == "4:FOK") pOrder.nOrderCondDown = 4;

                if (comboBoxnOrderTypeUp.Text == "0:") pOrder.nOrderTypeUp = 0;
                else if (comboBoxnOrderTypeUp.Text == "3:") pOrder.nOrderTypeUp = 3;
                else if (comboBoxnOrderTypeUp.Text == "4:") pOrder.nOrderTypeUp = 4;
                else if (comboBoxnOrderTypeUp.Text == "8:") pOrder.nOrderTypeUp = 8;

                if (comboBoxnOrderTypeDown.Text == "0:") pOrder.nOrderTypeDown = 0;
                else if (comboBoxnOrderTypeDown.Text == "3:") pOrder.nOrderTypeDown = 3;
                else if (comboBoxnOrderTypeDown.Text == "4:") pOrder.nOrderTypeDown = 4;
                else if (comboBoxnOrderTypeDown.Text == "8:") pOrder.nOrderTypeDown = 8;

                if (comboBoxnOrderPriceTypeUp.Text == "1:") pOrder.nOrderPriceTypeUp = 1;
                else if (comboBoxnOrderPriceTypeUp.Text == "2:") pOrder.nOrderPriceTypeUp = 2;

                if (comboBoxnOrderPriceTypeDown.Text == "1:") pOrder.nOrderPriceTypeDown = 1;
                else if (comboBoxnOrderPriceTypeDown.Text == "2:") pOrder.nOrderPriceTypeDown = 2;

                string bstrMessage;
                // OCO。
                int nCode = m_pSKOrder.SendStockStrategyOCO(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyOCO】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSendStockStrategyMIOC_Click(object sender, EventArgs e)
        {
            if (textBoxnOneceQtyLimit.Text == "" || textBoxnTotalQty.Text == "") // ，!
            {
                MessageBox.Show("(/)！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDERMIOC pOrder = new STOCKSTRATEGYORDERMIOC();
                pOrder.bstrFullAccount = comboBoxAccount.Text;
                pOrder.bstrStockNo = textBoxbstrStockNoMIOC.Text;
                pOrder.bstrOrderPriceUp = textBoxbstrOrderPriceUp.Text;
                pOrder.bstrOrderPriceDown = textBoxbstrOrderPriceDown.Text;
                pOrder.nOneceQtyLimit = int.Parse(textBoxnOneceQtyLimit.Text);
                pOrder.nTotalQty = int.Parse(textBoxnTotalQty.Text);

                if (comboBoxnPrimeMIOC.Text == "0:") pOrder.nPrime = 0;
                else if (comboBoxnPrimeMIOC.Text == "1:") pOrder.nPrime = 1;

                if (comboBoxnBuySellMIOC1.Text == "1:") pOrder.nBuySell = 1;
                else if (comboBoxnBuySellMIOC1.Text == "2:") pOrder.nBuySell = 2;

                if (comboBoxnOrderTypeMIOC.Text == "0:") pOrder.nOrderType = 0;
                else if (comboBoxnOrderTypeMIOC.Text == "3:") pOrder.nOrderType = 3;
                else if (comboBoxnOrderTypeMIOC.Text == "4:") pOrder.nOrderType = 4;
                else if (comboBoxnOrderTypeMIOC.Text == "8:") pOrder.nOrderType = 8;

                if (comboBoxnOrderPriceTypeMIOC.Text == "0:") pOrder.nOrderPriceType = 1;
                else if (comboBoxnOrderPriceTypeMIOC.Text == "1:()()") pOrder.nOrderPriceType = 2;

                string bstrMessage;
                // IOC。
                int nCode = m_pSKOrder.SendStockStrategyMIOC(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyMIOC】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSendStockStrategyMST_Click(object sender, EventArgs e)
        {
            if (textBoxnQtyMST.Text == "") // ，!
            {
                MessageBox.Show("()！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDERMIT pOrder = new STOCKSTRATEGYORDERMIT();
                pOrder.bstrFullAccount = comboBoxAccount.Text;
                pOrder.bstrStockNo = textBoxbstrStockNoMST.Text;
                pOrder.bstrOrderPrice = textBoxbstrOrderPriceMST.Text;
                pOrder.bstrMovePoint = textBoxbstrMovePoint.Text;
                pOrder.bstrDealPrice = textBoxbstrDealPriceMST.Text;
                pOrder.bstrStartPrice = textBoxbstrStartPriceMST.Text;
                pOrder.nQty = int.Parse(textBoxnQtyMST.Text);
                pOrder.bstrLongEndDate = textBoxbstrLongEndDateMST.Text;

                if (comboBoxnOrderTypeMST.Text == "0:") pOrder.nOrderType = 0;
                else if (comboBoxnOrderTypeMST.Text == "3") pOrder.nOrderType = 3;
                else if (comboBoxnOrderTypeMST.Text == "4:") pOrder.nOrderType = 4;
                else if (comboBoxnOrderTypeMST.Text == "8:") pOrder.nOrderType = 8;

                if (comboBoxnBuySellMST.Text == "0:") pOrder.nBuySell = 0;
                else if (comboBoxnBuySellMST.Text == "1:") pOrder.nBuySell = 1;

                if (comboBoxnOrderPriceTypeMST.Text == "1:") pOrder.nOrderPriceType = 1;
                else if (comboBoxnOrderPriceTypeMST.Text == "2:") pOrder.nOrderPriceType = 2;

                if (comboBoxnOrderCondMST.Text == "0:ROD") pOrder.nOrderCond = 0;
                else if (comboBoxnOrderCondMST.Text == "3:IOC") pOrder.nOrderCond = 3;
                else if (comboBoxnOrderCondMST.Text == "4:FOK") pOrder.nOrderCond = 4;

                if (comboBoxnTriggerDirMST.Text == "1:GTE") pOrder.nTriggerDir = 1;
                else if (comboBoxnTriggerDirMST.Text == "2:LTE") pOrder.nTriggerDir = 2;

                if (comboBoxnTriggerMethod.Text == "0:,") pOrder.nTriggerMethod = 0;
                else if (comboBoxnTriggerMethod.Text == "1:,") pOrder.nTriggerMethod = 1;

                if (comboBoxnPrimeMST.Text == "0:") pOrder.nPrime = 0;
                else if (comboBoxnPrimeMST.Text == "1:") pOrder.nPrime = 1;

                if (comboBoxnLongActionFlagMST.Text == "0:") pOrder.nLongActionFlag = 0;
                else if (comboBoxnLongActionFlagMST.Text == "1:") pOrder.nLongActionFlag = 1;

                if (comboBoxnLATypeMST.Text == "0:") pOrder.nLAType = 0;
                else if (comboBoxnLATypeMST.Text == "1:") pOrder.nLAType = 1;

                string bstrMessage;
                // 。
                int nCode = m_pSKOrder.SendStockStrategyMST(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyMST】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSendStockStrategyAB_Click(object sender, EventArgs e)
        {
            if (textBoxnQtyAB.Text == "") // ，!
            {
                MessageBox.Show("()！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDERMIT pOrder = new STOCKSTRATEGYORDERMIT();
                pOrder.bstrFullAccount = comboBoxAccount.Text;
                pOrder.bstrStockNo = textBoxbstrStockNoAB.Text;
                pOrder.bstrOrderPrice = textBoxbstrOrderPriceAB.Text;
                pOrder.bstrTriggerPrice = textBoxbstrTriggerPriceAB.Text;
                pOrder.bstrExchangeNo = textBoxbstrExchangeNo.Text;
                pOrder.bstrStockNo2 = textBoxbstrStockNo2.Text;
                pOrder.bstrStartPrice = textBoxbstrStartPriceAB.Text;
                pOrder.nQty = int.Parse(textBoxnQtyAB.Text);

                if (comboBoxnOrderTypeAB.Text == "0:") pOrder.nOrderType = 0;
                else if (comboBoxnOrderTypeAB.Text == "3") pOrder.nOrderType = 3;
                else if (comboBoxnOrderTypeAB.Text == "4:") pOrder.nOrderType = 4;
                else if (comboBoxnOrderTypeAB.Text == "8:") pOrder.nOrderType = 8;

                if (comboBoxnBuySellAB.Text == "0:") pOrder.nBuySell = 0;
                else if (comboBoxnBuySellAB.Text == "1:") pOrder.nBuySell = 1;

                if (comboBoxnOrderPriceTypeAB.Text == "1:") pOrder.nOrderPriceType = 1;
                else if (comboBoxnOrderPriceTypeAB.Text == "2:") pOrder.nOrderPriceType = 2;

                if (comboBoxnOrderCondAB.Text == "0:ROD") pOrder.nOrderCond = 0;
                else if (comboBoxnOrderCondAB.Text == "3:IOC") pOrder.nOrderCond = 3;
                else if (comboBoxnOrderCondAB.Text == "4:FOK") pOrder.nOrderCond = 4;

                if (comboBoxnTriggerDirAB.Text == "1:GTE") pOrder.nTriggerDir = 1;
                else if (comboBoxnTriggerDirAB.Text == "2:LTE") pOrder.nTriggerDir = 2;

                if (comboBoxnMarketNo.Text == "1:") pOrder.nMarketNo = 1;
                else if (comboBoxnMarketNo.Text == "2:") pOrder.nMarketNo = 2;
                else if (comboBoxnMarketNo.Text == "3:") pOrder.nMarketNo = 3;
                else if (comboBoxnMarketNo.Text == "4:") pOrder.nMarketNo = 4;

                if (comboBoxnReserved.Text == "0:") pOrder.nReserved = 0;
                else if (comboBoxnReserved.Text == "1:") pOrder.nReserved = 1;

                if (comboBoxnPrimeAB.Text == "0:") pOrder.nPrime = 1;
                else if (comboBoxnPrimeAB.Text == "1:") pOrder.nPrime = 2;

                string bstrMessage;
                // AB。
                int nCode = m_pSKOrder.SendStockStrategyAB(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyAB】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonSendStockStrategyCB_Click(object sender, EventArgs e)
        {
            if (textBoxnQtyCB.Text == "") // ，!
            {
                MessageBox.Show("()！", "", MessageBoxButtons.OK);
            }
            else
            {
                STOCKSTRATEGYORDER pOrder = new STOCKSTRATEGYORDER();
                pOrder.bstrFullAccount = comboBoxAccount.Text;
                pOrder.bstrStockNo = textBoxbstrStockNoCB.Text;
                pOrder.nQty = int.Parse(textBoxnQtyCB.Text);
                pOrder.bstrOrderPrice = textBoxbstrOrderPriceCB.Text;
                pOrder.bstrMITTriggerPrice = textBoxbstrMITTriggerPriceCB.Text;
                pOrder.bstrTPOrderPrice = textBoxbstrTPOrderPriceCB.Text;
                pOrder.bstrSLOrderPrice = textBoxbstrSLOrderPriceCB.Text;
                pOrder.bstrTick = textBoxbstrTickCB.Text;
                pOrder.bstrSLPercent = textBoxbstrSLPercentCB.Text;
                pOrder.bstrPreQty = textBoxbstrPreQtyCB.Text;
                pOrder.bstrSumQty = textBoxbstrSumQtyCB.Text;
                pOrder.bstrClearCancelTime = textBoxbstrClearCancelTimeCB.Text;

                if (comboBoxnOrderTypeCB.Text == "0:") pOrder.nOrderType = 0;
                else if (comboBoxnOrderTypeCB.Text == "3:") pOrder.nOrderType = 3;
                else if (comboBoxnOrderTypeCB.Text == "4:") pOrder.nOrderType = 4;
                else if (comboBoxnOrderTypeCB.Text == "8:") pOrder.nOrderType = 8;

                if (comboBoxnOrderPriceCondCB.Text == "0:ROD") pOrder.nOrderPriceCond = 0;
                else if (comboBoxnOrderPriceCondCB.Text == "3:IOC") pOrder.nOrderPriceCond = 3;
                else if (comboBoxnOrderPriceCondCB.Text == "4:FOK") pOrder.nOrderPriceCond = 4;

                if (comboBoxnBuySellCB.Text == "0:") pOrder.nBuySell = 0;
                else if (comboBoxnBuySellCB.Text == "1:") pOrder.nBuySell = 1;

                if (comboBoxnOrderPriceTypeCB.Text == "1:") pOrder.nOrderPriceType = 1;
                else if (comboBoxnOrderPriceTypeCB.Text == "2:") pOrder.nOrderPriceType = 2;

                if (comboBoxnInnerOrderIsMITCB.Text == "0:") pOrder.nInnerOrderIsMIT = 0;
                else if (comboBoxnInnerOrderIsMITCB.Text == "1:") pOrder.nInnerOrderIsMIT = 1;

                if (comboBoxnTPDir.Text == "0:None") pOrder.nTPDir = 0;
                else if (comboBoxnTPDir.Text == "1:GTE") pOrder.nTPDir = 1;
                else if (comboBoxnTPDir.Text == "2:LTE") pOrder.nTPDir = 2;

                if (comboBoxnStopLossFlagCB.Text == "0:") pOrder.nStopLossFlag = 0;
                else if (comboBoxnStopLossFlagCB.Text == "1:") pOrder.nStopLossFlag = 1;

                if (comboBoxnSLDirCB.Text == "0:None") pOrder.nSLDir = 0;
                else if (comboBoxnSLDirCB.Text == "1:GTE") pOrder.nSLDir = 1;
                else if (comboBoxnSLDirCB.Text == "2:LTE") pOrder.nSLDir = 2;

                if (comboBoxnTickFlagCB.Text == "0:") pOrder.nTickFlag = 0;
                else if (comboBoxnTickFlagCB.Text == "1:") pOrder.nTickFlag = 1;

                if (comboBoxnTickDir.Text == "0:None") pOrder.nTickDir = 0;
                else if (comboBoxnTickDir.Text == "1:GTE") pOrder.nTickDir = 1;
                else if (comboBoxnTickDir.Text == "2:LTE") pOrder.nTickDir = 2;

                if (comboBoxnUpDownFlag.Text == "0:") pOrder.nUpDownFlag = 0;
                else if (comboBoxnUpDownFlag.Text == "1:") pOrder.nUpDownFlag = 1;

                if (comboBoxnUpDownDir.Text == "0:None") pOrder.nUpDownDir = 0;
                else if (comboBoxnUpDownDir.Text == "1:GTE") pOrder.nUpDownDir = 1;
                else if (comboBoxnUpDownDir.Text == "2:LTE") pOrder.nUpDownDir = 2;

                if (comboBoxnPreQtyFlagCB.Text == "0:") pOrder.nPreQtyFlag = 0;
                else if (comboBoxnPreQtyFlagCB.Text == "1:") pOrder.nPreQtyFlag = 1;

                if (comboBoxnPreQtyDirCB.Text == "0:None") pOrder.nPreQtyDir = 0;
                else if (comboBoxnPreQtyDirCB.Text == "1:GTE") pOrder.nPreQtyDir = 1;
                else if (comboBoxnPreQtyDirCB.Text == "2:LTE") pOrder.nPreQtyDir = 2;

                if (comboBoxnSumQtyFlagCB.Text == "0:") pOrder.nSumQtyFlag = 0;
                else if (comboBoxnSumQtyFlagCB.Text == "1:") pOrder.nSumQtyFlag = 1;

                if (comboBoxnSumQtyDir.Text == "0:None") pOrder.nSumQtyDir = 0;
                else if (comboBoxnSumQtyDir.Text == "1:GTE") pOrder.nSumQtyDir = 1;
                else if (comboBoxnSumQtyDir.Text == "2:LTE") pOrder.nSumQtyDir = 2;

                if (comboBoxnClearAllFlagCB.Text == "0:") pOrder.nClearAllFlag = 0;
                else if (comboBoxnClearAllFlagCB.Text == "1:") pOrder.nClearAllFlag = 1;

                if (comboBoxnFinalClearFlagCB.Text == "0:") pOrder.nFinalClearFlag = 0;
                else if (comboBoxnFinalClearFlagCB.Text == "1:") pOrder.nFinalClearFlag = 1;

                if (comboBoxnMITDirCB.Text == "0:None") pOrder.nMITDir = 0;
                else if (comboBoxnMITDirCB.Text == "1:GTE") pOrder.nMITDir = 1;
                else if (comboBoxnMITDirCB.Text == "2:LTE") pOrder.nMITDir = 2;

                if (comboBoxnTakeProfitFlagCB.Text == "0:") pOrder.nTakeProfitFlag = 0;
                else if (comboBoxnTakeProfitFlagCB.Text == "1:") pOrder.nTakeProfitFlag = 1;

                string bstrMessage;
                // 。
                int nCode = m_pSKOrder.SendStockStrategyCB(comboBoxUserID.Text, bAsyncOrder, ref pOrder, out bstrMessage);
                // 
                string msg = "【SendStockStrategyCB】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }

        private void comboBoxnTradeKindV1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(comboBoxnTradeKindV1.Text == "11:")
            {
                labelbstrParentSmartKey.Visible = true;
                textBoxbstrParentSmartKey.Visible = true;
                labelnDeletType.Visible = true;
                comboBoxnDeletType.Visible = true;
                labelbstrSmartKeyOut.Visible = true;
                label3.Visible = true;
                label4.Visible = true;
                textBoxbstrSmartKeyOut.Visible = true;
            }
            else
            {
                labelbstrParentSmartKey.Visible = false;
                textBoxbstrParentSmartKey.Visible = false;
                labelnDeletType.Visible = false;
                comboBoxnDeletType.Visible = false;
                labelbstrSmartKeyOut.Visible = false;
                label3.Visible = false;
                label4.Visible = false;
                textBoxbstrSmartKeyOut.Visible = false;
            }
        }

        private void comboBoxnTradeKindVersion_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxnTradeKindVersion.Text == "")
            {
                labelnTradeKind2.Visible = true;
                comboBoxnTradeKind.Visible = true;
                buttonCancelTSStrategyOrder.Visible = true;

                labelnTradeKind.Visible = false;
                comboBoxnTradeKindV1.Visible = false;
                buttonCancelTSStrategyOrderV1.Visible = false;
            }
            else
            {
                labelnTradeKind2.Visible = false;
                comboBoxnTradeKind.Visible = false;
                buttonCancelTSStrategyOrder.Visible = false;

                labelnTradeKind.Visible = true;
                comboBoxnTradeKindV1.Visible = true;
                buttonCancelTSStrategyOrderV1.Visible = true;
            }
        }

        private void buttonCancelStrategyList_Click(object sender, EventArgs e)
        {
            CANCELSTRATEGYORDER pCancelOrder = new CANCELSTRATEGYORDER(); // 
            pCancelOrder.bstrLogInID = comboBoxUserID.Text;
            pCancelOrder.bstrFullAccount = comboBoxAccount.Text;
            // 1：、2：、3：、4：
            if (comboBoxnMarket.Text == "1：") pCancelOrder.nMarket = 1;
            else if (comboBoxnMarket.Text == "2：") pCancelOrder.nMarket = 2;
            else if (comboBoxnMarket.Text == "3：") pCancelOrder.nMarket = 3;
            else if (comboBoxnMarket.Text == "4：") pCancelOrder.nMarket = 4;

            pCancelOrder.bstrSmartKey = textBoxbstrSmartKey.Text; // 

            string bstrMessage;
            // 。。
            int nCode = m_pSKOrder.CancelStrategyList(comboBoxUserID.Text, pCancelOrder, out bstrMessage);
            // 
            string msg = "【CancelStrategyList】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
    }
}
