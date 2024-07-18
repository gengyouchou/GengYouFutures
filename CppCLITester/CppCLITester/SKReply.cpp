#include "SKReply.h"

//custom
DataTable^ CreateDataTable()
{
    DataTable^ mDataTable = gcnew DataTable();

    DataColumn^ mDataColumn;

    //索引
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.Int32");
    mDataColumn->ColumnName = "Index";
    mDataTable->Columns->Add(mDataColumn);

    //委託序號
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "KeyNo";
    mDataTable->Columns->Add(mDataColumn);

    //狀態
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Type";
    mDataTable->Columns->Add(mDataColumn);

    // 市場別
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "MarketType";
    mDataTable->Columns->Add(mDataColumn);

    //帳號
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "CustNo";
    mDataTable->Columns->Add(mDataColumn);

    //商品代碼
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "ComId";
    mDataTable->Columns->Add(mDataColumn);

    //名稱 
    //待補

    //委託總類
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "BuySell";
    mDataTable->Columns->Add(mDataColumn);

    //委託價
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Price";
    mDataTable->Columns->Add(mDataColumn);

    //委託量
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Qty";
    mDataTable->Columns->Add(mDataColumn);

    //成交量 
    //待補



    //委託書
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "OrederNo";
    mDataTable->Columns->Add(mDataColumn);

    //委託時間
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "TradeData";
    mDataTable->Columns->Add(mDataColumn);

    //委託有效日期
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "OrderEffective";
    mDataTable->Columns->Add(mDataColumn);

    //盤別
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Reserved";
    mDataTable->Columns->Add(mDataColumn);

    return mDataTable;

    return mDataTable;
}

Void CppCLITester::SKReply::OnConnect(System::String^ bstrUserID, int nErrorCode)
{
    if (bstrUserID == m_UserID)
    {
        lblSignalReplySolace->ForeColor = Color::Yellow;
    }
}

Void CppCLITester::SKReply::OnDisconnect(System::String^ bstrUserID, int nErrorCode)
{
    if (bstrUserID == m_UserID)
    {
        lblSignalReplySolace->ForeColor = Color::Red;
    }
}

Void CppCLITester::SKReply::OnComplete(System::String^ bstrUserID)
{
    // 資料都接收完畢了
    if (bstrUserID == m_UserID)
    {
        lblSignalReplySolace->ForeColor = Color::Green;

        //將資料放到DataView當中
        myDataTable = CreateDataTable();

        myDataTable->Clear();
        dataGridView1->DataSource = myDataTable;

        InitialDataGridView("");
    }
}

Void CppCLITester::SKReply::OnNewData(System::String^ bstrUserID,System::String^ bstrMessage)
{
   // WriteMessage(bstrMessage);
    if (bstrUserID == m_UserID)
    {
        Stringarray->Add(bstrMessage);
    }
}

