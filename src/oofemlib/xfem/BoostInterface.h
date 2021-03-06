/*
 * BoostInterface.h
 *
 *  Created on: Jun 5, 2013
 *
 *  @author: Erik Svenning
 *
 *  Description: Interface to some useful Boost functions.
 *  Boost is subject to the Boost license:
 *
 *      Boost Software License - Version 1.0 - August 17th, 2003
 *
 *              Permission is hereby granted, free of charge, to any person or organization
 *              obtaining a copy of the software and accompanying documentation covered by
 *              this license (the "Software") to use, reproduce, display, distribute,
 *              execute, and transmit the Software, and to prepare derivative works of the
 *              Software, and to permit third-parties to whom the Software is furnished to
 *              do so, all subject to the following:
 *
 *              The copyright notices in the Software and this entire statement, including
 *              the above license grant, this restriction and the following disclaimer,
 *              must be included in all copies of the Software, in whole or in part, and
 *              all derivative works of the Software, unless such copies or derivative
 *              works are solely in the form of machine-executable object code generated by
 *              a source language processor.
 *
 *              THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *              IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *              FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 *              SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 *              FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 *              ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *              DEALINGS IN THE SOFTWARE.
 *
 */

#ifdef __BOOST_MODULE

#ifndef BOOSTINTERFACE_H_
 #define BOOSTINTERFACE_H_


 #include <iostream>
 #include <list>

 #include <boost/geometry.hpp>
 #include <boost/geometry/geometries/segment.hpp>
 #include <boost/geometry/geometries/linestring.hpp>
 #include <boost/geometry/geometries/point_xy.hpp>
 #include <boost/geometry/geometries/polygon.hpp>
 #include <boost/geometry/multi/geometries/multi_point.hpp>
 #include <boost/geometry/multi/geometries/multi_polygon.hpp>

 #include <boost/foreach.hpp>

 #include <boost/numeric/ublas/matrix.hpp>

/////////////////////////////////////
// Typedefs for boost types

// 2D point
typedef boost :: geometry :: model :: d2 :: point_xy< double >bPoint2;

// 2D segment
typedef boost :: geometry :: model :: segment< bPoint2 >bSeg2;

// 2D line
typedef boost :: geometry :: model :: linestring< bPoint2 >bLine2;

// Matrix
typedef boost :: numeric :: ublas :: matrix< double >bMatrix;

/////////////////////////////////////
// Distance

// Distance between two points in 2D
inline double bDist(const bPoint2 &iP1, const bPoint2 &iP2) { return boost :: geometry :: distance(iP1, iP2); }

// Distance between point and line in 2D
inline double bDist(const bPoint2 &iP, const bLine2 &iL) { return boost :: geometry :: distance(iP, iL); }

// Distance between point and line segment in 2D
inline double bDist(const bPoint2 &iP, const bSeg2 &iL) { return boost :: geometry :: distance(iP, iL); }

// Distane between two line segments in 2D
inline double bDist(const bSeg2 &iLS1, const bSeg2 &iLS2, bPoint2 *oIntersectionPoint = NULL);



/////////////////////////////////////
// Overlap checks
inline bool bOverlap(const bPoint2 &iLC1, const bPoint2 &iUC1, const bPoint2 &iLC2, const bPoint2 &iUC2);

/////////////////////////////////////
// Vector operations

inline double bDot(const bPoint2 &iP1, const bPoint2 &iP2) { return boost :: geometry :: dot_product(iP1, iP2); }

inline double bNorm(const bPoint2 &iP1) { return sqrt( iP1.x() * iP1.x() + iP1.y() * iP1.y() ); }
inline void bNormalized(bPoint2 &ioP);
/////////////////////////////////////
// Matrix operations
inline bool bSolve2by2(const bMatrix &iA, const bPoint2 &ib, bPoint2 &ox);


/////////////////////////////////////
// Basic operations
//inline int bSign(const double &iVal) { return boost::math::sign(iVal);}



















/////////////////////////////////////
// Implementation

