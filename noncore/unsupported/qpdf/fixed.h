#ifndef __FIXED_H__
#define __FIXED_H__

#include <iostream>

#define _GCC_TEMPLATE_BUG_ 1

template <unsigned int SH> class fixed {
public:
	inline fixed ( int i = 0 )      : m_f ( i2f( i ) ) { }
	inline fixed ( double d )       : m_f ( d2f( d )) { }
	inline fixed ( const fixed &f ) : m_f ( f. m_f ) { }
	
	inline operator bool ( ) const   { return m_f != 0; }	
	inline operator double ( ) const { return f2d( m_f ); }
	inline operator float ( ) const  { return (float) f2d( m_f ); }
	inline operator int ( ) const    { return (int) f2i( m_f ); }

	inline fixed &operator = ( const fixed &f ) { m_f = f. m_f; return *this; }
	inline fixed &operator = ( double d )       { m_f = d2f( d ); return *this; }
	inline fixed &operator = ( float f )        { m_f = d2f( f ); return *this; }
	inline fixed &operator = ( int i )          { m_f = i2f( i ); return *this; }

	inline fixed &operator += ( const fixed &f ) { m_f += f. m_f; return *this; }
	inline fixed &operator -= ( const fixed &f ) { m_f -= f. m_f; return *this; }
	inline fixed &operator *= ( const fixed &f ) { m_f = mul ( m_f, f. m_f ); return *this; }
	inline fixed &operator /= ( const fixed &f ) { m_f = div ( m_f, f. m_f ); return *this; }

	inline fixed &operator += ( int i ) { m_f += i2f( i ); return *this; }
	inline fixed &operator -= ( int i ) { m_f -= i2f( i ); return *this; }
	inline fixed &operator *= ( int i ) { m_f *= i; return *this; }
	inline fixed &operator /= ( int i ) { m_f /= i; return *this; }

	inline fixed &operator += ( double d ) { m_f += d2f( d ); return *this; }
	inline fixed &operator -= ( double d ) { m_f -= d2f( d ); return *this; }
	inline fixed &operator *= ( double d ) { m_f = mul ( m_f, d2f( d )); return *this; }
	inline fixed &operator /= ( double d ) { m_f = div ( m_f, d2f( d )); return *this; }

	inline fixed operator - ( ) const { return fixed ( -m_f, true ); }

	inline fixed operator + ( const fixed &f ) const { return fixed ( m_f + f. m_f, true ); }
	inline fixed operator - ( const fixed &f ) const { return fixed ( m_f - f. m_f, true ); }
	inline fixed operator * ( const fixed &f ) const { return fixed ( mul ( m_f, f. m_f ), true ); }
	inline fixed operator / ( const fixed &f ) const { return fixed ( div ( m_f, f. m_f ), true ); }

	inline fixed operator + ( double d ) const { return fixed ( m_f + d2f( d ), true ); }
	inline fixed operator - ( double d ) const { return fixed ( m_f - d2f( d ), true ); }
	inline fixed operator * ( double d ) const { return fixed ( mul ( m_f, d2f( d )), true ); }
	inline fixed operator / ( double d ) const { return fixed ( div ( m_f, d2f( d )), true ); }

	inline fixed operator + ( int i ) const { return fixed ( m_f + i2f( i ), true ); }
	inline fixed operator - ( int i ) const { return fixed ( m_f - i2f( i ), true ); }
	inline fixed operator * ( int i ) const { return fixed ( m_f * i, true ); }
	inline fixed operator / ( int i ) const { return fixed ( m_f / i, true ); }

	inline bool operator <  ( const fixed &f ) const { return m_f < f. m_f; }
	inline bool operator >  ( const fixed &f ) const { return m_f > f. m_f; }
	inline bool operator <= ( const fixed &f ) const { return m_f <= f. m_f; }
	inline bool operator >= ( const fixed &f ) const { return m_f >= f. m_f; }
	inline bool operator == ( const fixed &f ) const { return m_f == f. m_f; }
	inline bool operator != ( const fixed &f ) const { return m_f != f. m_f; }

	inline bool operator <  ( double d ) const { return m_f < d2f( d ); }
	inline bool operator >  ( double d ) const { return m_f > d2f( d ); }
	inline bool operator <= ( double d ) const { return m_f <= d2f( d ); }
	inline bool operator >= ( double d ) const { return m_f >= d2f( d ); }
	inline bool operator == ( double d ) const { return m_f == d2f( d ); }
	inline bool operator != ( double d ) const { return m_f != d2f( d ); }

	inline bool operator <  ( int i ) const { return m_f < i2f( i ); }
	inline bool operator >  ( int i ) const { return m_f > i2f( i ); }
	inline bool operator <= ( int i ) const { return m_f <= i2f( i ); }
	inline bool operator >= ( int i ) const { return m_f >= i2f( i ); }
	inline bool operator == ( int i ) const { return m_f == i2f( i ); }
	inline bool operator != ( int i ) const { return m_f != i2f( i ); }

#if _GCC_TEMPLATE_BUG_
public: 
#else
private:
#endif
	typedef int fix_t;

	inline static double f2d ( fix_t f )   { return ((double) f ) / ((double) ( 1 << SH )); }
	inline static fix_t  d2f ( double d )  { return (fix_t) ( d * ((double) ( 1 << SH ))); }
	
	inline static int    f2i ( fix_t f )   { return (int) ( f >> SH ); }
	inline static fix_t  i2f ( int i )     { return (fix_t) ( i << SH ); }	
	
	inline static fix_t mul ( fix_t m1, fix_t m2 )	{ return (fix_t) ((((long long int) m1 ) * m2 ) >> SH ); }	
	inline static fix_t div ( fix_t d1, fix_t d2 )	{ return (fix_t) ((((long long int) d1 ) << SH ) / d2 ); }
	
	fixed ( fix_t f, bool /*dummy*/ ) : m_f ( f ) { }

	//data
	fix_t m_f;

// friends:		
#if !_GCC_TEMPLATE_BUG_
	friend fixed operator + <> ( int i, const fixed &f );
	friend fixed operator - <> ( int i, const fixed &f );
	friend fixed operator * <> ( int i, const fixed &f );
	friend fixed operator / <> ( int i, const fixed &f );

	friend fixed operator + <> ( double d, const fixed &f );
	friend fixed operator - <> ( double d, const fixed &f );
	friend fixed operator * <> ( double d, const fixed &f );
	friend fixed &operator / <> ( double d, const fixed<SH> &f );

	friend bool operator <  <> ( double d, const fixed &f );
	friend bool operator >  <> ( double d, const fixed &f );
	friend bool operator <= <> ( double d, const fixed &f );
	friend bool operator >= <> ( double d, const fixed &f );
	friend bool operator == <> ( double d, const fixed &f );
	friend bool operator != <> ( double d, const fixed &f );

	friend bool operator <  <> ( int i, const fixed &f );
	friend bool operator >  <> ( int i, const fixed &f );
	friend bool operator <= <> ( int i, const fixed &f );
	friend bool operator >= <> ( int i, const fixed &f );
	friend bool operator == <> ( int i, const fixed &f );
	friend bool operator != <> ( int i, const fixed &f );
	
	friend long int lrint ( const fixed &f );
	friend fixed sqrt ( const fixed &f );
	friend fixed fabs ( const fixed &f );
#endif
};


