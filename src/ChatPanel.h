#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QDateTime>

class ChatPanel : public QWidget {
    Q_OBJECT

public:
    explicit ChatPanel(QWidget* parent = nullptr);
    ~ChatPanel() = default;

signals:
    void messageSent(const QString& conversationId, const QString& content);

public slots:
    void setConversation(const QString& id, const QString& name);
    void clearConversation();
    void addMessage(const QString& sender, const QString& content, 
                    const QDateTime& timestamp, bool isMe);
    void clearMessages();

private slots:
    void onSendClicked();
    void onReturnPressed();

private:
    void setupUI();
    void setupEmptyState();
    void setupChatState();
    void scrollToBottom();

    QString m_currentConversationId;
    QString m_currentConversationName;

    // Main layouts
    QVBoxLayout* m_mainLayout;
    QStackedWidget* m_stackedWidget;

    // Empty state widgets
    QWidget* m_emptyStateWidget;
    QLabel* m_emptyStateLabel;

    // Chat state widgets
    QWidget* m_chatStateWidget;
    QVBoxLayout* m_chatLayout;
    QLabel* m_titleLabel;
    QScrollArea* m_scrollArea;
    QWidget* m_messagesContainer;
    QVBoxLayout* m_messagesLayout;
    QWidget* m_inputWidget;
    QHBoxLayout* m_inputLayout;
    QLineEdit* m_messageInput;
    QPushButton* m_sendButton;
};
