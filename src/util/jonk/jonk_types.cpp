#include "jonk_types.h"

namespace jonk {

	std::ostream& operator <<(std::ostream& os, const JonkType& jonkType) {
		switch (jonkType) {
		case JonkType::Null:
			os << "JonkType::Null";
			break;
		case JonkType::Object:
			os << "JonkType::Object";
			break;
		case JonkType::Array:
			os << "JonkType::Array";
			break;
		case JonkType::String:
			os << "JonkType::String";
			break;
		case JonkType::Float:
			os << "JonkType::Float";
			break;
		case JonkType::Int:
			os << "JonkType::Int";
			break;
		case JonkType::Uint:
			os << "JonkType::Uint";
			break;
		case JonkType::Bool:
			os << "JonkType::Bool";
			break;
		default:
			os << "JonkType::";
			break;
		}
		return os;
	}

}