#include "SKQuoteLib.h"
#include <deque>
#include <iostream>

std::deque<long> gDaysKlineDiff;

long CalculateDiff(const std::string &data);

CSKQuoteLib::CSKQuoteLib()
{
	m_pSKQuoteLib.CreateInstance(__uuidof(SKCOMLib::SKQuoteLib));
	m_pSKQuoteLibEventHandler = new ISKQuoteLibEventHandler(*this, m_pSKQuoteLib, &CSKQuoteLib::OnEventFiringObjectInvoke);
}

CSKQuoteLib::~CSKQuoteLib()
{
	if (m_pSKQuoteLibEventHandler)
	{
		m_pSKQuoteLibEventHandler->ShutdownConnectionPoint();
		m_pSKQuoteLibEventHandler->Release();
		m_pSKQuoteLibEventHandler = NULL;
	}

	if (m_pSKQuoteLib)
	{
		m_pSKQuoteLib->Release();
	}
}

HRESULT CSKQuoteLib::OnEventFiringObjectInvoke(
	ISKQuoteLibEventHandler *pEventHandler,
	DISPID dispidMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS *pdispparams,
	VARIANT *pvarResult,
	EXCEPINFO *pexcepinfo,
	UINT *puArgErr)
{
	DEBUG("dispidMember == %d", dispidMember);

	VARIANT varlValue;
	VariantInit(&varlValue);
	VariantClear(&varlValue);

	switch (dispidMember)
	{
	case 1: // OnConnection
	{
		long nKind = V_I4(&(pdispparams->rgvarg)[1]);
		long nCode = V_I4(&(pdispparams->rgvarg)[0]);
		OnConnection(nKind, nCode);
		break;
	}
	case 16: // RequestStockList
	{
		short sMarketNo = V_I2(&(pdispparams->rgvarg)[1]);
		_bstr_t bstrStockData = V_BSTR(&(pdispparams->rgvarg)[0]);
		OnNotifyStockList(sMarketNo, string(bstrStockData));
		break;
	}
	case 19: // OnNotifyQuoteLONG
	{
		short sMarketNo = V_I2(&(pdispparams->rgvarg)[1]);
		long nStockIndex = V_I4(&(pdispparams->rgvarg)[0]);
		OnNotifyQuoteLONG(sMarketNo, nStockIndex);
		break;
	}
	case 20: // OnNotifyHistoryTicksLONG
	{
		break;
		long nStockIndex = V_I4(&(pdispparams->rgvarg)[9]);
		long nPtr = V_I4(&(pdispparams->rgvarg)[8]);
		long nDate = V_I4(&(pdispparams->rgvarg)[7]);
		long lTimehms = V_I4(&(pdispparams->rgvarg)[6]);
		long nBid = V_I4(&(pdispparams->rgvarg)[4]);
		long nAsk = V_I4(&(pdispparams->rgvarg)[3]);
		long nClose = V_I4(&(pdispparams->rgvarg)[2]);
		long nQty = V_I4(&(pdispparams->rgvarg)[1]);
		OnNotifyHistoryTicksLONG(nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);
		break;
	}
	case 21: // OnNotifyTicksLONG
	{
		long nStockIndex = V_I4(&(pdispparams->rgvarg)[9]);
		long nPtr = V_I4(&(pdispparams->rgvarg)[8]);
		long nDate = V_I4(&(pdispparams->rgvarg)[7]);
		long lTimehms = V_I4(&(pdispparams->rgvarg)[6]);
		long nBid = V_I4(&(pdispparams->rgvarg)[4]);
		long nAsk = V_I4(&(pdispparams->rgvarg)[3]);
		long nClose = V_I4(&(pdispparams->rgvarg)[2]);
		long nQty = V_I4(&(pdispparams->rgvarg)[1]);
		OnNotifyTicksLONG(nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);
		break;
	}
	case 22: // OnNotifyBest5LONG
	{
		// from last to first
		// LONG nSimulate = V_I4(&(pdispparams->rgvarg)[0]);
		// LONG nExtendAskQty = V_I4(&(pdispparams->rgvarg)[1]);
		LONG nExtendAsk = V_I4(&(pdispparams->rgvarg)[2]);
		LONG nBestAskQty5 = V_I4(&(pdispparams->rgvarg)[3]);
		LONG nBestAsk5 = V_I4(&(pdispparams->rgvarg)[4]);
		LONG nBestAskQty4 = V_I4(&(pdispparams->rgvarg)[5]);
		LONG nBestAsk4 = V_I4(&(pdispparams->rgvarg)[6]);
		LONG nBestAskQty3 = V_I4(&(pdispparams->rgvarg)[7]);
		LONG nBestAsk3 = V_I4(&(pdispparams->rgvarg)[8]);
		LONG nBestAskQty2 = V_I4(&(pdispparams->rgvarg)[9]);
		LONG nBestAsk2 = V_I4(&(pdispparams->rgvarg)[10]);
		LONG nBestAskQty1 = V_I4(&(pdispparams->rgvarg)[11]);
		LONG nBestAsk1 = V_I4(&(pdispparams->rgvarg)[12]);
		// LONG nExtendBidQty = V_I4(&(pdispparams->rgvarg)[13]);
		// LONG nExtendBid = V_I4(&(pdispparams->rgvarg)[14]);
		LONG nBestBidQty5 = V_I4(&(pdispparams->rgvarg)[15]);
		LONG nBestBid5 = V_I4(&(pdispparams->rgvarg)[16]);
		LONG nBestBidQty4 = V_I4(&(pdispparams->rgvarg)[17]);
		LONG nBestBid4 = V_I4(&(pdispparams->rgvarg)[18]);
		LONG nBestBidQty3 = V_I4(&(pdispparams->rgvarg)[19]);
		LONG nBestBid3 = V_I4(&(pdispparams->rgvarg)[20]);
		LONG nBestBidQty2 = V_I4(&(pdispparams->rgvarg)[21]);
		LONG nBestBid2 = V_I4(&(pdispparams->rgvarg)[22]);
		LONG nBestBidQty1 = V_I4(&(pdispparams->rgvarg)[23]);
		LONG nBestBid1 = V_I4(&(pdispparams->rgvarg)[24]);
		// LONG nStockidx = V_I4(&(pdispparams->rgvarg)[25]);
		// SHORT sMarketNo = V_I2(&(pdispparams->rgvarg)[26]);

		OnNotifyBest5LONG(nBestBid1,
						  nBestBidQty1,
						  nBestBid2,
						  nBestBidQty2,
						  nBestBid3,
						  nBestBidQty3,
						  nBestBid4,
						  nBestBidQty4,
						  nBestBid5,
						  nBestBidQty5,
						  nBestAsk1,
						  nBestAskQty1,
						  nBestAsk2,
						  nBestAskQty2,
						  nBestAsk3,
						  nBestAskQty3,
						  nBestAsk4,
						  nBestAskQty4,
						  nBestAsk5,
						  nBestAskQty5);

		break;
	}

	case 6:
	{
		BSTR bstrStockNo = pdispparams->rgvarg[1].bstrVal;
		BSTR bstrData = pdispparams->rgvarg[0].bstrVal;

		OnNotifyKLineData(bstrStockNo, bstrData);

		break;
	}
	case 7:
	{
		break;
	}
	}

	return S_OK;
}

