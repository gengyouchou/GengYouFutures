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
    public partial class TSTFUpdateOrderForm : Form
    {
        // 
        SKCenterLib m_pSKCenter = new SKCenterLib(); //&
        SKOrderLib m_pSKOrder = new SKOrderLib(); //
        // 
        bool bAsyncOrder = false;
        // [UserID] 
        Dictionary<string, List<string>> m_dictUserID = new Dictionary<string, List<string>>();
        List<string> allkeys;
        static void AddUserID(Dictionary<string, List<string>> dictUserID, string UserID, string AccountData)
        {
            string[] values = AccountData.Split(',');
            string Account = values[1] + values[3]; // broker ID (IB)4 + 7
            if (dictUserID.ContainsKey(UserID))
            {
                dictUserID[UserID].Add(Account);
            }
            else
            {
                dictUserID[UserID] = new List<string> { Account };
            }
        }
        public TSTFUpdateOrderForm()
        {
            // Init
            {
                InitializeComponent();
                //comboBox
                {
                    // comboBoxTradeType
                    {
                        comboBoxTradeType.Items.Add("0:ROD");
                        comboBoxTradeType.Items.Add("1:IOC");
                        comboBoxTradeType.Items.Add("2:FOK");
                    }
                    // comboBoxCorrectPriceByBookNoMarketSymbol
                    {
                        comboBoxMarketSymbol.Items.Add("TS:");
                        comboBoxMarketSymbol.Items.Add("TF:");
                        comboBoxMarketSymbol.Items.Add("TO:");
                    }
                    // comboBoxUpdateTFOrder
                    {
                        comboBoxUpdateTFOrder.Items.Add("");
                        comboBoxUpdateTFOrder.Items.Add("");
                        comboBoxUpdateTFOrder.Items.Add("");
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
        private void buttonCancelOrderBySeqNo_Click(object sender, EventArgs e)
        {
            string bstrSeqNo = textBoxSeqNo.Text;
            string bstrAccount = comboBoxAccount.Text;

            string bstrMessage; //： 0，。0，。：4 - 2 - b OnAsyncOrder。

            // (By)          
            int nCode = m_pSKOrder.CancelOrderBySeqNo(comboBoxUserID.Text, bAsyncOrder, bstrAccount, bstrSeqNo, out bstrMessage);
            // 
            string msg = "【CancelOrderBySeqNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode) + bstrMessage;
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonCancelOrderByBookNo_Click(object sender, EventArgs e)
        {
            string bstrBookNo = textBoxBookNo.Text;
            string bstrAccount = comboBoxAccount.Text;
            string bstrMessage; //： 0，。0，。：4 - 2 - b OnAsyncOrder。

            // (By)          
            int nCode = m_pSKOrder.CancelOrderByBookNo(comboBoxUserID.Text, bAsyncOrder, bstrAccount, bstrBookNo, out bstrMessage);
            // 
            string msg = "【CancelOrderByBookNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode) + bstrMessage;
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonCancelOrderByStockNo_Click(object sender, EventArgs e)
        {
            string bstrStockNo = textBoxCancelOrderByStockNo.Text;
            string bstrAccount = comboBoxAccount.Text;
            string bstrMessage; //： 0，。0，。：4 - 2 - b OnAsyncOrder。

            // (By ID+)          
            int nCode = m_pSKOrder.CancelOrderByStockNo(comboBoxUserID.Text, bAsyncOrder, bstrAccount, bstrStockNo, out bstrMessage);
            // 
            string msg = "【CancelOrderByStockNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode) + bstrMessage;
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonDecreaseOrderBySeqNo_Click(object sender, EventArgs e)
        {
            if (textBoxStockDecreaseQty.Text == "") // ，!
            {
                MessageBox.Show("！", "", MessageBoxButtons.OK);
            }
            else
            {
                string bstrSeqNo = textBoxSeqNo.Text;
                string bstrAccount = comboBoxAccount.Text;
                int nDecreaseQty = int.Parse(textBoxStockDecreaseQty.Text);
                string bstrMessage; //： 0，。0，。：4 - 2 - b OnAsyncOrder。

                // (By)          
                int nCode = m_pSKOrder.DecreaseOrderBySeqNo(comboBoxUserID.Text, bAsyncOrder, bstrAccount, bstrSeqNo, nDecreaseQty, out bstrMessage);
                // 
                string msg = "【DecreaseOrderBySeqNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode) + bstrMessage;
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void buttonCorrectPriceBySeqNo_Click(object sender, EventArgs e)
        {
            string bstrAccount = comboBoxAccount.Text;
            string bstrSeqNo = textBoxSeqNo.Text; // 
            string bstrPrice = textBoxPrice.Text; // 
            int nTradeType = 0; // 0:ROD 0: ROD  1:IOC  2:FOK

            string selectedValue2 = comboBoxTradeType.Text;
            if (selectedValue2 == "0:ROD") nTradeType = 0;
            else if (selectedValue2 == "1:IOC") nTradeType = 1;
            else if (selectedValue2 == "2:FOK") nTradeType = 2;
            string bstrMessage; //： 0，。0，。：4 - 2 - b OnAsyncOrder。

            // (By)          
            int nCode = m_pSKOrder.CorrectPriceBySeqNo(comboBoxUserID.Text, bAsyncOrder, bstrAccount, bstrSeqNo, bstrPrice, nTradeType, out bstrMessage);
            // 
            string msg = "【CorrectPriceBySeqNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode) + bstrMessage;
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonCorrectPriceByBookNo_Click(object sender, EventArgs e)
        {
            string bstrAccount = comboBoxAccount.Text;
            string bstrMarketSymbol = ""; //  TS: TF: TO:
            string selectedValue3 = comboBoxMarketSymbol.Text;
            if (selectedValue3 == "TS:") bstrMarketSymbol = "TS";
            else if (selectedValue3 == "TF:") bstrMarketSymbol = "TF";
            else if (selectedValue3 == "TO:") bstrMarketSymbol = "TO";

            string bstrBookNo = textBoxBookNo.Text; // 
            string bstrPrice = textBoxPrice.Text; // 
            int nTradeType = 0; // 0:ROD 0: ROD  1:IOC  2:FOK

            string selectedValue2 = comboBoxTradeType.Text;
            if (selectedValue2 == "0:ROD") nTradeType = 0;
            else if (selectedValue2 == "1:IOC") nTradeType = 1;
            else if (selectedValue2 == "2:FOK") nTradeType = 2;

            string bstrMessage; //： 0，。0，。：4 - 2 - b OnAsyncOrder。

            // (By)          
            int nCode = m_pSKOrder.CorrectPriceByBookNo(comboBoxUserID.Text, bAsyncOrder, bstrAccount, bstrMarketSymbol, bstrBookNo, bstrPrice, nTradeType, out bstrMessage);
            // 
            string msg = "【CorrectPriceByBookNo】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode) + bstrMessage;
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void comboBoxUserID_DropDown(object sender, EventArgs e)
        {
            m_dictUserID.Clear(); //
            // 
            {
                int nCode = m_pSKOrder.GetUserAccount();
                // 
                string msg = "【GetUserAccount】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
               richTextBoxMethodMessage.AppendText(msg + "\n"); 
            }
        }
        private void comboBoxUpdateTFOrder_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxUpdateTFOrder.Text == "")
            {
                // 
                {
                    // Stock
                    {
                        labelCancelOrderByStockNo2.Visible = true;
                        buttonCancelOrderBySeqNo.Visible = true;
                        buttonCancelOrderByBookNo.Visible = true;
                        buttonCancelOrderByStockNo.Visible = true;
                    }
                }
                // 
                {
                    // Stock
                    {
                        labelDecreaseOrderBySeqNo.Visible = false;
                        textBoxStockDecreaseQty.Visible = false;
                        buttonDecreaseOrderBySeqNo.Visible = false;
                    }
                }
                // 
                {
                    // Stock
                    {
                        labelCorrectPriceBySeqNo.Visible = false;
                        textBoxPrice.Visible = false;
                        labelTradeType.Visible = false;
                        comboBoxTradeType.Visible = false;
                        labelCorrectPriceByBookNoMarketSymbol.Visible = false;
                        comboBoxMarketSymbol.Visible = false;
                        buttonCorrectPriceBySeqNo.Visible = false;
                        buttonCorrectPriceByBookNo.Visible = false;
                    }
                }
            }
            else if (comboBoxUpdateTFOrder.Text == "")
            {
                // 
                {
                    // Stock
                    {
                        labelCancelOrderByStockNo2.Visible = false;
                        buttonCancelOrderBySeqNo.Visible = false;
                        buttonCancelOrderByBookNo.Visible = false;
                        buttonCancelOrderByStockNo.Visible = false;
                    }
                }
                // 
                {
                    // Stock
                    {
                        labelDecreaseOrderBySeqNo.Visible = true;
                        textBoxStockDecreaseQty.Visible = true;
                        buttonDecreaseOrderBySeqNo.Visible = true;
                    }
                }
                // 
                {
                    // Stock
                    {
                        labelCorrectPriceBySeqNo.Visible = false;
                        textBoxPrice.Visible = false;
                        labelTradeType.Visible = false;
                        comboBoxTradeType.Visible = false;
                        labelCorrectPriceByBookNoMarketSymbol.Visible = false;
                        comboBoxMarketSymbol.Visible = false;
                        buttonCorrectPriceBySeqNo.Visible = false;
                        buttonCorrectPriceByBookNo.Visible = false;
                    }
                }
            }
            else if (comboBoxUpdateTFOrder.Text == "")
            {
                // 
                {
                    // Stock
                    {
                        labelCancelOrderByStockNo2.Visible = false;
                        buttonCancelOrderBySeqNo.Visible = false;
                        buttonCancelOrderByBookNo.Visible = false;
                        buttonCancelOrderByStockNo.Visible = false;
                    }
                }
                // 
                {
                    // Stock
                    {
                        labelDecreaseOrderBySeqNo.Visible = false;
                        textBoxStockDecreaseQty.Visible = false;
                        buttonDecreaseOrderBySeqNo.Visible = false;
                    }
                }
                // 
                {
                    // Stock
                    {
                        labelCorrectPriceBySeqNo.Visible = true;
                        textBoxPrice.Visible = true;
                        labelTradeType.Visible = true;
                        comboBoxTradeType.Visible = true;
                        labelCorrectPriceByBookNoMarketSymbol.Visible = true;
                        comboBoxMarketSymbol.Visible = true;
                        buttonCorrectPriceBySeqNo.Visible = true;
                        buttonCorrectPriceByBookNo.Visible = true;
                    }
                }
            }
        }
        private void OrderControlForm_Load(object sender, EventArgs e)
        {
            //
            m_pSKOrder.OnAccount += new _ISKOrderLibEvents_OnAccountEventHandler(OnAccount);
            void OnAccount(string bstrLogInID, string bstrAccountData)
            {
                string[] values = bstrAccountData.Split(',');
                if (values[0] == "TS" || values[0] == "TF")
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
        }
    }
}
