// { dg-options "-fno-lto" }

// Only the symbols pr100010_0.C leaves undefined.  The test just links, so the
// constructor can be an empty stub of the same name.  A real one would refer to
// the vtable of its class and keep the method in question alive.

struct String {
  String (char);
  ~String ();
};

String::String (char) {}
String::~String () {}

struct LandmarkErrorHandler {
  LandmarkErrorHandler ();
};

LandmarkErrorHandler::LandmarkErrorHandler () {}
