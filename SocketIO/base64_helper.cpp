#include "base64_helper.h"

/*
<Function Description>
base64 decode �Լ�.

<parameter>
in : base64 encoding ���ڿ�.

<return>
out : base64 decoding ���ڿ�.
*/
std::string base64_decode(const std::string &in) {

	typedef unsigned char uchar;
	static const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string out;
	std::vector<int> T(256, -1);

	for (int i = 0; i < 64; i++)
		T[b[i]] = i;

	int val = 0, valb = -8;

	for (uchar c : in) {
		if (T[c] == -1)
			break;

		val = (val << 6) + T[c];
		valb += 6;

		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

std::string base64_encode(const std::string &in) {

	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : in) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}