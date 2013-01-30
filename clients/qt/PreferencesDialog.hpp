//
// PreferencesDialog.hpp
//

#ifndef _PREFERENCES_DIALOG_HPP_INCLUDED
#define _PREFERENCES_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <QStringList>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>

#include "settings.hpp"

#include "FileChooser.hpp"


class PreferencesDialog : public QDialog
{
	Q_OBJECT
		
	public:
		PreferencesDialog( ApplicationSettings &settings, QStringList _languages, QWidget *_parent = 0 );
	
	private:
		ApplicationSettings &m_settings;
		QRadioButton *m_uiLoadModeLocalFile;
		QRadioButton *m_uiLoadModeNetwork;
		QRadioButton *m_dataLoadModeLocalFile;
		QRadioButton *m_dataLoadModeNetwork;
		QDialogButtonBox* m_buttons;
		FileChooser *m_uiFormsDir;
		FileChooser *m_uiFormTranslationsDir;
		FileChooser *m_uiFormResourcesDir;
		FileChooser *m_dataLoaderDir;
		QCheckBox *m_debug;
		QCheckBox *m_developer;
		QComboBox *m_locale;
		QCheckBox *m_systemLanguage;
		QStringList m_languages;
		QCheckBox *m_mdi;
	
	signals:
		void prefsChanged( );
		
	private:
		void initialize( );
		void loadSettings( );
		
	private slots:
		void toggleLoadMode( bool checked );
		void toggleSystemLanguage( int state );
		void apply( );
		void cancel( );
};


#endif // _PREFERENCES_DIALOG_HPP_INCLUDED
