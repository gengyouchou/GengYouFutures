namespace SKOrderTester
{
    partial class WithDrawInOutControl
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.boxCurrency = new System.Windows.Forms.ComboBox();
            this.boxTypeIn = new System.Windows.Forms.ComboBox();
            this.boxTypeOut = new System.Windows.Forms.ComboBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnSend = new System.Windows.Forms.Button();
            this.txtPWD = new System.Windows.Forms.TextBox();
            this.txtDollars = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.boxAccountIn = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.boxAccountOut = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // boxCurrency
            // 
            this.boxCurrency.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.boxCurrency.FormattingEnabled = true;
            this.boxCurrency.Items.AddRange(new object[] {
            "AUD",
            "EUR",
            "GBP",
            "HKD",
            "JPY",
            "NTD",
            "NZD",
            "RMB",
            "USD",
            "ZAR"});
            this.boxCurrency.Location = new System.Drawing.Point(398, 33);
            this.boxCurrency.Name = "boxCurrency";
            this.boxCurrency.Size = new System.Drawing.Size(62, 20);
            this.boxCurrency.TabIndex = 8;
            // 
            // boxTypeIn
            // 
            this.boxTypeIn.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.boxTypeIn.FormattingEnabled = true;
            this.boxTypeIn.Items.AddRange(new object[] {
            "0:國內",
            "1:國外"});
            this.boxTypeIn.Location = new System.Drawing.Point(206, 33);
            this.boxTypeIn.Name = "boxTypeIn";
            this.boxTypeIn.Size = new System.Drawing.Size(54, 20);
            this.boxTypeIn.TabIndex = 7;
            this.boxTypeIn.SelectedIndexChanged += new System.EventHandler(this.boxTypeIn_SelectedIndexChanged);
            // 
            // boxTypeOut
            // 
            this.boxTypeOut.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.boxTypeOut.FormattingEnabled = true;
            this.boxTypeOut.Items.AddRange(new object[] {
            "0:國內",
            "1:國外"});
            this.boxTypeOut.Location = new System.Drawing.Point(6, 33);
            this.boxTypeOut.Name = "boxTypeOut";
            this.boxTypeOut.Size = new System.Drawing.Size(54, 20);
            this.boxTypeOut.TabIndex = 6;
            this.boxTypeOut.SelectedIndexChanged += new System.EventHandler(this.boxTypeOut_SelectedIndexChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnSend);
            this.groupBox1.Controls.Add(this.txtPWD);
            this.groupBox1.Controls.Add(this.txtDollars);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.boxAccountIn);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.boxCurrency);
            this.groupBox1.Controls.Add(this.boxTypeIn);
            this.groupBox1.Controls.Add(this.boxTypeOut);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.boxAccountOut);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(14, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(805, 120);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "國內外保證金互轉申請";
            // 
            // btnSend
            // 
            this.btnSend.Location = new System.Drawing.Point(734, 30);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(65, 23);
            this.btnSend.TabIndex = 20;
            this.btnSend.Text = "執行互轉";
            this.btnSend.UseVisualStyleBackColor = true;
            this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
            // 
            // txtPWD
            // 
            this.txtPWD.Location = new System.Drawing.Point(603, 31);
            this.txtPWD.Name = "txtPWD";
            this.txtPWD.PasswordChar = '*';
            this.txtPWD.Size = new System.Drawing.Size(107, 22);
            this.txtPWD.TabIndex = 18;
            // 
            // txtDollars
            // 
            this.txtDollars.Location = new System.Drawing.Point(499, 33);
            this.txtDollars.Name = "txtDollars";
            this.txtDollars.Size = new System.Drawing.Size(84, 22);
            this.txtDollars.TabIndex = 17;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(607, 15);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(89, 12);
            this.label10.TabIndex = 15;
            this.label10.Text = "出金密碼檢核：";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(497, 15);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(53, 12);
            this.label9.TabIndex = 14;
            this.label9.Text = "轉帳金額";
            // 
            // boxAccountIn
            // 
            this.boxAccountIn.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.boxAccountIn.FormattingEnabled = true;
            this.boxAccountIn.Location = new System.Drawing.Point(266, 33);
            this.boxAccountIn.Name = "boxAccountIn";
            this.boxAccountIn.Size = new System.Drawing.Size(115, 20);
            this.boxAccountIn.TabIndex = 10;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(229, 15);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(65, 12);
            this.label6.TabIndex = 9;
            this.label6.Text = "轉入帳號：";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(405, 15);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 12);
            this.label3.TabIndex = 3;
            this.label3.Text = "幣別：";
            // 
            // boxAccountOut
            // 
            this.boxAccountOut.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.boxAccountOut.FormattingEnabled = true;
            this.boxAccountOut.Location = new System.Drawing.Point(66, 33);
            this.boxAccountOut.Name = "boxAccountOut";
            this.boxAccountOut.Size = new System.Drawing.Size(120, 20);
            this.boxAccountOut.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(32, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "轉出帳號：";
            // 
            // WithDrawInOutControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox1);
            this.Name = "WithDrawInOutControl";
            this.Size = new System.Drawing.Size(877, 547);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox boxCurrency;
        private System.Windows.Forms.ComboBox boxTypeIn;
        private System.Windows.Forms.ComboBox boxTypeOut;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnSend;
        private System.Windows.Forms.ComboBox boxAccountIn;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox boxAccountOut;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtPWD;
        private System.Windows.Forms.TextBox txtDollars;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
    }
}
