#ifndef CUSASSERTION
#define CUSASSERTION

#include <string>
#include <sstream>

#ifdef NDEBUG
#define CusAssertion(Expr, Msg)//Debug mode not available, do nothing
#else
#define CusAssertion(Expr, Msg)\
if (!Expr){\
std::stringstream Output; Output << Msg; std::cerr << "Assertion failed in file: " << __FILE__\
	<< " at line: " << __LINE__ << " with debug message: " << Output.str().c_str() << std::endl; exit(-1); \
}
#endif

#endif