/*****************************************************************************/
/**
 *  @file   ScreenBase.cpp
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include "ScreenBase.h"
#include "Application.h"
#include "KVSMouseButton.h"
#include "KVSKey.h"
#include <kvs/Message>
#include <kvs/EventListener>


namespace
{

/*===========================================================================*/
/**
 *  @brief  Returns pointer to the screen.
 *  @param  handler [in] the window handler
 *  @return pointer to the screen
 */
/*===========================================================================*/
kvs::glfw::ScreenBase* ThisScreen( GLFWwindow* handler )
{
    return static_cast<kvs::glfw::ScreenBase*>( glfwGetWindowUserPointer( handler ) );
}

class EventTimer : public kvs::EventTimer
{
private:
    double m_now_time;
    double m_last_time;
    double m_elapse_time;

public:
    EventTimer( kvs::EventListener* listener ): kvs::EventTimer( listener ) {}
    virtual ~EventTimer() {}
    void start( int msec );
    void nortify();
};

void EventTimer::start( int msec )
{
    if ( msec < 0 ) { return; }
    if ( this->isStopped() )
    {
        this->setInterval( msec );
        this->setStopped( false );
        m_now_time = glfwGetTime();
        m_last_time = m_now_time;
        m_elapse_time = 0.0;
    }
}

void EventTimer::nortify()
{
    if ( !this->isStopped() )
    {
        m_now_time = glfwGetTime();
        m_elapse_time += ( m_now_time - m_last_time );
        m_last_time = m_now_time;

        if ( m_elapse_time * 1000.0 > this->interval() )
        {
            if ( this->eventListener() )
            {
                this->eventListener()->onEvent( this->timeEvent() );
            }
            m_elapse_time = 0.0;
        }
    }
}

}

