enum RL_PreviewDisplayMode {
	CHARACTER_VIEW,
	ITEM_VIEW
}

class RL_PreviewComponent : ScriptedWidgetComponent {
	
	// UI Constants
	private const string PREVIEW_WORLD_PREFAB = "{DB4D5D3418488473}Prefabs/Preview/RL_PreviewWorld.et";
	private const string RENDER_TARGET_WIDGET_NAME = "ItemPreview";
	
	// Camera Configuration
	private const float CAMERA_FAR_PLANE = 200.0;
	private const float CAMERA_NEAR_PLANE = 0.005;
	private const float CAMERA_FOV = 38.0;
	private const float CAMERA_MOVE_THRESHOLD = 0.03;
	private const float CAMERA_SENSITIVITY = 0.01;
	private const float GAMEPAD_SENSITIVITY = 5.0;
	
	// Camera Limits
	private const float MIN_PITCH_ANGLE = -60.0;
	private const float MAX_PITCH_ANGLE = 45.0;
	private const float MOVEMENT_THRESHOLD = 0.05;
	private const float DISTANCE_THRESHOLD = 0.1;
	
	// Distance Multipliers
	private const float CHARACTER_DISTANCE_MULTIPLIER = 1.5;
	private const float ITEM_DISTANCE_MULTIPLIER = 1.25;
	
	// Core Components
	private Widget rootWidget;
	private RenderTargetWidget renderWidget;
	private InputManager inputManager;
	
	// Preview World Management
	private BaseWorld previewWorld;
	private ref SharedItemRef worldReference;
	private int cameraId = -1;
	private GenericEntity previewEntity;
	
	// Item Management
	private InventoryItemComponent targetItemComponent;
	private IEntity sourceEntity;
	
	// Camera State
	private vector targetCameraPosition = vector.One;
	private vector targetLookPosition = vector.Zero;
	private vector currentLookPosition;
	private vector currentCameraMatrix[4];
	private vector cameraDirection;
	private float cameraDistance;
	
	// Entity Measurements
	private vector entityCenter = vector.Zero;
	private float entitySize = 0.0;
	private float defaultDistance = 1.0;
	
	// Control State
	private RL_PreviewDisplayMode displayMode;
	private bool isFocused = false;
	private bool canMoveCamera = true;
	private bool allowCameraTargetChange = true;
	private bool disableAutoFocus = false;
	
	// Input Tracking
	private int previousMouseX;
	private int previousMouseY;

	//------------------------------------------------------------------------------------------------
	// Initialization & Setup
	//------------------------------------------------------------------------------------------------
	
	override protected void HandlerAttached(Widget w) {
		if (SCR_Global.IsEditMode())
			return;
	
		rootWidget = w;
		renderWidget = RenderTargetWidget.Cast(w.FindAnyWidget(RENDER_TARGET_WIDGET_NAME));
		
		inputManager = GetGame().GetInputManager();
		if (!inputManager) {
			Print("RL_PreviewComponent: Failed to get InputManager", LogLevel.ERROR);
			return;
		}
	}
	
	void Initialize(IEntity characterEntity) {
		// Reserved for future character-specific initialization
	}
	
	//------------------------------------------------------------------------------------------------
	// Public Interface
	//------------------------------------------------------------------------------------------------
	
	void SetCameraMovementEnabled(bool enabled) {
		canMoveCamera = enabled;
	}
	
	void ConfigurePreview(RL_PreviewDisplayMode mode, IEntity entity, bool resetCamera = true) {
		if (!worldReference)
			CreatePreviewWorld();
		
		sourceEntity = entity;
		
		if (entity) {
			targetItemComponent = InventoryItemComponent.Cast(entity.FindComponent(InventoryItemComponent));
			if (!targetItemComponent) {
				LogError(string.Format("Entity %1 missing InventoryItemComponent", entity));
				return;
			}
		} else {
			targetItemComponent = null;
		}
		
		disableAutoFocus = false;

		if (mode != displayMode) {
			disableAutoFocus = false;
			displayMode = mode;
			resetCamera = true;
		}

		if (!targetItemComponent)
			return;
		
		RefreshPreviewEntity();
		ConfigureCameraTarget(resetCamera);
	}

	void SetCameraPosition(vector position) {
		if (allowCameraTargetChange)
			targetCameraPosition = position;
	}
	
	//------------------------------------------------------------------------------------------------
	// Preview World Management
	//------------------------------------------------------------------------------------------------
	
	private void CreatePreviewWorld() {
		DestroyPreview();
		
		worldReference = BaseWorld.CreateWorld("PreviewWorld", "PreviewWorld");
		previewWorld = worldReference.GetRef();
		cameraId = 0;
		
		SetupCamera();
		CreateWorldEnvironment();
		
		renderWidget.SetWorld(previewWorld, cameraId);
	}
	
