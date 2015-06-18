#ifndef MACROSS_STRATEGY_H
#define MACROSS_STRATEGY_H

#include "Strategy.h"
#include "TechVec.h"

class MACrossStratgyTechVec;
class MACrossStratgy : public Strategy
{
public:
	MACrossStratgy(size_t short_ma, size_t long_ma);
	virtual ~MACrossStratgy();

	virtual bool tryInvoke(const std::list<CThostFtdcDepthMDFieldWrapper>& data, CThostFtdcDepthMDFieldWrapper& info);
	virtual Order generateOrder();

protected:
	virtual double calculateK(const std::list<CThostFtdcDepthMDFieldWrapper>& data, const CThostFtdcDepthMDFieldWrapper& current, int seconds) const;
	virtual MACrossStratgyTechVec* generateTechVec(const CThostFtdcDepthMDFieldWrapper& info) const;

protected:
	size_t m_shortMA;
	size_t m_longMA;

private:
	Order* m_curOrder;

};

typedef StratgyType::MACrossStratgyType CrossStratgyType;

class MACrossStratgyTechVec : public StrategyTechVec{
public:
	MACrossStratgyTechVec(CrossStratgyType type, size_t shortMA, size_t longMA, long long uuid, const std::string& instrumentID, const std::string& time = "", double lastprice = 0);
	virtual ~MACrossStratgyTechVec(){}

	virtual size_t ObjSize(){
		return sizeof(*this);
	}

	inline void setShortMA(double input){
		m_shortMAVal = input;
	}

	inline double ShortMA() const {
		return m_shortMAVal;
	}

	inline void setLongMA(double input){
		m_longMAVal = input;
	}

	inline double LongMA() const {
		return m_longMAVal;
	}

	virtual void SetTickType(TickType type){
		m_ticktype = type;
	}

	virtual TickType GetTickType(){
		return m_ticktype;
	}

	virtual bool IsUpThough() const;

	virtual void serializeToDB(DBWrapper& db, const std::string& mark);

protected:
	virtual int CreateTableIfNotExists(const std::string& dbname, const std::string& tableName);
	static bool IsTableCreated;

private:
	double m_longMAVal; //average line for 5 minutes
	double m_shortMAVal; //average line for 3 minutes
	TickType m_ticktype;
	char m_instrumentId[32];
	char m_time[64];// for check result 

	double m_lastprice; // for check result
	long long m_id; //time_stamp * 2  (unit : 500ms)
	StratgyType::MACrossStratgyType m_type;
	size_t m_shortMA;
	size_t m_longMA;
};

#endif