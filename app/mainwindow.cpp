#include "mainwindow.h"
#include "databasemodel.h"
#include "iconnames.h"
#include "item.h"
#include "queuemodel.h"

#include <QApplication>
#include <QAbstractItemView>
#include <QEvent>
#include <QFrame>
#include <QFont>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScroller>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSettings>
#include <QSlider>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleHints>
#include <QTouchEvent>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include <memory>

namespace
{
class TrackInfoCard : public QFrame
{
public:
    explicit TrackInfoCard(QWidget *parent)
        : QFrame(parent)
    {
        setObjectName("trackInfoCard");
        setFrameShape(QFrame::StyledPanel);
        setStyleSheet(R"(
            QFrame#trackInfoCard {
                background: palette(window);
                color: palette(window-text);
                border: 2px solid palette(mid);
                border-radius: 16px;
            }
            QLabel#trackTitle {
                font-size: 26px;
                font-weight: bold;
                border: none;
            }
            QLabel#trackArtist {
                font-size: 20px;
                color: palette(highlight);
                border: none;
            }
            QLabel#trackDetails {
                font-size: 17px;
                border: none;
            }
            QPushButton {
                font-size: 24px;
                font-weight: bold;
                min-width: 56px;
                min-height: 56px;
                border: 1px solid palette(mid);
                border-radius: 12px;
                background: palette(button);
            }
        )");

        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(20, 16, 20, 16);
        layout->setSpacing(12);

        auto heading = new QHBoxLayout();
        auto titleLayout = new QVBoxLayout();
        m_title = new QLabel(this);
        m_title->setObjectName("trackTitle");
        m_title->setWordWrap(true);
        m_artist = new QLabel(this);
        m_artist->setObjectName("trackArtist");
        m_artist->setWordWrap(true);
        titleLayout->addWidget(m_title);
        titleLayout->addWidget(m_artist);

        auto closeButton = new QPushButton(QString::fromUtf8("×"), this);
        closeButton->setAccessibleName(tr("Close song information"));
        closeButton->setToolTip(tr("Close"));
        connect(closeButton, &QPushButton::clicked, this, &QWidget::hide);
        heading->addLayout(titleLayout, 1);
        heading->addWidget(closeButton, 0, Qt::AlignTop);
        layout->addLayout(heading);

        auto separator = new QFrame(this);
        separator->setFrameShape(QFrame::HLine);
        separator->setStyleSheet("color: palette(mid);");
        layout->addWidget(separator);

        auto detailsScroll = new QScrollArea(this);
        detailsScroll->setWidgetResizable(true);
        detailsScroll->setFrameShape(QFrame::NoFrame);
        detailsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_details = new QLabel(detailsScroll);
        m_details->setObjectName("trackDetails");
        m_details->setWordWrap(true);
        m_details->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        detailsScroll->setWidget(m_details);
        layout->addWidget(detailsScroll, 1);

        m_playButton = new QPushButton(tr("Play"), this);
        m_playButton->setObjectName("playSongButton");
        m_playButton->setIcon(QIcon::fromTheme(IconNames::Start));
        m_playButton->setMinimumHeight(72);
        m_playButton->setStyleSheet(R"(
            QPushButton#playSongButton {
                font-size: 24px;
                font-weight: bold;
                min-height: 72px;
                border: 1px solid palette(highlight);
                border-radius: 12px;
                background: palette(highlight);
                color: palette(highlighted-text);
            }
        )");
        layout->addWidget(m_playButton);
    }

    void setTrackInfo(unsigned songId, const QString &title, const QString &artist, const QString &metadata)
    {
        m_songId = songId;
        m_title->setText(title);
        m_artist->setText(artist.isEmpty() ? tr("Unknown artist") : artist);

        QStringList details;
        const auto lines = metadata.split('\n', Qt::SkipEmptyParts);
        for (const auto &line : lines) {
            const auto separator = line.indexOf(": ");
            if (separator < 0) {
                continue;
            }

            const auto field = line.left(separator);
            if (field == "Title" || field == "Artist" || field == "Album Artist") {
                continue;
            }

            details.append(QStringLiteral("%1: %2").arg(field, line.mid(separator + 2)));
        }
        m_details->setText(details.join('\n'));
    }

    unsigned songId() const
    {
        return m_songId;
    }

    QPushButton *playButton() const
    {
        return m_playButton;
    }

