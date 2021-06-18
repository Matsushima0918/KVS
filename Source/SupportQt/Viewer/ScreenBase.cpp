/*****************************************************************************/
/**
 *  @file   ScreenBase.cpp
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include "ScreenBase.h"
#include <kvs/Assert>
#include <kvs/MouseEvent>
#include <kvs/KeyEvent>
#include <kvs/WheelEvent>
#include <kvs/EventTimer>
#include <kvs/EventListener>
#include <kvs/qt/Qt>
#include <kvs/qt/Application>
#include <SupportQt/Viewer/KVSMouseButton.h>
#include <SupportQt/Viewer/KVSKey.h>
#include <kvs/OpenGL>
#include <kvs/Version>


namespace
{

class EventTimer : public QObject, public kvs::EventTimer
{
public:
    EventTimer( kvs::EventListener* listener ): kvs::EventTimer( listener ) {}
    virtual ~EventTimer() {}
    void start( int msec );
    void stop();
    void nortify();

private:
    void timerEvent( QTimerEvent* e ) { this->nortify(); }
};

void EventTimer::start( int msec )
{
    if ( msec < 0 ) { return; }
    if ( this->isStopped() )
    {
        this->setInterval( msec );
        this->setStopped( false );
        this->setID( QObject::startTimer( msec ) );
    }
}

void EventTimer::stop()
{
    if ( !this->isStopped() )
    {
        this->setStopped( true );
        QObject::killTimer( this->id() );
    }
}

void EventTimer::nortify()
{
    if ( !this->isStopped() )
    {
        if ( this->eventListener() )
        {
            this->eventListener()->onEvent( this->timeEvent() );
        }
    }
}

}

namespace kvs
{

namespace qt
{

/*===========================================================================*/
/**
 *  @brief  Returns the pointer to the qt screen base downcasted from the screen base.
 *  @param  screen [in] the screen base.
 *  @return pointer to the qt screen base
 */
/*===========================================================================*/
ScreenBase* ScreenBase::DownCast( kvs::ScreenBase* screen )
{
    return dynamic_cast<ScreenBase*>( screen );
}

/*===========================================================================*/
/**
 *  @brief  Returns the const pointer to the qt screen base downcasted from the screen base.
 *  @param  screen [in] the screen base.
 *  @return const pointer to the qt screen base
 */
