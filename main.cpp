#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <hiredis/hiredis.h>
#include <string>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
using namespace rapidjson;


void do_rapidjson(std::string &out)
{
    Document document; 			// Null
    document.SetObject();		//如果不使用这个方法，AddMember将会报错
    //下面会遇到多次使用allocator的情况，采用这种方式避免多次调用GetAllocator()去获取allocator
    Document::AllocatorType& allocator = document.GetAllocator();

    /***********************/
    Value value(kObjectType);
    value.SetInt(123);
    document.AddMember("i_key",value,allocator); //document添加int类型数据
    value.SetFloat(1.23);
    document.AddMember("f_key",value,allocator); //document添加float类型数据
    value.SetString("Mio");
    document.AddMember("s_key",value,allocator); //document添加string类型数据

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    document.Accept(writer);

    std::cout <<sb.GetString()<<std::endl;
    out = std::string(sb.GetString());
//    writer.StartObject();
//    writer.String("i_key");
//    writer.String("f_key");
//    writer.String("s_key");
//    writer.EndObject();
}

void doTest()
{
    //redis默认监听端口为6387 可以再配置文件中修改
    redisContext* redis = redisConnect("127.0.0.1", 6379);
    if ( NULL == redis || redis->err)
    {       // redis为NULL与redis->err是两种不同的错误，若redis->err为true，可使用redis->errstr查看错误信息
        redisFree(redis);
        printf("Connect to redisServer faile\n");
        return ;
    }
    printf("Connect to redisServer Success\n");

    const char* passcanmd = "123456";
    redisReply *reply = (redisReply *)redisCommand(redis, "AUTH %s", passcanmd);

    const char* command1 = "set stest1 value1";
    reply = (redisReply*)redisCommand(redis, command1);    // 执行命令，结果强转成redisReply*类型
    if( NULL == reply)
    {
        printf("Execut command1 failure\n");
        redisFree(redis);     // 命令执行失败，释放内存
        return;
    }

    if( !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str,"OK")==0))
    {       // 判断命令执行的返回值
        printf("Failed to execute command[%s]\n",command1);
        freeReplyObject(reply);
        redisFree(redis);
        return;
    }


    std::string para;
    do_rapidjson(para);
    printf("do_rapidjson \n [%s]\n", para.c_str());
    std::string str = "set stest2 ";
    const char* command2 = str.append(para).c_str();
    reply = (redisReply*)redisCommand(redis, command2);    // 执行命令，结果强转成redisReply*类型
    if( NULL == reply)
    {
        printf("Execut command1 failure\n");
        redisFree(redis);     // 命令执行失败，释放内存
        return;
    }

    if( !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str,"OK")==0))
    {       // 判断命令执行的返回值
        printf("Failed to execute command[%s]\n",command2);
        freeReplyObject(reply);
        redisFree(redis);
        return;
    }


    freeReplyObject(reply);
    printf("Succeed to execute command[%s]\n", command1);
    // 一切正常，则对返回值进行处理

}
int main()
{
    doTest();
    return 0;
}
