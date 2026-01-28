# logos-chatsdk-ui Specification

## Overview

A Qt-based UI module for the Logos platform that provides a chat interface using the `logos-chatsdk-module` backend. This module follows the same architectural patterns as `logos-chat-ui` but implements a two-panel conversation-based chat interface.

## Architecture

### Module Structure

```
logos-chatsdk-ui/
├── app/                           # Standalone app (loads the plugin)
│   ├── CMakeLists.txt             # App build configuration
│   ├── main.cpp                   # App entry point (initializes Logos core)
│   ├── mainwindow.h               # App main window header
│   └── mainwindow.cpp             # App main window (loads plugin via QPluginLoader)
├── interfaces/
│   └── IComponent.h               # Component interface (same as logos-chat-ui)
├── src/                           # Plugin UI widgets
│   ├── main.cpp                   # Plugin entry point
│   ├── ChatSDKWindow.h            # Main window (QMainWindow)
│   ├── ChatSDKWindow.cpp
│   ├── ConversationListPanel.h    # Left panel widget
│   ├── ConversationListPanel.cpp
│   ├── ChatPanel.h                # Right panel widget
│   ├── ChatPanel.cpp
│   ├── MessageBubble.h            # Custom message display widget
│   ├── MessageBubble.cpp
│   └── MockData.h                 # Mock data for UI testing
├── nix/
│   ├── default.nix                # Common build configuration
│   ├── lib.nix                    # Library/plugin build
│   └── app.nix                    # Standalone app build
├── ChatSDKUIComponent.h           # Plugin component header
├── ChatSDKUIComponent.cpp         # Plugin component implementation
├── CMakeLists.txt                 # Root build configuration
├── metadata.json                  # Module metadata
├── flake.nix                      # Nix flake
├── flake.lock                     # Nix flake lock
├── run.sh                         # Build & run script
├── .gitignore                     # Git ignore file
└── spec.md                        # This file
```

**Total: 25 files**

### Dependencies

| Dependency | Purpose |
|------------|---------|
| `Qt6::Core` | Core Qt functionality |
| `Qt6::Widgets` | UI widgets |
| `Qt6::RemoteObjects` | For future LogosAPI integration |
| `logos-cpp-sdk` | LogosAPI, generator for module bindings |
| `logos-liblogos` | Core Logos library (logoscore, logos_host) |
| `logos-chatsdk-module` | Chat SDK backend module |

### Build Targets

| Target | Output | Description |
|--------|--------|-------------|
| `chatsdk_ui` (lib) | `chatsdk_ui.dylib` / `.so` | Qt plugin library |
| `logos-chatsdk-ui-app` (app) | `logos-chatsdk-ui-app` | Standalone executable |

---

## UI Layout

### Main Window

The main window (`ChatSDKWindow`) is split into two panels using a `QSplitter`:

```
┌─────────────────────────────────────────────────────────────────────┐
│  Menu Bar (File | Help)                                             │
├──────────────────────┬──────────────────────────────────────────────┤
│                      │                                              │
│   CONVERSATIONS      │          CHAT PANEL                          │
│   [+]                │                                              │
│                      │    Conversation Title                        │
│  ┌────────────────┐  │  ───────────────────────────────────────     │
│  │ Alice          │  │                                              │
│  │ 2 min ago      │  │     [Message from counterparty]              │
│  └────────────────┘  │      10:30 AM                                │
│                      │                                              │
│  ┌────────────────┐  │                        [My message]          │
│  │ Bob            │  │                         10:31 AM             │
│  │ 5 min ago      │  │                                              │
│  └────────────────┘  │     [Message from counterparty]              │
│                      │      10:32 AM                                │
│                      │                                              │
│                      │  ───────────────────────────────────────     │
│                      │  ┌──────────────────────────────┐  ┌──────┐  │
│                      │  │ Type a message...            │  │ Send │  │
│  ┌────────────────┐  │  └──────────────────────────────┘  └──────┘  │
│  │  [My Bundle]   │  │                                              │
│  └────────────────┘  │                                              │
├──────────────────────┴──────────────────────────────────────────────┤
│  Status Bar                                                         │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Component Specifications

### 1. ConversationListPanel (Left Panel)

**Class**: `ConversationListPanel : public QWidget`

#### Layout
- **Header**: Horizontal layout containing:
  - `QLabel` with text "Conversations" (bold, larger font)
  - `QPushButton` with "new conversation" icon (square with pen) - icon only, no text
- **Conversation List**: `QListWidget` showing all conversations
  - Each item displays:
    - Conversation name (bold)
    - Relative timestamp of last activity (e.g., "2 min ago", "Yesterday")
- **Footer**: `QPushButton` labeled "My Bundle" spanning full width

#### Signals
```cpp
signals:
    void conversationSelected(const QString& conversationId);
    void newConversationRequested();
    void myBundleRequested();