Void CppCLITester::SKReply::ShowDetail(System::String^ type)
{
    int num = Stringarray->Count;

    for(int i=0 ; i<num ; i++)
    {
        array<String^>^ strValue;

        strValue = Stringarray[i]->Split(',');

        
        if (strValue[2] == type || type == "")
        {

            DataIndex += 1;
            DataRow^ mDataRow = myDataTable->NewRow();
            //索引
            mDataRow["Index"] = DataIndex;

            //委託序號
            mDataRow["KeyNo"] = strValue[0];

            //狀態
            if (strValue[2] == "N")
            {
                mDataRow["Type"] = "委託";
            }
            else if (strValue[2] == "C")
            {
                mDataRow["Type"] = "取消";
            }
            else if (strValue[2] == "U")
            {
                mDataRow["Type"] = "改量";
            }
            else if (strValue[2] == "P")
            {
                mDataRow["Type"] = "改價";
            }
            else if (strValue[2] == "D")
            {
                mDataRow["Type"] = "成交";
            }
            else if (strValue[2] == "B")
            {
                mDataRow["Type"] = "改價改量";
            }
            else if (strValue[2] == "S")
            {
                mDataRow["Type"] = "動態退單";
            }


            // 市場別
            if (strValue[1] == "TS")
            {
                mDataRow["MarketType"] = "證券";
            }
            else if (strValue[1] == "TA")
            {
                mDataRow["MarketType"] = "盤後";
            }
            else if (strValue[1] == "TL")
            {
                mDataRow["MarketType"] = "零股";
            }
            else if (strValue[1] == "TP")
            {
                mDataRow["MarketType"] = "興櫃";
            }
            else if (strValue[1] == "TF")
            {
                mDataRow["MarketType"] = "期貨";
            }
            else if (strValue[1] == "TO")
            {
                mDataRow["MarketType"] = "選擇權";
            }
            else if (strValue[1] == "OF")
            {
                mDataRow["MarketType"] = "海期";
            }
            else if (strValue[1] == "OO")
            {
                mDataRow["MarketType"] = "海選";
            }
            else if (strValue[1] == "OS")
            {
                mDataRow["MarketType"] = "複委託";
            }


            //帳號
            mDataRow["CustNo"] = strValue[5];

            //商品代碼
            mDataRow["ComId"] = strValue[8];

            //名稱 
            //待補

            //委託總類 
            System::String^  str = strValue[6];
            System::String^  strPrint = "";


                //期貨
            if (strValue[1] == "TF")
            {
                //[0] B/S 買/賣
                if (str[0] == 'B')
                {
                    strPrint += "買 ";
                }
                else if(str[0] == 'S')
                {
                    strPrint += "賣 ";
                }

                // [1] Y/當沖, N/新倉, O/平倉, 7/代沖銷
                if(str[1] ==  'Y')
                    strPrint += "當沖　";
                
                else if(str[1]== 'N')
                    strPrint += "新倉　";
                   
                else if(str[1] == 'O')
                    strPrint += "平倉　";
                   
                else if(str[1]== '7')
                    strPrint += "代沖銷　";

                // [2] I/R/F  IOC / ROD / FOK
                if (str[2] == 'I')
                {
                    strPrint += "IOC　";
                }
                else if (str[2] == 'R')
                {
                    strPrint += "ROD　";
                }
                else if (str[2] == 'F')
                {
                    strPrint += "FOK　";
                }

                // [3] 1/2/3/4/5　市價/限價/停損/停損限價/收市
                if (str[3] == '1')
                {
                    strPrint += "市價　";
                }
                else if (str[3] == '2')
                {
                    strPrint += "限價　";
                }
                else if (str[3] == '3')
                {
                    strPrint += "停損　";
                }
                else if (str[3] == '4')
                {
                    strPrint += "停損限價　";
                }
                else if (str[3] == '5')
                {
                    strPrint += "收市　";
                }
            }
            else if (strValue[1] == "OF") // 海期
            {
            }
            else if (strValue[1] == "OO") //海選
            {
                //[0] B/S 買/賣
                if (str[0] == 'B')
                {
                    strPrint += "買 ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "賣 ";
                }

                // [1] N/O 新倉 / 平倉
                if (str[1] == 'N')
                {
                    strPrint += "新倉　";
                }
                else if (str[1] == 'O')
                {
                    strPrint += "平倉　";
                }

                // [2] I/R/F  IOC / ROD / FOK
                if (str[2] == 'I')
                {
                    strPrint += "IOC　";
                }
                else if (str[2] == 'R')
                {
                    strPrint += "ROD　";
                }
                else if (str[2] == 'F')
                {
                    strPrint += "FOK　";
                }

                // [3] 1/2/3/4/5　市價/限價/停損/停損限價/收市
                if (str[3] == '1')
                {
                    strPrint += "市價　";
                }
                else if (str[3] == '2')
                {
                    strPrint += "限價　";
                }
                else if (str[3] == '3')
                {
                    strPrint += "停損　";
                }
                else if (str[3] == '4')
                {
                    strPrint += "停損限價　";
                }
                else if (str[3] == '5')
                {
                    strPrint += "收市　";
                }


            }
            else if (strValue[1] == "OS")
            {
                //[0] B/S 買/賣
                if (str[0] == 'B')
                {
                    strPrint += "買 ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "賣 ";
                }

                //
                if (str[1] == '1')
                {
                    strPrint += "市價　";
                }
                else if (str[1] == '2')
                {
                    strPrint += "限價　";
                }
                else if (str[1] == '3')
                {
                    strPrint += "停損　";
                }
                else if (str[1] == '4')
                {
                    strPrint += "停損限價　";
                }
                else if (str[1] == '5')
                {
                    strPrint += "收市　";
                }
            }
            else if (strValue[1] == "IO")//選擇權
            {
                //[0] B/S 買/賣
                if (str[0] == 'B')
                {
                    strPrint += "買 ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "賣 ";
                }

                // [1] Y/當沖, N/新倉, O/平倉, 7/代沖銷
                if (str[1] == 'Y')
                    strPrint += "當沖　";

                else if (str[1] == 'N')
                    strPrint += "新倉　";

                else if (str[1] == 'O')
                    strPrint += "平倉　";

                else if (str[1] == '7')
                    strPrint += "代沖銷　";

                // [2] I/R/F  IOC / ROD / FOK
                if (str[2] == 'I')
                {
                    strPrint += "IOC　";
                }
                else if (str[2] == 'R')
                {
                    strPrint += "ROD　";
                }
                else if (str[2] == 'F')
                {
                    strPrint += "FOK　";
                }

                // [3] 1/2/3/4/5　市價/限價/停損/停損限價/收市
                if (str[3] == '1')
                {
                    strPrint += "市價　";
                }
                else if (str[3] == '2')
                {
                    strPrint += "限價　";
                }
                else if (str[3] == '3')
                {
                    strPrint += "停損　";
                }
                else if (str[3] == '4')
                {
                    strPrint += "停損限價　";
                }
                else if (str[3] == '5')
                {
                    strPrint += "收市　";
                }
            }
            else
            {
                //[0] B/S 買/賣
                if (str[0] == 'B')
                {
                    strPrint += "買 ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "賣 ";
                }

                // [1,2] 00 現股，01 代資，02 代券，03 融資，04 融券，08 無券，20 零股，40 拍賣現股 
                System::String^ tempstring = str->Substring(1, 2);
                if (tempstring == "00")
                {
                    strPrint += "現股 ";
                }
                else if (tempstring == "01")
                {
                    strPrint += "代資 ";
                }
                else if (tempstring == "02")
                {
                    strPrint += "代券 ";
                }
                else if (tempstring == "03")
                {
                    strPrint += "融資 ";
                }
                else if (tempstring == "04")
                {
                    strPrint += "融券 ";
                }
                else if (tempstring == "08")
                {
                    strPrint += "無券 ";
                }
                else if (tempstring == "20")
                {
                    strPrint += "零股 ";
                }
                else if (tempstring == "40")
                {
                    strPrint += "拍賣現股 ";
                }

                // [3] I/R/F  IOC / ROD / FOK 
                if (str[3] == 'I')
                {
                    strPrint += "IOC　";
                }
                else if (str[3] == 'R')
                {
                    strPrint += "ROD　";
                }
                else if (str[3] == 'F')
                {
                    strPrint += "FOK　";
                }
                // [4] 1/2　市價/限價
                if (str[4] == '1')
                {
                    strPrint += "市價　";
                }
                else if (str[4] == '2')
                {
                    strPrint += "限價　";
                }
            }
            mDataRow["BuySell"] = strPrint;

            //委託價
            mDataRow["Price"] = strValue[11];

            //委託量
            mDataRow["Qty"] = strValue[20];

            //成交量 
            //待補



            //委託書
            mDataRow["OrederNo"] = strValue[10];

            //委託時間
            mDataRow["TradeData"] = strValue[23]->Substring(0, 4)+"/"+ strValue[23]->Substring(4, 2)+"/" + strValue[23]->Substring(6, 2)+ strValue[24];



            //委託有效日期
            mDataRow["OrderEffective"] = strValue[29]->Substring(0, 4)+"/"+strValue[29]->Substring(4, 2)+"/"+strValue[29]->Substring(6, 2);

            //盤別
            mDataRow["Reserved"] = strValue[31];

            if (strValue[31] == "A")
            {
                mDataRow["Reserved"] = "T盤";
            }
            else if (strValue[31] == "B")
            {
                mDataRow["Reserved"] = "T+1盤";
            }


            myDataTable->Rows->Add(mDataRow);
        }
    }

    DataIndex = 0;
}

