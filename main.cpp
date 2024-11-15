#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <deque>
#include <cstdint>

#include <iostream>

// i had to make an infinite number class because the pascal
// intermediate values are so large that they overflow
// unsigned __int128
class InfNum {
public:
	InfNum() = default;
	InfNum(int i) {
		operator=(i);
	}
	InfNum(std::deque<int8_t> i) {
		iBuf = i;
	}
	InfNum(std::string n) : InfNum(StrToIbuf(n)) {
		
	}
	InfNum(const char *s) : InfNum(std::string(s)) {
		
	}
	int operator=(int i) {
		iBuf = ConvertIntToiVec(i);
		return i;
	}
	InfNum operator+(int i) {
		auto old = *this;
		IncBy(i);
		auto newt = *this;
		*this = old;
		return newt;
	}
	InfNum& operator+=(InfNum i) {
		IncBy(i);
		return *this;
	}
	InfNum& operator*=(InfNum i) {
		MulBy(i);
		return *this;
	}
	void IncBy(int i) {
		IncBy(InfNum(i));
	}
	void IncBy(InfNum i) {
		iBuf = AddTwoiVecs(i.iBuf, iBuf);
	}
	void DecBy(int i) {
		DecBy(InfNum(i));
	}
	void DecBy(InfNum i) {
		iBuf = SubTwoiVecs(iBuf, i.iBuf);
	}
	InfNum operator+(InfNum i) {
		return AddTwoiVecs(iBuf, i.iBuf);
	}
	InfNum operator-(InfNum i) {
		return SubTwoiVecs(iBuf, i.iBuf);
	}
	InfNum operator*(InfNum i) {
		InfNum dthis = *this;
		dthis.MulBy(i);
		return dthis;
	}
	InfNum operator/(InfNum i) {
		InfNum dthis = *this;
		dthis.DivBy(i);
		return dthis;
	}
	int8_t operator[](int i) {
		return iBuf[i];
	}
	void MulBySlow(int i) {
		if (i == 0) {
			iBuf.clear();
			return;
		}
		std::deque<int8_t> bAdd = iBuf; // store curVec
		for (int j = 0; j < i - 1; j++) {
			auto nBuf = AddTwoiVecs(bAdd, iBuf); // add to our vector over and over
			iBuf = nBuf;
		}
	}
	// performs grade school multiplication using the more primitive mul function
	void MulBy(InfNum in) {
		InfNum out = 0, old = *this;
		int ct = 0;
		for (auto i : in.iBuf) {
			auto store = old;
			old.MulBySlow(+i);
			InfNum inter = old;
			old = store;
			inter.Mul10Pow(ct);
			out += inter;
			++ct;
		}
		*this = out;
	}
	std::pair<InfNum, InfNum> DivBySlow_r(InfNum dsor) { // output: pair<result, remainder>
		InfNum res = 0, dthis = *this;
		while (dthis >= dsor) {
			dthis.DecBy(dsor);
			res += 1;
		}
		return {res, dthis};
	}
	void DivBySlow(InfNum d) {
		*this = DivBySlow_r(d).first;
	}
	// performs grade school long division using the more primitive division function
	void DivBy(InfNum dsor) {
		InfNum cNum = *this, xNum = 0, out = 0; // xnum: middle number in calcing
		for (int i = cNum.iBuf.size() - 1; i >= 0; i--) {
			xNum.Mul10Pow(1);
			xNum.TrimLeadingZeros(xNum.iBuf);
			xNum += cNum.iBuf[i];
			auto inter = xNum.DivBySlow_r(dsor);
			xNum = inter.second;
			out.iBuf.push_front(inter.first.iBuf[0]); // res will never be > 9
		}
		out.TrimLeadingZeros(out.iBuf);
		*this = out;
	}
	static std::deque<int8_t> ConvertIntToiVec(int v) {
		std::deque<int8_t> out;
		int nDigits = iLog10(v) + 1;
		for (int i = 0; i < nDigits; i++) {
			out.push_back(static_cast<int>(v/pow(10, i)) % 10);
		}
		return out;
	}
	static std::deque<int8_t> AddTwoiVecs(std::deque<int8_t> v1, std::deque<int8_t> v2) {
		bool carry = false;
		auto& bigger = (v1.size() > v2.size()) ? v1 : v2;
		auto& smaller = (v1.size() > v2.size()) ? v2 : v1;
		
		int i = 0;
		std::deque<int8_t> out;
		for (; i < bigger.size(); i++) {
			int val = bigger[i] + ((i < smaller.size()) ? smaller[i] : 0);
			if (carry)
				++val;
			carry = val > 9;

			val %= 10;
			out.push_back(val);
		}
		if (carry)
			out.push_back(1);
		return out;
	}
	static std::deque<int8_t> SubTwoiVecs(std::deque<int8_t> v1, std::deque<int8_t> v2) {
		int carry = false;
		auto& bigger =  v1;
		auto& smaller = v2;
		if (v1.size() < v2.size())
			return std::deque<int8_t>();
		int i = 0;
		std::deque<int8_t> out;
		for (; i < bigger.size(); i++) {
			int val = bigger[i] - ((i < smaller.size()) ? smaller[i] : 0);
			carry = val < 0;
			if (carry) {
				if (bigger.size() > i + 1) {
					bigger[i + 1] -= 1;
				} else {
					// negative number
					return std::deque<int8_t>();
				}
				val += 10;
			}
			val %= 10;
			out.push_back(val);
		}
		TrimLeadingZeros(out);
		return out;
	}
	static std::deque<int8_t> StrToIbuf(std::string n) {
		std::deque<int8_t> out;
		for (auto c : n) {
			if (isdigit(c)) {
				out.push_front(c - '0');
			} else {
				return {};
			}
		}
		return out;
	}
	void print() const {
		for (int i = iBuf.size() - 1; i >= 0; i--) {
#ifdef INCLUDE_COMMAS
			if (i % 3 == 2 && i != iBuf.size() - 1)
				putchar(',');
#endif
			printf("%d", +iBuf[i]);
		}
	}
	bool operator<(InfNum i) {
		i.DecBy(*this);
		return i.IsNotZero();
	}
	bool operator>(InfNum i) {
		i.IncBy(1);
		return !operator<(std::move(i));
	}
	bool operator>=(InfNum i) {
		return !operator<(std::move(i));
	}
	bool operator<=(InfNum i) {
		return !operator>(std::move(i));
	}
	InfNum& operator+=(int i) {
		IncBy(i);
		return *this;
	}
	InfNum& operator-=(int i) {
		DecBy(i);
		return *this;
	}
	bool IsNotZero() {
		return iBuf.size() != 0;
	}
	int ToInt() {
		return ToInt128();
	}
	size_t ToSize_t() {
		return ToInt128();
	}
	unsigned __int128 ToInt128() {
		unsigned __int128 ret = 0;
		for (int i = 0; i < iBuf.size(); i++) {
			ret += (unsigned __int128)iBuf[i]*(unsigned __int128)pow(10, i);
		}
		return ret;
	}
	void Mul10Pow(int p) {
		for (int i = 0; i < p; i++) {
			iBuf.push_front(0);
		}
	}
	InfNum Exp(InfNum d) {
		InfNum out = 1;
		for (; d.IsNotZero(); d.DecBy(1)) {
			out *= *this;
		}
		return out;
	}
	static InfNum Factorial(InfNum t) {
		return FactorialLimit(t, t);
	}
	static InfNum FactorialLimit(InfNum t, InfNum limit) {
		InfNum res = 1;
		for (; t.IsNotZero() && limit.IsNotZero(); t.DecBy(1), limit.DecBy(1)) {
			res *= t;
		}
		return res;
	}
private:
	std::deque<int8_t> iBuf;
	static int iLog10(int n) { // this might be faster than float log10
		for (int i = 1, tenExp = 1; i <= 10; i++) { // 10 is the max digits in an int
			if (n < (tenExp *= 10))
				return i - 1;
		}
		return 0;
	}
	static void TrimLeadingZeros(std::deque<int8_t>& dV) {
		for (int i = dV.size(); i > 0; i--) {
			int rI = i - 1;
			if (dV[rI] == 0)
				dV.pop_back();
			else
				break;
		}
	}
	bool isNegative = false;
};

