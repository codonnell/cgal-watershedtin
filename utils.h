#ifndef __UTILS_H__
#define __UTILS_H__

#include "definitions.h"

/**
 * Set the label on all edges to be CHANNEL, RIDGE, or TRANSVERSE.
 */
void label_all_edges(Polyhedron& p);

/**
 * Calculates the edge type of a halfedge that has not already been typed.
 */
enum EdgeType edge_type(const Halfedge_const_handle& h);

/**
 * Determines whether v is not a saddle.
 */
bool is_not_saddle(const Vertex& v);

/**
 * Determines whether v is a saddle.
 *
 * A point is a saddle if it has a border halfedge coming from it or more than
 * one channel or ridge.
 */
bool is_saddle(const Vertex_const_handle& v);

/**
 * Prints all points adjacent to the input vertex.
 */
void print_neighborhood(const Vertex& v);

/**
 * Prints the two points of a halfedge.
 */
void print_halfedge(const Halfedge_const_handle& h);

#endif
