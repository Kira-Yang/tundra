/**
    For conditions of distribution and use, see copyright notice in LICENSE

    @file   TundraProtocolModuleFwd.h
    @brief  Forward declarations and type defines for commonly used TundraProtocolModule plugin classes. */

#pragma once

#include "CoreTypes.h"

#include <kNetFwd.h>

#include <map>

class KristalliProtocolModule;

namespace TundraLogic
{
    class TundraLogicModule;
    class Client;
    class Server;
    class SyncManager;
}

using TundraLogic::TundraLogicModule;

class UserConnection;
typedef shared_ptr<UserConnection> UserConnectionPtr;
Q_DECLARE_METATYPE(UserConnectionPtr)
typedef weak_ptr<UserConnection> UserConnectionWeakPtr;
typedef std::list<UserConnectionPtr> UserConnectionList;
Q_DECLARE_METATYPE(UserConnectionList)

class SceneSyncState;
struct EntitySyncState;
struct ComponentSyncState;
struct UserConnectedResponseData;

typedef std::map<QString, QString> LoginPropertyMap; ///< propertyName-propertyValue map of login properties.
Q_DECLARE_METATYPE(LoginPropertyMap)

struct MsgLogin;
struct MsgLoginReply;
struct MsgClientJoined;
struct MsgClientLeft;
struct MsgAssetDiscovery;
struct MsgAssetDeleted;
struct MsgEntityAction;
struct MsgCameraOrientationRequest;
