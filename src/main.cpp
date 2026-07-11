#include <app/app.h>
#include <mwac/result.hpp>

int main()
{
  auto appResult = run();

  if (!appResult)
  {
    err_log(appResult.error());
    return 1;
  }

  return 0;
}
