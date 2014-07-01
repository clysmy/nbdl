#include "Address.h"

void Address::validate(ErrorBinder &e)
{
	validateString(e, line1)
		.required()
		.maxLen(50);
	validateString(e, line2)
		.optional()
		.maxLen(50);
	validateString(e, zipCode)
		.required()
		.matches("^[0-9]{5}$");
	validateString(e, city)
		.required();
	validateString(e, state)
		.required()
		.matches("^[A-Z]{2}$");
}
