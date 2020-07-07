#ifndef REDISCONTROL_H
#define REDISCONTROL_H
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <vector>
#include <hiredis/hiredis.h>
#include <string>
using namespace std;
class redisControl
{
public:
    redisControl();
    ~redisControl();
    redisControl(const std::vector<string> &keys);
    int connect(const std::string &ip, int port);
    void setpublishForceData( std::vector<double> &data);

    int publish(const std::string &channel = "publish XZJQR_DATAARR %s");

private:
    void transfromJson(const std::vector<string> &keys, const std::vector<double> &val, string &out);
    double getTime();
    redisContext* redis;
    std::vector<std::string> keys;
    std::vector<double> val;
    std::string out;
};

#endif // REDISCONTROL_H
