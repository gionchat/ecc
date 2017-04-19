//Headers
#include <iostream>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <random>
#include <map>
typedef long number;

#ifndef ELLIPTIC_CURVE_CRYPTOGRAPHY_H
	#define ELLIPTIC_CURVE_CRYPTOGRAPHY_H
#endif

class Point;
class InfinityPoint;
class SimpleEllipticCurve;
class EllipticCurve;
class EllipticCurvePoint;	

class SimpleEllipticCurve { //defines simple EC
	protected:
		number a,b; //parameters
		EllipticCurvePoint *G; //generator point
		number n; //order
		number h; //cofactor
	private:
		virtual EllipticCurvePoint add (EllipticCurvePoint &P, EllipticCurvePoint &Q);
	public:
		SimpleEllipticCurve(number a_, number b_, EllipticCurvePoint *G_, number n_, number h_) : a(a_), b(b_), G(G_), n(n_), h(h_) { //TODO check if on ECC
			assert(4*a*a*a + 27*b*b != 0);
			//assert((*this)(G->x) == *G)
		};
		virtual EllipticCurvePoint operator() (number x);
		friend class EllipticCurvePoint;
};

class EllipticCurve : public SimpleEllipticCurve { //Elliptic Curve over Z/pZ
	public:
		EllipticCurve(number a_, number b_, number p_, EllipticCurvePoint *G_, number n_, number h_) : SimpleEllipticCurve(a_,b_,G_,n_,h_), p(p_) {};
		EllipticCurvePoint operator() (number x);
		number order() {return SimpleEllipticCurve::n;}
	protected:
		number p;
	private:
		EllipticCurvePoint add (EllipticCurvePoint &P, EllipticCurvePoint &Q);
	public:	
		static number multiplicativeInverse(number x, number p);
		static number gcd(number a, number b);
		static number fastPowMod(number a, number b, number n);	
		
};


class Point {
	protected:
		number x,y;
	public:
		Point(number x_, number y_);
		Point(const Point& p);
		virtual Point& operator=(const Point &p);
		virtual Point operator+ (Point &other);
		virtual bool operator== (Point &other);
		friend std::ostream& operator << (std::ostream &out, const Point &p);
		virtual number getX(){return x;}
		virtual number getY(){return y;}
};

class EllipticCurvePoint : public Point {
	protected:
		SimpleEllipticCurve *E;
	public:
		EllipticCurvePoint (number x_, number y_, SimpleEllipticCurve *E_) : Point(x_, y_), E(E_) {};
		EllipticCurvePoint operator+(EllipticCurvePoint other);
		EllipticCurvePoint operator* (number m);
		EllipticCurvePoint& operator+=(EllipticCurvePoint other);
		EllipticCurvePoint& operator=(const EllipticCurvePoint &other);
		bool operator== (const EllipticCurvePoint &other);
		void setCurve(SimpleEllipticCurve *e) {
			E = e;
		}	
		virtual bool isInfinity() { return false; }
		friend class SimpleEllipticCurve;
		friend class EllipticCurve;
};

//infinity singleton
class InfinityPoint : public EllipticCurvePoint {
	public:
		bool isInfinity() { return true; }
		static InfinityPoint& instance() {
			static InfinityPoint& INFINITY_POINT = *new InfinityPoint();
			return INFINITY_POINT;
		} 
		EllipticCurvePoint operator+(const EllipticCurvePoint &p) {return p;}
		InfinityPoint(const InfinityPoint&) = delete;
		void operator=(const InfinityPoint&) = delete;
	private:
		InfinityPoint() : EllipticCurvePoint(0,0,NULL) {};
};

class SharedKey {
	public:
		EllipticCurve *curve;
		EllipticCurvePoint *generator_point;
		SharedKey(number, number, number, EllipticCurvePoint*, number, number);
		EllipticCurvePoint* getGeneratorPoint();
		void generatePair (number&, EllipticCurvePoint&); 
		number random();
	private:
};

class ECDH {
	public:
		SharedKey *sh_key;
		ECDH (SharedKey *);
		EllipticCurvePoint encrypt ();
		EllipticCurvePoint decrypt (ECDH&); 
		std::pair<number,number> signECDSA(number z); //z to string -> hash
		static number multiplicativeInverse(number a, number n, number, number);
		int verify(std::pair<number,number>, number);
	private:
		number pr_key;
		EllipticCurvePoint pub_key = *new EllipticCurvePoint(0,0,NULL);
};

