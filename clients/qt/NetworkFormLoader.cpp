//
// NetworkFormLoader.cpp
//

#include "NetworkFormLoader.hpp"

#include <QDir>
#include <QFile>

#include <QDebug>

namespace _Wolframe {
	namespace QtClient {

NetworkFormLoader::NetworkFormLoader( WolframeClient *_client )
	: m_client( _client )
{
}

void NetworkFormLoader::initiateListLoad( )
{
// doing nothing here, deferred to getFormNames on demand
	//~ emit formListLoaded( );
}

void NetworkFormLoader::initiateFormLocalizationLoad( QString &name, QLocale locale )
{
	//~ QByteArray localization = readFile( m_localeDir + "/" + name + "." + locale.name( ) + ".qm" );
	//~ 
	//~ emit formLocalizationLoaded( name, localization );
}

void NetworkFormLoader::initiateFormLoad( QString &name )
{
//~ // read directly here and stuff data into the signal
	//~ QByteArray form = readFile( m_formDir + "/" + name + ".ui" );
//~ 
	//~ emit formLoaded( name, form );
}

QStringList NetworkFormLoader::getFormNames( )
{
	QStringList forms;
	
//~ // read list of forms directory on demand
	//~ QDir formsDir( QLatin1String( "forms" ) );
	//~ QStringList forms = formsDir.entryList( QDir::Files | QDir::NoDotAndDotDot, QDir::Name )
		//~ .replaceInStrings( ".ui", "" );
	return forms;
}

QStringList NetworkFormLoader::getLanguageCodes( )
{
	QStringList languageCodes;
	languageCodes.push_back( "en_US" ); // default locale, always around
	
//~ // read list of supperted languages for all forms based on their qm files available
	//~ QDir translationDir( QLatin1String( "i18n" ) );
	//~ translationDir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
	//~ translationDir.setSorting( QDir::Name );
	//~ QStringList filters;
	//~ filters << "*.qm";
	//~ translationDir.setNameFilters( filters );
	//~ QStringList localeFiles = translationDir.entryList( );
	//~ QMutableStringListIterator it( localeFiles );
	//~ while( it.hasNext( ) ) {
		//~ it.next( );
		//~ QStringList parts = it.value( ).split( "." );
		//~ languageCodes.push_back( parts[1] );		
	//~ }
	//~ 
	//~ languageCodes.removeDuplicates( );

	return languageCodes;
}

} // namespace QtClient
} // namespace _Wolframe
