
/* * * * * * * * * * * * * * * * * * * *
*
*      信息服务网络消息解析
*                  by Jampe
*
* * * * * * * * * * * * * * * * * * * */


#if !defined __INFONET_H__
#define __INFONET_H__


#include <time.h>


#define     INFO_DEFAULT                0       //  information return default
#define     INFO_SUCCESS                1       //  information return success
#define     INFO_FAILED                 2       //  information return failure

#define     INFO_LOGIN                  1000    //  login request
#define     INFO_REQUEST_ACCOUNT        1010    //  fetch account information
#define     INFO_EXCEPTION_SERVICE      1900    //  service refused
#define     INFO_REQUEST_STORE          2001    //  fetch the shop list
#define     INFO_REQUEST_AFFICHE        2002    //  fetch the announcement list
#define     INFO_STORE_BUY              2011    //  buy an item
#define     INFO_EXCHANGE_MONEY         2012    //  exchange currency
#define     INFO_REGISTER_VIP           2013    //  buy a VIP
#define     INFO_REQUEST_HISTORY        2021    //  fetch the trade records
#define     INFO_SND_GM_MAIL            4500    //  send a GM mail
#define     INFO_RCV_GM_MAIL            4501    //  collect the GM reply


#define     INFO_MAN_DEFAULT            INFO_DEFAULT    //  management default value
#define     INFO_MAN_SUCCESS            10              //  management message returned successfully
#define     INFO_MAN_FAILED             11              //  management message returned failure

#define     INFO_MAN_ADD                0               //  management add
#define     INFO_MAN_MODIFY             1               //  management modify
#define     INFO_MAN_DEL                2               //  management delete

#define     INFO_MAN_ITEM               6000            //  goods management
#define     INFO_MAN_CLASS              6001            //  category management
#define     INFO_MAN_AFFICHE            6002            //  announcement management
#define     INFO_MAN_ALL_ITEM           6003            //  manage every good that is not yet listed
#define     INFO_GET_GM_MAIL            6500            //  collect GM mail
#define     INFO_RPY_GM_MAIL            6501            //  reply to GM mail
#define     INFO_MAIL_QUEUE             6502            //  number of mails

#define     INFO_MAN_VIP                7000            //  VIP management
#define     INFO_MAN_RATE               7001            //  modify the exchange rate



#define     INFO_TYPE_HISTORY           0               //  trade records
#define     INFO_TYPE_EXCHANGE          1               //  exchange records
#define     INFO_TYPE_VIP               2               //  VIP purchase records

#define     INFO_CHECK_LIVE             0               //  check online

typedef unsigned char       _byte;


//  account information
typedef struct _RoleInfo_
{
    long        actID;          //	account number
    char        actName[21];    //	account name
    long        chaID;          //	character number
    char        chaName[17];    //	character name
    long        moBean;         //	MoDou count
    long        rplMoney;       //	token count
    long        vip;            //  VIP kind (20-50); 0 means not a VIP
}RoleInfo, *pRoleInfo;



//  trade record
typedef struct _HistoryInfo_
{
    time_t      tradeTime;      //	trade time
    long        comID;          //	goods number
    char        comName[21];    //	goods name
    int         tradeResult;    //  result        0 success, 1 wrong goods price, 2 insufficient balance, 3 charge failed
    long        tradeMoney;     //	trade amount
}HistoryInfo, *pHistoryInfo;



//  announcement information
typedef struct _AfficheInfo_
{
    long        affiID;         //	announcement number
    char        affiTitle[64];  //	announcement title
    char        comID[33];      //	linked goods number
    time_t      affiTime;       //  publish time
}AfficheInfo, *pAfficheInfo;



//  category information
typedef struct _ClassInfo_
{
    short        clsID;        //	category number
    char         clsName[17];   //	category name
    short        parentID;      //	parent category number
}ClassInfo, *pClassInfo;



//  item information
typedef struct _ItemInfo_
{
    short       itemID;         //	item number
    _byte       itemNum;        //	item count
    _byte       itemFlute;      //	slot count
    short       itemAttrID[5];  //	five attribute numbers
    short       itemAttrVal[5]; //	five attribute values
}ItemInfo, *pItemInfo;