Void CppCLITester::SKReply::InitialDataGridView(System::String^ type)
{
    myDataTable->Clear();
    dataGridView1->ClearSelection();
    dataGridView1->DataSource = myDataTable;
   //索引
    dataGridView1->Columns["Index"]->HeaderText = " ";

    //委託序號
    dataGridView1->Columns["KeyNo"]->HeaderText = "委託序號";

    //狀態
    dataGridView1->Columns["Type"]->HeaderText = "狀態";

    // 市場別
    dataGridView1->Columns["MarketType"]->HeaderText = "市場別";

    //帳號
    dataGridView1->Columns["CustNo"]->HeaderText = "帳號";

    //商品代碼
    dataGridView1->Columns["ComId"]->HeaderText = "商品代碼";

    //名稱 
    //待補

    //委託總類
    dataGridView1->Columns["BuySell"]->HeaderText = "委託總類";

    //委託價
    dataGridView1->Columns["Price"]->HeaderText = "委託價";

    //委託量
    dataGridView1->Columns["Qty"]->HeaderText = "委託量";

    //成交量 
    //待補



    //委託書
    dataGridView1->Columns["OrederNo"]->HeaderText = "委託書";

    //委託時間
    dataGridView1->Columns["TradeData"]->HeaderText = "委託時間";

    //委託有效日期
    dataGridView1->Columns["OrderEffective"]->HeaderText = "委託有效日期";

    //盤別
    dataGridView1->Columns["Reserved"]->HeaderText = "盤別";

    // 將資料放入資料表裡
    ShowDetail(type);
}

