#include "uCCButil.h"

std::string NumToHexString(uint32 val, uint8 minLength)
{
	const char data[] = "0123456789ABCDEF";
	std::string hex = "";
	for (uint8 i = 8; i != 0; i--)
	{
		int16 roll = (i - 1) << 2;
		int32 index = (val & (0xF << roll)) >> roll;
		if (!hex.empty() || index || i == 1)
			hex += data[index];
	}
	if (hex.length() < minLength)
	{
		hex = std::string(minLength - hex.length(), '0') + hex;
	}

	return hex;
}

uint32 HexStringToNum(std::string hex)
{
	/* removing (0x000...) prefix*/
	uint32 val = 0;
	if (hex.substr(0, 2) == "0x" || hex.substr(0, 2) == "0X")
		hex = hex.substr(2);

	int32 i = 0;
	while (hex[i] == '0')
		i++;

	hex = hex.substr(i);
	if (hex.length() > 8)
		return 0;

	uint32 j;
	for (i = hex.length() - 1, j = 0; i >= 0; i--, j += 4)
	{
		if (hex[i] >= '0' && hex[i] <= '9')
		{
		}
		else if (hex[i] >= 'a' && hex[i] <= 'f')
		{
			hex[i] -= ('a' - '9' - 1);
		}
		else if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			hex[i] -= ('A' - '9' - 1);
		}
		else
			return 0;

		val += (hex[i] - '0') << j;
	}
	return val;
}
