#include "encoders.h"

using namespace minibot;

/* ** *************************************************************
* Constructors
** ** ************************************************************/
Encoders::Encoders(int32_t f, int32_t b, int32_t l, int32_t r)
	: front(f), back(b), left(l), right(r) {}

/* ** *************************************************************
* Methods
** ** ************************************************************/
int32_t Encoders::average() const{
	return (front + back + left + right) / 4;
}

/* ** *************************************************************
* Operator overloading
** ** ************************************************************/
Encoders Encoders::operator+(const Encoders& other) const {
	return Encoders(
		this->front + other.front,
		this->back  + other.back,
		this->left  + other.left,
		this->right + other.right
	);
}


Encoders Encoders::operator-(const Encoders& other) const{
	return Encoders(
		this->front - other.front,
		this->back  - other.back,
		this->left  - other.left,
		this->right - other.right
	);
}



Encoders Encoders::operator+(const uint32_t& scalar) const{
	return Encoders(
		this->front + scalar,
		this->back  + scalar,
		this->left  + scalar,
		this->right + scalar
	);
}


Encoders Encoders::operator-(const uint32_t& scalar) const{
	return Encoders(
		this->front - scalar,
		this->back  - scalar,
		this->left  - scalar,
		this->right - scalar
	);
}


Encoders& Encoders::operator+=(const Encoders& other){
	this->front += other.front;
	this->back  += other.back;
	this->left  += other.left;
	this->right += other.right;
	return *this; // Must return a reference to *this
}


Encoders& Encoders::operator-=(const Encoders& other){
	this->front -= other.front;
	this->back  -= other.back;
	this->left  -= other.left;
	this->right -= other.right;
	return *this;
}

Encoders& Encoders::operator+=(const uint32_t& scalar){
	this->front += scalar;
	this->back  += scalar;
	this->left  += scalar;
	this->right += scalar;
	return *this;
}

Encoders& Encoders::operator-=(const uint32_t& scalar){
	this->front -= scalar;
	this->back  -= scalar;
	this->left  -= scalar;
	this->right -= scalar;
	return *this;
}


bool Encoders::operator==(const Encoders& other) const {
	return (this->front == other.front &&
	        this->back  == other.back  &&
	        this->left  == other.left  &&
	        this->right == other.right);
}


bool Encoders::operator!=(const Encoders& other) const {
	return (this->front != other.front ||
	        this->back  != other.back  ||
	        this->left  != other.left  ||
	        this->right != other.right);
}
