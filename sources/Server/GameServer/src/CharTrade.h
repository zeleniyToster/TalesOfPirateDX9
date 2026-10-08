// CharTrade.h Created by knight-gongjian 2004.12.7.
//---------------------------------------------------------
#pragma once

#ifndef _CHARTRADE_H_
#define _CHARTRADE_H_


#include "Character.h"
//---------------------------------------------------------
_DBC_USING

namespace mission
{
	typedef struct _TRADE_DATA
	{
		struct TRADE_ITEM_DATA
		{
			BYTE byType : 2;
			BYTE byIndex: 6;
			BYTE byCount;
			USHORT sItemID;			
		};
		TRADE_ITEM_DATA ItemArray[ROLE_MAXNUM_TRADEDATA];
		BYTE  byItemCount;						// item count
		DWORD dwMoney;							// money involved in the trade
		DWORD dwIMP;

	} TRADE_DATA, *PTRADE_DATA;

	class CTradeData : public dbc::PreAllocStru
	{
	public:
		CTradeData(dbc::uLong lSize);
		virtual ~CTradeData();

		void Clear()
		{
			pRequest	= NULL;
			pAccept		= NULL;
			byValue		= 0;

			memset( &ReqTradeData, 0, sizeof(TRADE_DATA) );
			memset( &AcpTradeData, 0, sizeof(TRADE_DATA) );
		}

		CCharacter *pRequest, *pAccept;
		union
		{
			struct
			{
				BYTE  bTradeStart : 1;		// whether the trade has started
				BYTE  bReqTrade : 1;		// the requesting side has confirmed the trade data
				BYTE  bAcpTrade : 1;		// the accepting side has confirmed the trade data
				BYTE  bReqOk : 1;			// the requesting side has confirmed the trade operation
				BYTE  bAcpOk : 1;			// the accepting side has confirmed the trade operation
				BYTE  byParam : 3;			// reserved
			};
			BYTE byValue;
		};
		//USHORT sxPos, syPos;		// 交易地点信息
		TRADE_DATA ReqTradeData;	// the requesting side's trade data
		TRADE_DATA AcpTradeData;	// the accepting side's trade data
		
		DWORD dwTradeTime;			// trade timer: if the trade runs past the allotted time, the system cancels the trade and recycles the buffer
	};

	class CTradeSystem
	{
	public:
		CTradeSystem();
		~CTradeSystem();

		// trade operation
		BOOL Request( BYTE byType, CCharacter& character, DWORD dwAcceptID );
		BOOL Accept( BYTE byType, CCharacter& character,  DWORD dwRequestID );
		BOOL Cancel( BYTE byType, CCharacter& character,  DWORD dwCharID );
		
		// clears the trade information when the character goes offline
		BOOL Clear( BYTE byType, CCharacter& character );

		// confirms the trade information and operation
		BOOL ValidateItemData( BYTE byType, CCharacter& character, DWORD dwCharID );
		BOOL ValidateTrade( BYTE byType, CCharacter& character, DWORD dwCharID );

		// places or takes an item into or out of the trade slot
		BOOL AddItem( BYTE byType, CCharacter& character, DWORD dwCharID, BYTE byOpType, BYTE byIndex, BYTE byItemIndex, BYTE byCount );
		BOOL AddMoney( BYTE byType, CCharacter& charactar, DWORD dwCharID, BYTE byOpType, DWORD dwMoney );
		BOOL AddIMP(BYTE byType, CCharacter& charactar, DWORD dwCharID, BYTE byOpType, DWORD dwMoney);
		BOOL IsTradeDist( CCharacter& Char1, CCharacter& Char2, DWORD dwDist );
	private:
		void ResetItemState( CCharacter& character, CTradeData& TradeData );

	};

#pragma pack( push, before_InfoNet )
#pragma pack( 8 )

    // shop order information
    struct SOrderData
    {        
        long long	lOrderID;  // order number
        long		lComID;    // goods ID
		long		lNum;	   // quantity
        long		ChaID;	   // character database ID
		long		lRecDBID;
		char		ChaName[defENTITY_NAME_LEN];
		DWORD		dwTickCount;
    };

	// goods information
	struct SItemData
	{
		SItemData():pItemArray(NULL)
		{
		}

		SItemData(const SItemData &rItem)
		{
			memcpy(&store_head, &rItem.store_head, sizeof(StoreInfo));
			int nItemNum = store_head.itemNum;
			if(nItemNum > 0)
			{
				pItemArray = new ItemInfo[nItemNum];
				memcpy(pItemArray, rItem.pItemArray, nItemNum * sizeof(ItemInfo));
			}
			else
				pItemArray = NULL;
		}

