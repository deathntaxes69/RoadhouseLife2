[EntityEditorProps(category: "NarcosLife/Building", description: "Placement")]
class RL_PlacementComponentClass : SCR_GadgetComponentClass
{
	[Attribute(desc: "Prefab to be placed when placement is confirmed")]
	ResourceName m_sBuildingPrefab;
	
	[Attribute("5.0", desc: "Maximum placement distance from player")]
	float m_fMaxPlacementDistance;
	
	[Attribute("0.5", desc: "Rotation step angle")]
	float m_fRotationStep;
	
	[Attribute("0", desc: "Allow placement in air, false will snap to ground/entity below")]
	bool m_bAllowAirPlacement;
	
	[Attribute("0", desc: "Restrict placement to characters house bounds only")]
	bool m_bRestrictToHouseBounds;
	
	[Attribute("1", desc: "Check for collisions with other entities when validating placement")]
	bool m_bCheckCollisions;
	
	[Attribute("0", desc: "Match roll and pitch of surface below when placing (terrain snap)")]
	bool m_bMatchSurfaceOrientation;
	
	protected bool m_bSignalsInitialized;
	protected int m_iInHandSignalIndex = -1;
	
	//------------------------------------------------------------------------------------------------
	void InitializeSignals(IEntity owner)
	{
		if (m_bSignalsInitialized)
			return;
			
		SignalsManagerComponent signalManager = SignalsManagerComponent.Cast(owner.FindComponent(SignalsManagerComponent));
		if (!signalManager)
			return;
		
		m_iInHandSignalIndex = signalManager.FindSignal("InHand");
		m_bSignalsInitialized = (m_iInHandSignalIndex != -1);
	}
	
