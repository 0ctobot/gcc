// { dg-lto-do link }
// { dg-lto-options { { -O2 -flto -fdevirtualize-at-ltrans } } }

// ErrorHandler::decorate only exists as an inline clone by the time LTRANS
// boundaries are computed.  Looking up devirtualization targets for those
// boundaries must not create a new root for the clone tree of that method.

struct String {
	String(char);
	~String();
};
struct ErrorHandler {
	virtual String decorate(const String &);
};
String ErrorHandler::decorate(const String &str) {
	decorate(str);
	return 0;
}
struct LandmarkErrorHandler : ErrorHandler {
	String _landmark;
	LandmarkErrorHandler();
	String decorate(const String &);
};
String LandmarkErrorHandler::decorate(const String &) {
	return ErrorHandler::decorate(0);
}
struct Charstring {
	virtual ~Charstring();
	virtual void process() = 0;
} *interpret_cs;
Charstring::~Charstring() {}
struct : Charstring {
	void process() {}
} _cdv;
int main() {
	LandmarkErrorHandler();
	interpret_cs->process();
}
