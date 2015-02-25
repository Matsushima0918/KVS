/*****************************************************************************/
/**
 *  @file   UniformGrid.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#include "GridBase.h"


namespace kvs
{

class UniformGrid : public kvs::GridBase
{
public:

    typedef kvs::GridBase BaseClass;

public:

    UniformGrid( const kvs::StructuredVolumeObject* volume );

    int findGrid( const kvs::Vec3& global ) const;
    kvs::Vec3 globalToLocal( const kvs::Vec3& global ) const;
};

} // end of namespace kvs
