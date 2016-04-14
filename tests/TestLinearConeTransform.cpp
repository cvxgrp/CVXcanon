
#include "Expression.hpp"
#include "ExpressionUtil.hpp"
#include "LinearConeTransform.hpp"
#include "TextFormat.hpp"

void test_linear_cone_transform() {
  const int m = 10;
  const int n = 5;

  printf("constructing variables/constants\n");
  Expression A = constant(m,n);
  Expression b = constant(m,1);
  Expression x = var(n,1);

  printf("constructing problem\n");
  Problem problem = {
    Problem::MINIMIZE,
    add(quad_over_lin(add(mul(A, x), neg(b)), constant(1)),
        mul(constant(1), p_norm(x, 1))),
    {}};

  printf("orig nodes: %d\n", count_nodes(problem));
  printf("%s\n", format_problem(problem).c_str());

  printf("transforming problem\n");
  LinearConeTransform transform;
  problem = transform.transform(problem);

  printf("final nodes: %d\n", count_nodes(problem));
  printf("%s\n", format_problem(problem).c_str());
}

int main(int argc, char* argv[]) {
  test_linear_cone_transform();
  return 0;
}