	//------------------------------------------------------------------------------------------------
	bool AreSignalsInitialized()
	{
		return m_bSignalsInitialized;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetInHandSignalIndex()
	{
		return m_iInHandSignalIndex;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool ValidateResourceName(ResourceName prefabPath)
	{
		if (prefabPath.IsEmpty())
			return false;
			
		Resource prefabResource = Resource.Load(prefabPath);
		return prefabResource != null;
	}
}

class RL_PlacementComponent : SCR_GadgetComponent
{
	protected static const ResourceName VALID_PLACEMENT_MATERIAL = "{E9FF559506FA3F8F}Scripts/Game/Building/PlacementValid.emat";
	protected static const ResourceName INVALID_PLACEMENT_MATERIAL = "{5790CFB1DB0C8D2E}Scripts/Game/Building/PlacementInvalid.emat";
	
	protected static const string ACTION_ROTATE_LEFT = "RL_PlacementRotateLeft";
	protected static const string ACTION_ROTATE_RIGHT = "RL_PlacementRotateRight";
	protected static const string ACTION_CONFIRM = "RL_PlacementConfirm";
	
	protected static const float GROUND_TRACE_HEIGHT_OFFSET = 0.5;
	protected static const float GROUND_TRACE_DEPTH = 1000.0;
	protected static const float ENTITY_BOTTOM_SAFETY_MARGIN = 0.01;
	protected static const float SURFACE_ALIGNMENT_SAFETY_MARGIN = 0.05;
	protected static const float COLLISION_TOLERANCE = 0.1;
	
	protected SignalsManagerComponent m_SignalManager;
	protected RL_PlacementComponentClass m_ComponentData;
	
	protected IEntity m_PreviewEntity;
	protected ref Resource m_BuildingPrefabResource;
	protected ResourceName m_CurrentPreviewMaterial;
	protected vector m_vPositionOffset;
	protected float m_fUserYawRotation;
	
	protected bool m_bIsPlacementValid;
	protected float m_fValidationTimer;
	protected static const float VALIDATION_UPDATE_INTERVAL = 0.3;
	
	protected int m_iTimeBetweenPlacements = 3000;
	protected int m_iLastPlacementTime = -1;
	
	//------------------------------------------------------------------------------------------------
	protected bool IsSpamming(bool silent = false)
	{
		int currentTime = System.GetTickCount();
		int diff;

		if (m_iLastPlacementTime == -1)
			diff = m_iTimeBetweenPlacements + 1;
		else
			diff = currentTime - m_iLastPlacementTime;

		if (diff < m_iTimeBetweenPlacements)
		{
			if (!silent)
				RL_Utils.Notify("Canceled, slow down", "ACTION");
			return true;
		}

		m_iLastPlacementTime = currentTime;
		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected TraceParam CreateGroundTrace(vector startPosition, float heightOffset = GROUND_TRACE_HEIGHT_OFFSET, float depth = GROUND_TRACE_DEPTH)
	{
		TraceParam trace = new TraceParam();
		trace.Start = startPosition + Vector(0, heightOffset, 0);
		trace.End = startPosition + Vector(0, -depth, 0);
		trace.Flags = TraceFlags.DEFAULT;
		trace.LayerMask = EPhysicsLayerDefs.Default;
		return trace;
	}
	
	//------------------------------------------------------------------------------------------------
	protected vector ProcessGroundTraceResult(TraceParam trace, float bottomOffset, float hitDistance)
	{
		vector groundPosition = trace.Start + ((trace.End - trace.Start) * hitDistance);
		groundPosition[1] = groundPosition[1] + bottomOffset;
		return groundPosition;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsValidForPlacement()
	{
		return m_PreviewEntity && m_ComponentData;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RL_PlacementComponent()
	{
		UnregisterInputListeners();
		CleanupPreviewEntity();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_ComponentData = RL_PlacementComponentClass.Cast(GetComponentData(owner));
		m_SignalManager = SignalsManagerComponent.Cast(owner.FindComponent(SignalsManagerComponent));

		if (m_ComponentData && RL_PlacementComponentClass.ValidateResourceName(m_ComponentData.m_sBuildingPrefab))
		{
			m_BuildingPrefabResource = Resource.Load(m_ComponentData.m_sBuildingPrefab);
		}
		
		RegisterInputListeners();
		UpdateGadgetState();
	}
	
	//------------------------------------------------------------------------------------------------
	override void ModeSwitch(EGadgetMode mode, IEntity charOwner)
	{
		super.ModeSwitch(mode, charOwner);
		
		bool shouldActivate = (mode == EGadgetMode.IN_HAND);
		
		if (shouldActivate)
		{
			m_bActivated = true;
			ToggleActive(!m_bActivated, SCR_EUseContext.FROM_ACTION);
		}
		else
		{
			OnToggleActive(false);
		}
		
		UpdateGadgetState();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnToggleActive(bool state)
	{
		m_bActivated = state;
		UpdateGadgetState();
	}
	
	//------------------------------------------------------------------------------------------------
	override void ActivateAction()
	{
		ToggleActive(!m_bActivated, SCR_EUseContext.FROM_ACTION);
		
		if (IsToggledOn())
		{
			StartPlacementMode();
		}
		else
		{
			StopPlacementMode();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnModeChanged(EGadgetMode mode, IEntity charOwner)
	{
		super.OnModeChanged(mode, charOwner);
		CleanupPreviewEntity();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		CleanupPreviewEntity();
		super.OnDelete(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_bActivated)
			return;
		
		UpdatePreviewEntityPlacement();
		m_fValidationTimer += timeSlice;
		if (m_fValidationTimer >= VALIDATION_UPDATE_INTERVAL)
		{
			UpdatePlacementValidation();
			m_fValidationTimer = 0.0;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void DeactivateGadgetUpdate()
	{
		super.DeactivateGadgetUpdate();
		
		if (System.IsConsoleApp())
			return;
		
		ClearEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void ActivateGadgetUpdate()
	{
		super.ActivateGadgetUpdate();
		
		if (IsToggledOn())
		{
			StartPlacementMode();
		}
		else
		{
			StopPlacementMode();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override EGadgetType GetType()
	{
		return EGadgetType.BUILDING_TOOL;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeRaised()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool IsUsingADSControls()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplSave(ScriptBitWriter writer)
	{
		if (!super.RplSave(writer))
			return false;
		
		writer.WriteBool(m_bActivated);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool RplLoad(ScriptBitReader reader)
	{
		if (!super.RplLoad(reader))
			return false;
		
		reader.ReadBool(m_bActivated);
		return true;
	}
	

	
	//------------------------------------------------------------------------------------------------
	protected void StartPlacementMode()
	{
		SetEventMask(GetOwner(), EntityEvent.FRAME);
		m_fUserYawRotation = 0.0;
		m_fValidationTimer = VALIDATION_UPDATE_INTERVAL;
		CreatePreviewEntity();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void StopPlacementMode()
	{
		ClearEventMask(GetOwner(), EntityEvent.FRAME);
		CleanupPreviewEntity();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterInputListeners()
	{
		if (Replication.IsServer())
			return;
			
		GetGame().GetInputManager().AddActionListener(ACTION_ROTATE_LEFT, EActionTrigger.PRESSED, OnRotateLeftInput);
		GetGame().GetInputManager().AddActionListener(ACTION_ROTATE_RIGHT, EActionTrigger.PRESSED, OnRotateRightInput);
		GetGame().GetInputManager().AddActionListener(ACTION_CONFIRM, EActionTrigger.DOWN, OnConfirmPlacement);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UnregisterInputListeners()
	{
		if (Replication.IsServer())
			return;
			
		GetGame().GetInputManager().RemoveActionListener(ACTION_ROTATE_LEFT, EActionTrigger.PRESSED, OnRotateLeftInput);
		GetGame().GetInputManager().RemoveActionListener(ACTION_ROTATE_RIGHT, EActionTrigger.PRESSED, OnRotateRightInput);
		GetGame().GetInputManager().RemoveActionListener(ACTION_CONFIRM, EActionTrigger.DOWN, OnConfirmPlacement);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnRotateLeftInput(float value = 0.0, EActionTrigger reason = 0)
	{
		HandleRotationInput(-m_ComponentData.m_fRotationStep, reason);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnRotateRightInput(float value = 0.0, EActionTrigger reason = 0)
	{
		HandleRotationInput(m_ComponentData.m_fRotationStep, reason);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void HandleRotationInput(float rotationAngle, EActionTrigger reason)
	{
		if (!m_bActivated || !m_PreviewEntity || reason != EActionTrigger.PRESSED)
			return;
		
		m_fUserYawRotation += rotationAngle;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnConfirmPlacement()
	{

		if (IsSpamming())
			return;

		if (!m_bActivated || !IsValidForPlacement() || !m_bIsPlacementValid)
		{
			if (m_bActivated)
				ShowPlacementError();
			return;
		}
		
		vector position = GetPlacementPosition();
		vector angles = Vector(m_fUserYawRotation, 0, 0);
		
		if (m_ComponentData.m_bMatchSurfaceOrientation)
		{
			CalculateSurfaceAlignedTransform(position, angles, position, angles);
		}
		
		bool isDynamicUpgrade = m_ComponentData.m_bRestrictToHouseBounds;
		if (!RequestPlacementViaCharacter(position, angles, m_ComponentData.m_sBuildingPrefab, isDynamicUpgrade))
		{
			ShowPlacementError();
			return;
		}
		
		ClearEventMask(GetOwner(), EntityEvent.FRAME);
		m_bActivated = false;
		CleanupPreviewEntity();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreatePreviewEntity()
	{
		if (m_PreviewEntity || !m_BuildingPrefabResource || !m_bActivated)
			return;
		
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.Transform[3] = GetCharacterOwner().GetOrigin();
		spawnParams.TransformMode = ETransformMode.WORLD;
		m_PreviewEntity = GetGame().SpawnEntityPrefabLocal(m_BuildingPrefabResource, GetOwner().GetWorld(), spawnParams);
		if (m_PreviewEntity)
			SetupPreviewEntityProperties();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetupPreviewEntityProperties()
	{
		if (!m_PreviewEntity)
			return;
		
		SetPreviewMaterial(INVALID_PLACEMENT_MATERIAL);
		Physics previewPhysics = m_PreviewEntity.GetPhysics();
		if (previewPhysics)
		{
			previewPhysics.SetActive(false);
			previewPhysics.Destroy();
		}
		
		RL_PickupComponent pickupComponent = RL_PickupComponent.Cast(m_PreviewEntity.FindComponent(RL_PickupComponent));
		if (pickupComponent)
			pickupComponent.SetBeingPlaced(true);
		
		m_PreviewEntity.SetFlags(EntityFlags.VISIBLE, false);
		m_PreviewEntity.SetFlags(EntityFlags.VISIBLE, true);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CleanupPreviewEntity()
	{
		if (m_PreviewEntity)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(m_PreviewEntity);
			m_PreviewEntity = null;
		}
		
		m_CurrentPreviewMaterial = string.Empty;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdatePreviewEntityPlacement()
	{
		if (!m_PreviewEntity)
		{
			CreatePreviewEntity();
			return;
		}
		
		vector targetPosition = GetPlacementPosition();
		vector currentAngles = Vector(m_fUserYawRotation, 0, 0);
		
		if (m_ComponentData && m_ComponentData.m_bMatchSurfaceOrientation)
		{
			CalculateSurfaceAlignedTransform(targetPosition, currentAngles, targetPosition, currentAngles);
		}
		
		vector transform[4];
		Math3D.AnglesToMatrix(currentAngles, transform);
		transform[3] = targetPosition + m_vPositionOffset;
		m_PreviewEntity.SetTransform(transform);
		m_PreviewEntity.Update();
	}
	
	//------------------------------------------------------------------------------------------------
	protected vector GetPlacementPosition()
	{
		vector cameraTransform[4];
		GetGame().GetCameraManager().CurrentCamera().GetWorldCameraTransform(cameraTransform);
		vector startPosition = cameraTransform[3];
		vector forwardDirection = cameraTransform[2];
		autoptr TraceParam trace = TraceParam();
		trace.Start = startPosition;
		trace.End = startPosition + (forwardDirection * m_ComponentData.m_fMaxPlacementDistance);
		trace.Flags = TraceFlags.DEFAULT;
		trace.LayerMask = EPhysicsLayerDefs.Default;
		float hitDistance = GetGame().GetWorld().TraceMove(trace, null);
		
		vector targetPosition = startPosition + (forwardDirection * (hitDistance * m_ComponentData.m_fMaxPlacementDistance));
	
		if (!m_ComponentData.m_bAllowAirPlacement)
		{
			targetPosition = SnapToGround(targetPosition);
		}
		
		return targetPosition;
	}
	
	//------------------------------------------------------------------------------------------------
	protected vector SnapToGround(vector airPosition)
	{
		TraceParam groundTrace = CreateGroundTrace(airPosition);
		float hitDistance = GetGame().GetWorld().TraceMove(groundTrace, null);
		
		if (hitDistance < 1.0)
		{
			float verticalOffset = GetEntityBottomOffset();
			return ProcessGroundTraceResult(groundTrace, verticalOffset, hitDistance);
		}
		
		return airPosition;
	}

	//------------------------------------------------------------------------------------------------
	protected float GetEntityBottomOffset()
	{
		if (!m_PreviewEntity)
			return ENTITY_BOTTOM_SAFETY_MARGIN;
		
		vector boundingBoxMin, boundingBoxMax;
		m_PreviewEntity.GetBounds(boundingBoxMin, boundingBoxMax);
		return -boundingBoxMin[1] + ENTITY_BOTTOM_SAFETY_MARGIN;
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdatePlacementValidation()
	{
		if (!m_PreviewEntity)
			return;
		
		bool isValidPlacement = ValidatePlacementLocation();
		
		ResourceName targetMaterial;
		if (isValidPlacement)
			targetMaterial = VALID_PLACEMENT_MATERIAL;
		else
			targetMaterial = INVALID_PLACEMENT_MATERIAL;
		
		if (m_CurrentPreviewMaterial != targetMaterial)
			SetPreviewMaterial(targetMaterial);
		
		m_bIsPlacementValid = isValidPlacement;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ValidatePlacementLocation()
	{
		if (!m_ComponentData)
			return false;

		if (m_ComponentData.m_bCheckCollisions && !IsPlacementClearOfCollisions())
			return false;
	
		if (m_ComponentData.m_bRestrictToHouseBounds && !IsWithinHouseBounds())
			return false;
	
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsWithinHouseBounds()
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetCharacterOwner());
		if (!character)
			return false;
		
		if (!character.HasHouseBounds())
			return false;
		
		vector boundsMin, boundsMax;
		if (!character.GetHouseBounds(boundsMin, boundsMax))
			return false;
		
		vector placementPosition = GetPlacementPosition();
		
		bool isWithinBounds = (placementPosition[0] >= boundsMin[0] && placementPosition[0] <= boundsMax[0] &&
				placementPosition[1] >= boundsMin[1] && placementPosition[1] <= boundsMax[1] &&
				placementPosition[2] >= boundsMin[2] && placementPosition[2] <= boundsMax[2]);
		
		if (!isWithinBounds)
		{
			PrintFormat("[RL_PlacementComponent] House bounds check failed not within bounds");
			PrintFormat("[RL_PlacementComponent] Placement position: %1", placementPosition);
			PrintFormat("[RL_PlacementComponent] Bounds min: %1", boundsMin);
			PrintFormat("[RL_PlacementComponent] Bounds max: %1", boundsMax);
		}
			
		return isWithinBounds;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsPlacementClearOfCollisions()
	{
		if (!IsValidForPlacement())
			return false;
		
		vector placementPosition = GetPlacementPosition() + m_vPositionOffset;
		vector transformMatrix[4];
		m_PreviewEntity.GetWorldTransform(transformMatrix);
		transformMatrix[3] = placementPosition;
		
		vector boundingBoxMin, boundingBoxMax;
		m_PreviewEntity.GetBounds(boundingBoxMin, boundingBoxMax);
		
		vector toleranceVector = Vector(COLLISION_TOLERANCE, COLLISION_TOLERANCE, COLLISION_TOLERANCE);
		boundingBoxMin = boundingBoxMin + toleranceVector;
		boundingBoxMax = boundingBoxMax - toleranceVector;
		
		TraceOBB collisionTrace = new TraceOBB();
		Math3D.MatrixIdentity3(collisionTrace.Mat);
		
		collisionTrace.Mat[0] = transformMatrix[0];
		collisionTrace.Mat[1] = transformMatrix[1];
		collisionTrace.Mat[2] = transformMatrix[2];
		collisionTrace.Start = transformMatrix[3];
		collisionTrace.Flags = TraceFlags.ENTS;
		collisionTrace.LayerMask = EPhysicsLayerPresets.Projectile;
		collisionTrace.Mins = boundingBoxMin;
		collisionTrace.Maxs = boundingBoxMax;
		
		autoptr array<IEntity> entitiesToIgnore = {};
		if (GetCharacterOwner())
			entitiesToIgnore.Insert(GetCharacterOwner());
		if (m_PreviewEntity)
			entitiesToIgnore.Insert(m_PreviewEntity);
		
		collisionTrace.ExcludeArray = entitiesToIgnore;
		GetGame().GetWorld().TracePosition(collisionTrace, null);
		
		return !collisionTrace.TraceEnt;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetPreviewMaterial(ResourceName materialPath)
	{
		if (m_PreviewEntity && m_CurrentPreviewMaterial != materialPath)
		{
			SCR_Global.SetMaterial(m_PreviewEntity, materialPath);
			m_CurrentPreviewMaterial = materialPath;
		}
	}
	

	
	//------------------------------------------------------------------------------------------------
	protected void ShowPlacementError()
	{
		RL_Utils.Notify("Invalid placement", "HOUSE");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateGadgetState()
	{
		if (!m_CharacterOwner)
			return;
		
		RplComponent rplComponent = RplComponent.Cast(m_CharacterOwner.FindComponent(RplComponent));
		if (!rplComponent || !rplComponent.IsOwner())
			return;
		
		if (IsToggledOn())
		{
			ActivateGadgetUpdate();
		}
		else
		{
			DeactivateGadgetUpdate();
		}
		
		UpdateSignalState();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSignalState()
	{
		if (!m_ComponentData || !m_SignalManager)
			return;
		
		if (!m_ComponentData.AreSignalsInitialized())
		{
			m_ComponentData.InitializeSignals(GetOwner());
		}
		
		if (m_iMode != EGadgetMode.IN_SLOT)
		{
			int signalIndex = m_ComponentData.GetInHandSignalIndex();
			if (signalIndex != -1)
			{
				m_SignalManager.SetSignalValue(signalIndex, 1);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CalculateSurfaceAlignedTransform(vector inputPosition, vector inputAngles, out vector outputPosition, out vector outputAngles)
	{
		outputPosition = inputPosition;
		outputAngles = inputAngles;
		
		if (!m_PreviewEntity)
			return;
		
		TraceParam surfaceTrace = CreateGroundTrace(inputPosition);
		float hitDistance = GetGame().GetWorld().TraceMove(surfaceTrace, null);
		
		if (hitDistance >= 1.0 || !surfaceTrace.TraceEnt)
			return;
		
		vector surfaceNormal = surfaceTrace.TraceNorm;
		vector groundPosition = surfaceTrace.Start + ((surfaceTrace.End - surfaceTrace.Start) * hitDistance);
		
		outputAngles = CalculateSurfaceAlignedAngles(inputAngles[0], surfaceNormal);
		outputPosition = CalculateRotatedGroundPosition(groundPosition, outputAngles);
	}
	
	//------------------------------------------------------------------------------------------------
	protected vector CalculateSurfaceAlignedAngles(float originalYaw, vector surfaceNormal)
	{
		vector forward = Vector(Math.Sin(originalYaw * Math.DEG2RAD), 0, Math.Cos(originalYaw * Math.DEG2RAD));
		vector right = Vector(forward[2], 0, -forward[0]);
		
		vector surfaceForward = forward - (surfaceNormal * vector.Dot(forward, surfaceNormal));
		surfaceForward.Normalize();
		
		vector surfaceRight = right - (surfaceNormal * vector.Dot(right, surfaceNormal));
		surfaceRight.Normalize();
		
		vector rotMatrix[3];
		rotMatrix[0] = surfaceRight;
		rotMatrix[1] = surfaceNormal;  
		rotMatrix[2] = surfaceForward;
		
		return Math3D.MatrixToAngles(rotMatrix);
	}
	
	//------------------------------------------------------------------------------------------------
	protected vector CalculateRotatedGroundPosition(vector groundPosition, vector rotationAngles)
	{
		if (!m_PreviewEntity)
			return groundPosition;
			
		vector originalTransform[4];
		m_PreviewEntity.GetWorldTransform(originalTransform);
		
		vector tempTransform[4];
		Math3D.AnglesToMatrix(rotationAngles, tempTransform);
		tempTransform[3] = groundPosition;
		m_PreviewEntity.SetTransform(tempTransform);
		
		vector boundingBoxMin, boundingBoxMax;
		m_PreviewEntity.GetBounds(boundingBoxMin, boundingBoxMax);
		float bottomOffset = -boundingBoxMin[1] + SURFACE_ALIGNMENT_SAFETY_MARGIN;
		
		m_PreviewEntity.SetTransform(originalTransform);
		
		return Vector(groundPosition[0], groundPosition[1] + bottomOffset, groundPosition[2]);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool RequestPlacementViaCharacter(vector position, vector angles, ResourceName prefabPath, bool isDynamicUpgrade)
	{
		SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(GetCharacterOwner());
		if (!playerCharacter)
			return false;
		
		ResourceName gadgetPrefab = GetOwner().GetPrefabData().GetPrefabName();
		return playerCharacter.RequestBuildingPlacement(position, angles, prefabPath, isDynamicUpgrade, gadgetPrefab);
	}
	

}
