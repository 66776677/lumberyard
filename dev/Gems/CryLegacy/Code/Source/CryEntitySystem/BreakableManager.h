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
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#ifndef CRYINCLUDE_CRYENTITYSYSTEM_BREAKABLEMANAGER_H
#define CRYINCLUDE_CRYENTITYSYSTEM_BREAKABLEMANAGER_H
#pragma once

#if ENABLE_CRY_PHYSICS

#include "IBreakableManager.h"

class CEntitySystem;
struct GeomRef;
//////////////////////////////////////////////////////////////////////////
//
// BreakableManager manager handles all the code for breaking/destroying entity geometry.
//
//////////////////////////////////////////////////////////////////////////
class CBreakableManager
    : public IBreakableManager
{
public:
    CBreakableManager(CEntitySystem* pEntitySystem);

    // actual breaking function.
    void BreakIntoPieces(GeomRef& geoOrig, const Matrix34& srcObjTM,
        IStatObj* pPiecesObj, const Matrix34& piecesObjTM,
        BreakageParams const& Breakage, int nMatLayers);

    //////////////////////////////////////////////////////////////////////////
    // IBreakableManager implementation
    //////////////////////////////////////////////////////////////////////////
    virtual void BreakIntoPieces(IEntity* pEntity, int nOrigSlot, int nPiecesSlot, BreakageParams const& Breakage);

    // Breakable Plane
    virtual EProcessImpactResult ProcessPlaneImpact(const SProcessImpactIn& in, SProcessImpactOut& out);
    virtual void ExtractPlaneMeshIsland(const SExtractMeshIslandIn& in, SExtractMeshIslandOut& out);
    virtual void GetPlaneMemoryStatistics(void* pPlaneRaw, ICrySizer* pSizer);
    virtual bool IsGeometryBreakable(IPhysicalEntity* pEntity, IStatObj* pStatObj, _smart_ptr<IMaterial> pMaterial);

    // Check if this stat object can shatter.
    // Check if its materials support shattering.
    virtual bool CanShatter(IStatObj* pStatObj);
    virtual bool CanShatterEntity(IEntity* pEntity, int nSlot = -1);

    // Attach the effect & params specified by material of object in slot.
    virtual void AttachSurfaceEffect(IEntity* pEntity, int nSlot, const char* sType, SpawnParams const& paramsIn, uint uEmitterFlags = 0);
    virtual void CreateSurfaceEffect(IStatObj* pStatObj, const Matrix34& tm, const char* sType);
    //////////////////////////////////////////////////////////////////////////

    bool CanShatterRenderMesh(IRenderMesh* pMesh, _smart_ptr<IMaterial> pMtl);

    virtual void AddBreakEventListener(IBreakEventListener* pListener);
    virtual void RemoveBreakEventListener(IBreakEventListener* pListener);

    virtual void EntityDrawSlot(IEntity* pEntity, int32 slot, int32 flags);

    virtual ISurfaceType* GetFirstSurfaceType(IStatObj* pStatObj);
    virtual ISurfaceType* GetFirstSurfaceType(ICharacterInstance* pCharacter);

    virtual void ReplayRemoveSubPartsEvent(const EventPhysRemoveEntityParts* pRemoveEvent);

    void HandlePhysicsCreateEntityPartEvent(const EventPhysCreateEntityPart* pCreateEvent);
    void HandlePhysicsRemoveSubPartsEvent(const EventPhysRemoveEntityParts* pRemoveEvent);
    void HandlePhysicsRevealSubPartEvent(const EventPhysRevealEntityPart* pRevealEvent);
    int  HandlePhysics_UpdateMeshEvent(const EventPhysUpdateMesh* pEvent);

    void FakePhysicsEvent(EventPhys* pEvent);

    // Resets broken objects
    void ResetBrokenObjects();

    // Freeze/unfreeze render node.
    void FreezeRenderNode(IRenderNode* pRenderNode, bool bEnable);

    void CreateObjectAsParticles(IStatObj* pStatObj, IPhysicalEntity* pPhysEnt, IBreakableManager::SCreateParams& createParams);
    virtual IEntity* CreateObjectAsEntity(IStatObj* pStatObj, IPhysicalEntity* pPhysEnt, IPhysicalEntity* pSrcPhysEnt, IBreakableManager::SCreateParams& createParams, bool bCreateSubstProxy = false);
    bool CheckForPieces(IStatObj* pStatObjSrc, IStatObj::SSubObject* pSubObj, const Matrix34& worldTM, int nMatLayers, IPhysicalEntity* pPhysEnt);
    IStatObj::SSubObject* CheckSubObjBreak(IStatObj* pStatObj, IStatObj::SSubObject* pSubObj, const EventPhysCreateEntityPart* epcep);

    virtual const IBreakableManager::SBrokenObjRec* GetPartBrokenObjects(int& brokenObjectCount) { brokenObjectCount = m_brokenObjs.size(); return &m_brokenObjs[0]; }

    virtual void GetBrokenObjectIndicesForCloning(int32* pPartRemovalIndices, int32& iNumPartRemovalIndices,
        int32* pOutIndiciesForCloning, int32& iNumEntitiesForCloning,
        const EventPhysRemoveEntityParts* BreakEvents);
    virtual void ClonePartRemovedEntitiesByIndex(int32* pBrokenObjectIndices, int32 iNumBrokenObjectIndices,
        EntityId* pOutClonedEntities, int32& iNumClonedBrokenEntities,
        const EntityId* pRecordingEntities, int32 iNumRecordingEntities,
        SRenderNodeCloneLookup& nodeLookup);

    virtual void HideBrokenObjectsByIndex(const int32* pBrokenObjectIndices, const int32 iNumBrokenObjectIndices);
    virtual void UnhidePartRemovedObjectsByIndex(const int32* pPartRemovalIndices, const int32 iNumPartRemovalIndices, const EventPhysRemoveEntityParts* pBreakEvents);
    virtual void ApplySinglePartRemovalFromEventIndex(int32 iPartRemovalEventIndex, const SRenderNodeCloneLookup& renderNodeLookup, const EventPhysRemoveEntityParts* pBreakEvents);
    virtual void ApplyPartRemovalsUntilIndexToObjectList(int32 iFirstEventIndex, const SRenderNodeCloneLookup& renderNodeLookup, const EventPhysRemoveEntityParts* pBreakEvents);

private:
    void CreateObjectCommon(IStatObj* pStatObj, IPhysicalEntity* pPhysEnt, IBreakableManager::SCreateParams& createParams);

    void ApplyPartBreakToClonedObjectFromEvent(const SRenderNodeCloneLookup& renderNodeLookup, const EventPhysRemoveEntityParts& OriginalEvent);

    // Remove Parts
    bool RemoveStatObjParts(IStatObj*& pStatObj);

    CEntitySystem* m_pEntitySystem;
    IBreakEventListener* m_pBreakEventListener;
    //////////////////////////////////////////////////////////////////////////

    std::vector<IBreakableManager::SBrokenObjRec> m_brokenObjs;
    std::vector<IPhysicalEntity*> m_brokenObjsParticles;
};

#endif // ENABLE_CRY_PHYSICS

#endif // CRYINCLUDE_CRYENTITYSYSTEM_BREAKABLEMANAGER_H
