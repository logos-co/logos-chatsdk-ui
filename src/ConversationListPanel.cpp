#include "ConversationListPanel.h"
#include <QFont>

ConversationListPanel::ConversationListPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void ConversationListPanel::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Set dark background for the entire panel
    setStyleSheet("background-color: #1c1c1e;");

    // Header with title and new conversation button
    QWidget* headerWidget = new QWidget(this);
    headerWidget->setStyleSheet("background-color: #2c2c2e; border-bottom: 1px solid #3a3a3c;");
    m_headerLayout = new QHBoxLayout(headerWidget);
    m_headerLayout->setContentsMargins(15, 12, 15, 12);

    m_titleLabel = new QLabel("Conversations", headerWidget);
    m_titleLabel->setStyleSheet("color: #ffffff; background: transparent;");
    QFont titleFont = m_titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    m_titleLabel->setFont(titleFont);

    m_newConversationButton = new QPushButton(headerWidget);
    m_newConversationButton->setText("+");
    m_newConversationButton->setFixedSize(32, 32);
    m_newConversationButton->setToolTip("New Conversation");
    m_newConversationButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #0a84ff;"
        "  border: none;"
        "  border-radius: 16px;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  color: white;"
        "}"
        "QPushButton:hover {"
        "  background-color: #409cff;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #0060df;"
        "}"
    );

    m_headerLayout->addWidget(m_titleLabel);
    m_headerLayout->addStretch();
    m_headerLayout->addWidget(m_newConversationButton);

    // Conversation list
    m_conversationList = new QListWidget(this);
    m_conversationList->setStyleSheet(
        "QListWidget {"
        "  border: none;"
        "  background-color: #1c1c1e;"
        "}"
        "QListWidget::item {"
        "  padding: 12px 15px;"
        "  border-bottom: 1px solid #3a3a3c;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #3a3a3c;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: #2c2c2e;"
        "}"
    );

    // My Bundle button at bottom
    m_myBundleButton = new QPushButton("My Bundle", this);
    m_myBundleButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #2c2c2e;"
        "  border: none;"
        "  border-top: 1px solid #3a3a3c;"
        "  border-radius: 0px;"
        "  padding: 15px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "  color: #0a84ff;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3a3a3c;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #4a4a4c;"
        "}"
    );

    m_mainLayout->addWidget(headerWidget);
    m_mainLayout->addWidget(m_conversationList, 1);
    m_mainLayout->addWidget(m_myBundleButton);

    // Connect signals
    connect(m_newConversationButton, &QPushButton::clicked, 
            this, &ConversationListPanel::onNewConversationClicked);
    connect(m_myBundleButton, &QPushButton::clicked, 
            this, &ConversationListPanel::onMyBundleClicked);
    connect(m_conversationList, &QListWidget::itemClicked, 
            this, &ConversationListPanel::onItemClicked);
}

void ConversationListPanel::addConversation(const QString& id, const QString& name, 
                                             const QDateTime& lastActivity)
{
    if (m_conversationItems.contains(id)) {
        updateConversation(id, lastActivity);
        return;
    }

    QListWidgetItem* item = new QListWidgetItem(m_conversationList);
    item->setData(Qt::UserRole, id);
    updateConversationDisplay(item, name, lastActivity);

    m_conversationItems[id] = item;
    m_conversationData[id] = {name, lastActivity};
}

void ConversationListPanel::updateConversation(const QString& id, const QDateTime& lastActivity)
{
    if (!m_conversationItems.contains(id)) return;

    QListWidgetItem* item = m_conversationItems[id];
    m_conversationData[id].lastActivity = lastActivity;
    updateConversationDisplay(item, m_conversationData[id].name, lastActivity);
}

void ConversationListPanel::removeConversation(const QString& id)
{
    if (!m_conversationItems.contains(id)) return;

    QListWidgetItem* item = m_conversationItems[id];
    delete item;
    m_conversationItems.remove(id);
    m_conversationData.remove(id);
}

void ConversationListPanel::clearConversations()
{
    m_conversationList->clear();
    m_conversationItems.clear();
    m_conversationData.clear();
}

void ConversationListPanel::selectConversation(const QString& id)
{
    if (!m_conversationItems.contains(id)) return;
    m_conversationList->setCurrentItem(m_conversationItems[id]);
}

void ConversationListPanel::onItemClicked(QListWidgetItem* item)
{
    QString id = item->data(Qt::UserRole).toString();
    emit conversationSelected(id);
}

void ConversationListPanel::onNewConversationClicked()
{
    emit newConversationRequested();
}

void ConversationListPanel::onMyBundleClicked()
{
    emit myBundleRequested();
}

QString ConversationListPanel::formatRelativeTime(const QDateTime& dateTime)
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 secs = dateTime.secsTo(now);

    if (secs < 60) {
        return "Just now";
    } else if (secs < 3600) {
        int mins = secs / 60;
        return QString("%1 min ago").arg(mins);
    } else if (secs < 86400) {
        int hours = secs / 3600;
        return QString("%1 hour%2 ago").arg(hours).arg(hours > 1 ? "s" : "");
    } else if (secs < 172800) {
        return "Yesterday";
    } else {
        return dateTime.toString("MMM d");
    }
}

void ConversationListPanel::updateConversationDisplay(QListWidgetItem* item, 
                                                       const QString& name, 
                                                       const QDateTime& lastActivity)
{
    QString displayText = QString("<b style='color: #ffffff;'>%1</b><br><span style='color: #8e8e93; font-size: 10pt;'>%2</span>")
                          .arg(name)
                          .arg(formatRelativeTime(lastActivity));
    
    QLabel* label = new QLabel(displayText);
    label->setTextFormat(Qt::RichText);
    label->setContentsMargins(0, 0, 0, 0);
    label->setStyleSheet("background: transparent;");
    
    item->setSizeHint(QSize(0, 50));
    m_conversationList->setItemWidget(item, label);
}
