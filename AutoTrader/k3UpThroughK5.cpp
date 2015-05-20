#include "stdafx.h"
#include "config.h"
#include "DBWrapper.h"
#include "TechVec.h"
#include "k3UpThroughK5.h"
#include "Order.h"

#include <sstream>


k3UpThroughK5::k3UpThroughK5()
:m_curOrder(new Order())
{
}

k3UpThroughK5::~k3UpThroughK5()
{
}

double k3UpThroughK5::calculateK(const std::list<CThostFtdcDepthMDFieldWrapper>& data, const CThostFtdcDepthMDFieldWrapper& current, int seconds) const
{
	//datetime to timestamp
	double totalExchangePrice = current.TurnOver();
	long long totalVolume = current.Volume();

	long long leftedge = current.toTimeStamp() - seconds * 2;
	for (auto it = data.begin(); it != data.end(); it++)
	{
		if (it->toTimeStamp() > leftedge){
			totalExchangePrice += it->TurnOver();
			totalVolume += it->Volume();
		}
		else{
			break;
		}
	}

	//assert(totalVolume != 0);
	//assert(totalExchangePrice >= 0.0);

	return totalExchangePrice / totalVolume;
}

bool k3UpThroughK5::TryInvoke(const std::list<CThostFtdcDepthMDFieldWrapper>& data, CThostFtdcDepthMDFieldWrapper& info)
{
	k3UpThroughK5TechVec* curPtr = new k3UpThroughK5TechVec(info.UUID(), info.InstrumentId());
	bool orderSingal = false;
	double k3 = calculateK(data, info, 3 * 60);
	double k5 = calculateK(data, info, 5 * 60);
	curPtr->setK3m(k3);
	curPtr->setK5m(k5);

	//assert(!data.empty());
	if (!data.empty())
	{
		auto preNode = data.begin();

		k3UpThroughK5TechVec* prePtr = dynamic_cast<k3UpThroughK5TechVec*>(preNode->GetTechVec());
		if (prePtr){
			if (prePtr->K5m() > prePtr->K3m())
			{
				if (curPtr->K3m() > curPtr->K5m()){
					// Buy Singal
					// construct Buy Order ptr
					//std::cout << "[Buy Signal]" << std::endl;
					//std::cout << "LastPrice: " << info.LastPrice() << std::endl;
					//Order ord(info.InstrumentId(), info.LastPrice(), ExchangeDirection::Buy);
					m_curOrder->SetInstrumentId(info.InstrumentId());
					m_curOrder->SetRefExchangePrice(info.LastPrice());
					m_curOrder->SetExchangeDirection(ExchangeDirection::Buy);
					curPtr->SetTickType(TickType::BuyPoint);
					orderSingal = true;
				}
			}
			else{
				if (curPtr->K3m() < curPtr->K5m()){
					//Sell Singal
					// construct Sell Order ptr
					//std::cout << "[Sell Signal]" << std::endl;
					//std::cout << "LastPrice: " << info.LastPrice() << std::endl;
					//Order ord(info.InstrumentId(), info.LastPrice(), ExchangeDirection::Sell);
					m_curOrder->SetInstrumentId(info.InstrumentId());
					m_curOrder->SetRefExchangePrice(info.LastPrice());
					m_curOrder->SetExchangeDirection(ExchangeDirection::Sell);
					curPtr->SetTickType(TickType::SellPoint);
					orderSingal = true;
				}
			}
		}

	}

	info.SetTechVec((StrategyTechVec*)curPtr);
	return orderSingal;
}

Order k3UpThroughK5::generateOrder(){
	return *m_curOrder;
}


k3UpThroughK5TechVec::k3UpThroughK5TechVec(long long uuid, const std::string& instrumentID)
: StrategyTechVec(uuid, instrumentID)
, m_ticktype(TickType::Commom)
{
}

void k3UpThroughK5TechVec::serializeToDB(DBWrapper& db)
{
	std::string&& tableName = m_instrumentId + "_k3UpThroughK5";

	DBUtils::CreateK3K5StrategyTableIfNotExists(Config::Instance()->DBName(), tableName);

	std::stringstream sql;
	sql << "INSERT INTO `" << tableName << "` (`";
	sql << "uuid" << "`,`";
	sql << "k5m" << "`,`";
	sql << "k3m" << "`,`";
	sql << "Ticktype" << "`";
	sql << ") VALUES(";
	sql << m_id << ", ";
	sql << m_k5m << ", ";
	sql << m_k3m << ", ";
	sql << (int)m_ticktype << ")";

	//std::cerr << sql.str() << std::endl;
	db.ExecuteNoResult(sql.str());
}