private:
    QLabel *m_title{};
    QLabel *m_artist{};
    QLabel *m_details{};
    QPushButton *m_playButton{};
    unsigned m_songId{};
};

class TrackInfoDismissFilter : public QObject
{
public:
    TrackInfoDismissFilter(QWidget *card, QObject *parent)
        : QObject(parent)
        , m_card(card)
    {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched)
        if (!m_card->isVisible()) {
            return false;
        }

        QPoint globalPosition;
        if (event->type() == QEvent::MouseButtonPress) {
            globalPosition = static_cast<QMouseEvent *>(event)->globalPosition().toPoint();
        } else if (event->type() == QEvent::TouchBegin) {
            const auto points = static_cast<QTouchEvent *>(event)->points();
            if (points.isEmpty()) {
                return false;
            }
            globalPosition = points.first().globalPosition().toPoint();
        } else {
            return false;
        }

        const QRect cardRect(m_card->mapToGlobal(QPoint(0, 0)), m_card->size());
        if (!cardRect.contains(globalPosition)) {
            m_card->hide();
        }
        return false;
    }

private:
    QWidget *m_card;
};

class QueueTreeView : public QTreeView
{
public:
    using QTreeView::QTreeView;

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QTreeView::resizeEvent(event);
        if (model() && header()->count() >= 2) {
            header()->resizeSection(0, viewport()->width() / 3);
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        m_touchInput = event->source() != Qt::MouseEventNotSynthesized;
        QTreeView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        m_touchInput = event->source() != Qt::MouseEventNotSynthesized;
        QTreeView::mouseMoveEvent(event);
    }

