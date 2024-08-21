#include "SKReply.h"

// custom
DataTable ^ CreateDataTable() {
    DataTable ^ mDataTable = gcnew DataTable();

    DataColumn ^ mDataColumn;

    // ����
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.Int32");
    mDataColumn->ColumnName = "Index";
    mDataTable->Columns->Add(mDataColumn);

    // �e��Ǹ�
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "KeyNo";
    mDataTable->Columns->Add(mDataColumn);

    // ���A
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Type";
    mDataTable->Columns->Add(mDataColumn);

    // �����O
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "MarketType";
    mDataTable->Columns->Add(mDataColumn);

    // �b��
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "CustNo";
    mDataTable->Columns->Add(mDataColumn);

    // �~�N�X
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "ComId";
    mDataTable->Columns->Add(mDataColumn);

    // �W��
    // ��

    // �e�U�`��
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "BuySell";
    mDataTable->Columns->Add(mDataColumn);

    // �e�U��
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Price";
    mDataTable->Columns->Add(mDataColumn);

    // �e�U�q
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Qty";
    mDataTable->Columns->Add(mDataColumn);

    // ����q
    // ��

    // �e�U��
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "OrederNo";
    mDataTable->Columns->Add(mDataColumn);

    // �e��ɶ�
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "TradeData";
    mDataTable->Columns->Add(mDataColumn);

    // �e����Ĥ��
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "OrderEffective";
    mDataTable->Columns->Add(mDataColumn);

    // �L�O
    mDataColumn = gcnew DataColumn();
    mDataColumn->DataType = Type::GetType("System.String");
    mDataColumn->ColumnName = "Reserved";
    mDataTable->Columns->Add(mDataColumn);

    return mDataTable;

    return mDataTable;
}

    Void CppCLITester::SKReply::OnConnect(System::String ^ bstrUserID, int nErrorCode)
{
    if (bstrUserID == m_UserID)
    {
        lblSignalReplySolace->ForeColor = Color::Yellow;
    }
}

Void CppCLITester::SKReply::OnDisconnect(System::String ^ bstrUserID, int nErrorCode)
{
    if (bstrUserID == m_UserID)
    {
        lblSignalReplySolace->ForeColor = Color::Red;
    }
}

Void CppCLITester::SKReply::OnComplete(System::String ^ bstrUserID)
{
    // �Ƴ����������F
    if (bstrUserID == m_UserID)
    {
        lblSignalReplySolace->ForeColor = Color::Green;

        // �N�Ʃ��DataView����
        myDataTable = CreateDataTable();

        myDataTable->Clear();
        dataGridView1->DataSource = myDataTable;

        InitialDataGridView("");
    }
}

Void CppCLITester::SKReply::OnNewData(System::String ^ bstrUserID, System::String ^ bstrMessage)
{
    // WriteMessage(bstrMessage);
    if (bstrUserID == m_UserID)
    {
        Stringarray->Add(bstrMessage);
    }
}

