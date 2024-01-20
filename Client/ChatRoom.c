#include "ChatRoom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <json-c/json.h>

/* 状态码 */
enum STATUS_CODE
{
    SUCCESS = 0,        // 成功
    NULL_PTR = -1,      // 空指针
    MALLOC_ERROR = -2,  // 内存分配失败
    ILLEGAL_ACCESS = -3,// 非法访问
    UNDERFLOW = -4,     // 下溢
    OVERFLOW = -5,      // 上溢
    SOCKET_ERROR = -6,  // socket错误
    CONNECT_ERROR = -7, // 连接错误
    SEND_ERROR = -8,    // 发送错误
    RECV_ERROR = -9,    // 接收错误
    FILE_ERROR = -10,   // 文件错误
    JSON_ERROR = -11,   // json错误
    OTHER_ERROR = -12,  // 其他错误

};

#define SERVER_PORT 8888        // 服务器端口号,暂定为8888
#define SERVER_IP "127.0.0.1"   // 服务器ip,暂定为本机ip
#define NAME_SIZE 10            // 用户名长度
#define PASSWORD_SIZE 20        // 密码长度
#define MAX_FRIEND_NUM 10       // 最大好友数量
#define MAX_GROUP_NUM 10        // 最大群组数量
#define MAX_GROUP_MEMBERS_NUM 20// 最大群组成员数量
#define CONTENT_SIZE 1024       // 信息内容长度
#define PATH_SIZE 256           // 文件路径长度



/* 发送json到服务器 */
static int SendJsonToServer(int fd, const char *json)
{
    int ret = 0;
    int len = strlen(json);
    int sendLen = 0;
    while (sendLen < len)
    {
        ret = send(fd, json + sendLen, len - sendLen, 0);
        if (ret < 0)
        {
            perror("send error");
            return ret;
        }
        sendLen += ret;
    }
    return SUCCESS;
}
/* 接收json */
static int RecvJsonFromServer(int fd, char *json)
{
    int ret = 0;            // 返回值
    int len = 0;            // 当前json长度
    int recvLen = 0;        // 已接收的长度
    while (recvLen < CONTENT_SIZE)
    {
        ret = recv(fd, json + recvLen, CONTENT_SIZE - recvLen, 0);  //  从recvLen位置读取CONTENT_SIZE - recvLen长的数据
        if (ret == -1)
        {
            perror("recv error");
            return JSON_ERROR;
        }
        if (ret == 0)   // 读取结束
        {
            break;
        }
        recvLen += ret; // 已接收长度加上新读取的长度ret
        len = strlen(json); // 更新当前长度
        if (recvLen >= CONTENT_SIZE)
        {
            printf("接收的json过长\n");
            return OVERFLOW;
        }
        if (len >= CONTENT_SIZE)
        {
            printf("接收的json过长\n");
            return OVERFLOW;
        }
    }
    return SUCCESS;
}

/* 登录成功的主界面 */
static int ChatRoomMain(int fd, json_object *json)
{
    /* 用户名 */
    const char *username = json_object_get_string(json_object_object_get(json, "name"));
    
    /* 创建用户本地数据目录 */
    char path[strlen("./usersData") + NAME_SIZE + 1] = "./usersData";
    sprintf(path, "%s/%s", path, username);     // 拼接路径
    if(access(path, F_OK) == -1)
    {
        mkdir(path, 0777);
    }
    /* 好友列表 */
    json_object * friends = json_object_object_get(json, "friends");
    /* 群组列表 */
    json_object * groups = json_object_object_get(json, "groups");
    /* 显示好友列表和群组列表 */
    printf("a.显示好友列表\nb.显示群聊列表\ne.退出聊天室\n其他无效");
    char ch;
    scanf("%c", &ch);
    switch (ch)
    {
        case 'a':
            ChatRoomShowFriends(fd, friends,username);
            break;
        case 'b':
            ChatRoomShowGroupChat(fd, groups,username);
            break;
        case 'e':
            /* todo... */
            break;
        default:
            printf("无效操作\n");
            break;
    }
    
    return SUCCESS;
}