using Inf_t = InfNum;

std::ostream &operator<<(std::ostream &os, InfNum const &m) {
	m.print();
	return os;
}

#include <vector>

size_t GeneratePascalNumOld(int pascalPlace, int pascalDegree) {
    int pascalNumber = 0;
    
	if (pascalPlace == 0)
		return 1;
    
	unsigned __int128 inverseLC = 1;
    for (int i = 0; i < pascalPlace; i++) {
        inverseLC *= (i + 1);
        //printf("Multiplied: %d\n", inverseLC);
    }
	if (inverseLC == 0)
		inverseLC -= 1;
	//printf("Inverse LC: %d\n", inverseLC);
    //printf("pascal deg - pascal plc: %d\n", (pascalDegree - pascalPlace));
	unsigned __int128 output = (pascalDegree - pascalPlace);
	//printf("output first %f\n", output);
    for (int i = 0; i < (pascalPlace - 1); i++) {
        output *= ((pascalDegree - pascalPlace) + (i + 1));
	}
	//printf("output now %lld\n", output);
	output /= inverseLC;
	
    //printf("%d", inverseLC);
    return (size_t)output;
}

InfNum GeneratePascalNum(int pascalPlace, int pascalDegree) {
	if (pascalPlace == 0)
		return 1;
	pascalDegree--;
    return InfNum::FactorialLimit(pascalDegree, pascalPlace)/(InfNum::Factorial(pascalPlace));
}

int main() {
	int option;
	std::cout << "0: print triangle, 1: print row. choose: ";
	std::cin >> option;
	
    int pascalRow = 0;
    printf("enter max degree: ");
    scanf("%d", &pascalRow);
	//printf("Results:\n");
	for (int j = (!option) ? 1 : pascalRow; j <= pascalRow; j++) {
		//printf("Triangle: %d\n", j);
		std::vector<InfNum> curRow;
		if (j % 2 == 0) {
			for (int i = 0; i < j/2; i++) {
				auto num = GeneratePascalNum(i, j);
				std::cout << num << " ";
				curRow.push_back(num);
			}
			for (int i = j/2 - 1; i >= 0; i--) {
				std::cout << curRow[i] << " ";
			}
		} else if (j == 1) {
			printf("1");
		} else {
			for (int i = 0; i < j/2 + 1; i++) {
				auto num = GeneratePascalNum(i, j);
				std::cout << num << " ";
				curRow.push_back(num);
			}
			for (int i = j/2 - 1; i >= 0; i--) {
				std::cout << curRow[i] << " ";
			}
		}
		printf("\n--------------\n");
	}
    
    return 0;
}