//  goods information
typedef struct _StoreInfo_
{
    long        comID;          //	goods number
    char        comName[21];    //	goods name
    long        comClass;       //	goods category
    time_t      comTime;        //	goods listing time
    long        comPrice;       //	price
    char        comRemark[129]; //	goods remark
    int         comNumber;      //  remaining number of the goods. -1 unlimited, 0 delisted
    time_t      comExpire;      //  goods expiry time.
    _byte       itemNum;        //  item count
    _byte       isHot;          //  whether it is on hot sale
    time_t      beginTime;      //  listing time
}StoreInfo, *pStoreInfo;



//  goods structure
typedef struct _StoreStruct_
{
    StoreInfo   store_head;     //  goods information header
    pItemInfo   item_body;      //  item information body
}StoreStruct, *pStoreStruct;



//  login information
typedef struct _LoginInfo_
{
    _byte       id;             //  server number
    char        ip[16];         //  login IP
    char        pwd[33];        //  verification code
    _byte       ban;            //  whether blacklisted
}LoginInfo, *pLoginInfo;


//  VIP information
typedef struct _VIPInfo_
{
    int         vipID;          //  VIP number (the type, ranging between 20 and 50)
    int         vipPrice;       //  VIP price
}VIPInfo, *pVIPInfo;


//  Mail information
typedef struct _MailInfo_
{
    long id;                    //  mail number
    char title[33];				//	mail title
    char description[513];		//	mail body
    long actID;				    //	sender account ID
    char actName[64];			//	sender account name
    long chaID;				    //	sender character ID
    char chaName[64];			//	sender character name
    time_t sendTime;			//	send time
}MailInfo, *pMailInfo;


// message header
typedef struct _NetMessageHead_
{
    unsigned char msgChk[4];    //  the valid network message identifiers are 0x4A, 0x4D, 0x50, 0x53
    long msgID;                 // message number
    long subID;                 // message sub-number
    long long msgOrder;         //  message handshake
    long msgSection;            //  shard number
    long msgExtend;             //  extension field
    long msgSize;               //  message body length
    long msgEncSize;            //  length after encryption
}NetMessageHead, *pNetMessageHead;



//  network message
typedef struct _NetMessage_
{
    NetMessageHead      msgHead;    // message header
    void*               msgBody;    // message body
}NetMessage, *pNetMessage;


#define PNM_SUCCESS             0           //  decoded successfully
#define PNM_FAILED              1           //  wrong protocol
#define PNM_UNKNOWN             2           //  unknown protocol
#define PNM_EXCEPTION           3           //  exception


//  gets the message header
extern int GetMessageHead(pNetMessageHead mh, unsigned char* msg, int size);

//  parses the message
extern int PeekNetMessage(pNetMessage* nm, unsigned char** buf, long& bufsize, void** tmpbuf, long& tmpsize);

//  frees the message
extern void FreeNetMessage(pNetMessage msg);

//  builds the message
extern bool BuildNetMessage(pNetMessage nm, long msgID, long subID, long extend, long section, unsigned char* body, long size);

//  gets the key
extern _byte* GetInfoKey();

//  gets the password (for GameServer use only)
extern _byte* GetPassword();

class InfoThread;
//  client interface
class InfoNetBase
{
public:
    InfoNetBase();
    virtual ~InfoNetBase();
    virtual bool RunInfoService(const char* ip, short port);  //  runs the client service
    virtual bool StopInfoService(); //  stops the client service
    virtual bool PostInfoSend(long msgID, long subID, long extend, long section, unsigned char* body, long size, bool resnd = false);   //  sends a message
    virtual bool PostInfoSend(pNetMessage msg, bool resnd = false);     //  sends a message
    virtual bool IsConnect();

public:
    virtual void OnConnect(bool result) = 0;            //  responds on connect
    virtual void OnNetMessage(pNetMessage msg) = 0;     //  user interface protocol; msg is freed by the user manually.
    virtual void OnResend(pNetMessage msg) = 0;         //  after the resend completes, msg is freed automatically.
    virtual void OnDisconnect() = 0;        //  responds on disconnect

protected:
    virtual bool Connect();
    virtual bool Disconnect();      //  disconnect
    virtual bool PostInfoRecv();    //  posts a receive
    friend class InfoThread;

protected:
    void*       m_client;
    void*       m_list;
    int         m_state;
    char        m_ip[260];
    short       m_port;
};

#ifndef __INFO_NET__
    #pragma comment(lib, "InfoNet.lib")
#endif

#endif