namespace kvs
{

namespace glfw
{

/*===========================================================================*/
/**
 *  @brief  Returns the pointer to the glfw screen base downcasted from the screen base.
 *  @param  screen [in] the screen base.
 *  @return pointer to the glfw screen base
 */
/*===========================================================================*/
ScreenBase* ScreenBase::DownCast( kvs::ScreenBase* screen )
{
    return dynamic_cast<ScreenBase*>( screen );
}

/*===========================================================================*/
/**
 *  @brief  Returns the const pointer to the glfw screen base downcasted from the screen base.
 *  @param  screen [in] the screen base.
 *  @return const pointer to the glfw screen base
 */
/*===========================================================================*/
const ScreenBase* ScreenBase::DownCast( const kvs::ScreenBase* screen )
{
    return dynamic_cast<ScreenBase*>( const_cast<kvs::ScreenBase*>( screen ) );
}

/*===========================================================================*/
/**
 *  @brief  Callback function, which is called when the window is resized.
 *  @param  handler [in] the window handler
 *  @param  width [in] the window width
 *  @param  height [in] the window height
 */
/*===========================================================================*/
void WindowSizeCallback( GLFWwindow* handler, int width, int height )
{
    auto* this_screen = ::ThisScreen( handler );
    this_screen->aquireContext();
    this_screen->resizeEvent( width, height );
    this_screen->releaseContext();
    this_screen->redraw();
}

/*===========================================================================*/
/**
 *  @brief  Callback function, which is called when a mouse button is pressed or released.
 *  @param  handler [in] the window handler
 *  @param  button [in] the mouse button
 *  @param  action [in] the action (GLFW_PRESS or GLFW_RELEASE)
 *  @param  mods [in] the modifier keys
 */
/*===========================================================================*/
void MouseButtonCallback( GLFWwindow* handler, int button, int action, int mods )
{
    auto* this_screen = ::ThisScreen( handler );
    this_screen->aquireContext();

    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos( handler, &x, &y );

    this_screen->m_mouse_event->setPosition( int( x ), int( y ) );
    this_screen->m_mouse_event->setButton( kvs::glfw::KVSMouseButton::Button( button ) );
    this_screen->m_mouse_event->setState( kvs::glfw::KVSMouseButton::State( action ) );
    this_screen->m_mouse_event->setModifiers( kvs::glfw::KVSKey::Modifier( mods ) );

    switch ( action )
    {
    case GLFW_PRESS:
    {
        this_screen->m_elapse_time_counter.stop();
        if ( this_screen->m_elapse_time_counter.sec() < 0.2f )
        {
            this_screen->m_mouse_event->setAction( kvs::MouseButton::DoubleClicked );
            this_screen->mouseDoubleClickEvent( this_screen->m_mouse_event );
        }
        else
        {
            this_screen->m_mouse_event->setAction( kvs::MouseButton::Pressed );
            this_screen->mousePressEvent( this_screen->m_mouse_event );
        }
        this_screen->m_elapse_time_counter.start();
        break;
    }
    case GLFW_RELEASE:
    {
        this_screen->m_mouse_event->setAction( kvs::MouseButton::Released );
        this_screen->mouseReleaseEvent( this_screen->m_mouse_event );
        break;
    }
    default: break;
    }

    this_screen->releaseContext();
}

/*===========================================================================*/
/**
 *  @brief  Callback function, which is called when the cursor is moved.
 *  @param  handler [in] the window handler
 *  @param  x [in] the x-coordinate of the cursor
 *  @param  y [in] the y-coordinate of the cursor
 */
/*===========================================================================*/
void CursorPosCallback( GLFWwindow* handler, double x, double y )
{
    auto* this_screen = ::ThisScreen( handler );
    this_screen->aquireContext();

    if ( this_screen->m_mouse_event->state() == kvs::MouseButton::Down )
    {
        this_screen->m_mouse_event->setPosition( x, y );
        this_screen->m_mouse_event->setAction( kvs::MouseButton::Moved );
        this_screen->mouseMoveEvent( this_screen->m_mouse_event );
    }

    this_screen->releaseContext();
}

/*===========================================================================*/
/**
 *  @brief  Callback function, which is called when the scrolling device is used.
 *  @param  handler [in] the window handler
 *  @param  x [in] the scroll offset along the x-axis
 *  @param  y [in] the scroll offset along the y-axis
 */
/*===========================================================================*/
void ScrollCallback( GLFWwindow* handler, double x, double y )
{
    auto* this_screen = ::ThisScreen( handler );
    this_screen->aquireContext();

    this_screen->m_wheel_event->setPosition( x, y );
    this_screen->m_wheel_event->setDirection( y > 0.0 ? 1 : -1 );
    this_screen->wheelEvent( this_screen->m_wheel_event );

    this_screen->releaseContext();
}

/*===========================================================================*/
/**
 *  @brief  Callback function, which is called when a key is pressed, repeated or released.
 *  @param  handler [in] the window handler
 *  @param  key [in] the key that was pressed or released
 *  @param  scancode [in] the system-specific scancode of the key
 *  @param  action [in] the action (GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT)
 *  @param  mods [in] the modifier keys
 */
/*===========================================================================*/
void KeyCallback( GLFWwindow* handler, int key, int scancode, int action, int mods )
{
    auto* this_screen = ::ThisScreen( handler );
    this_screen->aquireContext();

    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos( handler, &x, &y );

    this_screen->m_key_event->setPosition( int( x ), int( y ) );
    this_screen->m_key_event->setModifiers( kvs::glfw::KVSKey::Modifier( mods ) );
    this_screen->m_key_event->setKey( kvs::glfw::KVSKey::Code( key, mods ) );
    switch ( action )
    {
    case GLFW_PRESS:
        this_screen->m_key_event->setAction( kvs::Key::Pressed );
        this_screen->keyPressEvent( this_screen->m_key_event );
        break;
    case GLFW_REPEAT:
        this_screen->m_key_event->setAction( kvs::Key::Repeated );
        this_screen->keyRepeatEvent( this_screen->m_key_event );
        break;
    case GLFW_RELEASE:
        this_screen->m_key_event->setAction( kvs::Key::Released );
        this_screen->keyReleaseEvent( this_screen->m_key_event );
        break;
    default:
        break;
    }

    this_screen->releaseContext();
}

/*===========================================================================*/
/**
 *  @brief  Constructs a ScreenBase class.
 *  @param  application [in] the application
 */
/*===========================================================================*/
ScreenBase::ScreenBase( kvs::glfw::Application* application ):
    m_handler( nullptr ),
    m_id( -1 ),
    m_mouse_event( new kvs::MouseEvent() ),
    m_key_event( new kvs::KeyEvent() ),
    m_wheel_event( new kvs::WheelEvent() )
//    m_is_fullscreen( false )
{
    if ( application ) { application->attach( this ); }
    m_elapse_time_counter.start();
}

/*===========================================================================*/
/**
 *  @brief  Destroys the ScreenBase class.
 */
/*===========================================================================*/
ScreenBase::~ScreenBase()
{
    if ( m_mouse_event ) { delete m_mouse_event; }
    if ( m_key_event ) { delete m_key_event; }
    if ( m_wheel_event ) { delete m_wheel_event; }
    if ( m_handler ) { glfwDestroyWindow( m_handler ); }
}

/*===========================================================================*/
/**
 *  @brief  Sets an event.
 *  @param  event [in] event
 *  @param  name [in] name of the event
 */
/*===========================================================================*/
void ScreenBase::setEvent( kvs::EventListener* event, const std::string& name )
{
    if ( event->eventType() & kvs::EventBase::TimerEvent )
    {
        event->setEventTimer( new ::EventTimer( event ) );
    }
    BaseClass::setEvent( event, name );
}

/*===========================================================================*/
/**
 *  @brief  Adds an event.
 *  @param  event [in] event
 *  @param  name [in] name of the event
 */
/*===========================================================================*/
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

