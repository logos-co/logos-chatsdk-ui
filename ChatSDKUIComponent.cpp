#include "ChatSDKUIComponent.h"
#include "src/ChatSDKWindow.h"

QWidget* ChatSDKUIComponent::createWidget(LogosAPI* logosAPI) {
    // Pass LogosAPI to ChatSDKWindow for chatsdk module integration
    return new ChatSDKWindow(logosAPI);
}

void ChatSDKUIComponent::destroyWidget(QWidget* widget) {
    delete widget;
}
