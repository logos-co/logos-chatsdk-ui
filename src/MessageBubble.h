#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDateTime>

class MessageBubble : public QWidget {
    Q_OBJECT

public:
    explicit MessageBubble(const QString& content, const QDateTime& timestamp, 
                           bool isMe, QWidget* parent = nullptr);
    ~MessageBubble() = default;

    QString content() const { return m_content; }
    QDateTime timestamp() const { return m_timestamp; }
    bool isMe() const { return m_isMe; }

private:
    void setupUI();

    QString m_content;
    QDateTime m_timestamp;
    bool m_isMe;

    QLabel* m_contentLabel;
    QLabel* m_timestampLabel;
};
