
#include "aftencodecglobal.h"

#include "aftencodecwidget.h"
#include "../../core/conversionoptions.h"

#include <KLocale>
#include <KComboBox>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>


AftenCodecWidget::AftenCodecWidget()
    : CodecWidget(),
    currentFormat( "ac3" )
{
    QGridLayout *grid = new QGridLayout( this );
    grid->setContentsMargins( 0, 0, 0, 0 );
    grid->setSpacing( 6 );

    // set up encoding options selection

    QHBoxLayout *topBox = new QHBoxLayout();
    grid->addLayout( topBox, 0, 0 );

    QLabel *lMode = new QLabel( i18n("Mode")+":", this );
    topBox->addWidget( lMode );
    cMode = new KComboBox( this );
    cMode->addItem( i18n("Quality") );
    cMode->addItem( i18n("Bitrate") );
    connect( cMode, SIGNAL(activated(int)), this, SLOT(modeChanged(int)) );
    connect( cMode, SIGNAL(activated(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( cMode );

    sQuality = new QSlider( Qt::Horizontal, this );
    connect( sQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySliderChanged(int)) );
    connect( sQuality, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( sQuality );

    dQuality = new QSpinBox( this );
    dQuality->setRange( 32, 640 );
    dQuality->setSuffix( " kbps" );
    dQuality->setFixedWidth( dQuality->sizeHint().width() );
    connect( dQuality, SIGNAL(valueChanged(int)), this, SLOT(qualitySpinBoxChanged(int)) );
    connect( dQuality, SIGNAL(valueChanged(int)), SIGNAL(somethingChanged()) );
    topBox->addWidget( dQuality );

    topBox->addStretch();

    grid->setRowStretch( 1, 1 );

    modeChanged( 0 );
}

AftenCodecWidget::~AftenCodecWidget()
{}

// TODO optimize !!!
int AftenCodecWidget::bitrateForQuality( int quality )
{
    return quality*100/3;
}

// TODO optimize !!!
int AftenCodecWidget::qualityForBitrate( int bitrate )
{
    return bitrate*3/100;
}

ConversionOptions *AftenCodecWidget::currentConversionOptions()
{
    ConversionOptions *options = new ConversionOptions();

    if( cMode->currentText()==i18n("Quality") )
    {
        options->qualityMode = ConversionOptions::Quality;
        options->quality = dQuality->value();
        options->bitrate = bitrateForQuality( options->quality );
        options->bitrateMin = 0;
        options->bitrateMax = 0;
    }
    else
    {
        options->qualityMode = ConversionOptions::Bitrate;
        options->bitrate = dQuality->value();
        options->quality = qualityForBitrate( options->bitrate );
        options->bitrateMin = 0;
        options->bitrateMax = 0;
    }

    return options;
}

bool AftenCodecWidget::setCurrentConversionOptions( ConversionOptions *_options )
{
    if( !_options || _options->pluginName != global_plugin_name )
        return false;

    ConversionOptions *options = _options;

    if( options->qualityMode == ConversionOptions::Quality )
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Quality")) );
        modeChanged( cMode->currentIndex() );
        dQuality->setValue( options->quality );
    }
    else
    {
        cMode->setCurrentIndex( cMode->findText(i18n("Bitrate")) );
        modeChanged( cMode->currentIndex() );
        dQuality->setValue( options->bitrate );
    }

    return true;
}

void AftenCodecWidget::setCurrentFormat( const QString& format )
{
    if( currentFormat == format )
        return;

    currentFormat = format;
    setEnabled( currentFormat != "wav" );
}

// TODO optimize !!!
QString AftenCodecWidget::currentProfile()
{
    if( currentFormat == "wav" )
    {
        return i18n("Lossless");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 60 )
    {
        return i18n("Very low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 80 )
    {
        return i18n("Low");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 100 )
    {
        return i18n("Medium");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 140 )
    {
        return i18n("High");
    }
    else if( cMode->currentIndex() == 0 && dQuality->value() == 180 )
    {
        return i18n("Very high");
    }

    return i18n("User defined");
}

// TODO optimize !!!
bool AftenCodecWidget::setCurrentProfile( const QString& profile )
{
    if( profile == i18n("Very low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 60 );
        dQuality->setValue( 60 );
        return true;
    }
    else if( profile == i18n("Low") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 80 );
        dQuality->setValue( 80 );
        return true;
    }
    else if( profile == i18n("Medium") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 100 );
        dQuality->setValue( 100 );
        return true;
    }
    else if( profile == i18n("High") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 140 );
        dQuality->setValue( 140 );
        return true;
    }
    else if( profile == i18n("Very high") )
    {
        cMode->setCurrentIndex( 0 );
        modeChanged( 0 );
        sQuality->setValue( 180 );
        dQuality->setValue( 180 );
        return true;
    }

    return false;
}

QDomDocument AftenCodecWidget::customProfile()
{
    QDomDocument profile("soundkonverter_profile");
    QDomElement root = profile.createElement("soundkonverter");
    root.setAttribute("type","profile");
    root.setAttribute("codecName",currentFormat);
    profile.appendChild(root);
    QDomElement encodingOptions = profile.createElement("encodingOptions");
    encodingOptions.setAttribute("qualityMode",cMode->currentIndex());
    encodingOptions.setAttribute("quality",dQuality->value());
    root.appendChild(encodingOptions);
    return profile;
}

bool AftenCodecWidget::setCustomProfile( const QString& profile, const QDomDocument& document )
{
    Q_UNUSED(profile)

    QDomElement root = document.documentElement();
    QDomElement encodingOptions = root.elementsByTagName("encodingOptions").at(0).toElement();
    sQuality->setValue( (int)(encodingOptions.attribute("quality").toDouble()*100) );
    dQuality->setValue( encodingOptions.attribute("quality").toInt() );
    return true;
}

int AftenCodecWidget::currentDataRate()
{
    int dataRate;

    if( currentFormat == "wav" )
    {
        dataRate = 10590000;
    }
    else
    {
        dataRate = 6400000;
    }

    return dataRate;
}

void AftenCodecWidget::modeChanged( int mode )
{
    if( mode == 0 )
    {
        sQuality->setRange( 0, 1023 );
        sQuality->setSingleStep( 10 );
        dQuality->setRange( 0, 1023 );
        dQuality->setSingleStep( 1 );
        dQuality->setSuffix( "" );
        sQuality->setValue( 240 );
        dQuality->setValue( 240 );
//         dQuality->setValue( qualityForBitrate(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
    }
    else
    {
        sQuality->setRange( 32, 640 );
        sQuality->setSingleStep( 8 );
        dQuality->setRange( 32, 640 );
        dQuality->setSingleStep( 1 );
        dQuality->setSuffix( " kbps" );
        sQuality->setValue( 192 );
        dQuality->setValue( 192 );
//         dQuality->setValue( bitrateForQuality(dQuality->value()) );
//         qualitySpinBoxChanged( dQuality->value() );
    }
}

void AftenCodecWidget::qualitySliderChanged( int quality )
{
    dQuality->setValue( quality );
}

void AftenCodecWidget::qualitySpinBoxChanged( int quality )
{
    sQuality->setValue( quality );
}