    void startDrag(Qt::DropActions supportedActions) override
    {
        if (!m_touchInput) {
            QTreeView::startDrag(supportedActions);
        }
    }

private:
    bool m_touchInput = false;
};
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Quetzalcoatl"));
    resize(800, 480);

    // Include the system and bundled icon themes so embedded Linux deployments can use Breeze.
    auto themeSearchPaths = QIcon::themeSearchPaths();
    themeSearchPaths << "/usr/share/icons" << ":/icons";
    QIcon::setThemeSearchPaths(themeSearchPaths);

    // On non-Linux system, we use the bundled Breeze icons. They need to change as the system switches between
    // Light and Dark mode.
    if (!iconsAreThere()) {
        setIconTheme(qApp->styleHints()->colorScheme());
        connect(qApp->styleHints(),
                &QStyleHints::colorSchemeChanged,
                this,
                &MainWindow::setIconTheme);
    }

    setWindowIcon(QIcon::fromTheme(IconNames::Player));

    m_controller = new Controller(this);
    connect(m_controller, &Controller::connectionState, this, &MainWindow::setConnectionState);

    auto toolBar = addToolBar("ToolBar");
    toolBar->setMovable(false);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setMinimumHeight(100);
    toolBar->setIconSize(QSize(52, 52));

    auto stopAction = toolBar->addAction(QIcon::fromTheme(IconNames::Stop), "Stop");
    stopAction->setShortcut(Qt::Key::Key_MediaStop);
    stopAction->setEnabled(false);
    connect(stopAction, &QAction::triggered, m_controller, &Controller::stop);
    m_connectedActions.append(stopAction);

    auto playAction = toolBar->addAction(QIcon::fromTheme(IconNames::Start), "Play");
    playAction->setShortcut(Qt::Key::Key_MediaPlay);
    playAction->setEnabled(false);
    connect(playAction, &QAction::triggered, m_controller, &Controller::play);
    m_connectedActions.append(playAction);

    auto pauseAction = toolBar->addAction(QIcon::fromTheme(IconNames::Pause), "Pause");
    pauseAction->setShortcut(Qt::Key::Key_MediaPause);
    pauseAction->setEnabled(false);
    connect(pauseAction, &QAction::triggered, m_controller, &Controller::pause);
    m_connectedActions.append(pauseAction);

    auto skipBackAction = toolBar->addAction(QIcon::fromTheme(IconNames::SkipBackward), "Previous");
    skipBackAction->setShortcut(Qt::Key::Key_MediaPrevious);
    skipBackAction->setEnabled(false);
    connect(skipBackAction, &QAction::triggered, m_controller, &Controller::skipBack);
    m_connectedActions.append(skipBackAction);

    auto skipForthAction = toolBar->addAction(QIcon::fromTheme(IconNames::SkipForward), "Next");
    skipForthAction->setShortcut(Qt::Key::Key_MediaNext);
    skipForthAction->setEnabled(false);
    connect(skipForthAction, &QAction::triggered, m_controller, &Controller::skipForth);
    m_connectedActions.append(skipForthAction);

    toolBar->addSeparator();

    auto randomAction = toolBar->addAction(QIcon::fromTheme(IconNames::Shuffle), "Random");
    randomAction->setCheckable(true);
    randomAction->setEnabled(false);
    connect(randomAction, &QAction::triggered, m_controller, &Controller::random);
    m_connectedActions.append(randomAction);

    auto repeatAction = toolBar->addAction(QIcon::fromTheme(IconNames::Repeat), "Repeat");
    repeatAction->setCheckable(true);
    repeatAction->setEnabled(false);
    connect(repeatAction, &QAction::triggered, m_controller, &Controller::repeat);

    m_connectedActions.append(repeatAction);

    toolBar->addSeparator();

    auto volumeDownButton = new QToolButton(toolBar);
    volumeDownButton->setText("-");
    volumeDownButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    volumeDownButton->setAccessibleName(tr("Decrease volume"));
    volumeDownButton->setToolTip(tr("Decrease volume by 5%"));
    auto volumeButtonFont = volumeDownButton->font();
    volumeButtonFont.setPointSize(20);
    volumeButtonFont.setBold(true);
    volumeDownButton->setFont(volumeButtonFont);
    volumeDownButton->setFixedSize(42, 54);
    toolBar->addWidget(volumeDownButton);
    m_connectedWidgets.append(volumeDownButton);

    // volume slider
    v_slider = new QSlider(Qt::Horizontal);

    v_slider->setTracking(false);
    v_slider->setMinimumHeight(48);
    v_slider->setRange(0, 100);
    v_slider->setMinimumWidth(100);
    v_slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    v_slider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 10px;
            border-radius: 5px;
            background: palette(mid);
        }
        QSlider::handle:horizontal {
            width: 22px;
            margin: -7px 0;
            border-radius: 11px;
            background: palette(button);
            border: 1px solid palette(dark);
        }
    )");
    m_connectedWidgets.append(v_slider);
    toolBar->addWidget(v_slider);

    connect(v_slider, &QSlider::valueChanged, [this]() {
        if (actionVolume) {
            m_controller->setVolume(v_slider->value());
            actionVolume = false;
        }
    });

    connect(v_slider, &QSlider::actionTriggered, [this]() { actionVolume = true; });

    auto volumeUpButton = new QToolButton(toolBar);
    volumeUpButton->setText("+");
    volumeUpButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    volumeUpButton->setAccessibleName(tr("Increase volume"));
    volumeUpButton->setToolTip(tr("Increase volume by 5%"));
    volumeUpButton->setFont(volumeButtonFont);
    volumeUpButton->setFixedSize(42, 54);
    toolBar->addWidget(volumeUpButton);
    m_connectedWidgets.append(volumeUpButton);

    auto adjustVolume = [this](int change) {
        const int volume = qBound(0, v_slider->value() + change, 100);
        if (volume != v_slider->value()) {
            v_slider->setValue(volume);
            m_controller->setVolume(static_cast<unsigned>(volume));
        }
    };
    connect(volumeDownButton, &QToolButton::clicked, this, [adjustVolume]() { adjustVolume(-5); });
    connect(volumeUpButton, &QToolButton::clicked, this, [adjustVolume]() { adjustVolume(5); });

    toolBar->addSeparator();

    // close button
    auto closeAction = toolBar->addAction(QIcon::fromTheme(IconNames::Close), "Close");

    connect(closeAction, &QAction::triggered, [=]() { close(); });
    auto layout = new QVBoxLayout();

    // Is the slider being dragged?
    bool isDragging = false;

    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setTracking(false);
    m_slider->setMinimumHeight(44);
    m_slider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 10px;
            border-radius: 5px;
            background: palette(mid);
        }
        QSlider::sub-page:horizontal {
            border-radius: 5px;
            background: palette(highlight);
        }
        QSlider::handle:horizontal {
            width: 22px;
            margin: -7px 0;
            border-radius: 11px;
            background: palette(button);
            border: 1px solid palette(dark);
        }
    )");

    connect(m_controller, &Controller::sliderMax, [=](int value) {
        if (!m_slider->isSliderDown()) {
            m_slider->setMaximum(value);

            // Skipping only works if you have a song playing.
            skipForthAction->setEnabled(value);
            skipBackAction->setEnabled(value);
        }
    });
    connect(m_controller, &Controller::sliderValue, [=](int value) {
        if (!m_slider->isSliderDown()) {
            m_slider->setValue(value);
        }
    });

    connect(m_controller, &Controller::statusMessage, statusBar(), &QStatusBar::showMessage);
    layout->addWidget(m_slider);
    m_slider->setEnabled(false);
    m_connectedWidgets.append(m_slider);

    auto splitter = new QSplitter();

    auto databaseModel = new DatabaseModel(m_controller);
    auto databaseView = new QTreeView();
    auto playlistFont = databaseView->font();
    playlistFont.setPointSizeF(playlistFont.pointSizeF() * 1.5);
    databaseView->setFont(playlistFont);
    connect(databaseView, &QTreeView::doubleClicked, databaseModel, &ItemModel::onDoubleClicked);
    databaseView->setHeaderHidden(true);
    databaseView->setModel(databaseModel);
    // Show the playlists directly, without the surrounding "Playlists" category.
    databaseView->setRootIndex(databaseModel->index(0, 0));
    databaseView->setRootIsDecorated(false);
    databaseView->setItemsExpandable(false);
    databaseView->setExpandsOnDoubleClick(false);
    databaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto isPlaylistIndex = [](const QModelIndex &index) {
        // Playlist items are direct children of the hidden Playlists category.
        return index.isValid() && index.parent().isValid() && !index.parent().parent().isValid();
    };

    auto longPressTimer = new QTimer(databaseView);
    longPressTimer->setSingleShot(true);
    auto longPressPlaylist = std::make_shared<QString>();
    connect(longPressTimer, &QTimer::timeout, [this, longPressPlaylist]() {
        m_controller->replaceQueueWithPlaylist(*longPressPlaylist);
    });

    connect(databaseView, &QTreeView::pressed, [longPressTimer, longPressPlaylist, isPlaylistIndex](const QModelIndex &index) {
        if (!isPlaylistIndex(index)) {
            longPressTimer->stop();
            return;
        }

        *longPressPlaylist = index.data().toString();
        longPressTimer->start(600);
    });
    connect(databaseView, &QTreeView::clicked, [longPressTimer]() {
        longPressTimer->stop();
    });

    connect(databaseView, &QTreeView::doubleClicked, [this, longPressTimer, isPlaylistIndex](const QModelIndex &index) {
        if (!isPlaylistIndex(index)) {
            return;
        }

        longPressTimer->stop();
        m_controller->replaceQueueWithPlaylist(index.data().toString());
    });

    databaseView->setFixedWidth(220);

    databaseView->setEnabled(false);
    databaseView->setSelectionMode(QTreeView::ExtendedSelection);
    databaseView->setDragEnabled(true);
    databaseView->setUniformRowHeights(true);

    m_connectedWidgets.append(databaseView);
    splitter->addWidget(databaseView);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    // https://stackoverflow.com/a/22198672/240515
    databaseView->setContextMenuPolicy(Qt::NoContextMenu);
    auto deletePlaylistAction = new QAction("Delete", this);

    // currentIndex does appear to be the right way to do it...
    // https://www.qtcentre.org/threads/19919-Custom-context-menu-in-QTreeView?p=97803#post97803

    connect(deletePlaylistAction, &QAction::triggered, [=]() {
        auto item = static_cast<Item *>(databaseView->currentIndex().internalPointer());
        m_controller->deletePlaylist(item->text(0));
    });

    auto renamePlaylistAction = new QAction("Rename", this);
    connect(renamePlaylistAction, &QAction::triggered, [=]() {
        databaseView->edit(databaseView->currentIndex());
    });

    connect(databaseView, &QTreeView::customContextMenuRequested, [=](const QPoint &point) {
        auto index = databaseView->indexAt(point);
        if (!index.isValid()) {
            return;
        }

        auto item = static_cast<Item *>(index.internalPointer());
        if (item->parent()->text(0) == "Playlists") {
            QMenu::exec({deletePlaylistAction, renamePlaylistAction},
                        databaseView->viewport()->mapToGlobal(point));
        }
    });

    auto queueModel = new QueueModel(m_controller);

    connect(m_slider, &QSlider::sliderMoved, [=](int value) { m_seekPosition = value; });
    connect(m_slider, &QSlider::sliderReleased, [=]() {
        m_controller->seek(queueModel->songId(), m_seekPosition);
    });

    auto queueView = new QueueTreeView();
    auto songFont = queueView->font();
    songFont.setPointSizeF(songFont.pointSizeF() * 1.25);
    queueView->setFont(songFont);

    queueView->setAcceptDrops(true);
    queueView->setDropIndicatorShown(true);
    queueView->setDragEnabled(true);
    queueView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    queueView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    queueView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    queueView->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    QScroller::grabGesture(queueView->viewport(), QScroller::TouchGesture);
    queueView->setModel(queueModel);
    auto queueHeader = queueView->header();
    queueHeader->setSectionResizeMode(0, QHeaderView::Interactive);
    queueHeader->setSectionResizeMode(1, QHeaderView::Stretch);
    queueHeader->resizeSection(0, queueView->viewport()->width() / 3);
    queueView->setSelectionMode(QTreeView::ExtendedSelection);
    connect(queueView, &QTreeView::doubleClicked, queueModel, &ItemModel::onDoubleClicked);
    splitter->addWidget(queueView);
    queueView->setEnabled(false);
    queueView->setUniformRowHeights(true);

    // This needs to be done after setting the model.
    // https://stackoverflow.com/a/30793898/240515
    connect(queueView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            queueModel,
            &QueueModel::onSelectionChanged);

    connect(queueModel, &QueueModel::songIndex, queueView, [=](const QModelIndex &index) {
        queueView->scrollTo(index);
    });

    m_connectedWidgets.append(queueView);
    layout->addWidget(splitter);
    auto widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);

    auto trackInfoCard = new TrackInfoCard(widget);
    trackInfoCard->hide();
    m_connectedWidgets.append(trackInfoCard->playButton());
    connect(trackInfoCard->playButton(),
            &QPushButton::clicked,
            queueModel,
            [queueModel, trackInfoCard]() {
                queueModel->playSong(trackInfoCard->songId());
                trackInfoCard->hide();
            });
    qApp->installEventFilter(new TrackInfoDismissFilter(trackInfoCard, qApp));
    connect(queueView, &QTreeView::clicked, queueView, [queueView, widget, trackInfoCard](const QModelIndex &index) {
        const auto artist = index.siblingAtColumn(0).data().toString();
        const auto title = index.siblingAtColumn(1).data().toString();
        const auto metadata = index.data(Qt::ToolTipRole).toString();
        const auto songId = static_cast<Item *>(index.internalPointer())->id();
        trackInfoCard->setTrackInfo(songId, title, artist, metadata);

        const int width = qMin(560, widget->width() - 32);
        const int height = qMin(380, widget->height() - 32);
        trackInfoCard->resize(width, height);
        trackInfoCard->move((widget->width() - width) / 2, (widget->height() - height) / 2);
        trackInfoCard->raise();
        trackInfoCard->show();
    });

    auto status = statusBar();
    auto combinedTimeLabel = new QLabel();
    status->addPermanentWidget(combinedTimeLabel);
    connect(m_controller, &Controller::combinedTime, combinedTimeLabel, &QLabel::setText);

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, m_controller, &Controller::updateStatus);
    timer->start(1000);

    // Error messages from the Controller are unforeseen and unrecoverable. If we get one, then we just
    // let the user shut the program down.
    connect(m_controller, &Controller::errorMessage, [=](QString message) {
        QMessageBox::critical(this, "Critical error", message);
        setConnectionState(MPDConnection::State::Disconnected);
        timer->stop();
    });

    // These are, say, when you rename a playlist to a name that already exists.
    connect(m_controller, &Controller::serverErrorMessage, [=](QString message) {
        QMessageBox::critical(this, "Server error", message);
    });

    connect(m_controller, &Controller::repeating, repeatAction, &QAction::setChecked);

    connect(m_controller, &Controller::volume, [this](const int &volume) {
        if (tempVolume != volume) {
            v_slider->setValue(volume);
            tempVolume = volume;
        }
    });

    connect(m_controller, &Controller::shuffled, randomAction, &QAction::setChecked);

    QSettings settings;
    if (!settings.contains("host") || !settings.contains("port")) {
        settings.setValue("host", QVariant("localhost"));
        settings.setValue("port", QVariant("6600"));
        settings.sync();
    }
    m_controller->connectToMPD(settings.value("host").toString(),
                               settings.value("port").toInt(),
                               200);
}