    // Create window.
    m_handler = glfwCreateWindow(
        BaseClass::width(),
        BaseClass::height(),
        BaseClass::title().c_str(),
        NULL, NULL );
    if ( !m_handler )
    {
        kvsMessageError() << "Cannot create GLFW screen." << std::endl;
        return;
    }

    static int id = 0;
    m_id = id++;

    glfwMakeContextCurrent( m_handler );
    glfwSwapInterval(0);

    // Set screen position.
    if ( BaseClass::x() < 0 && BaseClass::y() < 0 )
    {
        // Centering
        const GLFWvidmode* mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
        const int px = ( mode->width - BaseClass::width() ) / 2;
        const int py = ( mode->height - BaseClass::height() ) / 2;
        const int offset = 20;
        static int counter = 0;
        this->setPosition( px + offset * counter, py + offset * counter );
        counter++;
    }
    glfwSetWindowPos( m_handler, BaseClass::x(), BaseClass::y() );

    // Set callback functions.
    glfwSetWindowUserPointer( m_handler, this );
    glfwSetWindowSizeCallback( m_handler, WindowSizeCallback );
    glfwSetMouseButtonCallback( m_handler, MouseButtonCallback );
    glfwSetCursorPosCallback( m_handler, CursorPosCallback );
    glfwSetScrollCallback( m_handler, ScrollCallback );
    glfwSetKeyCallback( m_handler, KeyCallback );

    // Initialize GLEW. (after glfwMakeContextCurrent)
#if defined( KVS_ENABLE_GLEW )
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

    glfwMakeContextCurrent( NULL );
}

/*===========================================================================*/
/**
 *  @brief  Shows the screen.
 */
/*===========================================================================*/
void ScreenBase::show()
{
#if 1 // KVS_ENABLE_DEPRECATED
//    if ( m_id == -1 ) { this->create(); }
    if ( !m_handler ) { this->create(); }
#endif
    BaseClass::show();
    glfwShowWindow( m_handler );
}

/*===========================================================================*/
/**
 *  @brief  Hides the screen.
 */
/*===========================================================================*/
void ScreenBase::hide()
{
    BaseClass::hide();
//    glfwMakeContextCurrent( m_handler );
    glfwHideWindow( m_handler );
//    glfwMakeContextCurrent( nullptr );
}

/*===========================================================================*/
/**
 *  @brief  Shows the screen as fullscreen size.
 */
/*===========================================================================*/
void ScreenBase::showFullScreen()
{
//    if ( m_is_fullscreen ) return;
//    m_is_fullscreen = true;
    if ( BaseClass::isFullScreen() ) { return; }
    BaseClass::showFullScreen();

    int x = 0, y = 0;
    glfwGetWindowPos( m_handler, &x, &y );
    BaseClass::setPosition( x, y );

    auto* monitor = glfwGetPrimaryMonitor();
    const auto* mode = glfwGetVideoMode( monitor );
    glfwSetWindowMonitor( m_handler, monitor, 0, 0, mode->width, mode->height, 0 );
}

/*===========================================================================*/
/**
 *  @brief  Shows the screen as normal size.
 */
/*===========================================================================*/
void ScreenBase::showNormal()
{
//    if ( !m_is_fullscreen ) return;
//    m_is_fullscreen = false;
    if ( !BaseClass::isFullScreen() ) { return; }
    BaseClass::showNormal();

    const int x = BaseClass::x();
    const int y = BaseClass::y();
    const int w = BaseClass::width();
    const int h = BaseClass::height();
    glfwSetWindowMonitor( m_handler, nullptr, x, y, w, h, 0 );
}

/*===========================================================================*/
/**
 *  @brief  Pop up the screen.
 */
/*===========================================================================*/
void ScreenBase::popUp()
{
    glfwFocusWindow( m_handler );
}

/*===========================================================================*/
/**
 *  @brief  Push down the screen. (not yet implemented)
 */
/*===========================================================================*/
void ScreenBase::pushDown()
{
    // To-Do
}

/*===========================================================================*/
/**
 *  @brief  Redraw the screen.
 */
/*===========================================================================*/
void ScreenBase::redraw()
{
    this->aquireContext();
    this->paintEvent();
    this->releaseContext();
}

/*===========================================================================*/
/**
 *  @brief  Resize the screen.
 *  @param  width [in] the window width
 *  @param  height [in] the window height
 */
/*===========================================================================*/
void ScreenBase::resize( int width, int height )
{
    glfwSetWindowSize( m_handler, width, height );
}

/*===========================================================================*/
/**
 *  @brief  Returns true if the screen is shown as fullscreen size.
 *  @return true if the screen is fullscreen.
 */
/*===========================================================================*/
//bool ScreenBase::isFullScreen() const
//{
//    return m_is_fullscreen;
//}

} // end of namespace glfw

} // end of namespace kvs
