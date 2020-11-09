#ifndef MAINWINDOWSTATEMACHINEPRIVATE_H
#define MAINWINDOWSTATEMACHINEPRIVATE_H

#include <QtCore/QObject>
#include <QtCore/QStateMachine>
#include "FailedPluginData.h"
#include "ActionData.h"

class MainWindowStateMachinePrivate : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowStateMachinePrivate(QObject *parent = nullptr);

    QStateMachine StateMachine;
    FailedPluginData FailedPluginData;
    ActionData ActionData;

signals:
    void configuringCoreUserInterfaceFinished();
    void displayingCoreUserInterfaceFinished();
    void configuringApplicationFinished();
    void loadingPluginsFinished();
    void loadingPluginsFailed();
    void displayingPluginLoadErrorFinished();
    void displayingPluginUIFinished();
    void processingStarted();
    void processingFinished();
};

#endif // MAINWINDOWSTATEMACHINEPRIVATE_H
