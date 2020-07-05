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
    document.SetObject();		//�����ʹ�����������AddMember���ᱨ��
    //������������ʹ��allocator��������������ַ�ʽ�����ε���GetAllocator()ȥ��ȡallocator
    Document::AllocatorType& allocator = document.GetAllocator();

    /***********************/
    Value value(kObjectType);
    value.SetInt(123);
    document.AddMember("i_key",value,allocator); //document���int��������
    value.SetFloat(1.23);
    document.AddMember("f_key",value,allocator); //document���float��������
    value.SetString("Mio");
    document.AddMember("s_key",value,allocator); //document���string��������

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
    //redisĬ�ϼ����˿�Ϊ6387 �����������ļ����޸�
    redisContext* redis = redisConnect("127.0.0.1", 6379);
    if ( NULL == redis || redis->err)
    {       // redisΪNULL��redis->err�����ֲ�ͬ�Ĵ�����redis->errΪtrue����ʹ��redis->errstr�鿴������Ϣ
        redisFree(redis);
        printf("Connect to redisServer faile\n");
        return ;
    }
    printf("Connect to redisServer Success\n");

    const char* passcanmd = "123456";
    redisReply *reply = (redisReply *)redisCommand(redis, "AUTH %s", passcanmd);

    const char* command1 = "set stest1 value1";
    reply = (redisReply*)redisCommand(redis, command1);    // ִ��������ǿת��redisReply*����
    if( NULL == reply)
    {
        printf("Execut command1 failure\n");
        redisFree(redis);     // ����ִ��ʧ�ܣ��ͷ��ڴ�
        return;
    }

    if( !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str,"OK")==0))
    {       // �ж�����ִ�еķ���ֵ
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
    reply = (redisReply*)redisCommand(redis, command2);    // ִ��������ǿת��redisReply*����
    if( NULL == reply)
    {
        printf("Execut command1 failure\n");
        redisFree(redis);     // ����ִ��ʧ�ܣ��ͷ��ڴ�
        return;
    }

    if( !(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str,"OK")==0))
    {       // �ж�����ִ�еķ���ֵ
        printf("Failed to execute command[%s]\n",command2);
        freeReplyObject(reply);
        redisFree(redis);
        return;
    }


    freeReplyObject(reply);
    printf("Succeed to execute command[%s]\n", command1);
    // һ����������Է���ֵ���д���

}
int main()
{
    doTest();
    return 0;
}