inline double bDist(const bSeg2 &iLS1, const bSeg2 &iLS2, bPoint2 *oIntersectionPoint)
{
    bPoint2 pSpE( iLS1.second.x() - iLS1.first.x(), iLS1.second.y() - iLS1.first.y() );
    double l1 = bNorm(pSpE);
    bPoint2 n1(pSpE.x() / l1, pSpE.y() / l1);

    bPoint2 qSqE( iLS2.second.x() - iLS2.first.x(), iLS2.second.y() - iLS2.first.y() );
    double l2 = bNorm(qSqE);
    bPoint2 n2(qSqE.x() / l2, qSqE.y() / l2);


    bPoint2 psqs( iLS2.first.x() - iLS1.first.x(), iLS2.first.y() - iLS1.first.y() );
    bPoint2 b( 2.0 * l1 * bDot(psqs, n1), -2.0 * l2 * bDot(psqs, n2) );


    bMatrix A(2, 2);
    A(0, 0) =  2.0 * l1 * l1;
    A(0, 1) = -2.0 *l1 *l2 *bDot(n1, n2);
    A(1, 0) = A(0, 1);
    A(1, 1) = 2.0 * l2 * l2;

    bPoint2 x(0.0, 0.0);

    // If we do not succeed to invert A,
    // the lines are probably parallel.
    // This case needs to be treated separately
    if ( bSolve2by2(A, b, x) ) {
        // Clip to the range of the segments
        if ( x.x() < 0.0 ) {
            x.x(0.0);
        }

        if ( x.x() > 1.0 ) {
            x.x(1.0);
        }

        if ( x.y() < 0.0 ) {
            x.y(0.0);
        }

        if ( x.y() > 1.0 ) {
            x.y(1.0);
        }

        bPoint2 p( ( 1.0 - x.x() ) * iLS1.first.x() + x.x() * iLS1.second.x(), ( 1.0 - x.x() ) * iLS1.first.y() + x.x() * iLS1.second.y() );
        bPoint2 q( ( 1.0 - x.y() ) * iLS2.first.x() + x.y() * iLS2.second.x(), ( 1.0 - x.y() ) * iLS2.first.y() + x.y() * iLS2.second.y() );

        if ( oIntersectionPoint != NULL ) {
            oIntersectionPoint->x( 0.5 * ( p.x() + q.x() ) );
            oIntersectionPoint->y( 0.5 * ( p.y() + q.y() ) );
        }

        return bDist(p, q);
    } else {
        // The lines are parallel (or we have screwed up the input).

        // TODO: Check with end points properly.
        // For now, pick the start point on segment 1
        // and compute the distance from that point to
        // segment 2.

        bPoint2 p( iLS1.first.x(), iLS1.first.y() );
        return bDist(p, iLS2);
    }
}

inline bool bOverlap(const bPoint2 &iLC1, const bPoint2 &iUC1, const bPoint2 &iLC2, const bPoint2 &iUC2)
{
    if ( iUC1.x() > iLC2.x() && iUC1.y() > iLC2.y() ) {
        if ( iLC1.x() < iUC2.x() && iLC1.y() < iUC2.y() ) {
            return true;
        }
    }

    return false;
}

inline void bNormalized(bPoint2 &ioP)
{
    double l = bNorm(ioP);

    ioP.x(ioP.x() / l);
    ioP.y(ioP.y() / l);
}

inline bool bSolve2by2(const bMatrix &iA, const bPoint2 &ib, bPoint2 &ox)
{
    double tol = 1.0e-12;
    double det = iA(0, 0) * iA(1, 1) - iA(0, 1) * iA(1, 0);

    if ( fabs(det) > tol ) {
        ox.x( ( iA(1, 1) * ib.x() - iA(0, 1) * ib.y() ) / det );
        ox.y( ( -iA(1, 0) * ib.x() + iA(0, 0) * ib.y() ) / det );
        return true;
    } else {
        return false;
        //printf("Warning in bSolve2by2. det: %e\n", det);
    }
}




#endif /* BOOSTINTERFACE_H_ */

#endif
