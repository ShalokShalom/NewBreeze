/*
	*
	* NBAppFile.cpp - NewBreeze Application File Class
	*
*/

#include <NBAppFile.hpp>

inline QString findIn( QString what, QString where ) {

	QRegExp rx( what );
	rx.setMinimal( true );

	if ( rx.indexIn( where ) >= 0 )
		return rx.cap( 1 );

	return QString();
};

inline QString dirName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( dirname( strdup( qPrintable( path ) ) ) ) + "/";
};

inline QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( basename( strdup( qPrintable( path ) ) ) );
};

NBAppFile::NBAppFile() {

	fileUrl = QString();

	__name = QString();
	__type = QString();
	__exec = QString();
	__icon = QString();
	__mimeTypes =     QStringList();
	__workPath =      QString();
	__terminalMode =  false;
	__categories =    QStringList();
	__comment =       QString();
	__nodisplay =     true;
	__takesArgs =     false;
	__multipleFiles = false;
	__grade = -1;
	__execArgs = QStringList();
};

NBAppFile::NBAppFile( QString path ) {

	if ( exists( path ) ) {

		fileUrl = path;
	}

	else{
		if ( exists( NBXdg::home() + "/.local/share/applications/" + path ) )
			fileUrl = NBXdg::home() + "/.local/share/applications/" + path;

		else if ( exists( "/usr/local/share/applications/" + path ) )
			fileUrl = "/usr/local/share/applications/" + path;

		else if ( exists( "/usr/share/applications/" + path ) )
			fileUrl = "/usr/share/applications/" + path;

		else
			return;
	}

	parseDesktopFile();
};

QVariant NBAppFile::value( NBAppFile::Fields field ) const {

	switch( field ) {
		case Name:
			return __name;

		case Type:
			return __type;

		case Exec:
			return __exec;

		case Icon:
			return __icon;

		case MimeTypes:
			return __mimeTypes;

		case WorkPath:
			return __workPath;

		case TerminalMode:
			return __terminalMode;

		case Categories:
			return __categories;

		case Comment:
			return __comment;

		case NoDisplay:
			return __nodisplay;

		default:
			return QVariant();
	};
};

QStringList NBAppFile::execArgs() const {

	return __execArgs;
};

bool NBAppFile::multipleArgs() const {

	return __multipleFiles;
};

bool NBAppFile::takesArgs() const {

	return __takesArgs;
};

short NBAppFile::grade() const {

	return __grade;
};

QString NBAppFile::desktopFileName() const {

	return baseName( fileUrl );
};

int NBAppFile::compare( NBAppFile other ) const {

	if ( __execArgs.at( 0 ) == other.execArgs().at( 0 ) )
		return __grade - other.grade();

	return INT_MAX;
};

