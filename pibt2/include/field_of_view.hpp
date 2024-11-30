#pragma once

#include <graph.hpp>

/**
 * @brief Get the field of view from the current node location.
 * 
 * @param g - The graph in which the nodes are from. Actually is of type Grid *.
 * @param node - The node to calculate the field of view from.
 * @param field_radius - The field of view radius
 * @return Nodes - The field of view from the current node location.
 */
Nodes get_field_of_view(Graph * g, Node * node, int field_radius);