// Methods
long CSKQuoteLib::EnterMonitorLONG()
{
	return m_pSKQuoteLib->SKQuoteLib_EnterMonitorLONG();
}

long CSKQuoteLib::IsConnected()
{
	return m_pSKQuoteLib->SKQuoteLib_IsConnected();
}

long CSKQuoteLib::LeaveMonitor()
{
	return m_pSKQuoteLib->SKQuoteLib_LeaveMonitor();
}

long CSKQuoteLib::RequestStocks(short *psPageNo, string strStockNos)
{
	return m_pSKQuoteLib->SKQuoteLib_RequestStocks(psPageNo, _bstr_t(strStockNos.c_str()));
}

long CSKQuoteLib::GetStockByIndexLONG(short sMarketNo, long nStockIndex, SKCOMLib::SKSTOCKLONG *pSKStock)
{
	return m_pSKQuoteLib->SKQuoteLib_GetStockByIndexLONG(sMarketNo, nStockIndex, pSKStock);
}

long CSKQuoteLib::RequestTicks(short *psPageNo, string strStockNos)
{
	// SKQuoteLib_RequestLiveTick
	return m_pSKQuoteLib->SKQuoteLib_RequestTicks(psPageNo, _bstr_t(strStockNos.c_str()));
}

long CSKQuoteLib::RequestStockList(short MarketNo)
{
	return m_pSKQuoteLib->SKQuoteLib_RequestStockList(MarketNo);
}