/* 聊天室初始化 */
int ChatRoomInit()
{
    /* 创建用户本地数据目录 */
    if(access("./usersData", F_OK) == -1)
    {
        mkdir("./usersData", 0777);
    }
    /* 初始化与服务器的连接 */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket error");
        return SOCKET_ERROR;
    }
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    int ret = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret == -1)
    {
        perror("connect error");
        return CONNECT_ERROR;
    }


    printf("欢迎使用网络聊天室\n");
    while(1)
    {
        printf("请输入要进行的功能:\na.登录\nb.注册\n其他.退出聊天室");
        char ch;
        scanf("%c", &ch);
        switch (ch)
        {
            case 'a':
                ChatRoomLogin(fd);
                break;
            case 'b':
                ChatRoomRegister(fd);
                break;
            default:
                ChatRoomExit();
                break;
        }
    }

}

/* 聊天室退出 */
int ChatRoomExit()
{
    /* todo... */
    printf("感谢您的使用\n");
    return 0;
}

/* 聊天室注册 */
int ChatRoomRegister(int sockfd)
{
    char name[NAME_SIZE] = {0};
    char password[PASSWORD_SIZE] = {0};

    printf("注册\n");
    printf("请输入账号:");
    scanf("%s", name);
    printf("请输入密码:");
    scanf("%s", password);

    /* 注册信息转化为json，发送给服务器 */
    json_object *jobj = json_object_new_object();
    json_object_object_add(jobj, "type", json_object_new_string("register"));
    json_object_object_add(jobj, "name", json_object_new_string(name));
    json_object_object_add(jobj, "password", json_object_new_string(password));
    char *json = json_object_to_json_string(jobj);

    /* 发送json */
    /*
        发送给服务器的信息：
            请求类型，账号，密码
    */
    SendJsonToServer(sockfd, json);


    /* 等待服务器响应 */
    printf("注册中 ");
    /*
        预期接收到的服务器信息：
        receipt:success/fail
        name:自己的ID
        friends:
            name:待处理消息数
        groups:
            name:待处理消息数

    */
    RecvJsonFromServer(sockfd, json);

    json_object *jreceipt = json_object_object_get(json_tokener_parse(json), "receipt");
    if (jreceipt == NULL)
    {
        printf("注册失败\n");
        json_object_put(jreceipt);
        json_object_put(jobj);
        return JSON_ERROR;
    }

    char *receipt = json_object_get_string(jreceipt);
    if (strcmp(receipt, "success") == 0)
    {
        printf("注册成功\n");
        json_object_put(jobj);
        json_object_object_del(jreceipt, "receipt");    // 删除掉多余的回执数据
        ChatRoomMain(sockfd,jreceipt);  
    }
    else
    {
        printf("注册失败\n");
        json_object_put(jreceipt);
        json_object_put(jobj);
    }

    return SUCCESS;
}

/* 聊天室登录 */
int ChatRoomLogin(int sockfd)
{
    char name[NAME_SIZE] = {0};
    char password[PASSWORD_SIZE] = {0};

    printf("登录\n");
    printf("请输入账号:");
    scanf("%s", name);
    printf("请输入密码:");
    scanf("%s", password);
    /* 登录信息转化为json，发送给服务器 */
    json_object *jobj = json_object_new_object();
    json_object_object_add(jobj, "type", json_object_new_string("login"));
    json_object_object_add(jobj, "name", json_object_new_string(name));
    json_object_object_add(jobj, "password", json_object_new_string(password));
    char *json = json_object_to_json_string(jobj);

    /*
        发送给服务器的信息：
            请求类型，账号，密码
    */
    SendJsonToServer(sockfd, json);

    /* 等待服务器响应 */
    printf("登录中 ");
    RecvJsonFromServer(sockfd, json);
    /*
        预期接收到的服务器信息：
        receipt:success/fail
        name:自己的ID
        friends:
            name:待处理消息数
        groups:
            name:待处理消息数

    */
    json_object *jreceipt = json_object_object_get(json_tokener_parse(json), "receipt");

    if (jreceipt == NULL)
    {
        printf("登录失败\n");
        json_object_put(jreceipt);
        json_object_put(jobj);
        return JSON_ERROR;
    }

    char *receipt = json_object_get_string(jreceipt);
    if (strcmp(receipt, "success") == 0)
    {
        printf("登录成功\n");
        json_object_put(jobj);
        json_object_object_del(jreceipt, "receipt");    // 删除掉多余的回执数据
        ChatRoomMain(sockfd,jreceipt);
    }
    else
    {
        printf("登录失败\n");
        json_object_put(jreceipt);
        json_object_put(jobj);
        return SUCCESS;
    }
    return SUCCESS;
}

