#include "../platform/generic/ws_string.h"
#include <cassert>


using websocket::String;
using websocket::Stream;

int main()
{
    String bla("asd");

    assert(bla.length() == 3);
    assert(bla.c_str() != 0);
    assert(strcmp(bla.c_str(), "asd") == 0);

    assert(bla.startsWith("as"));
    assert(bla == "asd");
    assert(bla != "asda");


    Stream wsDebug(stdout);
    wsDebug << bla << "\n";
}
