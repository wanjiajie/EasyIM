/** 
 * ChatSession.h
 **/
#pragma once 
#include "../net/Buffer.h"
#include "../net/TimerId.h"
#include "TcpSession.h"

using namespace net;

struct OnlineUserInfo {
    int32_t     userid;
    std::string username;
    std::string nickname;
    std::string password;
    int32_t     clienttype;     //客户�?类型,0�?知，pc=1，android=2
    int32_t     status;         //在线状�? 0离线 1在线 2忙�?? 3离开 4隐身
};

/**
 * 聊天会话�?
 */
class ChatSession : public TcpSession {
public:
    ChatSession(const std::shared_ptr<TcpConnection>& conn,int sessionid);
    virtual ~ChatSession();

    ChatSession(const ChatSession& rhs) = delete;
    ChatSession& operator = (const ChatSession& rhs) = delete;

    void onRead(const std::shared_ptr<TcpConnection>& conn,Buffer* pBuffer,Timestamp receivTime) ;

    int32_t getSessionId() {
        return m_id;
    }

    int32_t getUserId() {
        return m_userinfo.userid;
    }

    std::string getUsername() {
        return m_userinfo.username;
    }

    std::string getNickname() {
        return m_userinfo.password;
    }

    std::string getPassword() {
        return m_userinfo.password;
    }

    int32_t getClientType() {
        return m_userinfo.clienttype;
    }

    int32_t getUserStatus() {
        return m_userinfo.status;
    }

    /**
     *@param type ȡֵ�� 1 �û����ߣ� 2 �û����ߣ� 3 �����ǳơ�ͷ��ǩ������Ϣ����
     */
    void sendUserStatusChangeMsg(int32_t userid, int type, int status = 0);

    //��SessionʧЧ�����ڱ������ߵ��û�Session
    void makeSessionInvalid();
    bool isSessionValid();

    void enableHeartbeatCheck();
    void disableHeartbeatCheck();

    //�������������ָ��ʱ���ڣ�30s��δ�յ����ݰ��������Ͽ���ͻ��˵�����
    void checkHeartbeat(const std::shared_ptr<TcpConnection>& conn);
private:
    bool process(const std::shared_ptr<TcpConnection>& conn, const char* inbuf, size_t buflength);
    
    void onHeartbeatResponse(const std::shared_ptr<TcpConnection>& conn);
    void onRegisterResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onLoginResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onGetFriendListResponse(const std::shared_ptr<TcpConnection>& conn);
    void onFindUserResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onChangeUserStatusResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onOperateFriendResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onAddGroupResponse(int32_t groupId, const std::shared_ptr<TcpConnection>& conn);
    void onUpdateUserInfoResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onModifyPasswordResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onCreateGroupResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onGetGroupMembersResponse(const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onChatResponse(int32_t targetid, const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    void onMultiChatResponse(const std::string& targets, const std::string& data, const std::shared_ptr<TcpConnection>& conn);
    //void onScreenshotResponse(int32_t targetid, const std::string& bmpHeader, const std::string& bmpData, const std::shared_ptr<TcpConnection>& conn);
    void onUpdateTeamInfoResponse(int32_t operationType, const std::string& newTeamName, const std::string& oldTeamName, const std::shared_ptr<TcpConnection>& conn);
    void onModifyMarknameResponse(int32_t friendid, const std::string& newmarkname, const std::shared_ptr<TcpConnection>& conn);
    void onMoveFriendToOtherTeamResponse(int32_t friendid, const std::string& newteamname, const std::string& oldteamname, const std::shared_ptr<TcpConnection>& conn);

    void deleteFriend(const std::shared_ptr<TcpConnection>& conn, int32_t friendid);

    //�����û�������Ϣ��װӦ����ͻ��˵ĺ����б���Ϣ
    void makeUpFriendListInfo(std::string& friendinfo, const std::shared_ptr<TcpConnection>& conn);

    //��������Ϣ�ı���ʱ��ĳɷ�����ʱ�䣬�޸ĳɹ�����true,ʧ�ܷ���false��
    bool modifyChatMsgLocalTimeToServerTime(const std::string& chatInputJson, std::string& chatOutputJson);

    int32_t             m_id;                   //session ID
    OnlineUserInfo      m_userinfo;
    int32_t             m_seq;                  //��ǰSession���ݰ����к�  
    bool                m_isLogin;              //��ǰSession��Ӧ���û��Ƿ��ѵ�¼
    time_t              m_lastPackageTime;      //��һ���շ�����ʱ��
    TimerId             m_checkOnlineTimerId;  //����������Ķ�ʱ��
};