#pragma once

#include <cstdint>


namespace minibot{

struct Encoders {
public:
	int32_t front;
	int32_t back;
	int32_t left;
	int32_t right;

public:
	/* ** *************************************************************
	* Constructors
	** ** ************************************************************/
	Encoders(int32_t f=0, int32_t b=0, int32_t l=0, int32_t r=0);

	/* ** *************************************************************
	* Methods
	** ** ************************************************************/
public:
	int32_t average() const;
	uint32_t absAverage() const;

	/* ** *************************************************************
	* Operator overloading
	** ** ************************************************************/
public:
	Encoders operator+(const Encoders& other) const;
	Encoders operator-(const Encoders& other) const;

	Encoders operator+(const uint32_t& scalar) const;
	Encoders operator-(const uint32_t& scalar) const;

	Encoders& operator+=(const Encoders& other);
	Encoders& operator-=(const Encoders& other);

	Encoders& operator+=(const uint32_t& scalar);
	Encoders& operator-=(const uint32_t& scalar);

	bool operator==(const Encoders& other) const;
	bool operator!=(const Encoders& other) const;

};

}; // End namespac