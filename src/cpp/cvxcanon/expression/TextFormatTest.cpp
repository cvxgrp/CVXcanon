

#include "gtest/gtest.h"

#include "cvxcanon/expression/ExpressionUtil.hpp"
#include "cvxcanon/expression/TextFormat.hpp"

extern std::unordered_map<int, std::string> kExpressionNames;
TEST(TextFormatTest, Names) {
  for (int i = 0; i < Expression::NUM_TYPES; i++) {
    EXPECT_TRUE(kExpressionNames.find(i) != kExpressionNames.end());
  }
}

TEST(TextFormatTest, Basic) {
  Expression x = var(10, 5, 0);
  EXPECT_EQ("var", format_expression(x));
}
