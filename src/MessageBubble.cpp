#include "MessageBubble.h"
#include <QHBoxLayout>
#include <QFrame>

MessageBubble::MessageBubble(const QString& content, const QDateTime& timestamp, 
                             bool isMe, QWidget* parent)
    : QWidget(parent)
    , m_content(content)
    , m_timestamp(timestamp)
    , m_isMe(isMe)
    , m_contentLabel(nullptr)
    , m_timestampLabel(nullptr)
{
    setupUI();
}

void MessageBubble::setupUI()
{
    // Main horizontal layout for alignment
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->setSpacing(0);

    // Container frame for the bubble (QFrame works better with border-radius)
    QFrame* bubbleContainer = new QFrame(this);
    bubbleContainer->setFrameShape(QFrame::NoFrame);
    bubbleContainer->setAutoFillBackground(true);
    QVBoxLayout* bubbleLayout = new QVBoxLayout(bubbleContainer);
    bubbleLayout->setContentsMargins(14, 10, 14, 10);
    bubbleLayout->setSpacing(4);

    // Content label
    m_contentLabel = new QLabel(m_content, bubbleContainer);
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_contentLabel->setMinimumWidth(100);
    
    QFont contentFont = m_contentLabel->font();
    contentFont.setPointSize(13);
    m_contentLabel->setFont(contentFont);

    // Timestamp label
    m_timestampLabel = new QLabel(m_timestamp.toString("h:mm AP"), bubbleContainer);
    QFont timestampFont = m_timestampLabel->font();
    timestampFont.setPointSize(10);
    m_timestampLabel->setFont(timestampFont);

    bubbleLayout->addWidget(m_contentLabel);
    bubbleLayout->addWidget(m_timestampLabel);

    // Spacer widget for the empty half
    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    // Bubble container should expand but not beyond half
    bubbleContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Style the bubble based on sender
    if (m_isMe) {
        // My message - right aligned, blue background (iOS style)
        bubbleContainer->setStyleSheet(
            "QFrame {"
            "  background-color: #0a84ff;"
            "  border-radius: 16px;"
            "}"
            "QFrame QLabel {"
            "  background: transparent;"
            "}"
        );
        m_contentLabel->setStyleSheet("QLabel { color: #ffffff; background: transparent; }");
        m_timestampLabel->setStyleSheet("QLabel { color: rgba(255, 255, 255, 0.7); background: transparent; }");
        m_timestampLabel->setAlignment(Qt::AlignRight);
        // Spacer on left (50%), bubble on right (50%)
        mainLayout->addWidget(spacer, 1);
        mainLayout->addWidget(bubbleContainer, 1);
    } else {
        // Counterparty message - left aligned, dark gray background
        bubbleContainer->setStyleSheet(
            "QFrame {"
            "  background-color: #3a3a3c;"
            "  border-radius: 16px;"
            "}"
            "QFrame QLabel {"
            "  background: transparent;"
            "}"
        );
        m_contentLabel->setStyleSheet("QLabel { color: #ffffff; background: transparent; }");
        m_timestampLabel->setStyleSheet("QLabel { color: #8e8e93; background: transparent; }");
        m_timestampLabel->setAlignment(Qt::AlignLeft);
        // Bubble on left (50%), spacer on right (50%)
        mainLayout->addWidget(bubbleContainer, 1);
        mainLayout->addWidget(spacer, 1);
    }

    // Set reasonable min/max widths for the bubble
    bubbleContainer->setMinimumWidth(120);
}