Void CppCLITester::SKReply::ShowDetail(System::String ^ type)
{
    int num = Stringarray->Count;

    for (int i = 0; i < num; i++)
    {
        array<String ^> ^ strValue;

        strValue = Stringarray[i]->Split(',');

        if (strValue[2] == type || type == "")
        {

            DataIndex += 1;
            DataRow ^ mDataRow = myDataTable->NewRow();
            // ����
            mDataRow["Index"] = DataIndex;

            // �e��Ǹ�
            mDataRow["KeyNo"] = strValue[0];

            // ���A
            if (strValue[2] == "N")
            {
                mDataRow["Type"] = "�e�U";
            }
            else if (strValue[2] == "C")
            {
                mDataRow["Type"] = "����";
            }
            else if (strValue[2] == "U")
            {
                mDataRow["Type"] = "��q";
            }
            else if (strValue[2] == "P")
            {
                mDataRow["Type"] = "���";
            }
            else if (strValue[2] == "D")
            {
                mDataRow["Type"] = "����";
            }
            else if (strValue[2] == "B")
            {
                mDataRow["Type"] = "�����q";
            }
            else if (strValue[2] == "S")
            {
                mDataRow["Type"] = "�A�h��";
            }

            // �����O
            if (strValue[1] == "TS")
            {
                mDataRow["MarketType"] = "��";
            }
            else if (strValue[1] == "TA")
            {
                mDataRow["MarketType"] = "�L��";
            }
            else if (strValue[1] == "TL")
            {
                mDataRow["MarketType"] = "�s��";
            }
            else if (strValue[1] == "TP")
            {
                mDataRow["MarketType"] = "���d";
            }
            else if (strValue[1] == "TF")
            {
                mDataRow["MarketType"] = "���f";
            }
            else if (strValue[1] == "TO")
            {
                mDataRow["MarketType"] = "����v";
            }
            else if (strValue[1] == "OF")
            {
                mDataRow["MarketType"] = "����";
            }
            else if (strValue[1] == "OO")
            {
                mDataRow["MarketType"] = "����";
            }
            else if (strValue[1] == "OS")
            {
                mDataRow["MarketType"] = "�e�U";
            }

            // �b��
            mDataRow["CustNo"] = strValue[5];

            // �~�N�X
            mDataRow["ComId"] = strValue[8];

            // �W��
            // ��

            // �e�U�`��
            System::String ^ str = strValue[6];
            System::String ^ strPrint = "";

            // ���f
            if (strValue[1] == "TF")
            {
                //[0] B/S �R/��
                if (str[0] == 'B')
                {
                    strPrint += "�R ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "�� ";
                }

                // [1] Y/���R, N/�s��, O/����, 7/�N�R�P
                if (str[1] == 'Y')
                    strPrint += "���R�@";

                else if (str[1] == 'N')
                    strPrint += "��ܡ@";

                else if (str[1] == 'O')
                    strPrint += "��ܡ@";

                else if (str[1] == '7')
                    strPrint += "�N�R�P�@";

                // [2] I/R/F  IOC / ROD / FOK
                if (str[2] == 'I')
                {
                    strPrint += "IOC�@";
                }
                else if (str[2] == 'R')
                {
                    strPrint += "ROD�@";
                }
                else if (str[2] == 'F')
                {
                    strPrint += "FOK�@";
                }

                // [3] 1/2/3/4/5�@����/����/���l/���l����/����
                if (str[3] == '1')
                {
                    strPrint += "�����@";
                }
                else if (str[3] == '2')
                {
                    strPrint += "�����@";
                }
                else if (str[3] == '3')
                {
                    strPrint += "���l�@";
                }
                else if (str[3] == '4')
                {
                    strPrint += "���l�����@";
                }
                else if (str[3] == '5')
                {
                    strPrint += "�����@";
                }
            }
            else if (strValue[1] == "OF") // ����
            {
            }
            else if (strValue[1] == "OO") // ����
            {
                //[0] B/S �R/��
                if (str[0] == 'B')
                {
                    strPrint += "�R ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "�� ";
                }

                // [1] N/O �s�� / ����
                if (str[1] == 'N')
                {
                    strPrint += "��ܡ@";
                }
                else if (str[1] == 'O')
                {
                    strPrint += "��ܡ@";
                }

                // [2] I/R/F  IOC / ROD / FOK
                if (str[2] == 'I')
                {
                    strPrint += "IOC�@";
                }
                else if (str[2] == 'R')
                {
                    strPrint += "ROD�@";
                }
                else if (str[2] == 'F')
                {
                    strPrint += "FOK�@";
                }

                // [3] 1/2/3/4/5�@����/����/���l/���l����/����
                if (str[3] == '1')
                {
                    strPrint += "�����@";
                }
                else if (str[3] == '2')
                {
                    strPrint += "�����@";
                }
                else if (str[3] == '3')
                {
                    strPrint += "���l�@";
                }
                else if (str[3] == '4')
                {
                    strPrint += "���l�����@";
                }
                else if (str[3] == '5')
                {
                    strPrint += "�����@";
                }
            }
            else if (strValue[1] == "OS")
            {
                //[0] B/S �R/��
                if (str[0] == 'B')
                {
                    strPrint += "�R ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "�� ";
                }

                //
                if (str[1] == '1')
                {
                    strPrint += "�����@";
                }
                else if (str[1] == '2')
                {
                    strPrint += "�����@";
                }
                else if (str[1] == '3')
                {
                    strPrint += "���l�@";
                }
                else if (str[1] == '4')
                {
                    strPrint += "���l�����@";
                }
                else if (str[1] == '5')
                {
                    strPrint += "�����@";
                }
            }
            else if (strValue[1] == "IO") // ����v
            {
                //[0] B/S �R/��
                if (str[0] == 'B')
                {
                    strPrint += "�R ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "�� ";
                }

                // [1] Y/���R, N/�s��, O/����, 7/�N�R�P
                if (str[1] == 'Y')
                    strPrint += "���R�@";

                else if (str[1] == 'N')
                    strPrint += "��ܡ@";

                else if (str[1] == 'O')
                    strPrint += "��ܡ@";

                else if (str[1] == '7')
                    strPrint += "�N�R�P�@";

                // [2] I/R/F  IOC / ROD / FOK
                if (str[2] == 'I')
                {
                    strPrint += "IOC�@";
                }
                else if (str[2] == 'R')
                {
                    strPrint += "ROD�@";
                }
                else if (str[2] == 'F')
                {
                    strPrint += "FOK�@";
                }

                // [3] 1/2/3/4/5�@����/����/���l/���l����/����
                if (str[3] == '1')
                {
                    strPrint += "�����@";
                }
                else if (str[3] == '2')
                {
                    strPrint += "�����@";
                }
                else if (str[3] == '3')
                {
                    strPrint += "���l�@";
                }
                else if (str[3] == '4')
                {
                    strPrint += "���l�����@";
                }
                else if (str[3] == '5')
                {
                    strPrint += "�����@";
                }
            }
            else
            {
                //[0] B/S �R/��
                if (str[0] == 'B')
                {
                    strPrint += "�R ";
                }
                else if (str[0] == 'S')
                {
                    strPrint += "�� ";
                }

                // [1,2] 00 ��ѡA01 �N��A02 �N�A0 �ĸ�A04 �Ĩ�08 �L��A20 �s�ѡA40 ���{��
                System::String ^ tempstring = str->Substring(1, 2);
                if (tempstring == "00")
                {
                    strPrint += "�{�� ";
                }
                else if (tempstring == "01")
                {
                    strPrint += "�N�� ";
                }
                else if (tempstring == "02")
                {
                    strPrint += "�N�� ";
                }
                else if (tempstring == "03")
                {
                    strPrint += "�� ";
                }
                else if (tempstring == "04")
                {
                    strPrint += "�� ";
                }
                else if (tempstring == "08")
                {
                    strPrint += "�L�� ";
                }
                else if (tempstring == "20")
                {
                    strPrint += "�s�� ";
                }
                else if (tempstring == "40")
                {
                    strPrint += "���{�� ";
                }

                // [3] I/R/F  IOC / ROD / FOK
                if (str[3] == 'I')
                {
                    strPrint += "IOC�@";
                }
                else if (str[3] == 'R')
                {
                    strPrint += "ROD�@";
                }
                else if (str[3] == 'F')
                {
                    strPrint += "FOK�@";
                }
                // [4] 1/2�@����/����
                if (str[4] == '1')
                {
                    strPrint += "�����@";
                }
                else if (str[4] == '2')
                {
                    strPrint += "�����@";
                }
            }
            mDataRow["BuySell"] = strPrint;

            // �e�U��
            mDataRow["Price"] = strValue[11];

            // �e�U�q
            mDataRow["Qty"] = strValue[20];

            // ����q
            // ��

            // �e�U��
            mDataRow["OrederNo"] = strValue[10];

            // �e��ɶ�
            mDataRow["TradeData"] = strValue[23]->Substring(0, 4) + "/" + strValue[23]->Substring(4, 2) + "/" + strValue[23]->Substring(6, 2) + strValue[24];

            // �e����Ĥ��
            mDataRow["OrderEffective"] = strValue[29]->Substring(0, 4) + "/" + strValue[29]->Substring(4, 2) + "/" + strValue[29]->Substring(6, 2);

            // �L�O
            mDataRow["Reserved"] = strValue[31];

            if (strValue[31] == "A")
            {
                mDataRow["Reserved"] = "T�L";
            }
            else if (strValue[31] == "B")
            {
                mDataRow["Reserved"] = "T+1�L";
            }

            myDataTable->Rows->Add(mDataRow);
        }
    }

    DataIndex = 0;
}