/*===========================================================================*/
const ScreenBase* ScreenBase::DownCast( const kvs::ScreenBase* screen )
{
    return dynamic_cast<ScreenBase*>( const_cast<kvs::ScreenBase*>( screen ) );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new ScreenBase class.
 *  @param  application [in] pointer to the application
 *  @param  parent [in] parent widget
 */
/*===========================================================================*/
ScreenBase::ScreenBase( kvs::qt::Application* application, QWidget* parent ):
    QGLWidget( parent ),
    m_id( -1 ),
    m_mouse_event( 0 ),
    m_key_event( 0 ),
    m_wheel_event( 0 )
//    m_is_fullscreen( false )
{
    if ( application ) application->attach( this );

    m_mouse_event = new kvs::MouseEvent();
    m_key_event = new kvs::KeyEvent();
    m_wheel_event = new kvs::WheelEvent();
}

/*===========================================================================*/
/**
 *  @brief  Destroys the ScreenBase class.
 */
/*===========================================================================*/
ScreenBase::~ScreenBase()
{
    delete m_mouse_event;
    delete m_key_event;
    delete m_wheel_event;
}

void ScreenBase::setEvent( kvs::EventListener* event, const std::string& name )
{
    if ( event->eventType() & kvs::EventBase::TimerEvent )
    {
        event->setEventTimer( new ::EventTimer( event ) );
    }
    BaseClass::setEvent( event, name );
}

void ScreenBase::addEvent( kvs::EventListener* event, const std::string& name )
{
    if ( event->eventType() & kvs::EventBase::TimerEvent )
    {
        event->setEventTimer( new ::EventTimer( event ) );
    }
    BaseClass::addEvent( event, name );
}

/*===========================================================================*/
/**
 *  @brief  Creates a screen.
 */
/*===========================================================================*/
void ScreenBase::create()
{
    KVS_ASSERT( m_id == -1 );
    static int id = 0;
    m_id = id++;

    // Initialize display mode.
    QGLFormat f = QGLFormat::defaultFormat();
    f.setDoubleBuffer( displayFormat().doubleBuffer() );
    f.setRgba( displayFormat().colorBuffer() );
    f.setDepth( displayFormat().depthBuffer() );
    f.setAccum( displayFormat().accumulationBuffer() );
    f.setStencil( displayFormat().stencilBuffer() );
    f.setStereo( displayFormat().stereoBuffer() );
    f.setSampleBuffers( displayFormat().multisampleBuffer() );
    f.setAlpha( displayFormat().alphaChannel() );
    QGLFormat::setDefaultFormat( f );

    // Set screen geometry.
    if ( BaseClass::x() < 0 && BaseClass::y() < 0 )
    {
        // Centering
        const QRect desk = QApplication::desktop()->screenGeometry();
        const int px = ( desk.width() - BaseClass::width() ) / 2;
        const int py = ( desk.height() - BaseClass::height() ) / 2;
        const int offset = 20;
        QWidget::setGeometry( px + offset * m_id, py + offset * m_id, BaseClass::width(), BaseClass::height() );
    }
    else
    {
        // User specified geometry.
        QWidget::setGeometry( BaseClass::x(), BaseClass::y(), BaseClass::width(), BaseClass::height() );
    }

    QGLWidget::makeCurrent();
    QWidget::show();
}

/*===========================================================================*/
/**
 *  @brief  Shows the screen.
 *  @return window ID
 */
/*===========================================================================*/
void ScreenBase::show()
{
    BaseClass::show();

#if 1 // KVS_ENABLE_DEPRECATED
    if ( m_id == -1 ) { this->create(); }
    else {
#endif

    QWidget::show();

#if 1 // KVS_ENABLE_DEPRECATED
    }
#endif
}

/*===========================================================================*/
/**
 *  @brief  Shows the window as full-screen.
 */
/*===========================================================================*/
void ScreenBase::showFullScreen()
{
//    if ( m_is_fullscreen ) return;
//    m_is_fullscreen = true;
    if ( BaseClass::isFullScreen() ) { return; }
    BaseClass::showFullScreen();

    const int x = QGLWidget::pos().x();
    const int y = QGLWidget::pos().y();
    BaseClass::setPosition( x, y );

    QWidget::showFullScreen();
}

/*===========================================================================*/
/**
 *  @brief  Shows the window as normal screen.
 */
/*===========================================================================*/
void ScreenBase::showNormal()
{
//    if ( !m_is_fullscreen ) return;
//    m_is_fullscreen = false;
    if ( !BaseClass::isFullScreen() ) { return; }
    BaseClass::showNormal();

    QWidget::resize( BaseClass::width(), BaseClass::height() );
    QWidget::move( BaseClass::x(), BaseClass::y() );
    QWidget::showNormal();
}

/*===========================================================================*/
/**
 *  @brief  Hides the window.
 */
/*===========================================================================*/
void ScreenBase::hide()
{
    BaseClass::hide();
    QWidget::hide();
}

/*===========================================================================*/
/**
 *  @brief  Pops up the window.
 */
/*===========================================================================*/
void ScreenBase::popUp()
{
#if ( KVS_QT_VERSION >= 4 )
    QWidget::activateWindow();
#else
    QWidget::setActiveWindow();
#endif
}

/*===========================================================================*/
/**
 *  @brief  Pushes down the window.
 */
/*===========================================================================*/
void ScreenBase::pushDown()
{
    QWidget::clearFocus();
}

/*===========================================================================*/
/**
 *  @brief  Redraws the window.
 */
/*===========================================================================*/
void ScreenBase::redraw()
{
    QGLWidget::updateGL();
}

/*===========================================================================*/
/**
 *  @brief  Resizes the window.
 *  @param  width [in] resized window width
 *  @param  height [in] resized window height
 */
/*===========================================================================*/
void ScreenBase::resize( int width, int height )
{
    BaseClass::setSize( width, height );
    QGLWidget::resize( width, height );
}

/*===========================================================================*/
/**
 *  @brief  Checks whether the window is full-screen or not.
 *  @return true, if the window is full-screen
 */
/*===========================================================================*/
//bool ScreenBase::isFullScreen() const
//{
//    return m_is_fullscreen;
//}

/*===========================================================================*/
/**
 *  @brief  Initialize event for Qt.
 */
/*===========================================================================*/
void ScreenBase::initializeGL()
{
#if defined( KVS_ENABLE_GLEW )
    // Initialize GLEW.
    GLenum result = glewInit();
    if ( result != GLEW_OK )
    {
        const GLubyte* message = glewGetErrorString( result );
        kvsMessageError( "GLEW initialization failed: %s.", message );
    }
#endif

    // Create paint device.
    BaseClass::paintDevice()->create();

    // Set device pixel ratio.
    const kvs::Vec4 vp = kvs::OpenGL::Viewport();
    BaseClass::setDevicePixelRatio( vp[2] / BaseClass::width() );

    this->initializeEvent();

    for ( auto& l : BaseClass::eventHandler()->listeners() )
    {
        if ( l->eventType() & kvs::EventBase::TimerEvent )
        {
            auto* t = l->eventTimer();
            if ( t )
            {
                auto i = l->timerInterval();
                l->eventTimer()->start( i );
            }
        }
    }
}

/*===========================================================================*/
/**
 *  @brief  Paint event for Qt.
 */
/*===========================================================================*/
void ScreenBase::paintGL()
{
    this->paintEvent();
}

/*===========================================================================*/
/**
 *  @brief  Resize event for Qt.
 *  @param  width [in] window width
 *  @param  height [in] window height
 */
/*===========================================================================*/
void ScreenBase::resizeGL( int width, int height )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
//#if ( KVS_VERSION_MAJOR > 2 || ( KVS_VERSION_MAJOR == 2 && KVS_VERSION_MINOR > 8 ) )
    // NOTE: High-dpi model such as retina display has been supported in Qt5.
    // Therefore, when using Qt5 on Mac with retina display, the 'width' and
    // 'height' specified as arguments of this method are scaled by a device
    // pixel ratio (DPR) parameter. The DPR has been supported in KVS since
    // 2.9. In KVS with SupportQt, the width and height which are not scaled
    // are passed to the resizeEvent method in order to keep the compatibility
    // with the previous version of KVS-based applications.
    const qreal scale = QGLWidget::devicePixelRatio();
    width = static_cast<size_t>( width / scale + 0.5 );
    height = static_cast<size_t>( height / scale + 0.5 );
//#endif
#endif

    this->resizeEvent( width, height );
}