```

#### Slots
```cpp
public slots:
    void addConversation(const QString& id, const QString& name, const QDateTime& lastActivity);
    void updateConversation(const QString& id, const QDateTime& lastActivity);
    void removeConversation(const QString& id);
    void clearConversations();
```

#### Behavior
- Clicking a conversation item emits `conversationSelected(id)`
- Clicking the "+" icon emits `newConversationRequested()`
- Clicking "My Bundle" emits `myBundleRequested()`
- Selected conversation should be visually highlighted

---

### 2. ChatPanel (Right Panel)

**Class**: `ChatPanel : public QWidget`

#### Layout (No Conversation Selected)
- Centered `QLabel` with text "Select a conversation or start a new one"
- Gray, italic styling

#### Layout (Conversation Selected)
- **Header**: `QLabel` with conversation name as title (bold, larger font)
- **Messages Area**: `QScrollArea` containing `QVBoxLayout` with message bubbles
  - Scrolls to bottom when new messages arrive
- **Input Area**: Horizontal layout containing:
  - `QLineEdit` for message input (placeholder: "Type a message...")
  - `QPushButton` labeled "Send"

#### Message Display
Each message is displayed using `MessageBubble` widget:
- **My messages**: Right-aligned, colored background (e.g., light blue #DCF8C6)
- **Counterparty messages**: Left-aligned, neutral background (e.g., white/light gray #FFFFFF)
- **Timestamp**: Small, gray text below message content

#### Signals
```cpp
signals:
    void messageSent(const QString& conversationId, const QString& content);
```

#### Slots
```cpp
public slots:
    void setConversation(const QString& id, const QString& name);
    void clearConversation();
    void addMessage(const QString& sender, const QString& content, 
                    const QDateTime& timestamp, bool isMe);
    void clearMessages();
```

#### Behavior
- Send button click or Enter key press:
  1. Validates message is not empty
  2. Emits `messageSent(conversationId, content)`
  3. Clears input field
- Messages auto-scroll to bottom on new message arrival
- Input is disabled when no conversation is selected

---

### 3. MessageBubble Widget

**Class**: `MessageBubble : public QWidget`

#### Properties
- `QString content` - The message text
- `QDateTime timestamp` - When the message was sent
- `bool isMe` - Whether this message is from the current user

#### Visual Design
```
My Message (right-aligned):
                                    ┌─────────────────────┐
                                    │ Hello, how are you? │
                                    └─────────────────────┘
                                              10:31 AM

Counterparty Message (left-aligned):
┌─────────────────────────┐
│ I'm doing great, thanks!│
└─────────────────────────┘
        10:32 AM
```

#### Styling
- Border radius: 12px
- Padding: 10px
- Max width: 70% of chat panel width
- My messages: Background `#DCF8C6`, aligned right
- Counterparty: Background `#FFFFFF`, aligned left, subtle border
- Timestamp: Font size 10px, color `#888888`

---

### 4. ChatSDKWindow (Main Window)

**Class**: `ChatSDKWindow : public QMainWindow`

#### Menu Structure
- **File**
  - Exit (`Ctrl+Q`)
- **Help**
  - About

#### Components
- `ConversationListPanel* conversationList`
- `ChatPanel* chatPanel`
- `QSplitter* splitter` (horizontal, to allow resizing panels)
- `QStatusBar* statusBar`

#### Dialog Handlers

##### New Conversation Dialog
When `newConversationRequested()` is received:
```cpp
void onNewConversationRequested() {
    bool ok;
    QString bundle = QInputDialog::getText(this, 
        "New Conversation",
        "Paste counterparty's intro bundle:",
        QLineEdit::Normal, "", &ok);
    if (ok && !bundle.isEmpty()) {
        // Future: Call chatsdk_module.newPrivateConversation()
        // For now: Show placeholder message
        QMessageBox::information(this, "Info", 
            "New conversation will be created with bundle:\n" + bundle);
    }
}
```

