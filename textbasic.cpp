#include <tuple>

#include "textbasic.h"


bool TextRange::valid() const
{
    return    begin.valid()
           && end.valid()
           && std::tie(begin.line, begin.column) <= std::tie(end.line, end.column);
}
