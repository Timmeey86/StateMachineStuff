#include "MainWindowStateMachine.h"
#include "MainWindowStateMachinePrivate.h"
#include "ActionData.h"
#include "FailedPluginData.h"

#include <QtCore/QState>
#include <QtCore/QStateMachine>
#include <QtCore/QList>


MainWindowStateMachine::MainWindowStateMachine() :
    m_p{new MainWindowStateMachinePrivate()}
{
    auto configuringCoreUI = new QState();
    auto displayingCoreUI = new QState();
    auto configuringApplication = new QState();
    auto loadingPlugins = new QState();
    auto displayingPluginLoadError = new QState();
    auto displayingPluginUI = new QState();
    auto idle = new QState();
    auto processing = new QState();

    // The absolute minimum configuration for creating the Core UI is being read here. Failure results in a shutdown.
    m_p->StateMachine.addState(configuringCoreUI);
    // Displays just the Core UI without any specific content. Failure results in a shutdown.
    m_p->StateMachine.addState(displayingCoreUI);
    // Configures anything which was not required for displaying the base UI
    m_p->StateMachine.addState(configuringApplication);
    // Loads actual content. Failure results in displaying an error message, then continuing with whatever plugins could be loaded
    m_p->StateMachine.addState(loadingPlugins);
    // Displays information about the failed plugins, then resumes with the CreatingPluginUI state for successfully loaded plugins
    m_p->StateMachine.addState(displayingPluginLoadError);
    // Creates UI as provided by plugins, while restoring user settings
    m_p->StateMachine.addState(displayingPluginUI);
    // The application is currently waiting for user input. User input should solely be accepted in this state.
    m_p->StateMachine.addState(idle);
    // The application is processing any kind of operation, like Saving, Reloading, Generating, Loading, ...
    m_p->StateMachine.addState(processing);

    m_p->StateMachine.setInitialState(configuringCoreUI);
    configuringCoreUI->addTransition(m_p.get(), &MainWindowStateMachinePrivate::configuringCoreUserInterfaceFinished, displayingCoreUI);
    displayingCoreUI->addTransition(m_p.get(), &MainWindowStateMachinePrivate::displayingCoreUserInterfaceFinished, configuringApplication);
    configuringApplication->addTransition(m_p.get(), &MainWindowStateMachinePrivate::configuringApplicationFinished, loadingPlugins);
    loadingPlugins->addTransition(m_p.get(), &MainWindowStateMachinePrivate::loadingPluginsFinished, displayingPluginUI);
    loadingPlugins->addTransition(m_p.get(), &MainWindowStateMachinePrivate::loadingPluginsFailed, displayingPluginLoadError);
    displayingPluginLoadError->addTransition(m_p.get(), &MainWindowStateMachinePrivate::displayingPluginLoadErrorFinished, displayingPluginUI);
    displayingPluginUI->addTransition(m_p.get(), &MainWindowStateMachinePrivate::displayingPluginUIFinished, idle);
    idle->addTransition(m_p.get(), &MainWindowStateMachinePrivate::processingStarted, processing);
    processing->addTransition(m_p.get(), &MainWindowStateMachinePrivate::processingFinished, idle);

    connect(configuringCoreUI, &QState::entered, [this](){ emit configureCoreUserInterfaceTriggered(this); });
    connect(displayingCoreUI, &QState::entered, [this](){ emit displayCoreUserInterfaceTriggered(this); });
    connect(configuringApplication, &QState::entered, [this](){ emit configureApplicationTriggered(this); });
    connect(loadingPlugins, &QState::entered, [this]() { emit loadPluginsTriggered(this); });
    connect(displayingPluginLoadError, &QState::entered, [this]() { emit displayPluginLoadErrorTriggered(this, m_p->FailedPluginData); });
    connect(displayingPluginUI, &QState::entered, [this]() { emit displayPluginUITriggered(this); });
    connect(idle, &QState::entered, [this]() { emit idleStateEntered(); });
    connect(processing, &QState::entered, [this]() { emit processingStateEntered(m_p->ActionData); });
}

void MainWindowStateMachine::startAsync()
{
    m_p->StateMachine.start();
}

// The following slot implementations simply advance the state machine and store data where necessary
void MainWindowStateMachine::configuringCoreUserInterfaceFinished()
{
    m_p->configuringCoreUserInterfaceFinished();
}

void MainWindowStateMachine::displayingCoreUserInterfaceFinished()
{
    m_p->displayingCoreUserInterfaceFinished();
}

void MainWindowStateMachine::configuringApplicationFinished()
{
    m_p->configuringApplicationFinished();
}

void MainWindowStateMachine::loadingPluginsFinished(const FailedPluginData failedPluginData)
{
    m_p->FailedPluginData = failedPluginData;
    if(failedPluginData.Dummy.empty())
    {
        m_p->loadingPluginsFinished();
    }
    else
    {
        m_p->loadingPluginsFailed();
    }
}

void MainWindowStateMachine::displayingPluginLoadErrorFinished()
{
    m_p->displayingPluginLoadErrorFinished();
}

void MainWindowStateMachine::displayingPluginUIFinished()
{
    m_p->FailedPluginData = FailedPluginData();
    m_p->displayingPluginUIFinished();
}

void MainWindowStateMachine::processingStarted(const ActionData actionData)
{
    m_p->ActionData = actionData;
    m_p->processingStarted();
}

void MainWindowStateMachine::processingFinished()
{
    m_p->ActionData = ActionData();
    m_p->processingFinished();
}

