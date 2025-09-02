sealed class RL_MapMarkerSearchWidgetComponent : ScriptedWidgetComponent
{
    private RL_MapUIComponent m_mapUiComp;
    private ImageWidget m_imageWidget;
    private RichTextWidget m_textWidget;

    private SCR_EditBoxComponent searchInputField;
    private SCR_ListBoxComponent searchResultsList;

    // Widget element names
    private const string SEARCH_INPUT_WIDGET_NAME = "SearchInput";
    private const string RESULT_LIST_WIDGET_NAME = "ResultList";

    override protected void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

        searchInputField = SCR_EditBoxComponent.Cast(
            w.FindAnyWidget(SEARCH_INPUT_WIDGET_NAME).FindHandler(SCR_EditBoxComponent)
        );
        searchResultsList = SCR_ListBoxComponent.Cast(
            w.FindAnyWidget(RESULT_LIST_WIDGET_NAME).FindHandler(SCR_ListBoxComponent)
        );
		
	}
    void SetMapUiComp(RL_MapUIComponent comp)
    {
        m_mapUiComp = comp;
        searchInputField.m_OnChanged.Insert(HandleSearchInputChange);
        searchResultsList.m_OnChanged.Insert(HandleSearchResultSelection);
        HandleSearchInputChange();
        
    }
    private void AddMarkerToSearchResults(RL_MapMarker marker) {
        RL_MarkerData markerData = RL_MarkerData(marker.GetOwnerOrigin());
        
        searchResultsList.AddItemAndIcon(
            marker.GetText(),
            marker.GetMarkerIconPath(),
            marker.GetMarkerIconName(),
            null,
            markerData
        );
    }
    void HandleSearchInputChange() {
        
        ClearSearchResults();
        string searchFilter = GetNormalizedSearchFilter();
        bool focusedOnResult = false;
        SCR_ChimeraCharacter localPlayer = RL_Utils.GetLocalCharacter();
        foreach(int index, RL_MapMarker marker: m_mapUiComp.GetMarkerArray()) {
            //Print("[RL_MapMarkerSearchWidgetComponent] HandleSearchInputChange foreach loop");
            if (!RL_MapUtils.ShouldIncludeMarkerInSearch(marker, searchFilter)) continue;

            if (localPlayer && !RL_MapUtils.ShouldMarkerBeVisible(marker, localPlayer)) continue;
            
            AddMarkerToSearchResults(marker);
            
            if (searchFilter.IsEmpty() || focusedOnResult) continue;

            focusedOnResult = true;
            m_mapUiComp.FocusOnMarkerLocation(marker.GetOwnerOrigin());
        }
    }
    private void HandleSearchResultSelection(Managed obj, int itemIndex, int selectedIndex) {
        if (itemIndex < 0) return;
        
        Managed searchResultData = searchResultsList.GetItemData(itemIndex);
        if (!searchResultData)
            return;
        RL_MarkerData selectedMarkerData = RL_MarkerData.Cast(searchResultData);
        if (!selectedMarkerData)
            return;
        
        m_mapUiComp.FocusOnMarkerLocation(selectedMarkerData.m_vMarkerPos);
    }
    private void ClearSearchResults() {
        while (searchResultsList.GetItemCount() > 0) {
            Print("[RL_MapMarkerSearchWidgetComponent] ClearSearchResults while loop");
            searchResultsList.RemoveItem(0);
        }
    }
    private string GetNormalizedSearchFilter() {
        string filter = searchInputField.GetValue();
        filter.ToLower();
        return filter;
    }
   
}
sealed class RL_MarkerData : Managed {
    
    vector m_vMarkerPos;

    void RL_MarkerData(vector markerPos) {
        m_vMarkerPos = markerPos;
    }
}