		~SItemData()
		{
			if(pItemArray)
			{
				delete [] pItemArray;
				pItemArray = NULL;
			}
		}

		SItemData& operator=(const SItemData &rItem)
		{
			memcpy(&store_head, &rItem.store_head, sizeof(StoreInfo));
			int nItemNum = store_head.itemNum;
			if(nItemNum > 0)
			{
				if(pItemArray)
				{
					delete [] pItemArray;
				}
				pItemArray = new ItemInfo[nItemNum];
				memcpy(pItemArray, rItem.pItemArray, nItemNum * sizeof(ItemInfo));
			}
			else
				pItemArray = NULL;
			return *this;
		}

		StoreInfo   store_head;       //  goods information header
		ItemInfo	*pItemArray;      //  item information body
	};

    // shop system
    class CStoreSystem
    {
    public:
        CStoreSystem();
        ~CStoreSystem();

        // user buys an item
        BOOL Request( CCharacter *pCha, long lComID );
		BOOL Accept( long long lOrderID, RoleInfo *ChaInfo );
		BOOL Accept( CCharacter *pCha, long lComID );
		BOOL Cancel( long long lOrderID );

		BOOL RequestVIP(CCharacter *pCha, short sVipID, short sMonth);
		BOOL AcceptVIP(long long lOrderID, RoleInfo *ChaInfo, DWORD dwVipParam);
		BOOL CancelVIP(long long lOrderID);

		// user fetches the goods list
		BOOL RequestItemList(CCharacter *pCha, long lClsID, short sPage, short sNum);

		// user asks to exchange currency
		BOOL RequestChange(CCharacter *pCha, long lNum);
		BOOL AcceptChange(long long lOrderID, RoleInfo *ChaInfo);
		BOOL CancelChange(long long lOrderID);

		// looks up character information
		BOOL RequestRoleInfo(CCharacter *pCha);
		BOOL AcceptRoleInfo(long long lOrderID, RoleInfo *ChaInfo);
		BOOL CancelRoleInfo(long long lOrderID);

		// looks up trade records
		BOOL RequestRecord(CCharacter *pCha, long lNum);
		BOOL AcceptRecord(long long lOrderID, HistoryInfo *pRecord);
		BOOL CancelRecord(long long lOrderID);

		BOOL RequestGMSend(CCharacter *pCha, cChar *szTitle, cChar *szContent);
		BOOL AcceptGMSend(long long lOrderID, long lMailID);
		BOOL CancelGMSend(long long lOrderID);

		BOOL RequestGMRecv(CCharacter *pCha);
		BOOL AcceptGMRecv(long long lOrderID, MailInfo *pMi);
		BOOL CancelGMRecv(long long lOrderID);

		// user opens the shop
		BOOL Open(CCharacter *pCha, long vip);

		// shop list
		BOOL GetItemList();
		BOOL GetAfficheList();

		BOOL SetItemList(void *pItemList, long lNum);
		BOOL ClearItemList();

		BOOL SetItemClass(ClassInfo *pClassList, long lNum);
		BOOL ClearItemClass();

		BOOL SetAfficheList(AfficheInfo *pAfficheList, long lNum);
		BOOL ClearAfficheList();

		void InValid() { m_bValid = false; }
		void SetValid() { m_bValid = true; }
		BOOL IsValid() { return m_bValid; }

        void Run( DWORD dwCurTime, DWORD dwIntervalTime, DWORD dwOrderTime );    // handles the order list
        
    private:
		// looks up goods
		long		GetClassId(long lComID);
		cChar		*GetClassName(long lClsID);
		SItemData	*GetItemData(long lComID);
		BOOL		DelItemData(long lComID);

		// order operation
		BOOL		PushOrder(CCharacter *pCha, long long lOrderID, long lComID, long lNum);
		SOrderData	PopOrder(long long lOrderID);
		BOOL		HasOrder(long long lOrderID);

		//map<long long, SOrderData>		m_OrderList;		//订单列表
		std::vector<SOrderData>				m_OrderList;
		std::map<long, long>					m_ItemSearchList;	// mapping between goods ID and category
		std::map< long, std::vector<SItemData> >	m_ItemList;			// goods list
		std::vector<ClassInfo>				m_ItemClass;		// goods categories
		std::vector<AfficheInfo>				m_AfficheList;		// announcement list
		BOOL							m_bValid;
        
    };

#pragma pack( pop, before_InfoNet )

}

extern mission::CTradeSystem g_TradeSystem;
extern mission::CStoreSystem g_StoreSystem;

//---------------------------------------------------------

#endif // _CHARTRADE_H_