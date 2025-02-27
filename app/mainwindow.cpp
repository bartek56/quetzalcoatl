#include "mainwindow.h"
#include "databasemodel.h"
#include "iconnames.h"
#include "item.h"
#include "queuemodel.h"

#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QSlider>
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Quetzalcoatl"));
    onPaletteChanged();

    QIcon::setThemeSearchPaths(QStringList() << "/usr/share/icons");
    QIcon::setThemeName("breeze");

    setWindowIcon(QIcon::fromTheme(IconNames::Player));

    m_controller = new Controller(this);
    connect(m_controller, &Controller::connectionState, this, &MainWindow::setConnectionState);

    auto toolBar = addToolBar("ToolBar");
    toolBar->setMovable(false);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->setMinimumHeight(250);
    toolBar->setIconSize(QSize(60, 60));

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

    // volume slider
    v_slider = new QSlider(Qt::Horizontal);

    v_slider->setTracking(false);
    v_slider->setMinimumHeight(50);
    v_slider->setRange(0, 100);
    v_slider->setMaximumWidth(250);
    m_connectedWidgets.append(v_slider);
    toolBar->addWidget(v_slider);

    connect(v_slider, &QSlider::valueChanged, [this]() {
        if (actionVolume) {
            m_controller->setVolume(v_slider->value());
            actionVolume = false;
        }
    });

    connect(v_slider, &QSlider::actionTriggered, [this]() { actionVolume = true; });

    toolBar->addSeparator();

    // close button
    auto closeAction = toolBar->addAction(QIcon::fromTheme(IconNames::Close), "Close");

    connect(closeAction, &QAction::triggered, [=]() { close(); });
    auto layout = new QVBoxLayout();

    // Is the slider being dragged?
    bool isDragging = false;

    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setTracking(false);

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
    connect(databaseView, &QTreeView::doubleClicked, databaseModel, &ItemModel::onDoubleClicked);
    databaseView->setHeaderHidden(true);
    databaseView->setModel(databaseModel);
    databaseView->setEnabled(false);
    databaseView->setSelectionMode(QTreeView::ExtendedSelection);
    databaseView->setDragEnabled(true);
    databaseView->setUniformRowHeights(true);

    m_connectedWidgets.append(databaseView);
    splitter->addWidget(databaseView);

    // https://stackoverflow.com/a/22198672/240515
    databaseView->setContextMenuPolicy(Qt::CustomContextMenu);
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

    auto queueView = new QTreeView();

    queueView->setAcceptDrops(true);
    queueView->setDropIndicatorShown(true);
    queueView->setDragEnabled(true);
    queueView->setModel(queueModel);
    queueView->setSelectionMode(QTreeView::ExtendedSelection);
    connect(queueView, &QTreeView::doubleClicked, queueModel, &ItemModel::onDoubleClicked);
    connect(queueModel, &ItemModel::columnResized, queueView, &QTreeView::resizeColumnToContents);
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

void MainWindow::changeEvent(QEvent *event)
{
    onPaletteChanged();
}

void MainWindow::onPaletteChanged()
{
#ifndef Q_OS_LINUX
    constexpr int OSX_LIGHT_MODE = 236;
    constexpr int OSX_DARK_MODE = 50;
    constexpr int THRESHOLD = OSX_LIGHT_MODE / 2 - OSX_DARK_MODE / 2;

    if (palette().color(QPalette::Active, QPalette::Window).lightness() < THRESHOLD) {
        QIcon::setThemeName(IconNames::Dark);
    } else {
        QIcon::setThemeName(IconNames::Light);
    }
#endif
}
