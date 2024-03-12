#include "GlobalConstant.h"

namespace application
{
	bool GlobalConstant::PreEncoding(json& data) const
	{
		FieldPreEncoding<boost::pfr::tuple_size_v<POD_GlobalConstant>>(pod, data["GlobalConstant"]["POD"]);

		return true;
	}

	bool GlobalConstant::PreDecoding(const json& data)
	{
		if(!data.contains("GlobalConstant"))
			return true;

		FieldPreDecoding<boost::pfr::tuple_size_v<POD_GlobalConstant>>(pod, data["GlobalConstant"]["POD"]);

		return true;
	}
}