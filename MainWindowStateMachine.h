#pragma once

#include <QtCore/QObject>

#include <memory>
#include <string>
#include <functional>

#include "FailedPluginData.h"
#include "ActionData.h"

class MainWindowStateMachinePrivate;

class MainWindowStateMachine : public QObject
{
    Q_OBJECT

public:
    MainWindowStateMachine();
    virtual ~MainWindowStateMachine() = default;

    void startAsync();

public slots:
    // Actions which can be performed on the state machine
    void configuringCoreUserInterfaceFinished();
    void displayingCoreUserInterfaceFinished();
    void configuringApplicationFinished();
    void loadingPluginsFinished(const FailedPluginData failedPluginData);
    void displayingPluginLoadErrorFinished();
    void displayingPluginUIFinished();
    void processingStarted(const ActionData actionData);
    void processingFinished();

signals:
    // Events which will be fired by the state machine
    void configureCoreUserInterfaceTriggered(MainWindowStateMachine*);
    void displayCoreUserInterfaceTriggered(MainWindowStateMachine*);
    void configureApplicationTriggered(MainWindowStateMachine*);
    void loadPluginsTriggered(MainWindowStateMachine*);
    void displayPluginLoadErrorTriggered(MainWindowStateMachine*, const FailedPluginData&);
    void displayPluginUITriggered(MainWindowStateMachine*);
    /// Allow user interactions whenever this state is being entered.
    void idleStateEntered();
    /// Disable user interactions whenever this state is being entered.
    void processingStateEntered(const ActionData&);

private:
    std::shared_ptr<MainWindowStateMachinePrivate> m_p;
};