long CSKQuoteLib::RequestKLine(string strStockNo)
{
	DEBUG("start");
	BSTR BstrStockNo = _bstr_t(strStockNo.c_str());

	// string StartDateStr = "20240716";
	// BSTR StartDate = _bstr_t(StartDateStr.c_str());

	// string EndDateStr = "20240719";
	// BSTR EndDate = _bstr_t(EndDateStr.c_str());

	long res = 0;

	// res = m_pSKQuoteLib->SKQuoteLib_RequestKLineAMByDate(BstrStockNo, 4, 1, 1, StartDate, EndDate, 0);

	// DEBUG("m_pSKQuoteLib->SKQuoteLib_RequestKLineAMByDate = %d", res);

	// if (res != 0)
	// {
	res = m_pSKQuoteLib->SKQuoteLib_RequestKLine(BstrStockNo, 4, 1);
	DEBUG("m_pSKQuoteLib->SKQuoteLib_RequestKLine = %d", res);
	//}

	// WaitForSingleObject(hEvent, INFINITE);
	// DEBUG("Event received, proceeding with next step");

	return res;
}

// Events
void CSKQuoteLib::OnConnection(long nKind, long nCode)
{
	switch (nKind)
	{
	case 3001: // Connected
	{
		cout << endl
			 << "OnConnection" << endl;
		break;
	}
	case 3002: // Disconnected
	{
		cout << endl
			 << "OnConnection Disconnected" << endl;
		break;
	}
	case 3003: //
	{
		cout << endl
			 << "OnConnection 3003" << endl;
		break;
	}
	}
}

void CSKQuoteLib::OnNotifyQuoteLONG(short sMarketNo, long nStockIndex)
{
	DEBUG("Start");

	SKCOMLib::SKSTOCKLONG skStock;
	long nResult = GetStockByIndexLONG(sMarketNo, nStockIndex, &skStock);
	if (nResult != 0)
		return;

	char *szStockNo = _com_util::ConvertBSTRToString(skStock.bstrStockNo);
	char *szStockName = _com_util::ConvertBSTRToString(skStock.bstrStockName);

	DEBUG("szStockNo: %s, szStockName : %s, bid: %d, ask: %d, last: %d, volume: %d",
		  szStockNo,
		  szStockName,
		  skStock.nBid,
		  skStock.nAsk,
		  skStock.nClose,
		  skStock.nTQty);

	printf("OnNotifyQuoteLONG : %s %s bid:%d ask:%d last:%d volume:%d\n",
		   szStockNo,
		   szStockName,
		   skStock.nBid,
		   skStock.nAsk,
		   skStock.nClose,
		   skStock.nTQty);

	delete[] szStockName;
	delete[] szStockNo;
}

