/*
  This file is part of Shuriken Beat Slicer.

  Copyright (C) 2014, 2015 Andrew M Taylor <a.m.taylor303@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>
  or write to the Free Software Foundation, Inc., 51 Franklin Street,
  Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "akaifilehandler.h"
#include <QFileDialog>
#include <QDebug>
#include "messageboxes.h"


//==================================================================================================
// Public:

ExportDialog::ExportDialog( QWidget* parent ) :
    QDialog( parent ),
    m_ui( new Ui::ExportDialog ),
    m_lastOpenedExportDir( QDir::homePath() )
{
    // Set up user interface
    m_ui->setupUi( this );


    // Set up input validators
    m_directoryValidator = std::make_unique<DirectoryValidator>();
    m_ui->lineEdit_OutputDir->setValidator( m_directoryValidator.get() );

    connect( m_directoryValidator.get(), SIGNAL( isValid(bool) ),
             this, SLOT( displayDirValidityText(bool) ) );

    connect( m_directoryValidator.get(), SIGNAL( isValid(bool) ),
             m_ui->pushButton_Create, SLOT( setDisabled(bool) ) );

    connect( m_ui->lineEdit_OutputDir, SIGNAL( textChanged(QString) ),
             this, SLOT( enableOkButtonIfInputValid() ) );

    connect( m_ui->lineEdit_FileName, SIGNAL( textChanged(QString) ),
             this, SLOT( enableOkButtonIfInputValid() ) );

    connect( m_ui->checkBox_ExportAudio, SIGNAL( clicked(bool) ),
             this, SLOT( enableOkButtonIfInputValid() ) );

    connect( m_ui->checkBox_ExportMidi, SIGNAL( clicked(bool) ),
             this, SLOT( enableOkButtonIfInputValid() ) );


    // Populate combo boxes
    m_ui->radioButton_AudioFiles->click();

    // Populate "Akai Model" combo box
    {
        QStringList textList;
        QList<int> dataList;

        textList << "MPC 1000/2500" << "MPC 500";
        dataList << AkaiModelID::MPC1000_ID << AkaiModelID::MPC500_ID;

        for ( int i = 0; i < textList.size(); i++ )
        {
            m_ui->comboBox_Model->addItem( textList[ i ], dataList[ i ] );
        }
    }

    m_ui->label_Model->setVisible( false );
    m_ui->comboBox_Model->setVisible( false );

    // Populate "Sample Rate" combo box
    {
        QStringList textList;
        QList<int> dataList;

        textList << "Keep Same" << "8,000 Hz" << "11,025 Hz" << "16,000 Hz"<< "22,050 Hz" << "32,000 Hz" << "44,100 Hz" << "48,000 Hz" << "88,200 Hz" << "96,000 Hz" << "192,000 Hz";
        dataList << SAMPLE_RATE_KEEP_SAME << 8000 << 11025 << 16000 << 22050 << 32000 << 44100 << 48000 << 88200 << 96000 << 192000;

        for ( int i = 0; i < textList.size(); i++ )
        {
            m_ui->comboBox_SampleRate->addItem( textList[ i ], dataList[ i ] );
        }
    }

    // Populate "Mute Group" combo box
    {
        QStringList textList;
        QList<int> dataList;

        textList << "Off" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10" << "11" << "12" << "13" << "14" << "15" << "16"
                 << "17" << "18" << "19" << "20" << "21" << "22" << "23" << "24" << "25" << "26" << "27" << "28" << "29" << "30" << "31" << "32";
        dataList << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15 << 16
                 << 17 << 18 << 19 << 20 << 21 << 22 << 23 << 24 << 25 << 26 << 27 << 28 << 29 << 30 << 31 << 32;

        for ( int i = 0; i < textList.size(); i++ )
        {
            m_ui->comboBox_MuteGroup->addItem( textList[ i ], dataList[ i ] );
        }
        m_ui->comboBox_MuteGroup->setCurrentIndex( 1 ); // Mute group 1
    }
}



ExportDialog::~ExportDialog()
{
    delete m_ui;
}



QString ExportDialog::getOutputDirPath() const
{
    return m_ui->lineEdit_OutputDir->text();
}



int ExportDialog::getExportType() const
{
    int exportType = 0;

    if ( m_ui->checkBox_ExportAudio->isChecked() )
    {
        if ( m_ui->radioButton_AudioFiles->isChecked() )
        {
            exportType = EXPORT_AUDIO_FILES;
        }
        else if ( m_ui->radioButton_H2Drumkit->isChecked() )
        {
            exportType = EXPORT_H2DRUMKIT | EXPORT_AUDIO_FILES;
        }
        else if ( m_ui->radioButton_SFZ->isChecked() )
        {
            exportType = EXPORT_SFZ | EXPORT_AUDIO_FILES;
        }
        else if ( m_ui->radioButton_Akai->isChecked() )
        {
            exportType = EXPORT_AKAI_PGM | EXPORT_AUDIO_FILES;
        }
    }

    if ( m_ui->checkBox_ExportMidi->isChecked() )
    {
        exportType |= EXPORT_MIDI_FILE;
    }

    return exportType;
}



int ExportDialog::getMidiFileType() const
{
    if ( m_ui->radioButton_MidiType0->isChecked() )
    {
        return 0;
    }
    else // mUI->radioButton_MidiType1->isChecked()
    {
        return 1;
    }
}



QString ExportDialog::getFileName() const
{
    return m_ui->lineEdit_FileName->text();
}



ExportDialog::NumberingStyle ExportDialog::getNumberingStyle() const
{
    if ( m_ui->radioButton_Prefix->isChecked() )
    {
        return NUMBERING_PREFIX;
    }
    else
    {
        return NUMBERING_SUFFIX;
    }
}



bool ExportDialog::isOverwriteEnabled() const
{
    return m_ui->checkBox_Overwrite->isChecked();
}



int ExportDialog::getSndFileFormat() const
{
    const int index = m_ui->comboBox_Encoding->currentIndex();
    const int encoding = m_ui->comboBox_Encoding->itemData( index ).toInt();

    QString formatName = m_ui->comboBox_Format->currentText();
    int format = 0;

    if ( formatName.contains( "WAV" ) )
    {
        format = SF_FORMAT_WAV;
    }
    else if ( formatName == "AIFF" )
    {
        format = SF_FORMAT_AIFF;
    }
    else if ( formatName == "AU")
    {
        format = SF_FORMAT_AU;
    }
    else if ( formatName == "FLAC" )
    {
        format = SF_FORMAT_FLAC;
    }
    else if ( formatName == "Ogg" )
    {
        format = SF_FORMAT_OGG;
    }
    else
    {
        qDebug() << "Unknown format: " << formatName;
    }

    return format | encoding;
}



int ExportDialog::getSampleRate() const
{
    const int index = m_ui->comboBox_SampleRate->currentIndex();
    const int sampleRate = m_ui->comboBox_SampleRate->itemData( index ).toInt();

    return sampleRate;
}



int ExportDialog::getAkaiModelID() const
{
    const int index = m_ui->comboBox_Model->currentIndex();
    const int modelID = m_ui->comboBox_Model->itemData( index ).toInt();

    return modelID;
}



bool ExportDialog::isVoiceOverlapMono() const
{
    return m_ui->radioButton_Mono->isChecked();
}



int ExportDialog::getMuteGroup() const
{
    return m_ui->comboBox_MuteGroup->currentIndex();
}



//==================================================================================================
// Protected:

void ExportDialog::changeEvent( QEvent* event )
{
    QDialog::changeEvent( event );

    switch ( event->type() )
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}



void ExportDialog::showEvent( QShowEvent* event )
{
    // If the dialog is not being maximised, i.e. it has not previoulsy been minimised...
    if ( ! event->spontaneous() )
    {
        m_ui->lineEdit_OutputDir->setText( m_lastOpenedExportDir );
        m_ui->lineEdit_FileName->clear();

        resize( 502, 282 );
    }

    QDialog::showEvent( event );
}



//==================================================================================================
// Private:

void ExportDialog::setPlatformFileNameValidator()
{
    QString pattern;

#if defined(__linux__)
    pattern = "[^/\\s]+"; // Match any character except forward slash and white space
#elif defined(_WIN32)
    pattern = "^[0-9a-zA-Z_\\-. ]+$";
#else
    static_assert(false, "Platform not found");
#endif

    if ( m_ui->lineEdit_FileName->validator() != NULL )
    {
        const QValidator* validator = m_ui->lineEdit_FileName->validator();
        const QRegExpValidator* regExpValidator = static_cast<const QRegExpValidator*>( validator );

        if ( regExpValidator->regExp().pattern() != pattern )
        {
            m_ui->lineEdit_FileName->setValidator( new QRegExpValidator( QRegExp(pattern), this ) );
        }
    }
    else
    {
        m_ui->lineEdit_FileName->setValidator( new QRegExpValidator( QRegExp(pattern), this ) );
    }
}



void ExportDialog::enableMidiFileTypeRadioButtons()
{
    if ( m_ui->checkBox_ExportMidi->isChecked() )
    {
        foreach ( QAbstractButton* button, m_ui->buttonGroup_Midi->buttons() )
        {
            button->setEnabled( true );
        }
    }
}



void ExportDialog::disableMidiFileTypeRadioButtons()
{
    foreach ( QAbstractButton* button, m_ui->buttonGroup_Midi->buttons() )
    {
        button->setEnabled( false );
    }
}



//==================================================================================================
// Private Slots:

void ExportDialog::displayDirValidityText( const bool isValid )
{
    if ( isValid )
    {
        m_ui->label_DirValidity->setVisible( false );
    }
    else
    {
        m_ui->label_DirValidity->setText( tr("Dir doesn't exist") );
        m_ui->label_DirValidity->setVisible( true );
    }
}



void ExportDialog::enableOkButtonIfInputValid()
{
    QPushButton* okButton = m_ui->buttonBox->button( QDialogButtonBox::Ok );

    if ( m_ui->checkBox_ExportAudio->isChecked() || m_ui->checkBox_ExportMidi->isChecked() )
    {
        if ( m_ui->lineEdit_OutputDir->hasAcceptableInput() && m_ui->lineEdit_FileName->hasAcceptableInput() )
        {
            okButton->setEnabled( true );
        }
        else
        {
            okButton->setEnabled( false );
        }
    }
    else
    {
        okButton->setEnabled( false );
    }
}



void ExportDialog::on_pushButton_Choose_clicked()
{
    const QString dirPath = QFileDialog::getExistingDirectory( this, tr("Choose Output Directory"), m_lastOpenedExportDir );

    // If user didn't click Cancel
    if ( ! dirPath.isEmpty() )
    {
        m_ui->lineEdit_OutputDir->setText( dirPath );
        m_lastOpenedExportDir = dirPath;
    }
}



void ExportDialog::on_pushButton_Create_clicked()
{
    const QString path = m_ui->lineEdit_OutputDir->text();
    File newDir;

    if ( QFileInfo( path ).isAbsolute() )
    {
        newDir = File( path.toLocal8Bit().data() );
    }
    else
    {
        newDir = File::getCurrentWorkingDirectory().getChildFile( path.toLocal8Bit().data() );
    }

    const Result result = newDir.createDirectory();

    if ( result.wasOk() )
    {
        m_ui->pushButton_Create->setEnabled( false );
        m_ui->label_DirValidity->setVisible( false );
        enableOkButtonIfInputValid();
    }
    else
    {
        QString errorInfo( result.getErrorMessage().toUTF8() );
        MessageBoxes::showWarningDialog( tr( "Couldn't create directory!" ), errorInfo );
    }
}



void ExportDialog::on_comboBox_Format_currentIndexChanged( const QString text )
{
    QStringList encodingTextList;
    QList<int> encodingDataList;
    int index = 0;

    if ( text == "WAV")
    {
        encodingTextList << "Unsigned 8 bit PCM" << "Signed 16 bit PCM" << "Signed 24 bit PCM" << "Signed 32 bit PCM" << "32 bit float" << "64 bit double" << "u-law encoding" << "A-law encoding";
        encodingDataList << SF_FORMAT_PCM_U8 << SF_FORMAT_PCM_16 << SF_FORMAT_PCM_24 << SF_FORMAT_PCM_32 << SF_FORMAT_FLOAT << SF_FORMAT_DOUBLE << SF_FORMAT_ULAW << SF_FORMAT_ALAW;
        index = 1; // Signed 16 bit PCM
    }
    else if ( text == "AIFF" )
    {
        encodingTextList << "Unsigned 8 bit PCM" << "Signed 8 bit PCM" << "Signed 16 bit PCM" << "Signed 24 bit PCM" << "Signed 32 bit PCM" << "32 bit float" << "64 bit double" << "u-law encoding" << "A-law encoding";
        encodingDataList << SF_FORMAT_PCM_U8 << SF_FORMAT_PCM_S8 << SF_FORMAT_PCM_16 << SF_FORMAT_PCM_24 << SF_FORMAT_PCM_32 << SF_FORMAT_FLOAT << SF_FORMAT_DOUBLE << SF_FORMAT_ULAW << SF_FORMAT_ALAW;
        index = 2; // Signed 16 bit PCM
    }
    else if ( text == "AU")
    {
        encodingTextList << "Signed 8 bit PCM" << "Signed 16 bit PCM" << "Signed 24 bit PCM" << "Signed 32 bit PCM" << "32 bit float" << "64 bit double" << "u-law encoding" << "A-law encoding";
        encodingDataList << SF_FORMAT_PCM_S8 << SF_FORMAT_PCM_16 << SF_FORMAT_PCM_24 << SF_FORMAT_PCM_32 << SF_FORMAT_FLOAT << SF_FORMAT_DOUBLE << SF_FORMAT_ULAW << SF_FORMAT_ALAW;
        index = 1; // Signed 16 bit PCM
    }
    else if ( text == "FLAC" )
    {
        encodingTextList << "Signed 8 bit PCM" << "Signed 16 bit PCM" << "Signed 24 bit PCM";
        encodingDataList << SF_FORMAT_PCM_S8 << SF_FORMAT_PCM_16 << SF_FORMAT_PCM_24;
        index = 1; // Signed 16 bit PCM
    }
    else if ( text == "Ogg" )
    {
        encodingTextList << "Vorbis";
        encodingDataList << SF_FORMAT_VORBIS;
    }
    else if ( text == "PGM, WAV" )
    {
        encodingTextList << "16 bit PCM, 44,100 Hz";
        encodingDataList << ( SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE );
    }
    else
    {
        qDebug() << "Unknown format: " << text;
    }

    m_ui->comboBox_Encoding->clear();

    for ( int i = 0; i < encodingTextList.size(); i++ )
    {
        m_ui->comboBox_Encoding->addItem( encodingTextList[ i ], encodingDataList[ i ] );
    }

    m_ui->comboBox_Encoding->setCurrentIndex( index );
}



void ExportDialog::on_radioButton_AudioFiles_clicked()
{
    foreach ( QAbstractButton* button, m_ui->buttonGroup_Numbering->buttons() )
    {
        button->setEnabled( true );
    }

    QStringList fileFormatTextList;
    fileFormatTextList << "WAV" << "AIFF" << "AU" << "FLAC" << "Ogg";
    m_ui->comboBox_Format->clear();
    m_ui->comboBox_Format->addItems( fileFormatTextList );

    m_ui->label_Model->setVisible( false );
    m_ui->comboBox_Model->setVisible( false );

    m_ui->label_SampleRate->setVisible( true );
    m_ui->comboBox_SampleRate->setVisible( true );

    m_ui->lineEdit_FileName->clear();
    setPlatformFileNameValidator();

    const int index = m_ui->comboBox_SampleRate->findData( SAMPLE_RATE_KEEP_SAME );
    m_ui->comboBox_SampleRate->setCurrentIndex( index );

    enableMidiFileTypeRadioButtons();

    m_ui->label_VoiceOverlap->setVisible( false );
    m_ui->label_MuteGroup->setVisible( false );
    m_ui->radioButton_Mono->setVisible( false );
    m_ui->radioButton_Poly->setVisible( false );
    m_ui->comboBox_MuteGroup->setVisible( false );
}



void ExportDialog::on_radioButton_H2Drumkit_clicked()
{
    foreach ( QAbstractButton* button, m_ui->buttonGroup_Numbering->buttons() )
    {
        button->setEnabled( false );
    }

    m_ui->radioButton_Suffix->setChecked( true );

    QStringList fileFormatTextList;
    fileFormatTextList << "FLAC" << "WAV" << "AIFF" << "AU";
    m_ui->comboBox_Format->clear();
    m_ui->comboBox_Format->addItems( fileFormatTextList );

    m_ui->label_Model->setVisible( false );
    m_ui->comboBox_Model->setVisible( false );

    m_ui->label_SampleRate->setVisible( true );
    m_ui->comboBox_SampleRate->setVisible( true );

    m_ui->lineEdit_FileName->clear();
    setPlatformFileNameValidator();

    const int index = m_ui->comboBox_SampleRate->findData( SAMPLE_RATE_KEEP_SAME );
    m_ui->comboBox_SampleRate->setCurrentIndex( index );

    enableMidiFileTypeRadioButtons();

    m_ui->label_VoiceOverlap->setVisible( false );
    m_ui->label_MuteGroup->setVisible( false );
    m_ui->radioButton_Mono->setVisible( false );
    m_ui->radioButton_Poly->setVisible( false );
    m_ui->comboBox_MuteGroup->setVisible( false );
}



void ExportDialog::on_radioButton_SFZ_clicked()
{
    foreach ( QAbstractButton* button, m_ui->buttonGroup_Numbering->buttons() )
    {
        button->setEnabled( false );
    }

    m_ui->radioButton_Suffix->setChecked( true );

    QStringList fileFormatTextList;
    fileFormatTextList << "WAV" << "FLAC" << "Ogg";
    m_ui->comboBox_Format->clear();
    m_ui->comboBox_Format->addItems( fileFormatTextList );

    m_ui->label_Model->setVisible( false );
    m_ui->comboBox_Model->setVisible( false );

    m_ui->label_SampleRate->setVisible( true );
    m_ui->comboBox_SampleRate->setVisible( true );

    m_ui->lineEdit_FileName->clear();
    setPlatformFileNameValidator();

    const int index = m_ui->comboBox_SampleRate->findData( SAMPLE_RATE_KEEP_SAME );
    m_ui->comboBox_SampleRate->setCurrentIndex( index );

    enableMidiFileTypeRadioButtons();

    m_ui->label_VoiceOverlap->setVisible( false );
    m_ui->label_MuteGroup->setVisible( false );
    m_ui->radioButton_Mono->setVisible( false );
    m_ui->radioButton_Poly->setVisible( false );
    m_ui->comboBox_MuteGroup->setVisible( false );
}



void ExportDialog::on_radioButton_Akai_clicked()
{
    foreach ( QAbstractButton* button, m_ui->buttonGroup_Numbering->buttons() )
    {
        button->setEnabled( false );
    }

    m_ui->radioButton_Suffix->setChecked( true );

    QStringList fileFormatTextList;
    fileFormatTextList << "PGM, WAV";
    m_ui->comboBox_Format->clear();
    m_ui->comboBox_Format->addItems( fileFormatTextList );

    m_ui->label_SampleRate->setVisible( false );
    m_ui->comboBox_SampleRate->setVisible( false );

    m_ui->label_Model->setVisible( true );
    m_ui->comboBox_Model->setVisible( true );

    m_ui->lineEdit_FileName->clear();

    const QRegExp regexp( AkaiFileHandler::getFileNameRegExpMPC1000() );
    m_ui->lineEdit_FileName->setValidator( new QRegExpValidator( regexp, this ) );

    const int index = m_ui->comboBox_SampleRate->findData( 44100 );
    m_ui->comboBox_SampleRate->setCurrentIndex( index );

    disableMidiFileTypeRadioButtons();

    m_ui->radioButton_MidiType1->setChecked( true );

    m_ui->label_VoiceOverlap->setVisible( true );
    m_ui->label_MuteGroup->setVisible( true );
    m_ui->radioButton_Mono->setVisible( true );
    m_ui->radioButton_Poly->setVisible( true );
    m_ui->comboBox_MuteGroup->setVisible( true );
}



void ExportDialog::on_checkBox_ExportAudio_clicked( const bool isChecked )
{
    foreach ( QAbstractButton* button, m_ui->buttonGroup_Export->buttons() )
    {
        button->setEnabled( isChecked );
    }

    {
        const bool enable = isChecked && m_ui->radioButton_AudioFiles->isChecked();

        foreach ( QAbstractButton* button, m_ui->buttonGroup_Numbering->buttons() )
        {
            button->setEnabled( enable );
        }
    }

    m_ui->comboBox_Encoding->setEnabled( isChecked );
    m_ui->comboBox_Format->setEnabled( isChecked );
    m_ui->comboBox_Model->setEnabled( isChecked );
    m_ui->comboBox_SampleRate->setEnabled( isChecked );

    if ( !isChecked && m_ui->checkBox_ExportMidi->isChecked() )
    {
        enableMidiFileTypeRadioButtons();
    }
    else if ( isChecked && m_ui->radioButton_Akai->isChecked() )
    {
        disableMidiFileTypeRadioButtons();
        m_ui->radioButton_MidiType1->setChecked( true );
    }
}



void ExportDialog::on_checkBox_ExportMidi_clicked( const bool isChecked )
{
    if ( isChecked )
    {
        if ( m_ui->checkBox_ExportAudio->isChecked() && m_ui->radioButton_Akai->isChecked() )
        {
            disableMidiFileTypeRadioButtons();
            m_ui->radioButton_MidiType1->setChecked( true );
        }
        else
        {
            enableMidiFileTypeRadioButtons();
        }
    }
    else
    {
        disableMidiFileTypeRadioButtons();
    }
}
