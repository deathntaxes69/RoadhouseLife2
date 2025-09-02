sealed class RL_MapMarkerWidgetComponent : ScriptedWidgetComponent
{
    private ImageWidget m_imageWidget;
    private RichTextWidget m_textWidget;

    // Widget element names
    private const string IMAGE_WIDGET_NAME = "IconImage";
    private const string TEXT_WIDGET_NAME = "MarkerText";

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
        m_imageWidget = ImageWidget.Cast(w.FindAnyWidget(IMAGE_WIDGET_NAME));
        m_textWidget = RichTextWidget.Cast(w.FindAnyWidget(TEXT_WIDGET_NAME));

		
	}
    Widget GetImageWidget()
    {
        return m_imageWidget;
    }
    Widget GetTextWidget()
    {
        return m_textWidget;
    }
    void SetTextSize(int size) {
        if (!m_textWidget) return;
        
        m_textWidget.SetExactFontSize(size);
    }
    void SetIconSize(int size) {
        if (!m_imageWidget) return;
        //Print(string.Format("SETICONSIZE %1 %2", size, size));
        m_imageWidget.SetSize(size, size);
    }
    
    void SetIconRotation(float rotation) {
        if (!m_imageWidget) return;

        m_imageWidget.SetRotation(rotation);
    }
    void HideText() {
        if (!m_textWidget) return;

        m_textWidget.SetVisible(false);
    }
    void SetupMarkerText(string text, Color color) {
        if (!m_textWidget) return;
        
        m_textWidget.SetText(text);
        m_textWidget.SetColor(color);
    }
    void SetupMarkerIcon(string imagesetPath, string iconName) {
        if (!m_imageWidget) return;
        
        if (!imagesetPath.IsEmpty() && !iconName.IsEmpty()) {
            m_imageWidget.LoadImageFromSet(0, imagesetPath, iconName);
        } else {
            m_imageWidget.SetVisible(false);
        }
    }
    void SetupRadiusMarker(Color color, string text, bool useControlZoneLayout) {
        if (m_imageWidget)
            m_imageWidget.SetColor(color);
        
        if (useControlZoneLayout && m_textWidget) {
            m_textWidget.SetText(text);
        }
    }
    
}