##### My Bundle Dialog
When `myBundleRequested()` is received:
```cpp
void onMyBundleRequested() {
    // Future: Get bundle from chatsdk_module.createIntroBundle()
    QString mockBundle = "logos://bundle/abc123xyz789...";
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("My Bundle");
    msgBox.setText("Share this bundle with others to start a conversation:");
    msgBox.setInformativeText(mockBundle);
    msgBox.setStandardButtons(QMessageBox::Ok);
    
    QPushButton* copyBtn = msgBox.addButton("Copy to Clipboard", QMessageBox::ActionRole);
    msgBox.exec();
    
    if (msgBox.clickedButton() == copyBtn) {
        QGuiApplication::clipboard()->setText(mockBundle);
        statusBar->showMessage("Bundle copied to clipboard", 3000);
    }
}
```

---

### 5. ChatSDKUIComponent (Plugin)

**Class**: `ChatSDKUIComponent : public QObject, public IComponent`

```cpp
class ChatSDKUIComponent : public QObject, public IComponent {
    Q_OBJECT
    Q_INTERFACES(IComponent)
    Q_PLUGIN_METADATA(IID IComponent_iid FILE "metadata.json")

public:
    Q_INVOKABLE QWidget* createWidget(LogosAPI* logosAPI = nullptr) override;
    void destroyWidget(QWidget* widget) override;
};
```

---

## Mock Data

For initial UI testing, include mock data in `MockData.h`:

```cpp
namespace MockData {
    struct Message {
        QString sender;
        QString content;
        QDateTime timestamp;
        bool isMe;
    };

    struct Conversation {
        QString id;
        QString name;
        QDateTime lastActivity;
        QList<Message> messages;
    };

    inline Conversation getSampleConversation() {
        Conversation convo;
        convo.id = "conv-001";
        convo.name = "Alice";
        convo.lastActivity = QDateTime::currentDateTime().addSecs(-120); // 2 min ago
        
        QDateTime baseTime = QDateTime::currentDateTime().addSecs(-300);
        
        convo.messages = {
            {"Alice", "Hey! How are you?", baseTime, false},
            {"Me", "I'm doing great, thanks for asking!", baseTime.addSecs(30), true},
            {"Alice", "Did you see the new Logos update?", baseTime.addSecs(60), false},
            {"Me", "Yes! The chat SDK looks amazing", baseTime.addSecs(90), true},
            {"Alice", "Right? Can't wait to try the private messaging", baseTime.addSecs(120), false}
        };
        
        return convo;
    }
}
```

---

## Styling Guidelines

### Colors
| Element | Color Code |
|---------|------------|
| My message background | `#DCF8C6` |
| Counterparty message background | `#FFFFFF` |
| Counterparty message border | `#E0E0E0` |
| Timestamp text | `#888888` |
| Header background | `#F5F5F5` |
| Selected conversation | `#E3F2FD` |
| Panel divider | `#DDDDDD` |

### Fonts
- **Headers**: Bold, 14pt
- **Conversation name**: Bold, 12pt
- **Conversation timestamp**: Normal, 10pt, gray
- **Message content**: Normal, 12pt
- **Message timestamp**: Normal, 10pt, gray

### Dimensions
- Minimum window size: 800x600
- Default left panel width: 250px
- Minimum left panel width: 200px
- Maximum message bubble width: 70% of chat panel
- Message bubble border radius: 12px
- Message bubble padding: 10px

---

## Build Configuration

### metadata.json

```json
{
  "name": "chatsdk_ui",
  "version": "1.0.0",
  "description": "Chat SDK UI for Logos - Private messaging interface",
  "author": "Logos Core Team",
  "type": "ui",
  "main": "chatsdk_ui",
  "dependencies": ["chatsdk_module"],
  "category": "chat",
  "build": {
    "type": "cmake",
    "files": [
      "src/ChatSDKWindow.cpp",
      "src/ChatSDKWindow.h",
      "src/ConversationListPanel.cpp",
      "src/ConversationListPanel.h",
      "src/ChatPanel.cpp",
      "src/ChatPanel.h",
      "src/MessageBubble.cpp",
      "src/MessageBubble.h"
    ]
  },
  "capabilities": [
    "ui_components",
    "private_messaging"
  ]
}
```

### CMakeLists.txt Key Points

```cmake
cmake_minimum_required(VERSION 3.16)
project(ChatSDKUIPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    ChatSDKUIComponent.cpp
    ChatSDKUIComponent.h
    src/ChatSDKWindow.cpp
    src/ChatSDKWindow.h
    src/ConversationListPanel.cpp
    src/ConversationListPanel.h
    src/ChatPanel.cpp
    src/ChatPanel.h
    src/MessageBubble.cpp
    src/MessageBubble.h
    src/MockData.h
    src/main.cpp
)

add_library(chatsdk_ui SHARED ${SOURCES})

target_link_libraries(chatsdk_ui PRIVATE
    Qt6::Core
    Qt6::Widgets
    component-interfaces
)
```

