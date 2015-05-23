#pragma once

#include <time.h>
#include <string>


typedef int seconds;

class CommonUtils
{
public:
	CommonUtils();
	~CommonUtils();
	static seconds TimeToSenconds(const char* time_src);
	static long long DateTimeToTimestamp(const char* date_src, const char* time_src);

	// |bopen = false & eopen = true| -> [begin, end)
	// input_time belongTo [begin, end)
	static bool TimeInRange(const char* begin, bool bopen, const char* end, bool eopen,  const char* input);
	static bool IsMarketingTime(const char * time);

	static int StringtoInt(const std::string& str);
	static double StringtoDouble(const std::string& str);
	static long long Stringtolong(const std::string& str);
	static std::string ConvertTime(const std::string& src);
};