MainWindow::~MainWindow() {}

bool MainWindow::iconsAreThere()
{
    if (QIcon::fromTheme(IconNames::Player).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Connect).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Close).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Stop).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Start).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Pause).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::SkipForward).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::SkipBackward).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Shuffle).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Repeat).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Remove).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::SaveAll).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Configure).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Playlist).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Audio).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Favorites).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Documents).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Database).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Harddisk).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::Sound).isNull()) {
        return false;
    }

    if (QIcon::fromTheme(IconNames::OpticalAudio).isNull()) {
        return false;
    }

    return true;
}

void MainWindow::setConnectionState(MPDConnection::State connectionState)
{
    if (MPDConnection::State::Connected == connectionState) {
        for (auto widget : m_connectedWidgets) {
            widget->setEnabled(true);
        }

        for (auto action : m_connectedActions) {
            action->setEnabled(true);
        }

    } else {
        for (auto widget : m_connectedWidgets) {
            widget->setEnabled(false);
        }

        for (auto action : m_connectedActions) {
            action->setEnabled(false);
        }

        statusBar()->clearMessage();
    }
}

void MainWindow::setIconTheme(Qt::ColorScheme colorScheme)
{
    if (Qt::ColorScheme::Dark == colorScheme) {
        QIcon::setThemeName(IconNames::Dark);
    } else {
        QIcon::setThemeName(IconNames::Light);
    }
}
