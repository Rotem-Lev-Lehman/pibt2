#pragma once

#include <graph.hpp>

/**
 * @brief Get the field of view from the current node location.
 * 
 * @param g - The graph in which the nodes are from. Actually is of type Grid *.
 * @param node - The node to calculate the field of view from.
 * @param field_radius - The field of view radius.
 * @return Nodes - The field of view from the current node location.
 */
Nodes get_field_of_view(Graph * g, Node * node, int field_radius);

/**
 * @brief Checks if the two given nodes are within each others field of view.
 * 
 * @param n1 - First node.
 * @param n2 - Second node.
 * @param field_radius - The field of view radius.
 * @return true - If both nodes are in each others field of view.
 * @return false - Else (not in each others field of view).
 */
bool in_field_of_view(Node *n1, Node *n2, int field_radius);
