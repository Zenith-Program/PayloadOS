#include "PayloadOSToken.h"

using namespace PayloadOS;
using namespace PayloadOS::Interpreter;

//implementation of static TokenBuffer interface---------------
SerialIO* Token::buffer = nullptr;
void Token::setTokenBuffer(SerialIO* buffer) {
	Token::buffer = buffer;
}

//implementation of command interface--------------------------
error_t Token::copyStringData(char* commandBuffer, uint_t size) const {
	uint_t start = data.strData.start, end = data.strData.end;
	if (end - start >= size) {
		if(size > 0) commandBuffer[0] = '\0';
		return ERROR;
	}
	buffer->copy(commandBuffer, start, end);
	commandBuffer[end - start] = '\0';
	return GOOD;
}

float_t Token::getFloatData() const {
	return data.floatData;
}

uint_t Token::getUnsignedData() const {
	return data.unsignedData;
}

int_t Token::getSignedData() const {
	return data.signedData;
}

//implementation of console interface--------------------------

error_t Token::setInterpretation(TokenTypes type) {
	if (static_cast<uint_t>(type) & interpretations && type != TokenTypes::Invalid) {
		switch (type) {
		case TokenTypes::unsignedData:
			interpretUnsignedData();
			break;
		case TokenTypes::signedData:
			interpretSignedData();
			break;
		case TokenTypes::FloatData:
			interpretFloatData();
			break;
		case TokenTypes::StringData:
			interpretStringData();
			break;
		case TokenTypes::WordData:
			interpretWordData();
			break;
        default:
            break;
		}
		return GOOD;
	}
	else
		return ERROR;
}

bool Token::extract(int_t& pos) {
	//clear white space
	for (; isWhiteSpace(buffer->at(pos)); pos--);
	//save end of token
	int_t end = pos + 1;
	//check for newline (\n) or command start (>)
	if (isStopCharacter(buffer->at(pos))) return true;
	int_t start;
	interpretations = static_cast<uint_t>(TokenTypes::Empty);
	if (buffer->at(pos) == '"') { //string token
		end = pos;
		pos--;
		for (; buffer->at(pos) != '"'; pos--);
		if (end == (pos % buffer->size() + buffer->size()) % buffer->size()) return true;
		start = pos + 1;
		pos--;
		setInterpretationRange(start, end);
		checkStringData();
	}
	else { //non-string token
		//find begining of token
		for (; !isEndCharacter(buffer->at(pos)); pos--);
		start = pos + 1;
		setInterpretationRange(start, end);
		checkUnsignedData();
		checkSignedData();
		checkFloatData();
		checkWordData();
	}
	return false;
}

//private implementation functions-----------------------------

void Token::setInterpretationRange(uint_t start, uint_t end) {
	data.strData.start = start;
	data.strData.end = end;
}

//interpretations for each type----------------

/* unsigned formats
* Decimal: 12345
* Hex: 0x43F2Ea02
* Binary 0b01001011
*/
void Token::interpretUnsignedData() {
	interpretations = static_cast<uint_t>(TokenTypes::Empty);
	int_t start = data.strData.start;
	int_t end = data.strData.end;
	uint_t value = 0;
	//Decimal Case
	int_t i;
	for (i = start; i < end && isNumeric(buffer->at(i)); i++)
		value += powerOfTen_u(end - start - 1 - (i - start)) * numberValue_u(buffer->at(i));
	if (i == end) {
		data.unsignedData = value;
		interpretations |= static_cast<uint_t>(TokenTypes::unsignedData);
		return;
	}
	//Hex case
	value = 0;
	if (end - start > 1 && buffer->at(start) == '0' && buffer->at(start + 1) == 'x') {
		for (i = start + 2; i < end && isHex(buffer->at(i)); i++)
			value += numberValue_u(buffer->at(i)) << ((end - (start + 2) - 1 - (i - (start + 2))) << 2);
		if (i == end) {
			data.unsignedData = value;
			interpretations |= static_cast<uint_t>(TokenTypes::unsignedData);
			return;
		}
	}
	//Binary
	value = 0;
	if (end - start > 1 && buffer->at(start) == '0' && buffer->at(start + 1) == 'b') {
		for (i = start + 2; i < end && isBinary(buffer->at(i)); i++)
			value += numberValue_u(buffer->at(i)) << (end - (start + 2) - 1 - (i - (start + 2)));
		if (i == end) {
			data.unsignedData = value;
			interpretations |= static_cast<uint_t>(TokenTypes::unsignedData);
			return;
		}
	}
}
void Token::interpretSignedData() {
	interpretations = static_cast<uint_t>(TokenTypes::Empty);
	int_t start = data.strData.start;
	int_t end = data.strData.end;
	int_t i;
	int_t value = 0;
	//negative case
	if (buffer->at(start) == '-') {
		for (i = start + 1; i < end && isNumeric(buffer->at(i)); i++) {
			value += powerOfTen_s(end - start - 2 - (i - start - 1)) * numberValue_s(buffer->at(i));
		}
		if (i == end) {
			data.signedData = -value;
			interpretations |= static_cast<int_t>(TokenTypes::signedData);
			return;
		}
	}
	else {
		//positive case
		value = 0;
		for (i = start; i < end && isNumeric(buffer->at(i)); i++)
			value += powerOfTen_s(end - start - 1 - (i - start)) * numberValue_s(buffer->at(i));
		if (i == end) {
			data.signedData = value;
			interpretations |= static_cast<int_t>(TokenTypes::signedData);
			return;
		}
	}
}
void Token::interpretWordData() {
	interpretations = static_cast<uint_t>(TokenTypes::WordData);
}
void Token::interpretStringData() {
	interpretations = static_cast<uint_t>(TokenTypes::StringData);
}
void Token::interpretFloatData() {
	interpretations = static_cast<uint_t>(TokenTypes::Empty);
	int_t start = data.strData.start;
	int_t end = data.strData.end;
	float_t value = 0;
	bool neg = false;
	if (buffer->at(start) == '-') {
		neg = true;
		start++;
	}
	int_t i;
	for (i = start; i < end && isNumeric(buffer->at(i)); i++);
	if (i == end) {//no decimal point case
		for (i = start; i < end; i++)
			value += powerOfTen_f(end - start - 1 - (i - start)) * numberValue_s(buffer->at(i));
		if (neg) value = -value;
		data.floatData = value;
		interpretations |= static_cast<uint_t>(TokenTypes::FloatData);
		return;
	}
	else if (buffer->at(i) == '.') {//decimal point case
		uint_t decimalPoint = i;
		//before decimal point
		for (uint_t j = start; j < decimalPoint; j++)
			value += powerOfTen_f(decimalPoint - start - 1 - (j - start)) * numberValue_s(buffer->at(j));
		//after decimal point
		for (i++; i < end && isNumeric(buffer->at(i)); i++)
			value += powerOfTen_f(decimalPoint - i) * numberValue_s(buffer->at(i));
		if (i == end) {
			if (neg) value = -value;
			data.floatData = value;
			interpretations |= static_cast<uint_t>(TokenTypes::FloatData);
			return;
		}
	}
}