	private void SetupCamera() {
		previewWorld.SetCameraType(cameraId, CameraType.PERSPECTIVE);
		previewWorld.SetCameraFarPlane(cameraId, CAMERA_FAR_PLANE);
		previewWorld.SetCameraNearPlane(cameraId, CAMERA_NEAR_PLANE);
		previewWorld.SetCameraVerticalFOV(cameraId, CAMERA_FOV);
		
		vector cameraMatrix[4];
		cameraMatrix[3] = targetCameraPosition;
		SCR_Math3D.LookAt(targetCameraPosition, targetLookPosition, vector.Up, cameraMatrix);
		
		previewWorld.SetCameraEx(cameraId, cameraMatrix);
	}
	
	private void CreateWorldEnvironment() {
		Resource worldResource = Resource.Load(PREVIEW_WORLD_PREFAB);
		if (worldResource.IsValid())
			GetGame().SpawnEntityPrefabLocal(worldResource, previewWorld);
	}
	
	private void DestroyPreview() {
		if (previewEntity)
			SCR_EntityHelper.DeleteEntityAndChildren(previewEntity);
			
		if (worldReference) {
			cameraId = -1;
			previewEntity = null;
			previewWorld = null;
			worldReference = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Entity Management
	//------------------------------------------------------------------------------------------------
	
	private void RefreshPreviewEntity() {
		if (previewEntity)
			SCR_EntityHelper.DeleteEntityAndChildren(previewEntity);

		previewEntity = GenericEntity.Cast(targetItemComponent.CreatePreviewEntity(previewWorld, cameraId));
		OptimizeEntityForPreview(previewEntity);
		
		PositionAndMeasureEntity();
		OptimizeChildEntities();
	}
	
	private void PositionAndMeasureEntity() {
		previewEntity.SetOrigin(vector.Up);
		previewEntity.Update();

		vector minBounds, maxBounds;
		previewEntity.GetWorldBounds(minBounds, maxBounds);

		entityCenter = vector.Lerp(minBounds, maxBounds, 0.5);
		defaultDistance = vector.Distance(minBounds, maxBounds);
		entitySize = defaultDistance;
	}
	
	private void OptimizeChildEntities() {
		IEntity child = previewEntity.GetChildren();
		while (child) {
			Print("[RL_PreviewComponent] OptimizeChildEntities while loop");
			OptimizeEntityForPreview(child);
			child = child.GetSibling();
		}
	}
	
	private void OptimizeEntityForPreview(IEntity entity) {
		VObject visualObject = entity.GetVObject();
		entity.SetVComponentFlags(VCFlags.NOFILTER & VCFlags.NOLIGHT);
		entity.SetFixedLOD(0);
	}
	
	//------------------------------------------------------------------------------------------------
	// Camera Control
	//------------------------------------------------------------------------------------------------
	
	private void ConfigureCameraTarget(bool shouldReset) {
		vector lookPosition, direction;
		float targetDistance = entitySize * CHARACTER_DISTANCE_MULTIPLIER;

		if (!disableAutoFocus && CalculateCameraTargets(sourceEntity, lookPosition, direction, targetDistance)) {
			cameraDirection = direction.Normalized();
			targetLookPosition = lookPosition;
			cameraDistance = targetDistance;
		}
	}
	
	private bool CalculateCameraTargets(IEntity entity, out vector lookPosition, out vector direction, out float distance) {
		if (!entity)
			return false;
			
		lookPosition = entityCenter;
		
		if (SCR_ChimeraCharacter.Cast(entity)) {
			distance = entitySize * CHARACTER_DISTANCE_MULTIPLIER;
			direction = vector.Direction(lookPosition, lookPosition + (vector.Forward + vector.Right));
		} else {
			distance = entitySize * ITEM_DISTANCE_MULTIPLIER;
			
			// Maintain camera side preference
			if (currentCameraMatrix[3][0] < -0.1)
				direction = vector.Direction(lookPosition, lookPosition - vector.Right);
			else
				direction = vector.Direction(lookPosition, lookPosition + vector.Right);
		}
		
		return true;
	}
	
	private void UpdateCameraPosition(float deltaTime) {
		if (!worldReference)
			return;
		
		previewWorld.GetCamera(cameraId, currentCameraMatrix);
		
		vector targetPosition = targetLookPosition + (cameraDirection * cameraDistance);
		
		// Smooth camera position interpolation
		float positionDistance = vector.Distance(currentCameraMatrix[3], targetPosition);
		if (positionDistance > DISTANCE_THRESHOLD) {
			currentCameraMatrix[3] = currentCameraMatrix[3] + ((targetPosition - currentCameraMatrix[3]));
		}
		
		// Smooth look position interpolation
		float lookDistance = vector.Distance(currentLookPosition, targetLookPosition);
		if (lookDistance > DISTANCE_THRESHOLD) {
			currentLookPosition = currentLookPosition + ((targetLookPosition - currentLookPosition));
		}

		SCR_Math3D.LookAt(currentCameraMatrix[3], currentLookPosition, vector.Up, currentCameraMatrix);
		previewWorld.SetCameraEx(cameraId, currentCameraMatrix);
	}
	
	//------------------------------------------------------------------------------------------------
	// Input Handling
	//------------------------------------------------------------------------------------------------
	
	private void ProcessCameraInput(float deltaTime) {
		if (!canMoveCamera)
			return;

		EInputDeviceType deviceType = inputManager.GetLastUsedInputDevice();
		bool isGamepad = deviceType == EInputDeviceType.GAMEPAD;
		
		// Temporary gamepad disable
		if (isGamepad)
			return;
		
		if (!ValidateInputConditions(isGamepad))
			return;
		
		vector movement = GetInputMovement(isGamepad);
		if (movement.Length() == 0)
			return;
		
		disableAutoFocus = true;
		RotateCamera(movement[0], movement[1]);
	}
	
	private bool ValidateInputConditions(bool isGamepad) {
		if (!isGamepad && !isFocused)
			return false;
		
		if (!isGamepad && (Debug.GetMouseState(MouseState.LEFT) > 0))
			return false;
			
		return true;
	}
	
	private vector GetInputMovement(bool isGamepad) {
		float currentX = inputManager.GetActionValue("MouseX");
		float currentY = inputManager.GetActionValue("MouseY");
		float moveX = 0, moveY = 0;

		if (isGamepad) {
			if ((Math.AbsFloat(currentX) + Math.AbsFloat(currentY)) < CAMERA_MOVE_THRESHOLD)
				return vector.Zero;

			moveX = currentX * GAMEPAD_SENSITIVITY;
			moveY = currentY * -GAMEPAD_SENSITIVITY;
		} else {
			if (previousMouseX != 0 && previousMouseY != 0) {
				moveX = previousMouseX - currentX;
				moveY = previousMouseY - currentY;
			}

			previousMouseX = currentX;
			previousMouseY = currentY;
		}

		return Vector(moveX, moveY, 0);
	}
	
	private void RotateCamera(float inputX, float inputY) {
		if (!previewWorld)
			return;
		
		vector newMatrix[4];
		
		if (((Math.AbsFloat(inputY) + Math.AbsFloat(inputX)) * CAMERA_SENSITIVITY) > MOVEMENT_THRESHOLD)
			allowCameraTargetChange = false;

		newMatrix = currentCameraMatrix;
		newMatrix[3] = targetLookPosition + (cameraDirection * cameraDistance);

		// Horizontal rotation
		SCR_Math3D.RotateAround(newMatrix, targetLookPosition, currentCameraMatrix[1], inputX * CAMERA_SENSITIVITY, newMatrix);
		cameraDirection = vector.Direction(targetLookPosition, newMatrix[3]).Normalized();

		// Vertical rotation with limits
		SCR_Math3D.RotateAround(newMatrix, targetLookPosition, currentCameraMatrix[0], inputY * CAMERA_SENSITIVITY, newMatrix);

		if (!IsValidCameraAngle(newMatrix, inputY))
			return;

		cameraDirection = vector.Direction(targetLookPosition, newMatrix[3]).Normalized();
	}
	
	private bool IsValidCameraAngle(vector matrix[4], float inputY) {
		vector angles = Math3D.MatrixToAngles(matrix);
		
		if (inputY > 0 && angles[1] < MIN_PITCH_ANGLE)
			return false;
		
		if (inputY < 0 && angles[1] > MAX_PITCH_ANGLE)
			return false;
			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	// Event Handlers
	//------------------------------------------------------------------------------------------------
	
	override bool OnFocus(Widget w, int x, int y) {
		isFocused = true;
		return super.OnFocus(w, x, y);
	}
	
	override bool OnFocusLost(Widget w, int x, int y) {
		isFocused = false;
		return super.OnFocusLost(w, x, y);
	}
	
	override bool OnMouseEnter(Widget w, int x, int y) {
		isFocused = true;
		return super.OnMouseEnter(w, x, y);
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
		isFocused = false;
		return super.OnMouseLeave(w, enterW, x, y);
	}
	
	//------------------------------------------------------------------------------------------------
	// Update Loop
	//------------------------------------------------------------------------------------------------
	
	void Update(float deltaTime) {
		ProcessCameraInput(deltaTime);
		UpdateCameraPosition(deltaTime);
	}
	
	//------------------------------------------------------------------------------------------------
	// Utility Methods
	//------------------------------------------------------------------------------------------------
	
	private void LogError(string message) {
		Print(string.Format("RL_PreviewComponent: %1", message), LogLevel.ERROR);
	}
}
