#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "connectionstate.h"
#include "controller.h"

#include <QMainWindow>
#include <QVector>

class ConnectionDialog;
class Controller;

class QAction;
class QSlider;
class QWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void changeEvent(QEvent *);

private:
    QVector<QWidget *> m_connectedWidgets;
    QVector<QAction *> m_connectedActions;

    QSlider *m_slider;
    QSlider *v_slider;
    bool volumeSliderIsBusy = false;
    bool actionVolume = false;
    int tempVolume = 0;

private:
    void onPaletteChanged();
    int m_seekPosition{};
    Controller *m_controller{};
private slots:
    void setConnectionState(MPDConnection::State);
};
#endif // MAINWINDOW_H
