using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SKCOMLib;

namespace SKOrderTester
{
    public partial class WithDrawInOutControl : UserControl
    {

        #region Define Variable
        //----------------------------------------------------------------------
        // Define Variable
        //----------------------------------------------------------------------
        public delegate void WithDrawSignalHandler(string strLogInID, string strAccountOut, int nInType,string strAccountIn,int nOutType, int nCurrency,string strDollars, string strPWD);
        public event WithDrawSignalHandler OnWithDrawSignal;


        private int m_nCode;
        public string m_strMessage;
        
        private string m_UserID = "";
        public string UserID
        {
            get { return m_UserID; }
            set { m_UserID = value; }
        }
        
        //string strInfo = boxOSFutureAccount.Text;
        private string m_UserAccountTF = "";
        public string UserAccountTF
        {
            get { return m_UserAccountTF; }
            set { m_UserAccountTF = value; }
        }

        private string m_UserAccountOF = "";
        public string UserAccountOF
        {
            get { return m_UserAccountOF; }
            set { m_UserAccountOF = value; }
        }

        #endregion

        #region Initialize
        //----------------------------------------------------------------------
        // Initialize
        //----------------------------------------------------------------------
        
        
        public WithDrawInOutControl()
        {
            InitializeComponent();
        }

        private void boxTypeOut_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (boxTypeOut.SelectedIndex == 0)
            {
                if (!boxAccountOut.Items.Contains(m_UserAccountTF))
                    boxAccountOut.Items.Add(m_UserAccountTF);
            }
            else if (boxTypeOut.SelectedIndex == 1)
            {
                if (!boxAccountOut.Items.Contains(m_UserAccountOF))
                    boxAccountOut.Items.Add(m_UserAccountOF);
            }
        }
        #endregion

        private void boxTypeIn_SelectedIndexChanged(object sender, EventArgs e)
        {

            if (boxTypeIn.SelectedIndex == 0)
            {
                if (!boxAccountIn.Items.Contains(m_UserAccountTF))
                { 
                    boxAccountIn.Items.Add(m_UserAccountTF);
                }

            }
            else if (boxTypeIn.SelectedIndex == 1)
            {
                if (!boxAccountIn.Items.Contains(m_UserAccountOF))
                {
                    boxAccountIn.Items.Add(m_UserAccountOF);
                }
            }

        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            string strAccountOut;
            string strAccountIn;
            int nTypeOut;
            int nTypeIn;
            int nCurrency;
            string strDollars;
            string strPWD;
            
            if (boxTypeOut.SelectedIndex <0)
            {
                MessageBox.Show("請選擇轉出類別");
                return;
            }
            nTypeOut = boxTypeOut.SelectedIndex;

            if (boxAccountOut.SelectedIndex < 0)
            {
                MessageBox.Show("請選擇轉出帳號");
                return;
            }
            strAccountOut = boxAccountOut.Text;


            if (boxTypeIn.SelectedIndex < 0)
            {
                MessageBox.Show("請選擇轉入類別");
                return;
            }
            nTypeIn = boxTypeIn.SelectedIndex;

            if (boxAccountIn.SelectedIndex < 0)
            {
                MessageBox.Show("請選擇轉入帳號");
                return;
            }
            strAccountIn = boxAccountIn.Text;
            
            if (boxCurrency.SelectedIndex < 0)
            {
                MessageBox.Show("請選擇幣別");
                return;
            }
            nCurrency = boxCurrency.SelectedIndex;

            if (txtDollars.Text == "")
            {
                MessageBox.Show("請輸入互轉金額");
                return;
            }
            strDollars = txtDollars.Text;

            if (txtPWD.Text == "")
            {
                MessageBox.Show("請輸入出金密碼");
                return;
            }
            strPWD = txtPWD.Text;

            if (OnWithDrawSignal != null)
            {
                OnWithDrawSignal(m_UserID, strAccountOut, nTypeOut, strAccountIn, nTypeIn, nCurrency, strDollars, strPWD);
            }

            
        }
    }
}
