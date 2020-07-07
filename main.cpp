
#include <string>
#include <iostream>
#include "rediscontrol.h"
#include <thread>
using namespace rapidjson;

int main()
{

//    doTest();
    redisControl red;
    assert(0 == red.connect("192.168.100.79", 6379));
//    assert(0 == red.connect("127.0.0.1", 6379));
    vector<double> temp{1, 2,3,4,5,6};
    red.setpublishForceData(temp);
//    for(int i = 0 ; i < 55;  i++){
    for(;;){
        if(0!= red.publish())
        {
            perror("publish error");
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "publish ok "<<std::endl;
    }
    return 0;
}
