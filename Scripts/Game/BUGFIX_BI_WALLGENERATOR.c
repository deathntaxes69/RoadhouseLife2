// delete me after BI finally fixed the wall generator bug
// ben


#ifdef WORKBENCH
modded class WallGeneratorEntity
{
override void Generate()
{
if (!m_bEnableGeneration)
{
Print("Wall generation is disabled for this shape - tick it back on before saving", LogLevel.NORMAL);
return;
}

m_RandomGenerator.SetSeed(m_iSeed);

WorldEditorAPI worldEditorAPI = _WB_GetEditorAPI();
if (!worldEditorAPI || worldEditorAPI.UndoOrRedoIsRestoring())
return;

BaseWorld world = worldEditorAPI.GetWorld();
if (!world)
return;
 
//Need to set it here, because wall groups need to measure!
s_World = ((WorldEditor)Workbench.GetModule(WorldEditor)).GetApi().GetWorld();

if (!m_WallGroupContainer)
m_WallGroupContainer = new SCR_WallGroupContainer(m_aWallGroups, UseXAsForward, MiddleObject);

DeleteAllChildren();

if (!m_ParentShapeSource || !m_ShapeNextPointHelper || !m_ShapeNextPointHelper.IsValid())
{
s_World = null;
return;
}

ShapeEntity shapeEntity = ShapeEntity.Cast(worldEditorAPI.SourceToEntity(m_ParentShapeSource));
if (!shapeEntity)
{
s_World = null;
return;
}

float rotationOffset;

int forwardAxis;
if (UseXAsForward)
rotationOffset = -90;
else
forwardAxis = 2;

if (Rotate180)
rotationOffset += 180;

if (m_bStartFromTheEnd)
rotationOffset += 180;

MEASUREMENTS.Clear();

array<vector> anchorPoints = m_ShapeNextPointHelper.GetAnchorPoints();

typename type = m_ParentShapeSource.GetClassName().ToType();
bool isPolyline = type && type.IsInherited(PolylineShapeEntity); // no "exact placement" otherwise (potential unknown shape type)

// straight line mode variables start
bool isStraightLineMode = isPolyline; // in case we make it a checkbox

if (isStraightLineMode)
GenerateInStraightLine(anchorPoints, rotationOffset, forwardAxis);
else
GenerateClosestToShape(anchorPoints, rotationOffset);

s_World = null; // see? I told you - used wisely
}
}

#endif