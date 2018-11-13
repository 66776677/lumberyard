/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

// include the required headers
#include "ManipulatorCallbacks.h"

#include <MCore/Source/StringConversions.h>
#include <EMotionFX/Source/EMotionFXManager.h>
#include <EMotionFX/Source/ActorManager.h>

namespace EMStudio
{

    void TranslateManipulatorCallback::Update(const AZ::Vector3& value)
    {
        ManipulatorCallback::Update(value);

        // update the position, if actorinstance is still valid
        uint32 actorInstanceID = EMotionFX::GetActorManager().FindActorInstanceIndex(mActorInstance);
        if (actorInstanceID != MCORE_INVALIDINDEX32)
        {
            mActorInstance->SetLocalPosition(value);
        }
    }

    void TranslateManipulatorCallback::UpdateOldValues()
    {
        // update the rotation, if actorinstance is still valid
        uint32 actorInstanceID = EMotionFX::GetActorManager().FindActorInstanceIndex(mActorInstance);
        if (actorInstanceID != MCORE_INVALIDINDEX32)
        {
            mOldValueVec = mActorInstance->GetLocalPosition();
        }
    }

    void TranslateManipulatorCallback::ApplyTransformation()
    {
        EMotionFX::ActorInstance* actorInstance = GetCommandManager()->GetCurrentSelection().GetSingleActorInstance();
        if (actorInstance)
        {
            AZ::Vector3 newPos = actorInstance->GetLocalPosition();
            actorInstance->SetLocalPosition(mOldValueVec);

            if (AZ::Vector3(mOldValueVec - newPos).GetLength() >= MCore::Math::epsilon)
            {
                AZStd::string outResult;
                if (GetCommandManager()->ExecuteCommand(
                    AZStd::string::format("AdjustActorInstance -actorInstanceID %i -pos %s", actorInstance->GetID(), AZStd::to_string(newPos).c_str()),
                    outResult) == false)
                {
                    MCore::LogError(outResult.c_str());
                }
                else
                {
                    UpdateOldValues();
                }
            }
        }
    }

    void RotateManipulatorCallback::Update(const MCore::Quaternion& value)
    {
        // update the rotation, if actorinstance is still valid
        uint32 actorInstanceID = EMotionFX::GetActorManager().FindActorInstanceIndex(mActorInstance);
        if (actorInstanceID != MCORE_INVALIDINDEX32)
        {
            // temporarily update the actor instance
            mActorInstance->SetLocalRotation((value * mActorInstance->GetLocalRotation()).Normalize());

            // update the callback parent
            ManipulatorCallback::Update(mActorInstance->GetLocalRotation());
        }
    }

    void RotateManipulatorCallback::UpdateOldValues()
    {
        // update the rotation, if actorinstance is still valid
        uint32 actorInstanceID = EMotionFX::GetActorManager().FindActorInstanceIndex(mActorInstance);
        if (actorInstanceID != MCORE_INVALIDINDEX32)
        {
            mOldValueQuat = mActorInstance->GetLocalRotation();
        }
    }

    void RotateManipulatorCallback::ApplyTransformation()
    {
        EMotionFX::ActorInstance* actorInstance = GetCommandManager()->GetCurrentSelection().GetSingleActorInstance();
        if (actorInstance)
        {
            MCore::Quaternion newRot = actorInstance->GetLocalRotation();
            actorInstance->SetLocalRotation(mOldValueQuat);

            const float dot = newRot.Dot(mOldValueQuat);
            if (dot < 1.0f - MCore::Math::epsilon && dot > -1.0f + MCore::Math::epsilon)
            {
                AZStd::string outResult;
                if (GetCommandManager()->ExecuteCommand(
                    AZStd::string::format("AdjustActorInstance -actorInstanceID %i -rot \"%s\"",
                        actorInstance->GetID(),
                        AZStd::to_string(AZ::Vector4(newRot.x, newRot.y, newRot.z, newRot.w)).c_str()).c_str(),
                    outResult) == false)
                {
                    MCore::LogError(outResult.c_str());
                }
                else
                {
                    UpdateOldValues();
                }
            }
        }
    }

    AZ::Vector3 ScaleManipulatorCallback::GetCurrValueVec()
    {
        uint32 actorInstanceID = EMotionFX::GetActorManager().FindActorInstanceIndex(mActorInstance);
        if (actorInstanceID != MCORE_INVALIDINDEX32)
        {
            return mActorInstance->GetLocalScale();
        }
        else
        {
            return AZ::Vector3(1.0f, 1.0f, 1.0f);
        }
    }

    void ScaleManipulatorCallback::Update(const AZ::Vector3& value)
    {
        // update the position, if actorinstance is still valid
        uint32 actorInstanceID = EMotionFX::GetActorManager().FindActorInstanceIndex(mActorInstance);
        if (actorInstanceID != MCORE_INVALIDINDEX32)
        {
            float minScale = 0.001f;
            AZ::Vector3 scale = AZ::Vector3(
                MCore::Max(float(mOldValueVec.GetX() * value.GetX()), minScale),
                MCore::Max(float(mOldValueVec.GetY() * value.GetY()), minScale),
                MCore::Max(float(mOldValueVec.GetZ() * value.GetZ()), minScale));
            mActorInstance->SetLocalScale(scale);

            // update the callback
            ManipulatorCallback::Update(scale);
        }
    }

    void ScaleManipulatorCallback::UpdateOldValues()
    {
        // update the rotation, if actorinstance is still valid
        uint32 actorInstanceID = EMotionFX::GetActorManager().FindActorInstanceIndex(mActorInstance);
        if (actorInstanceID != MCORE_INVALIDINDEX32)
        {
            mOldValueVec = mActorInstance->GetLocalScale();
        }
    }

    void ScaleManipulatorCallback::ApplyTransformation()
    {
        EMotionFX::ActorInstance* actorInstance = GetCommandManager()->GetCurrentSelection().GetSingleActorInstance();
        if (actorInstance)
        {
            AZ::Vector3 newScale = actorInstance->GetLocalScale();
            actorInstance->SetLocalScale(mOldValueVec);

            if (AZ::Vector3(mOldValueVec - newScale).GetLength() >= MCore::Math::epsilon)
            {
                AZStd::string outResult;
                if (GetCommandManager()->ExecuteCommand(
                    AZStd::string::format("AdjustActorInstance -actorInstanceID %i -scale %s", actorInstance->GetID(),
                        AZStd::to_string(newScale).c_str()).c_str(),
                    outResult) == false)
                {
                    MCore::LogError(outResult.c_str());
                }
                else
                {
                    UpdateOldValues();
                }
            }
        }
    }
} // namespace EMStudio