
#include "Expression.hpp"

Expression::Expression(
    Type type,
    std::vector<Expression> args)
    : data_(new Data(type, std::move(args))) {}