System::Void CppCLITester::SKReply::btnConnect_Click(System::Object^ sender, System::EventArgs^ e)
{
    if (m_first == true)
    {
        m_pSKReply->OnConnect += gcnew SKCOMLib::_ISKReplyLibEvents_OnConnectEventHandler(this, &CppCLITester::SKReply::OnConnect);
        m_pSKReply->OnDisconnect += gcnew SKCOMLib::_ISKReplyLibEvents_OnDisconnectEventHandler(this, &CppCLITester::SKReply::OnDisconnect);
        m_pSKReply->OnComplete += gcnew SKCOMLib::_ISKReplyLibEvents_OnCompleteEventHandler(this, &CppCLITester::SKReply::OnComplete);
        m_pSKReply->OnNewData += gcnew SKCOMLib::_ISKReplyLibEvents_OnNewDataEventHandler(this, &CppCLITester::SKReply::OnNewData);
       
    }
    int n_mCode = m_pSKReply->SKReplyLib_ConnectByID(m_UserID);

    GetMessage("SKReply", n_mCode, "SKReplyLib_ConnectByID");
}

System::Void CppCLITester::SKReply::btnDisconnect_Click(System::Object^ sender, System::EventArgs^ e)
{
    int n_mCode = m_pSKReply->SKReplyLib_CloseByID(m_UserID);

    GetMessage("SKReply", n_mCode, "SKReplyLib_CloseByID");
}

System::Void CppCLITester::SKReply::btnIsConnected_Click(System::Object^ sender, System::EventArgs^ e)
{
    int n_mCode = m_pSKReply->SKReplyLib_IsConnectedByID(m_UserID);

    if (n_mCode == 0) //斷線
    {
        ConnectedLabel->Text = "False";
        ConnectedLabel->BackColor = Color::Red;
    }
    else if (n_mCode == 1) //連線中
    {
        ConnectedLabel->Text = "True";
        ConnectedLabel->BackColor = Color::Green;
    }
    else if (n_mCode == 2) //下載中
    {
        ConnectedLabel->Text = "False";
        ConnectedLabel->BackColor = Color::Yellow;
    }
    else
    {
        ConnectedLabel->Text = "False";
        ConnectedLabel->BackColor = Color::DarkRed;
    }
    GetMessage("SKReply", n_mCode, "SKReplyLib_IsConnectedByID");
}

System::Void CppCLITester::SKReply::comboBox1_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
{
    if (comboBox1->SelectedIndex == 0)
    {
        InitialDataGridView("");
    }
    else if (comboBox1->SelectedIndex == 1)
    {
        InitialDataGridView("D");
    }
    else if (comboBox1->SelectedIndex == 2)
    {
        InitialDataGridView("N");
    }
    else if (comboBox1->SelectedIndex == 3)
    {
        InitialDataGridView("C");
    }
    else if (comboBox1->SelectedIndex == 4)
    {
        InitialDataGridView("U");
    }
    else if (comboBox1->SelectedIndex == 5)
    {
        InitialDataGridView("P");
    }
    else if (comboBox1->SelectedIndex == 6)
    {
        InitialDataGridView("B");
    }
    else if (comboBox1->SelectedIndex == 7)
    {
        InitialDataGridView("S");
    }
}

