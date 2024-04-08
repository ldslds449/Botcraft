#include "botcraft/AI/Tasks/PathfindingTask.hpp"
#include "botcraft/AI/Blackboard.hpp"
#include "botcraft/AI/Tasks/EntitiesTasks.hpp"
#include "botcraft/AI/BehaviourClient.hpp"

#include "botcraft/Network/NetworkManager.hpp"
#include "botcraft/Game/Entities/EntityManager.hpp"
#include "botcraft/Game/Entities/entities/Entity.hpp"
#include "botcraft/Game/Entities/LocalPlayer.hpp"

#include "botcraft/Utilities/Logger.hpp"

using namespace ProtocolCraft;

namespace Botcraft
{
    Status InteractEntityImpl(BehaviourClient& client, const int entity_id, const Hand hand, const bool swing, const bool auto_move)
    {
        std::shared_ptr<EntityManager> entity_manager = client.GetEntityManager();

        std::shared_ptr<Entity> entity = entity_manager->GetEntity(entity_id);

        if (!entity)
        {
            LOG_WARNING("Can't interact with unexisting entity: " << entity_id);
            return Status::Failure;
        }

        std::shared_ptr<LocalPlayer> local_player = entity_manager->GetLocalPlayer();

        Vector3<double> entity_position = entity->GetPosition();
        Vector3<double> position = local_player->GetPosition();

        while (position.SqrDist(entity_position) > 16.0)
        {
            if (auto_move) 
            {
                // Go in range
                if (GoTo(client, entity_position, 3, 0) == Status::Failure)
                {
                    return Status::Failure;
                }
            }
            else
            {
                LOG_WARNING("Target is too far away.");
                return Status::Failure;
            }

            entity_position = entity->GetPosition();
            position = local_player->GetPosition();
        }

        local_player->LookAt(entity_position + Vector3<double>(0, entity->GetHeight() / 2.0, 0), true);

        std::shared_ptr<NetworkManager> network_manager = client.GetNetworkManager();
        std::shared_ptr<ServerboundInteractPacket> msg_interact = std::make_shared<ServerboundInteractPacket>();
        msg_interact->SetEntityId(entity_id);
        msg_interact->SetAction(0);
        msg_interact->SetHand(static_cast<int>(hand)); 
#if PROTOCOL_VERSION > 722 /* > 1.15.2 */
        msg_interact->SetUsingSecondaryAction(false);
#endif
        network_manager->Send(msg_interact);
        
        if (swing)
        {
            std::shared_ptr<ServerboundSwingPacket> msg_swing = std::make_shared<ServerboundSwingPacket>();
            msg_swing->SetHand(static_cast<int>(hand));

            network_manager->Send(msg_swing);
        }

        return Status::Success;
    }

    Status InteractEntity(BehaviourClient& client, const int entity_id, const Hand hand, const bool swing, const bool auto_move)
    {
        constexpr std::array variable_names = {
            "InteractEntity.entity_id",
            "InteractEntity.hand",
            "InteractEntity.swing",
            "InteractEntity.auto_move"
        };

        Blackboard& blackboard = client.GetBlackboard();

        blackboard.Set<int>(variable_names[0], entity_id);
        blackboard.Set<Hand>(variable_names[1], hand);
        blackboard.Set<bool>(variable_names[2], swing);
        blackboard.Set<bool>(variable_names[3], auto_move);

        return InteractEntityImpl(client, entity_id, hand, swing, auto_move);
    }

    Status InteractEntityBlackboard(BehaviourClient& client)
    {
        constexpr std::array variable_names = {
            "InteractEntity.entity_id",
            "InteractEntity.hand",
            "InteractEntity.swing",
            "InteractEntity.auto_move"
        };

        Blackboard& blackboard = client.GetBlackboard();

        // Mandatory
        const int entity_id = blackboard.Get<int>(variable_names[0]);

        // Optional
        const Hand hand = blackboard.Get<Hand>(variable_names[1], Hand::Main);
        const bool swing = blackboard.Get<bool>(variable_names[2], false);
        const bool auto_move = blackboard.Get<bool>(variable_names[3], false);

        return InteractEntityImpl(client, entity_id, hand, swing, auto_move);
    }
}
