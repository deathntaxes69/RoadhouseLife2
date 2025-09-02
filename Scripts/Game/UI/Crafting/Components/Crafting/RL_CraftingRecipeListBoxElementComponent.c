class RL_CraftingRecipeListBoxElementComponent : RL_CraftingListBoxElementComponent
{
	[Attribute("preview")]
	protected string m_sWidgetPreview;

	[Attribute("text")]
	protected string m_sWidgetTextName;
	
	[Attribute("category")]
	protected string m_sWidgetCategory;
	
	[Attribute("icon")]
	protected string m_sWidgetIcon;
	
	[Attribute("quantity")]
	protected string m_sWidgetQuantity;
	
	static ItemPreviewManagerEntity m_previewManager;
	
	static ItemPreviewManagerEntity GetPreviewManager() {
	if (!m_previewManager)
		m_previewManager = ChimeraWorld.CastFrom(GetGame().GetWorld()).GetItemPreviewManager();
	
	return m_previewManager;
	}
	
	void SetCategory(string text)
	{
		TextWidget w = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetCategory));
		if (w)
			w.SetText(text);
	}
	
	void SetText(string text)
	{
		TextWidget w = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetTextName));
		if (w)
			w.SetText(text);
	}
	
	void SetQuantity(string text)
	{
		TextWidget w = TextWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetQuantity));
		if (w)
			w.SetText(text);
	}
	
	void SetIcon(ResourceName icon)
	{
		ImageWidget w = ImageWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetIcon));
		if (w)
			w.LoadImageTexture(0, icon);
	}
	
	void SetIconFromImageset(ResourceName imageset, string iconName)
	{
		ImageWidget w = ImageWidget.Cast(m_wRoot.FindAnyWidget(m_sWidgetIcon));
		if (w)
			w.LoadImageFromSet(0, imageset, iconName);
	}
	
	void SetPreview(ResourceName resource)
	{
		ItemPreviewWidget m_itemPreview = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget("preview"));
		
		ItemPreviewManagerEntity previewManager = GetPreviewManager();
		if (!previewManager)
			return;
		
		previewManager.SetPreviewItemFromPrefab(m_itemPreview, resource);
	}
	
}