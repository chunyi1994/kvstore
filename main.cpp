#include "redis-server.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
using namespace std;
using namespace libevent;

int main(int argc, char** argv){
    //if(argc <= 1){
        //return -1;
    //}

    //int port = atoi(argv[1]);
    RedisServer server(23333);
    server.start();
    return 0;
}