Void CppCLITester::SKReply::InitialDataGridView(System::String ^ type)
{
    myDataTable->Clear();
    dataGridView1->ClearSelection();
    dataGridView1->DataSource = myDataTable;
    // ����
    dataGridView1->Columns["Index"]->HeaderText = " ";

    // �e��Ǹ�
    dataGridView1->Columns["KeyNo"]->HeaderText = "�e��Ǹ�";

    // ���A
    dataGridView1->Columns["Type"]->HeaderText = "���A";

    // �����O
    dataGridView1->Columns["MarketType"]->HeaderText = "�����O";

    // �b��
    dataGridView1->Columns["CustNo"]->HeaderText = "�b��";

    // �~�N�X
    dataGridView1->Columns["ComId"]->HeaderText = "�~�N�X";

    // �W��
    // ��

    // �e�U�`��
    dataGridView1->Columns["BuySell"]->HeaderText = "�e�U�`��";

    // �e�U��
    dataGridView1->Columns["Price"]->HeaderText = "�e�U��";

    // �e�U�q
    dataGridView1->Columns["Qty"]->HeaderText = "�e�U�q";

    // ����q
    // ��

    // �e�U��
    dataGridView1->Columns["OrederNo"]->HeaderText = "�e�U��";

    // �e��ɶ�
    dataGridView1->Columns["TradeData"]->HeaderText = "�e��ɶ�";

    // �e����Ĥ��
    dataGridView1->Columns["OrderEffective"]->HeaderText = "�e����Ĥ��";

    // �L�O
    dataGridView1->Columns["Reserved"]->HeaderText = "�L�O";

    // �N�Ʃ�J�ƪ���
    ShowDetail(type);
}

System::Void CppCLITester::SKReply::btnConnect_Click(System::Object ^ sender, System::EventArgs ^ e)
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

System::Void CppCLITester::SKReply::btnDisconnect_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    int n_mCode = m_pSKReply->SKReplyLib_CloseByID(m_UserID);

    GetMessage("SKReply", n_mCode, "SKReplyLib_CloseByID");
}

System::Void CppCLITester::SKReply::btnIsConnected_Click(System::Object ^ sender, System::EventArgs ^ e)
{
    int n_mCode = m_pSKReply->SKReplyLib_IsConnectedByID(m_UserID);

    if (n_mCode == 0) // �_�u
    {
        ConnectedLabel->Text = "False";
        ConnectedLabel->BackColor = Color::Red;
    }
    else if (n_mCode == 1) // �s�u��
    {
        ConnectedLabel->Text = "True";
        ConnectedLabel->BackColor = Color::Green;
    }
    else if (n_mCode == 2) // �U����
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

System::Void CppCLITester::SKReply::comboBox1_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
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
