/*
	*
	* NBTermPlugin.hpp - NewBreeze MarkDownPreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "newbreeze.hpp"
#include "NBPluginInterface.hpp"

class NBTermPlugin : public QObject, NBPluginInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPluginInterface" FILE "TermPlugin5.json" )
	#endif
	Q_INTERFACES( NBPluginInterface )

	public:
		/* Name of the plugin */
		 QString name();

		/* The plugin version */
		 QString version();

		/* The QAction */
		 QList<QAction*> actions( Interface, QStringList );

		/* Interface type: preview, rename etc */
		 Interfaces interfaces();

		/* Interface type: preview, rename etc */
		 Type type( Interface );

		/* Plugin load context */
		 Contexts contexts( Interface );

		/* Mimetypes handled by the plugin */
		 QStringList mimetypes();

		/* Invoke slots called called by triggering the actions */
		void actionTrigger( Interface, QString, QStringList nodes );

		/* Store the called widget pointer */
		 void setCaller( QWidget *caller );
};
