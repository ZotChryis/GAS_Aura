#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	bHasScriptImplementedTick = true;
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
