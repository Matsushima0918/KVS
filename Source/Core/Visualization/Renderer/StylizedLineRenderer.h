/*****************************************************************************/
/**
 *  @file   StylizedLineRenderer.h
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#pragma once
#include <kvs/DebugNew>
#include <kvs/Module>
#include <kvs/LineObject>
#include <kvs/LineRenderer>
#include <kvs/Shader>
#include <kvs/ProgramObject>
#include <kvs/VertexBufferObjectManager>
#include <kvs/Texture2D>
#include <string>


namespace kvs
{

class StylizedLineRenderer : public kvs::LineRenderer
{
    kvsModule( kvs::StylizedLineRenderer, Renderer );
    kvsModuleBaseClass( kvs::LineRenderer );

public:
    class BufferObject
    {
    private:
        kvs::VertexBufferObjectManager m_manager{}; ///< VBOs
        kvs::ValueArray<GLint> m_first_array{}; ///< array of starting indices for the polyline
        kvs::ValueArray<GLsizei> m_count_array{}; ///< array of the number of indices for the polyline
        kvs::Texture2D m_shape_texture{};
        kvs::Texture2D m_diffuse_texture{};
        bool m_has_connection = false;
    public:
        BufferObject() = default;
        virtual ~BufferObject() { this->release(); }
        kvs::VertexBufferObjectManager& manager() { return m_manager; }
        void release() { m_manager.release(); }
        void create( const kvs::LineObject* line, const kvs::Real32 halo, const kvs::Real32 radius );
        void draw( const kvs::LineObject* line );
    private:
        void create_shape_texture();
        void create_diffuse_texture();
    };

    class RenderPass
    {
    private:
        BufferObject& m_buffer_object; ///< buffer object (reference)
        std::string m_vert_shader_file = "stylized_line.vert"; ///< vertex shader file
        std::string m_frag_shader_file = "stylized_line.frag"; ///< fragment shader file
        kvs::ProgramObject m_shader_program{}; ///< shader program
        kvs::Real32 m_radius_size = 0.05f;
        kvs::Real32 m_halo_size = 0.0f;
    public:
        RenderPass( BufferObject& buffer_object ): m_buffer_object( buffer_object ) {}
        virtual ~RenderPass() { this->release(); }
        BufferObject& bufferObject() { return m_buffer_object; }
        const std::string& vertexShaderFile() const { return m_vert_shader_file; }
        const std::string& fragmentShaderFile() const { return m_frag_shader_file; }
        kvs::ProgramObject& shaderProgram() { return m_shader_program; }
        kvs::Real32 radiusSize() const { return m_radius_size; }
        kvs::Real32 haloSize() const { return m_halo_size; }
        void setVertexShaderFile( const std::string& file ) { m_vert_shader_file = file; }
        void setFragmentShaderFile( const std::string& file ) { m_frag_shader_file = file; }
        void setShaderFiles( const std::string& vert_file, const std::string& frag_file );
        void setRadiusSize( const kvs::Real32 size ) { m_radius_size = size; }
        void setHaloSize( const kvs::Real32 size ) { m_halo_size = size; }
        virtual void release() { m_shader_program.release(); }
        virtual void create( const kvs::Shader::ShadingModel& model, const bool enable );
        virtual void update( const kvs::Shader::ShadingModel& model, const bool enable );
        virtual void setup( const kvs::Shader::ShadingModel& model );
        virtual void draw( const kvs::ObjectBase* object );
    };

private:
    size_t m_width = 0; ///< window width
    size_t m_height = 0; ///< window height
    const kvs::ObjectBase* m_object = nullptr; ///< pointer to the rendering object
    kvs::Shader::ShadingModel* m_shading_model = nullptr; ///< shading method
    BufferObject m_buffer_object{}; ///< buffer object
    RenderPass m_render_pass{ m_buffer_object }; ///< render pass

public:
    StylizedLineRenderer(): m_shading_model( new kvs::Shader::Lambert() ) {}
    virtual ~StylizedLineRenderer() { if ( m_shading_model ) { delete m_shading_model; } }

    void exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light );

    const std::string& vertexShaderFile() const { return m_render_pass.vertexShaderFile(); }
    const std::string& fragmentShaderFile() const { return m_render_pass.fragmentShaderFile(); }
    kvs::Real32 radiusSize() const { return m_render_pass.radiusSize(); }
    kvs::Real32 haloSize() const { return m_render_pass.haloSize(); }

    void setVertexShaderFile( const std::string& vert_file ) { m_render_pass.setVertexShaderFile( vert_file ); }
    void setFragmentShaderFile( const std::string& frag_file ) { m_render_pass.setFragmentShaderFile( frag_file ); }
    void setRadiusSize( const kvs::Real32 size ) { m_render_pass.setRadiusSize( size ); }
    void setHaloSize( const kvs::Real32 size ) { m_render_pass.setHaloSize( size ); }

    template <typename Model>
    void setShadingModel( const Model model )
    {
        if ( m_shading_model ) { delete m_shading_model; m_shading_model = NULL; }
        m_shading_model = new Model( model );
        if ( !m_shading_model )
        {
            kvsMessageError("Cannot create a specified shading model.");
        }
    }

protected:
    kvs::Shader::ShadingModel& shadingModel() { return *m_shading_model; }
    kvs::ProgramObject& shader() { return m_render_pass.shaderProgram(); }

    RenderPass& renderPass() { return m_render_pass; }
    BufferObject& bufferObject() { return m_buffer_object; }

    bool isWindowCreated() { return m_width == 0 && m_height == 0; }
    bool isWindowResized( size_t w, size_t h ) { return m_width != w || m_height != h; }
    bool isObjectChanged( const kvs::ObjectBase* o ) { return m_object != o; }
    void setWindowSize( size_t w, size_t h ) { m_width = w; m_height = h; }

    void createBufferObject( const kvs::ObjectBase* object );
    void updateBufferObject( const kvs::ObjectBase* object );
    void drawBufferObject( const kvs::Camera* camera );

public:
    template <typename ShadingType>
    KVS_DEPRECATED( void setShader( const ShadingType shader ) );
};

template <typename ShadingType>
inline void StylizedLineRenderer::setShader( const ShadingType shader )
{ this->setShadingModel<ShadingType>( shader ); }

} // end of namespace kvs
