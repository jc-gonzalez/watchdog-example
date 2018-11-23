/******************************************************************************
 * File:    ca_types.h
 *          This file is part of the Cellular Automata library
 *
 * Domain:  CellularAutomata.libca.DockerMng
 *
 * Last update:  2.0
 *
 * Date:    2018/11/13
 *
 * Author:  J C Gonzalez
 *
 * Copyright (C) 2015-2018 by J C Gonzalez
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare DockerMng class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   Component
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef CA_TYPES_H
#define CA_TYPES_H

//============================================================
// Group: External Dependencies
//============================================================

//------------------------------------------------------------
// Topic: System headers
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: External packages
//   none
//------------------------------------------------------------

//------------------------------------------------------------
// Topic: Project headers
//   none
//------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Namespace: QPF
// -----------------------
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
//namespace QPF {

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>

#include <gd.h>

typedef unsigned char byte;

// Declare lists of items

#ifdef T
#    undef T
#endif

#define T_NEIGHBORHOOD_LIST                     \
    T(Moore),                                   \
        T(ExtendedMoore),                       \
        T(VonNeumann),                          \
        T(ExtendedVonNeuman),                   \
        T(Manhattan2),                          \
        T(Manhattan3),                          \
        T(Manhattan4)

#define T_BOUNDARYTYPE_LIST                     \
    T(OpenBoundary),                            \
        T(PeriodicBoundary),                    \
        T(ReflectiveBoundary)

// Declare enumerated types and corresponding name strings

extern const std::string NeighborhoodNames[];
extern const std::string BoundaryTypeNames[];

#define T(x) x

enum Neighborhood { T_NEIGHBORHOOD_LIST };
enum BoundaryType { T_BOUNDARYTYPE_LIST };

#undef T

// Forward declarations

class Cell;
class Plane;
class Rule;

//typedef Cell * CellRow;
//typedef CellRow * CellMatrix;
typedef std::vector<Cell> CellRow;
typedef std::vector<CellRow> CellMatrix;
typedef std::vector<Plane> Generations;
typedef Rule * RuleHdl;

//}

#endif  /* CA_TYPES_H */
