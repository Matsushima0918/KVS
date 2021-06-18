/*****************************************************************************/
/**
 *  @file   DataWriter.h
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#pragma once
#include <kvs/ValueArray>
#include <kvs/XMLNode>
#include <kvs/Type>


namespace kvs
{

namespace kvsml
{

enum WritingDataType
{
    Ascii = 0,     ///< ascii data type
    ExternalAscii, ///< external ascii data type
    ExternalBinary ///< external binary data type
};

bool WriteCoordData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::ValueArray<kvs::Real32>& coords );

bool WriteColorData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::ValueArray<kvs::UInt8>& colors );

bool WriteNormalData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::ValueArray<kvs::Real32>& normals );

bool WriteSizeData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::ValueArray<kvs::Real32>& sizes );

bool WriteConnectionData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::ValueArray<kvs::UInt32>& connections );

bool WriteOpacityData(
    kvs::XMLNode::SuperClass* parent,
    const kvs::kvsml::WritingDataType writing_type,
    const std::string& filename,
    const kvs::ValueArray<kvs::UInt8>& opacities );

} // end of namespace kvsml

} // end of namespace kvs