/*===========================================================================*/
/**
 *  @brief  Mouse press event for Qt.
 *  @param  event [in] pointer to the mouse event
 */
/*===========================================================================*/
void ScreenBase::mousePressEvent( QMouseEvent* event )
{
#if ( KVS_QT_VERSION >= 4 )
    const int modifier = kvs::qt::KVSKey::Modifier( event->modifiers() );
#else
    const int modifier = kvs::qt::KVSKey::Modifier( event->state() );
#endif
    m_mouse_event->setPosition( event->x(), event->y() );
    m_mouse_event->setButton( kvs::qt::KVSMouseButton::Button( event->button() ) );
    m_mouse_event->setState( kvs::MouseButton::Down );
    m_mouse_event->setModifiers( modifier );
    m_mouse_event->setAction( kvs::MouseButton::Pressed );

    this->mousePressEvent( m_mouse_event );
}

/*===========================================================================*/
/**
 *  @brief  Mouse move event for Qt.
 *  @param  event [in] pointer to the mouse event
 */
/*===========================================================================*/
void ScreenBase::mouseMoveEvent( QMouseEvent* event )
{
#if ( KVS_QT_VERSION >= 4 )
    const int modifier = kvs::qt::KVSKey::Modifier( event->modifiers() );
#else
    const int modifier = kvs::qt::KVSKey::Modifier( event->state() );
#endif
    m_mouse_event->setPosition( event->x(), event->y() );
    m_mouse_event->setButton( kvs::qt::KVSMouseButton::Button( event->button() ) );
    m_mouse_event->setState( kvs::MouseButton::Down );
    m_mouse_event->setModifiers( modifier );
    m_mouse_event->setAction( kvs::MouseButton::Moved );

    this->mouseMoveEvent( m_mouse_event );
}

