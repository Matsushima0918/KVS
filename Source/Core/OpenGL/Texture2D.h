/****************************************************************************/
/**
 *  @file   Texture2D.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#pragma once
#include <kvs/Texture>
#include <kvs/Deprecated>
#include <cstddef>


namespace kvs
{

/*==========================================================================*/
/**
 *  2D texture class.
 */
/*==========================================================================*/
class Texture2D : public kvs::Texture
{
private:
    bool m_is_loaded = false; ///< if true, the texture is loaded

public:
    using BaseClass = kvs::Texture;
    static void Unbind();

public:
    Texture2D(): kvs::Texture( GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D ) {}
    virtual ~Texture2D() { this->release(); }

    bool isLoaded() const { return m_is_loaded; }

    void create( const size_t width, const size_t height, const void* data = NULL );
    void release();
    void load(
        const size_t width,
        const size_t height,
        const void* data,
        const size_t xoffset = 0,
        const size_t yoffset = 0 );
    void loadFromFrameBuffer(
        const int x,
        const int y,
        const size_t width,
        const size_t height,
        const size_t xoffset = 0,
        const size_t yoffset = 0 );
};

} // end of namespace kvs
