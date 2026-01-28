#include "ChatPanel.h"
#include "MessageBubble.h"
#include <QStackedWidget>
#include <QScrollBar>
#include <QTimer>

ChatPanel::ChatPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void ChatPanel::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_stackedWidget = new QStackedWidget(this);

    setupEmptyState();
    setupChatState();

    m_stackedWidget->addWidget(m_emptyStateWidget);
    m_stackedWidget->addWidget(m_chatStateWidget);
    m_stackedWidget->setCurrentWidget(m_emptyStateWidget);

    m_mainLayout->addWidget(m_stackedWidget);
}

void ChatPanel::setupEmptyState()
{
    m_emptyStateWidget = new QWidget(this);
    m_emptyStateWidget->setStyleSheet("background-color: #1c1c1e;");
    QVBoxLayout* emptyLayout = new QVBoxLayout(m_emptyStateWidget);

    m_emptyStateLabel = new QLabel("Select a conversation or start a new one", m_emptyStateWidget);
    m_emptyStateLabel->setAlignment(Qt::AlignCenter);
    QFont emptyFont = m_emptyStateLabel->font();
    emptyFont.setPointSize(14);
    emptyFont.setItalic(true);
    m_emptyStateLabel->setFont(emptyFont);
    m_emptyStateLabel->setStyleSheet("color: #8e8e93; background: transparent;");

    emptyLayout->addStretch();
    emptyLayout->addWidget(m_emptyStateLabel);
    emptyLayout->addStretch();
}

void ChatPanel::setupChatState()
{
    m_chatStateWidget = new QWidget(this);
    m_chatLayout = new QVBoxLayout(m_chatStateWidget);
    m_chatLayout->setContentsMargins(0, 0, 0, 0);
    m_chatLayout->setSpacing(0);

    // Title header - dark theme
    QWidget* headerWidget = new QWidget(m_chatStateWidget);
    headerWidget->setStyleSheet("background-color: #2c2c2e; border-bottom: 1px solid #3a3a3c;");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(15, 12, 15, 12);

    m_titleLabel = new QLabel("", headerWidget);
    m_titleLabel->setStyleSheet("color: #ffffff; background: transparent;");
    QFont titleFont = m_titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    m_titleLabel->setFont(titleFont);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    // Messages scroll area - dark theme
    m_scrollArea = new QScrollArea(m_chatStateWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet(
        "QScrollArea {"
        "  border: none;"
        "  background-color: #1c1c1e;"
        "}"
        "QScrollBar:vertical {"
        "  background-color: #1c1c1e;"
        "  width: 8px;"
        "  margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background-color: #4a4a4c;"
        "  border-radius: 4px;"
        "  min-height: 20px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "}"
    );

    m_messagesContainer = new QWidget(m_scrollArea);
    m_messagesContainer->setStyleSheet("background-color: #1c1c1e;");
    m_messagesLayout = new QVBoxLayout(m_messagesContainer);
    m_messagesLayout->setContentsMargins(10, 10, 10, 10);
    m_messagesLayout->setSpacing(5);
    m_messagesLayout->addStretch();

    m_scrollArea->setWidget(m_messagesContainer);

    // Input area - dark theme
    m_inputWidget = new QWidget(m_chatStateWidget);
    m_inputWidget->setStyleSheet("background-color: #2c2c2e; border-top: 1px solid #3a3a3c;");
    m_inputLayout = new QHBoxLayout(m_inputWidget);
    m_inputLayout->setContentsMargins(15, 10, 15, 10);
    m_inputLayout->setSpacing(10);

    m_messageInput = new QLineEdit(m_inputWidget);
    m_messageInput->setPlaceholderText("Type a message...");
    m_messageInput->setStyleSheet(
        "QLineEdit {"
        "  border: 1px solid #4a4a4c;"
        "  border-radius: 20px;"
        "  padding: 10px 15px;"
        "  background-color: #3a3a3c;"
        "  color: #ffffff;"
        "  font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "  border-color: #0a84ff;"
        "}"
        "QLineEdit::placeholder {"
        "  color: #8e8e93;"
        "}"
    );

    m_sendButton = new QPushButton("Send", m_inputWidget);
    m_sendButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #0a84ff;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 20px;"
        "  padding: 10px 20px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #409cff;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #0060df;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #4a4a4c;"
        "  color: #8e8e93;"
        "}"
    );

    m_inputLayout->addWidget(m_messageInput, 1);
    m_inputLayout->addWidget(m_sendButton);

    // Add widgets to chat layout
    m_chatLayout->addWidget(headerWidget);
    m_chatLayout->addWidget(m_scrollArea, 1);
    m_chatLayout->addWidget(m_inputWidget);

    // Connect signals
    connect(m_sendButton, &QPushButton::clicked, this, &ChatPanel::onSendClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatPanel::onReturnPressed);

    // Initially disable input
    m_messageInput->setEnabled(false);
    m_sendButton->setEnabled(false);
}

void ChatPanel::setConversation(const QString& id, const QString& name)
{
    m_currentConversationId = id;
    m_currentConversationName = name;
    m_titleLabel->setText(name);

    // Enable input
    m_messageInput->setEnabled(true);
    m_sendButton->setEnabled(true);
    m_messageInput->setFocus();

    // Show chat state
    m_stackedWidget->setCurrentWidget(m_chatStateWidget);
}

void ChatPanel::clearConversation()
{
    m_currentConversationId.clear();
    m_currentConversationName.clear();
    m_titleLabel->setText("");

    // Disable input
    m_messageInput->setEnabled(false);
    m_sendButton->setEnabled(false);
    m_messageInput->clear();

    // Clear messages
    clearMessages();

    // Show empty state
    m_stackedWidget->setCurrentWidget(m_emptyStateWidget);
}

void ChatPanel::addMessage(const QString& sender, const QString& content, 
                           const QDateTime& timestamp, bool isMe)
{
    Q_UNUSED(sender);
    
    // Insert message before the stretch
    int insertIndex = m_messagesLayout->count() - 1;
    if (insertIndex < 0) insertIndex = 0;

    MessageBubble* bubble = new MessageBubble(content, timestamp, isMe, m_messagesContainer);
    m_messagesLayout->insertWidget(insertIndex, bubble);

    // Scroll to bottom after a short delay to ensure layout is updated
    QTimer::singleShot(10, this, &ChatPanel::scrollToBottom);
}

void ChatPanel::clearMessages()
{
    // Remove all widgets except the stretch
    while (m_messagesLayout->count() > 1) {
        QLayoutItem* item = m_messagesLayout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void ChatPanel::onSendClicked()
{
    QString content = m_messageInput->text().trimmed();
    if (content.isEmpty() || m_currentConversationId.isEmpty()) {
        return;
    }

    emit messageSent(m_currentConversationId, content);

    // Add the message to the UI immediately (optimistic update)
    addMessage("Me", content, QDateTime::currentDateTime(), true);

    m_messageInput->clear();
    m_messageInput->setFocus();
}

void ChatPanel::onReturnPressed()
{
    onSendClicked();
}

void ChatPanel::scrollToBottom()
{
    QScrollBar* scrollBar = m_scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
