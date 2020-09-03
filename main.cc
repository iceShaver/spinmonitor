#include "app.hh"
#include <iostream>
#include "config.hh"
#include "tools.hh"
/*
 * SpinMonitor
 * * monitors spinning of HDD
 * * logs spinning info with specified interval
 * * draws plot of spinning time
 * * show spinning statistics
 * *
 * TODO: add time statistics
 * TODO: add daemon concurrent run protection
 *
 */

int main(int argc, char **argv) {
  App::Init(argc, argv);
  App::Run();
  return 0;
}