---

## Implementation Phases

### Phase 1: UI Skeleton (Current)
- [x] Create spec document
- [x] Create project structure
- [x] Implement `MessageBubble` widget
- [x] Implement `ConversationListPanel` 
- [x] Implement `ChatPanel`
- [x] Implement `ChatSDKWindow`
- [x] Implement `ChatSDKUIComponent`
- [x] Add mock data and verify UI renders correctly
- [x] Setup CMakeLists.txt and build configuration
- [x] Setup flake.nix

### Phase 2: Backend Integration (Future)
- [ ] Connect to `logos-chatsdk-module`
- [ ] Implement `initChat`, `startChat`, `stopChat`
- [ ] Implement `listConversations` → populate conversation list
- [ ] Implement `getConversation` → load messages
- [ ] Implement `sendMessage` → send messages
- [ ] Handle `eventResponse` signals for real-time updates

### Phase 3: Identity & Bundle (Future)
- [ ] Implement `getIdentity` → display user info
- [ ] Implement `createIntroBundle` → "My Bundle" feature
- [ ] Implement `newPrivateConversation` → start new chats

---

## How to Run

### Option 1: Using run.sh (Development)

```bash
cd logos-chatsdk-ui
./run.sh
```

This script:
1. Creates a build directory if it doesn't exist
2. Runs CMake configuration
3. Builds the project
4. Runs the standalone application

### Option 2: Using Nix (Recommended)

```bash
# Build and run the app
nix run .#app

# Or build just the library
nix build .#lib

# Build the default package (library)
nix build

# Enter development shell
nix develop
```

### Option 3: Manual CMake Build

```bash
cd logos-chatsdk-ui
mkdir build && cd build
cmake .. -DLOGOS_CPP_SDK_ROOT=/path/to/logos-cpp-sdk \
         -DLOGOS_LIBLOGOS_ROOT=/path/to/logos-liblogos
make -j$(nproc)

# Run the app
./bin/logos-chatsdk-ui-app
```

---

## Nix Flake Configuration

### Inputs

```nix
inputs = {
  nixpkgs.follows = "logos-liblogos/nixpkgs";
  logos-cpp-sdk.url = "github:logos-co/logos-cpp-sdk";
  logos-liblogos.url = "github:logos-co/logos-liblogos";
  logos-chatsdk-module.url = "github:logos-co/logos-chatsdk-module";
};
```

### Outputs

| Output | Description |
|--------|-------------|
| `packages.${system}.lib` | The plugin library (`chatsdk_ui.dylib`/`.so`) |
| `packages.${system}.app` | The standalone application |
| `packages.${system}.default` | Same as `lib` |
| `devShells.${system}.default` | Development shell with all dependencies |

---

## App Directory Details

The `app/` directory contains a minimal standalone application that:

1. **Initializes Qt** - Creates `QApplication`
2. **Sets up plugins directory** - Points to `../modules` relative to executable
3. **Starts Logos core** - Calls `logos_core_start()`
4. **Loads the chatsdk_ui plugin** - Uses `QPluginLoader` to load the plugin
5. **Creates main window** - Instantiates the plugin widget via `createWidget()`
6. **Runs event loop** - `app.exec()`
7. **Cleans up** - Calls `logos_core_cleanup()` on exit

This follows the exact same pattern as `logos-chat-ui/app/`.

---

## Implementation Order

1. Create directory structure (`mkdir -p`)
2. Copy interface (`IComponent.h` from logos-chat-ui)
3. Create `MessageBubble` - Simplest widget, no dependencies
4. Create `ConversationListPanel` - Left panel with signals
5. Create `ChatPanel` - Right panel, uses MessageBubble
6. Create `ChatSDKWindow` - Main window, connects panels
7. Create `MockData.h` - Sample conversation data
8. Create `ChatSDKUIComponent` - Plugin wrapper
9. Create `src/main.cpp` - Simple entry point
10. Create root `CMakeLists.txt` - Build config
11. Create `metadata.json` - Module metadata
12. Create `app/` files - Standalone application
13. Create `nix/` files - Nix build configuration
14. Create `flake.nix` - Nix flake
15. Create `run.sh` and `.gitignore` - Utility files
16. Test build and verify UI

---

## Notes

- The UI is designed to be functional independently with mock data before backend integration
- All backend calls are currently stubbed and will be implemented in Phase 2
- The module follows the same patterns as `logos-chat-ui` for consistency
- Qt signals/slots are used for component communication to maintain loose coupling
- The standalone app requires `logos-liblogos` for the core runtime
- The plugin can also be loaded by other Logos applications
