#include "JsonHelper.h"

//#include <easylogging++.h>

using namespace std;
using json = nlohmann::json;

bool JsonHelper::GetBoolField(const json& obj, const char* fieldName, bool& fieldValue)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj))
		return false;

	if (!fieldObj.is_boolean())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Boolean in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<bool>();

	return true;
}

bool JsonHelper::GetFloatField(const json& obj, const char* fieldName, float& fieldValue)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj))
		return false;

	if (!fieldObj.is_number())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Number in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<float>();

	return true;
}

bool JsonHelper::GetDoubleField(const json& obj, const char* fieldName, double& fieldValue)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj))
		return false;

	if (!fieldObj.is_number())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Number in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<double>();

	return true;
}

bool JsonHelper::GetInt64Field(const json& obj, const char* fieldName, int64& fieldValue)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj))
		return false;

	if (!fieldObj.is_number_integer())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Integer64 in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<int64>();

	return true;
}

bool JsonHelper::GetLongField(const json& obj, const char* fieldName, long& fieldValue)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj))
		return false;

	if (!fieldObj.is_number_integer())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Long in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<long>();

	return true;
}

bool JsonHelper::GetUintField(const json& obj, const char* fieldName, uint32& fieldValue, bool must)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj, must))
		return false;

	if (!fieldObj.is_number_unsigned())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Integer in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<size_t>();

	return true;
}

bool JsonHelper::GetIntField(const json& obj, const char* fieldName, int32& fieldValue, bool must)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj, must))
		return false;

	if (!fieldObj.is_number_integer())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Integer in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<int32>();

	return true;
}

bool JsonHelper::GetInt64ArrayField(const json& obj, const char* fieldName, list<int64>& fieldValue)
{
	json fieldObj;
	if (!GetArrayField(obj, fieldName, fieldObj))
		return false;

	fieldValue = fieldObj.get<list<int64>>();

	return true;
}

bool JsonHelper::GetUintArrayField(const json& obj, const char* fieldName, list<uint32>& fieldValue)
{
	json fieldObj;
	if (!GetArrayField(obj, fieldName, fieldObj))
		return false;

	fieldValue = fieldObj.get<list<uint32>>();

	return true;
}

bool JsonHelper::GetIntArrayField(const json& obj, const char* fieldName, list<int32>& fieldValue)
{
	json fieldObj;
	if (!GetArrayField(obj, fieldName, fieldObj))
		return false;

	fieldValue = fieldObj.get<list<int32>>();

	return true;
}

bool JsonHelper::GetStringField(const json& obj, const char* fieldName, string& fieldValue)
{
	json fieldObj;
	if (!GetField(obj, fieldName, fieldObj))
		return false;

	if (!fieldObj.is_string())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't String in " << obj.dump();
		return false;
	}

	fieldValue = fieldObj.get<string>();

	return true;
}

bool JsonHelper::GetStringArrayField(const json& obj, const char* fieldName, list<string>& fieldValue)
{
	json fieldObj;
	if (!GetArrayField(obj, fieldName, fieldObj))
		return false;

	fieldValue = fieldObj.get<list<string>>();

	return true;
}

bool JsonHelper::GetArrayField(const json& obj, const char* fieldName, json& fieldObj, bool must)
{
	if (!GetField(obj, fieldName, fieldObj, must))
		return false;

	if (!fieldObj.is_array())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Array in " << obj.dump();
		return false;
	}

	return true;
}

bool JsonHelper::GetObjectField(const json& obj, const char* fieldName, json& fieldObj)
{
	if (!GetField(obj, fieldName, fieldObj))
		return false;

	if (!fieldObj.is_object())
	{
		//LOG(ERROR) << "[" << fieldName << "] isn't Object in " << obj.dump();
		return false;
	}

	return true;
}

bool JsonHelper::GetField(const json& obj, const char* fieldName, json& fieldObj, bool must)
{
	if (obj.find(fieldName) == obj.end())
	{
		if(must)
			//LOG(ERROR) << "can't find [" << fieldName << "] in " << obj.dump();
		return false;
	}

	fieldObj = obj[fieldName];

	return true;
}
