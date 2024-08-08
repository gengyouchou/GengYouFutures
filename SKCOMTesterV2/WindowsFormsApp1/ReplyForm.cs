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
    public partial class ReplyForm : Form
    {
        // ，From，
        bool isClosing = false;
        // 
        SKCenterLib m_pSKCenter = new SKCenterLib(); // &
        SKReplyLib m_pSKReply = new SKReplyLib(); // 
        SKOrderLib m_pSKOrder = new SKOrderLib(); //
        // [UserID] 
        Dictionary<string, List<string>> m_dictUserID = new Dictionary<string, List<string>>();
        List<string> allkeys; // UserID
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
        public ReplyForm()
        {
            //Init
            {
                InitializeComponent();
                //dataGridView
                {
                    // 
                    {
                        //dataGridViewTS
                        {
                            dataGridViewTS.Columns.Add("Column1", "13");
                            dataGridViewTS.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewTS.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewTS.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewTS.Columns.Add("Column5", "");
                            dataGridViewTS.Columns.Add("Column6", "");
                            dataGridViewTS.Columns.Add("Column7", "");
                            dataGridViewTS.Columns.Add("Column8", "");
                            dataGridViewTS.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewTS.Columns.Add("Column10", "/");

                            dataGridViewTS.Columns.Add("Column11", "");
                            dataGridViewTS.Columns.Add("Column12", "");
                            dataGridViewTS.Columns.Add("Column13", "");
                            dataGridViewTS.Columns.Add("Column14", "");
                            dataGridViewTS.Columns.Add("Column15", "");
                            dataGridViewTS.Columns.Add("Column16", "");
                            dataGridViewTS.Columns.Add("Column17", "");
                            dataGridViewTS.Columns.Add("Column18", "");
                            dataGridViewTS.Columns.Add("Column19", "");
                            dataGridViewTS.Columns.Add("Column20", "");

                            dataGridViewTS.Columns.Add("Column21", "");
                            dataGridViewTS.Columns.Add("Column22", "(E:)");
                            dataGridViewTS.Columns.Add("Column23", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewTS.Columns.Add("Column24", "13(IOC/FOK)");
                        }
                        //dataGridViewTA
                        {
                            dataGridViewTA.Columns.Add("Column1", "13");
                            dataGridViewTA.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewTA.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewTA.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewTA.Columns.Add("Column5", "");
                            dataGridViewTA.Columns.Add("Column6", "");
                            dataGridViewTA.Columns.Add("Column7", "");
                            dataGridViewTA.Columns.Add("Column8", "");
                            dataGridViewTA.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewTA.Columns.Add("Column10", "/");

                            dataGridViewTA.Columns.Add("Column11", "");
                            dataGridViewTA.Columns.Add("Column12", "");
                            dataGridViewTA.Columns.Add("Column13", "");
                            dataGridViewTA.Columns.Add("Column14", "");
                            dataGridViewTA.Columns.Add("Column15", "");
                            dataGridViewTA.Columns.Add("Column16", "");
                            dataGridViewTA.Columns.Add("Column17", "");
                            dataGridViewTA.Columns.Add("Column18", "");
                            dataGridViewTA.Columns.Add("Column19", "");
                            dataGridViewTA.Columns.Add("Column20", "(E:)");

                            dataGridViewTA.Columns.Add("Column21", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewTA.Columns.Add("Column22", "13(IOC/FOK)");
                        }
                        //dataGridViewTL
                        {
                            dataGridViewTL.Columns.Add("Column1", "13");
                            dataGridViewTL.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewTL.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewTL.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewTL.Columns.Add("Column5", "");
                            dataGridViewTL.Columns.Add("Column6", "");
                            dataGridViewTL.Columns.Add("Column7", "");
                            dataGridViewTL.Columns.Add("Column8", "");
                            dataGridViewTL.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewTL.Columns.Add("Column10", "/");

                            dataGridViewTL.Columns.Add("Column11", "");
                            dataGridViewTL.Columns.Add("Column12", "");
                            dataGridViewTL.Columns.Add("Column13", "");
                            dataGridViewTL.Columns.Add("Column14", "");
                            dataGridViewTL.Columns.Add("Column15", "");
                            dataGridViewTL.Columns.Add("Column16", "");
                            dataGridViewTL.Columns.Add("Column17", "");
                            dataGridViewTL.Columns.Add("Column18", "");
                            dataGridViewTL.Columns.Add("Column19", "");
                            dataGridViewTL.Columns.Add("Column20", "(E:)");

                            dataGridViewTL.Columns.Add("Column21", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewTL.Columns.Add("Column22", "13(IOC/FOK)");
                        }
                        //dataGridViewTP
                        {
                            dataGridViewTP.Columns.Add("Column1", "13");
                            dataGridViewTP.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewTP.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewTP.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewTP.Columns.Add("Column5", "");
                            dataGridViewTP.Columns.Add("Column6", "");
                            dataGridViewTP.Columns.Add("Column7", "");
                            dataGridViewTP.Columns.Add("Column8", "");
                            dataGridViewTP.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewTP.Columns.Add("Column10", "/");

                            dataGridViewTP.Columns.Add("Column11", "");
                            dataGridViewTP.Columns.Add("Column12", "");
                            dataGridViewTP.Columns.Add("Column13", "");
                            dataGridViewTP.Columns.Add("Column14", "");
                            dataGridViewTP.Columns.Add("Column15", "");
                            dataGridViewTP.Columns.Add("Column16", "");
                            dataGridViewTP.Columns.Add("Column17", "");
                            dataGridViewTP.Columns.Add("Column18", "");
                            dataGridViewTP.Columns.Add("Column19", "");
                            dataGridViewTP.Columns.Add("Column20", "(E:)");

                            dataGridViewTP.Columns.Add("Column21", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewTP.Columns.Add("Column22", "13(IOC/FOK)");
                        }
                        //dataGridViewTC
                        {
                            dataGridViewTC.Columns.Add("Column1", "13");
                            dataGridViewTC.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewTC.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewTC.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewTC.Columns.Add("Column5", "");
                            dataGridViewTC.Columns.Add("Column6", "");
                            dataGridViewTC.Columns.Add("Column7", "");
                            dataGridViewTC.Columns.Add("Column8", "");
                            dataGridViewTC.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewTC.Columns.Add("Column10", "/");

                            dataGridViewTC.Columns.Add("Column11", "");
                            dataGridViewTC.Columns.Add("Column12", "");
                            dataGridViewTC.Columns.Add("Column13", "");
                            dataGridViewTC.Columns.Add("Column14", "");
                            dataGridViewTC.Columns.Add("Column15", "");
                            dataGridViewTC.Columns.Add("Column16", "");
                            dataGridViewTC.Columns.Add("Column17", "");
                            dataGridViewTC.Columns.Add("Column18", "");
                            dataGridViewTC.Columns.Add("Column19", "");
                            dataGridViewTC.Columns.Add("Column20", "(E:)");

                            dataGridViewTC.Columns.Add("Column21", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewTC.Columns.Add("Column22", "13(IOC/FOK)");
                        }
                        //dataGridViewTF
                        {
                            dataGridViewTF.Columns.Add("Column1", "13");
                            dataGridViewTF.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewTF.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewTF.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewTF.Columns.Add("Column5", "");
                            dataGridViewTF.Columns.Add("Column6", "");
                            dataGridViewTF.Columns.Add("Column7", "");
                            dataGridViewTF.Columns.Add("Column8", "");
                            dataGridViewTF.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewTF.Columns.Add("Column10", "");

                            dataGridViewTF.Columns.Add("Column11", "");
                            dataGridViewTF.Columns.Add("Column12", "");
                            dataGridViewTF.Columns.Add("Column13", "");
                            dataGridViewTF.Columns.Add("Column14", "/");
                            dataGridViewTF.Columns.Add("Column15", "");
                            dataGridViewTF.Columns.Add("Column16", "");
                            dataGridViewTF.Columns.Add("Column17", "");
                            dataGridViewTF.Columns.Add("Column18", "");
                            dataGridViewTF.Columns.Add("Column19", "");
                            dataGridViewTF.Columns.Add("Column20", "");

                            dataGridViewTF.Columns.Add("Column21", "A: B:");
                            dataGridViewTF.Columns.Add("Column22", "");
                            dataGridViewTF.Columns.Add("Column23", "");
                            dataGridViewTF.Columns.Add("Column24", "");
                            dataGridViewTF.Columns.Add("Column25", "");
                            dataGridViewTF.Columns.Add("Column26", "");
                            dataGridViewTF.Columns.Add("Column27", "");
                            dataGridViewTF.Columns.Add("Column28", "A：T  B：T+1");
                            dataGridViewTF.Columns.Add("Column29", "");
                            dataGridViewTF.Columns.Add("Column30", "");

                            dataGridViewTF.Columns.Add("Column31", "(E:)");
                            dataGridViewTF.Columns.Add("Column32", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewTF.Columns.Add("Column33", "13(IOC/FOK)");
                        }
                        //dataGridViewTO
                        {
                            dataGridViewTO.Columns.Add("Column1", "13");
                            dataGridViewTO.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewTO.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewTO.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewTO.Columns.Add("Column5", "");
                            dataGridViewTO.Columns.Add("Column6", "");
                            dataGridViewTO.Columns.Add("Column7", "");
                            dataGridViewTO.Columns.Add("Column8", "");
                            dataGridViewTO.Columns.Add("Column9", "");
                            dataGridViewTO.Columns.Add("Column10", "(N:「」；D:「」)");

                            dataGridViewTO.Columns.Add("Column11", "");
                            dataGridViewTO.Columns.Add("Column12", "");
                            dataGridViewTO.Columns.Add("Column13", "");
                            dataGridViewTO.Columns.Add("Column14", "");
                            dataGridViewTO.Columns.Add("Column15", "/");
                            dataGridViewTO.Columns.Add("Column16", "");
                            dataGridViewTO.Columns.Add("Column17", "");
                            dataGridViewTO.Columns.Add("Column18", "");
                            dataGridViewTO.Columns.Add("Column19", "");
                            dataGridViewTO.Columns.Add("Column20", "");

                            dataGridViewTO.Columns.Add("Column21", "");
                            dataGridViewTO.Columns.Add("Column22", "A: B:");
                            dataGridViewTO.Columns.Add("Column23", "");
                            dataGridViewTO.Columns.Add("Column24", "");
                            dataGridViewTO.Columns.Add("Column25", "");
                            dataGridViewTO.Columns.Add("Column26", "");
                            dataGridViewTO.Columns.Add("Column27", "");
                            dataGridViewTO.Columns.Add("Column28", "");
                            dataGridViewTO.Columns.Add("Column29", "");
                            dataGridViewTO.Columns.Add("Column30", "");

                            dataGridViewTO.Columns.Add("Column31", "A：T  B：T+1");
                            dataGridViewTO.Columns.Add("Column32", "");
                            dataGridViewTO.Columns.Add("Column33", "C：Call　P：Put");
                            dataGridViewTO.Columns.Add("Column34", "");
                            dataGridViewTO.Columns.Add("Column35", "(E:)");
                            dataGridViewTO.Columns.Add("Column36", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewTO.Columns.Add("Column37", "13(IOC/FOK)");
                        }
                        //dataGridViewOF
                        {
                            dataGridViewOF.Columns.Add("Column1", "13");
                            dataGridViewOF.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewOF.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewOF.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewOF.Columns.Add("Column5", "");
                            dataGridViewOF.Columns.Add("Column6", "");
                            dataGridViewOF.Columns.Add("Column7", "");
                            dataGridViewOF.Columns.Add("Column8", "");
                            dataGridViewOF.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewOF.Columns.Add("Column10", "");

                            dataGridViewOF.Columns.Add("Column11", "");
                            dataGridViewOF.Columns.Add("Column12", "");
                            dataGridViewOF.Columns.Add("Column13", "");
                            dataGridViewOF.Columns.Add("Column14", "");
                            dataGridViewOF.Columns.Add("Column15", "");
                            dataGridViewOF.Columns.Add("Column16", "");
                            dataGridViewOF.Columns.Add("Column17", "");
                            dataGridViewOF.Columns.Add("Column18", "/");
                            dataGridViewOF.Columns.Add("Column19", "");
                            dataGridViewOF.Columns.Add("Column20", "");

                            dataGridViewOF.Columns.Add("Column21", "");
                            dataGridViewOF.Columns.Add("Column22", "");
                            dataGridViewOF.Columns.Add("Column23", "");
                            dataGridViewOF.Columns.Add("Column24", "");
                            dataGridViewOF.Columns.Add("Column25", "");
                            dataGridViewOF.Columns.Add("Column26", "");
                            dataGridViewOF.Columns.Add("Column27", "");
                            dataGridViewOF.Columns.Add("Column28", "");
                            dataGridViewOF.Columns.Add("Column29", "");
                            dataGridViewOF.Columns.Add("Column30", "");

                            dataGridViewOF.Columns.Add("Column31", "");
                            dataGridViewOF.Columns.Add("Column32", "");
                            dataGridViewOF.Columns.Add("Column33", "");
                            dataGridViewOF.Columns.Add("Column34", "(E:)");
                            dataGridViewOF.Columns.Add("Column35", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewOF.Columns.Add("Column36", "13(IOC/FOK)");
                        }
                        //dataGridViewOO
                        {
                            dataGridViewOO.Columns.Add("Column1", "13");
                            dataGridViewOO.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewOO.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewOO.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewOO.Columns.Add("Column5", "");
                            dataGridViewOO.Columns.Add("Column6", "");
                            dataGridViewOO.Columns.Add("Column7", "");
                            dataGridViewOO.Columns.Add("Column8", "");
                            dataGridViewOO.Columns.Add("Column9", "");
                            dataGridViewOO.Columns.Add("Column10", "(N:「」；D:「」)");

                            dataGridViewOO.Columns.Add("Column11", "");
                            dataGridViewOO.Columns.Add("Column12", "");
                            dataGridViewOO.Columns.Add("Column13", "");
                            dataGridViewOO.Columns.Add("Column14", "");
                            dataGridViewOO.Columns.Add("Column15", "");
                            dataGridViewOO.Columns.Add("Column16", "");
                            dataGridViewOO.Columns.Add("Column17", "");
                            dataGridViewOO.Columns.Add("Column18", "");
                            dataGridViewOO.Columns.Add("Column19", "/");
                            dataGridViewOO.Columns.Add("Column20", "");

                            dataGridViewOO.Columns.Add("Column21", "");
                            dataGridViewOO.Columns.Add("Column22", "");
                            dataGridViewOO.Columns.Add("Column23", "");
                            dataGridViewOO.Columns.Add("Column24", "");
                            dataGridViewOO.Columns.Add("Column25", "");
                            dataGridViewOO.Columns.Add("Column26", "");
                            dataGridViewOO.Columns.Add("Column27", "");
                            dataGridViewOO.Columns.Add("Column28", "");
                            dataGridViewOO.Columns.Add("Column29", "");
                            dataGridViewOO.Columns.Add("Column30", "");

                            dataGridViewOO.Columns.Add("Column31", "");
                            dataGridViewOO.Columns.Add("Column32", "");
                            dataGridViewOO.Columns.Add("Column33", "");
                            dataGridViewOO.Columns.Add("Column34", "");
                            dataGridViewOO.Columns.Add("Column35", "C：Call　P：Put");
                            dataGridViewOO.Columns.Add("Column36", "");
                            dataGridViewOO.Columns.Add("Column37", "");
                            dataGridViewOO.Columns.Add("Column38", "(E:)");
                            dataGridViewOO.Columns.Add("Column39", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewOO.Columns.Add("Column40", "13(IOC/FOK)");
                        }
                        //dataGridViewOS
                        {
                            dataGridViewOS.Columns.Add("Column1", "13");
                            dataGridViewOS.Columns.Add("Column2", "(N: C: U: P:D: B:S:)");
                            dataGridViewOS.Columns.Add("Column3", "OrderErr(Y: T: N:)");
                            dataGridViewOS.Columns.Add("Column4", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewOS.Columns.Add("Column5", "");
                            dataGridViewOS.Columns.Add("Column6", "");
                            dataGridViewOS.Columns.Add("Column7", "");
                            dataGridViewOS.Columns.Add("Column8", "");
                            dataGridViewOS.Columns.Add("Column9", "(N:「」；D:「」)");
                            dataGridViewOS.Columns.Add("Column10", "/");

                            dataGridViewOS.Columns.Add("Column11", "");
                            dataGridViewOS.Columns.Add("Column12", "");
                            dataGridViewOS.Columns.Add("Column13", "");
                            dataGridViewOS.Columns.Add("Column14", "");
                            dataGridViewOS.Columns.Add("Column15", "");
                            dataGridViewOS.Columns.Add("Column16", "");
                            dataGridViewOS.Columns.Add("Column17", "");
                            dataGridViewOS.Columns.Add("Column18", "");
                            dataGridViewOS.Columns.Add("Column19", "");
                            dataGridViewOS.Columns.Add("Column20", "");

                            dataGridViewOS.Columns.Add("Column21", "");
                            dataGridViewOS.Columns.Add("Column22", "(E:)");
                            dataGridViewOS.Columns.Add("Column23", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewOS.Columns.Add("Column24", "13(IOC/FOK)");
                        }
                        //dataGridViewNoClass
                        {
                            dataGridViewNoClass.Columns.Add("Column1", "13");
                            dataGridViewNoClass.Columns.Add("Column2", "(TS: TA: TL: TP:TC: TF: TO:OF:OO: OS:)");
                            dataGridViewNoClass.Columns.Add("Column3", "(N: C: U: P:D: B:S:)");
                            dataGridViewNoClass.Columns.Add("Column4", "OrderErr(Y: T: N:)");
                            dataGridViewNoClass.Columns.Add("Column5", "Broker(TS,TA,TL,TP:  unit noTF,TO: IB  broker id)");
                            dataGridViewNoClass.Columns.Add("Column6", "");
                            dataGridViewNoClass.Columns.Add("Column7", "");
                            dataGridViewNoClass.Columns.Add("Column8", "");
                            dataGridViewNoClass.Columns.Add("Column9", "");
                            dataGridViewNoClass.Columns.Add("Column10", "");

                            dataGridViewNoClass.Columns.Add("Column11", "");
                            dataGridViewNoClass.Columns.Add("Column12", "(N:「」；D:「」)");
                            dataGridViewNoClass.Columns.Add("Column13", "()");
                            dataGridViewNoClass.Columns.Add("Column14", "()");
                            dataGridViewNoClass.Columns.Add("Column15", "()/");
                            dataGridViewNoClass.Columns.Add("Column16", "()");
                            dataGridViewNoClass.Columns.Add("Column17", "()");
                            dataGridViewNoClass.Columns.Add("Column18", "()");
                            dataGridViewNoClass.Columns.Add("Column19", "");
                            dataGridViewNoClass.Columns.Add("Column20", "");

                            dataGridViewNoClass.Columns.Add("Column21", "/");
                            dataGridViewNoClass.Columns.Add("Column22", "");
                            dataGridViewNoClass.Columns.Add("Column23", "");
                            dataGridViewNoClass.Columns.Add("Column24", "");
                            dataGridViewNoClass.Columns.Add("Column25", "");
                            dataGridViewNoClass.Columns.Add("Column26", "(ExecutionNo)");
                            dataGridViewNoClass.Columns.Add("Column27", "");
                            dataGridViewNoClass.Columns.Add("Column28", "");
                            dataGridViewNoClass.Columns.Add("Column29", "");
                            dataGridViewNoClass.Columns.Add("Column30", "");

                            dataGridViewNoClass.Columns.Add("Column31", "");
                            dataGridViewNoClass.Columns.Add("Column32", "A: B:");
                            dataGridViewNoClass.Columns.Add("Column33", "");
                            dataGridViewNoClass.Columns.Add("Column34", "");
                            dataGridViewNoClass.Columns.Add("Column35", "");
                            dataGridViewNoClass.Columns.Add("Column36", "");
                            dataGridViewNoClass.Columns.Add("Column37", "");
                            dataGridViewNoClass.Columns.Add("Column38", "");
                            dataGridViewNoClass.Columns.Add("Column39", "(ExecutionNo)");
                            dataGridViewNoClass.Columns.Add("Column40", "");

                            dataGridViewNoClass.Columns.Add("Column41", "A：T  B：T+1");
                            dataGridViewNoClass.Columns.Add("Column42", "");
                            dataGridViewNoClass.Columns.Add("Column43", "C：Call　P：Put");
                            dataGridViewNoClass.Columns.Add("Column44", "");
                            dataGridViewNoClass.Columns.Add("Column45", "");
                            dataGridViewNoClass.Columns.Add("Column46", "(E:)");
                            dataGridViewNoClass.Columns.Add("Column47", "([00]:2,;[000]:3,;[D],)");
                            dataGridViewNoClass.Columns.Add("Column48", "13(IOC/FOK)");
                            dataGridViewNoClass.Columns.Add("Column49", "[][/][][] :Y");
                        }
                    }
                    // 
                    {
                        // 
                        {
                            //dataGridViewTSMST
                            {
                                //dataGridViewTSMST ()
                                {
                                    dataGridViewTSMST.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSMST.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSMST.Columns.Add("Column3", "()");
                                    dataGridViewTSMST.Columns.Add("Column4", "()");
                                    dataGridViewTSMST.Columns.Add("Column5", "");
                                    dataGridViewTSMST.Columns.Add("Column6", "");
                                    dataGridViewTSMST.Columns.Add("Column7", "");
                                    dataGridViewTSMST.Columns.Add("Column8", "");
                                    dataGridViewTSMST.Columns.Add("Column9", "13");
                                    dataGridViewTSMST.Columns.Add("Column10", "13");

                                    dataGridViewTSMST.Columns.Add("Column11", "");
                                    dataGridViewTSMST.Columns.Add("Column12", "");
                                    dataGridViewTSMST.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSMST.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSMST.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSMST.Columns.Add("Column16", "");
                                    dataGridViewTSMST.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSMST.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSMST.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSMST.Columns.Add("Column20", "");

                                    dataGridViewTSMST.Columns.Add("Column21", "");
                                    dataGridViewTSMST.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSMST.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSMST.Columns.Add("Column24", "");
                                    dataGridViewTSMST.Columns.Add("Column25", "");
                                    dataGridViewTSMST.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSMST.Columns.Add("Column27", "");
                                    dataGridViewTSMST.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSMST.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSMST.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSMST.Columns.Add("Column31", "");
                                    dataGridViewTSMST.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSMST ONLY
                                {
                                    dataGridViewTSMST2.Columns.Add("Column33", "MST:"); //  33
                                    dataGridViewTSMST2.Columns.Add("Column34", "MST:");
                                    dataGridViewTSMST2.Columns.Add("Column35", "MST:");
                                    dataGridViewTSMST2.Columns.Add("Column36", "");
                                    dataGridViewTSMST2.Columns.Add("Column37", "[0: 1: ]");
                                    dataGridViewTSMST2.Columns.Add("Column38", "[]");
                                    dataGridViewTSMST2.Columns.Add("Column39", "[] 0:none 1:  2:");
                                    dataGridViewTSMST2.Columns.Add("Column40", "");
                                    dataGridViewTSMST2.Columns.Add("Column41", "");

                                    dataGridViewTSMST2.Columns.Add("Column42", "");
                                    dataGridViewTSMST2.Columns.Add("Column43", "(true)0:， 1:，，");
                                    dataGridViewTSMST2.Columns.Add("Column44", " 0：None 1： 2：[] 3：");
                                    dataGridViewTSMST2.Columns.Add("Column45", "");
                                    dataGridViewTSMST2.Columns.Add("Column46", "");
                                }
                            }
                            //dataGridViewTSMIOC
                            {
                                //dataGridViewTSMIOC ()
                                {
                                    dataGridViewTSMIOC.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSMIOC.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSMIOC.Columns.Add("Column3", "()");
                                    dataGridViewTSMIOC.Columns.Add("Column4", "()");
                                    dataGridViewTSMIOC.Columns.Add("Column5", "");
                                    dataGridViewTSMIOC.Columns.Add("Column6", "");
                                    dataGridViewTSMIOC.Columns.Add("Column7", "");
                                    dataGridViewTSMIOC.Columns.Add("Column8", "");
                                    dataGridViewTSMIOC.Columns.Add("Column9", "13");
                                    dataGridViewTSMIOC.Columns.Add("Column10", "13");

                                    dataGridViewTSMIOC.Columns.Add("Column11", "");
                                    dataGridViewTSMIOC.Columns.Add("Column12", "");
                                    dataGridViewTSMIOC.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSMIOC.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSMIOC.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSMIOC.Columns.Add("Column16", "");
                                    dataGridViewTSMIOC.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSMIOC.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSMIOC.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSMIOC.Columns.Add("Column20", "");

                                    dataGridViewTSMIOC.Columns.Add("Column21", "");
                                    dataGridViewTSMIOC.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSMIOC.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSMIOC.Columns.Add("Column24", "");
                                    dataGridViewTSMIOC.Columns.Add("Column25", "");
                                    dataGridViewTSMIOC.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSMIOC.Columns.Add("Column27", "");
                                    dataGridViewTSMIOC.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSMIOC.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSMIOC.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSMIOC.Columns.Add("Column31", "");
                                    dataGridViewTSMIOC.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSMIOC ONLY
                                {
                                    dataGridViewTSMIOC2.Columns.Add("Column33", ""); //  33
                                    dataGridViewTSMIOC2.Columns.Add("Column34", "");
                                    dataGridViewTSMIOC2.Columns.Add("Column35", "");
                                    dataGridViewTSMIOC2.Columns.Add("Column36", "");
                                    dataGridViewTSMIOC2.Columns.Add("Column37", "");
                                    dataGridViewTSMIOC2.Columns.Add("Column38", "");
                                }
                            }
                            //dataGridViewTSMIT
                            {
                                //dataGridViewTSMIT ()
                                {
                                    dataGridViewTSMIT.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSMIT.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSMIT.Columns.Add("Column3", "()");
                                    dataGridViewTSMIT.Columns.Add("Column4", "()");
                                    dataGridViewTSMIT.Columns.Add("Column5", "");
                                    dataGridViewTSMIT.Columns.Add("Column6", "");
                                    dataGridViewTSMIT.Columns.Add("Column7", "");
                                    dataGridViewTSMIT.Columns.Add("Column8", "");
                                    dataGridViewTSMIT.Columns.Add("Column9", "13");
                                    dataGridViewTSMIT.Columns.Add("Column10", "13");

                                    dataGridViewTSMIT.Columns.Add("Column11", "");
                                    dataGridViewTSMIT.Columns.Add("Column12", "");
                                    dataGridViewTSMIT.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSMIT.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSMIT.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSMIT.Columns.Add("Column16", "");
                                    dataGridViewTSMIT.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSMIT.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSMIT.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSMIT.Columns.Add("Column20", "");

                                    dataGridViewTSMIT.Columns.Add("Column21", "");
                                    dataGridViewTSMIT.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSMIT.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSMIT.Columns.Add("Column24", "");
                                    dataGridViewTSMIT.Columns.Add("Column25", "");
                                    dataGridViewTSMIT.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSMIT.Columns.Add("Column27", "");
                                    dataGridViewTSMIT.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSMIT.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSMIT.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSMIT.Columns.Add("Column31", "");
                                    dataGridViewTSMIT.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSMIT ONLY
                                {
                                    dataGridViewTSMIT2.Columns.Add("Column33", "，0"); //  33
                                    dataGridViewTSMIT2.Columns.Add("Column34", "");
                                    dataGridViewTSMIT2.Columns.Add("Column35", "");
                                    dataGridViewTSMIT2.Columns.Add("Column36", "");
                                    dataGridViewTSMIT2.Columns.Add("Column37", "");
                                    dataGridViewTSMIT2.Columns.Add("Column38", "");
                                    dataGridViewTSMIT2.Columns.Add("Column39", "");

                                    dataGridViewTSMIT2.Columns.Add("Column40", "(true)0:， 1:，，");
                                    dataGridViewTSMIT2.Columns.Add("Column41", " 0：None 1： 2：[] 3：");
                                    dataGridViewTSMIT2.Columns.Add("Column42", "");
                                    dataGridViewTSMIT2.Columns.Add("Column43", "");
                                }
                            }
                            //dataGridViewTSDayTrading
                            {
                                //dataGridViewTSDayTrading ()
                                {
                                    dataGridViewTSDayTrading.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSDayTrading.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSDayTrading.Columns.Add("Column3", "()");
                                    dataGridViewTSDayTrading.Columns.Add("Column4", "()");
                                    dataGridViewTSDayTrading.Columns.Add("Column5", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column6", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column7", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column8", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column9", "13");
                                    dataGridViewTSDayTrading.Columns.Add("Column10", "13");

                                    dataGridViewTSDayTrading.Columns.Add("Column11", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column12", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSDayTrading.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSDayTrading.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSDayTrading.Columns.Add("Column16", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSDayTrading.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSDayTrading.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSDayTrading.Columns.Add("Column20", "");

                                    dataGridViewTSDayTrading.Columns.Add("Column21", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSDayTrading.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSDayTrading.Columns.Add("Column24", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column25", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSDayTrading.Columns.Add("Column27", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSDayTrading.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSDayTrading.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSDayTrading.Columns.Add("Column31", "");
                                    dataGridViewTSDayTrading.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSDayTrading ONLY
                                {
                                    dataGridViewTSDayTrading2.Columns.Add("Column33", "MIT 0:;1:"); //  33
                                    dataGridViewTSDayTrading2.Columns.Add("Column34", "");
                                    dataGridViewTSDayTrading2.Columns.Add("Column35", "17:18:GTE()19:LTE()20:GTE_LTE(+)");
                                    dataGridViewTSDayTrading2.Columns.Add("Column36", "1:;2:(%)");
                                    dataGridViewTSDayTrading2.Columns.Add("Column37", "");
                                    dataGridViewTSDayTrading2.Columns.Add("Column38", "");
                                    dataGridViewTSDayTrading2.Columns.Add("Column39", "1: ;2:");
                                    dataGridViewTSDayTrading2.Columns.Add("Column40", "");

                                    dataGridViewTSDayTrading2.Columns.Add("Column41", " 0:ROD 3:IOC 4:FOK");
                                    dataGridViewTSDayTrading2.Columns.Add("Column42", "1: ;2:(%)");
                                    dataGridViewTSDayTrading2.Columns.Add("Column43", "");
                                    dataGridViewTSDayTrading2.Columns.Add("Column44", "");
                                    dataGridViewTSDayTrading2.Columns.Add("Column45", " 0:ROD 3:IOC 4:FOK");
                                    dataGridViewTSDayTrading2.Columns.Add("Column46", " 0:;1:");
                                    dataGridViewTSDayTrading2.Columns.Add("Column47", "");
                                    dataGridViewTSDayTrading2.Columns.Add("Column48", " 1:;2:");
                                    dataGridViewTSDayTrading2.Columns.Add("Column49", "");
                                    dataGridViewTSDayTrading2.Columns.Add("Column50", " 0: 3:IOC() 4:FOK()");

                                    dataGridViewTSDayTrading2.Columns.Add("Column51", "0:;1:");
                                    dataGridViewTSDayTrading2.Columns.Add("Column52", "");
                                }
                            }
                            //dataGridViewTSClearOut
                            {
                                //dataGridViewTSClearOut ()
                                {
                                    dataGridViewTSClearOut.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSClearOut.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSClearOut.Columns.Add("Column3", "()");
                                    dataGridViewTSClearOut.Columns.Add("Column4", "()");
                                    dataGridViewTSClearOut.Columns.Add("Column5", "");
                                    dataGridViewTSClearOut.Columns.Add("Column6", "");
                                    dataGridViewTSClearOut.Columns.Add("Column7", "");
                                    dataGridViewTSClearOut.Columns.Add("Column8", "");
                                    dataGridViewTSClearOut.Columns.Add("Column9", "13");
                                    dataGridViewTSClearOut.Columns.Add("Column10", "13");

                                    dataGridViewTSClearOut.Columns.Add("Column11", "");
                                    dataGridViewTSClearOut.Columns.Add("Column12", "");
                                    dataGridViewTSClearOut.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSClearOut.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSClearOut.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSClearOut.Columns.Add("Column16", "");
                                    dataGridViewTSClearOut.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSClearOut.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSClearOut.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSClearOut.Columns.Add("Column20", "");

                                    dataGridViewTSClearOut.Columns.Add("Column21", "");
                                    dataGridViewTSClearOut.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSClearOut.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSClearOut.Columns.Add("Column24", "");
                                    dataGridViewTSClearOut.Columns.Add("Column25", "");
                                    dataGridViewTSClearOut.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSClearOut.Columns.Add("Column27", "");
                                    dataGridViewTSClearOut.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSClearOut.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSClearOut.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSClearOut.Columns.Add("Column31", "");
                                    dataGridViewTSClearOut.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSClearOut ONLY
                                {
                                    dataGridViewTSClearOut2.Columns.Add("Column33", "17: 18:GTE() 19:LTE() 20:GTE_LTE(+)"); //  33
                                    dataGridViewTSClearOut2.Columns.Add("Column34", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column35", " 1:; 2:");
                                    dataGridViewTSClearOut2.Columns.Add("Column36", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column37", " 0:ROD;3:IOC;4:FOK");
                                    dataGridViewTSClearOut2.Columns.Add("Column38", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column39", " 1:; 2:");
                                    dataGridViewTSClearOut2.Columns.Add("Column40", "");

                                    dataGridViewTSClearOut2.Columns.Add("Column41", " 0:ROD;3:IOC;4:FOK");
                                    dataGridViewTSClearOut2.Columns.Add("Column42", " 0:;1:");
                                    dataGridViewTSClearOut2.Columns.Add("Column43", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column44", " 1:; 2:");
                                    dataGridViewTSClearOut2.Columns.Add("Column45", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column46", " 0:ROD;3:IOC;4:FOK");
                                    dataGridViewTSClearOut2.Columns.Add("Column47", " 0:;1:");
                                    dataGridViewTSClearOut2.Columns.Add("Column48", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column49", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column50", "");

                                    dataGridViewTSClearOut2.Columns.Add("Column51", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column52", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column53", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column54", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column55", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column56", "");
                                    dataGridViewTSClearOut2.Columns.Add("Column57", "");
                                }
                            }
                            //dataGridViewTSOCO
                            {
                                //dataGridViewTSOCO ()
                                {
                                    dataGridViewTSOCO.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSOCO.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSOCO.Columns.Add("Column3", "()");
                                    dataGridViewTSOCO.Columns.Add("Column4", "()");
                                    dataGridViewTSOCO.Columns.Add("Column5", "");
                                    dataGridViewTSOCO.Columns.Add("Column6", "");
                                    dataGridViewTSOCO.Columns.Add("Column7", "");
                                    dataGridViewTSOCO.Columns.Add("Column8", "");
                                    dataGridViewTSOCO.Columns.Add("Column9", "13");
                                    dataGridViewTSOCO.Columns.Add("Column10", "13");

                                    dataGridViewTSOCO.Columns.Add("Column11", "");
                                    dataGridViewTSOCO.Columns.Add("Column12", "");
                                    dataGridViewTSOCO.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSOCO.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSOCO.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSOCO.Columns.Add("Column16", "");
                                    dataGridViewTSOCO.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSOCO.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSOCO.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSOCO.Columns.Add("Column20", "");

                                    dataGridViewTSOCO.Columns.Add("Column21", "");
                                    dataGridViewTSOCO.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSOCO.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSOCO.Columns.Add("Column24", "");
                                    dataGridViewTSOCO.Columns.Add("Column25", "");
                                    dataGridViewTSOCO.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSOCO.Columns.Add("Column27", "");
                                    dataGridViewTSOCO.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSOCO.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSOCO.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSOCO.Columns.Add("Column31", "");
                                    dataGridViewTSOCO.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSOCO ONLY
                                {
                                    dataGridViewTSOCO2.Columns.Add("Column33", ""); //  33
                                    dataGridViewTSOCO2.Columns.Add("Column34", "");
                                    dataGridViewTSOCO2.Columns.Add("Column35", "");
                                    dataGridViewTSOCO2.Columns.Add("Column36", " 1： 2：");
                                    dataGridViewTSOCO2.Columns.Add("Column37", " 0:ROD;3:IOC;4:FOK");
                                    dataGridViewTSOCO2.Columns.Add("Column38", "[] 0： 3：() 4：() 8：");
                                    dataGridViewTSOCO2.Columns.Add("Column39", "");
                                    dataGridViewTSOCO2.Columns.Add("Column40", "0: 1: 2: 7");
                                }
                            }
                            //dataGridViewTSAB
                            {
                                //dataGridViewTSAB ()
                                {
                                    dataGridViewTSAB.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSAB.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSAB.Columns.Add("Column3", "()");
                                    dataGridViewTSAB.Columns.Add("Column4", "()");
                                    dataGridViewTSAB.Columns.Add("Column5", "");
                                    dataGridViewTSAB.Columns.Add("Column6", "");
                                    dataGridViewTSAB.Columns.Add("Column7", "");
                                    dataGridViewTSAB.Columns.Add("Column8", "");
                                    dataGridViewTSAB.Columns.Add("Column9", "13");
                                    dataGridViewTSAB.Columns.Add("Column10", "13");

                                    dataGridViewTSAB.Columns.Add("Column11", "");
                                    dataGridViewTSAB.Columns.Add("Column12", "");
                                    dataGridViewTSAB.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSAB.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSAB.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSAB.Columns.Add("Column16", "");
                                    dataGridViewTSAB.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSAB.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSAB.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSAB.Columns.Add("Column20", "");

                                    dataGridViewTSAB.Columns.Add("Column21", "");
                                    dataGridViewTSAB.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSAB.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSAB.Columns.Add("Column24", "");
                                    dataGridViewTSAB.Columns.Add("Column25", "");
                                    dataGridViewTSAB.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSAB.Columns.Add("Column27", "");
                                    dataGridViewTSAB.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSAB.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSAB.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSAB.Columns.Add("Column31", "");
                                    dataGridViewTSAB.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSAB ONLY
                                {
                                    dataGridViewTSAB2.Columns.Add("Column33", ""); //  33
                                }
                            }
                            //dataGridViewTSCB
                            {
                                //dataGridViewTSCB ()
                                {
                                    dataGridViewTSCB.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTSCB.Columns.Add("Column2", "0:  1: 2:");
                                    dataGridViewTSCB.Columns.Add("Column3", "()");
                                    dataGridViewTSCB.Columns.Add("Column4", "()");
                                    dataGridViewTSCB.Columns.Add("Column5", "");
                                    dataGridViewTSCB.Columns.Add("Column6", "");
                                    dataGridViewTSCB.Columns.Add("Column7", "");
                                    dataGridViewTSCB.Columns.Add("Column8", "");
                                    dataGridViewTSCB.Columns.Add("Column9", "13");
                                    dataGridViewTSCB.Columns.Add("Column10", "13");

                                    dataGridViewTSCB.Columns.Add("Column11", "");
                                    dataGridViewTSCB.Columns.Add("Column12", "");
                                    dataGridViewTSCB.Columns.Add("Column13", "B:  S:");
                                    dataGridViewTSCB.Columns.Add("Column14", "0： 3：)4：) 8：");
                                    dataGridViewTSCB.Columns.Add("Column15", "0= ();1: ;2:;7:");
                                    dataGridViewTSCB.Columns.Add("Column16", "");
                                    dataGridViewTSCB.Columns.Add("Column17", "1;2;3:");
                                    dataGridViewTSCB.Columns.Add("Column18", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTSCB.Columns.Add("Column19", "TS:; TF:");
                                    dataGridViewTSCB.Columns.Add("Column20", "");

                                    dataGridViewTSCB.Columns.Add("Column21", "");
                                    dataGridViewTSCB.Columns.Add("Column22", "0: None;1:GTE(); 2:LTE()");
                                    dataGridViewTSCB.Columns.Add("Column23", " ：() ：:；:Y");
                                    dataGridViewTSCB.Columns.Add("Column24", "");
                                    dataGridViewTSCB.Columns.Add("Column25", "");
                                    dataGridViewTSCB.Columns.Add("Column26", "USER PC IP");
                                    dataGridViewTSCB.Columns.Add("Column27", "");
                                    dataGridViewTSCB.Columns.Add("Column28", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTSCB.Columns.Add("Column29", "(Y;N)");
                                    dataGridViewTSCB.Columns.Add("Column30", "EX:[][]");

                                    dataGridViewTSCB.Columns.Add("Column31", "");
                                    dataGridViewTSCB.Columns.Add("Column32", " Sataus999");
                                }
                                //dataGridViewTSCB ONLY
                                {
                                    dataGridViewTSCB2.Columns.Add("Column33", " AND: true OR: flase"); //  33
                                    dataGridViewTSCB2.Columns.Add("Column34", "");
                                    dataGridViewTSCB2.Columns.Add("Column35", "");
                                    dataGridViewTSCB2.Columns.Add("Column36", "");
                                    dataGridViewTSCB2.Columns.Add("Column37", "");
                                    dataGridViewTSCB2.Columns.Add("Column38", " 0：None 1：GTE 2：LTE");
                                    dataGridViewTSCB2.Columns.Add("Column39", "");
                                    dataGridViewTSCB2.Columns.Add("Column40", "");

                                    dataGridViewTSCB2.Columns.Add("Column41", " 0：None 1：GTE 2：LTE");
                                    dataGridViewTSCB2.Columns.Add("Column42", "");
                                    dataGridViewTSCB2.Columns.Add("Column43", "");
                                    dataGridViewTSCB2.Columns.Add("Column44", " 0：None 1：GTE 2：LTE");
                                    dataGridViewTSCB2.Columns.Add("Column45", "tick");
                                    dataGridViewTSCB2.Columns.Add("Column46", "Tick");
                                    dataGridViewTSCB2.Columns.Add("Column47", "tick 0：None 1：GTE 2：LTE");
                                    dataGridViewTSCB2.Columns.Add("Column48", "(%)");
                                    dataGridViewTSCB2.Columns.Add("Column49", "(%)");
                                    dataGridViewTSCB2.Columns.Add("Column50", " 0：None 1：GTE 2：LTE");
                                    
                                    dataGridViewTSCB2.Columns.Add("Column51", "");
                                    dataGridViewTSCB2.Columns.Add("Column52", "");
                                    dataGridViewTSCB2.Columns.Add("Column53", " 0：None 1：GTE 2：LTE");
                                    dataGridViewTSCB2.Columns.Add("Column54", " 0： 1：");
                                    dataGridViewTSCB2.Columns.Add("Column55", " 0： 1：");
                                    dataGridViewTSCB2.Columns.Add("Column56", " 0： 1：");
                                    dataGridViewTSCB2.Columns.Add("Column57", "tick 0： 1：");
                                    dataGridViewTSCB2.Columns.Add("Column58", " 0： 1：");
                                    dataGridViewTSCB2.Columns.Add("Column59", " 0： 1：");
                                    dataGridViewTSCB2.Columns.Add("Column60", "");

                                    dataGridViewTSCB2.Columns.Add("Column61", "");
                                    dataGridViewTSCB2.Columns.Add("Column62", "");
                                    dataGridViewTSCB2.Columns.Add("Column63", "ticktick");
                                    dataGridViewTSCB2.Columns.Add("Column64", "");
                                    dataGridViewTSCB2.Columns.Add("Column65", "");
                                    dataGridViewTSCB2.Columns.Add("Column66", "");
                                    dataGridViewTSCB2.Columns.Add("Column67", "");
                                    dataGridViewTSCB2.Columns.Add("Column68", " 0：None 1：GTE 2：LTE");
                                    dataGridViewTSCB2.Columns.Add("Column69", " 0： 1：");
                                    dataGridViewTSCB2.Columns.Add("Column70", "");
                                }
                            }
                            
                        }
                        // 
                        {
                            //dataGridViewTFSTP
                            {
                                //dataGridViewTFSTP ()
                                {
                                    dataGridViewTFSTP.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTFSTP.Columns.Add("Column2", "()");
                                    dataGridViewTFSTP.Columns.Add("Column3", "()");
                                    dataGridViewTFSTP.Columns.Add("Column4", "");
                                    dataGridViewTFSTP.Columns.Add("Column5", "");
                                    dataGridViewTFSTP.Columns.Add("Column6", "");
                                    dataGridViewTFSTP.Columns.Add("Column7", "");
                                    dataGridViewTFSTP.Columns.Add("Column8", "13");
                                    dataGridViewTFSTP.Columns.Add("Column9", "13");
                                    dataGridViewTFSTP.Columns.Add("Column10", "");

                                    dataGridViewTFSTP.Columns.Add("Column11", "");
                                    dataGridViewTFSTP.Columns.Add("Column12", "B:  S:");
                                    dataGridViewTFSTP.Columns.Add("Column13", "0= ();1: ;2:;7:");
                                    dataGridViewTFSTP.Columns.Add("Column14", "");
                                    dataGridViewTFSTP.Columns.Add("Column15", "1;2;3: :");
                                    dataGridViewTFSTP.Columns.Add("Column16", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTFSTP.Columns.Add("Column17", "TS:; TF:");
                                    dataGridViewTFSTP.Columns.Add("Column18", "");
                                    dataGridViewTFSTP.Columns.Add("Column19", "");
                                    dataGridViewTFSTP.Columns.Add("Column20", "0: None;1:GTE(); 2:LTE()");

                                    dataGridViewTFSTP.Columns.Add("Column21", " ：:；:Y");
                                    dataGridViewTFSTP.Columns.Add("Column22", "");
                                    dataGridViewTFSTP.Columns.Add("Column23", "");
                                    dataGridViewTFSTP.Columns.Add("Column24", "USER PC IP");
                                    dataGridViewTFSTP.Columns.Add("Column25", "");
                                    dataGridViewTFSTP.Columns.Add("Column26", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTFSTP.Columns.Add("Column27", "(Y;N)");
                                    dataGridViewTFSTP.Columns.Add("Column28", "EX:[][]");
                                    dataGridViewTFSTP.Columns.Add("Column29", "");
                                    dataGridViewTFSTP.Columns.Add("Column30", "[] 0:;1:; 2:");

                                    dataGridViewTFSTP.Columns.Add("Column31", "[]EX: 202212");
                                    dataGridViewTFSTP.Columns.Add("Column32", "[]");
                                    dataGridViewTFSTP.Columns.Add("Column33", "[]0: ;1:");
                                    dataGridViewTFSTP.Columns.Add("Column34", "[]N: C:CallP:Put ");
                                    dataGridViewTFSTP.Columns.Add("Column35", "[]0:;1: ");
                                    dataGridViewTFSTP.Columns.Add("Column36", "[]0:;1:");
                                    dataGridViewTFSTP.Columns.Add("Column37", "[] 0");
                                    dataGridViewTFSTP.Columns.Add("Column38", "[]");
                                    dataGridViewTFSTP.Columns.Add("Column39", "[]EX:TF");

                                    dataGridViewTFSTP.Columns.Add("Column40", "[]EX: TAIFEX");
                                    dataGridViewTFSTP.Columns.Add("Column41", "[]EX:FITX");
                                    dataGridViewTFSTP.Columns.Add("Column42", "[]");
                                    dataGridViewTFSTP.Columns.Add("Column43", "[]");
                                    dataGridViewTFSTP.Columns.Add("Column44", "[]");
                                    dataGridViewTFSTP.Columns.Add("Column45", " Sataus999");
                                }
                                //dataGridViewTFSTP ONLY
                                {
                                    dataGridViewTFSTP2.Columns.Add("Column46", ""); //  46
                                    dataGridViewTFSTP2.Columns.Add("Column47", "");
                                    dataGridViewTFSTP2.Columns.Add("Column48", "");
                                    dataGridViewTFSTP2.Columns.Add("Column49", "(true) 0:， 1:，，");

                                    dataGridViewTFSTP2.Columns.Add("Column50", " 0：None 1： 2：[] 3：");
                                    dataGridViewTFSTP2.Columns.Add("Column51", "");
                                    dataGridViewTFSTP2.Columns.Add("Column52", "");
                                }
                            }
                            //dataGridViewTFMST
                            {
                                //dataGridViewTFMST ()
                                {
                                    dataGridViewTFMST.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTFMST.Columns.Add("Column2", "()");
                                    dataGridViewTFMST.Columns.Add("Column3", "()");
                                    dataGridViewTFMST.Columns.Add("Column4", "");
                                    dataGridViewTFMST.Columns.Add("Column5", "");
                                    dataGridViewTFMST.Columns.Add("Column6", "");
                                    dataGridViewTFMST.Columns.Add("Column7", "");
                                    dataGridViewTFMST.Columns.Add("Column8", "13");
                                    dataGridViewTFMST.Columns.Add("Column9", "13");
                                    dataGridViewTFMST.Columns.Add("Column10", "");

                                    dataGridViewTFMST.Columns.Add("Column11", "");
                                    dataGridViewTFMST.Columns.Add("Column12", "B:  S:");
                                    dataGridViewTFMST.Columns.Add("Column13", "0= ();1: ;2:;7:");
                                    dataGridViewTFMST.Columns.Add("Column14", "");
                                    dataGridViewTFMST.Columns.Add("Column15", "1;2;3: :");
                                    dataGridViewTFMST.Columns.Add("Column16", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTFMST.Columns.Add("Column17", "TS:; TF:");
                                    dataGridViewTFMST.Columns.Add("Column18", "");
                                    dataGridViewTFMST.Columns.Add("Column19", "");
                                    dataGridViewTFMST.Columns.Add("Column20", "0: None;1:GTE(); 2:LTE()");

                                    dataGridViewTFMST.Columns.Add("Column21", " ：:；:Y");
                                    dataGridViewTFMST.Columns.Add("Column22", "");
                                    dataGridViewTFMST.Columns.Add("Column23", "");
                                    dataGridViewTFMST.Columns.Add("Column24", "USER PC IP");
                                    dataGridViewTFMST.Columns.Add("Column25", "");
                                    dataGridViewTFMST.Columns.Add("Column26", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTFMST.Columns.Add("Column27", "(Y;N)");
                                    dataGridViewTFMST.Columns.Add("Column28", "EX:[][]");
                                    dataGridViewTFMST.Columns.Add("Column29", "");
                                    dataGridViewTFMST.Columns.Add("Column30", "[] 0:;1:; 2:");

                                    dataGridViewTFMST.Columns.Add("Column31", "[]EX: 202212");
                                    dataGridViewTFMST.Columns.Add("Column32", "[]");
                                    dataGridViewTFMST.Columns.Add("Column33", "[]0: ;1:");
                                    dataGridViewTFMST.Columns.Add("Column34", "[]N: C:CallP:Put ");
                                    dataGridViewTFMST.Columns.Add("Column35", "[]0:;1: ");
                                    dataGridViewTFMST.Columns.Add("Column36", "[]0:;1:");
                                    dataGridViewTFMST.Columns.Add("Column37", "[] 0");
                                    dataGridViewTFMST.Columns.Add("Column38", "[]");
                                    dataGridViewTFMST.Columns.Add("Column39", "[]EX:TF");

                                    dataGridViewTFMST.Columns.Add("Column40", "[]EX: TAIFEX");
                                    dataGridViewTFMST.Columns.Add("Column41", "[]EX:FITX");
                                    dataGridViewTFMST.Columns.Add("Column42", "[]");
                                    dataGridViewTFMST.Columns.Add("Column43", "[]");
                                    dataGridViewTFMST.Columns.Add("Column44", "[]");
                                    dataGridViewTFMST.Columns.Add("Column45", " Sataus999");
                                }
                                //dataGridViewTFMST ONLY
                                {
                                    dataGridViewTFMST2.Columns.Add("Column46", "MST:"); //  46
                                    dataGridViewTFMST2.Columns.Add("Column47", "MST:");
                                    dataGridViewTFMST2.Columns.Add("Column48", "MST:");
                                    dataGridViewTFMST2.Columns.Add("Column49", "");
                                }
                            }
                            //dataGridViewTFMIT
                            {
                                //dataGridViewTFMIT ()
                                {
                                    dataGridViewTFMIT.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTFMIT.Columns.Add("Column2", "()");
                                    dataGridViewTFMIT.Columns.Add("Column3", "()");
                                    dataGridViewTFMIT.Columns.Add("Column4", "");
                                    dataGridViewTFMIT.Columns.Add("Column5", "");
                                    dataGridViewTFMIT.Columns.Add("Column6", "");
                                    dataGridViewTFMIT.Columns.Add("Column7", "");
                                    dataGridViewTFMIT.Columns.Add("Column8", "13");
                                    dataGridViewTFMIT.Columns.Add("Column9", "13");
                                    dataGridViewTFMIT.Columns.Add("Column10", "");

                                    dataGridViewTFMIT.Columns.Add("Column11", "");
                                    dataGridViewTFMIT.Columns.Add("Column12", "B:  S:");
                                    dataGridViewTFMIT.Columns.Add("Column13", "0= ();1: ;2:;7:");
                                    dataGridViewTFMIT.Columns.Add("Column14", "");
                                    dataGridViewTFMIT.Columns.Add("Column15", "1;2;3: :");
                                    dataGridViewTFMIT.Columns.Add("Column16", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTFMIT.Columns.Add("Column17", "TS:; TF:");
                                    dataGridViewTFMIT.Columns.Add("Column18", "");
                                    dataGridViewTFMIT.Columns.Add("Column19", "");
                                    dataGridViewTFMIT.Columns.Add("Column20", "0: None;1:GTE(); 2:LTE()");

                                    dataGridViewTFMIT.Columns.Add("Column21", " ：:；:Y");
                                    dataGridViewTFMIT.Columns.Add("Column22", "");
                                    dataGridViewTFMIT.Columns.Add("Column23", "");
                                    dataGridViewTFMIT.Columns.Add("Column24", "USER PC IP");
                                    dataGridViewTFMIT.Columns.Add("Column25", "");
                                    dataGridViewTFMIT.Columns.Add("Column26", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTFMIT.Columns.Add("Column27", "(Y;N)");
                                    dataGridViewTFMIT.Columns.Add("Column28", "EX:[][]");
                                    dataGridViewTFMIT.Columns.Add("Column29", "");
                                    dataGridViewTFMIT.Columns.Add("Column30", "[] 0:;1:; 2:");

                                    dataGridViewTFMIT.Columns.Add("Column31", "[]EX: 202212");
                                    dataGridViewTFMIT.Columns.Add("Column32", "[]");
                                    dataGridViewTFMIT.Columns.Add("Column33", "[]0: ;1:");
                                    dataGridViewTFMIT.Columns.Add("Column34", "[]N: C:CallP:Put ");
                                    dataGridViewTFMIT.Columns.Add("Column35", "[]0:;1: ");
                                    dataGridViewTFMIT.Columns.Add("Column36", "[]0:;1:");
                                    dataGridViewTFMIT.Columns.Add("Column37", "[] 0");
                                    dataGridViewTFMIT.Columns.Add("Column38", "[]");
                                    
                                    dataGridViewTFMIT.Columns.Add("Column39", "[]EX:TF");

                                    dataGridViewTFMIT.Columns.Add("Column40", "[]EX: TAIFEX");
                                    dataGridViewTFMIT.Columns.Add("Column41", "[]EX:FITX");
                                    dataGridViewTFMIT.Columns.Add("Column42", "[]");
                                    dataGridViewTFMIT.Columns.Add("Column43", "[]");
                                    dataGridViewTFMIT.Columns.Add("Column44", "[]");
                                    dataGridViewTFMIT.Columns.Add("Column45", " Sataus999");
                                }
                                //dataGridViewTFMIT ONLY
                                {
                                    dataGridViewTFMIT2.Columns.Add("Column46", "，0"); //  46
                                    dataGridViewTFMIT2.Columns.Add("Column47", "");
                                }
                            }
                            //dataGridViewTFOCO
                            {
                                //dataGridViewTFOCO ()
                                {
                                    dataGridViewTFOCO.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTFOCO.Columns.Add("Column2", "()");
                                    dataGridViewTFOCO.Columns.Add("Column3", "()");
                                    dataGridViewTFOCO.Columns.Add("Column4", "");
                                    dataGridViewTFOCO.Columns.Add("Column5", "");
                                    dataGridViewTFOCO.Columns.Add("Column6", "");
                                    dataGridViewTFOCO.Columns.Add("Column7", "");
                                    dataGridViewTFOCO.Columns.Add("Column8", "13");
                                    dataGridViewTFOCO.Columns.Add("Column9", "13");
                                    dataGridViewTFOCO.Columns.Add("Column10", "");

                                    dataGridViewTFOCO.Columns.Add("Column11", "");
                                    dataGridViewTFOCO.Columns.Add("Column12", "B:  S:");
                                    dataGridViewTFOCO.Columns.Add("Column13", "0= ();1: ;2:;7:");
                                    dataGridViewTFOCO.Columns.Add("Column14", "");
                                    dataGridViewTFOCO.Columns.Add("Column15", "1;2;3: :");
                                    dataGridViewTFOCO.Columns.Add("Column16", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTFOCO.Columns.Add("Column17", "TS:; TF:");
                                    dataGridViewTFOCO.Columns.Add("Column18", "");
                                    dataGridViewTFOCO.Columns.Add("Column19", "");
                                    dataGridViewTFOCO.Columns.Add("Column20", "0: None;1:GTE(); 2:LTE()");

                                    dataGridViewTFOCO.Columns.Add("Column21", " ：:；:Y");
                                    dataGridViewTFOCO.Columns.Add("Column22", "");
                                    dataGridViewTFOCO.Columns.Add("Column23", "");
                                    dataGridViewTFOCO.Columns.Add("Column24", "USER PC IP");
                                    dataGridViewTFOCO.Columns.Add("Column25", "");
                                    dataGridViewTFOCO.Columns.Add("Column26", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTFOCO.Columns.Add("Column27", "(Y;N)");
                                    dataGridViewTFOCO.Columns.Add("Column28", "EX:[][]");
                                    dataGridViewTFOCO.Columns.Add("Column29", "");
                                    dataGridViewTFOCO.Columns.Add("Column30", "[] 0:;1:; 2:");

                                    dataGridViewTFOCO.Columns.Add("Column31", "[]EX: 202212");
                                    dataGridViewTFOCO.Columns.Add("Column32", "[]");
                                    dataGridViewTFOCO.Columns.Add("Column33", "[]0: ;1:");
                                    dataGridViewTFOCO.Columns.Add("Column34", "[]N: C:CallP:Put ");
                                    dataGridViewTFOCO.Columns.Add("Column35", "[]0:;1: ");
                                    dataGridViewTFOCO.Columns.Add("Column36", "[]0:;1:");
                                    dataGridViewTFOCO.Columns.Add("Column37", "[] 0");
                                    dataGridViewTFOCO.Columns.Add("Column38", "[]");
                                    
                                    dataGridViewTFOCO.Columns.Add("Column39", "[]EX:TF");

                                    dataGridViewTFOCO.Columns.Add("Column40", "[]EX: TAIFEX");
                                    dataGridViewTFOCO.Columns.Add("Column41", "[]EX:FITX");
                                    dataGridViewTFOCO.Columns.Add("Column42", "[]");
                                    dataGridViewTFOCO.Columns.Add("Column43", "[]");
                                    dataGridViewTFOCO.Columns.Add("Column44", "[]");
                                    dataGridViewTFOCO.Columns.Add("Column45", " Sataus999");
                                }
                                //dataGridViewTFOCO ONLY
                                {
                                    dataGridViewTFOCO2.Columns.Add("Column46", ""); //  46
                                    dataGridViewTFOCO2.Columns.Add("Column47", "");
                                    dataGridViewTFOCO2.Columns.Add("Column48", "");
                                    dataGridViewTFOCO2.Columns.Add("Column49", " 1： 2：");

                                    dataGridViewTFOCO2.Columns.Add("Column50", " 0:ROD;3:IOC;4:FOK");
                                    dataGridViewTFOCO2.Columns.Add("Column51", "");
                                    dataGridViewTFOCO2.Columns.Add("Column52", " 0: 1: 2: 7");
                                    dataGridViewTFOCO2.Columns.Add("Column53", "[]");
                                    dataGridViewTFOCO2.Columns.Add("Column54", "");
                                    dataGridViewTFOCO2.Columns.Add("Column55", "");
                                    dataGridViewTFOCO2.Columns.Add("Column56", "");
                                    dataGridViewTFOCO2.Columns.Add("Column57", "(true) 0:， 1:，，");
                                    dataGridViewTFOCO2.Columns.Add("Column58", " 0：None 1： 2：[] 3：");
                                    dataGridViewTFOCO2.Columns.Add("Column59", "");

                                    dataGridViewTFOCO2.Columns.Add("Column60", "");
                                }
                            }
                            //dataGridViewTFAB
                            {
                                //dataGridViewTFAB ()
                                {
                                    dataGridViewTFAB.Columns.Add("Column1", "1:  2:");
                                    dataGridViewTFAB.Columns.Add("Column2", "()");
                                    dataGridViewTFAB.Columns.Add("Column3", "()");
                                    dataGridViewTFAB.Columns.Add("Column4", "");
                                    dataGridViewTFAB.Columns.Add("Column5", "");
                                    dataGridViewTFAB.Columns.Add("Column6", "");
                                    dataGridViewTFAB.Columns.Add("Column7", "");
                                    dataGridViewTFAB.Columns.Add("Column8", "13");
                                    dataGridViewTFAB.Columns.Add("Column9", "13");
                                    dataGridViewTFAB.Columns.Add("Column10", "");

                                    dataGridViewTFAB.Columns.Add("Column11", "");
                                    dataGridViewTFAB.Columns.Add("Column12", "B:  S:");
                                    dataGridViewTFAB.Columns.Add("Column13", "0= ();1: ;2:;7:");
                                    dataGridViewTFAB.Columns.Add("Column14", "");
                                    dataGridViewTFAB.Columns.Add("Column15", "1;2;3: :");
                                    dataGridViewTFAB.Columns.Add("Column16", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewTFAB.Columns.Add("Column17", "TS:; TF:");
                                    dataGridViewTFAB.Columns.Add("Column18", "");
                                    dataGridViewTFAB.Columns.Add("Column19", "");
                                    dataGridViewTFAB.Columns.Add("Column20", "0: None;1:GTE(); 2:LTE()");

                                    dataGridViewTFAB.Columns.Add("Column21", " ：:；:Y");
                                    dataGridViewTFAB.Columns.Add("Column22", "");
                                    dataGridViewTFAB.Columns.Add("Column23", "");
                                    dataGridViewTFAB.Columns.Add("Column24", "USER PC IP");
                                    dataGridViewTFAB.Columns.Add("Column25", "");
                                    dataGridViewTFAB.Columns.Add("Column26", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewTFAB.Columns.Add("Column27", "(Y;N)");
                                    dataGridViewTFAB.Columns.Add("Column28", "EX:[][]");
                                    dataGridViewTFAB.Columns.Add("Column29", "");
                                    dataGridViewTFAB.Columns.Add("Column30", "[] 0:;1:; 2:");

                                    dataGridViewTFAB.Columns.Add("Column31", "[]EX: 202212");
                                    dataGridViewTFAB.Columns.Add("Column32", "[]");
                                    dataGridViewTFAB.Columns.Add("Column33", "[]0: ;1:");
                                    dataGridViewTFAB.Columns.Add("Column34", "[]N: C:CallP:Put ");
                                    dataGridViewTFAB.Columns.Add("Column35", "[]0:;1: ");
                                    dataGridViewTFAB.Columns.Add("Column36", "[]0:;1:");
                                    dataGridViewTFAB.Columns.Add("Column37", "[] 0");
                                    dataGridViewTFAB.Columns.Add("Column38", "[]");
                                    
                                    dataGridViewTFAB.Columns.Add("Column39", "[]EX:TF");

                                    dataGridViewTFAB.Columns.Add("Column40", "[]EX: TAIFEX");
                                    dataGridViewTFAB.Columns.Add("Column41", "[]EX:FITX");
                                    dataGridViewTFAB.Columns.Add("Column42", "[]");
                                    dataGridViewTFAB.Columns.Add("Column43", "[]");
                                    dataGridViewTFAB.Columns.Add("Column44", "[]");
                                    dataGridViewTFAB.Columns.Add("Column45", " Sataus999");
                                }
                                //dataGridViewTFAB ONLY
                                {
                                    dataGridViewTFAB2.Columns.Add("Column46", ""); //  46
                                }
                            }
                        }
                        // 
                        {
                            //dataGridViewOFOCO
                            {
                                //dataGridViewOFOCO ()
                                {
                                    dataGridViewOFOCO.Columns.Add("Column1", "1:  2:");
                                    dataGridViewOFOCO.Columns.Add("Column2", "()");
                                    dataGridViewOFOCO.Columns.Add("Column3", "()");
                                    dataGridViewOFOCO.Columns.Add("Column4", "");
                                    dataGridViewOFOCO.Columns.Add("Column5", "");
                                    dataGridViewOFOCO.Columns.Add("Column6", "");
                                    dataGridViewOFOCO.Columns.Add("Column7", "");
                                    dataGridViewOFOCO.Columns.Add("Column8", "13");
                                    dataGridViewOFOCO.Columns.Add("Column9", "13");
                                    dataGridViewOFOCO.Columns.Add("Column10", "");

                                    dataGridViewOFOCO.Columns.Add("Column11", "");
                                    dataGridViewOFOCO.Columns.Add("Column12", "B:  S:");
                                    dataGridViewOFOCO.Columns.Add("Column13", "0= ();1: ;2:;7:");
                                    dataGridViewOFOCO.Columns.Add("Column14", "");
                                    dataGridViewOFOCO.Columns.Add("Column15", "1;2;3: :");
                                    dataGridViewOFOCO.Columns.Add("Column16", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewOFOCO.Columns.Add("Column17", "TS:; TF:");
                                    dataGridViewOFOCO.Columns.Add("Column18", "");
                                    dataGridViewOFOCO.Columns.Add("Column19", "");
                                    dataGridViewOFOCO.Columns.Add("Column20", "0: None;1:GTE(); 2:LTE()");

                                    dataGridViewOFOCO.Columns.Add("Column21", " ：:；:Y");
                                    dataGridViewOFOCO.Columns.Add("Column22", "");
                                    dataGridViewOFOCO.Columns.Add("Column23", "");
                                    dataGridViewOFOCO.Columns.Add("Column24", "USER PC IP");
                                    dataGridViewOFOCO.Columns.Add("Column25", "");
                                    dataGridViewOFOCO.Columns.Add("Column26", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewOFOCO.Columns.Add("Column27", "(Y;N)");
                                    dataGridViewOFOCO.Columns.Add("Column28", "EX:[][]");
                                    dataGridViewOFOCO.Columns.Add("Column29", "");
                                    dataGridViewOFOCO.Columns.Add("Column30", " 0:;1:; 2:");

                                    dataGridViewOFOCO.Columns.Add("Column31", "EX: 202212");
                                    dataGridViewOFOCO.Columns.Add("Column32", "");
                                    dataGridViewOFOCO.Columns.Add("Column33", "0: ;1:");
                                    dataGridViewOFOCO.Columns.Add("Column34", "N: C:CallP:Put ");
                                    dataGridViewOFOCO.Columns.Add("Column35", "0:;1: ");
                                    dataGridViewOFOCO.Columns.Add("Column36", "0:;1:");
                                    dataGridViewOFOCO.Columns.Add("Column37", " 0");
                                    dataGridViewOFOCO.Columns.Add("Column38", "");
                                    
                                    dataGridViewOFOCO.Columns.Add("Column39", "EX:TF");

                                    dataGridViewOFOCO.Columns.Add("Column40", "EX: TAIFEX");
                                    dataGridViewOFOCO.Columns.Add("Column41", "EX:FITX");
                                    dataGridViewOFOCO.Columns.Add("Column42", "");
                                    dataGridViewOFOCO.Columns.Add("Column43", "");
                                    dataGridViewOFOCO.Columns.Add("Column44", "");
                                    dataGridViewOFOCO.Columns.Add("Column45", " Sataus999");
                                }
                                //dataGridViewOFOCO ONLY
                                {
                                    dataGridViewOFOCO2.Columns.Add("Column46", ""); //  46
                                    dataGridViewOFOCO2.Columns.Add("Column47", "");
                                    dataGridViewOFOCO2.Columns.Add("Column48", "");
                                    dataGridViewOFOCO2.Columns.Add("Column49", "");
                                    dataGridViewOFOCO2.Columns.Add("Column50", "");

                                    dataGridViewOFOCO2.Columns.Add("Column51", "");
                                    dataGridViewOFOCO2.Columns.Add("Column52", "");
                                    dataGridViewOFOCO2.Columns.Add("Column53", "");
                                    dataGridViewOFOCO2.Columns.Add("Column54", "");
                                    dataGridViewOFOCO2.Columns.Add("Column55", " 1： 2：");
                                    dataGridViewOFOCO2.Columns.Add("Column56", " 0：ROD 3：IOC 4：FOK");
                                    dataGridViewOFOCO2.Columns.Add("Column57", "");
                                    dataGridViewOFOCO2.Columns.Add("Column58", " 0： 1： 2：");
                                    dataGridViewOFOCO2.Columns.Add("Column59", "");
                                    dataGridViewOFOCO2.Columns.Add("Column60", "");

                                    dataGridViewOFOCO2.Columns.Add("Column61", "");
                                    dataGridViewOFOCO2.Columns.Add("Column62", "(true) 0:， 1:，，");
                                    dataGridViewOFOCO2.Columns.Add("Column63", " 0：None 1： 2：[] 3：");
                                    dataGridViewOFOCO2.Columns.Add("Column64", "");
                                    dataGridViewOFOCO2.Columns.Add("Column65", "");
                                }
                            }
                            //dataGridViewOFAB
                            {
                                //dataGridViewOFAB ()
                                {
                                    dataGridViewOFAB.Columns.Add("Column1", "1:  2:");
                                    dataGridViewOFAB.Columns.Add("Column2", "()");
                                    dataGridViewOFAB.Columns.Add("Column3", "()");
                                    dataGridViewOFAB.Columns.Add("Column4", "");
                                    dataGridViewOFAB.Columns.Add("Column5", "");
                                    dataGridViewOFAB.Columns.Add("Column6", "");
                                    dataGridViewOFAB.Columns.Add("Column7", "");
                                    dataGridViewOFAB.Columns.Add("Column8", "13");
                                    dataGridViewOFAB.Columns.Add("Column9", "13");
                                    dataGridViewOFAB.Columns.Add("Column10", "");

                                    dataGridViewOFAB.Columns.Add("Column11", "");
                                    dataGridViewOFAB.Columns.Add("Column12", "B:  S:");
                                    dataGridViewOFAB.Columns.Add("Column13", "0= ();1: ;2:;7:");
                                    dataGridViewOFAB.Columns.Add("Column14", "");
                                    dataGridViewOFAB.Columns.Add("Column15", "1;2;3: :");
                                    dataGridViewOFAB.Columns.Add("Column16", "0：ROD 3：IOC  4：FOK");
                                    dataGridViewOFAB.Columns.Add("Column17", "TS:; TF:");
                                    dataGridViewOFAB.Columns.Add("Column18", "");
                                    dataGridViewOFAB.Columns.Add("Column19", "");
                                    dataGridViewOFAB.Columns.Add("Column20", "0: None;1:GTE(); 2:LTE()");

                                    dataGridViewOFAB.Columns.Add("Column21", " ：:；:Y");
                                    dataGridViewOFAB.Columns.Add("Column22", "");
                                    dataGridViewOFAB.Columns.Add("Column23", "");
                                    dataGridViewOFAB.Columns.Add("Column24", "USER PC IP");
                                    dataGridViewOFAB.Columns.Add("Column25", "");
                                    dataGridViewOFAB.Columns.Add("Column26", "32： 33： 34： 35：-() 36：  37： 38： 39： 40： 999:,UniversalMsg");
                                    dataGridViewOFAB.Columns.Add("Column27", "(Y;N)");
                                    dataGridViewOFAB.Columns.Add("Column28", "EX:[][]");
                                    dataGridViewOFAB.Columns.Add("Column29", "");
                                    dataGridViewOFAB.Columns.Add("Column30", " 0:;1:; 2:");

                                    dataGridViewOFAB.Columns.Add("Column31", "EX: 202212");
                                    dataGridViewOFAB.Columns.Add("Column32", "");
                                    dataGridViewOFAB.Columns.Add("Column33", "0: ;1:");
                                    dataGridViewOFAB.Columns.Add("Column34", "N: C:CallP:Put ");
                                    dataGridViewOFAB.Columns.Add("Column35", "0:;1: ");
                                    dataGridViewOFAB.Columns.Add("Column36", "0:;1:");
                                    dataGridViewOFAB.Columns.Add("Column37", " 0");
                                    dataGridViewOFAB.Columns.Add("Column38", "");

                                    dataGridViewOFAB.Columns.Add("Column39", "EX:TF");

                                    dataGridViewOFAB.Columns.Add("Column40", "EX: TAIFEX");
                                    dataGridViewOFAB.Columns.Add("Column41", "EX:FITX");
                                    dataGridViewOFAB.Columns.Add("Column42", "");
                                    dataGridViewOFAB.Columns.Add("Column43", "");
                                    dataGridViewOFAB.Columns.Add("Column44", "");
                                    dataGridViewOFAB.Columns.Add("Column45", " Sataus999");
                                }
                                //dataGridViewOFAB ONLY
                                {
                                    dataGridViewOFAB2.Columns.Add("Column46", ""); //  46
                                }
                            }
                        }
                    }
                }
            }         
        }
        private void ReplyForm_Load(object sender, EventArgs e)
        {
            //
            {
                m_pSKOrder.OnAccount += new _ISKOrderLibEvents_OnAccountEventHandler(OnAccount);
                void OnAccount(string bstrLogInID, string bstrAccountData)
                {
                    AddUserID(m_dictUserID, bstrLogInID, bstrAccountData);

                    //key
                    if (allkeys != null) allkeys.Clear();
                    allkeys = new List<string>(m_dictUserID.Keys);

                    if (comboBoxUserID.DataSource != null) comboBoxUserID.DataSource = null;
                    comboBoxUserID.DataSource = allkeys;
                }
            }
            // ，。( )
            {
                m_pSKReply.OnNewData += new _ISKReplyLibEvents_OnNewDataEventHandler(OnNewData);
                void OnNewData(string bstrLogInID, string bstrData)
                {
                    if (isClosing != true)
                    {
                        // bstrData
                        string[] values = new string[48];

                        string KeyNo;
                        string MarketType;
                        string Type;
                        string OrderErr;
                        string Broker;
                        string CustNo;
                        string BuySell;
                        string ExchangeID;
                        string ComId;
                        string StrikePrice;

                        string OrderNo;
                        string Price;
                        string Numerator;
                        string Denominator;
                        string Price1;
                        string Numerator1;
                        string Denominator1;
                        string Price2;
                        string Numerator2;
                        string Denominator2;

                        string Qty;
                        string BeforeQty;
                        string AfterQty;
                        string Date;
                        string Time;
                        string OkSeq;
                        string SubID;
                        string SaleNo;
                        string Agent;
                        string TradeDate;

                        string MsgNo;
                        string PreOrder;
                        string ComId1;
                        string YearMonth1;
                        string StrikePrice1;
                        string ComId2;
                        string YearMonth2;
                        string StrikePrice2;
                        string ExecutionNo;
                        string PriceSymbol;

                        string Reserved;
                        string OrderEffective;
                        string CallPut;
                        string OrderSeq;
                        string ErrorMsg;
                        string CancelOrderMarkByExchange;
                        string ExchangeTandemMsg;
                        string SeqNo;
                        string OFSTPFlag;

                        //  Split 
                        values = bstrData.Split(',');
                        if (values[0] == "980") // 980()
                        {
                            dataGridViewNoClass.Rows.Add(bstrData);
                        }
                        else
                        {
                            // bstrData
                            KeyNo = values[0];
                            MarketType = values[1];
                            Type = values[2];
                            OrderErr = values[3];
                            Broker = values[4];
                            CustNo = values[5];
                            BuySell = values[6];
                            ExchangeID = values[7];
                            ComId = values[8];
                            StrikePrice = values[9];

                            OrderNo = values[10];
                            Price = values[11];
                            Numerator = values[12];
                            Denominator = values[13];
                            Price1 = values[14];
                            Numerator1 = values[15];
                            Denominator1 = values[16];
                            Price2 = values[17];
                            Numerator2 = values[18];
                            Denominator2 = values[19];

                            Qty = values[20];
                            BeforeQty = values[21];
                            AfterQty = values[22];
                            Date = values[23];
                            Time = values[24];
                            OkSeq = values[25];
                            SubID = values[26];
                            SaleNo = values[27];
                            Agent = values[28];
                            TradeDate = values[29];

                            MsgNo = values[30];
                            PreOrder = values[31];
                            ComId1 = values[32];
                            YearMonth1 = values[33];
                            StrikePrice1 = values[34];
                            ComId2 = values[35];
                            YearMonth2 = values[36];
                            StrikePrice2 = values[37];
                            ExecutionNo = values[38];
                            PriceSymbol = values[39];

                            Reserved = values[40];
                            OrderEffective = values[41];
                            CallPut = values[42];
                            OrderSeq = values[43];
                            ErrorMsg = values[44];
                            CancelOrderMarkByExchange = values[45];
                            ExchangeTandemMsg = values[46];
                            SeqNo = values[47];
                            //OFSTPFlag = values[48];
                            if (MarketType == "TS") dataGridViewTS.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Qty, BeforeQty, AfterQty, Date, Time, SubID, SaleNo, Agent, MsgNo, ExecutionNo, OrderEffective, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "TA") dataGridViewTA.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, ExecutionNo, OrderEffective, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "TL") dataGridViewTL.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, ExecutionNo, OrderEffective, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "TP") dataGridViewTP.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, ExecutionNo, OrderEffective, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "TC") dataGridViewTC.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, ExecutionNo, OrderEffective, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "TF") dataGridViewTF.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Price1, Price2, Numerator2, Denominator2, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, PreOrder, ComId1, YearMonth1, ComId2, YearMonth2, ExecutionNo, PriceSymbol, Reserved, OrderEffective, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "TO") dataGridViewTO.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, StrikePrice, OrderNo, Price, Price1, Price2, Numerator2, Denominator2, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, PreOrder, ComId1, YearMonth1, StrikePrice1, ComId2, YearMonth2, StrikePrice2, ExecutionNo, PriceSymbol, Reserved, OrderEffective, CallPut, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "OF") dataGridViewOF.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Numerator, Denominator, Price1, Numerator1, Denominator1, Price2, Numerator2, Denominator2, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, ComId1, YearMonth1, ComId2, YearMonth2, ExecutionNo, PriceSymbol, OrderEffective, OrderSeq, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "OO") dataGridViewOO.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, StrikePrice, OrderNo, Price, Numerator, Denominator, Price1, Numerator1, Denominator1, Price2, Numerator2, Denominator2, Qty, Date, Time, SubID, SaleNo, Agent, MsgNo, ComId1, YearMonth1, StrikePrice1, ComId2, YearMonth2, StrikePrice2, ExecutionNo, PriceSymbol, OrderEffective, CallPut, OrderSeq, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            if (MarketType == "OS") dataGridViewOS.Rows.Add(KeyNo, Type, OrderErr, Broker, CustNo, BuySell, ComId, OrderNo, Price, Qty, BeforeQty, AfterQty, Date, Time, SubID, SaleNo, Agent, MsgNo, ExecutionNo, OrderEffective, ErrorMsg, CancelOrderMarkByExchange, ExchangeTandemMsg, SeqNo);
                            dataGridViewNoClass.Rows.Add(values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47]);
                        }
                    }
                }
            }
            // ，
            {
                m_pSKReply.OnComplete += new _ISKReplyLibEvents_OnCompleteEventHandler(OnComplete);
                void OnComplete(string bstrUserID)
                {
                    if (isClosing != true)
                    {
                        // 
                        string msg = "【OnComplete】" + bstrUserID + "&";
                        richTextBoxMessage.AppendText(msg + "\n");
                    }
                }
            }
            // ，，
            {
                m_pSKReply.OnReplyClear += new _ISKReplyLibEvents_OnReplyClearEventHandler(OnReplyClear);
                void OnReplyClear(string bstrMarket)
                {
                    if (isClosing != true)
                    {
                        string msg = "";
                        // 
                        if (bstrMarket == "R1") msg = "";
                        else if (bstrMarket == "R2") msg = "";
                        else if (bstrMarket == "R3") msg = "";
                        else if (bstrMarket == "R4") msg = "";
                        else if (bstrMarket == "R11") msg = "";
                        else if (bstrMarket == "R20" || bstrMarket == "R21" || bstrMarket == "R22" || bstrMarket == "R23") msg = "";
                        msg = "【OnReplyClear】" + msg + "!!!";
                        richTextBoxMessage.AppendText(msg + "\n");
                    }
                }
            }
            // ，
            {
                m_pSKReply.OnReplyClearMessage += new _ISKReplyLibEvents_OnReplyClearMessageEventHandler(OnReplyClearMessage);
                void OnReplyClearMessage(string bstrUserID)
                {
                    if (isClosing != true)
                    {
                        // 
                        string msg = "【OnReplyClearMessage】" + bstrUserID + "!";
                        richTextBoxMessage.AppendText(msg + "\n");
                    }
                }
            }
            // solace，
            {
                m_pSKReply.OnSolaceReplyDisconnect += new _ISKReplyLibEvents_OnSolaceReplyDisconnectEventHandler(OnSolaceReplyDisconnect);
                void OnSolaceReplyDisconnect(string bstrUserID, int nErrorCode)
                {
                    if (isClosing != true)
                    {
                        string msg;
                        if (nErrorCode == 3002)
                        {
                            msg = "";
                        }
                        else if (nErrorCode == 3033) //SK_SUBJECT_SOLACE_SESSION_EVENT_ERROR (Solace Session down (AP，))
                        {
                            msg = "";
                            timerSolaceReconnect.Enabled = true; // timer，5
                        }
                        else
                        {
                            msg = "" + nErrorCode;
                        }
                        // 
                        msg = "【OnSolaceReplyDisconnect】" + bstrUserID + "_" + msg;
                        richTextBoxMessage.AppendText(msg + "\n");
                    }
                }
            }
            // solace，
            {
                m_pSKReply.OnSolaceReplyConnection += new _ISKReplyLibEvents_OnSolaceReplyConnectionEventHandler(OnSolaceReplyConnection);
                void OnSolaceReplyConnection(string bstrUserID, int nErrorCode)
                {
                    if (isClosing != true)
                    {
                        string msg;
                        if (nErrorCode == 0) msg = "";
                        else msg = "";
                        // 
                        msg = "【OnSolaceReplyConnection】" + bstrUserID + "_" + msg;
                        richTextBoxMessage.AppendText(msg + "\n");
                    }
                }
            }
            // ，
            {
                m_pSKReply.OnStrategyData += new _ISKReplyLibEvents_OnStrategyDataEventHandler(OnStrategyData);
                void OnStrategyData(string bstrLogInID, string bstrData)
                {
                    if (isClosing != true)
                    {
                        //  Split 
                        string[] values = bstrData.Split(',');                       
                        if (values[0] == "980") // 980()
                        {
                            dataGridViewTSMST.Rows.Add(bstrData); // -MST  0 Row
                        }
                        else
                        {
                            string TradeKind = values[5]; // 
                            if (values[0] == "TS") // 
                            {
                                if (TradeKind == "0") // None
                                {
                                    // 
                                }
                                else if (TradeKind == "9") // MST
                                {
                                    dataGridViewTSMST.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSMST2.Rows.Add(values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47]);
                                }
                                else if (TradeKind == "29") // MIOC
                                {
                                    dataGridViewTSMIOC.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSMIOC2.Rows.Add(values[34], values[35], values[36], values[37], values[38], values[39]);
                                }
                                else if (TradeKind == "8") // MIT
                                {
                                    dataGridViewTSMIT.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSMIT2.Rows.Add(values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44]);
                                }
                                else if (TradeKind == "11") // DayTrading
                                {
                                    dataGridViewTSDayTrading.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSDayTrading2.Rows.Add(values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48], values[49], values[50], values[51], values[52], values[53]);
                                }
                                else if (TradeKind == "17") // ClearOut
                                {
                                    dataGridViewTSClearOut.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSClearOut2.Rows.Add(values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48], values[49], values[50], values[51], values[52], values[53]);
                                }
                                else if (TradeKind == "3") // OCO
                                {
                                    dataGridViewTSOCO.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSOCO2.Rows.Add(values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41]);
                                }
                                else if (TradeKind == "10") // AB
                                {
                                    dataGridViewTSAB.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSAB2.Rows.Add(values[34]);
                                }
                                else if (TradeKind == "27") // CB
                                {
                                    dataGridViewTSCB.Rows.Add(values[1], values[2], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33]);
                                    dataGridViewTSCB2.Rows.Add(values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48], values[49], values[50], values[51], values[52], values[53], values[54], values[55], values[56], values[57], values[58], values[59], values[60], values[61], values[62], values[63], values[64], values[65], values[66], values[67], values[68], values[69], values[70], values[71]);
                                }
                            }
                            else if (values[0] == "TF") // 
                            {
                                TradeKind = values[5]; // 
                                if (TradeKind == "0") // None
                                {
                                    // 
                                }
                                else if (TradeKind == "5") // STP
                                {
                                    dataGridViewTFSTP.Rows.Add(values[1], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48]);
                                    dataGridViewTFSTP2.Rows.Add(values[49], values[50], values[51], values[52], values[53], values[54], values[55]);
                                }
                                else if (TradeKind == "8") // MIT
                                {
                                    dataGridViewTFMIT.Rows.Add(values[1], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48]);
                                    dataGridViewTFMIT2.Rows.Add(values[49], values[50]);
                                }
                                else if (TradeKind == "9") // MST
                                {
                                    dataGridViewTFMST.Rows.Add(values[1], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48]);
                                    dataGridViewTFMST2.Rows.Add(values[49], values[50], values[51], values[52]);
                                }
                                else if (TradeKind == "3") // OCO
                                {
                                    dataGridViewTFOCO.Rows.Add(values[1], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48]);
                                    dataGridViewTFOCO2.Rows.Add(values[49], values[50], values[51], values[52], values[53], values[54], values[55], values[56], values[57], values[58], values[59], values[60], values[61], values[62], values[63]);
                                }
                                else if (TradeKind == "10") // AB
                                {
                                    dataGridViewTFAB.Rows.Add(values[1], values[3], values[4], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46], values[47], values[48]);
                                    dataGridViewTFAB2.Rows.Add(values[49]);
                                }
                            }
                            else if (values[0] == "OF")// OF: 
                            {
                                TradeKind = values[4]; // 
                                if (TradeKind == "0") // None
                                {
                                    // 
                                }
                                else if (TradeKind == "3") // OCO
                                {
                                    dataGridViewOFOCO.Rows.Add(values[1], values[2], values[3], values[5], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46]);
                                    dataGridViewOFOCO2.Rows.Add(values[47], values[48], values[49], values[50], values[51], values[52], values[53], values[54], values[55], values[56], values[57], values[58], values[59], values[60], values[61], values[62], values[63], values[64], values[65], values[66]);
                                }
                                else if (TradeKind == "10") // AB
                                {
                                    dataGridViewOFAB.Rows.Add(values[1], values[2], values[3], values[5], values[6], values[7], values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15], values[16], values[17], values[18], values[19], values[20], values[21], values[22], values[23], values[24], values[25], values[26], values[27], values[28], values[29], values[30], values[31], values[32], values[33], values[34], values[35], values[36], values[37], values[38], values[39], values[40], values[41], values[42], values[43], values[44], values[45], values[46]);
                                    dataGridViewOFAB2.Rows.Add(values[47]);
                                }
                            }
                        }
                    }
                }
            }
            // 
            {
                int nCode = m_pSKOrder.SKOrderLib_Initialize();
                // 
                string msg = "【SKOrderLib_Initialize】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
            // 
            {
                int nCode = m_pSKOrder.GetUserAccount();
                // 
                string msg = "【GetUserAccount】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                richTextBoxMethodMessage.AppendText(msg + "\n");
            }
        }
        private void ReplyForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            isClosing = true;
            // 
            m_pSKReply.SKReplyLib_SolaceCloseByID(comboBoxUserID.Text);
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
        private void timerSolaceReconnect_Tick(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKReply.SKReplyLib_ConnectByID(comboBoxUserID.Text);
            string msg = "【...】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
            if (nCode == 0) // 
            {
                // 
                m_pSKReply.SKReplyLib_SolaceCloseByID(comboBoxUserID.Text);
                // 
                nCode = m_pSKReply.SKReplyLib_ConnectByID(comboBoxUserID.Text);
                if (nCode == 0) // 
                {
                    msg = "【SKReplyLib_ConnectByID】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
                    richTextBoxMethodMessage.AppendText(msg + "\n");

                    timerSolaceReconnect.Enabled = false; // timer
                }
            }
        }
        private void buttonSKReplyLib_IsConnectedByID_Click(object sender, EventArgs e)
        {
            string msg;
            // 
            int nCode = m_pSKReply.SKReplyLib_IsConnectedByID(comboBoxUserID.Text);

            // 
            switch (nCode)
            {
                case 0:
                    msg = "【SKReplyLib_IsConnectedByID】";
                    break;
                case 1:
                    msg = "【SKReplyLib_IsConnectedByID】";
                    break;
                case 2:
                    msg = "【SKReplyLib_IsConnectedByID】";
                    break;
                default:
                    msg = "【SKReplyLib_IsConnectedByID】";
                    break;
            }
            richTextBoxMessage.AppendText(msg + "\n");          
        }
        private void buttonSKReplyLib_SolaceCloseByID_Click(object sender, EventArgs e)
        {
            // 
            int nCode = m_pSKReply.SKReplyLib_SolaceCloseByID(comboBoxUserID.Text);

            // ->
            string msg = "【SKReplyLib_SolaceCloseByID】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
        private void buttonSKReplyLib_ConnectByID_Click(object sender, EventArgs e)
        {
            // Clear dataGridView
            {
                // 
                {
                    dataGridViewTS.Rows.Clear();
                    dataGridViewTA.Rows.Clear();
                    dataGridViewTL.Rows.Clear();
                    dataGridViewTP.Rows.Clear();
                    dataGridViewTC.Rows.Clear();
                    dataGridViewTF.Rows.Clear();
                    dataGridViewTO.Rows.Clear();
                    dataGridViewOF.Rows.Clear();
                    dataGridViewOO.Rows.Clear();
                    dataGridViewOS.Rows.Clear();
                    dataGridViewNoClass.Rows.Clear();
                }
                // 
                {
                    // 
                    {
                        dataGridViewTSMST.Rows.Clear();
                        dataGridViewTSMIOC.Rows.Clear();
                        dataGridViewTSMIT.Rows.Clear();
                        dataGridViewTSDayTrading.Rows.Clear();
                        dataGridViewTSClearOut.Rows.Clear();
                        dataGridViewTSOCO.Rows.Clear();
                        dataGridViewTSAB.Rows.Clear();
                        dataGridViewTSCB.Rows.Clear();

                        dataGridViewTSMST2.Rows.Clear();
                        dataGridViewTSMIOC2.Rows.Clear();
                        dataGridViewTSMIT2.Rows.Clear();
                        dataGridViewTSDayTrading2.Rows.Clear();
                        dataGridViewTSClearOut2.Rows.Clear();
                        dataGridViewTSOCO2.Rows.Clear();
                        dataGridViewTSAB2.Rows.Clear();
                        dataGridViewTSCB2.Rows.Clear();
                    }
                    // 
                    {
                        dataGridViewTFSTP.Rows.Clear();
                        dataGridViewTFMIT.Rows.Clear();
                        dataGridViewTFMST.Rows.Clear();
                        dataGridViewTFOCO.Rows.Clear();
                        dataGridViewTFAB.Rows.Clear();

                        dataGridViewTFSTP2.Rows.Clear();
                        dataGridViewTFMIT2.Rows.Clear();
                        dataGridViewTFMST2.Rows.Clear();
                        dataGridViewTFOCO2.Rows.Clear();
                        dataGridViewTFAB2.Rows.Clear();
                    }
                    // 
                    {
                        dataGridViewOFOCO.Rows.Clear();
                        dataGridViewOFAB.Rows.Clear();

                        dataGridViewOFOCO2.Rows.Clear();
                        dataGridViewOFAB2.Rows.Clear();
                    }
                }
            }
            // 
            int nCode = m_pSKReply.SKReplyLib_ConnectByID(comboBoxUserID.Text);

            // 
            string msg = "【SKReplyLib_ConnectByID】" + m_pSKCenter.SKCenterLib_GetReturnCodeMessage(nCode);
            richTextBoxMethodMessage.AppendText(msg + "\n");
        }
    }
}
