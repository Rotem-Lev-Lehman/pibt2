#include "../include/field_of_view.hpp"

Nodes get_field_of_view(Graph * g, Node * node, int field_radius){
    Nodes field_of_view;
    Grid* grid = reinterpret_cast<Grid*>(g);
    int min_x = std::max(0, node->pos.x - field_radius);
    int min_y = std::max(0, node->pos.y - field_radius);
    // plus 1 since we want to include the field of view inside the for loop:
    int max_x = std::min(grid->getWidth(), node->pos.x + field_radius + 1);
    int max_y = std::min(grid->getHeight(), node->pos.y + field_radius + 1);
    for (int x = min_x; x < max_x; x++){
        for(int y = min_y; y < max_y; y++){
            // check if position in map (no obstacles):
            if(grid->existNode(x, y)){
                field_of_view.push_back(grid->getNode(x, y));
            }
        }
    }
    return field_of_view;
}