QString NBAppFile::category() const {

	QStringList Accessories = QStringList() << "Utility" << "Utilities" << "Accessory" << "Accessories";
	QStringList Development = QStringList() << "Development";
	QStringList Education = QStringList() << "Education";
	QStringList Games = QStringList() << "Games" << "Game" << "ArcadeGame" << "StrategyGame" << "LogicGame";
	QStringList Graphics = QStringList() << "Graphics";
	QStringList Internet = QStringList() << "Network" << "Internet";
	QStringList Multimedia = QStringList() << "Audio" << "Video" << "AudioVideo" << "Multimedia";
	QStringList Office = QStringList() << "Office";
	QStringList ScienceMath = QStringList() << "Science" << "Math";
	QStringList Settings = QStringList() << "Settings";
	QStringList System = QStringList() << "System";

	/*
		*
		* Some applications that declare themselves as Education are also Science and Math are also Educational
		* So we prefer them to be in Science and Math category. So Science and Math categories are checked for
		* before Education.
		*
		* Some applications that are Office applications are also Graphics applications, like okular
		* We want them to be in Office. So we check for Office category first and then Graphics.
		*
	*/

	foreach( QString cate, __categories ) {
		if ( Accessories.contains( cate ) )
			return "Accessories";

		else if ( Development.contains( cate ) )
			return "Development";

		else if ( ScienceMath.contains( cate ) )
			return "Science and Math";

		else if ( Education.contains( cate ) )
			return "Education";

		else if ( Games.contains( c�o�Hwe��Z����_[���Nj�D�OManager.cpp - NewBreeze FileIO Manager
	*
*/

#include <NBFileIO.hpp>

#define COPY_BUF_SIZE ( 64 * 1024 )

/*
	*
	* NBFileIO Initialization.
	*
	* @v mode is set to NBIOMode::Copy
	* @v errorNodes and @v sourceList lists are cleared
	* @v targetDir and @v ioTarget are set to empty strings
	* All sizes are set to 0
	* @v wasCanceled and @v isPaused set to false
	*
*/
NBFileIO::NBFileIO() {

	jobID = MD5( QDateTime::currentDateTime().toString( Qt::ISODate ) );

	errorNodes.clear();
	sourceList.clear();
	targetDir = QString();

	ioTarget = QString();

	mode = NBIOMode::Copy;

	totalSize = 0;
	copiedSize = 0;
	fTotalBytes = 0;
	fWritten = 0;
	wasCanceled = false;
	isPaused = false;

};

void NBFileIO::setSources( QStringList sources ) {

	sourceList << sources;
};

QStringList NBFileIO::sources() {

	return sourceList;
};

void NBFileIO::setTarget( QString target ) {

	targetDir = target.endsWith( "/" ) ? target : target + "/";
	jobID = MD5( targetDir + QDateTime::currentDateTime().toString( Qt::ISODate ) );
};

QString NBFileIO::target() {

	return targetDir;
};

void NBFileIO::setIOMode( NBIOMode::Mode io_Mode ) {

	mode = io_Mode;
};

NBIOMode::Mode NBFileIO::ioMode() {

	return mode;
};

void NBFileIO::cancel() {

	wasCanceled = true;
};

void NBFileIO::pause() {

	isPaused = true;
};

void NBFileIO::resume() {

	isPaused = false;
};

const QString NBFileIO::id() {

	return jobID;
};

int NBFileIO::result() {

	if ( errorNodes.count() )
		return 1;

	else
		return 0;
};

QStringList NBFileIO::errors() {

	return errorNodes;
};

void NBFileIO::performIO() {

	preIO();

	struct statvfs info;
	statvfs( targetDir.toLocal8Bit(), &info );
	quint64 availSize = ( quint64 ) ( info.f_bavail ) * info.f_frsize;
	if ( availSize < totalSize ) {
		errorNodes << sourceList;
		emit IOComplete();

		return;
	}

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, sourceList ) {
		if ( wasCanceled )
			break;

		while ( isPaused ) {
			if ( wasCanceled )
				break;

			usleep( 100 );
			qApp->processEvents();
		}

		QString srcPath = dirName( node );
		QString srcBase = baseName( node );

		chdir( qPrintable( srcPath ) );
		if ( isDir( node ) )
			copyDir( srcBase );

		else
			copyFile( srcBase );

		chdir( qPrintable( curWD ) );
	}

	emit IOComplete();
};

void NBFileIO::preIO() {

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, sourceList ) {
		QString srcPath = dirName( node );
		QString srcBase = baseName( node );

		chdir( qPrintable( srcPath ) );
		if ( isDir( node ) ) {
			/*
				*
				* This is a top level directory. This won't be created in @f getDirSize( ... )
				* We need to create this directory and get its size.
				*
			*/

			/* If we are moving and its an intra-device move then mkpath won't be necessary */
			struct stat iStat, oStat;
			stat( qPrintable( node ), &iStat );
			stat( qPrintable( targetDir ), &oStat );

			if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) )
				continue;

			else {
				mkpath( srcBase, QFile::permissions( srcBase ) );
				getDirSize( srcBase );
			}
		}

		else {
			// This is a file. Just get its size
			totalSize += getSize( srcBase );
		}

		chdir( qPrintable( curWD ) );
	}
};

void NBFileIO::getDirSize( QString path ) {

	DIR* d_fh;
	struct dirent* entry;
	QString longest_name;

	while( ( d_fh = opendir( qPrintable( path ) ) ) == NULL ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	longest_name = QString( path );
	if ( not longest_name.endsWith( "/" ) )
		longest_name += "/";

	while( ( entry = readdir( d_fh ) ) != NULL ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			if ( entry->d_type == DT_DIR ) {

				/*
					*
					* Prepend the current directory and recurse
					* We also need to create this directory in
					* @v targetDir
					*
				*/
				mkpath( longest_name + entry->d_name, QFile::permissions( longest_name + entry->d_name ) );
				getDirSize( longest_name + entry->d_name );J�-����8�2_^�'sɥ^?���MIoi����r�»/�*��N�?��V�ƒ�{�0�hIJ��d��O�q:�[���a��C"6Z�O�~�<��z5�� �W}��Z��9�^ٗ�{D�Q��3#�w��y�e%��U�N�BQ	�	͆�>#V����ǧ���������W8N=w$��f>.�i��#H�ל�ie"J��ss�Gf��$N��q��1������ d3����-�+���PÔ����K0.dPz���wGf�bC���&�	E��CϷ�MK)�����uW�4���g2��݊��qM� �]�6��o�����W��nq��T�"�"2=��o�i�2�S4eĜ͘7B�Y����a�k�g���D#��_�����X�J�@�*Ӹ[\����f����oBd�<�¯`�z�vs%������ǽ�Q���N+[�l�)�Ο	���e`+L����O�LD� ��/���yu��X���=׻��@!��oB���w�)UV���0��/jJ����)	&Qx�^���Y���X��!�A@�[�{z�4����s��N�+��.�����%�zl��<�v\]`�:���y'�@�c�YN�Ϲ�z`N��|?������l'i|:�,����M���v�%��gIIS�(���4	w,ޥV+�7��&v���fN��I�%9I$ZT�+¢�:IE��⏍2���I�V"�9CՆ��o�_���y}��E45k�O��u%j��籿�4Xd��@6؉}�X�?�Z���
UM���I��)����;�yqLsX(\薔|Tc�/
Vx%%o~!a�ȝ�ڤ}.^y�&ˇ������hr9{�dK9��8jJ�#�>��GPGkz ���h�f�7|p�> -#��}y�?wSĺg��l�' T��BF��S��jo��`�4"(�L�.ɁY��𝕨�FN�4� ʧ7\R����'\Y�!�	#'9�`$�jȉ`�L,U���8�ķ�"���ie
�-���.�n#�.��������}F_غ�t�{4:W�1`�C�T?����[,�.��â���ވ����-������dl-D�������ڷ>�X��� b`�$��*?��\�gIk�X��0��~O)v�Uj�LT�Y���ұ%ʤ�
P0&^�G��φB!��wdߊ{9m��4,����s@V���ﶡ��W��E8�1<؝mՎ �6�=�I*�+e�xW~5Ȍ#�l�d.�m'��fv��4WX�>�%����&.��qH��$����A�.@�xы���z�F')'�y�Ctփt�_x�	`���5��}VhkU}�	C���5���Op=��k
��؜PO����=���.�+g}����elv� '9n^�@frs��_i�B9�qw
�̈́���R�q�� ��EF�#*x��B�s��[��J�w�����C}%��o�Y0~&��#j�^�Q�v��u�@���*�:d���]xOa,W�����u��a��C�ͮ���HOzHS�ە)o�m��և<_�R2 �M#4k�RE�@�bxy|��~x�A�W5�:��5z�H��|n����R�`��E^f�G�4<.I����g���o���e���/�\C[6
��I��{���~��������]Vyڮ�^��3!��4_P՛Y��ѧh��+��U\�Q_���^=胈���w�����~���D��-��}�����]\��E�VCj�-e+�W����/l�MP@�ჁS+��_�|��gV\9�_��֑�R�0Q0�9��'[Db�������M�]��+4��~�m���[T�a��PSV����A��"�M.zH��߾�٫��_TF!-AAy<���ŪQF�h�s�,>� �!�����i��v��D�=�C�z���Kʇ��-���E��qUo��Ek3|:k�E{�� ��s�k�[��q�n���<�� ��ʖr�֣	ѹ�Z�5Y&~G"SLe�/(��$�4�=K�.�ս2�-L��*B�=�U.��f:h�7pL)�򗵺UWR���w�����D�B��[�s]�e�����~�k��\6$;7ZG�i�采WEc��bm�N	�\>��%�X>��L~��P�U�>O���j��Y�J��P��-ҿ�S�韩%zuJ{m�B���}��C�8��]~F��X��#s�Ce�����E��T�u�J�K.�W[�pm�ij�*�M��Zl��2���v�{o�0���
W�#>�O:�'g��� ���P7�Ĺ����9�"�/F�`S��A��~'ϐǐ16��c�f��K�m�5B��a��+'W�j�m�N��S�Q� �'�)�K��<� �Y����Gz2�oE��7|ӽA��&�En�G�����]Jb{��8���5F��j�kc��ӒҊ���8$���04��V)�n�M��hw�xaz����pA�3����DQ)���k~s�}ǈH��	�pU�O��n5@�|��uN۫EִL������r����O�¤U�sc��+������t8�x[��'Vc:dp�n��M��D���h�!, {'=��n\�������S>�{&����V�O+�$&�d���z�v�Rk5$~����t�x�j���:Џi` �r%�i��X �7������{�������]�^�g���N����r���K|�^eGY���|�B���,��y�u�t�R��]������S��^ߢ��z̯�����t��b���;c���o�vK�7�����L���.y��4
��V�f�uS�bE�Y��x��ǃ��'��
�vʽ� �b����4��mO�����ƍ���g�X��4�2�Y��]KJ��@l�����n�p���((�3�	c��zh���a��-�|B@�7���(�J�H�၅���� v�ț�1}�*��;i�j+vUo*	�����������A�����8�x�S>�N��p*7~�!�����nn7�VN�4�V�de#�� ���v�0�~�|G��-�!h�9�=E�?�Ԧ����\k�����ʊ$D�����C)�:��9��'b"�q��K�(){�Zeڭ���PM_��jP2�+��l�}�WS�F	҂ġZu�F&��"��HOO��Q�7"A��z0�Ry�bĚp���'�^�W��j��US�X�{�����=xI<��[�Z��Ĵb�.D��%�������wlJ�\��qi1A�zǥ{��y�9��s��_���X��k�닓��1����uui"޾h���T�I'x�Fr�s<���bV*_�j������]Լ	?�~^GEM�����������ٷ��g��^����:��=����l��<R+�c6Q3i�|n���n;�T5��NvG<�:��aiFC�x�s�e��;z,�=B��P;DaB��Z���3.@ѕ~	*��zB��%Hݣ�������`�}��Z�Lh��=F��E�NQ�Xi��ك
ώvy��G2��>Kл����Ş�1���`7����m0�'�U�mo�G��������޼� ��$}%;,&�8��5��kH�諒Mwf���W��� &��^�?������٬d)�5���좥�ʊ��=�B��e!��ŵV����¨�%��B�9E)ex
F���|����.*�-J�9���P���\����@�1��p���&�`�0z���	�x����i[���<�!���D�v�,ԨG���WVJ'%9J�/Q�O�RI�3��>�=��$Ht��F|�ؕG��y��8�F��=e�n�jL+Mo���-�4�½�Յ]�2y��IWmNصs��N��pFvS� �a��E��bFdiS� �a��E��bFdiS� �a��E��bFdiS� �a��E��bFdiS� �a��E��bFdiS� �a��E��bFdiS� �a��E��bFdiS� �a��E��bFdiS� �a��E�
崝e�b� �o%�ppFile other, __appsList ) {
			if ( __appsList.indexOf( thisApp ) == __appsList.indexOf( other ) ) {
				continue;
			}

			if ( thisApp.compare( other ) == INT_MAX ) {
				continue;
			}

			else if ( thisApp.compare( other ) < 0 ) {
				__appsList.removeAt( __appsList.indexOf( thisApp ) );
				break;
			}

			else if ( thisApp.compare( other ) > 0 ) {
				/*
					*
					* Since @other is deleted, we have more to compare @thisApp with
					*
				*/
				__appsList.removeAt( __appsList.indexOf( other ) );
			}

			else if ( thisApp.compare( other ) == 0 ) {
				// Same location multiple files
				qDebug() << thisApp.value( NBAppFile::Name ).toString();
				qDebug() << thisApp.grade();
			}
		}
	}

	__clearedOfDuplicates = true;
};

NBAppsList NBAppsList::operator<<( NBAppsList newList ) {

	__appsList << newList.toQList();

	return *this;
};

NBAppsList NBAppsList::operator<<( NBAppFile app ) {

	// If app is not an Application type,do not add it.
	if ( app.value( NBAppFile::Type ).toString() != QString( "Application" ) )
		return *this;

	foreach( NBAppFile other, __appsList ) {
		if ( app.compare( other ) == INT_MAX ) {
			// This means nothing. Just that the other app is not the same as this.
			// So we continue the search.
			continue;
		}

		else if ( app.compare( other ) < 0 ) {
			// This has less priority than the one existing.
			// So no more checks. We do not include this at all.
			return *this;
		}

		else if ( app.compare( other ) > 0 ) {
			// This has a higher priority than the one existing.
			// We delete the other and append this.
			__appsList.removeAt( __appsList.indexOf( other ) );
			__appsList << app;

			return *this;
		}

		else if ( app.compare( other ) == 0 ) {
			// We already have an equivalent application.
			// We'll try to merge the two and return.

			__appsList.removeAt( __appsList.indexOf( other ) );
			__appsList << NBAppFile::merge( app, other );

			return *this;
		}
	}

	// This means no application was found similar to the one being added.
	// We add this application
	__appsList << app;

	return *this;
};

NBAppsList NBAppsList::operator=( NBAppsList newList ) {

	__appsList = newList.toQList();

	return *this;
};

bool NBAppsList::operator==( NBAppsList newList ) {

	if ( count() != newList.count() )
		return false;

	return __appsList == newList.toQList();
};

NBAppFile NBAppsList::operator[]( int pos ) {

	return __appsList.at( pos );
};

uint qHash( const NBAppFile &app ) {

	QString hashString;
	hashString += app.value( NBAppFile::Name ).toString();
	hashString += app.value( NBAppFile::Type ).toString();
	hashString += app.value( NBAppFile::Exec ).toString();
	hashString += app.value( NBAppFile::Icon ).toString();
	hashString += app.value( NBAppFile::MimeTypes ).toStringList().join( " " );
	hashString += app.value( NBAppFile::WorkPath ).toString();
	hashString += app.value( NBAppFile::TerminalMode ).toString();
	hashString += app.value( NBAppFile::Categories ).toStringList().join( " " );
	hashString += app.value( NBAppFile::Comment ).toString();
	hashString += app.value( NBAppFile::NoDisplay ).toString();

	return qChecksum( qPrintable( hashString ), hashString.count() );
};