void CSKQuoteLib::OnNotifyTicksLONG(long nStockIndex, long nPtr, long nDate, long lTimehms, long nBid, long nAsk, long nClose, long nQty)
{
	printf("OnNotifyTicksLONG : %ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);

	DEBUG("nStockIndex: %ld, nPtr: %ld,nDate: %ld,lTimehms: %ld,nBid: %ld,nAsk: %ld,nClose: %ld,nQty: %ld\n", nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);

	// CalculateLongOrShort();
}

void CSKQuoteLib::OnNotifyHistoryTicksLONG(long nStockIndex, long nPtr, long nDate, long lTimehms, long nBid, long nAsk, long nClose, long nQty)
{
	printf("OnNotifyHistoryTicksLONG : %ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);
	DEBUG("nStockIndex: %ld, nPtr: %ld,nDate: %ld,lTimehms: %ld,nBid: %ld,nAsk: %ld,nClose: %ld,nQty: %ld\n", nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);
}

void CSKQuoteLib::OnNotifyBest5LONG(
	long nBestBid1, long nBestBidQty1,
	long nBestBid2, long nBestBidQty2,
	long nBestBid3, long nBestBidQty3,
	long nBestBid4, long nBestBidQty4,
	long nBestBid5, long nBestBidQty5,
	long nBestAsk1, long nBestAskQty1,
	long nBestAsk2, long nBestAskQty2,
	long nBestAsk3, long nBestAskQty3,
	long nBestAsk4, long nBestAskQty4,
	long nBestAsk5, long nBestAskQty5)
{
	DEBUG("start");

	printf("OnNotifyBest5LONG\n");

	printf("Ofr1: [%ld],q1G: [%ld]\n", nBestAsk1, nBestAskQty1);
	printf("Ofr2: [%ld],q2G: [%ld]\n", nBestAsk2, nBestAskQty2);
	printf("Ofr3: [%ld],q3G: [%ld]\n", nBestAsk3, nBestAskQty3);
	printf("Ofr4: [%ld],q4G: [%ld]\n", nBestAsk4, nBestAskQty4);
	printf("Ofr5: [%ld],q5G: [%ld]\n\n", nBestAsk5, nBestAskQty5);

	printf("Bid1: [%ld], q1G: [%ld]\n", nBestBid1, nBestBidQty1);
	printf("Bid2: [%ld], q2G: [%ld]\n", nBestBid2, nBestBidQty2);
	printf("Bid3: [%ld], q3G: [%ld]\n", nBestBid3, nBestBidQty3);
	printf("Bid4: [%ld], q4G: [%ld]\n", nBestBid4, nBestBidQty4);
	printf("Bid5: [%ld], q5G: [%ld]\n\n", nBestBid5, nBestBidQty5);

	printf("=================================\n\n");

	DEBUG("Ofr1: [%ld], q1G: [%ld]\n", nBestAsk1, nBestAskQty1);
	DEBUG("Ofr2: [%ld], q2G: [%ld]\n", nBestAsk2, nBestAskQty2);
	DEBUG("Ofr3: [%ld], q3G: [%ld]\n", nBestAsk3, nBestAskQty3);
	DEBUG("Ofr4: [%ld], q4G: [%ld]\n", nBestAsk4, nBestAskQty4);
	DEBUG("Ofr5: [%ld], q5G: [%ld]\n\n", nBestAsk5, nBestAskQty5);

	DEBUG("Bid1: [%ld], q1G: [%ld]\n", nBestBid1, nBestBidQty1);
	DEBUG("Bid2: [%ld], q2G: [%ld]\n", nBestBid2, nBestBidQty2);
	DEBUG("Bid3: [%ld], q3G: [%ld]\n", nBestBid3, nBestBidQty3);
	DEBUG("Bid4: [%ld], q4G: [%ld]\n", nBestBid4, nBestBidQty4);
	DEBUG("Bid5: [%ld], q5G: [%ld]\n\n", nBestBid5, nBestBidQty5);

	// CalculateLongOrShort();

	DEBUG("end");
}

void CSKQuoteLib::OnNotifyStockList(long sMarketNo, string strStockData)
{
	string tempstr = "";
	for (int i = 0; i < strStockData.length(); i++)
	{
		if (strStockData[i] == ';')
		{
			cout << tempstr << endl;
			if (tempstr == "##,,")
			{
				break;
			}
			tempstr = "";
			continue;
		}
		tempstr += strStockData[i];
	}

	cout << endl;
}

void CSKQuoteLib::OnNotifyKLineData(BSTR bstrStockNo, BSTR bstrData)
{
	DEBUG("start");

	string strStockNo = string(_bstr_t(bstrStockNo));

	cout << "OnNotifyKLineData : " << endl;
	cout << "strStockNo : " << strStockNo << endl;

	DEBUG("strStockNo= %s", strStockNo);

	string strData = string(_bstr_t(bstrData));

	cout << "strData : " << strData;

	DEBUG("strData= %s", strData);

	cout << endl;

	long diff = CalculateDiff(strData);

	gDaysKlineDiff.push_back(diff);

	if (gDaysKlineDiff.size() > DayMA)
	{
		gDaysKlineDiff.pop_front();
	}

	// CalculateDailyWavesAndKeyPrices()

	// SetEvent(hEvent);

	// CalculateLongOrShort();

	DEBUG("end");
}

long CalculateDiff(const std::string &data)
{
	std::istringstream stream(data);
	std::string token;
	std::string highStr, lowStr;

	// Skip to the third and fourth values (22156.00 and 21918.00)
	for (int i = 0; i < 4; ++i)
	{
		std::getline(stream, token, ',');
		if (i == 2)
		{
			highStr = token;
		}
		else if (i == 3)
		{
			lowStr = token;
		}
	}

	// Convert the strings to doubles
	double high = std::stod(highStr);
	double low = std::stod(lowStr);

	// Calculate and return the absolute difference
	return std::lround(std::abs(high - low));
}