template <unsigned int SH> inline fixed<SH> operator + ( int i, const fixed<SH> &f )  { return fixed<SH> ( fixed<SH>::i2f( i ) + f. m_f, true ); }
template <unsigned int SH> inline fixed<SH> operator - ( int i, const fixed<SH> &f )  { return fixed<SH> ( fixed<SH>::i2f( i ) - f. m_f, true ); }
template <unsigned int SH> inline fixed<SH> operator * ( int i, const fixed<SH> &f )  { return fixed<SH> ( i * f. m_f, true ); }
template <unsigned int SH> inline fixed<SH> operator / ( int i, const fixed<SH> &f )  { return fixed<SH> ( fixed<SH>::div ( fixed<SH>::i2f( i ), f. m_f ), true ); } 
//template <unsigned int SH> inline fixed<SH> operator / ( int i, const fixed<SH> &f )  { return fixed<SH> ( fixed<SH>::i2f ( i / fixed<SH>::f2i ( f. m_f )), true ); } 

template <unsigned int SH> inline fixed<SH> operator + ( double d, const fixed<SH> &f ) { return fixed<SH> ( fixed<SH>::d2f( d ) + f. m_f, true ); }
template <unsigned int SH> inline fixed<SH> operator - ( double d, const fixed<SH> &f ) { return fixed<SH> ( fixed<SH>::d2f( d ) - f. m_f, true ); }
template <unsigned int SH> inline fixed<SH> operator * ( double d, const fixed<SH> &f ) { return fixed<SH> ( fixed<SH>::mul ( fixed<SH>::d2f( d ), f. m_f ), true ); }
template <unsigned int SH> inline fixed<SH> operator / ( double d, const fixed<SH> &f ) { return fixed<SH> ( fixed<SH>::mul ( fixed<SH>::d2f( d ), f. m_f ), true ); }