//checks for each type-------------------------
void Token::checkUnsignedData() {
	int_t start = data.strData.start;
	int_t end = data.strData.end;
	//decimal case
	int_t i;
	for (i = start; i < end && isNumeric(buffer->at(i)); i++);
	if (end == i) {
		interpretations |= static_cast<uint_t>(TokenTypes::unsignedData);
		return;
	}
	//hex case
	if (end - start > 1 && buffer->at(start) == '0' && buffer->at(start + 1) == 'x') {
		for (i = start + 2; i < end && isHex(buffer->at(i)); i++);
		if (i == end) {
			interpretations |= static_cast<uint_t>(TokenTypes::unsignedData);
			return;
		}
	}
	//binary case
	if (end - start > 1 && buffer->at(start) == '0' && buffer->at(start + 1) == 'b') {
		for (i = start + 2; i < end && isBinary(buffer->at(i)); i++);
		if (i == end) {
			interpretations |= static_cast<uint_t>(TokenTypes::unsignedData);
			return;
		}
	}
}
void Token::checkSignedData() {
	int_t start = data.strData.start;
	int_t end = data.strData.end;
	int_t i;
	//negative case
	if (buffer->at(start) == '-') {
		for (i = start + 1; i < end && isNumeric(buffer->at(i)); i++);
		if (i == end) {
			interpretations |= static_cast<int_t>(TokenTypes::signedData);
			return;
		}
	}
	else {
		//positive case
		for (i = start; i < end && isNumeric(buffer->at(i)); i++);
		if (i == end) {
			interpretations |= static_cast<int_t>(TokenTypes::signedData);
			return;
		}
	}
}
void Token::checkWordData() {
	interpretations |= static_cast<uint_t>(TokenTypes::WordData);
}
void Token::checkStringData() {
	interpretations |= static_cast<uint_t>(TokenTypes::StringData);
}
void Token::checkFloatData() {
	int_t start = data.strData.start;
	int_t end = data.strData.end;
	if (buffer->at(start) == '-') {
		start++;
	}
	int_t i;
	for (i = start; i < end && isNumeric(buffer->at(i)); i++);
	if (i == end) {
		interpretations |= static_cast<uint_t>(TokenTypes::FloatData);
		return;
	}
	else if (buffer->at(i) == '.') {
		for (i++; i < end && isNumeric(buffer->at(i)); i++);
		if (i == end) {
			interpretations |= static_cast<uint_t>(TokenTypes::FloatData);
			return;
		}
	}
}

//statics--------------------------------------
bool Token::isEndCharacter(char c) {
	if (isWhiteSpace(c) || c == '>' || c == '\n' || c == '"') return true;
	return false;
}

bool Token::isStopCharacter(char c) {
	if (c == '\n' || c == '>') return true;
	return false;
}

bool Token::isWhiteSpace(char c) {
	if (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f') return true;
	return false;
}

bool Token::isNumeric(char c) {
	if (c >= '0' && c <= '9') return true;
	return false;
}

bool Token::isHex(char c) {
	if (isNumeric(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) return true;
	return false;
}

bool Token::isBinary(char c) {
	if (c == '1' || c == '0') return true;
	return false;
}

uint_t Token::powerOfTen_u(uint_t n) {
	uint_t value = 1;
	for (uint_t i = 0; i < n; i++)
		value *= 10;
	return value;
}

int_t Token::powerOfTen_s(uint_t n) {
	int_t value = 1;
	for (uint_t i = 0; i < n; i++)
		value *= 10;
	return value;
}
float_t Token::powerOfTen_f(int_t n) {
	float_t value = 1;
	if (n < 0) {
		for (int_t i = 0; i < (-1 * n); i++)
			value *= 1.0 / 10;
	}
	else {
		for (int_t i = 0; i < n; i++)
			value *= 10;
	}
	return value;
}

uint_t Token::numberValue_u(char c) {
	uint_t value;
	if (c >= '0' && c <= '9')
		value = c - '0';
	else if (c >= 'a' && c <= 'f')
		value = c - 'a' + 10;
	else
		value = c - 'A' + 10;
	return value;
}

int_t Token::numberValue_s(char c) {
	return c - '0';
}