/* 添加好友 */
int ChatRoomAddFriend(int sockfd, const char *name);

/* 显示好友 */
int ChatRoomShowFriends(int sockfd, json_object* friends, const char *username)
{

    while(1)
    {    
        printf("好友列表:\n");
        if(json_object_array_length(friends) == 0)
        {
            printf("暂无好友\n");
            return SUCCESS;
        }
        else
        {
            json_object_object_foreach(friends, key, value)
            {
                if(value != 0)
                {
                    printf("\t%s*\n", key);
                }
                else
                {
                    printf("\t%s\n", key);
                }
            }
        }
        printf("a.添加好友\nb.删除好友\nc.私聊\nd.退出\n其他.返回上一级");
        char ch;
        char name[NAME_SIZE] = {0};
        scanf("%c", &ch);
        switch (ch)
        {
            case 'a':
                printf("请输入要添加的好友:");
                scanf("%s", name);
                ChatRoomAddFriend(sockfd, name);
                memset(name, 0, NAME_SIZE);
                break;
            case 'b':
                printf("请输入要删除的好友:");
                scanf("%s", name);
                ChatRoomDelFriend(sockfd, name);
                memset(name, 0, NAME_SIZE);
                break;
            case 'c':
                printf("请输入要私聊的好友:");
                scanf("%s", name);
                ChatRoomPrivateChat(sockfd, name, friends,username);
                memset(name, 0, NAME_SIZE);
                break;
            case 'd':
                /* todo... */
                break;
            default:
                return SUCCESS;
        }
    }
    return SUCCESS;
}

/* 删除好友 */
int ChatRoomDelFriend(int sockfd, const char *name);

/* 私聊 */
int ChatRoomPrivateChat(int sockfd, const char *name, json_object *friends, const char *username)
{
    /* 创建私聊的本地聊天记录文件 */
    char path[PATH_SIZE] = "./userData";
    sprintf(path, "%s/%s/%s.txt", path, username, name);

    char content[CONTENT_SIZE] = {0};
    while(1)
    {
        printf("请输入要私聊的内容:");
        scanf("%s", content);
        /* 私聊信息转化为json，发送给服务器 */
        json_object *jobj = json_object_new_object();
        json_object_object_add(jobj, "type", json_object_new_string("private"));
        json_object_object_add(jobj, "name", json_object_new_string(name));
        json_object_object_add(jobj, "content", json_object_new_string(content));
        char *json = json_object_to_json_string(jobj);
        /*
            发送给服务器的信息：
                type：private
                name：好友名
                content：私聊内容
        */
        SendJsonToServer(sockfd, json);
    }
}

/* 接收消息 */
int ChatRoomRecvMsg(int sockfd, json_object *friends)
{
    /*
        预期接收到的服务器信息：
            type:private/group
            name:发信人
            toname:收信人
            content:消息内容
            time:发送时间
    */
}

/* 发起群聊 */
int ChatRoomAddGroupChat(int sockfd, const char *name);

/* 显示群聊列表 */
int ChatRoomShowGroupChat(int sockfd, json_object *groups, const char *username)
{
    while(1)
    {

        printf("群组列表:\n");
        if(json_object_array_length(groups) == 0)
        {
            printf("暂无群组\n");
        }
        else
        {
            json_object_object_foreach(groups, key, value)
            {
                if(value != 0)
                {
                    printf("\t%s*\n", key);
                }
                else
                {
                    printf("\t[%s]\n", key);
                }
            }
        }
    }
}

/* 群聊 */
int ChatRoomGroupChat(int sockfd, const char *name);

/* 退出群聊 */
int ChatRoomExitGroupChat(int sockfd, const char *name);