template <unsigned int SH> inline bool operator <  ( double d, const fixed<SH> &f ) { return fixed<SH>::d2f( d ) < f. m_f; }
template <unsigned int SH> inline bool operator >  ( double d, const fixed<SH> &f ) { return fixed<SH>::d2f( d ) > f. m_f; }
template <unsigned int SH> inline bool operator <= ( double d, const fixed<SH> &f ) { return fixed<SH>::d2f( d ) <= f. m_f; }
template <unsigned int SH> inline bool operator >= ( double d, const fixed<SH> &f ) { return fixed<SH>::d2f( d ) >= f. m_f; }
template <unsigned int SH> inline bool operator == ( double d, const fixed<SH> &f ) { return fixed<SH>::d2f( d ) == f. m_f; }
template <unsigned int SH> inline bool operator != ( double d, const fixed<SH> &f ) { return fixed<SH>::d2f( d ) != f. m_f; }

template <unsigned int SH> inline bool operator <  ( int i, const fixed<SH> &f ) { return fixed<SH>::i2f( i ) < f. m_f; }
template <unsigned int SH> inline bool operator >  ( int i, const fixed<SH> &f ) { return fixed<SH>::i2f( i ) > f. m_f; }
template <unsigned int SH> inline bool operator <= ( int i, const fixed<SH> &f ) { return fixed<SH>::i2f( i ) <= f. m_f; }
template <unsigned int SH> inline bool operator >= ( int i, const fixed<SH> &f ) { return fixed<SH>::i2f( i ) >= f. m_f; }
template <unsigned int SH> inline bool operator == ( int i, const fixed<SH> &f ) { return fixed<SH>::i2f( i ) == f. m_f; }
template <unsigned int SH> inline bool operator != ( int i, const fixed<SH> &f ) { return fixed<SH>::i2f( i ) != f. m_f; }

template <unsigned int SH> inline long int lrint ( const fixed<SH> &f )
{
	return fixed<SH>::f2i (( f. m_f < 0 ) ? f. m_f - ( 1 << ( SH - 1 )) : f. m_f + ( 1 << ( SH - 1 )));
}

template <unsigned int SH> inline fixed<SH> fabs ( const fixed<SH> &f )
{
	return ( f. m_f < 0 ) ? fixed<SH> ( -f. m_f, true ) : f;
}

// roughly from QPE / qmath.h 
template <unsigned int SH> inline fixed<SH> sqrt ( const fixed<SH> &f )
{
	if ( f. m_f <= 0 )
		return fixed<SH> ( 0, true );
		
	fixed<SH>::fix_t a0 = 0;
	fixed<SH>::fix_t a1 = f. m_f; // take value as first approximation
	
	do {
		a0 = a1;
		a1 = ( a0 + fixed<SH>::div ( f. m_f, a0 )) >> 1;
	} while ( abs ( fixed<SH>::div ( a1 - a0,  a1 )) > 1 );
	
	return fixed<SH> ( a1, true );
}

template <unsigned int SH> inline ostream &operator << ( ostream &o, const fixed<SH> &f )
{
	o << double( f );
	return o;
}

#endif