/*===========================================================================*/
/**
 *  @brief  Mouse release event for Qt
 *  @param  event [in] pointer to the mouse event
 */
/*===========================================================================*/
void ScreenBase::mouseReleaseEvent( QMouseEvent* event )
{
#if ( KVS_QT_VERSION >= 4 )
    const int modifier = kvs::qt::KVSKey::Modifier( event->modifiers() );
#else
    const int modifier = kvs::qt::KVSKey::Modifier( event->state() );
#endif
    m_mouse_event->setPosition( event->x(), event->y() );
    m_mouse_event->setButton( kvs::qt::KVSMouseButton::Button( event->button() ) );
    m_mouse_event->setState( kvs::MouseButton::Up );
    m_mouse_event->setModifiers( modifier );
    m_mouse_event->setAction( kvs::MouseButton::Released );

    this->mouseReleaseEvent( m_mouse_event );
}

/*===========================================================================*/
/**
 *  @brief  Mouse double-click event for Qt.
 *  @param  event [in] pointer to the mouse event
 */
/*===========================================================================*/
void ScreenBase::mouseDoubleClickEvent( QMouseEvent* event )
{
#if ( KVS_QT_VERSION >= 4 )
    const int modifier = kvs::qt::KVSKey::Modifier( event->modifiers() );
#else
    const int modifier = kvs::qt::KVSKey::Modifier( event->state() );
#endif
    m_mouse_event->setPosition( event->x(), event->y() );
    m_mouse_event->setButton( kvs::qt::KVSMouseButton::Button( event->button() ) );
    m_mouse_event->setState( kvs::MouseButton::Down );
    m_mouse_event->setModifiers( modifier );
    m_mouse_event->setAction( kvs::MouseButton::DoubleClicked );

    this->mouseDoubleClickEvent( m_mouse_event );
}

/*===========================================================================*/
/**
 *  @brief  Wheel event for Qt.
 *  @param  event [in] pointer to the wheel event
 */
/*===========================================================================*/
void ScreenBase::wheelEvent( QWheelEvent* event )
{
    m_wheel_event->setDirection( event->delta() > 0 ? 1 : -1 );
    m_wheel_event->setPosition( event->x(), event->y() );

    this->wheelEvent( m_wheel_event );
}

/*===========================================================================*/
/**
 *  @brief  Key press event for Qt.
 *  @param  event [in] pointer to the key event
 */
/*===========================================================================*/
void ScreenBase::keyPressEvent( QKeyEvent* event )
{
    m_key_event->setPosition( 0, 0 );
    m_key_event->setKey( kvs::qt::KVSKey::Code( event->key(), event->modifiers() ) );
    m_key_event->setModifiers( kvs::qt::KVSKey::Modifier( event->modifiers() ) );
    if ( event->isAutoRepeat() )
    {
        m_key_event->setAction( kvs::Key::Repeated );
        this->keyRepeatEvent( m_key_event );
    }
    else
    {
        m_key_event->setAction( kvs::Key::Pressed );
        this->keyPressEvent( m_key_event );
    }
}

void ScreenBase::keyReleaseEvent( QKeyEvent* event )
{
    m_key_event->setPosition( 0, 0 );
    m_key_event->setKey( kvs::qt::KVSKey::Code( event->key(), event->modifiers() ) );
    m_key_event->setModifiers( kvs::qt::KVSKey::Modifier( event->modifiers() ) );
    m_key_event->setAction( kvs::Key::Released );
    this->keyReleaseEvent( m_key_event );
}

} // end of namespace qt

} // end of namespace kvs
