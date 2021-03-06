/*
	*
	* NBProcessManager.cpp - NewBreeze ProcessManager Class
	*
	* NBProcessManager is a backend for a graphical display of the running processes.
	* If a graphical display of the progress for a process is not needed, then it is
	* not necessary to register that process with NBProcessManager.
	*
	* Registering a process with NBProcessManager has an added advantage, i.e., it is
	* possible to undo the process: UNDO not implemented.
	*
	* Note: All processes can not be undone, example Deleting a node from the disk can
	* not be undone using NBProcessManager. Similarly, it is not possible to undo the
	* permissions set for a file or a group of files.
	*
*/

#include "NBProcessManager.hpp"

NBProcessManager *NBProcessManager::pMgr = NULL;

NBProcessManager* NBProcessManager::instance() {

	/* If the Processess Manager has already been init, then return the instance */
	if ( pMgr != NULL and pMgr->init )
		return pMgr;

	/* Init our process manager. */
	pMgr = new NBProcessManager();

	return pMgr;
};

quint64 NBProcessManager::processCount() {

	return progressList.count();
};

ProcessList NBProcessManager::processes() {

	return processList;
};

ProgressList NBProcessManager::progresses() {

	return progressList;
};

quint64 NBProcessManager::activeProcessCount() {

	return progressList.count() - completedProcIDs.count();
};

ProcessList NBProcessManager::activeProcesses() {

	ProcessList active;
	for( quint64 i = 0; i < progressList.count(); i++ ) {
		if ( not completedProcIDs.contains( i ) )
			active << processList.at( i );
	}

	return active;
};

ProgressList NBProcessManager::activeProgresses() {

	ProgressList active;
	for( quint64 i = 0; i < progressList.count(); i++ ) {
		if ( not completedProcIDs.contains( i ) )
			active << progressList.at( i );
	}

	return active;
};

quint64 NBProcessManager::addProcess( NBProcess::Progress *progress, NBAbstractProcess *proc ) {

	emit processAdded( progress, proc );
	emit activeProcessCountChanged( processList.count() - completedProcIDs.count() + 1 );

	connect( proc, SIGNAL( completed( QStringList) ), this, SLOT( handleProcessComplete() ) );
	connect( proc, SIGNAL( canceled( QStringList) ), this, SLOT( handleProcessComplete() ) );
	connect( proc, SIGNAL( noSpace() ), this, SLOT( handleProcessComplete() ) );
	connect( proc, SIGNAL( noWriteAccess() ), this, SLOT( handleProcessComplete() ) );

	processList << proc;
	progressList << progress;

	return progressList.count() - 1;
};

NBProcess::Progress* NBProcessManager::progress( quint64 id ) {

	if ( id < 0 or id >= progressList.count() )
		return NULL;

	return progressList.at( id );
};

NBAbstractProcess* NBProcessManager::process( quint64 id ) {

	if ( id < 0 or id >= processList.count() )
		return NULL;

	return processList.at( id );
};

void NBProcessManager::handleProcessComplete() {

	NBAbstractProcess *proc = qobject_cast<NBAbstractProcess*>( sender() );
	quint64 id = processList.indexOf( proc );

	if ( id == -1 )
		return;

	/* Move this process to completedList */
	completedProcIDs << id;

	emit activeProcessCountChanged( processList.count() - completedProcIDs.count() );
};
